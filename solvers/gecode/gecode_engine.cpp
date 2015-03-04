 /* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:     
 *     Andrea RENDL (andrea.rendl@nicta.com.au)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/gecode/gecode_engine.hh>

namespace MiniZinc {
  
  void
  MiniZinc::Path::post(Gecode::Space& home) const {
    GECODE_ES_FAIL(Gecode::Search::Meta::NoGoodsProp::post(home,*this));
  }
 
}
