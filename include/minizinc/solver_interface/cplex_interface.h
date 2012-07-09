#ifndef __CPLEX_INTERFACE_H
#define __CPLEX_INTERFACE_H

#include "solver_interface.h"
#include <ilcplex/ilocplex.h>
namespace MiniZinc {
class CplexInterface: public SolverInterface {
  private:
    IloEnv env;
    IloModel* model;
  public:
    CplexInterface(){
      *model = IloModel(env);
    }
    virtual ~CplexInterface(){
      model->end();
      free(model);
      env.end();
    }
  void* addSolverVar(VarDecl* vd){
    MiniZinc::TypeInst* ti = vd->_ti;
    IloNumVar::Type type;
    switch(ti->_type._bt){
    case Type::BT_INT:
      type = ILOFLOAT;
      break;
    case Type::BT_BOOL:
      type = ILOBOOL;
      break;
    case Type::BT_FLOAT:
      type = ILOFLOAT;
      break;
    default:
      std::cerr << "This type of var is not handled by CPLEX." << std::endl;
      throw -1;
    }
    if(ti->isarray()){/*
      IloNumVarArray* res = new IloNumVarArray(env,IloNum(0),IloInfinity,type);
      for(unsigned int i = 0; i < ti->_ranges->size(); i++){
	//dimension i
	Expression* range = (*(ti->_ranges))[i];
	std::pair<int,int> rangebounds = getBounds(range);
      }
     Expression* domain = ti->_domain;
    std::pair<int,int> bounds = getBounds(domain);
    res->setBounds(bounds.first, bounds.second);
    return (void*)res;*/
      return NULL;
    }
    else{
      IloNumVar* res = new IloNumVar(env,IloNum(0),IloInfinity,type);
      Expression* domain = ti->_domain;
      std::pair<int,int> bounds = getBounds(domain);
      res->setBounds(bounds.first, bounds.second);
      return (void*)res;
    }
  }
  std::pair<int,int> getBounds(Expression* e){
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
