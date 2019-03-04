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

        if (vd->type().isint()) {
          if (!vd->e()) {
            Expression* domain = vd->ti()->domain();
            if (domain) {
              IntSetVal* isv = eval_intset(env().envi(), domain);
              auto var = _solver.new_intvar(static_cast<int64_t>(isv->min().toInt()), static_cast<int64_t>(isv->min().toInt()));
              _variableMap.insert(vd->id(), GeasVariable(GeasVariable::INT_TYPE, new geas::intvar(var)));
            } else {
              throw("GeasSolverInstance::processFlatZinc: Error: Unbounded variable: " + vd->id()->str().str());
            }
          } else {
            // TODO: Lookup expressions
          }
        } else if(vd->type().isbool()) {
          // TODO: Add boolean variable
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

  GeasTypes::Variable
  GeasSolverInstance::resolveVar(Expression* e) {
    if (Id* id = e->dyn_cast<Id>()) {
      return _variableMap.get(id->decl()->id());
    } else if (auto vd = e->dyn_cast<VarDecl>()) {
      return _variableMap.get(vd->id()->decl()->id());
    } else if (auto aa = e->dyn_cast<ArrayAccess>()) {
      // TODO: Deal with ArrayAccess
//      return _variableMap.get(resolveArrayAccess(aa)->id()->decl()->id());
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

