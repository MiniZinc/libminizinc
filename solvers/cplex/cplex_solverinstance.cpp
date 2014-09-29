/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cplex_solverinstance.hh"
#include <minizinc/eval_par.hh>

namespace MiniZinc {

  namespace CplexConstraints {

    enum LIN_CON_TYPE {LQ,EQ,GQ};

    template<typename T>
    void p_lin(SolverInstanceBase& si0, const Call* call, LIN_CON_TYPE lt) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      IloModel* model = (IloModel*) (si.getIloModel());
      ASTExprVec<Expression> args = call->args();
      IloNumArray ilocoeffs = si.exprToIloNumArray(args[0]);
      IloNumVarArray ilovars = si.exprToIloNumVarArray(args[1]);
      IloNum res = si.exprToIloNum(args[2]);
      
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
      range.setLinearCoefs(ilovars, ilocoeffs);
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
  
  SolverInstanceBase::Status CPLEXSolverInstance::solve(void) {
    if (_env.flat()->solveItem()->st() != SolveI::SolveType::ST_SAT) {
      IloObjective obj;
      if (_env.flat()->solveItem()->st() == SolveI::SolveType::ST_MAX)
        obj = IloMaximize(_iloenv);
      else
        obj = IloMinimize(_iloenv);
      
      IloNumVar v = exprToIloNumVar(_env.flat()->solveItem()->e());
      obj.setLinearCoef(v, 1);
      _ilomodel->add(obj);
    }

    _ilocplex = new IloCplex(*_ilomodel);
    
//    _ilocplex->setOut(_iloenv.getNullStream());
    
    try{
      _ilocplex->solve();
    } catch(IloCplex::Exception& e){
      std::cerr << "Caught IloCplex::Exception while solving : " << std::endl
      << e << std::endl;
      std::exit(0);
    }
    IloCplex::Status ss = _ilocplex->getCplexStatus();
    Status s;
    switch(ss) {
      case IloCplex::Status::Optimal:
      case IloCplex::Status::OptimalTol:
        s = OPT;
        break;
      case IloCplex::Status::Feasible:
        s = SAT;
        break;
      case IloCplex::Status::Infeasible:
        s = UNSAT;
        break;
      case IloCplex::Status::Unbounded:
        s = ERROR;
        break;
      case IloCplex::Status::AbortTimeLim:
        s = UNKNOWN;
        break;
      default:
        s = UNKNOWN;
    }
    return s;
    
  }
  
  void CPLEXSolverInstance::processFlatZinc(void) {
    _ilomodel = new IloModel(_iloenv);
  
    for (VarDeclIterator it = _env.flat()->begin_vardecls(); it != _env.flat()->end_vardecls(); ++it) {
      if (it->e()->type().dim() == 0 && it->e()->type().isvar()) {
        MiniZinc::TypeInst* ti = it->e()->ti();
        IloNumVar::Type type;
        switch (ti->type().bt()) {
          case Type::BT_INT:
            type = ILOINT;
            break;
          case Type::BT_BOOL:
            type = ILOBOOL;
            break;
          case Type::BT_FLOAT:
            type = ILOFLOAT;
            break;
          default:
            std::cerr << "This type of var is not handled by CPLEX: " << *it << std::endl;
            std::exit(-1);
        }
        IloNum lb, ub;
        if (ti->domain()) {
          if (type == ILOFLOAT) {
            FloatBounds fb = compute_float_bounds(it->e()->id());
            assert(fb.valid);
            lb = fb.l;
            ub = fb.u;
          } else if (type == ILOINT) {
            IntBounds ib = compute_int_bounds(it->e()->id());
            assert(ib.valid);
            lb = ib.l.toInt();
            ub = ib.u.toInt();
          } else {
            lb = -IloInfinity;
            ub = IloInfinity;
          }
        } else {
          lb = -IloInfinity;
          ub = IloInfinity;
        }
        IloNumVar res(_iloenv, lb, ub, type, it->e()->id()->str().c_str());
        _ilomodel->add(res);
        if (it->e()->e()) {
          _ilomodel->add(IloConstraint(res == exprToIloExpr(it->e()->e())));
        }
        _variableMap.insert(it->e()->id(), res);
      }
      
    }

    for (ConstraintIterator it = _env.flat()->begin_constraints(); it != _env.flat()->end_constraints(); ++it) {
      if (Call* c = it->e()->dyn_cast<Call>()) {
        _constraintRegistry.post(c);
      }
    }

  }
  
  void CPLEXSolverInstance::resetSolver(void) {}

  Expression* CPLEXSolverInstance::getSolutionValue(Id* id) {
    return NULL;
  }

  IloModel* CPLEXSolverInstance::getIloModel(void) { return _ilomodel; }

  IloNum CPLEXSolverInstance::exprToIloNum(MiniZinc::Expression *e) {
    if (IntLit* il = e->dyn_cast<IntLit>()) {
      return il->v().toInt();
    } else if (FloatLit* fl = e->dyn_cast<FloatLit>()) {
      return fl->v();
    } else if (BoolLit* bl = e->dyn_cast<BoolLit>()) {
      return bl->v();
    }
    assert(false);
    return 0;
  }

  IloNumExpr CPLEXSolverInstance::exprToIloExpr(MiniZinc::Expression *e) {
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

  IloNumArray CPLEXSolverInstance::exprToIloNumArray(Expression* e) {
    ArrayLit* al = eval_array_lit(e);
    IloNumArray a(_iloenv, al->v().size());
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (IntLit* il = al->v()[i]->dyn_cast<IntLit>()) {
        a[i] =  il->v().toInt();
      } else if (FloatLit* fl = al->v()[i]->dyn_cast<FloatLit>()) {
        a[i] = fl->v();
      } else if (BoolLit* bl = al->v()[i]->dyn_cast<BoolLit>()) {
        a[i] = bl->v();
      } else {
        throw "unexpected expression";
      }
    }
    return a;
  }

  IloNumVar CPLEXSolverInstance::exprToIloNumVar(Expression* e) {
    if (IntLit* il = e->dyn_cast<IntLit>()) {
      return IloNumVar(_iloenv, il->v().toInt(), il->v().toInt());
    } else if (FloatLit* fl = e->dyn_cast<FloatLit>()) {
      return IloNumVar(_iloenv, fl->v(), fl->v());
    } else if (BoolLit* bl = e->dyn_cast<BoolLit>()) {
      return IloNumVar(_iloenv, bl->v(), bl->v());
    } else if (Id* ident = e->dyn_cast<Id>()) {
      return _variableMap.get(ident);
    }
    assert(false);
    return IloNumVar();
  }

  IloNumVarArray CPLEXSolverInstance::exprToIloNumVarArray(Expression* e) {
    ArrayLit* al = eval_array_lit(e);
    IloNumVarArray a(_iloenv);
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (IntLit* il = al->v()[i]->dyn_cast<IntLit>()) {
        a.add(IloNumVar(_iloenv, il->v().toInt(), il->v().toInt()));
      } else if (FloatLit* fl = al->v()[i]->dyn_cast<FloatLit>()) {
        a.add(IloNumVar(_iloenv, fl->v(), fl->v()));
      } else if (BoolLit* bl = al->v()[i]->dyn_cast<BoolLit>()) {
        a.add(IloNumVar(_iloenv, bl->v(), bl->v()));
      } else if (Id* ident = al->v()[i]->dyn_cast<Id>()) {
        a.add(_variableMap.get(ident));
      } else {
        std::cerr << "unknown expression type\n";
        assert(false);
      }
    }
    return a;
  }
  
}
