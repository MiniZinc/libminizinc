/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
//#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {

  class FZNSolverOptions : public SolverInstanceBase::Options {
  public:
    std::string fzn_solver;
    std::string backend;
    std::vector<std::string> fzn_flags;
    int numSols = 1;
    bool allSols = false;
    std::string parallel;
    int fzn_time_limit_ms = 0;
    int solver_time_limit_ms = 0;
    bool fzn_sigint = false;

    bool fzn_needs_paths = false;
    bool fzn_output_passthrough = false;
    
    bool supports_a = false;
    bool supports_n = false;
    bool supports_f = false;
    bool supports_p = false;
    bool supports_s = false;
    bool supports_r = false;
    bool supports_v = false;
    bool supports_t = false;
    std::vector<MZNFZNSolverFlag> fzn_solver_flags;
  };

  class FZNSolverInstance : public SolverInstanceBase {
    private:
      std::string _fzn_solver;
    protected:
      Model* _fzn;
      Model* _ozn;
    public:
      FZNSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);

      ~FZNSolverInstance(void);

      Status next(void) {return SolverInstance::ERROR;}

      Status solve(void);

      void processFlatZinc(void);

      void resetSolver(void);

    protected:
      Expression* getSolutionValue(Id* id);
  };

  class FZN_SolverFactory: public SolverFactory {
  protected:
    virtual SolverInstanceBase* doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  public:
    FZN_SolverFactory(void);
    virtual SolverInstanceBase::Options* createOptions(void);
    virtual std::string getDescription(SolverInstanceBase::Options* opt=NULL);
    virtual std::string getVersion(SolverInstanceBase::Options* opt=NULL);
    virtual std::string getId(void);
    virtual bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv);
    virtual void printHelp(std::ostream& os);
    void setAcceptedFlags(SolverInstanceBase::Options* opt, const std::vector<MZNFZNSolverFlag>& flags);
  };

}
