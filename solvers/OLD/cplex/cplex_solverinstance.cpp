/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/OLD/cplex_solverinstance.hh>
#include <minizinc/eval_par.hh>

namespace MiniZinc {

  namespace CplexConstraints {
    
    bool CheckAnnUserCut(const Call* call) {
      if(!call->ann().isEmpty()) {
        if(call->ann().contains(constants().ann.user_cut)) {
          return true;
        }
      }
      return false;
    }
    bool CheckAnnLazyConstraint(const Call* call) {
      if(!call->ann().isEmpty()) {
        if(call->ann().contains(constants().ann.lazy_constraint)) {
          return true;
        }
      }
      return false;
    }

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
      const bool fUC = CheckAnnUserCut(call);
      const bool fLC = CheckAnnLazyConstraint(call);
      if (fUC) {
        si.userCuts->add(range);
        // std::cerr << " UC " << std::flush;
      }
      if (fLC) {
        si.lazyConstraints->add(range);
       // std::cerr << " UC " << std::flush;
      }
      if (!fUC && !fLC)
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
      
      const bool fUC = CheckAnnUserCut(call);
      const bool fLC = CheckAnnLazyConstraint(call);
      if (fUC)
        si.userCuts->add(constraint);
      if (fLC)
        si.lazyConstraints->add(constraint);
      if (!fUC && !fLC)
        model->add(constraint);
    }
    void p_le(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      ASTExprVec<Expression> args = call->args();
      IloExpr vara = si.exprToIloExpr(args[0]);
      IloExpr varb = si.exprToIloExpr(args[1]);
      IloModel* model = si.getIloModel();
      IloConstraint constraint(vara <= varb);
      const bool fUC = CheckAnnUserCut(call);
      const bool fLC = CheckAnnLazyConstraint(call);
      if (fUC)
        si.userCuts->add(constraint);
      if (fLC)
        si.lazyConstraints->add(constraint);
      if (!fUC && !fLC)
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
      const bool fUC = CheckAnnUserCut(call);
      const bool fLC = CheckAnnLazyConstraint(call);
      if (fUC)
        si.userCuts->add(constraint);
      if (fLC)
        si.lazyConstraints->add(constraint);
      if (!fUC && !fLC)
        model->add(constraint);
    }
  
  /// INDICATORS
    template<typename T>
    void p_lin_reif(SolverInstanceBase& si0, const Call* call, LIN_CON_TYPE lt) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      IloModel* model = (IloModel*) (si.getIloModel());
      ASTExprVec<Expression> args = call->args();
      IloNumArray ilocoeffs = si.exprToIloNumArray(args[0]);
      IloNumVarArray ilovars = si.exprToIloNumVarArray(args[1]);
      IloNum res = si.exprToIloNum(args[2]);
      IloNumVar flg = si.exprToIloNumVar(args[3]);

      switch (lt) {
        case LQ:
          model->add(flg == (IloScalProd(ilovars, ilocoeffs) <= res));
          break;
        case EQ:
          model->add(flg == (IloScalProd(ilovars, ilocoeffs) == res));
          break;
        default:
          abort();
      }
    }
    template<typename T>
    void p_lin_le0_if0(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      IloModel* model = (IloModel*) (si.getIloModel());
      ASTExprVec<Expression> args = call->args();
      IloNumVar res = si.exprToIloNumVar(args[0]);
      IloNumVar flg = si.exprToIloNumVar(args[1]);

      model->add(IloIfThen(model->getEnv(), 0==flg, res<=0));
    }
    template<typename T>
    void p_lin_eq_if1(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      IloModel* model = (IloModel*) (si.getIloModel());
      ASTExprVec<Expression> args = call->args();
      IloNumVar res1 = si.exprToIloNumVar(args[0]);
      IloNumVar res2 = si.exprToIloNumVar(args[1]);
      IloNumVar flg = si.exprToIloNumVar(args[2]);

      model->add(IloIfThen(model->getEnv(), 1==flg, res1==res2));
    }
    
    void p_int_lin_reif(SolverInstanceBase& si, const Call* call, LIN_CON_TYPE lt) {
      p_lin_reif<long long int>(si, call, lt);
    }
    void p_int_lin_le_reif(SolverInstanceBase& si, const Call* call) {
      p_int_lin_reif(si, call, LQ);
    }
    void p_int_lin_eq_reif(SolverInstanceBase& si, const Call* call) {
      p_int_lin_reif(si, call, EQ);
    }
    void p_float_lin_reif(SolverInstanceBase& si, const Call* call, LIN_CON_TYPE lt) {
      p_lin_reif<double>(si, call, lt);
    }
    void p_float_lin_le_reif(SolverInstanceBase& si, const Call* call) {
      p_float_lin_reif(si, call, LQ);
    }
    void p_float_lin_eq_reif(SolverInstanceBase& si, const Call* call) {
      p_float_lin_reif(si, call, EQ);
    }
    void p_int_le0_if0(SolverInstanceBase& si, const Call* call) {
      p_lin_le0_if0<long long int>(si, call);
    }
    void p_float_le0_if0(SolverInstanceBase& si, const Call* call) {
      p_lin_le0_if0<double>(si, call);
    }
    void p_float_eq_if1(SolverInstanceBase& si, const Call* call) {
      p_lin_eq_if1<double>(si, call);
    }
    void p_lin_ne(SolverInstanceBase& si0, const Call* call) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      ASTExprVec<Expression> args = call->args();
      IloNumArray coefs = si.exprToIloNumArray(args[0]);
      IloNumVarArray vars = si.exprToIloNumVarArray(args[1]);
      IloNum rhs = si.exprToIloNum(args[2]);
      IloModel* model = si.getIloModel();
      
      model->add(IloScalProd(coefs, vars) != rhs);
    }

    enum P_MINMAX {p_min, p_max};
    void p_minmax(SolverInstanceBase& si0, const Call* call, P_MINMAX pmm) {
      CPLEXSolverInstance& si = static_cast<CPLEXSolverInstance&>(si0);
      ASTExprVec<Expression> args = call->args();
      IloNumVar z = si.exprToIloNumVar(args[0]);
      IloNumVarArray vara = si.exprToIloNumVarArray(args[1]);
      IloModel* model = si.getIloModel();
      
      if (p_min == pmm)
        model->add(z == IloMin(vara));
      else
        model->add(z == IloMax(vara));
    }
    void p_int_min(SolverInstanceBase& si, const Call* call) {
      p_minmax(si, call, p_min);
    }
    void p_int_max(SolverInstanceBase& si, const Call* call) {
      p_minmax(si, call, p_max);
    }
    void p_float_min(SolverInstanceBase& si, const Call* call) {
      p_minmax(si, call, p_min);
    }
    void p_float_max(SolverInstanceBase& si, const Call* call) {
      p_minmax(si, call, p_max);
    }

  }  // namespace CplexConstraints

  
  CPLEXSolverInstance::CPLEXSolverInstance(Env& env, const Options& options)
  : SolverInstanceImpl<CPLEXSolver>(env,options), _ilomodel(NULL), _ilocplex(NULL) {
    fVerbose      = options.getBoolParam  ("verbose",          false);
    all_solutions = options.getBoolParam  ("all_solutions",    false);
    nThreads      = options.getIntParam   ("parallel_threads",     1);
    nTimeout      = options.getFloatParam ("timelimit",          0.0);
    sExportModel  = options.getStringParam("export_model",        "");
    nWorkMemLimit = options.getFloatParam ("memory_limit",        -1);
    sReadParam    = options.getStringParam("read_param",          "");
    sWriteParam   = options.getStringParam("write_param",         "");

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
    
    /// INDICATORS
    _constraintRegistry.add(ASTString("int_lin_eq_reif__IND"), CplexConstraints::p_int_lin_eq_reif);
    _constraintRegistry.add(ASTString("int_lin_le_reif__IND"), CplexConstraints::p_int_lin_le_reif);
    _constraintRegistry.add(ASTString("int_lin_ne__IND"), CplexConstraints::p_lin_ne);
    _constraintRegistry.add(ASTString("aux_int_le_zero_if_0__IND"), CplexConstraints::p_int_le0_if0);
    _constraintRegistry.add(ASTString("float_lin_le_reif__IND"), CplexConstraints::p_float_lin_le_reif);
    _constraintRegistry.add(ASTString("aux_float_eq_if_1__IND"), CplexConstraints::p_float_eq_if1);
    _constraintRegistry.add(ASTString("aux_float_le_zero_if_0__IND"), CplexConstraints::p_float_le0_if0);

    _constraintRegistry.add(ASTString("array_int_minimum__IND"), CplexConstraints::p_int_min);
    _constraintRegistry.add(ASTString("array_int_maximum__IND"), CplexConstraints::p_int_max);
    _constraintRegistry.add(ASTString("array_float_minimum__IND"), CplexConstraints::p_float_min);
    _constraintRegistry.add(ASTString("array_float_maximum__IND"), CplexConstraints::p_float_max);

  }
  
  CPLEXSolverInstance::~CPLEXSolverInstance(void) {
//     _ilomodel->end();           -- TAKES TOO LONG SOMETIMES
    delete userCuts;
    delete lazyConstraints;
    delete _ilomodel;
    delete _ilocplex;
    if(fVerbose) {
      std::cout << "  %DELETING IloEnv... " << std::flush;
    }
    _iloenv.end();
    if(fVerbose) {
      std::cout << " %DONE. " << std::endl;
    }
  }
  
  SolverInstanceBase::Status CPLEXSolverInstance::next(void) {
    return SolverInstance::ERROR;
  }

  class SolutionCallbackI : public IloCplex::MIPInfoCallbackI {
    IloNum lastIncumbent;
    CPLEXSolverInstance& csi;

    public:
    IloCplex::CallbackI* duplicateCallback() const {
      return (new (getEnv()) SolutionCallbackI(*this));
    }

    SolutionCallbackI(IloEnv env, IloNum lI, CPLEXSolverInstance& si)
      : IloCplex::MIPInfoCallbackI(env), lastIncumbent(lI), csi(si) {}

    double getValue(IloNumVar v) { return getIncumbentValue(v); }

    void main() {
      if (hasIncumbent()) {
        IloNum currIncumbent = getIncumbentObjValue();
        if (fabs(lastIncumbent - currIncumbent) > 1e-5*(1.0 + fabs(currIncumbent))) {
          lastIncumbent = currIncumbent;
          csi.printSolution(this);
        }
      }
    }
  };

  IloCplex::Callback SolutionCallback(IloEnv env, IloNum x2, CPLEXSolverInstance& x3) {
    return (IloCplex::Callback(new (env) SolutionCallbackI(env, x2, x3)));
  }

  void CPLEXSolverInstance::printSolution(SolutionCallbackI* cb) {
    assignSolutionToOutput(cb);
    std::stringstream ss;
    _env.evalOutput(ss);
    std::string output = ss.str();

    std::hash<std::string> str_hash;
    size_t h = str_hash(output);
    if(previousOutput.find(h) == previousOutput.end()) {
      previousOutput.insert(h);
      std::cout << output;
      std::cout << "----------" << std::endl;
    }
  }
  
  SolverInstanceBase::Status CPLEXSolverInstance::solve(void) {
    IloObjective obj;
    if (_env.flat()->solveItem()->st() != SolveI::SolveType::ST_SAT) {
      if (_env.flat()->solveItem()->st() == SolveI::SolveType::ST_MAX) {
        obj = IloMaximize(_iloenv);
        if(fVerbose)
          std::cout << "   %  MAXIMIZATION_PROBLEM. " << std::endl;
      }
      else {
        obj = IloMinimize(_iloenv);
        if(fVerbose)
          std::cout << "   %  MINIMIZATION_PROBLEM. " << std::endl;
      }

      IloNumVar v = exprToIloNumVar(_env.flat()->solveItem()->e());
      obj.setLinearCoef(v, 1);
      _ilomodel->add(obj);
    }
    else {
      if(fVerbose)
        std::cout << "   %  SATISFACTION_PROBLEM. " << std::endl;
    }

    try{
      _ilocplex = new IloCplex(*_ilomodel);
      
      if (userCuts->getSize()) {
        if (fVerbose)
          std::cerr << "  % ADDING " << userCuts->getSize() << " USER CUTS." << std::endl;
        _ilocplex->addUserCuts(*userCuts);
      }
      if (lazyConstraints->getSize()) {
        if (fVerbose)
          std::cerr << "  % ADDING " << lazyConstraints->getSize() << " LAZY CONSTRAINTS." << std::endl;
        _ilocplex->addLazyConstraints(*lazyConstraints);
      }

      if (not fVerbose && not all_solutions)
        _ilocplex->setOut(_iloenv.getNullStream());
      else
        _ilocplex->setOut(std::cerr);


      if(all_solutions && obj.getImpl()) {
        IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
          IloInfinity : -IloInfinity;

        _ilocplex->use(SolutionCallback(_iloenv, lastObjVal, *this));
        // Turn off CPLEX logging
        if(!fVerbose)
          _ilocplex->setParam(IloCplex::MIPDisplay, 0);
      }

      if (sExportModel.size())
        _ilocplex->exportModel(sExportModel.c_str());

      if (nThreads>0)
        _ilocplex->setParam(IloCplex::Param::Threads, nThreads);

      if (nTimeout>0) {
        _ilocplex->setParam(IloCplex::Param::ClockType, 1);   // 0 - auto, 1 - CPU, 2- wall clock
        _ilocplex->setParam(IloCplex::Param::TimeLimit, nTimeout);
      }

      if (nWorkMemLimit>0) {
        _ilocplex->setParam(IloCplex::Param::WorkMem, nWorkMemLimit);
      }
      
      if (sReadParam.size())
        _ilocplex->readParam(sReadParam.c_str());
      
      if (sWriteParam.size())
        _ilocplex->writeParam(sWriteParam.c_str());
      
      if(fVerbose)
        std::cerr << "   %  CPLEX VERSION: " << _ilocplex->getVersion() << std::endl;

    // 		_ilocplex->setParam(IloCplex::Param::Emphasis::MIP, 1);      -- SEEMS WORSE ON AMAZE.MZN

      _ilocplex->solve();
      if(fVerbose) std::cout << "  %IloCplex::solve() exited." << std::endl;
    } catch(IloCplex::Exception& e){
      std::stringstream ssm;
      ssm << "Caught IloCplex::Exception while solving : " << e << std::endl;
      throw InternalError(ssm.str());
    }
    IloAlgorithm::Status ss = _ilocplex->getStatus();
    Status s;
    switch(ss) {
      case IloAlgorithm::Status::Optimal:
        s = SolverInstance::OPT;
        if(fVerbose) std::cout << "\n   %----------------------  MIP__OPTIMAL  ----------------------------------" << std::endl;
        assignSolutionToOutput();
        break;
      case IloAlgorithm::Status::Feasible:
        s = SolverInstance::SAT;
        if(fVerbose) std::cout << "\n   %---------------------  MIP__FEASIBLE  ----------------------------------" << std::endl;
        assignSolutionToOutput();
        break;
      case IloAlgorithm::Status::Infeasible:
        s = SolverInstance::UNSAT;
        if(fVerbose) std::cout << "\n   %---------------------  MIP__INFEASIBLE  ----------------------------------" << std::endl;
        break;
      case IloAlgorithm::Status::Unbounded:
      case IloAlgorithm::Status::InfeasibleOrUnbounded:
      case IloAlgorithm::Status::Error:
        s = SolverInstance::ERROR;
        std::cout << "\n   %---------------------   MIP__ERROR   ----------------------------------" << std::endl;
        break;
        //       case IloCplex::Status::AbortTimeLim:    -- getCplexStatus()
        //         s = SolverInstance::TIMELIMIT;
        //         assignSolutionToOutput();
        //         break;
      default:
        s = SolverInstance::UNKNOWN;
        std::cout << "\n   %---------------------   MIP__UNKNOWN_STATUS   ----------------------------------" << std::endl;
    }

    if(fVerbose) {
      if (IloAlgorithm::Status::Optimal==ss || IloAlgorithm::Status::Feasible==ss) {
        /// PRINT THE MAIN RESULTS FIRST:
        std::cout << "% MIP_Objective_ : " << _ilocplex->getObjValue() << std::endl;
        std::cout << "% MIP_AbsGap__   : "
          << std::fabs(_ilocplex->getBestObjValue()-_ilocplex->getObjValue()) << std::endl;
        std::cout << "% MIP_RelGap__   : " << _ilocplex->getMIPRelativeGap() << std::endl;
      } else {
        std::cout << "\n   %------------  NO FEASIBLE SOLUTION FOUND  ----------------------------" << std::endl;
      }
      std::cout   << "% MIP_BestBound_ : " << _ilocplex->getBestObjValue() << std::endl;
      std::cout   << "% Real/CPU Time_ : " << _ilocplex->getTime() << " sec\n" << std::endl;
      std::cout << "%------------------------------------------------------------------------\n"<< std::endl;
    }
    return s;

  }
  
  void CPLEXSolverInstance::processFlatZinc(void) {
    _ilomodel = new IloModel(_iloenv);
    userCuts = new IloConstraintArray(_iloenv);
    lazyConstraints = new IloConstraintArray(_iloenv);
  
    for (VarDeclIterator it = _env.flat()->begin_vardecls(); it != _env.flat()->end_vardecls(); ++it) {
      VarDecl* vd = it->e();
      if(!vd->ann().isEmpty()) {
        if(vd->ann().containsCall(constants().ann.output_array.aststr()) ||
            vd->ann().contains(constants().ann.output_var)
          ) {
          _varsWithOutput.push_back(vd);
        }
      }
      if (vd->type().dim() == 0 && it->e()->type().isvar()) {
        MiniZinc::TypeInst* ti = it->e()->ti();
        IloNumVar::Type type;
        if (ti->type().isvarint()) {
          type = ILOINT;
        } else if (ti->type().isvarbool()) {
          type = ILOBOOL;
        } else if (ti->type().isvarfloat()) {
          type = ILOFLOAT;
        } else {
          std::stringstream ssm;
          ssm << "This type of var is not handled by CPLEX: " << *it << std::endl;
          throw InternalError(ssm.str());
        }
        IloNum lb, ub;
        if (ti->domain()) {
          if (type == ILOFLOAT) {
            FloatBounds fb = compute_float_bounds(_env.envi(), it->e()->id());
            assert(fb.valid);
            lb = fb.l;
            ub = fb.u;
          } else if (type == ILOINT) {
            IntBounds ib = compute_int_bounds(_env.envi(), it->e()->id());
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
        Id* id = it->e()->id();
        id = id->decl()->id();
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

  Expression* CPLEXSolverInstance::getSolutionValue(Id* id, SolutionCallbackI* cb) {
    id = id->decl()->id();
    if(id->type().isvar()) {
      IloNumVar var = exprToIloNumVar(id);
      IloNum val;
      if(cb) {
        val = cb->getValue(var);
      } else {
        val = _ilocplex->getValue(var);
      }

      switch (id->type().bt()) {
        case Type::BT_INT: return IntLit::a(round_to_longlong(val));
        case Type::BT_FLOAT: return new FloatLit(Location(), val);
        case Type::BT_BOOL: return new BoolLit(Location(), round_to_longlong(val));
        default: return NULL;
      }
    } else {
      return id->decl()->e();
    }
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
      ident = ident->decl()->id();
      return _variableMap.get(ident);
    }
    assert(false);
    return IloExpr();
  }

  IloNumArray CPLEXSolverInstance::exprToIloNumArray(Expression* e) {
    ArrayLit* al = eval_array_lit(_env.envi(), e);
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
      ident = ident->decl()->id();
      return _variableMap.get(ident);
    }
    assert(false);
    return IloNumVar();
  }

  IloNumVarArray CPLEXSolverInstance::exprToIloNumVarArray(Expression* e) {
    ArrayLit* al = eval_array_lit(_env.envi(), e);
    IloNumVarArray a(_iloenv);
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (IntLit* il = al->v()[i]->dyn_cast<IntLit>()) {
        a.add(IloNumVar(_iloenv, il->v().toInt(), il->v().toInt()));
      } else if (FloatLit* fl = al->v()[i]->dyn_cast<FloatLit>()) {
        a.add(IloNumVar(_iloenv, fl->v(), fl->v()));
      } else if (BoolLit* bl = al->v()[i]->dyn_cast<BoolLit>()) {
        a.add(IloNumVar(_iloenv, bl->v(), bl->v()));
      } else if (Id* ident = al->v()[i]->dyn_cast<Id>()) {
        ident = ident->decl()->id();
        a.add(_variableMap.get(ident));
      } else {
        std::cerr << "unknown expression type\n";
        assert(false);
      }
    }
    return a;
  }
  
  void CPLEXSolverInstance::assignSolutionToOutput(SolutionCallbackI* cb) {
    // should make this faster  TODO
    //iterate over set of ids that have an output annotation and obtain their right hand side from the flat model
    for(unsigned int i=0; i<_varsWithOutput.size(); i++) {
      VarDecl* vd = _varsWithOutput[i];
      //std::cout << "DEBUG: Looking at var-decl with output-annotation: " << *vd << std::endl;
      if(Call* output_array_ann = Expression::dyn_cast<Call>(getAnnotation(vd->ann(), constants().ann.output_array.aststr()))) {
        assert(vd->e());

        if(ArrayLit* al = vd->e()->dyn_cast<ArrayLit>()) {
          std::vector<Expression*> array_elems;
          ASTExprVec<Expression> array = al->v();
          for(unsigned int j=0; j<array.size(); j++) {
            if(Id* id = array[j]->dyn_cast<Id>()) {
              //std::cout << "DEBUG: getting solution value from " << *id  << " : " << id->v() << std::endl;
              array_elems.push_back(getSolutionValue(id, cb));
            } else if(FloatLit* floatLit = array[j]->dyn_cast<FloatLit>()) {
              array_elems.push_back(floatLit);
            } else if(IntLit* intLit = array[j]->dyn_cast<IntLit>()) {
              array_elems.push_back(intLit);
            } else if(BoolLit* boolLit = array[j]->dyn_cast<BoolLit>()) {
              array_elems.push_back(boolLit);
            } else {
              std::cerr << "Error: array element " << *array[j] << " is not an id nor a literal" << std::endl;
              assert(false);
            }
          }
          GCLock lock;
          ArrayLit* dims;
          Expression* e = output_array_ann->args()[0];
          if(ArrayLit* al = e->dyn_cast<ArrayLit>()) {
            dims = al;
          } else if(Id* id = e->dyn_cast<Id>()) {
            dims = id->decl()->e()->cast<ArrayLit>();
          } else {
            throw -1;
          }
          std::vector<std::pair<int,int> > dims_v;
          for(unsigned int i=0;i<dims->length();i++) {
            IntSetVal* isv = eval_intset(_env.envi(), dims->v()[i]);
            dims_v.push_back(std::pair<int,int>(isv->min(0).toInt(),isv->max(isv->size()-1).toInt()));
          }
          ArrayLit* array_solution = new ArrayLit(Location(),array_elems,dims_v);
          KeepAlive ka(array_solution);
          // add solution to the output
          for (VarDeclIterator it = _env.output()->begin_vardecls(); it != _env.output()->end_vardecls(); ++it) {
            if(it->e()->id()->str() == vd->id()->str()) {
              //std::cout << "DEBUG: Assigning array solution to " << it->e()->id()->str() << std::endl;
              it->e()->e(array_solution); // set the solution
            }
          }
        }
      } else if(vd->ann().contains(constants().ann.output_var)) {
        Expression* sol = getSolutionValue(vd->id(), cb);
        vd->e(sol);
        for (VarDeclIterator it = _env.output()->begin_vardecls(); it != _env.output()->end_vardecls(); ++it) {
          if(it->e()->id()->str() == vd->id()->str()) {
            //std::cout << "DEBUG: Assigning array solution to " << it->e()->id()->str() << std::endl;
            it->e()->e(sol); // set the solution
          }
        }
      }
    }

  }
}
