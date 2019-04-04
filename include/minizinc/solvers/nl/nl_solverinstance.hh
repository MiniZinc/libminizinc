/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_NL_SOLVER_INSTANCE_HH__
#define __MINIZINC_NL_SOLVER_INSTANCE_HH__

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/ast.hh>

#include <minizinc/solvers/nl/nl_file.hh>

#ifdef _WIN32
#undef ERROR
#endif

namespace MiniZinc {

  class NLSolverOptions : public SolverInstanceBase::Options {
  public:
    std::string nl_solver;
    std::vector<std::string> nl_flags;
    std::vector<MZNFZNSolverFlag> nl_solver_flags;
    bool do_hexafloat = false;
    bool do_keepfile = false;
  };

  class NLSolverInstance : public SolverInstanceBase {
    private:
      std::string _fzn_solver;
    protected:
      Model* _fzn;
      Model* _ozn;

      NLFile nl_file;

    public:
      NLSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);

      ~NLSolverInstance(void);

      Status next(void) {return SolverInstance::Status::ERROR;}

      Status solve(void);

      void processFlatZinc(void);

      void resetSolver(void);

    protected:
      Expression* getSolutionValue(Id* id);

      void analyse(const Item* i);

  };












  class NL_SolverFactory: public SolverFactory {
  protected:
    virtual SolverInstanceBase* doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  public:
    NL_SolverFactory(void);
    virtual SolverInstanceBase::Options* createOptions(void);
    virtual std::string getDescription(SolverInstanceBase::Options* opt=NULL);
    virtual std::string getVersion(SolverInstanceBase::Options* opt=NULL);
    virtual std::string getId(void);
    virtual bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv);
    virtual void printHelp(std::ostream& os);
    //void setAcceptedFlags(SolverInstanceBase::Options* opt, const std::vector<MZNFZNSolverFlag>& flags);
  };

}

#endif
