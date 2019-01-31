/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FLAT_EXP_HH__
#define __MINIZINC_FLAT_EXP_HH__

#include <minizinc/flatten_internal.hh>

namespace MiniZinc {
  void addPathAnnotation(EnvI& env, Expression* e);
  void addCtxAnn(VarDecl* vd, BCtx& c);
  bool istrue(EnvI& env, Expression* e);
  bool isfalse(EnvI& env, Expression* e);
  Expression* createDummyValue(EnvI& env, const Type& t);
  TypeInst* eval_typeinst(EnvI& env, VarDecl* vd);

  KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e);
  KeepAlive conj(EnvI& env,VarDecl* b,Ctx ctx,const std::vector<EE>& e);

  VarDecl* newVarDecl(EnvI& env, Ctx ctx, TypeInst* ti, Id* origId, VarDecl* origVd, Expression* rhs);

  KeepAlive flat_cv_exp(EnvI& env, Ctx ctx, Expression* e);

  void makeDefinedVar(VarDecl* vd, Call* c);
  void checkIndexSets(EnvI& env, VarDecl* vd, Expression* e);

  class CallArgItem {
  public:
    EnvI& env;
    CallArgItem(EnvI& env0);
    ~CallArgItem(void);
  };

}

#endif
