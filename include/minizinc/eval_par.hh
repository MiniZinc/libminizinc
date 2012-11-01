/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_EVAL_PAR_HH__
#define __MINIZINC_EVAL_PAR_HH__

#include <minizinc/model.hh>

namespace MiniZinc {
  
  IntVal eval_int(ASTContext& ctx, Expression* e);

  bool eval_bool(ASTContext& ctx, Expression* e);
  
  void eval_int(ASTContext& ctx, Model* m);
  ArrayLit* eval_array_lit(ASTContext& ctx, Expression* e);

  IntSetVal* eval_intset(ASTContext& ctx, Expression* e);

  Expression* eval_par(ASTContext& ctx, Expression* e);
  
}

#endif
