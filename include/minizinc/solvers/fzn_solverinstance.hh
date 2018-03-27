/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FZN_SOLVER_INSTANCE_HH__
#define __MINIZINC_FZN_SOLVER_INSTANCE_HH__

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
//#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {

  class FZNSolverInstance : public SolverInstanceBase {
    private:
      std::string _fzn_solver;

    protected:
      Model* _fzn;
      Model* _ozn;
    public:
      FZNSolverInstance(Env& env, std::ostream& log, const Options& options);

      ~FZNSolverInstance(void);

      Status next(void) {return SolverInstance::ERROR;}

      Status solve(void);

      void processFlatZinc(void);

      void resetSolver(void);

    protected:
      Expression* getSolutionValue(Id* id);
  };

  class FZN_SolverFactory: public SolverFactory {
    Options _options;
  public:
    SolverInstanceBase* doCreateSI(Env& env, std::ostream& log) {
      return new FZNSolverInstance(env, log, _options);
    }
    std::string getVersion(void);
    std::string getId(void);
    bool processOption(int& i, int argc, const char** argv);
    void printHelp(std::ostream& os);
  };

}

#endif
