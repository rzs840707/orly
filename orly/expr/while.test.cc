/* <orly/expr/while.test.cc>

   Unit test for <orly/expr/while.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/expr/test_kit.h>

FIXTURE(Typical) {
  // [0..1] while false
  auto while_expr0 = TWhile::New(range0, TPosRange());
  while_expr0->SetRhs(bool0);
  EXPECT_EQ(while_expr0->GetType(), seq_type);
}