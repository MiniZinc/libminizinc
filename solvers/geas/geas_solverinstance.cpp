/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/geas/geas_constraints.hh>
#include <minizinc/solvers/geas_solverinstance.hh>

namespace MiniZinc {
GeasSolverInstance::GeasSolverInstance(Env& env, std::ostream& log,
                                       SolverInstanceBase::Options* opt)
    : SolverInstanceImpl<GeasTypes>(env, log, opt), _flat(env.flat()) {
  registerConstraints();
}

void GeasSolverInstance::registerConstraint(const std::string& name, poster p) {
  _constraintRegistry.add("geas_" + name, p);
  _constraintRegistry.add(name, p);
}

void GeasSolverInstance::registerConstraints() {
  GCLock lock;

  /* Integer Comparison Constraints */
  registerConstraint("int_eq", GeasConstraints::p_int_eq);
  registerConstraint("int_ne", GeasConstraints::p_int_ne);
  registerConstraint("int_le", GeasConstraints::p_int_le);
  registerConstraint("int_lt", GeasConstraints::p_int_lt);
  registerConstraint("int_eq_imp", GeasConstraints::p_int_eq_imp);
  registerConstraint("int_ne_imp", GeasConstraints::p_int_ne_imp);
  registerConstraint("int_le_imp", GeasConstraints::p_int_le_imp);
  registerConstraint("int_lt_imp", GeasConstraints::p_int_lt_imp);
  registerConstraint("int_eq_reif", GeasConstraints::p_int_eq_reif);
  registerConstraint("int_ne_reif", GeasConstraints::p_int_ne_reif);
  registerConstraint("int_le_reif", GeasConstraints::p_int_le_reif);
  registerConstraint("int_lt_reif", GeasConstraints::p_int_lt_reif);

  /* Integer Arithmetic Constraints */
  registerConstraint("int_abs", GeasConstraints::p_int_abs);
  registerConstraint("int_times", GeasConstraints::p_int_times);
  registerConstraint("int_div", GeasConstraints::p_int_div);
  //    registerConstraint("int_mod", GeasConstraints::p_int_mod);
  registerConstraint("int_min", GeasConstraints::p_int_min);
  registerConstraint("int_max", GeasConstraints::p_int_max);

  /* Integer Linear Constraints */
  registerConstraint("int_lin_eq", GeasConstraints::p_int_lin_eq);
  registerConstraint("int_lin_ne", GeasConstraints::p_int_lin_ne);
  registerConstraint("int_lin_le", GeasConstraints::p_int_lin_le);
  registerConstraint("int_lin_eq_imp", GeasConstraints::p_int_lin_eq_imp);
  registerConstraint("int_lin_ne_imp", GeasConstraints::p_int_lin_ne_imp);
  registerConstraint("int_lin_le_imp", GeasConstraints::p_int_lin_le_imp);
  registerConstraint("int_lin_eq_reif", GeasConstraints::p_int_lin_eq_reif);
  registerConstraint("int_lin_ne_reif", GeasConstraints::p_int_lin_ne_reif);
  registerConstraint("int_lin_le_reif", GeasConstraints::p_int_lin_le_reif);

  /* Boolean Comparison Constraints */
  registerConstraint("bool_eq", GeasConstraints::p_bool_eq);
  registerConstraint("bool_ne", GeasConstraints::p_bool_ne);
  registerConstraint("bool_le", GeasConstraints::p_bool_le);
  registerConstraint("bool_lt", GeasConstraints::p_bool_lt);
  registerConstraint("bool_eq_imp", GeasConstraints::p_bool_eq_imp);
  registerConstraint("bool_ne_imp", GeasConstraints::p_bool_ne_imp);
  registerConstraint("bool_le_imp", GeasConstraints::p_bool_le_imp);
  registerConstraint("bool_lt_imp", GeasConstraints::p_bool_lt_imp);
  registerConstraint("bool_eq_reif", GeasConstraints::p_bool_eq_reif);
  registerConstraint("bool_ne_reif", GeasConstraints::p_bool_ne_reif);
  registerConstraint("bool_le_reif", GeasConstraints::p_bool_le_reif);
  registerConstraint("bool_lt_reif", GeasConstraints::p_bool_lt_reif);

  /* Boolean Arithmetic Constraints */
  registerConstraint("bool_or", GeasConstraints::p_bool_or);
  registerConstraint("bool_and", GeasConstraints::p_bool_and);
  registerConstraint("bool_xor", GeasConstraints::p_bool_xor);
  registerConstraint("bool_not", GeasConstraints::p_bool_not);
  registerConstraint("bool_or_imp", GeasConstraints::p_bool_or_imp);
  registerConstraint("bool_and_imp", GeasConstraints::p_bool_and_imp);
  registerConstraint("bool_xor_imp", GeasConstraints::p_bool_xor_imp);

  registerConstraint("bool_clause", GeasConstraints::p_bool_clause);
  registerConstraint("array_bool_or", GeasConstraints::p_array_bool_or);
  registerConstraint("array_bool_and", GeasConstraints::p_array_bool_and);
  registerConstraint("bool_clause_imp", GeasConstraints::p_bool_clause_imp);
  registerConstraint("array_bool_or_imp", GeasConstraints::p_array_bool_or_imp);
  registerConstraint("array_bool_and_imp", GeasConstraints::p_array_bool_and_imp);
  registerConstraint("bool_clause_reif", GeasConstraints::p_bool_clause_reif);

  /* Boolean Linear Constraints */
  registerConstraint("bool_lin_eq", GeasConstraints::p_bool_lin_eq);
  registerConstraint("bool_lin_ne", GeasConstraints::p_bool_lin_ne);
  registerConstraint("bool_lin_le", GeasConstraints::p_bool_lin_le);
  registerConstraint("bool_lin_eq_imp", GeasConstraints::p_bool_lin_eq_imp);
  registerConstraint("bool_lin_ne_imp", GeasConstraints::p_bool_lin_ne_imp);
  registerConstraint("bool_lin_le_imp", GeasConstraints::p_bool_lin_le_imp);
  registerConstraint("bool_lin_eq_reif", GeasConstraints::p_bool_lin_eq_reif);
  registerConstraint("bool_lin_ne_reif", GeasConstraints::p_bool_lin_ne_reif);
  registerConstraint("bool_lin_le_reif", GeasConstraints::p_bool_lin_le_reif);

  /* Coercion Constraints */
  registerConstraint("bool2int", GeasConstraints::p_bool2int);

  /* Element Constraints */
  registerConstraint("array_int_element", GeasConstraints::p_array_int_element);
  registerConstraint("array_bool_element", GeasConstraints::p_array_bool_element);
  registerConstraint("array_var_int_element", GeasConstraints::p_array_var_int_element);
  registerConstraint("array_var_bool_element", GeasConstraints::p_array_var_bool_element);

  /* Global Constraints */
  registerConstraint("all_different_int", GeasConstraints::p_all_different);
  registerConstraint("alldifferent_except_0", GeasConstraints::p_all_different_except_0);
  registerConstraint("at_most", GeasConstraints::p_at_most);
  registerConstraint("at_most1", GeasConstraints::p_at_most1);
  registerConstraint("cumulative", GeasConstraints::p_cumulative);
  registerConstraint("cumulative_var", GeasConstraints::p_cumulative);
  registerConstraint("disjunctive", GeasConstraints::p_disjunctive);
  registerConstraint("disjunctive_var", GeasConstraints::p_disjunctive);
  registerConstraint("global_cardinality", GeasConstraints::p_global_cardinality);
  registerConstraint("table_int", GeasConstraints::p_table_int);

  /**** TODO: NOT YET SUPPORTED: ****/
  /* Boolean Arithmetic Constraints */
  //    registerConstraint("array_bool_xor", GeasConstraints::p_array_bool_xor);
  //    registerConstraint("array_bool_xor_imp", GeasConstraints::p_array_bool_xor_imp);

  /* Floating Point Comparison Constraints */
  //    registerConstraint("float_eq", GeasConstraints::p_float_eq);
  //    registerConstraint("float_le", GeasConstraints::p_float_le);
  //    registerConstraint("float_lt", GeasConstraints::p_float_lt);
  //    registerConstraint("float_ne", GeasConstraints::p_float_ne);
  //    registerConstraint("float_eq_reif", GeasConstraints::p_float_eq_reif);
  //    registerConstraint("float_le_reif", GeasConstraints::p_float_le_reif);
  //    registerConstraint("float_lt_reif", GeasConstraints::p_float_lt_reif);

  /* Floating Point Arithmetic Constraints */
  //    registerConstraint("float_abs", GeasConstraints::p_float_abs);
  //    registerConstraint("float_sqrt", GeasConstraints::p_float_sqrt);
  //    registerConstraint("float_times", GeasConstraints::p_float_times);
  //    registerConstraint("float_div", GeasConstraints::p_float_div);
  //    registerConstraint("float_plus", GeasConstraints::p_float_plus);
  //    registerConstraint("float_max", GeasConstraints::p_float_max);
  //    registerConstraint("float_min", GeasConstraints::p_float_min);
  //    registerConstraint("float_acos", GeasConstraints::p_float_acos);
  //    registerConstraint("float_asin", GeasConstraints::p_float_asin);
  //    registerConstraint("float_atan", GeasConstraints::p_float_atan);
  //    registerConstraint("float_cos", GeasConstraints::p_float_cos);
  //    registerConstraint("float_exp", GeasConstraints::p_float_exp);
  //    registerConstraint("float_ln", GeasConstraints::p_float_ln);
  //    registerConstraint("float_log10", GeasConstraints::p_float_log10);
  //    registerConstraint("float_log2", GeasConstraints::p_float_log2);
  //    registerConstraint("float_sin", GeasConstraints::p_float_sin);
  //    registerConstraint("float_tan", GeasConstraints::p_float_tan);

  /* Floating Linear Constraints */
  //    registerConstraint("float_lin_eq", GeasConstraints::p_float_lin_eq);
  //    registerConstraint("float_lin_eq_reif", GeasConstraints::p_float_lin_eq_reif);
  //    registerConstraint("float_lin_le", GeasConstraints::p_float_lin_le);
  //    registerConstraint("float_lin_le_reif", GeasConstraints::p_float_lin_le_reif);

  /* Coercion Constraints */
  //    registerConstraint("int2float", GeasConstraints::p_int2float);
}

void GeasSolverInstance::processFlatZinc() {
  auto _opt = static_cast<GeasOptions&>(*_options);
  // Create variables
  zero = _solver.new_intvar(0, 0);
  for (auto it = _flat->vardecls().begin(); it != _flat->vardecls().end(); ++it) {
    if (!it->removed() && it->e()->type().isvar() && it->e()->type().dim() == 0) {
      VarDecl* vd = it->e();

      if (vd->type().isbool()) {
        if (vd->e() == nullptr) {
          Expression* domain = vd->ti()->domain();
          long long int lb;
          long long int ub;
          if (domain != nullptr) {
            IntBounds ib = compute_int_bounds(_env.envi(), domain);
            lb = ib.l.toInt();
            ub = ib.u.toInt();
          } else {
            lb = 0;
            ub = 1;
          }
          if (lb == ub) {
            geas::patom_t val = (lb == 0) ? geas::at_False : geas::at_True;
            _variableMap.insert(vd->id(), GeasVariable(val));
          } else {
            auto var = _solver.new_boolvar();
            _variableMap.insert(vd->id(), GeasVariable(var));
          }
        } else {
          Expression* init = vd->e();
          if (init->isa<Id>() || init->isa<ArrayAccess>()) {
            GeasVariable& var = resolveVar(init);
            assert(var.isBool());
            _variableMap.insert(vd->id(), GeasVariable(var.boolVar()));
          } else {
            auto b = init->cast<BoolLit>()->v();
            geas::patom_t val = b ? geas::at_True : geas::at_False;
            _variableMap.insert(vd->id(), GeasVariable(val));
          }
        }
      } else if (vd->type().isfloat()) {
        if (vd->e() == nullptr) {
          Expression* domain = vd->ti()->domain();
          double lb;
          double ub;
          if (domain != nullptr) {
            FloatBounds fb = compute_float_bounds(_env.envi(), vd->id());
            lb = fb.l.toDouble();
            ub = fb.u.toDouble();
          } else {
            std::ostringstream ss;
            ss << "GeasSolverInstance::processFlatZinc: Error: Unbounded variable: "
               << vd->id()->str();
            throw Error(ss.str());
          }
          // TODO: Error correction from double to float??
          auto var = _solver.new_floatvar(static_cast<geas::fp::val_t>(lb),
                                          static_cast<geas::fp::val_t>(ub));
          _variableMap.insert(vd->id(), GeasVariable(var));
        } else {
          Expression* init = vd->e();
          if (init->isa<Id>() || init->isa<ArrayAccess>()) {
            GeasVariable& var = resolveVar(init);
            assert(var.isFloat());
            _variableMap.insert(vd->id(), GeasVariable(var.floatVar()));
          } else {
            double fl = init->cast<FloatLit>()->v().toDouble();
            auto var = _solver.new_floatvar(static_cast<geas::fp::val_t>(fl),
                                            static_cast<geas::fp::val_t>(fl));
            _variableMap.insert(vd->id(), GeasVariable(var));
          }
        }
      } else if (vd->type().isint()) {
        if (vd->e() == nullptr) {
          Expression* domain = vd->ti()->domain();
          if (domain != nullptr) {
            IntSetVal* isv = eval_intset(env().envi(), domain);
            auto var = _solver.new_intvar(static_cast<geas::intvar::val_t>(isv->min().toInt()),
                                          static_cast<geas::intvar::val_t>(isv->max().toInt()));
            if (isv->size() > 1) {
              vec<int> vals(static_cast<int>(isv->card().toInt()));
              int i = 0;
              for (int j = 0; j < isv->size(); ++j) {
                for (auto k = isv->min(j).toInt(); k <= isv->max(j).toInt(); ++k) {
                  vals[i++] = static_cast<int>(k);
                }
              }
              assert(i == isv->card().toInt());
              auto res = geas::make_sparse(var, vals);
              assert(res);
            }
            _variableMap.insert(vd->id(), GeasVariable(var));
          } else {
            std::ostringstream ss;
            ss << "GeasSolverInstance::processFlatZinc: Error: Unbounded variable: "
               << vd->id()->str();
            throw Error(ss.str());
          }
        } else {
          Expression* init = vd->e();
          if (init->isa<Id>() || init->isa<ArrayAccess>()) {
            GeasVariable& var = resolveVar(init);
            assert(var.isInt());
            _variableMap.insert(vd->id(), GeasVariable(var.intVar()));
          } else {
            auto il = init->cast<IntLit>()->v().toInt();
            auto var = _solver.new_intvar(static_cast<geas::intvar::val_t>(il),
                                          static_cast<geas::intvar::val_t>(il));
            _variableMap.insert(vd->id(), GeasVariable(var));
          }
        }
      } else {
        std::stringstream ssm;
        ssm << "Type " << *vd->ti() << " is currently not supported by Geas.";
        throw InternalError(ssm.str());
      }
    }
  }

  // Post constraints
  for (ConstraintIterator it = _flat->constraints().begin(); it != _flat->constraints().end();
       ++it) {
    if (!it->removed()) {
      if (auto* c = it->e()->dynamicCast<Call>()) {
        _constraintRegistry.post(c);
      }
    }
  }
  // Set objective
  SolveI* si = _flat->solveItem();
  if (si->e() != nullptr) {
    _objType = si->st();
    if (_objType == SolveI::ST_MIN) {
      _objVar = std::unique_ptr<GeasTypes::Variable>(new GeasTypes::Variable(resolveVar(si->e())));
    } else if (_objType == SolveI::ST_MAX) {
      _objType = SolveI::ST_MIN;
      _objVar = std::unique_ptr<GeasTypes::Variable>(new GeasTypes::Variable(-asIntVar(si->e())));
    }
  }
  if (!si->ann().isEmpty()) {
    std::vector<Expression*> flatAnn;
    flattenSearchAnnotations(si->ann(), flatAnn);

    for (auto& ann : flatAnn) {
      if (ann->isa<Call>()) {
        Call* call = ann->cast<Call>();
        if (call->id() == "warm_start") {
          auto* vars = eval_array_lit(env().envi(), call->arg(0));
          auto* vals = eval_array_lit(env().envi(), call->arg(1));
          assert(vars->size() == vals->size());
          vec<geas::patom_t> ws(static_cast<int>(vars->size()));

          if (vars->type().isIntArray()) {
            assert(vals->type().isIntArray());
            for (int i = 0; i < vars->size(); ++i) {
              geas::intvar var = asIntVar((*vars)[i]);
              int val = asInt((*vals)[i]);
              ws.push(var == val);
            }
          } else if (vars->type().isBoolArray()) {
            assert(vals->type().isBoolArray());
            for (int i = 0; i < vars->size(); ++i) {
              geas::patom_t var = asBoolVar((*vars)[i]);
              bool val = asBool((*vals)[i]);
              ws.push(val ? var : ~var);
            }
          } else {
            std::cerr << "WARNING Geas: ignoring warm start annotation of invalid type: " << *ann
                      << std::endl;
            continue;
          }
          _solver.data->branchers.push(geas::warmstart_brancher(ws));
          continue;
        }

        vec<geas::pid_t> pids;
        geas::VarChoice select = geas::Var_FirstFail;
        geas::ValChoice choice = geas::Val_Min;
        if (call->id() == "int_search") {
          vec<geas::intvar> iv = asIntVar(eval_array_lit(env().envi(), call->arg(0)));
          pids.growTo(iv.size());
          for (int i = 0; i < iv.size(); ++i) {
            pids[i] = iv[i].p;
          }
        } else if (call->id() == "bool_search") {
          vec<geas::patom_t> bv = asBoolVar(eval_array_lit(env().envi(), call->arg(0)));
          pids.growTo(bv.size());
          for (int i = 0; i < bv.size(); ++i) {
            pids[i] = bv[i].pid;
          }
        } else {
          std::cerr << "WARNING Geas: ignoring unknown search annotation: " << *ann << std::endl;
          continue;
        }
        ASTString select_str = call->arg(1)->cast<Id>()->str();
        if (select_str == "input_order") {
          select = geas::Var_InputOrder;
        } else if (select_str == "first_fail") {
          select = geas::Var_FirstFail;
        } else if (select_str == "largest") {
          select = geas::Var_Largest;
        } else if (select_str == "smallest") {
          select = geas::Var_Smallest;
        } else {
          std::cerr << "WARNING Geas: unknown variable selection '" << select_str
                    << "', using default value First Fail." << std::endl;
        }
        ASTString choice_str = call->arg(2)->cast<Id>()->str();
        if (choice_str == "indomain_max") {
          choice = geas::Val_Max;
        } else if (choice_str == "indomain_min") {
          choice = geas::Val_Min;
        } else if (choice_str == "indomain_split") {
          choice = geas::Val_Split;
        } else {
          std::cerr << "WARNING Geas: unknown value selection '" << choice_str
                    << "', using Indomain Min." << std::endl;
        }

        geas::brancher* b = geas::basic_brancher(select, choice, pids);
        if (_opt.freeSearch) {
          vec<geas::brancher*> brv({b, _solver.data->last_branch});
          _solver.data->branchers.push(geas::toggle_brancher(brv));
        } else {
          _solver.data->branchers.push(b);
        }
      }
    }
  }
}

bool GeasSolverInstance::addSolutionNoGood() {
  assert(!_varsWithOutput.empty());
  geas::model solution = _solver.get_model();
  vec<geas::clause_elt> clause;
  for (auto& var : _varsWithOutput) {
    if (Expression::dynamicCast<Call>(
            get_annotation(var->ann(), constants().ann.output_array.aststr())) != nullptr) {
      if (auto* al = var->e()->dynamicCast<ArrayLit>()) {
        for (int j = 0; j < al->size(); j++) {
          if (Id* id = (*al)[j]->dynamicCast<Id>()) {
            auto geas_var = resolveVar(id);
            if (geas_var.isBool()) {
              geas::patom_t bv = geas_var.boolVar();
              clause.push(solution.value(bv) ? ~bv : bv);
            } else if (geas_var.isFloat()) {
              geas::fp::fpvar fv = geas_var.floatVar();
              clause.push(fv < solution[fv]);
              clause.push(fv > solution[fv]);
            } else {
              geas::intvar iv = geas_var.intVar();
              clause.push(~(iv == solution[iv]));
            }
          }
        }
      }
    } else {
      auto geas_var = resolveVar(var);
      if (geas_var.isBool()) {
        geas::patom_t bv = geas_var.boolVar();
        clause.push(solution.value(bv) ? ~bv : bv);
      } else if (geas_var.isFloat()) {
        geas::fp::fpvar fv = geas_var.floatVar();
        clause.push(fv < solution[fv]);
        clause.push(fv > solution[fv]);
      } else {
        geas::intvar iv = geas_var.intVar();
        clause.push(iv != solution[iv]);
      }
    }
  }
  return geas::add_clause(*_solver.data, clause);
}

SolverInstanceBase::Status MiniZinc::GeasSolverInstance::solve() {
  SolverInstanceBase::Status status = SolverInstance::ERROR;
  auto _opt = static_cast<GeasOptions&>(*_options);
  auto remaining_time = [_opt] {
    if (_opt.time == std::chrono::milliseconds(0)) {
      return 0.0;
    }
    using geas_time = std::chrono::duration<double>;
    static auto timeout = std::chrono::high_resolution_clock::now() + _opt.time;
    return geas_time(timeout - std::chrono::high_resolution_clock::now()).count();
  };
  if (_objType == SolveI::ST_SAT) {
    int nr_solutions = 0;
    geas::solver::result res = geas::solver::UNKNOWN;
    while ((_opt.allSolutions || nr_solutions < _opt.nrSolutions) && remaining_time() >= 0.0) {
      res = _solver.solve({remaining_time(), _opt.conflicts - _solver.data->stats.conflicts});
      printSolution();
      if (res != geas::solver::SAT) {
        break;
      }
      nr_solutions++;
      _solver.restart();
      if (!addSolutionNoGood()) {
        res = geas::solver::UNSAT;
        break;
      }
    }
    switch (res) {
      case geas::solver::SAT:
        status = SolverInstance::SAT;
        break;
      case geas::solver::UNSAT:
        if (nr_solutions > 0) {
          status = SolverInstance::OPT;
        } else {
          status = SolverInstance::UNSAT;
        }
        break;
      case geas::solver::UNKNOWN:
        if (nr_solutions > 0) {
          status = SolverInstance::SAT;
        } else {
          status = SolverInstance::UNKNOWN;
        }
        break;
      default:
        status = SolverInstance::ERROR;
        break;
    }
  } else {
    assert(_objType == SolveI::ST_MIN);
    // TODO: Add float objectives
    assert(_objVar->isInt());
    geas::intvar obj = _objVar->intVar();
    geas::solver::result res;
    while (true) {
      res = _solver.solve({remaining_time(), _opt.conflicts - _solver.data->stats.conflicts});
      geas::intvar::val_t obj_val;
      if (res != geas::solver::SAT) {
        break;
      }
      status = SolverInstance::SAT;
      if (_opt.allSolutions) {
        printSolution();
      }
      obj_val = _solver.get_model()[obj];

      int step = 1;
      while (_opt.objProbeLimit > 0) {
        geas::intvar::val_t assumed_obj;
        assumed_obj = obj_val - step;
        assumed_obj = obj.lb(_solver.data) > assumed_obj ? obj.lb(_solver.data) : assumed_obj;
        if (!_solver.assume(obj == assumed_obj)) {
          _solver.retract();
          break;
        }
        res = _solver.solve({remaining_time(), _opt.objProbeLimit});
        _solver.retract();
        if (res != geas::solver::SAT) {
          break;
        }
        step *= 2;
        if (_opt.allSolutions) {
          printSolution();
        }
        obj_val = _solver.get_model()[obj];
      }
      _solver.post(obj < obj_val);
    }
    if (status == SolverInstance::ERROR) {
      switch (res) {
        case geas::solver::UNSAT:
          status = SolverInstance::UNSAT;
          break;
        case geas::solver::UNKNOWN:
          status = SolverInstance::UNKNOWN;
          break;
        default:
          assert(false);
          status = SolverInstance::ERROR;
          break;
      }
    } else {
      if (res == geas::solver::UNSAT) {
        status = SolverInstance::OPT;
      }
      if (!_opt.allSolutions) {
        printSolution();
      }
    }
  }
  if (_opt.statistics) {
    printStatistics();
  }
  return status;
}

Expression* GeasSolverInstance::getSolutionValue(Id* id) {
  id = id->decl()->id();
  if (id->type().isvar()) {
    GeasVariable& var = resolveVar(id->decl()->id());
    geas::model solution = _solver.get_model();
    switch (id->type().bt()) {
      case Type::BT_BOOL:
        assert(var.isBool());
        return constants().boollit(solution.value(var.boolVar()));
      case Type::BT_FLOAT:
        assert(var.isFloat());
        return FloatLit::a(solution[var.floatVar()]);
      case Type::BT_INT:
        assert(var.isInt());
        return IntLit::a(solution[var.intVar()]);
      default:
        return nullptr;
    }
  } else {
    return id->decl()->e();
  }
}

void GeasSolverInstance::resetSolver() { assert(false); }

GeasTypes::Variable& GeasSolverInstance::resolveVar(Expression* e) {
  if (auto* id = e->dynamicCast<Id>()) {
    return _variableMap.get(id->decl()->id());
  }
  if (auto* vd = e->dynamicCast<VarDecl>()) {
    return _variableMap.get(vd->id()->decl()->id());
  }
  if (auto* aa = e->dynamicCast<ArrayAccess>()) {
    auto* ad = aa->v()->cast<Id>()->decl();
    auto idx = aa->idx()[0]->cast<IntLit>()->v().toInt();
    auto* al = eval_array_lit(_env.envi(), ad->e());
    return _variableMap.get((*al)[idx]->cast<Id>());
  }
  std::stringstream ssm;
  ssm << "Expected Id, VarDecl or ArrayAccess instead of \"" << *e << "\"";
  throw InternalError(ssm.str());
}

vec<bool> GeasSolverInstance::asBool(ArrayLit* al) {
  vec<bool> vec(static_cast<int>(al->size()));
  for (int i = 0; i < al->size(); ++i) {
    vec[i] = asBool((*al)[i]);
  }
  return vec;
}

geas::patom_t GeasSolverInstance::asBoolVar(Expression* e) {
  if (e->type().isvar()) {
    GeasVariable& var = resolveVar(follow_id_to_decl(e));
    assert(var.isBool());
    return var.boolVar();
  }
  if (auto* bl = e->dynamicCast<BoolLit>()) {
    return bl->v() ? geas::at_True : geas::at_False;
  }
  std::stringstream ssm;
  ssm << "Expected bool or int literal instead of: " << *e;
  throw InternalError(ssm.str());
}

vec<geas::patom_t> GeasSolverInstance::asBoolVar(ArrayLit* al) {
  vec<geas::patom_t> vec(static_cast<int>(al->size()));
  for (int i = 0; i < al->size(); ++i) {
    vec[i] = this->asBoolVar((*al)[i]);
  }
  return vec;
}

vec<int> GeasSolverInstance::asInt(ArrayLit* al) {
  vec<int> vec(static_cast<int>(al->size()));
  for (int i = 0; i < al->size(); ++i) {
    vec[i] = this->asInt((*al)[i]);
  }
  return vec;
}

geas::intvar GeasSolverInstance::asIntVar(Expression* e) {
  if (e->type().isvar()) {
    GeasVariable& var = resolveVar(follow_id_to_decl(e));
    assert(var.isInt());
    return var.intVar();
  }
  IntVal i;
  if (auto* il = e->dynamicCast<IntLit>()) {
    i = il->v().toInt();
  } else if (auto* bl = e->dynamicCast<BoolLit>()) {
    i = static_cast<long long>(bl->v());
  } else {
    std::stringstream ssm;
    ssm << "Expected bool or int literal instead of: " << *e;
    throw InternalError(ssm.str());
  }
  if (i == 0) {
    return zero;
  }
  return _solver.new_intvar(static_cast<geas::intvar::val_t>(i.toInt()),
                            static_cast<geas::intvar::val_t>(i.toInt()));
}

vec<geas::intvar> GeasSolverInstance::asIntVar(ArrayLit* al) {
  vec<geas::intvar> vec(static_cast<int>(al->size()));
  for (int i = 0; i < al->size(); ++i) {
    vec[i] = this->asIntVar((*al)[i]);
  }
  return vec;
}

void GeasSolverInstance::printStatistics() {
  auto& st = _solver.data->stats;
  auto& out = getSolns2Out()->getOutput();

  out << "%%%mzn-stat: failures=" << st.conflicts << std::endl;  // TODO: Statistic name
  out << "%%%mzn-stat: solveTime=" << st.time << std::endl;
  out << "%%%mzn-stat: solutions=" << st.solutions << std::endl;
  out << "%%%mzn-stat: restarts=" << st.restarts << std::endl;
  out << "%%%mzn-stat: nogoods=" << st.num_learnts << std::endl;             // TODO: Statistic name
  out << "%%%mzn-stat: learntLiterals=" << st.num_learnt_lits << std::endl;  // TODO: Statistic name
}

GeasSolverFactory::GeasSolverFactory() {
  SolverConfig sc("org.minizinc.geas", getVersion(nullptr));
  sc.name("Geas");
  sc.mznlib("-Ggeas");
  sc.mznlibVersion(1);
  sc.supportsMzn(false);
  sc.description(getDescription(nullptr));
  sc.tags({
      "api",
      "cp",
      "float",
      "int",
      "lcg",
  });
  sc.stdFlags({"-a", "-f", "-n", "-s", "-t"});
  sc.extraFlags({
      SolverConfig::ExtraFlag("--conflicts",
                              "Limit the maximum number of conflicts to be used during solving.",
                              "int", "0"),
      SolverConfig::ExtraFlag(
          "--obj-probe",
          "Number of conflicts to use to probe for better solutions after a new solution is found.",
          "int", "0"),
  });
  SolverConfigs::registerBuiltinSolver(sc);
};

SolverInstanceBase::Options* GeasSolverFactory::createOptions() { return new GeasOptions; }

SolverInstanceBase* GeasSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                  SolverInstanceBase::Options* opt) {
  return new GeasSolverInstance(env, log, opt);
}

bool GeasSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                      std::vector<std::string>& argv) {
  auto* _opt = static_cast<GeasOptions*>(opt);
  if (argv[i] == "-a" || argv[i] == "--all-solutions") {
    _opt->allSolutions = true;
  } else if (argv[i] == "--conflicts") {
    if (++i == argv.size()) {
      return false;
    }
    int nodes = atoi(argv[i].c_str());
    if (nodes >= 0) {
      _opt->conflicts = nodes;
    }
  } else if (argv[i] == "-f") {
    _opt->freeSearch = true;
  } else if (argv[i] == "-n") {
    if (++i == argv.size()) {
      return false;
    }
    int n = atoi(argv[i].c_str());
    if (n >= 0) {
      _opt->nrSolutions = n;
    }
  } else if (argv[i] == "--obj-probe") {
    if (++i == argv.size()) {
      return false;
    }
    int limit = atoi(argv[i].c_str());
    if (limit >= 0) {
      _opt->objProbeLimit = limit;
    }
  } else if (argv[i] == "--solver-statistics" || argv[i] == "-s") {
    _opt->statistics = true;
  } else if (argv[i] == "--solver-time-limit" || argv[i] == "-t") {
    if (++i == argv.size()) {
      return false;
    }
    int time = atoi(argv[i].c_str());
    if (time >= 0) {
      _opt->time = std::chrono::milliseconds(time);
    }
  } else {
    return false;
  }
  return true;
}

void GeasSolverFactory::printHelp(std::ostream& os) {
  os << "Geas solver plugin options:" << std::endl
     << "  --conflicts <int>" << std::endl
     << "    Limit the maximum number of conflicts to be used during solving." << std::endl
     << "  --obj-probe <int>" << std::endl
     << "    Number of conflicts to use to probe for better solutions after a new solution is "
        "found."
     << std::endl
     << std::endl;
}
}  // namespace MiniZinc
