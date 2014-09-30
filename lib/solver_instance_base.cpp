/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {

  SolverInstanceBase::Status
  SolverInstanceBase::solve(void) { return SolverInstance::ERROR; }
  
  void
  SolverInstanceBase::reset(void) { assert(false); }
  
  void
  SolverInstanceBase::resetWithConstraints(Model::iterator begin, Model::iterator end) {
    assert(false);
  }

  void
  SolverInstanceBase::processPermanentConstraints(Model::iterator begin, Model::iterator end) {
    assert(false);
  }
  
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

  void
  SolverInstanceBase::assignSolutionToOutput(void) {
    for (VarDeclIterator it = _env.output()->begin_vardecls(); it != _env.output()->end_vardecls(); ++it) {
      if (it->e()->e() == NULL) {
        it->e()->e(getSolutionValue(it->e()->id()));
      }
    }
  }
  
  
}
