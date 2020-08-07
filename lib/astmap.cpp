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

  template<>
#if defined(MINIZINC_GC_STATS)
  void ManagedASTStringMap<Expression*>::mark(std::map<int, GCStat>&) {
#else
  void ManagedASTStringMap<Expression*>::mark() {
#endif
    for (auto& it : *this) {
      it.first.mark();
      Expression::mark(it.second);
#if defined(MINIZINC_GC_STATS)
        gc_stats[it->second->_id].keepalive++;
#endif
    }
  };

  template<>
#if defined(MINIZINC_GC_STATS)
  void ManagedASTStringMap<VarDeclI*>::mark(std::map<int, GCStat>&) {
#else
  void ManagedASTStringMap<VarDeclI*>::mark() {
#endif
    for (auto& it : *this) {
      it.first.mark();
      it.second->mark();
      Expression::mark(it.second->e());
#if defined(MINIZINC_GC_STATS)
      gc_stats[it.second->e()->Expression::eid()].keepalive++;
#endif
    }
  };


}
