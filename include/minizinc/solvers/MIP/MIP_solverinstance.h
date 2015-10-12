/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_MIP_SOLVER_INSTANCE_H__
#define __MINIZINC_MIP_SOLVER_INSTANCE_H__

#include <minizinc/flattener.h>
#include <minizinc/solver.hh>
#include <minizinc/solvers/MIP/MIP_wrap.h>

namespace MiniZinc {
  
  // can be redefined as compilation parameter
#ifndef GETMIPWRAPPER     
  #define GETMIPWRAPPER MIP_WrapperFactory::GetDefaultMIPWrapper()
#endif
  
  class MIP_solver {
    public:
      typedef MIP_wrapper::VarId Variable;
      typedef MiniZinc::Statistics Statistics;
  };

  class MIP_solverinstance : public SolverInstanceImpl<MIP_solver> {
    protected:
      
      const auto_ptr<MIP_wrapper> mip_wrap;
      
      std::vector<VarDecl*> _varsWithOutput;
      UNORDERED_NAMESPACE::unordered_set<size_t> previousOutput;
      
      std::map<string, VarDecl*> mOutputDecls;
      
    public:
      double lastIncumbent;
    public:

      MIP_solverinstance(Env& env) :
        SolverInstanceImpl(env),
        mip_wrap(GETMIPWRAPPER)
      {
        assert(mip_wrap.get()); 
        registerConstraints();
      }
//       void setEnv(Env* pE) {
//         SolverInstanceBase::setEnv(pE);
//         registerConstraints();
//       }
      virtual MIP_wrapper* getMIPWrapper() const { return mip_wrap.get(); }
//       virtual void cleanup() {
//         mip_wrap->cleanup();
//         _varsWithOutput.clear();
//         SolverInstanceImpl<MIP_solver>::cleanup();
//       }

      virtual Status next(void) { assert(0); return SolverInstance::UNKNOWN; }
      virtual void processFlatZinc(void);
      virtual Status solve(void);
      virtual void resetSolver(void) { }

      void assignSolutionToOutput();   // needs to be public for the callback?
      void printSolution(ostream& );
      void printStatistics(ostream& );

//       /// PARAMS
//       int nThreads;
//       bool fVerbose;
//       std::string sExportModel, sReadParam, sWriteParam;
//       double nTimeout;
//       double nWorkMemLimit;
//       bool all_solutions;

    public:
//       double exprToDbl(Expression* e);
//       IloNumExpr exprToIloExpr(Expression* e);
      VarId exprToVar(Expression* e);
      void exprToArray(Expression* e, vector<double> &vals);
      void exprToVarArray(Expression* e, vector<VarId> &vars);

      Expression* getSolutionValue(Id* id);

      void registerConstraints(void);
  };  // MIP_solverinstance
  
  class MIP_SolverFactory: public SolverFactory {
  public:
    SolverInstanceBase* doCreateSI(Env& env)   { return new MIP_solverinstance(env); }
    
    bool processOption(int& i, int argc, const char** argv)
      { return MIP_WrapperFactory::processOption(i, argc, argv); }
    string getVersion( );
    void printHelp(std::ostream& os) { MIP_WrapperFactory::printHelp(os); }
  };

}

#endif  // __MINIZINC_MIP_SOLVER_INSTANCE_H__
