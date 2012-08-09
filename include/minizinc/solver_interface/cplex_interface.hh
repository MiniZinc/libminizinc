/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
      IloExpr* var = new IloExpr(model->getEnv(), v);
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
    void* resolveArrayLit(ArrayLit* al){
      unsigned int size = (*al->_dims)[0].second;
      IloArray<IloExpr*>* res = new IloArray<IloExpr*>(model->getEnv(), size);
      for(unsigned int i = 0; i < size; i++){
	(*res)[i] = (IloExpr*)resolveVar((*al->_v)[i]);
      }
      return (void*) res;
    }
  };
};
#endif
