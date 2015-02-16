/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "minizinc/search.hh"
#include "minizinc/solver_instance_base.hh"

namespace MiniZinc {
  
  SolverInstance::Status 
  interpretCombinator(Annotation& ann, Env& env, SolverInstanceBase* solver) {
    // TODO: implement
    return SolverInstance::UNKNOWN;
  }
  
}