/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_instance_base.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

  SolverInstanceBase::Status
  SolverInstanceBase::solve(void) { return SolverInstance::ERROR; } 
  
  void
  SolverInstanceBase::Registry::add(const ASTString& name, poster p) {
    _registry.insert(std::make_pair(name, p));
  }
  void
  SolverInstanceBase::Registry::post(Call* c) {
    ASTStringMap<poster>::t::iterator it = _registry.find(c->id());
    if (it == _registry.end()) {
      std::cerr << "Error: constraint not found: " << c->id() << "\n";
      exit(EXIT_FAILURE);
    }
    it->second(_base, c);
  }

  SolverInstanceBase::Status 
  SolverInstanceBase::nextSolution(void) {
    Status s = next();
    if(s == SolverInstance::SAT || s == SolverInstance::OPT)
      assignSolutionToOutput();
    return s;
  }
  
  void
  SolverInstanceBase::assignSolutionToOutput(void) {
    for (VarDeclIterator it = _env.output()->begin_vardecls(); it != _env.output()->end_vardecls(); ++it) {
      std::cout << "DEBUG: type of var decl in output: \"" << (it->e()->id()->type().toString()) << "\" of variable: " << *(it->e()) << std::endl;
      if(!it->e()->id()->type().ispar()) { // don't assign solutions to parameters
        it->e()->e(getSolutionValue(it->e()->id()));
        std::cout << "DEBUG: set solution value: " << *(it->e()) << std::endl;
      }
    }
  }
  
}
