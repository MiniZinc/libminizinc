/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astmap.hh>

namespace MiniZinc {

template <>
void ManagedASTStringMap<Expression*>::mark() {
  for (auto& it : *this) {
    it.first.mark();
    Expression::mark(it.second);
#if defined(MINIZINC_GC_STATS)
    GC::stats()[it.second->eid()].keepalive++;
#endif
  }
};

template <>
void ManagedASTStringMap<VarDeclI*>::mark() {
  for (auto& it : *this) {
    it.first.mark();
#if defined(MINIZINC_GC_STATS)
    GC::stats()[it.second->e()->Expression::eid()].keepalive++;
#endif
    Item::mark(it.second);
  }
};

}  // namespace MiniZinc
