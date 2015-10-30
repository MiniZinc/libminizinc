/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_CPLEX_SOLVER_INSTANCE_HH__
#define __MINIZINC_CPLEX_SOLVER_INSTANCE_HH__

#include <minizinc/solver_instance_base.hh>
#include <ilcplex/ilocplex.h>     // add -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 to the 1st call of cmake
#include <ilcplex/ilocplexi.h>     // add -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 to the 1st call of cmake

namespace MiniZinc {

  class SolutionCallbackI;

  class CPLEXSolver {
    public:
      typedef IloNumVar Variable;
      typedef MiniZinc::Statistics Statistics;
  };

  class CPLEXSolverInstance : public SolverInstanceImpl<CPLEXSolver> {
    protected:
      IloEnv _iloenv;
      IloModel* _ilomodel;
      IloCplex* _ilocplex;
      std::vector<VarDecl*> _varsWithOutput;
      UNORDERED_NAMESPACE::unordered_set<size_t> previousOutput;
    public:
      CPLEXSolverInstance(Env& env, const Options& options);

      virtual ~CPLEXSolverInstance(void);

      virtual Status next(void);

      virtual Status solve(void);

      virtual void processFlatZinc(void);

      virtual void resetSolver(void);

      void assignSolutionToOutput(SolutionCallbackI* cb = NULL);
      void printSolution(SolutionCallbackI* cb = NULL);

      /// PARAMS
      int nThreads;
      bool fVerbose;
      std::string sExportModel, sReadParam, sWriteParam;
      double nTimeout;
      double nWorkMemLimit;
      bool all_solutions;

      IloModel* getIloModel(void);
      IloNum exprToIloNum(Expression* e);
      IloNumExpr exprToIloExpr(Expression* e);
      IloNumVar exprToIloNumVar(Expression* e);
      IloNumArray exprToIloNumArray(Expression* e);
      IloNumVarArray exprToIloNumVarArray(Expression* e);

      IloConstraintArray *userCuts, *lazyConstraints;

    protected:
      Expression* getSolutionValue(Id* id, SolutionCallbackI* cb = NULL);
      virtual Expression* getSolutionValue(Id* id) {
        return getSolutionValue(id, NULL);
      }

      void registerConstraints(void);
  };

}

#endif
