/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver.hh>

namespace MiniZinc {

  class MZNSolverOptions : public SolverInstanceBase::Options {
  public:
    std::string mzn_solver;
    std::vector<std::string> mzn_flags;
    int numSols = 1;
    bool allSols = false;
    std::string parallel;
    int mzn_time_limit_ms = 0;
    int solver_time_limit_ms = 0;
    bool mzn_sigint = false;
    bool supports_t = false;
    std::vector<MZNFZNSolverFlag> mzn_solver_flags;
  };
  
  class MZNSolverInstance : public SolverInstanceBase {
  private:
    std::string _mzn_solver;
  public:
    MZNSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
    
    ~MZNSolverInstance(void);

    Status next(void) {return SolverInstance::ERROR;}

    Status solve(void);

    void processFlatZinc(void);

    void resetSolver(void);
  };

  class MZN_SolverFactory: public SolverFactory {
  protected:
    virtual SolverInstanceBase* doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  public:
    MZN_SolverFactory(void);
    virtual SolverInstanceBase::Options* createOptions(void);
    virtual std::string getDescription(SolverInstanceBase::Options* opt=NULL);
    virtual std::string getVersion(SolverInstanceBase::Options* opt=NULL);
    virtual std::string getId(void);
    virtual bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv);
    virtual void printHelp(std::ostream& os);
    void setAcceptedFlags(SolverInstanceBase::Options* opt, const std::vector<MZNFZNSolverFlag>& flags);
  };

}
