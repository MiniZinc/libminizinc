#ifndef __CPLEX_INTERFACE_H
#define __CPLEX_INTERFACE_H

#include "solver_interface.h"
#include <ilcplex/ilocplex.h>
#include <string>

namespace MiniZinc {
  class CplexInterface : public SolverInterface {
  private:
    IloEnv env;
    IloModel* model;
  protected:

    void* addSolverVar(VarDecl* vd);
  public:
    CplexInterface();
    virtual ~CplexInterface();

    void* resolveVar(Expression* e);
    void* getModel();
    void solve(SolveI* s);
    std::string showVariables(IloCplex& cplex);
    std::string showVariable(IloCplex& cplex, IloNumVar& v);
    enum LIN_CON_TYPE {LQ,EQ,GQ};
    template<typename T> void initArray(IloNumVarArray& res, CtxVec<Expression*>& ar);
    //    template<typename T, typename S> T getNumber(Expression* e);
  };
};
#endif
