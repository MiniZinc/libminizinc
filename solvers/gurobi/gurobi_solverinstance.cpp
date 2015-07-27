/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/gurobi_solverinstance.hh>
#include <minizinc/eval_par.hh>

namespace MiniZinc {

  GRBVar GurobiSolverInstance::exprToVar(Expression* arg) {
    if (IntLit* il = arg->dyn_cast<IntLit>()) {
      return _grb_model->addVar(il->v().toInt(), il->v().toInt(), 0.0, GRB_INTEGER, "temp_i");
    } else if (FloatLit* fl = arg->dyn_cast<FloatLit>()) {
      return _grb_model->addVar(fl->v(), fl->v(), 0.0, GRB_CONTINUOUS, "temp_f");
    } else if (BoolLit* bl = arg->dyn_cast<BoolLit>()) {
      return _grb_model->addVar(bl->v(), bl->v(), 0.0, GRB_INTEGER, "temp_b");
    } else if (Id* ident = arg->dyn_cast<Id>()) {
      return _variableMap.get(ident->decl()->id());
    }

    std::cerr << "unknown expression type\n";
    assert(false);
  }

  GRBVar* GurobiSolverInstance::exprToVarArray(Expression* arg) {
    ArrayLit* al = eval_array_lit(_env.envi(), arg);
    GRBVar* a = new GRBVar[al->v().size()];
    for (unsigned int i=0; i<al->v().size(); i++)
      a[i] = exprToVar(al->v()[i]);
    return a;
  }

  double* GurobiSolverInstance::exprToArray(Expression* arg) {
    ArrayLit* al = eval_array_lit(_env.envi(), arg);
    double* a = new double[al->v().size()];
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

  namespace GurobiConstraints {
    enum LIN_CON_TYPE {LQ,EQ,GQ};

    void p_lin(SolverInstanceBase& si, const Call* call, LIN_CON_TYPE lt) {
      GurobiSolverInstance& gi = (GurobiSolverInstance&) si;
      Env& _env = gi.env();
      GRBModel* grb_model = gi.getGRBModel();
      ASTExprVec<Expression> args = call->args();
      ArrayLit* al = eval_array_lit(_env.envi(), args[0]);
      int nvars = al->v().size();
      double* coefs = gi.exprToArray(args[0]);
      GRBVar* vars = gi.exprToVarArray(args[1]);
      IntVal ires;
      FloatVal fres;

      if(args[2]->type().isint()) {
        ires = eval_int(_env.envi(), args[2]);
      } else if(args[2]->type().isfloat()) {
        fres = eval_float(_env.envi(), args[2]);
      } else {
        throw -1;
      }

      double lb, ub;
      lb = -GRB_INFINITY;
      ub = GRB_INFINITY;
      switch (lt) {
        case LQ:
          if(args[2]->type().isint()) {
            ub = ires.toInt();
          } else {
            ub = fres;
          }
          break;
        case GQ:
          if(args[2]->type().isint()) {
            lb = ires.toInt();
          } else {
            lb = fres;
          }
          break;
        case EQ:
          if(args[2]->type().isint()) {
            lb = ires.toInt();
          } else {
            lb = fres;
          }
          ub = lb;
          break;
      }

      GRBLinExpr con;
      con.addTerms(coefs, vars, nvars);

      try {
        grb_model->addRange(con, lb, ub);
      } catch (GRBException e) {
        std::cerr << "Error code = " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
      } catch (...) {
        std::cerr << "Exception during optimization" << std::endl;
      }
    }

    void p_int_lin_le(SolverInstanceBase& si, const Call* call) {
      p_lin(si, call, LQ);
    }
    void p_int_lin_eq(SolverInstanceBase& si, const Call* call) {
      p_lin(si, call, EQ);
    }
    void p_float_lin_le(SolverInstanceBase& si, const Call* call) {
      p_lin(si, call, LQ);
    }
    void p_float_lin_eq(SolverInstanceBase& si, const Call* call) {
      p_lin(si, call, EQ);
    }

    void p_eq(SolverInstanceBase& si, const Call* call) {
      GurobiSolverInstance& gi = (GurobiSolverInstance&) si;
      GRBModel* grb_model = gi.getGRBModel();
      ASTExprVec<Expression> args = call->args();
      GRBVar* vars = new GRBVar[2];
      vars[0] = gi.exprToVar(args[0]);
      vars[1] = gi.exprToVar(args[1]);
      double coefs[2] = {-1.0, 1.0};
      GRBLinExpr con;
      con.addTerms(coefs, vars, 2);

      std::stringstream ss;
      ss << "p_eq" << grb_model->get(GRB_IntAttr_NumConstrs);
      try{
        grb_model->addConstr(con, GRB_EQUAL, 0.0, ss.str());
      } catch (GRBException e) {
        std::cerr << "Error code = " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
      } catch (...) {
        std::cerr << "Exception during optimization" << std::endl;
      }
    }
    void p_le(SolverInstanceBase& si, const Call* call) {
      GurobiSolverInstance& gi = (GurobiSolverInstance&) si;
      GRBModel* grb_model = gi.getGRBModel();
      ASTExprVec<Expression> args = call->args();
      GRBVar* vars = new GRBVar[2];
      vars[0] = gi.exprToVar(args[0]);
      vars[1] = gi.exprToVar(args[1]);
      double coefs[2] = {1.0, -1.0};

      std::stringstream ss;
      ss << "p_le" << grb_model->get(GRB_IntAttr_NumConstrs);
      GRBLinExpr con;
      con.addTerms(coefs, vars, 2);
      try {
        grb_model->addRange(con, -GRB_INFINITY, 0.0, ss.str());
      } catch (GRBException e) {
        std::cerr << "Error code = " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
      } catch (...) {
        std::cerr << "Exception during optimization" << std::endl;
      }

    }
    void p_plus(SolverInstanceBase& si, const Call* call) {
      GurobiSolverInstance& gi = (GurobiSolverInstance&) si;
      GRBModel* grb_model = gi.getGRBModel();
      ASTExprVec<Expression> args = call->args();
      GRBVar* vars = new GRBVar[3];
      vars[0] = gi.exprToVar(args[0]);
      vars[1] = gi.exprToVar(args[1]);
      vars[2] = gi.exprToVar(args[2]);
      double coefs[3] = {1.0, 1.0, -1.0};

      std::stringstream ss;
      ss << "p_plus" << grb_model->get(GRB_IntAttr_NumConstrs);
      GRBLinExpr con;
      con.addTerms(coefs, vars, 3);
      try {
        grb_model->addRange(con, 0.0, 0.0, ss.str());
      } catch (GRBException e) {
        std::cerr << "Error code = " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
      } catch (...) {
        std::cerr << "Exception during optimization" << std::endl;
      }
    }
  }

  GurobiSolverInstance::GurobiSolverInstance(Env& env, const Options& options)
    : SolverInstanceImpl<GurobiSolver>(env,options), _grb_model(NULL) {
      try {
        _grb_env = new GRBEnv();
        _grb_model = new GRBModel(*_grb_env);
      } catch (GRBException e) {
        std::cerr << "Error code = " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
        std::exit(1);
      }
      fVerbose      = options.getBoolParam  ("verbose",          false);
      all_solutions = options.getBoolParam  ("all_solutions",    false);
      nThreads      = options.getIntParam   ("parallel_threads",     1);
      nTimeout      = options.getFloatParam ("timelimit",          0.0);
      sExportModel  = options.getStringParam("export_model",        "");

      registerConstraints();
    }

  void GurobiSolverInstance::registerConstraints() {
    _constraintRegistry.add(ASTString("int2float"), GurobiConstraints::p_eq);
    _constraintRegistry.add(ASTString("bool_eq"), GurobiConstraints::p_eq);
    _constraintRegistry.add(ASTString("int_eq"), GurobiConstraints::p_eq);
    _constraintRegistry.add(ASTString("int_le"), GurobiConstraints::p_le);
    _constraintRegistry.add(ASTString("int_lin_eq"), GurobiConstraints::p_int_lin_eq);
    _constraintRegistry.add(ASTString("int_lin_le"), GurobiConstraints::p_int_lin_le);
    _constraintRegistry.add(ASTString("int_plus"), GurobiConstraints::p_plus);
    _constraintRegistry.add(ASTString("bool2int"), GurobiConstraints::p_eq);
    _constraintRegistry.add(ASTString("float_eq"), GurobiConstraints::p_eq);
    _constraintRegistry.add(ASTString("float_le"), GurobiConstraints::p_le);
    _constraintRegistry.add(ASTString("float_lin_eq"), GurobiConstraints::p_float_lin_eq);
    _constraintRegistry.add(ASTString("float_lin_le"), GurobiConstraints::p_float_lin_le);
    _constraintRegistry.add(ASTString("float_plus"), GurobiConstraints::p_plus);
  }

  GurobiSolverInstance::~GurobiSolverInstance(void) {
    delete _grb_model;
    delete _grb_env;
  }

  SolverInstanceBase::Status GurobiSolverInstance::next(void) {
    return SolverInstance::FAILURE;
  }


  class SolutionCallback: public GRBCallback
  {
    public:
      GurobiSolverInstance& gsi;
      double lastiter;
      double lastnode;
      int numvars;
      GRBVar* vars;
      SolutionCallback(GurobiSolverInstance& grbsi, int xnumvars, GRBVar* xvars) : gsi(grbsi) {
        lastiter = lastnode = -GRB_INFINITY;
        numvars = xnumvars;
        vars = xvars;
      }
      double getValue(GRBVar v) {
        return getSolution(v);
      }
    protected:
      void callback () {
        try {
          if (where == GRB_CB_MIPSOL) {

            gsi.printSolution(this);
          }
        } catch (GRBException e) {
          std::cout << "Error number: " << e.getErrorCode() << std::endl;
          std::cout << e.getMessage() << std::endl;
        } catch (...) {
          std::cout << "Error during callback" << std::endl;
        }
      }
  };

  void GurobiSolverInstance::printSolution(SolutionCallback* cb) {
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

  SolverInstanceBase::Status GurobiSolverInstance::solve(void) {
    SolveI* solveItem = _env.flat()->solveItem();
    try{
      if (_env.flat()->solveItem()->st() != SolveI::SolveType::ST_SAT) {
        short sense = -1;
        if (solveItem->st() == SolveI::SolveType::ST_MIN)
          sense = 1;
        _grb_model->set(GRB_IntAttr_ModelSense, sense);
      }

      _grb_model->getEnv().set(GRB_IntParam_OutputFlag, fVerbose);

      if (nThreads>0)
        _grb_model->getEnv().set(GRB_IntParam_Threads, nThreads);

      if (nTimeout>0)
        _grb_model->getEnv().set(GRB_DoubleParam_TimeLimit, nTimeout);

      int numvars = _grb_model->get(GRB_IntAttr_NumVars);
      GRBVar* vars = _grb_model->getVars();
      SolutionCallback cb = SolutionCallback(*this, numvars, vars);

      if (all_solutions) {
        _grb_model->setCallback(&cb);
      }

      if (!sExportModel.empty()) {
        _grb_model->write(sExportModel);
      }

      _grb_model->optimize();
      //std::cout << "  _grb_model::optimize() exited." << std::endl;
    } catch (GRBException e) {
      std::cerr << "Error code = " << e.getErrorCode() << std::endl;
      std::cerr << e.getMessage() << std::endl;
    } catch (...) {
      std::cerr << "Exception during optimization" << std::endl;
    }
    Status s;
    int status = _grb_model->get(GRB_IntAttr_Status);
    int nSolutions = _grb_model->get(GRB_IntAttr_SolCount);
    switch(status) {
      case GRB_OPTIMAL:
        s = SolverInstance::SUCCESS;
        //std::cout << "\n   ----------------------  MIP__OPTIMAL  ----------------------------------" << std::endl;
        assignSolutionToOutput();
        break;
        s = SolverInstance::SUCCESS;
        //std::cout << "\n   ---------------------  MIP__FEASIBLE  ----------------------------------" << std::endl;
        assignSolutionToOutput();
        break;
      case GRB_INFEASIBLE:
        s = SolverInstance::FAILURE;
        //std::cout << "\n   ---------------------  MIP__INFEASIBLE  ----------------------------------" << std::endl;
        break;
      case GRB_UNBOUNDED:
      case GRB_INF_OR_UNBD:
        s = SolverInstance::FAILURE;
        //std::cout << "\n   ---------------------   MIP__ERROR   ----------------------------------" << std::endl;
        break;
      case GRB_TIME_LIMIT:
      case GRB_SUBOPTIMAL:
        s = SolverInstance::FAILURE;
        if(nSolutions > 0)
          assignSolutionToOutput();
        break;
      default:
        s = SolverInstance::FAILURE;
        //std::cout << "\n   ---------------------   MIP__UNKNOWN_STATUS   ----------------------------------" << std::endl;
    }
    //if (IloAlgorithm::Status::Optimal==ss || IloAlgorithm::Status::Feasible==ss) {
    //  /// PRINT THE MAIN RESULTS FIRST:
    //  std::cout << "% MIP_Objective_ : " << _ilocplex->getObjValue() << std::endl;
    //  std::cout << "% MIP_AbsGap__   : "
    //    << std::fabs(_ilocplex->getBestObjValue()-_ilocplex->getObjValue()) << std::endl;
    //  std::cout << "% MIP_RelGap__   : " << _ilocplex->getMIPRelativeGap() << std::endl;
    //} else {
    //  std::cout << "\n   ------------  NO FEASIBLE SOLUTION FOUND  ----------------------------" << std::endl;
    //}
    //std::cout   << "% MIP_BestBound_ : " << _ilocplex->getBestObjValue() << std::endl;
    //std::cout   << "% Real/CPU Time_ : " << _ilocplex->getTime() << " sec\n" << std::endl;
    //std::cout << "------------------------------------------------------------------------\n"<< std::endl;
    return s;

  }

  void GurobiSolverInstance::processFlatZinc(void) {
    SolveI* solveItem = _env.flat()->solveItem();
    VarDecl* objVd = NULL;

    if (solveItem->st() != SolveI::SolveType::ST_SAT) {
      if(Id* id = solveItem->e()->dyn_cast<Id>()) {
        objVd = id->decl();
      } else {
        std::cerr << "Objective must be Id: " << solveItem->e() << std::endl;
        throw -1;
      }
    }

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
        char type;
        if (ti->type().isvarint()) {
          type = GRB_INTEGER;
        } else if (ti->type().isvarbool()) {
          type = GRB_INTEGER;
        } else if (ti->type().isvarfloat()) {
          type = GRB_CONTINUOUS;
        } else {
          std::stringstream ssm;
          ssm << "This type of var is not handled by Gurobi: " << *it << std::endl;
          throw InternalError(ssm.str());
        }
        double lb, ub;
        if (ti->domain()) {
          if (type == GRB_CONTINUOUS) {
            FloatBounds fb = compute_float_bounds(_env.envi(), it->e()->id());
            assert(fb.valid);
            lb = fb.l;
            ub = fb.u;
          } else if (type == GRB_INTEGER) {
            IntBounds ib = compute_int_bounds(_env.envi(), it->e()->id());
            assert(ib.valid);
            lb = ib.l.toInt();
            ub = ib.u.toInt();
          } else {
            lb = -GRB_INFINITY;
            ub = GRB_INFINITY;
          }
        } else {
          lb = -GRB_INFINITY;
          ub = GRB_INFINITY;
        }

        GRBVar res;
        Id* id = it->e()->id();
        id = id->decl()->id();
        if (it->e()->e()) {
          res = exprToVar(it->e()->e());
        } else {
          res = _grb_model->addVar(lb, ub, vd==objVd ? 1.0 : 0.0, type, id->str().c_str());
        }

        _variableMap.insert(id, res);
      }

    }
    _grb_model->update();

    for (ConstraintIterator it = _env.flat()->begin_constraints(); it != _env.flat()->end_constraints(); ++it) {
      if (Call* c = it->e()->dyn_cast<Call>()) {
        _constraintRegistry.post(c);
      }
    }

  }

  void GurobiSolverInstance::resetSolver(void) {}

  Expression* GurobiSolverInstance::getSolutionValue(Id* id, SolutionCallback* cb) {
    id = id->decl()->id();
    GRBVar var = exprToVar(id);
    double val;
    if(cb!=NULL) {
      val = cb->getValue(var);
    } else {
      val = var.get(GRB_DoubleAttr_X);
    }

    switch (id->type().bt()) {
      case Type::BT_INT: return new IntLit(Location(), round_to_longlong(val));
      case Type::BT_FLOAT: return new FloatLit(Location(), val);
      case Type::BT_BOOL: return new BoolLit(Location(), round_to_longlong(val));
      default: return NULL;
    }
  }

  GRBModel* GurobiSolverInstance::getGRBModel(void) { return _grb_model; }

  void GurobiSolverInstance::assignSolutionToOutput(SolutionCallback* cb) {

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
  
  SolverInstance::Status GurobiSolverInstance::best(VarDecl* objective, bool minimize, bool print) {
    return Status::FAILURE;  // TODO for MiniSearch!
  }
  
  SolverInstanceBase* GurobiSolverInstance::copy(CopyMap& cmap) {
    return NULL; // TODO for MiniSearch!
  }
  
  bool GurobiSolverInstance::updateIntBounds(VarDecl* vd, int lb, int ub) {
    return false; // TODO for MiniSearch!
  }
}
