/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cplex_solverinstance.hh"

namespace MiniZinc {

  namespace CplexConstraints {

    enum LIN_CON_TYPE {LQ,EQ,GQ};

    template<typename T>
    void p_lin(SolverInstanceBase& si0, const Call* call, LIN_CON_TYPE lt) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      IloModel* model = (IloModel*) (si.getIloModel());
      ASTExprVec<Expression> args = call->args();
//      IloNumArray  ilocoeffs = si.arg2NumArray<T>(args[0]);
//      IloNumVarArray ilovars = si.arg2VarArray(args[1]);
//      T res = si.getNumber<T>(args[2]);
      T res = 0;
      
      IloNum lb, ub;
      lb = -IloInfinity;
      ub = IloInfinity;
      switch (lt) {
        case LQ:
          ub = res;
          break;
        case GQ:
          lb = res;
          break;
        case EQ:
          ub = res;
          lb = res;
          break;
      }
      IloRange range(model->getEnv(), lb, ub);
//      range.setLinearCoefs(ilovars, ilocoeffs);
      model->add(range);
    }
    
    void p_int_lin(SolverInstanceBase& si, const Call* call, LIN_CON_TYPE lt) {
      p_lin<long long int>(si, call, lt);
    }
    void p_int_lin_le(SolverInstanceBase& si, const Call* call) {
      p_int_lin(si, call, LQ);
    }
    void p_int_lin_eq(SolverInstanceBase& si, const Call* call) {
      p_int_lin(si, call, EQ);
    }
    void p_float_lin(SolverInstanceBase& si, const Call* call, LIN_CON_TYPE lt) {
      p_lin<double>(si, call, lt);
    }
    void p_float_lin_le(SolverInstanceBase& si, const Call* call) {
      p_float_lin(si, call, LQ);
    }
    void p_float_lin_eq(SolverInstanceBase& si, const Call* call) {
      p_float_lin(si, call, EQ);
    }
    void p_eq(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      ASTExprVec<Expression> args = call->args();
      IloExpr vara = si.exprToIloExpr(args[0]);
      IloExpr varb = si.exprToIloExpr(args[1]);
      IloModel* model = si.getIloModel();
      IloConstraint constraint(vara == varb);
      
      model->add(constraint);
    }
    void p_le(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      ASTExprVec<Expression> args = call->args();
      IloExpr vara = si.exprToIloExpr(args[0]);
      IloExpr varb = si.exprToIloExpr(args[1]);
      IloModel* model = si.getIloModel();
      IloConstraint constraint(vara <= varb);
      model->add(constraint);
    }
    void p_plus(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      ASTExprVec<Expression> args = call->args();
      IloExpr vara = si.exprToIloExpr(args[0]);
      IloExpr varb = si.exprToIloExpr(args[1]);
      IloExpr varc = si.exprToIloExpr(args[2]);
      IloModel* model = si.getIloModel();
      IloConstraint constraint(varc == (vara + varb));
      model->add(constraint);
      
    }
  }
  
  CPLEXSolverInstance::CPLEXSolverInstance(Env& env, const Options& options)
  : SolverInstanceImpl<CPLEXSolver>(env,options), _ilomodel(NULL), _ilocplex(NULL) {
    
    registerConstraints();
    
    processFlatZinc();
  }

  void CPLEXSolverInstance::registerConstraints() {
    _constraintRegistry.add(ASTString("int2float"), CplexConstraints::p_eq);
    _constraintRegistry.add(ASTString("bool_eq"), CplexConstraints::p_eq);
    _constraintRegistry.add(ASTString("int_eq"), CplexConstraints::p_eq);
    _constraintRegistry.add(ASTString("int_le"), CplexConstraints::p_le);
    _constraintRegistry.add(ASTString("int_lin_eq"), CplexConstraints::p_int_lin_eq);
    _constraintRegistry.add(ASTString("int_lin_le"), CplexConstraints::p_int_lin_le);
    _constraintRegistry.add(ASTString("int_plus"), CplexConstraints::p_plus);
    _constraintRegistry.add(ASTString("bool2int"), CplexConstraints::p_eq);
    _constraintRegistry.add(ASTString("float_eq"), CplexConstraints::p_eq);
    _constraintRegistry.add(ASTString("float_le"), CplexConstraints::p_le);
    _constraintRegistry.add(ASTString("float_lin_eq"), CplexConstraints::p_float_lin_eq);
    _constraintRegistry.add(ASTString("float_lin_le"), CplexConstraints::p_float_lin_le);
    _constraintRegistry.add(ASTString("float_plus"), CplexConstraints::p_plus);
  }
  
  CPLEXSolverInstance::~CPLEXSolverInstance(void) {
    _ilomodel->end();
    delete _ilomodel;
    delete _ilocplex;
    _iloenv.end();
  }
  
  SolverInstanceBase::Status CPLEXSolverInstance::next(void) {
    return ERROR;
  }
  
  void CPLEXSolverInstance::processFlatZinc(void) {}
  
  void CPLEXSolverInstance::resetSolver(void) {}

  Expression* CPLEXSolverInstance::getSolutionValue(Id* id) {
    return NULL;
  }

  IloModel* CPLEXSolverInstance::getIloModel(void) { return _ilomodel; }

  IloExpr CPLEXSolverInstance::exprToIloExpr(MiniZinc::Expression *e) {
    if (IntLit* il = e->dyn_cast<IntLit>()) {
      return IloExpr(_iloenv, il->v().toInt());
    } else if (FloatLit* fl = e->dyn_cast<FloatLit>()) {
      return IloExpr(_iloenv, fl->v());
    } else if (BoolLit* bl = e->dyn_cast<BoolLit>()) {
      return IloExpr(_iloenv, bl->v());
    } else if (Id* ident = e->dyn_cast<Id>()) {
      return _variableMap.get(ident);
    }
    assert(false);
    return IloExpr();
  }
}

























