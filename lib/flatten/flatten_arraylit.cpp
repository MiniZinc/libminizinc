/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/flat_exp.hh>

namespace MiniZinc {

EE flatten_arraylit(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  auto* al = Expression::cast<ArrayLit>(e);
  if (al->flat()) {
    ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
    ret.r = bind(env, Ctx(), r, al);
  } else {
    VarDecl* rr = r == env.constants.varIgnore ? env.constants.varTrue : nullptr;
    bool c_root = ctx.b == C_ROOT && r != env.constants.varIgnore;
    Ctx eval_ctx = ctx;
    std::vector<EE> elems_ee(al->size());
    if (al->type().istuple() || al->type().isrecord()) {
      // Struct types have to check if any element is boolean to ensure correct context
      for (unsigned int i = al->size(); (i--) != 0U;) {
        eval_ctx.b = c_root && Expression::type((*al)[i]).isbool() ? C_MIX : C_ROOT;
        elems_ee[i] = flat_exp(env, eval_ctx, (*al)[i], rr, ctx.partialityVar(env));
      }
    } else {
      if (c_root && Expression::type(e).bt() == Type::BT_BOOL &&
          Expression::type(e).st() == Type::ST_PLAIN) {
        eval_ctx.b = C_MIX;
      }
      for (unsigned int i = al->size(); (i--) != 0U;) {
        elems_ee[i] = flat_exp(env, eval_ctx, (*al)[i], rr, ctx.partialityVar(env));
      }
    }
    std::vector<Expression*> elems(elems_ee.size());
    for (auto i = static_cast<unsigned int>(elems.size()); (i--) != 0U;) {
      elems[i] = elems_ee[i].r();
    }
    std::vector<std::pair<int, int> > dims(al->dims());
    for (unsigned int i = al->dims(); (i--) != 0U;) {
      dims[i] = std::pair<int, int>(al->min(i), al->max(i));
    }
    KeepAlive ka;
    {
      GCLock lock;
      if (al->type().istuple() || al->type().isrecord()) {
        assert(dims.size() == 1 && dims[0].first == 1 && dims[0].second == al->size());

        auto* alr = ArrayLit::constructTuple(Expression::loc(al).introduce(), elems);
        alr->type(al->type());
        alr->flat(true);

        // Add reverse mapper for tuple literal containing variables
        VarDecl* vd = new_vardecl(env, Ctx(), new TypeInst(Location().introduce(), al->type()),
                                  nullptr, nullptr, alr);
        vd->ti()->setStructDomain(env, al->type());
        env.reverseMappers.insert(vd->id(), alr);
        ka = vd->id();
      } else {
        auto* alr = new ArrayLit(Expression::loc(al).introduce(), elems, dims);
        alr->type(al->type());
        alr->flat(true);
        ka = alr;
      }
    }
    ret.b = conj(env, b, Ctx(), elems_ee);
    ret.r = bind(env, Ctx(), r, ka());
  }
  return ret;
}

}  // namespace MiniZinc
