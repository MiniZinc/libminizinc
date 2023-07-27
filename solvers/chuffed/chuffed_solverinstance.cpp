/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/exception.hh>
#include <minizinc/solvers/chuffed_solverinstance.hh>
#include <minizinc/statistics.hh>

#include <chuffed/core/engine.h>
#include <chuffed/core/sat.h>
#include <memory>

namespace MiniZinc {
ChuffedSolverInstance::ChuffedSolverInstance(Env& env, std::ostream& log,
                                             SolverInstanceBase::Options* opt)
    : SolverInstanceImpl<ChuffedTypes>(env, log, opt), _flat(env.flat()), _space(nullptr) {
  _space = nullptr;
}

void ChuffedSolverInstance::processFlatZinc() {
  try {
    auto boolVarCount = 0;
    auto intVarCount = 0;
    for (auto& it : _flat->vardecls()) {
      if (!it.removed() && it.e()->type().isvar() && it.e()->type().dim() == 0) {
        auto* vd = it.e();
        if (vd->type().isbool()) {
          boolVarCount++;
        } else if (vd->type().isint()) {
          intVarCount++;
        }
      }
    }
    std::vector<std::pair<int, int>> lastValBool;
    std::vector<std::pair<int, int>> lastValInt;
    std::vector<std::pair<Id*, std::unique_ptr<FlatZinc::BoolVarSpec>>> boolVars;
    boolVars.reserve(boolVarCount);
    std::vector<std::pair<Id*, std::unique_ptr<FlatZinc::IntVarSpec>>> intVars;
    intVars.reserve(intVarCount);
    std::vector<std::unique_ptr<FlatZinc::ConExpr>> domConstraints;

    // Create variables
    for (auto& it : _flat->vardecls()) {
      if (!it.removed() && it.e()->type().isvar()) {
        auto* vd = it.e();
        if (it.e()->type().dim() == 0) {
          auto output = Expression::ann(vd).contains(_env.envi().constants.ann.output_var);
          auto looks = vd->id()->idn() != -1;
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
                std::unique_ptr<FlatZinc::BoolVarSpec> spec(
                    new FlatZinc::BoolVarSpec(ub == 1, output, vd->introduced(), looks));
                _variableMap.insert(vd->id(),
                                    ChuffedVariable::boolVar(static_cast<int>(boolVars.size())));
                boolVars.emplace_back(vd->id(), std::move(spec));
              } else {
                auto dom = FlatZinc::Option<FlatZinc::AST::SetLit*>::none();
                std::unique_ptr<FlatZinc::BoolVarSpec> spec(
                    new FlatZinc::BoolVarSpec(dom, output, vd->introduced(), looks));
                _variableMap.insert(vd->id(),
                                    ChuffedVariable::boolVar(static_cast<int>(boolVars.size())));
                boolVars.emplace_back(vd->id(), std::move(spec));
              }
            } else {
              Expression* init = vd->e();
              if (auto* ident = Expression::dynamicCast<Id>(init)) {
                auto& var = _variableMap.get(ident);
                assert(var.isBool());
                std::unique_ptr<FlatZinc::BoolVarSpec> spec(new FlatZinc::BoolVarSpec(
                    FlatZinc::Alias(var.index()), output, vd->introduced(), looks));
                _variableMap.insert(vd->id(),
                                    ChuffedVariable::boolVar(static_cast<int>(boolVars.size())));
                boolVars.emplace_back(vd->id(), std::move(spec));
              } else {
                auto b = Expression::cast<BoolLit>(init)->v();
                std::unique_ptr<FlatZinc::BoolVarSpec> spec(
                    new FlatZinc::BoolVarSpec(b, output, vd->introduced(), looks));
                _variableMap.insert(vd->id(),
                                    ChuffedVariable::boolVar(static_cast<int>(boolVars.size())));
                boolVars.emplace_back(vd->id(), std::move(spec));
              }
            }
          } else if (vd->type().isint()) {
            FlatZinc::AST::SetLit* sl = nullptr;
            if (vd->ti()->domain() != nullptr) {
              IntSetVal* isv = eval_intset(env().envi(), vd->ti()->domain());
              if (isv->size() > 1) {
                std::vector<int> vs;
                for (unsigned int i = 0; i < isv->size(); i++) {
                  for (auto j = isv->min(i); j <= isv->max(i); j++) {
                    vs.push_back(static_cast<int>(j.toInt()));
                  }
                }
                sl = new FlatZinc::AST::SetLit(vs);
              } else {
                auto a = static_cast<int>(isv->min(0).toInt());
                auto b = static_cast<int>(isv->max(0).toInt());
                sl = new FlatZinc::AST::SetLit(a, b);
              }
            }
            if (vd->e() == nullptr) {
              auto dom = sl == nullptr ? FlatZinc::Option<FlatZinc::AST::SetLit*>::none()
                                       : FlatZinc::Option<FlatZinc::AST::SetLit*>::some(sl);
              std::unique_ptr<FlatZinc::IntVarSpec> spec(
                  new FlatZinc::IntVarSpec(dom, output, vd->introduced(), looks));
              _variableMap.insert(vd->id(),
                                  ChuffedVariable::intVar(static_cast<int>(intVars.size())));
              intVars.emplace_back(vd->id(), std::move(spec));
            } else {
              Expression* init = vd->e();
              if (auto* ident = Expression::dynamicCast<Id>(init)) {
                auto& var = _variableMap.get(ident);
                assert(var.isInt());
                std::unique_ptr<FlatZinc::IntVarSpec> spec(new FlatZinc::IntVarSpec(
                    FlatZinc::Alias(var.index()), output, vd->introduced(), looks));
                _variableMap.insert(vd->id(),
                                    ChuffedVariable::intVar(static_cast<int>(intVars.size())));
                intVars.emplace_back(vd->id(), std::move(spec));
              } else {
                auto il = static_cast<int>(IntLit::v(Expression::cast<IntLit>(init)).toInt());
                std::unique_ptr<FlatZinc::IntVarSpec> spec(
                    new FlatZinc::IntVarSpec(il, output, vd->introduced(), looks));
                _variableMap.insert(vd->id(),
                                    ChuffedVariable::intVar(static_cast<int>(intVars.size())));
                intVars.emplace_back(vd->id(), std::move(spec));
              }
              if (sl != nullptr) {
                domConstraints.emplace_back(new FlatZinc::ConExpr(
                    "set_in",
                    new FlatZinc::AST::Array(
                        {new FlatZinc::AST::IntVar(static_cast<int>(intVars.size() - 1)), sl})));
              }
            }
          } else {
            std::stringstream ssm;
            ssm << "Type " << *vd->ti() << " is currently not supported by Chuffed.";
            throw InternalError(ssm.str());
          }
        } else if (Expression::ann(vd).containsCall(_env.envi().constants.ann.output_array)) {
          auto* al = Expression::cast<ArrayLit>(vd->e());
          for (unsigned int i = 0; i < al->size(); i++) {
            if (auto* ident = Expression::dynamicCast<Id>((*al)[i])) {
              auto& var = _variableMap.get(ident->decl()->id());
              if (var.isBool()) {
                boolVars[var.index()].second->output = true;
              } else {
                intVars[var.index()].second->output = true;
              }
            }
          }
        }
      }
    }

    _space = new FlatZinc::FlatZincSpace(intVarCount, boolVarCount, 0);
    for (auto& iv : intVars) {
      auto name = iv.first->str();
      _space->newIntVar(iv.second.get(), std::string(name.c_str(), name.size()));
      if (iv.second->output) {
        std::unique_ptr<FlatZinc::AST::IntVar> ivn(
            new FlatZinc::AST::IntVar(_space->intVarCount - 1));
        _space->setOutputElem(ivn.get());
      }
    }
    for (auto& bv : boolVars) {
      auto name = bv.first->str();
      std::string label(name.c_str(), name.size());
      _space->newBoolVar(bv.second.get());
      BoolView newbv = _space->bv[_space->boolVarCount - 1];
      boolVarString.emplace(newbv, bv.second->assigned ? "ASSIGNED_AT_ROOT" : label);
      litString.emplace(toInt(newbv.getLit(true)), label + "=true");
      litString.emplace(toInt(newbv.getLit(false)), label + "=false");
      if (bv.second->output) {
        std::unique_ptr<FlatZinc::AST::BoolVar> bvn(
            new FlatZinc::AST::BoolVar(_space->boolVarCount - 1));
        _space->setOutputElem(bvn.get());
      }
    }

    std::function<FlatZinc::AST::Node*(Expression*)> toNode;
    toNode = [this, &toNode](Expression* e) -> FlatZinc::AST::Node* {
      switch (Expression::eid(e)) {
        case Expression::E_BOOLLIT: {
          return new FlatZinc::AST::BoolLit(Expression::cast<BoolLit>(e)->v());
        }
        case Expression::E_INTLIT: {
          auto v = static_cast<int>(IntLit::v(Expression::cast<IntLit>(e)).toInt());
          return new FlatZinc::AST::IntLit(v);
        }
        case Expression::E_STRINGLIT: {
          return new FlatZinc::AST::String(Expression::cast<StringLit>(e)->v().c_str());
        }
        case Expression::E_ID: {
          auto* ident = Expression::cast<Id>(e);
          if (ident->type().isAnn()) {
            return new FlatZinc::AST::Atom(ident->str().c_str());
          }
          if (ident->type().dim() > 0) {
            assert(ident->decl()->e() != nullptr);
            return toNode(ident->decl()->e());
          }
          auto& var = _variableMap.get(ident->decl()->id());
          if (var.isBool()) {
            return new FlatZinc::AST::BoolVar(var.index());
          }
          return new FlatZinc::AST::IntVar(var.index());
        }
        case Expression::E_CALL: {
          auto* c = Expression::cast<Call>(e);
          if (c->argCount() == 1) {
            return new FlatZinc::AST::Call(c->id().c_str(), toNode(c->arg(0)));
          }
          std::vector<FlatZinc::AST::Node*> args(c->argCount());
          for (unsigned int i = 0; i < c->argCount(); i++) {
            args[i] = toNode(c->arg(i));
          }
          return new FlatZinc::AST::Call(c->id().c_str(), new FlatZinc::AST::Array(args));
        }
        case Expression::E_ARRAYLIT: {
          auto* al = Expression::cast<ArrayLit>(e);
          std::vector<FlatZinc::AST::Node*> elems(al->size());
          for (unsigned int i = 0; i < al->size(); i++) {
            elems[i] = toNode((*al)[i]);
          }
          return new FlatZinc::AST::Array(elems);
        }
        case Expression::E_SETLIT: {
          auto* sl = Expression::cast<SetLit>(e);
          auto* isv = sl->isv();
          if (isv != nullptr) {
            if (isv->size() == 1) {
              return new FlatZinc::AST::SetLit(static_cast<int>(isv->min(0).toInt()),
                                               static_cast<int>(isv->max(0).toInt()));
            }
            std::vector<int> vs(isv->card().toInt());
            for (unsigned int i = 0; i < isv->size(); i++) {
              for (auto j = isv->min(i); j <= isv->max(i); j++) {
                vs.push_back(static_cast<int>(j.toInt()));
              }
            }
            return new FlatZinc::AST::SetLit(vs);
          }
        }
        default:
          break;
      }
      throw InternalError("Unsupported expression");
    };

    // Post constraints
    for (auto& it : _flat->constraints()) {
      if (!it.removed()) {
        auto* c = Expression::cast<Call>(it.e());
        std::vector<FlatZinc::AST::Node*> args(c->argCount());
        for (unsigned int i = 0; i < c->argCount(); i++) {
          args[i] = toNode(c->arg(i));
        }
        if (c->id() == "chuffed_on_restart_status") {
          _space->restart_status = args[0]->getIntVar();
          _space->enable_on_restart = true;
        } else if (c->id() == "chuffed_on_restart_complete") {
          mark_complete(_space->bv[args[0]->getBoolVar()], &_space->mark_complete);
          _space->enable_on_restart = true;
        } else if (c->id() == "chuffed_on_restart_uniform_int") {
          _space->int_uniform.emplace_back(
              std::array<int, 3>{args[0]->getInt(), args[1]->getInt(), args[2]->getIntVar()});
          _space->enable_on_restart = true;
        } else if (c->id() == "chuffed_on_restart_last_val_bool") {
          lastValBool.emplace_back(args[0]->getBoolVar(), args[1]->getBoolVar());
          _space->enable_on_restart = true;
        } else if (c->id() == "chuffed_on_restart_last_val_int") {
          lastValInt.emplace_back(args[0]->getIntVar(), args[1]->getIntVar());
          _space->enable_on_restart = true;
        } else if (c->id() == "chuffed_on_restart_sol_bool") {
          _space->bool_sol.emplace_back(
              std::tuple<int, bool, int>{args[0]->getBoolVar(), false, args[1]->getBoolVar()});
          _space->enable_on_restart = true;
          _space->enable_store_solution = true;
        } else if (c->id() == "chuffed_on_restart_sol_int") {
          _space->int_sol.emplace_back(
              std::array<int, 3>{args[0]->getIntVar(), 0, args[1]->getIntVar()});
          _space->enable_on_restart = true;
          _space->enable_store_solution = true;
        } else {
          std::unique_ptr<FlatZinc::AST::Array> ann;
          if (!Expression::ann(c).isEmpty()) {
            std::vector<FlatZinc::AST::Node*> annotations;
            for (const auto& ann : Expression::ann(c)) {
              annotations.push_back(toNode(ann));
            }
            ann = std::make_unique<FlatZinc::AST::Array>(annotations);
          }
          FlatZinc::FlatZincSpace::postConstraint(
              FlatZinc::ConExpr(c->id().c_str(), new FlatZinc::AST::Array(args)), ann.get());
        }
      }
    }

    // Set objective
    SolveI* si = _flat->solveItem();
    std::unique_ptr<FlatZinc::AST::Array> ann;
    if (!si->ann().isEmpty()) {
      std::vector<FlatZinc::AST::Node*> annotations;
      for (const auto& ann : si->ann()) {
        annotations.push_back(toNode(ann));
      }
      ann = std::make_unique<FlatZinc::AST::Array>(annotations);
    }

    switch (si->st()) {
      case SolveI::ST_SAT:
        _isSatisfaction = true;
        _space->solve(ann.get());
        break;
      case SolveI::ST_MIN:
        _isSatisfaction = false;
        _space->minimize(_variableMap.get(Expression::cast<Id>(si->e())->decl()->id()).index(),
                         ann.get());
        break;
      case SolveI::ST_MAX:
        _isSatisfaction = false;
        _space->maximize(_variableMap.get(Expression::cast<Id>(si->e())->decl()->id()).index(),
                         ann.get());
        break;
    }

    _space->bool_last_val.resize(lastValBool.size());
    for (size_t i = 0; i < lastValBool.size(); ++i) {
      _space->bool_last_val[i] = std::pair<int, bool>{lastValBool[i].second, false};
      last_val(&_space->bv[lastValBool[i].first], &(_space->bool_last_val[i].second));
    }
    _space->int_last_val.resize(lastValInt.size());
    for (size_t i = 0; i < lastValInt.size(); ++i) {
      _space->int_last_val[i] =
          std::array<int, 2>{lastValInt[i].second, _space->iv[lastValInt[i].first]->getMin()};
      last_val(_space->iv[lastValInt[i].first], &(_space->int_last_val[i][1]));
    }
  } catch (FlatZinc::Error& e) {
    throw Error(e.toString());
  }
}

SolverInstanceBase::Status ChuffedSolverInstance::solve() {
  SolverInstanceBase::Status status = SolverInstance::ERROR;
  auto& _opt = static_cast<ChuffedOptions&>(*_options);
  so.time_out = _opt.time;
  so.nof_solutions = _opt.nrSolutions;
  if (_opt.allSolutions) {
    so.nof_solutions = 0;
  }
  if (_opt.freeSearch) {
    so.toggle_vsids = true;
    so.restart_scale = 100;
  }
  if (_opt.verbose) {
    so.verbosity = 1;
  }
  so.rnd_seed = _opt.randomSeed;

  // Silence chuffed output since we are handling printing ourselves
  so.print_sol = false;
  std::stringstream ss;
  engine.setOutputStream(ss);

  bool lastSolutionOnly = !_opt.allSolutions && !_isSatisfaction;
  if (lastSolutionOnly) {
    engine.setSolutionCallback([this](Problem* p) {
      // Assign solution but don't print it
      GCLock lock;
      assignSolutionToOutput();
    });
  } else {
    engine.setSolutionCallback([this](Problem* p) { printSolution(); });
  }
  engine.set_assumptions(_space->assumptions);
  engine.solve(_space);
  if (lastSolutionOnly && engine.solutions > 0) {
    // Print optimal solution
    GCLock lock;
    SolverInstanceBase::printSolution();
  }
  switch (engine.status) {
    case RESULT::RES_UNK:
      if (engine.solutions > 0) {
        status = SolverInstanceBase::Status::SAT;
      } else {
        status = SolverInstanceBase::Status::UNKNOWN;
      }
      break;
    case RESULT::RES_SAT:
      status = SolverInstanceBase::Status::SAT;
      break;
    case RESULT::RES_GUN:
    case RESULT::RES_LUN:
      if (engine.solutions > 0) {
        status = SolverInstanceBase::Status::OPT;
      } else {
        status = SolverInstanceBase::Status::UNSAT;
      }
      break;
    default:
      assert(false);
      break;
  }
  if (_opt.statistics) {
    printStatistics();
  }
  return status;
}

Expression* ChuffedSolverInstance::getSolutionValue(Id* i) {
  auto* ident = i->decl()->id();
  if (ident->type().isvar()) {
    auto& var = _variableMap.get(ident);
    switch (ident->type().bt()) {
      case Type::BT_BOOL:
        assert(var.isBool());
        return Constants::constants().boollit(_space->bv[var.index()].getVal() != 0);
      case Type::BT_INT:
        assert(var.isInt());
        return IntLit::a(_space->iv[var.index()]->getVal());
      default:
        return nullptr;
    }
  }
  return ident->decl()->e();
}  // namespace MiniZinc

void ChuffedSolverInstance::printStatistics() {
  auto total_time = std::chrono::duration_cast<duration>(chuffed_clock::now() - engine.start_time);
  duration search_time = total_time - engine.init_time;

  auto* solns2out = getSolns2Out();
  StatisticsStream ss(solns2out->getOutput(), solns2out->opt.flagEncapsulateJSON);
  ss.add("nodes", engine.nodes);
  ss.add("failures", engine.conflicts);
  ss.add("restarts", engine.restart_count);
  ss.add("variables", engine.vars.size() + sat.nVars());
  ss.add("intVars", engine.vars.size());
  ss.add("boolVariables", sat.nVars() - 2);  // Do not count constant True/False
  ss.add("propagators", engine.propagators.size());
  ss.add("propagations", engine.propagations);
  ss.add("peakDepth", engine.peak_depth);
  ss.add("nogoods", engine.conflicts);
  ss.add("backjumps", sat.back_jumps);
  ss.add("peakMem", memUsed());
  ss.add("time", to_sec(total_time));
  ss.add("initTime", to_sec(engine.init_time));
  ss.add("solveTime", to_sec(search_time));

  // Chuffed specific statistics
  if (!_isSatisfaction) {
    ss.add("objective", engine.best_sol);
    ss.add("optTime", to_sec(engine.opt_time));
  }
  ss.add("baseMem", engine.base_memory);
  ss.add("trailMem", static_cast<double>(engine.trail.capacity() * sizeof(TrailElem)) / 1048576.0);
  ss.add("randomSeed", so.rnd_seed);
}

#define CHUFFED_VERSION_TO_STRING(x) #x

std::string ChuffedSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return CHUFFED_VERSION_TO_STRING(CHUFFED_VERSION);
}

SolverInstanceBase::Options* ChuffedSolverFactory::createOptions() { return new ChuffedOptions; }

SolverInstanceBase* ChuffedSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                     SolverInstanceBase::Options* opt) {
  return new ChuffedSolverInstance(env, log, opt);
}

bool ChuffedSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                         std::vector<std::string>& argv,
                                         const std::string& workingDir) {
  auto* _opt = static_cast<ChuffedOptions*>(opt);
  CLOParser cop(i, argv);
  std::string ss;
  int nn = -1;
  double dd = -1;
  if (cop.get("-a")) {
    _opt->allSolutions = true;
  } else if (cop.get("-f --free-search")) {
    _opt->freeSearch = true;
  } else if (cop.get("-n --num-solutions", &nn)) {
    _opt->nrSolutions = nn;
  } else if (cop.get("-s --solver-statistics")) {
    _opt->statistics = true;
  } else if (cop.get("-t --solver-time-limit", &nn)) {
    _opt->time = std::chrono::milliseconds(nn);
  } else if (cop.get("-v --verbose-solving")) {
    _opt->verbose = true;
  } else if (cop.get("-r --seed --random-seed", &nn)) {
    _opt->randomSeed = nn;
  } else if (cop.get("--prop-fifo")) {
    so.prop_fifo = true;
  } else if (cop.get("--vsids")) {
    so.vsids = true;
  } else if (cop.get("--toggle-vsids")) {
    so.toggle_vsids = true;
  } else if (cop.get("--restart", &ss)) {
    if (ss == "chuffed") {
      so.restart_type = CHUFFED_DEFAULT;
    } else if (ss == "none") {
      so.restart_type = NONE;
    } else if (ss == "constant") {
      so.restart_type = CONSTANT;
    } else if (ss == "linear") {
      so.restart_type = LINEAR;
    } else if (ss == "luby") {
      so.restart_type = LUBY;
    } else if (ss == "geometric") {
      so.restart_type = GEOMETRIC;
    } else {
      return false;
    }
  } else if (cop.get("--restart-base", &dd)) {
    so.restart_base = dd;
  } else if (cop.get("--restart-scale", &nn)) {
    so.restart_scale = static_cast<unsigned int>(nn);
    so.restart_scale_override = false;
  } else if (cop.get("--switch-to-vsids-after", &nn)) {
    so.switch_to_vsids_after = nn;
  } else if (cop.get("--branch-random")) {
    so.branch_random = true;
  } else if (cop.get("--sat-polarity", &nn)) {
    so.sat_polarity = nn;
  } else if (cop.get("--lazy", &ss)) {
    if (ss == "on") {
      so.lazy = true;
    } else if (ss == "off") {
      so.lazy = false;
    } else {
      return false;
    }
  } else if (cop.get("--finesse", &ss)) {
    if (ss == "on") {
      so.finesse = true;
    } else if (ss == "off") {
      so.finesse = false;
    } else {
      return false;
    }
  } else if (cop.get("--learn", &ss)) {
    if (ss == "on") {
      so.learn = true;
    } else if (ss == "off") {
      so.learn = false;
    } else {
      return false;
    }
  } else if (cop.get("--eager-limit", &nn)) {
    so.eager_limit = nn;
  } else if (cop.get("--sat-var-limit", &nn)) {
    so.sat_var_limit = nn;
  } else if (cop.get("--n-of-learnts", &nn)) {
    so.nof_learnts = nn;
  } else if (cop.get("--learnts-mlimit", &nn)) {
    so.learnts_mlimit = nn;
  } else if (cop.get("--sort-learnt-level")) {
    so.sort_learnt_level = true;
  } else if (cop.get("--one-watch", &ss)) {
    if (ss == "on") {
      so.one_watch = true;
    } else if (ss == "off") {
      so.one_watch = false;
    } else {
      return false;
    }
  } else if (cop.get("--bin-clause-opt", &ss)) {
    if (ss == "on") {
      so.bin_clause_opt = true;
    } else if (ss == "off") {
      so.bin_clause_opt = false;
    } else {
      return false;
    }
  } else if (cop.get("--introduced-heuristic")) {
    so.introduced_heuristic = true;
  } else if (cop.get("--use-var-is-introduced")) {
    so.use_var_is_introduced = true;
  } else if (cop.get("--exclude-introduced")) {
    so.exclude_introduced = true;
  } else if (cop.get("--decide-introduced", &ss)) {
    if (ss == "on") {
      so.decide_introduced = true;
    } else if (ss == "off") {
      so.decide_introduced = false;
    } else {
      return false;
    }
  } else if (cop.get("--fd-simplify", &ss)) {
    if (ss == "on") {
      so.fd_simplify = true;
    } else if (ss == "off") {
      so.fd_simplify = false;
    } else {
      return false;
    }
  } else if (cop.get("--sat-simplify", &ss)) {
    if (ss == "on") {
      so.sat_simplify = true;
    } else if (ss == "off") {
      so.sat_simplify = false;
    } else {
      return false;
    }
  } else if (cop.get("--cumu-global", &ss)) {
    if (ss == "on") {
      so.cumu_global = true;
    } else if (ss == "off") {
      so.cumu_global = false;
    } else {
      return false;
    }
  } else if (cop.get("--disj-edge-find", &nn)) {
    if (ss == "on") {
      so.disj_edge_find = true;
    } else if (ss == "off") {
      so.disj_edge_find = false;
    } else {
      return false;
    }
  } else if (cop.get("--disj-set-bp", &nn)) {
    if (ss == "on") {
      so.disj_set_bp = true;
    } else if (ss == "off") {
      so.disj_set_bp = false;
    } else {
      return false;
    }
  } else if (cop.get("--mdd")) {
    so.mdd = true;
  } else if (cop.get("--mip")) {
    so.mip = true;
  } else if (cop.get("--mip-branch")) {
    so.mip_branch = true;
  } else if (cop.get("--sym-static")) {
    so.sym_static = true;
  } else if (cop.get("--ldsb")) {
    so.ldsb = true;
  } else if (cop.get("--ldsbta")) {
    so.ldsbta = true;
  } else if (cop.get("--ldsbad")) {
    so.ldsbad = true;
  } else if (cop.get("--sbps")) {
    so.sbps = true;
  } else {
    return false;
  }
  return true;
}

}  // namespace MiniZinc
