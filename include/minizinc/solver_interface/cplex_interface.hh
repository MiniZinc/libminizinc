#ifndef __CPLEX_INTERFACE_H
#define __CPLEX_INTERFACE_H

#include "solver_interface.hh"
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

    void* getModel();
    void solve(SolveI* s);
    std::string showVariables(IloCplex& cplex);
    std::string showVariable(IloCplex& cplex, IloNumVar& v);
    enum LIN_CON_TYPE {LQ,EQ,GQ};
    template<typename S, typename T> void initArray(IloNumVarArray& res, CtxVec<Expression*>& ar);

    void* resolveArrayAccess(void* array, int index){
      IloNumVarArray *inva = static_cast<IloNumVarArray*>(array);    
      return (void*) (&((*inva)[index]));
    }
    template<typename T> void* resolveLit(T v, IloNumVar::Type type){
      IloNumVar* var = new IloNumVar(model->getEnv(), v, v, type);
      return (void*) var;
    }
    void* resolveIntLit(int v){
      return resolveLit<int>(v,ILOINT);
    }
    void* resolveBoolLit(bool v){
      return resolveLit<bool>(v,ILOBOOL);
    }
    void* resolveFloatLit(double v){
      return resolveLit<double>(v,ILOFLOAT);
    }
  };
};
#endif
