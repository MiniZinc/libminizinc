/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flat_exp.hh>

namespace MiniZinc {

EE flatten_fieldaccess(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  auto* fa = Expression::cast<FieldAccess>(e);
  assert(Expression::type(fa->v()).istuple() || Expression::type(fa->v()).isrecord());

  // Resolve tuple
  Ctx nctx = ctx;
  nctx.b = +nctx.b;
  nctx.neg = false;
  EE ret = flat_exp(env, nctx, fa->v(), nullptr, b);
  auto* al = Expression::cast<ArrayLit>(eval_array_lit(env, ret.r()));

  // Resolve field
  IntVal i = IntLit::v(Expression::cast<IntLit>(fa->field()));
  if (i < 1 || i > al->size()) {
    // This should not happen, type checking should ensure all fields are valid.
    throw EvalError(env, Expression::loc(fa), "Internal error: accessing invalid field");
  }

  // Bind result
  ret.r = bind(env, ctx, r, (*al)[static_cast<unsigned int>(i.toInt()) - 1]);
  return ret;
}

}  // namespace MiniZinc
