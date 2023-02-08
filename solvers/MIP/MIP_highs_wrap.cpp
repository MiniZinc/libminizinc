/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/exception.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solvers/MIP/MIP_highs_wrap.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#include <ostream>
#include <string>

using namespace MiniZinc;

#ifdef HIGHS_PLUGIN
#define load_symbol(plugin, name) load_symbol_dynamic(plugin, name)
#else
#define load_symbol(plugin, name) load_symbol_static(plugin, name)
#endif

HiGHSPlugin::HiGHSPlugin(const std::string& dll) {
#ifdef HIGHS_PLUGIN
  if (dll.empty()) {
    _inner = std::unique_ptr<Plugin>(new Plugin({
#ifdef _WIN32
        FileUtils::progpath() + "\\bin\\highs.dll", "highs"
#elif __APPLE__
        FileUtils::progpath() + "/../lib/libhighs.dylib", "libhighs"
#else
        FileUtils::progpath() + "/../lib/libhighs.so", "libhighs"
#endif
    }));
  } else {
    _inner = std::unique_ptr<Plugin>(new Plugin(dll));
  }
  auto& plugin = *_inner;
#endif
  load_symbol(plugin, Highs_create);
  load_symbol(plugin, Highs_destroy);
  load_symbol(plugin, Highs_version);
  load_symbol(plugin, Highs_getInfinity);
  load_symbol(plugin, Highs_getNumOptions);
  load_symbol(plugin, Highs_getOptionName);
  load_symbol(plugin, Highs_getOptionType);
  load_symbol(plugin, Highs_getBoolOptionValues);
  load_symbol(plugin, Highs_getIntOptionValues);
  load_symbol(plugin, Highs_getDoubleOptionValues);
  load_symbol(plugin, Highs_getStringOptionValues);
  load_symbol(plugin, Highs_setIntOptionValue);
  load_symbol(plugin, Highs_setDoubleOptionValue);
  load_symbol(plugin, Highs_setStringOptionValue);
  load_symbol(plugin, Highs_setBoolOptionValue);
  load_symbol(plugin, Highs_getNumRow);
  load_symbol(plugin, Highs_addRow);
  load_symbol(plugin, Highs_getNumCol);
  load_symbol(plugin, Highs_addCols);
  load_symbol(plugin, Highs_changeColsIntegralityByRange);
  load_symbol(plugin, Highs_changeObjectiveSense);
  load_symbol(plugin, Highs_changeColBounds);
  load_symbol(plugin, Highs_writeModel);
  load_symbol(plugin, Highs_run);
  load_symbol(plugin, Highs_getSolution);
  load_symbol(plugin, Highs_getModelStatus);
  load_symbol(plugin, Highs_getObjectiveValue);
  load_symbol(plugin, Highs_getIntInfoValue);
  load_symbol(plugin, Highs_getInt64InfoValue);
  load_symbol(plugin, Highs_getDoubleInfoValue);
  load_symbol(plugin, Highs_setCallback);
  load_symbol(plugin, Highs_startCallback);
  load_symbol(plugin, Highs_stopCallback);
}

std::string MIPHiGHSWrapper::getVersion(FactoryOptions& factoryOpt,
                                        MiniZinc::SolverInstanceBase::Options* /*opt*/) {
  try {
    HiGHSPlugin p(factoryOpt.highsDll);
    return p.Highs_version();
  } catch (PluginError&) {
    return "<unknown version>";
  }
}

std::string MIPHiGHSWrapper::getDescription(FactoryOptions& factoryOpt,
                                            MiniZinc::SolverInstanceBase::Options* opt) {
  std::ostringstream ss;
  ss << "MIP wrapper for HiGHS " << getVersion(factoryOpt, opt) << "\n  Compiled  "
     << ". Compiled  " __DATE__ "  " __TIME__;
  return ss.str();
}

std::vector<std::string> MIPHiGHSWrapper::getRequiredFlags(FactoryOptions& factoryOpt) {
  try {
    HiGHSPlugin p("");
    return {};
  } catch (PluginError&) {
    return {"--highs-dll"};
  }
}

void MIPHiGHSWrapper::Options::printHelp(std::ostream& os) {
  os << "HiGHS MIP wrapper options:" << std::endl
     << "  --writeModel <file>" << std::endl
     << "    write model to <file> (.mps)" << std::endl
     << "  -i" << std::endl
     << "    print intermediate solutions for optimization problems" << std::endl
     << "  -p <N>, --parallel <N>\n    use N threads, default: 1." << std::endl
     << "  -r <N>, --random-seed <N>\n    use random number generator seed N." << std::endl
     << "  --solver-time-limit <N>\n    stop search after N milliseconds" << std::endl
     << "  --absGap <n>\n    absolute gap |primal-dual| to stop" << std::endl
     << "  --relGap <n>\n    relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set "
        "<0 "
        "to use backend's default"
     << std::endl
     << "  --intTol <n>\n    integrality tolerance for a variable. Default 1e-8" << std::endl
     << "--highs-dll <file>   load the SCIP library from the given file (absolute path or file "
        "basename), default 'highs'"
     << std::endl;
}

bool MIPHiGHSWrapper::FactoryOptions::processOption(int& i, std::vector<std::string>& argv,
                                                    const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  return cop.get("--highs-dll", &highsDll);
}

bool MIPHiGHSWrapper::Options::processOption(int& i, std::vector<std::string>& argv,
                                             const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  std::string buffer;
  if (cop.get("-i")) {
    flagIntermediate = true;
  } else if (cop.get("-f --free-search")) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--writeModel", &buffer)) {
    sExportModel = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("-p --parallel", &nThreads)) {        // NOLINT: Allow repeated empty if
  } else if (cop.get("-r --random-seed", &randSeed)) {     // NOLINT: Allow repeated empty if
  } else if (cop.get("--solver-time-limit", &nTimeout)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--absGap", &absGap)) {               // NOLINT: Allow repeated empty if
  } else if (cop.get("--relGap", &relGap)) {               // NOLINT: Allow repeated empty if
  } else if (cop.get("--intTol", &intTol)) {               // NOLINT: Allow repeated empty if
  } else {
    return false;
  }
  return true;
}

std::vector<SolverConfig::ExtraFlag> MIPHiGHSWrapper::getExtraFlags(FactoryOptions& factoryOpt) {
  try {
    MIPHiGHSWrapper mhw(factoryOpt, nullptr);
    auto count = mhw._plugin->Highs_getNumOptions(mhw._highs);
    std::vector<MiniZinc::SolverConfig::ExtraFlag> res;
    res.reserve(count);
    for (auto i = 0; i < count; i++) {
      char* opt;
      checkHiGHSReturn(mhw._plugin->Highs_getOptionName(mhw._highs, i, &opt),
                       "Failed to get option name");
      HighsInt type;
      mhw._plugin->Highs_getOptionType(mhw._highs, opt, &type);
      MiniZinc::SolverConfig::ExtraFlag::FlagType t;
      std::string default_val;
      std::vector<std::string> range;
      switch (type) {
        case kHighsOptionTypeBool: {
          t = SolverConfig::ExtraFlag::FlagType::T_BOOL;
          HighsInt d;
          checkHiGHSReturn(mhw._plugin->Highs_getBoolOptionValues(mhw._highs, opt, nullptr, &d),
                           "Failed to get bool option values");
          default_val = d == 0 ? "false" : "true";
          break;
        }
        case kHighsOptionTypeInt: {
          t = SolverConfig::ExtraFlag::FlagType::T_INT;
          HighsInt d;
          HighsInt lb;
          HighsInt ub;
          checkHiGHSReturn(
              mhw._plugin->Highs_getIntOptionValues(mhw._highs, opt, nullptr, &lb, &ub, &d),
              "Failed to get int option values");
          default_val = std::to_string(d);
          range = {std::to_string(lb), std::to_string(ub)};
          break;
        }
        case kHighsOptionTypeDouble: {
          t = SolverConfig::ExtraFlag::FlagType::T_FLOAT;
          double d;
          double lb;
          double ub;
          checkHiGHSReturn(
              mhw._plugin->Highs_getDoubleOptionValues(mhw._highs, opt, nullptr, &lb, &ub, &d),
              "Failed to get double option values");
          default_val = std::to_string(d);
          range = {std::to_string(lb), std::to_string(ub)};
          break;
        }
        case kHighsOptionTypeString: {
          t = SolverConfig::ExtraFlag::FlagType::T_STRING;
          char d[255];
          checkHiGHSReturn(
              mhw._plugin->Highs_getStringOptionValues(mhw._highs, opt, nullptr, &d[0]),
              "Failed to get string option values");
          default_val = d;
          break;
        }
        default:
          // Unknown option type
          assert(false);
      }
      auto name = std::string("--highs-") + opt;
      // TODO: Get option description when available in C API
      res.emplace_back(name, opt, t, std::move(range), std::move(default_val));
      free(opt);
    }
    return res;
  } catch (PluginError&) {
    return {};
  }
  return {};
}

/// Add new variables to the solver
void MIPHiGHSWrapper::doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                                std::string* names) {
  HighsInt cur = _plugin->Highs_getNumCol(_highs);
  checkHiGHSReturn(_plugin->Highs_addCols(_highs, static_cast<const HighsInt>(n), obj, lb, ub, 0,
                                          nullptr, nullptr, nullptr),
                   "failed to add new variables");
  assert(cur + n == _plugin->Highs_getNumCol(_highs));
  std::vector<HighsInt> types;
  types.reserve(n);
  for (int i = 0; i < n; ++i) {
    switch (vt[i]) {
      case VarType::REAL: {
        types.push_back(kHighsVarTypeContinuous);
        break;
      }
      case VarType::BINARY:  // fall through
      case VarType::INT: {
        types.push_back(kHighsVarTypeInteger);
        break;
      }
    }
  }
  checkHiGHSReturn(_plugin->Highs_changeColsIntegralityByRange(
                       _highs, cur, _plugin->Highs_getNumCol(_highs) - 1, types.data()),
                   "unable to set integrality constraint");
}

/// Add a linear constraint
void MIPHiGHSWrapper::addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
                             int mask, const std::string& rowName) {
  /// Convert linear constraint type
  double rlb = rhs;
  double rub = rhs;
  switch (sense) {
    case LQ:
      rlb = -_plugin->Highs_getInfinity(_highs);
      break;
    case EQ:
      break;
    case GQ:
      rub = _plugin->Highs_getInfinity(_highs);
      break;
    default:
      throw MiniZinc::InternalError("MIPWrapper: unknown constraint type");
  }
  auto res = _plugin->Highs_addRow(_highs, rlb, rub, nnz, rmatind, rmatval);
  checkHiGHSReturn(res, "HiGHS Error: Unable to add linear constraint");
}

void MIPHiGHSWrapper::solve() {
  setOptions();

  output.status = Status::UNKNOWN;
  output.dWallTime0 = std::chrono::steady_clock::now();
  output.cCPUTime0 = std::clock();
  output.nCols = static_cast<int>(colObj.size());
  _x.resize(output.nCols);

  // Actually solve the current model
  checkHiGHSReturn(_plugin->Highs_run(_highs), "unable to solve model");

  output.dWallTime =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - output.dWallTime0).count();
  output.dCPUTime = double(std::clock() - output.cCPUTime0) / CLOCKS_PER_SEC;

  auto status = _plugin->Highs_getModelStatus(_highs);
  output.status = convertStatus(status);

  output.objVal = _plugin->Highs_getObjectiveValue(_highs);
  checkHiGHSReturn(_plugin->Highs_getDoubleInfoValue(_highs, "mip_dual_bound", &output.bestBound),
                   "failed to get mip_dual_bound");
  int64_t nNodes = output.nNodes;
  checkHiGHSReturn(_plugin->Highs_getInt64InfoValue(_highs, "mip_node_count", &nNodes),
                   "failed to get mip_node_count");
  output.nNodes = static_cast<int>(nNodes);

  if (output.status == MIPWrapper::OPT || output.status == MIPWrapper::SAT) {
    checkHiGHSReturn(_plugin->Highs_getSolution(_highs, _x.data(), nullptr, nullptr, nullptr),
                     "Failed to get solution");
    output.x = _x.data();
    if (cbui.solcbfn != nullptr) {
      cbui.solcbfn(output, cbui.psi);
    }
  }

  if (fVerbose) {
    checkHiGHSReturn(_plugin->Highs_stopCallback(_highs, kHighsCallbackLogging),
                     "Unable to stop logging callback");
  }
  if (_options->flagIntermediate) {
    checkHiGHSReturn(_plugin->Highs_stopCallback(_highs, kHighsCallbackMipImprovingSolution),
                     "Unable to stop solution callback");
  }
}

MIPWrapper::Status MIPHiGHSWrapper::convertStatus(const HighsInt model_status) {
  int primal_solution_status;
  checkHiGHSReturn(
      _plugin->Highs_getIntInfoValue(_highs, "primal_solution_status", &primal_solution_status),
      "failed to get primal_solution_status");
  bool valid = primal_solution_status == kHighsSolutionStatusFeasible;
  switch (model_status) {
    case kHighsModelStatusNotset:
      output.statusName = "Not set";
      return MIPWrapper::UNKNOWN;
    case kHighsModelStatusLoadError:
      output.statusName = "Load error";
      return MIPWrapper::ERROR_STATUS;
    case kHighsModelStatusModelError:
      output.statusName = "Model error";
      return MIPWrapper::ERROR_STATUS;
    case kHighsModelStatusPresolveError:
      output.statusName = "Presolve error";
      return MIPWrapper::ERROR_STATUS;
    case kHighsModelStatusSolveError:
      output.statusName = "Solve error";
      return MIPWrapper::ERROR_STATUS;
    case kHighsModelStatusPostsolveError:
      output.statusName = "Postsolve error";
      return MIPWrapper::ERROR_STATUS;
    case kHighsModelStatusModelEmpty:
      output.statusName = "Model empty";
      return MIPWrapper::ERROR_STATUS;
    case kHighsModelStatusOptimal:
      output.statusName = "Optimal";
      return MIPWrapper::OPT;
    case kHighsModelStatusInfeasible:
      output.statusName = "Infeasible";
      return MIPWrapper::UNSAT;
    case kHighsModelStatusUnboundedOrInfeasible:
      output.statusName = "Unbounded or infeasible";
      return MIPWrapper::UNSATorUNBND;
    case kHighsModelStatusUnbounded:
      output.statusName = "Unbounded";
      return MIPWrapper::UNBND;
    case kHighsModelStatusObjectiveBound:
      output.statusName = "Objective bound";
      return valid ? MIPWrapper::OPT : MIPWrapper::UNSAT;
    case kHighsModelStatusObjectiveTarget:
      output.statusName = "Objective target";
      return valid ? MIPWrapper::OPT : MIPWrapper::UNSAT;
    case kHighsModelStatusTimeLimit:
      output.statusName = "Time limit";
      return valid ? MIPWrapper::SAT : MIPWrapper::UNKNOWN;
    case kHighsModelStatusIterationLimit:
      output.statusName = "Iteration limit";
      return valid ? MIPWrapper::SAT : MIPWrapper::UNKNOWN;
    case kHighsModelStatusUnknown:
      output.statusName = "Unknown";
      return MIPWrapper::UNKNOWN;
    case kHighsModelStatusSolutionLimit:
      output.statusName = "Solution limit";
      return valid ? MIPWrapper::SAT : MIPWrapper::UNKNOWN;
    case kHighsModelStatusInterrupt:
      output.statusName = "Interrupt";
      return valid ? MIPWrapper::SAT : MIPWrapper::UNKNOWN;
    default:
      throw InternalError("Unknown HiGHS status");
  }
}

void log_to_stderr(HighsInt ty, const char* str, void* stream) {
  auto* out = static_cast<std::ostream*>(stream);
  *out << str;
}

void MIPHiGHSWrapper::setOptions() {
  if (_options->nThreads > 0) {
    checkHiGHSReturn(_plugin->Highs_setIntOptionValue(_highs, "threads", _options->nThreads),
                     "unable to set number of threads");
    checkHiGHSReturn(_plugin->Highs_setStringOptionValue(_highs, "parallel", "on"),
                     "unable to enable parallel mode");
  } else {
    checkHiGHSReturn(_plugin->Highs_setStringOptionValue(_highs, "parallel", "off"),
                     "unable to disable parallel mode");
  }
  if (_options->nTimeout > 0) {
    checkHiGHSReturn(_plugin->Highs_setDoubleOptionValue(
                         _highs, "time_limit", static_cast<double>(_options->nTimeout) / 1000.0),
                     "unable to time limit");
  }
  if (_options->randSeed >= 0) {
    checkHiGHSReturn(_plugin->Highs_setIntOptionValue(_highs, "random_seed", _options->randSeed),
                     "unable to set random seed");
  }
  if (_options->absGap >= 0.0) {
    checkHiGHSReturn(_plugin->Highs_setDoubleOptionValue(_highs, "mip_abs_gap", _options->absGap),
                     "unable to set absolute gap");
  }
  if (_options->relGap >= 0.0) {
    checkHiGHSReturn(_plugin->Highs_setDoubleOptionValue(_highs, "mip_rel_gap", _options->relGap),
                     "unable to set relative gap");
  }
  if (_options->intTol >= 0.0) {
    checkHiGHSReturn(
        _plugin->Highs_setDoubleOptionValue(_highs, "mip_feasibility_tolerance", _options->intTol),
        "unable to set integer tolerance");
  }
  if (!_options->sExportModel.empty()) {
    checkHiGHSReturn(_plugin->Highs_writeModel(_highs, _options->sExportModel.c_str()),
                     "Unable to write model to file");
  }
  checkHiGHSReturn(
      _plugin->Highs_setBoolOptionValue(_highs, "log_to_console", static_cast<HighsInt>(fVerbose)),
      "Unable to set verbosity");

  checkHiGHSReturn(_plugin->Highs_setCallback(_highs, MIPHiGHSWrapper::callback, &cbui),
                   "Unable to set callback");
  if (fVerbose) {
    checkHiGHSReturn(_plugin->Highs_startCallback(_highs, kHighsCallbackLogging),
                     "Unable to start logging callback");
  }
  if (_options->flagIntermediate) {
    checkHiGHSReturn(_plugin->Highs_startCallback(_highs, kHighsCallbackMipImprovingSolution),
                     "Unable to start solution callback");
  }

  for (const auto& it : _options->extraParams) {
    std::string name = it.first.substr(8);
    HighsInt type;
    checkHiGHSReturn(_plugin->Highs_getOptionType(_highs, name.c_str(), &type),
                     "Unable to find type of option `" + name + "'");
    switch (type) {
      case kHighsOptionTypeBool: {
        assert(it.second == "true" || it.second == "false");
        checkHiGHSReturn(_plugin->Highs_setBoolOptionValue(
                             _highs, name.c_str(), static_cast<HighsInt>(it.second == "true")),
                         "unable to set HiGHS option `" + name + "'");
        break;
      }
      case kHighsOptionTypeInt: {
        checkHiGHSReturn(_plugin->Highs_setIntOptionValue(_highs, name.c_str(), stoi(it.second)),
                         "unable to set HiGHS option `" + name + "'");
        break;
      }
      case kHighsOptionTypeDouble: {
        checkHiGHSReturn(_plugin->Highs_setDoubleOptionValue(_highs, name.c_str(), stod(it.second)),
                         "unable to set HiGHS option `" + name + "'");
        break;
      }
      case kHighsOptionTypeString: {
        checkHiGHSReturn(
            _plugin->Highs_setStringOptionValue(_highs, name.c_str(), it.second.c_str()),
            "unable to set HiGHS option `" + name + "'");
        break;
      }
      default:
        throw InternalError("Unknown HiGHS Option type");
    }
  }
}

void MIPHiGHSWrapper::callback(const int callback_type, const char* message,
                               const struct HighsCallbackDataOut* data_out,
                               struct HighsCallbackDataIn* data_in, void* user_callback_data) {
  auto* info = (MIPWrapper::CBUserInfo*)user_callback_data;
  auto* hw = static_cast<MIPHiGHSWrapper*>(info->wrapper);
  switch (callback_type) {
    case kHighsCallbackLogging:
      std::cerr << message;
      break;
    case kHighsCallbackMipImprovingSolution:
      hw->output.dWallTime =
          std::chrono::duration<double>(std::chrono::steady_clock::now() - hw->output.dWallTime0)
              .count();
      hw->output.dCPUTime = double(std::clock() - hw->output.cCPUTime0) / CLOCKS_PER_SEC;
      hw->output.status = Status::SAT;
      hw->output.statusName = "Unknown";
      hw->output.objVal = data_out->objective_function_value;
      hw->output.bestBound = data_out->mip_dual_bound;
      hw->output.nNodes = static_cast<int>(data_out->mip_node_count);
      hw->_x.assign(data_out->mip_solution, data_out->mip_solution + hw->output.nCols);
      hw->output.x = hw->_x.data();
      if (hw->_options->flagIntermediate && info->solcbfn != nullptr) {
        (info->solcbfn)(*info->pOutput, info->psi);
        info->printed = true;
      }
      break;
    default:
      break;
  }
}