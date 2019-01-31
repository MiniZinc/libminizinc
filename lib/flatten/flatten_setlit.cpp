/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flat_exp.hh>

namespace MiniZinc {

  EE flatten_setlit(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    SetLit* sl = e->cast<SetLit>();
    assert(sl->isv()==NULL && sl->fsv()==NULL);
    std::vector<EE> elems_ee(sl->v().size());
    for (unsigned int i=sl->v().size(); i--;)
      elems_ee[i] = flat_exp(env,ctx,sl->v()[i],NULL,NULL);
    std::vector<Expression*> elems(elems_ee.size());
    bool allPar = true;
    for (unsigned int i=static_cast<unsigned int>(elems.size()); i--;) {
      elems[i] = elems_ee[i].r();
      allPar = allPar && elems[i]->type().ispar();
    }
    
    ret.b = conj(env,b,Ctx(),elems_ee);
    if (allPar) {
      GCLock lock;
      Expression* ee = eval_par(env,e);
      ret.r = bind(env,Ctx(),r,ee);
    } else {
      GCLock lock;
      ArrayLit* al = new ArrayLit(sl->loc(),elems);
      al->type(Type::varint(1));
      std::vector<Expression*> args(1);
      args[0] = al;
      Call* cc = new Call(sl->loc().introduce(), "array2set", args);
      cc->type(Type::varsetint());
      FunctionI* fi = env.model->matchFn(env, cc->id(), args, false);
      if (fi==NULL) {
        throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
      }
      assert(fi);
      assert(env.isSubtype(fi->rtype(env, args, false),cc->type(),false));
      cc->decl(fi);
      EE ee = flat_exp(env, Ctx(), cc, NULL, constants().var_true);
      ret.r = bind(env,Ctx(),r,ee.r());
    }
    return ret;
  }
}
