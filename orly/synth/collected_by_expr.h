/* <orly/synth/collected_by_expr.h>

   TODO

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

#pragma once

#include <cassert>

#include <base/class_traits.h>
#include <orly/expr/collected_by.h>
#include <orly/expr/lhsrhsable.h>
#include <orly/orly.package.cst.h>
#include <orly/synth/lhsrhsable_expr.h>

namespace Orly {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TCollectedByExpr
        : public TLhsRhsableExpr {
      NO_COPY(TCollectedByExpr);
      public:

      /* TODO */
      TCollectedByExpr(
          const TExprFactory *expr_factory,
          const Package::Syntax::TCollectedByExpr *collected_by_expr);

      /* TODO */
      virtual ~TCollectedByExpr();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      const Expr::TCollectedBy::TPtr &GetSymbol() const;

      /* TODO */
      virtual Expr::TLhsRhsable::TPtr GetLhsRhsableSymbol() const;

      private:

      /* TODO */
      const Package::Syntax::TCollectedByExpr *CollectedByExpr;

      /* TODO */
      TExpr *Lhs;

      /* TODO */
      TExpr *Rhs;

      /* TODO */
      mutable Expr::TCollectedBy::TPtr Symbol;

    };  // TCollectedByExpr

  }  // Synth

}  // Orly
