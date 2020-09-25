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

EE flatten_arraylit(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  auto* al = e->cast<ArrayLit>();
  if (al->flat()) {
    ret.b = bind(env, Ctx(), b, constants().literalTrue);
    ret.r = bind(env, Ctx(), r, al);
  } else {
    std::vector<EE> elems_ee(al->size());
    for (unsigned int i = al->size(); (i--) != 0U;) {
      elems_ee[i] = flat_exp(env, ctx, (*al)[i], nullptr, nullptr);
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
      auto* alr = new ArrayLit(Location().introduce(), elems, dims);
      alr->type(al->type());
      alr->flat(true);
      ka = alr;
    }
    ret.b = conj(env, b, Ctx(), elems_ee);
    ret.r = bind(env, Ctx(), r, ka());
  }
  return ret;
}

}  // namespace MiniZinc
