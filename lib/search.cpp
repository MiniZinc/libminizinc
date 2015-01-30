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
  
  template<class SolverInstanceBase>
  void search(Env& env, MiniZinc::Options& opt) {   
    SolverInstanceBase* solver = new SolverInstanceBase(env,opt);
    solver->processFlatZinc();
    
    SolverInstance::Status status;    
    if(env.model()->solveItem()->combinator_lite()) {
      Annotation& combinator = env.model()->solveItem()->ann(); // TODO: get the actual combinator, and not the full annotation
      return interpretCombinator(combinator, env, solver);
    }
    else { // solve using normal solve call
      status = solver.solve();
    }    
    // process status and solution
    if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
      env.evalOutput(std::cout);
    }
    std::cout << "----------\n";
    switch(status) {
      case SolverInstance::SAT:
        break;
      case SolverInstance::OPT:
        std::cout << "==========\n";
        break;
      case SolverInstance::UNKNOWN:
        std::cout << "=====UNKNOWN=====";
        break;
      case SolverInstance::ERROR:
        std::cout << "=====ERROR=====";
        break;
      case SolverInstance::UNSAT:
        std::cout << "=====UNSAT=====";
        break;        
    }
  }
  
  SolverInstance::Status 
  interpretCombinator(Annotation& ann, Env& env, SolverInstanceBase* solver) {
    // TODO: implement
    return SolverInstance::UNKNOWN;
  }
  
}