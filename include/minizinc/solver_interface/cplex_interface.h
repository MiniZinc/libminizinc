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

    static std::pair<double,double> getBounds(Expression* e){
      BinOp* bo = e->cast<BinOp>();
      int b; int u;
      b = bo->_e0->cast<IntLit>()->_v;
      u = bo->_e1->cast<IntLit>()->_v;
      return std::pair<int,int>(b,u);
    }
    void solve(){
      std::cout << "Solving !" << std::endl;
    }
  };
};
#endif
