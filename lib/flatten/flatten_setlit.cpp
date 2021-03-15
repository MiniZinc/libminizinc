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

EE flatten_setlit(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  auto* sl = e->cast<SetLit>();
  assert(sl->isv() == nullptr && sl->fsv() == nullptr);
  std::vector<EE> elems_ee(sl->v().size());
  for (unsigned int i = sl->v().size(); (i--) != 0U;) {
    elems_ee[i] = flat_exp(env, ctx, sl->v()[i], nullptr, nullptr);
  }
  std::vector<Expression*> elems(elems_ee.size());
  bool allPar = true;
  bool hadOpt = false;
  for (auto i = static_cast<unsigned int>(elems.size()); (i--) != 0U;) {
    elems[i] = elems_ee[i].r();
    allPar = allPar && elems[i]->type().isPar();
    hadOpt = hadOpt || elems[i]->type().isOpt();
  }

  ret.b = conj(env, b, Ctx(), elems_ee);
  if (allPar) {
    GCLock lock;
    auto* nsl = new SetLit(Location().introduce(), elems);
    Type nsl_t(e->type());
    nsl_t.ti(Type::TI_PAR);
    nsl->type(nsl_t);
    Expression* ee = eval_set_lit(env, nsl);
    ret.r = bind(env, Ctx(), r, ee);
  } else {
    GCLock lock;
    auto* al = new ArrayLit(sl->loc(), elems);
    Type al_t = Type::varint(1);
    if (hadOpt) {
      al_t.ot(Type::OT_OPTIONAL);
    }
    al->type(al_t);
    std::vector<Expression*> args(1);
    args[0] = al;
    Call* cc = new Call(sl->loc().introduce(), "array2set", args);
    cc->type(Type::varsetint());
    FunctionI* fi = env.model->matchFn(env, cc->id(), args, false);
    if (fi == nullptr) {
      throw FlatteningError(env, cc->loc(), "cannot find matching declaration");
    }
    assert(fi);
    assert(env.isSubtype(fi->rtype(env, args, false), cc->type(), false));
    cc->decl(fi);
    EE ee = flat_exp(env, Ctx(), cc, nullptr, constants().varTrue);
    ret.r = bind(env, Ctx(), r, ee.r());
  }
  return ret;
}
}  // namespace MiniZinc
