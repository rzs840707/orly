#include <bit/jobs/link.h>

#include <chrono>
#include <queue>

#include <base/as_str.h>
#include <base/not_implemented.h>
#include <base/split.h>
#include <bit/file_environment.h>
#include <bit/file_info.h>
#include <bit/job_producer.h>
#include <util/stl.h>

// TODO(cmaloney)
#include <iostream>

using namespace Base;
using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

static unordered_set<TRelPath> GetOutputName(const TRelPath &input) {
  return {input.SwapExtension(".o", "")};
}

static TOpt<TRelPath> TryGetInputName(const TRelPath &output) { return output.AddExtension(".o"); }

TJobProducer TLink::GetProducer(const TJobConfig &job_config) {
  return TJobProducer{"link",
                      TFileType::Object,
                      {TFileType::Executable},
                      TryGetInputName,
                      GetOutputName,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TLink>(new TLink(std::move(metadata), &job_config));
                      }};
}

TJob::TOutput TLink::Run(TFileEnvironment *file_environment) {
  TJob::TOutput output;
  output.Result = TJob::TOutput::NewNeeds;

  // Seed the link search with the core input file.
  if (ObjFiles.empty()) {
    TFileInfo *input = GetInput();
    ObjToCheck.insert(input);
    ObjFiles.insert(input);
  }

  // We build a queue so we can fill ObjToCheck as we go, and add more to be processed while we
  // iterate.
  std::queue<TFileInfo *> to_check;
  for (TFileInfo *obj : ObjToCheck) {
    to_check.push(obj);
  }
  ObjToCheck.clear();

  // Get all the link objects of every link object until we have a complete set of link objects.
  vector<string> filtered_includes;  // Hoisted out of loop
  while (!to_check.empty()) {
    TFileInfo *obj = Util::Pop(to_check);

    if (obj->IsBuildable().IsUnknown()) {
      output.Needs.IfBuildable.insert(obj);
      ObjToCheck.insert(obj);
      continue;
    }

    // TODO(cmaloney): The source .o is mandatory to be buildable, but that is accounted for because
    // the job depends on that and can't be instantiated without it.
    // If the possible link dep isn't buildable, then skip it.
    if (!*obj->IsBuildable()) {
      continue;
    }

    if (!obj->IsComplete()) {
      output.Needs.Mandatory.insert(obj);
      ObjToCheck.insert(obj);
      continue;
    }

    for (const auto &dep : obj->GetCompleteConfig()->JobConfig.at("try_link").GetArray()) {
      TFileInfo *link_file =
          file_environment->GetFileInfo(*file_environment->TryGetRelPath(dep.GetString()));

      // If we haven't scheduled a check for this file, do so.
      if (ObjFiles.insert(link_file).second) {
        to_check.push(link_file);
      }
    }
  }

  if (!ObjToCheck.empty()) {
    return output;
  }

  // Sort based on completion into "AntiNeeds"
  // Build the link command line. All files which don't exist become "anti needs" (if they could
  // later be produced, we need to re-build the output).
  vector<string> cmd = {"clang++",  "-o",  GetSoleOutput()->CmdPath, "-pthread", "-ldl"};
  cmd.reserve(cmd.size() + ObjFiles.size());
  for (TFileInfo *file : ObjFiles) {
    if (file->IsComplete()) {
      cmd.push_back(file->CmdPath);
    } else {
      AntiNeeds.insert(file);
    }
  }

  output.Subprocess = Subprocess::Run(cmd);
  // Process the output
  if (output.Subprocess.ExitCode != 0) {
    output.Result = TJob::TOutput::Error;
    return output;
  }
  output.Result = TJob::TOutput::Complete;
  return output;
}

string TLink::GetConfigId() const {
  // TODO(cmaloney): Make stable.
  static const auto now = std::chrono::system_clock::to_time_t(chrono::system_clock::now());
  return AsStr(ctime(&now));
}

// TODO(Cmaloney): AntiNeeds
std::unordered_map<TFileInfo *, TJobConfig> TLink::GetOutputExtraData() const { return {}; }

TLink::TLink(TMetadata &&metadata, const TJobConfig *) : TJob(move(metadata)) {}