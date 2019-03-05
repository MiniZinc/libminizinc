/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/geas_solverinstance.hh>
#include <minizinc/solvers/geas/geas_constraints.hh>

namespace MiniZinc{
  GeasSolverInstance::GeasSolverInstance(Env &env, std::ostream &log, SolverInstanceBase::Options *opt)
      : SolverInstanceImpl<GeasTypes>(env, log, opt), _flat(env.flat()) {
    registerConstraints();
  }

  void GeasSolverInstance::registerConstraint(std::string name, poster p) {
    _constraintRegistry.add("geas_" + name, p);
    _constraintRegistry.add(name, p);
  }

  void GeasSolverInstance::registerConstraints() {
    GCLock lock;

    /* arithmetic constraints */
    registerConstraint("int_abs", GeasConstraints::p_int_abs);
    registerConstraint("int_times", GeasConstraints::p_int_times);
    registerConstraint("int_div", GeasConstraints::p_int_div);
//    registerConstraint("int_mod", GeasConstraints::p_int_mod);
    registerConstraint("int_min", GeasConstraints::p_int_min);
    registerConstraint("int_max", GeasConstraints::p_int_max);

    /* Linear Constraints */
    registerConstraint("int_lin_eq", GeasConstraints::p_int_lin_eq);
    registerConstraint("int_lin_ne", GeasConstraints::p_int_lin_ne);
    registerConstraint("int_lin_le", GeasConstraints::p_int_lin_le);
    registerConstraint("int_lin_eq_imp", GeasConstraints::p_int_lin_eq_imp);
    registerConstraint("int_lin_ne_imp", GeasConstraints::p_int_lin_ne_imp);
    registerConstraint("int_lin_le_imp", GeasConstraints::p_int_lin_le_imp);
    registerConstraint("int_lin_eq_reif", GeasConstraints::p_int_lin_eq_reif);
    registerConstraint("int_lin_ne_reif", GeasConstraints::p_int_lin_ne_reif);
    registerConstraint("int_lin_le_reif", GeasConstraints::p_int_lin_le_reif);

//    registerConstraint("bool_eq", GeasConstraints::p_bool_eq);
//    registerConstraint("bool_eq_reif", GeasConstraints::p_bool_eq_reif);
//    registerConstraint("bool_eq_imp", GeasConstraints::p_bool_eq_imp);
//    registerConstraint("bool_ne", GeasConstraints::p_bool_ne);
//    registerConstraint("bool_ne_reif", GeasConstraints::p_bool_ne_reif);
//    registerConstraint("bool_ne_imp", GeasConstraints::p_bool_ne_imp);
//    registerConstraint("bool_ge", GeasConstraints::p_bool_ge);
//    registerConstraint("bool_ge_reif", GeasConstraints::p_bool_ge_reif);
//    registerConstraint("bool_ge_imp", GeasConstraints::p_bool_ge_imp);
//    registerConstraint("bool_le", GeasConstraints::p_bool_le);
//    registerConstraint("bool_le_reif", GeasConstraints::p_bool_le_reif);
//    registerConstraint("bool_le_imp", GeasConstraints::p_bool_le_imp);
//    registerConstraint("bool_gt", GeasConstraints::p_bool_gt);
//    registerConstraint("bool_gt_reif", GeasConstraints::p_bool_gt_reif);
//    registerConstraint("bool_gt_imp", GeasConstraints::p_bool_gt_imp);
//    registerConstraint("bool_lt", GeasConstraints::p_bool_lt);
//    registerConstraint("bool_lt_reif", GeasConstraints::p_bool_lt_reif);
//    registerConstraint("bool_lt_imp", GeasConstraints::p_bool_lt_imp);
//    registerConstraint("bool_or", GeasConstraints::p_bool_or);
//    registerConstraint("bool_or_imp", GeasConstraints::p_bool_or_imp);
//    registerConstraint("bool_and", GeasConstraints::p_bool_and);
//    registerConstraint("bool_and_imp", GeasConstraints::p_bool_and_imp);
//    registerConstraint("bool_xor", GeasConstraints::p_bool_xor);
//    registerConstraint("bool_xor_imp", GeasConstraints::p_bool_xor_imp);
//    registerConstraint("array_bool_and", GeasConstraints::p_array_bool_and);
//    registerConstraint("array_bool_and_imp", GeasConstraints::p_array_bool_and_imp);
//    registerConstraint("array_bool_or", GeasConstraints::p_array_bool_or);
//    registerConstraint("array_bool_or_imp", GeasConstraints::p_array_bool_or_imp);
//    registerConstraint("array_bool_xor", GeasConstraints::p_array_bool_xor);
//    registerConstraint("array_bool_xor_imp", GeasConstraints::p_array_bool_xor_imp);
//    registerConstraint("bool_clause", GeasConstraints::p_array_bool_clause);
//    registerConstraint("bool_clause_reif", GeasConstraints::p_array_bool_clause_reif);
//    registerConstraint("bool_clause_imp", GeasConstraints::p_array_bool_clause_imp);
//    registerConstraint("bool_left_imp", GeasConstraints::p_bool_l_imp);
//    registerConstraint("bool_right_imp", GeasConstraints::p_bool_r_imp);
//    registerConstraint("bool_not", GeasConstraints::p_bool_not);
//    registerConstraint("array_int_element", GeasConstraints::p_array_int_element);
//    registerConstraint("array_var_int_element", GeasConstraints::p_array_int_element);
//    registerConstraint("array_bool_element", GeasConstraints::p_array_bool_element);
//    registerConstraint("array_var_bool_element", GeasConstraints::p_array_bool_element);
//    registerConstraint("bool2int", GeasConstraints::p_bool2int);
//    registerConstraint("int_in", GeasConstraints::p_int_in);
//    registerConstraint("int_in_reif", GeasConstraints::p_int_in_reif);
//    registerConstraint("int_in_imp", GeasConstraints::p_int_in_imp);
//    registerConstraint("set_in", GeasConstraints::p_int_in);
//    registerConstraint("set_in_reif", GeasConstraints::p_int_in_reif);
//    registerConstraint("set_in_imp", GeasConstraints::p_int_in_imp);
//
//    registerConstraint("all_different_int", GeasConstraints::p_distinct);
//    registerConstraint("all_different_offset", GeasConstraints::p_distinctOffset);
//    registerConstraint("all_equal_int", GeasConstraints::p_all_equal);
//
//    registerConstraint("array_int_lt", GeasConstraints::p_array_int_lt);
//    registerConstraint("array_int_lq", GeasConstraints::p_array_int_lq);
//    registerConstraint("array_bool_lt", GeasConstraints::p_array_bool_lt);
//    registerConstraint("array_bool_lq", GeasConstraints::p_array_bool_lq);
//    registerConstraint("count", GeasConstraints::p_count);
//    registerConstraint("count_reif", GeasConstraints::p_count_reif);
//    registerConstraint("count_imp", GeasConstraints::p_count_imp);
//    registerConstraint("at_least_int", GeasConstraints::p_at_least);
//    registerConstraint("at_most_int", GeasConstraints::p_at_most);
//    registerConstraint("bin_packing_load", GeasConstraints::p_bin_packing_load);
//    registerConstraint("global_cardinality", GeasConstraints::p_global_cardinality);
//    registerConstraint("global_cardinality_closed", GeasConstraints::p_global_cardinality_closed);
//    registerConstraint("global_cardinality_low_up", GeasConstraints::p_global_cardinality_low_up);
//    registerConstraint("global_cardinality_low_up_closed", GeasConstraints::p_global_cardinality_low_up_closed);
//    registerConstraint("array_int_minimum", GeasConstraints::p_minimum);
//    registerConstraint("array_int_maximum", GeasConstraints::p_maximum);
//    registerConstraint("minimum_arg_int", GeasConstraints::p_minimum_arg);
//    registerConstraint("maximum_arg_int", GeasConstraints::p_maximum_arg);
//    registerConstraint("regular", GeasConstraints::p_regular);
//    registerConstraint("sort", GeasConstraints::p_sort);
//    registerConstraint("inverse_offsets", GeasConstraints::p_inverse_offsets);
//    registerConstraint("increasing_int", GeasConstraints::p_increasing_int);
//    registerConstraint("increasing_bool", GeasConstraints::p_increasing_bool);
//    registerConstraint("decreasing_int", GeasConstraints::p_decreasing_int);
//    registerConstraint("decreasing_bool", GeasConstraints::p_decreasing_bool);
//    registerConstraint("table_int", GeasConstraints::p_table_int);
//    registerConstraint("table_bool", GeasConstraints::p_table_bool);
//    registerConstraint("cumulatives", GeasConstraints::p_cumulatives);
//    registerConstraint("among_seq_int", GeasConstraints::p_among_seq_int);
//    registerConstraint("among_seq_bool", GeasConstraints::p_among_seq_bool);
//
//
//    registerConstraint("bool_lin_eq", GeasConstraints::p_bool_lin_eq);
//    registerConstraint("bool_lin_ne", GeasConstraints::p_bool_lin_ne);
//    registerConstraint("bool_lin_le", GeasConstraints::p_bool_lin_le);
//    registerConstraint("bool_lin_lt", GeasConstraints::p_bool_lin_lt);
//    registerConstraint("bool_lin_ge", GeasConstraints::p_bool_lin_ge);
//    registerConstraint("bool_lin_gt", GeasConstraints::p_bool_lin_gt);
//
//    registerConstraint("bool_lin_eq_reif", GeasConstraints::p_bool_lin_eq_reif);
//    registerConstraint("bool_lin_eq_imp", GeasConstraints::p_bool_lin_eq_imp);
//    registerConstraint("bool_lin_ne_reif", GeasConstraints::p_bool_lin_ne_reif);
//    registerConstraint("bool_lin_ne_imp", GeasConstraints::p_bool_lin_ne_imp);
//    registerConstraint("bool_lin_le_reif", GeasConstraints::p_bool_lin_le_reif);
//    registerConstraint("bool_lin_le_imp", GeasConstraints::p_bool_lin_le_imp);
//    registerConstraint("bool_lin_lt_reif", GeasConstraints::p_bool_lin_lt_reif);
//    registerConstraint("bool_lin_lt_imp", GeasConstraints::p_bool_lin_lt_imp);
//    registerConstraint("bool_lin_ge_reif", GeasConstraints::p_bool_lin_ge_reif);
//    registerConstraint("bool_lin_ge_imp", GeasConstraints::p_bool_lin_ge_imp);
//    registerConstraint("bool_lin_gt_reif", GeasConstraints::p_bool_lin_gt_reif);
//    registerConstraint("bool_lin_gt_imp", GeasConstraints::p_bool_lin_gt_imp);
//
//    registerConstraint("schedule_unary", GeasConstraints::p_schedule_unary);
//    registerConstraint("schedule_unary_optional", GeasConstraints::p_schedule_unary_optional);
//    registerConstraint("schedule_cumulative_optional", GeasConstraints::p_cumulative_opt);
//
//    registerConstraint("circuit", GeasConstraints::p_circuit);
//    registerConstraint("circuit_cost_array", GeasConstraints::p_circuit_cost_array);
//    registerConstraint("circuit_cost", GeasConstraints::p_circuit_cost);
//    registerConstraint("nooverlap", GeasConstraints::p_nooverlap);
//    registerConstraint("precede", GeasConstraints::p_precede);
//    registerConstraint("nvalue", GeasConstraints::p_nvalue);
//    registerConstraint("among", GeasConstraints::p_among);
//    registerConstraint("member_int", GeasConstraints::p_member_int);
//    registerConstraint("member_int_reif", GeasConstraints::p_member_int_reif);
//    registerConstraint("member_bool", GeasConstraints::p_member_bool);
//    registerConstraint("member_bool_reif", GeasConstraints::p_member_bool_reif);
//
//    registerConstraint("int2float", GeasConstraints::p_int2float);
//    registerConstraint("float_abs", GeasConstraints::p_float_abs);
//    registerConstraint("float_sqrt", GeasConstraints::p_float_sqrt);
//    registerConstraint("float_eq", GeasConstraints::p_float_eq);
//    registerConstraint("float_eq_reif", GeasConstraints::p_float_eq_reif);
//    registerConstraint("float_le", GeasConstraints::p_float_le);
//    registerConstraint("float_le_reif", GeasConstraints::p_float_le_reif);
//    registerConstraint("float_lt", GeasConstraints::p_float_lt);
//    registerConstraint("float_lt_reif", GeasConstraints::p_float_lt_reif);
//    registerConstraint("float_ne", GeasConstraints::p_float_ne);
//    registerConstraint("float_times", GeasConstraints::p_float_times);
//    registerConstraint("float_div", GeasConstraints::p_float_div);
//    registerConstraint("float_plus", GeasConstraints::p_float_plus);
//    registerConstraint("float_max", GeasConstraints::p_float_max);
//    registerConstraint("float_min", GeasConstraints::p_float_min);
//    registerConstraint("float_lin_eq", GeasConstraints::p_float_lin_eq);
//    registerConstraint("float_lin_eq_reif", GeasConstraints::p_float_lin_eq_reif);
//    registerConstraint("float_lin_le", GeasConstraints::p_float_lin_le);
//    registerConstraint("float_lin_le_reif", GeasConstraints::p_float_lin_le_reif);
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
  }

  void GeasSolverInstance::processFlatZinc() {
    // Create variables
    for(auto it = _flat->begin_vardecls(); it != _flat->end_vardecls(); ++it) {
      if (!it->removed() && it->e()->type().isvar() && it->e()->type().dim() == 0) {
        VarDecl* vd = it->e();

        if(vd->type().isbool()) {
          if(!vd->e()) {
            Expression* domain = vd->ti()->domain();
            long long int lb, ub;
            if(domain) {
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
        } else if(vd->type().isfloat()) {
          if(!vd->e()) {
            Expression* domain = vd->ti()->domain();
            double lb, ub;
            if (domain) {
              FloatBounds fb = compute_float_bounds(_env.envi(), vd->id());
              lb = fb.l.toDouble();
              ub = fb.u.toDouble();
            } else {
              throw Error("GeasSolverInstance::processFlatZinc: Error: Unbounded variable: " + vd->id()->str().str());
            }
            // TODO: Error correction from double to float??
            auto var = _solver.new_floatvar(static_cast<geas::fp::val_t>(lb), static_cast<geas::fp::val_t>(ub));
            _variableMap.insert(vd->id(), GeasVariable(var));
          } else {
            Expression* init = vd->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              GeasVariable& var = resolveVar(init);
              assert(var.isFloat());
              _variableMap.insert(vd->id(), GeasVariable(var.floatVar()));
            } else {
              double fl = init->cast<FloatLit>()->v().toDouble();
              auto var = _solver.new_floatvar(static_cast<geas::fp::val_t>(fl), static_cast<geas::fp::val_t>(fl));
              _variableMap.insert(vd->id(), GeasVariable(var));
            }
          }
        } else if (vd->type().isint()) {
          if (!vd->e()) {
            Expression* domain = vd->ti()->domain();
            if (domain) {
              IntSetVal* isv = eval_intset(env().envi(), domain);
              auto var = _solver.new_intvar(static_cast<geas::intvar::val_t>(isv->min().toInt()), static_cast<geas::intvar::val_t>(isv->max().toInt()));
              if (isv->size() > 1) {
                vec<int> vals(static_cast<int>(isv->card().toInt()));
                int i = 0;
                for (int j = 0; j < isv->size(); ++j) {
                  for (auto k = isv->min(i).toInt(); k <= isv->max(j).toInt(); ++k) {
                    vals[i++] = static_cast<int>(k);
                  }
                }
                assert(i == isv->card().toInt());
                auto res = geas::make_sparse(var, vals);
                assert(res);
              }
              _variableMap.insert(vd->id(), GeasVariable(var));
            } else {
              throw Error("GeasSolverInstance::processFlatZinc: Error: Unbounded variable: " + vd->id()->str().str());
            }
          } else {
            Expression* init = vd->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              GeasVariable& var = resolveVar(init);
              assert(var.isInt());
              _variableMap.insert(vd->id(), GeasVariable(var.intVar()));
            } else {
              auto il = init->cast<IntLit>()->v().toInt();
              auto var = _solver.new_intvar(static_cast<geas::intvar::val_t>(il), static_cast<geas::intvar::val_t>(il));
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
    for (ConstraintIterator it = _flat->begin_constraints(); it != _flat->end_constraints(); ++it) {
      if(!it->removed()) {
        if (auto c = it->e()->dyn_cast<Call>()) {
          _constraintRegistry.post(c);
        }
      }
    }
    // Set objective
    SolveI* si = _flat->solveItem();
    if(si->e()) {
      auto id = si->e()->dyn_cast<Id>();
      assert(id); // The solve expression has to be a variable/id
      auto type = si->st();
      // TODO: Actually do something with the objective
    }
  }

  SolverInstanceBase::Status MiniZinc::GeasSolverInstance::solve() {
    // TODO: Other types of solving types
    geas::solver::result res = _solver.solve();
    printSolution();
    switch (res) {
      case geas::solver::SAT:
        return SolverInstance::SAT;
      case geas::solver::UNSAT:
        return SolverInstance::UNSAT;
      case geas::solver::UNKNOWN:
        return SolverInstance::UNKNOWN;
      default:
        return SolverInstance::ERROR;
    }
  }

  Expression* GeasSolverInstance::getSolutionValue(Id* id) {
    id = id->decl()->id();
    if(id->type().isvar()) {
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

  void GeasSolverInstance::resetSolver() {
    assert(false);
  }

  GeasTypes::Variable& GeasSolverInstance::resolveVar(Expression* e) {
    if (auto id = e->dyn_cast<Id>()) {
      return _variableMap.get(id->decl()->id());
    } else if (auto vd = e->dyn_cast<VarDecl>()) {
      return _variableMap.get(vd->id()->decl()->id());
    } else if (auto aa = e->dyn_cast<ArrayAccess>()) {
      auto ad = aa->v()->cast<Id>()->decl();
      auto idx = aa->idx()[0]->cast<IntLit>()->v().toInt();
      auto al = eval_array_lit(_env.envi(), ad->e());
      return _variableMap.get((*al)[idx]->cast<Id>());
    } else {
      std::stringstream ssm;
      ssm << "Expected Id, VarDecl or ArrayAccess instead of \"" << *e << "\"";
      throw InternalError(ssm.str());
    }
  }

  geas::patom_t GeasSolverInstance::arg2boolvar(Expression* e) {
    if (e->type().isvar()) {
      GeasVariable& var = resolveVar(follow_id_to_decl(e));
      assert(var.isBool());
      return var.boolVar();
    } else {
      if(auto bl = e->dyn_cast<BoolLit>()) {
        return bl->v() ? geas::at_True : geas::at_False;
      } else {
        std::stringstream ssm; ssm << "Expected bool or int literal instead of: " << *e;
        throw InternalError(ssm.str());
      }
    }
  }

  geas::intvar GeasSolverInstance::arg2intvar(Expression* e) {
    if (e->type().isvar()) {
      GeasVariable& var = resolveVar(follow_id_to_decl(e));
      assert(var.isInt());
      return var.intVar();
    } else {
      IntVal i;
      if(auto il = e->dyn_cast<IntLit>()) {
        i = il->v().toInt();
      } else if(auto bl = e->dyn_cast<BoolLit>()) {
        i = bl->v();
      } else {
        std::stringstream ssm; ssm << "Expected bool or int literal instead of: " << *e;
        throw InternalError(ssm.str());
      }
      return _solver.new_intvar(static_cast<geas::intvar::val_t>(i.toInt()), static_cast<geas::intvar::val_t>(i.toInt()));
    }
  }

  Geas_SolverFactory::Geas_SolverFactory() {
    SolverConfig sc("org.minizinc.geas", getVersion(nullptr));
    sc.name("Geas");
    sc.mznlib("-Ggeas");
    sc.mznlibVersion(1);
    sc.supportsMzn(false);
    sc.description(getDescription(nullptr));
    sc.tags({"api","cp","float","int","lcg",});
    sc.stdFlags({});
    SolverConfigs::registerBuiltinSolver(sc);
  };

  SolverInstanceBase::Options* Geas_SolverFactory::createOptions() {
    return new GeasOptions;
  }

  SolverInstanceBase* Geas_SolverFactory::doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt) {
    return new GeasSolverInstance(env, log, opt);
  }

  bool Geas_SolverFactory::processOption(SolverInstanceBase::Options* opt, int &i, std::vector<std::string> &argv) {
    return false;
  }

  void Geas_SolverFactory::printHelp(std::ostream &os) {
    os  << "Geas solver does not yet support any solver specific options." << std::endl;
  }
}

