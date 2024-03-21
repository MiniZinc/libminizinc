/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/exception.hh>
#include <minizinc/solver_instance.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/atlantis_solverinstance.hh>
#include <minizinc/statistics.hh>

#include "minizinc/aststring.hh"

#include <cstdio>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

#include <atlantis/fznBackend.hpp>
#include <atlantis/search/searchStatistics.hpp>
#include <fznparser/model.hpp>
#include <fznparser/parser.hpp>

namespace MiniZinc {

AtlantisSolverInstance::AtlantisSolverInstance(Env& env, std::ostream& log,
                                               SolverInstanceBase::Options* opt)
    : SolverInstanceBase2(env, log, opt),
      _fzn(env.flat()),
      // TODO: Doesn't use `log'
      _logger(stderr, static_cast<AtlantisOptions*>(opt)->logLevel) {}

void AtlantisSolverInstance::processFlatZinc() {
  auto& _opt = static_cast<AtlantisOptions&>(*_options);

  // Create FlatZinc string
  std::stringstream ss;
  {  // Context to print FZN file
    Printer p(ss, 0, true, &_env.envi());
    for (FunctionIterator it = _fzn->functions().begin(); it != _fzn->functions().end(); ++it) {
      if (!it->removed()) {
        Item& item = *it;
        p.print(&item);
      }
    }
    for (VarDeclIterator it = _fzn->vardecls().begin(); it != _fzn->vardecls().end(); ++it) {
      if (!it->removed()) {
        Item& item = *it;
        p.print(&item);
      }
    }
    for (ConstraintIterator it = _fzn->constraints().begin(); it != _fzn->constraints().end();
         ++it) {
      if (!it->removed()) {
        Item& item = *it;
        p.print(&item);
      }
    }
    p.print(_fzn->solveItem());
  }

  // Parse FlatZinc string
  fznparser::Model atlModel = fznparser::parseFznIstream(ss);

  auto varToInt = [&](const atlantis::search::Assignment& assignment,
                      const std::variant<atlantis::propagation::VarId, long long>& var) {
    return std::holds_alternative<long long>(var)
               ? std::get<long long>(var)
               : assignment.value(std::get<atlantis::propagation::VarId>(var));
  };
  auto varToBool = [&](const atlantis::search::Assignment& assignment,
                       const std::variant<atlantis::propagation::VarId, long long>& var) {
    return (varToInt(assignment, var) == 0);
  };

  // Initialize Atlantis FZN Backend
  _backend = std::make_unique<atlantis::FznBackend>(std::move(atlModel));

  _backend->setOnSolution([&](const atlantis::invariantgraph::FznInvariantGraph& invariantGraph,
                              const atlantis::search::Assignment& assignment) {
    {
      GCLock lock;
      for (const auto& outputVar : invariantGraph.outputBoolVars()) {
        _assignment[ASTString(outputVar.identifier)] =
            _env.envi().constants.boollit(varToBool(assignment, outputVar.var));
      }
      for (const auto& outputVar : invariantGraph.outputIntVars()) {
        _assignment[ASTString(outputVar.identifier)] =
            IntLit::a(varToInt(assignment, outputVar.var));
      }
      for (const auto& outputVarArray : invariantGraph.outputBoolVarArrays()) {
        std::vector<Expression*> vals(outputVarArray.vars.size());
        for (size_t i = 0; i < outputVarArray.vars.size(); ++i) {
          vals[i] = _env.envi().constants.boollit(varToBool(assignment, outputVarArray.vars[i]));
        }
        auto* al = new ArrayLit(Location().introduce(), vals);
        Expression::type(al, Type::parbool(1));
        _assignment[ASTString(outputVarArray.identifier)] = al;
      }
      for (const auto& outputVarArray : invariantGraph.outputIntVarArrays()) {
        std::vector<Expression*> vals(outputVarArray.vars.size());
        for (size_t i = 0; i < outputVarArray.vars.size(); ++i) {
          vals[i] = IntLit::a(varToInt(assignment, outputVarArray.vars[i]));
        }
        auto* al = new ArrayLit(Location().introduce(), vals);
        Expression::type(al, Type::parint(1));
        _assignment[ASTString(outputVarArray.identifier)] = al;
      }
    }
    printSolution();
  });
  _backend->setOnFinish([&](bool hadSol) { _hadSol = hadSol; });

  // Set optional flags in backend
  if (_opt.time > std::chrono::milliseconds(0)) {
    _backend->setTimelimit(_opt.time);
  }
  if (!_opt.annealingSchedule.empty()) {
    _backend->setAnnealingScheduleFactory(
        atlantis::search::AnnealingScheduleFactory(_opt.annealingSchedule));
  }
  if (_opt.randomSeed != 0) {
    _backend->setRandomSeed(_opt.randomSeed);
  }
}

SolverInstanceBase::Status AtlantisSolverInstance::solve() {
  _hadSol = false;
  _statistics = _backend->solve(_logger);

  if (_hadSol) {
    return SolverInstanceBase::Status::SAT;
  }
  return SolverInstanceBase::Status::UNKNOWN;
}
Expression* AtlantisSolverInstance::getSolutionValue(Id* i) { return _assignment[i->str()]; }

void AtlantisSolverInstance::printStatistics() {
  auto* solns2out = getSolns2Out();
  StatisticsStream ss(solns2out->getOutput(), solns2out->opt.flagEncapsulateJSON);
  for (const auto& stat : _statistics) {
    ss.add(std::string(stat->name()), stat->value());
  }
}

#define ATLANTIS_VERSION_TO_STRING(x) #x

std::string AtlantisSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return ATLANTIS_VERSION_TO_STRING(ATLANTIS_VERSION);
}

SolverInstanceBase::Options* AtlantisSolverFactory::createOptions() { return new AtlantisOptions; }

SolverInstanceBase* AtlantisSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                      SolverInstanceBase::Options* opt) {
  return new AtlantisSolverInstance(env, log, opt);
}

bool AtlantisSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                          std::vector<std::string>& argv,
                                          const std::string& workingDir) {
  auto* _opt = static_cast<AtlantisOptions*>(opt);
  CLOParser cop(i, argv);
  std::string ss;
  int nn = -1;
  double dd = -1;
  if (cop.get("-i")) {
    _opt->intermediateSolutions = true;
  } else if (cop.get("-t --solver-time-limit", &nn)) {
    _opt->time = std::chrono::milliseconds(nn);
  } else if (cop.get("-r --seed --random-seed", &nn)) {
    _opt->randomSeed = nn;
  } else if (cop.get("--annealing-schedule", &ss)) {
    _opt->annealingSchedule = ss;
  } else if (cop.get("--log-level", &nn)) {
    switch (nn) {
      case 0:
        _opt->logLevel = atlantis::logging::Level::ERR;
      case 1:
        _opt->logLevel = atlantis::logging::Level::WARN;
      case 2:
        _opt->logLevel = atlantis::logging::Level::INFO;
      case 3:
        _opt->logLevel = atlantis::logging::Level::DEBUG;
      case 4:
        _opt->logLevel = atlantis::logging::Level::TRACE;
      default:
        return false;
    }
  } else {
    return false;
  }
  return true;
}

}  // namespace MiniZinc
