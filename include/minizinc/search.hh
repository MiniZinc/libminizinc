/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_SEARCH_HH__
#define __MINIZINC_SEARCH_HH__

#include "minizinc/flatten.hh"
#include "minizinc/solver_instance_base.hh"

namespace MiniZinc {
  
  /// perform the search on the model environment using the specified solver 
  template<class SolverInstanceBase>
  void search(Env& e, MiniZinc::Options& opt);
  
  /// interpret and execute the combinator given in the annotation  
  SolverInstance::Status 
  interpretCombinator(Annotation& ann, Env& env, SolverInstanceBase* solver);
}

#endif