/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/fzn_presolverinstance.hh>

namespace MiniZinc {

  FZNPreSolverInstance::FZNPreSolverInstance(Env& env, const Options& options)
          : FZNSolverInstance(env, options), solns2Vector(&env) {
    this->setSolns2Out(&solns2Vector);
  }

  SolverInstance::Status FZNPreSolverInstance::solve(void) {
    auto status = FZNSolverInstance::solve();
    nr_solutions = solns2Vector.solutions.size();
    if(nr_solutions >= 1)
      status = next();
    return status;
  }

  SolverInstance::Status FZNPreSolverInstance::next(void) {
    if (solns2Vector.solutions.size() < 1)
      return SolverInstance::ERROR;

    solution = solns2Vector.solutions.back();
    solns2Vector.solutions.pop_back();
    return solns2Vector.solutions.size() == 0 ? SolverInstance::OPT : SolverInstance::SAT;
  }

  bool FZNPreSolverInstance::Solns2Vector::evalOutput() {
    GCLock lock;
    createOutputMap();
    ASTStringMap<Expression*>::t output;
    for (auto it = declmap.begin(); it != declmap.end(); ++it) {
      output[it->first] = copy(pEnv->envi(), it->second.first->e());
    }
    solutions.push_back(output);
  }
}