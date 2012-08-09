/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CPOPT_INTERFACE_H
#define __CPOPT_INTERFACE_H

#include "solver_interface.hh"
#include <ilcp/cp.h>
#include <string>

namespace MiniZinc {
  class CpOptInterface : public SolverInterface {
  private:
    IloEnv env;
    IloModel* model;
  protected:

    void* addSolverVar(VarDecl* vd);
  public:
    CpOptInterface();
    virtual ~CpOptInterface();

    void* getModel();
    void solve(SolveI* s);
    std::string showVariables(IloCP& cplex);
    std::string showVariable(IloCP& cplex, IloNumVar& v);
    enum LIN_CON_TYPE {LQ,EQ,GQ};
    template<typename S, typename T> void initArray(IloNumExprArray& res, CtxVec<Expression*>& ar);
    // template<typename S, typename T> void initArray(IloNumVarArray& res, CtxVec<Expression*>& ar);
   
    void* resolveArrayAccess(void* array, int index){
      IloNumVarArray *inva = static_cast<IloNumVarArray*>(array);    
      return (void*) (&((*inva)[index]));
    }
    template<typename T> void* resolveLit(T v, IloNumVar::Type type){
      IloNumExpr* var = new IloNumExpr(model->getEnv(),v);
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
    bool isLit(Expression* e){
      return e->isa<IntLit>() || e->isa<BoolLit>() || e->isa<FloatLit>() || (e->isa<UnOp>() && isLit(e->cast<UnOp>()->_e0));
    }
    void* resolveArrayLit(ArrayLit* al){
      unsigned int size = (*al->_dims)[0].second;
      if(size == 0){
	std::cerr << "ArrayLit of size 0 in resolveArrayLit" << std::endl;
      }
      bool allLit = true;
      for(unsigned int i = 0; i < size; i++){
	Expression* e = (*al->_v)[i];
	if(!isLit(e)){
	  allLit = false;
	  break;
	}
      }
      if(!allLit){
	IloIntVarArray* res = new IloIntVarArray(model->getEnv(), size);
	for(unsigned int i = 0; i < size; i++){
	  (*res)[i] = (*((IloIntVar*)resolveVar((*al->_v)[i])));
	}
	return (void*) res;
      } else {
	
	IloNumExprArray* res = new IloNumExprArray(model->getEnv());
	for(unsigned int i = 0; i < size; i++){
	  IloNumExpr* v = (IloNumExpr*)resolveVar((*al->_v)[i]);
	  res->add(*v);
	}
	return (void*) res;
      }
    }

    void searchGoal(Call* c, IloCP& cp);
    void setObjective(SolveI* s);
    
  
  };
};
#endif
