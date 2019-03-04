/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/geas_solverinstance.hh>

namespace MiniZinc{
  GeasSolverInstance::GeasSolverInstance(Env &env, std::ostream &log, SolverInstanceBase::Options *opt)
      : SolverInstanceImpl<GeasTypes>(env, log, opt), _flat(env.flat()) {}

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
            // TODO: Deal with actual domain
            auto var = _solver.new_boolvar();
            _variableMap.insert(vd->id(), GeasVariable(GeasVariable::BOOL_TYPE, new geas::patom_t(var)));
          } else {
            Expression* init = vd->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              GeasVariable var = resolveVar(init);
              assert(var.isBool());
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::BOOL_TYPE, new geas::patom_t(var.boolVar())));
            } else {
              auto b = (double) init->cast<BoolLit>()->v();
              // TODO: Deal with actual domain
              auto var = _solver.new_boolvar();
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::BOOL_TYPE, new geas::patom_t(var)));
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
            _variableMap.insert(vd->id(), GeasVariable(GeasVariable::FLOAT_TYPE, new geas::fp::fpvar(var)));
          } else {
            Expression* init = vd->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              GeasVariable var = resolveVar(init);
              assert(var.isFloat());
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::FLOAT_TYPE, new geas::fp::fpvar(var.floatVar())));
            } else {
              double fl = init->cast<FloatLit>()->v().toDouble();
              auto var = _solver.new_floatvar(static_cast<geas::fp::val_t>(fl), static_cast<geas::fp::val_t>(fl));
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::FLOAT_TYPE, new geas::fp::fpvar(var)));
            }
          }
        } else if (vd->type().isint()) {
          if (!vd->e()) {
            Expression* domain = vd->ti()->domain();
            if (domain) {
              IntSetVal* isv = eval_intset(env().envi(), domain);
              // TODO: Deal with domain gaps
              auto var = _solver.new_intvar(static_cast<geas::intvar::val_t>(isv->min().toInt()), static_cast<geas::intvar::val_t>(isv->min().toInt()));
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::INT_TYPE, new geas::intvar(var)));
            } else {
              throw Error("GeasSolverInstance::processFlatZinc: Error: Unbounded variable: " + vd->id()->str().str());
            }
          } else {
            Expression* init = vd->e();
            if (init->isa<Id>() || init->isa<ArrayAccess>()) {
              GeasVariable var = resolveVar(init);
              assert(var.isInt());
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::INT_TYPE, new geas::intvar(var.intVar())));
            } else {
              auto il = init->cast<IntLit>()->v().toInt();
              auto var = _solver.new_intvar(static_cast<geas::intvar::val_t>(il), static_cast<geas::intvar::val_t>(il));
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::INT_TYPE, new geas::intvar(var)));
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
        if (Call* c = it->e()->dyn_cast<Call>()) {
          // TODO: Post Constraints
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

  Expression* GeasSolverInstance::getSolutionValue(Id* id) {
    id = id->decl()->id();
    if(id->type().isvar()) {
      GeasVariable var = resolveVar(id->decl()->id());
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

  GeasTypes::Variable GeasSolverInstance::resolveVar(Expression* e) {
    if (Id* id = e->dyn_cast<Id>()) {
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

