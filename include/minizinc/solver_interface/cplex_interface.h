#ifndef __CPLEX_INTERFACE_H
#define __CPLEX_INTERFACE_H

#include "solver_interface.h"
#include <ilcplex/ilocplex.h>

namespace MiniZinc {
  class CplexInterface : public SolverInterface {
  private:
    IloEnv env;
    IloModel* model;
  public:
    CplexInterface();
    virtual ~CplexInterface();
    void* addSolverVar(VarDecl* vd);
    void* getEnv();
    void solve(SolveI* s);
  };
};
#endif
