
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This (main) file coordinates flattening and solving.
 * The corresponding modules are flexibly plugged in
 * as derived classes, prospectively from DLLs.
 * A flattening module should provide MinZinc::GetFlattener()
 * A solving module should provide an object of a class derived from SolverFactory.
 * Need to get more flexible for multi-pass & multi-solving stuff  TODO
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/param_config.hh>
#include <minizinc/solver.hh>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <sstream>

#ifdef HAS_OSICBC
#include <minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh>
#endif
#ifdef HAS_XPRESS
#include <minizinc/solvers/MIP/MIP_xpress_solverfactory.hh>
#endif
#ifdef HAS_GECODE
#include <minizinc/solvers/gecode_solverfactory.hh>
#endif
#ifdef HAS_GEAS
#include <minizinc/solvers/geas_solverfactory.hh>
#endif
#ifdef HAS_ATLANTIS
#include <minizinc/solvers/atlantis_solverfactory.hh>
#endif
#ifdef HAS_CHUFFED
#include <minizinc/solvers/chuffed_solverfactory.hh>
#endif
#ifdef HAS_SCIP
#include <minizinc/solvers/MIP/MIP_scip_solverfactory.hh>
#endif
#ifdef HAS_CPLEX
#include <minizinc/solvers/MIP/MIP_cplex_solverfactory.hh>
#endif
#ifdef HAS_GUROBI
#include <minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh>
#endif
#ifdef HAS_HIGHS
#include <minizinc/solvers/MIP/MIP_highs_solverfactory.hh>
#endif
#include <minizinc/exception.hh>
#include <minizinc/solvers/fzn_solverfactory.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>
#include <minizinc/solvers/mzn_solverfactory.hh>
#include <minizinc/solvers/mzn_solverinstance.hh>
#include <minizinc/solvers/nl/nl_solverfactory.hh>
#include <minizinc/solvers/nl/nl_solverinstance.hh>

using namespace std;
using namespace MiniZinc;

SolverInitialiser::SolverInitialiser() {
#ifdef HAS_OSICBC
  static OSICBCSolverFactoryInitialiser _osicbc_init;
#endif
#ifdef HAS_XPRESS
  static XpressSolverFactoryInitialiser _xpress_init;
#endif
#ifdef HAS_GECODE
  static GecodeSolverFactoryInitialiser _gecode_init;
#endif
#ifdef HAS_GEAS
  static GeasSolverFactoryInitialiser _geas_init;
#endif
#ifdef HAS_ATLANTIS
  static AtlantisSolverFactoryInitialiser _atlantis_init;
#endif
#ifdef HAS_CHUFFED
  static ChuffedSolverFactoryInitialiser _chuffed_init;
#endif
#ifdef HAS_SCIP
  static SCIPSolverFactoryInitialiser _scip_init;
#endif
#ifdef HAS_CPLEX
  static CplexSolverFactoryInitialiser _cplex_init;
#endif
  static FZNSolverFactoryInitialiser _fzn_init;
#ifdef HAS_GUROBI
  static GurobiSolverFactoryInitialiser _gurobi_init;
#endif
#ifdef HAS_HIGHS
  static HiGHSSolverFactoryInitialiser _highs_init;
#endif
  static MZNSolverFactoryInitialiser _mzn_init;
  static NLSolverFactoryInitialiser _nl_init;
}

MZNFZNSolverFlag MZNFZNSolverFlag::std(const std::string& n0) {
  const std::string argFlags("-I -n -p -r -n-o");
  if (argFlags.find(n0) != std::string::npos) {
    return MZNFZNSolverFlag(FT_ARG, n0);
  }
  return MZNFZNSolverFlag(FT_NOARG, n0);
}

MZNFZNSolverFlag MZNFZNSolverFlag::extra(const SolverConfig::ExtraFlag& ef) {
  return MZNFZNSolverFlag(
      ef.flagType == SolverConfig::ExtraFlag::FlagType::T_BOOL && ef.range.empty() ? FT_NOARG
                                                                                   : FT_ARG,
      ef.flag);
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SolverRegistry* MiniZinc::get_global_solver_registry() {
  static SolverRegistry sr;
  return &sr;
}

void SolverRegistry::addSolverFactory(SolverFactory* pSF) {
  assert(pSF);
  _sfstorage.push_back(pSF);
}

void SolverRegistry::removeSolverFactory(SolverFactory* pSF) {
  auto it = find(_sfstorage.begin(), _sfstorage.end(), pSF);
  assert(pSF);
  _sfstorage.erase(it);
}

void SolverRegistry::addFactoryFlag(const std::string& flag, SolverFactory* sf) {
  assert(sf);
  _factoryFlagStorage.emplace_back(flag, sf);
}

void SolverRegistry::removeFactoryFlag(const std::string& flag, SolverFactory* sf) {
  assert(sf);
  auto it = find(_factoryFlagStorage.begin(), _factoryFlagStorage.end(), std::make_pair(flag, sf));
  _factoryFlagStorage.erase(it);
}

/// Function createSI also adds each SI to the local storage
SolverInstanceBase* SolverFactory::createSI(Env& env, std::ostream& log,
                                            SolverInstanceBase::Options* opt) {
  SolverInstanceBase* pSI = doCreateSI(env, log, opt);
  if (pSI == nullptr) {
    throw InternalError("SolverFactory: failed to initialize solver " + getDescription());
  }
  _sistorage.resize(_sistorage.size() + 1);
  _sistorage.back().reset(pSI);
  return pSI;
}

/// also providing a destroy function for a DLL or just special allocator etc.
void SolverFactory::destroySI(SolverInstanceBase* pSI) {
  auto it = _sistorage.begin();
  for (; it != _sistorage.end(); ++it) {
    if (it->get() == pSI) {
      break;
    }
  }
  if (_sistorage.end() == it) {
    std::stringstream ss;
    ss << "  SolverFactory: failed to remove solver at " << pSI;
    throw InternalError(ss.str());
  }
  _sistorage.erase(it);
}

MznSolver::MznSolver(std::ostream& os0, std::ostream& log0, const Timer& startTime)
    : _startTime(startTime),
      _solverConfigs(log0),
      _flt(os0, log0, _solverConfigs.mznlibDir()),
      _executableName("<executable>"),
      _os(os0),
      _log(log0),
      s2out(os0, log0, _solverConfigs.mznlibDir()) {}

MznSolver::~MznSolver() {
  //   if (si)                         // first the solver
  //     CleanupSolverInterface(si);
  // TODO cleanup the used solver interfaces
  _si = nullptr;
  _siOpt = nullptr;
  GC::trigger();
}

bool MznSolver::ifMzn2Fzn() const { return _isMzn2fzn; }

bool MznSolver::ifSolns2out() const { return s2out.opt.flagStandaloneSolns2Out; }

void MznSolver::addSolverInterface(SolverFactory* sf) {
  _si = sf->createSI(*_flt.getEnv(), _log, _siOpt);
  assert(_si);
  if (s2out.getEnv() == nullptr) {
    s2out.initFromEnv(_flt.getEnv());
  }
  _si->setSolns2Out(&s2out);
  if (flagCompilerVerbose) {
    _log
        //     << "  ---------------------------------------------------------------------------\n"
        << "      % SOLVING PHASE\n"
        << sf->getDescription(_siOpt) << endl;
  }
}

void MznSolver::addSolverInterface() {
  GCLock lock;
  if (_sf == nullptr) {
    if (get_global_solver_registry()->getSolverFactories().empty()) {
      _log << " MznSolver: NO SOLVER FACTORIES LINKED." << endl;
      assert(0);
    }
    _sf = get_global_solver_registry()->getSolverFactories().back();
  }
  addSolverInterface(_sf);
}

void MznSolver::printUsage(std::ostream& os) {
  os << _executableName << ": ";
  if (ifMzn2Fzn()) {
    os << "MiniZinc to FlatZinc converter.\n"
       << "Usage: " << _executableName
       << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl;
  } else if (ifSolns2out()) {
    os << "Solutions to output translator.\n"
       << "Usage: " << _executableName << "  [<options>] <model>.ozn" << std::endl;
  } else {
    os << "MiniZinc driver.\n"
       << "Usage: " << _executableName
       << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...] or just <flat>.fzn"
       << std::endl;
  }
}

void MznSolver::printHelp(std::ostream& os, const std::string& selectedSolver) {
  printUsage(os);
  os << "General options:" << std::endl
     << "  --help, -h\n    Print this help message." << std::endl
     << "  --version\n    Print version information." << std::endl
     << "  --solvers\n    Print list of available solvers." << std::endl
     << "  --time-limit <ms>\n    Stop after <ms> milliseconds (includes compilation and solving)."
     << std::endl
     << "  --solver <solver id>, --solver <solver config file>.msc, --solver default\n    Select "
        "solver to use, or explicitly specify using the default solver."
     << std::endl
     << "  --help <solver id>\n    Print help for a particular solver." << std::endl
     << "  -v, -l, --verbose\n    Print progress/log statements. Note that some solvers may log "
        "to "
        "stdout."
     << std::endl
     << "  --verbose-compilation\n    Print progress/log statements for compilation." << std::endl
     << "  -s, --statistics\n    Print statistics." << std::endl
     << "  --compiler-statistics\n    Print statistics for compilation." << std::endl
     << "  -c, --compile\n    Compile only (do not run solver)." << std::endl
     << "  --config-dirs\n    Output configuration directories." << std::endl
     << "  --param-file <file>\n    Load parameters from the given JSON file." << std::endl
     << "  --json-stream\n    Print output as newline-delimited JSON message objects." << std::endl;

  if (selectedSolver.empty()) {
    _flt.printHelp(os);
    os << endl;
    if (!ifMzn2Fzn()) {
      Solns2Out::printHelp(os);
      os << endl;
    }
    os << "Available solvers (get help using --help <solver id>):" << endl;
    std::vector<std::string> solvers = _solverConfigs.solvers();
    if (solvers.empty()) {
      os << "  none.\n";
    }
    for (auto& solver : solvers) {
      os << "  " << solver << endl;
    }
  } else {
    const SolverConfig& sc = _solverConfigs.config(selectedSolver);
    string solverId;
    if (sc.executable().empty()) {
      solverId = sc.id();
    } else {
      switch (sc.inputType()) {
        case SolverConfig::O_FZN:
        case SolverConfig::O_JSON:
          solverId = "org.minizinc.mzn-fzn";
          break;
        case SolverConfig::O_MZN:
          solverId = "org.minizinc.mzn-mzn";
          break;
        case SolverConfig::O_NL:
          solverId = "org.minizinc.mzn-nl";
          break;
      }
    }
    bool found = false;
    for (auto it = get_global_solver_registry()->getSolverFactories().rbegin();
         it != get_global_solver_registry()->getSolverFactories().rend(); ++it) {
      if ((*it)->getId() == solverId) {
        os << endl;
        (*it)->printHelp(_os);
        if (!sc.executable().empty() && !sc.extraFlags().empty()) {
          os << "Extra solver flags (use with ";
          switch (sc.inputType()) {
            case SolverConfig::O_FZN:
            case SolverConfig::O_JSON:
              os << "--fzn-flags";
              break;
            case SolverConfig::O_MZN:
              os << "--mzn-flags";
              break;
            case SolverConfig::O_NL:
              os << "--nl-flags";
              break;
          }
          os << ")" << endl;
          for (const SolverConfig::ExtraFlag& ef : sc.extraFlags()) {
            os << "  " << ef.flag << endl << "    " << ef.description << endl;
          }
        }
        found = true;
      }
    }
    if (!found) {
      os << "No help found for solver " << selectedSolver << endl;
    }
  }
}

void add_flags(const std::string& sep, const std::vector<std::string>& in_args,
               std::vector<std::string>& out_args) {
  for (const std::string& arg : in_args) {
    out_args.push_back(sep);
    out_args.push_back(arg);
  }
}

MznSolver::OptionStatus MznSolver::processOptions(std::vector<std::string>& argv) {
  std::ostringstream cmdline_ss;
  for (const auto& arg : argv) {
    cmdline_ss << arg << " ";
  }

  _executableName = argv[0];
  _executableName = _executableName.substr(_executableName.find_last_of("/\\") + 1);
  size_t lastdot = _executableName.find_last_of('.');
  if (lastdot != std::string::npos) {
    _executableName = _executableName.substr(0, lastdot);
  }
  string solver;
  bool load_params = false;
  bool mzn2fzn_exe = (_executableName == "mzn2fzn");
  if (mzn2fzn_exe) {
    _isMzn2fzn = true;
  } else if (_executableName == "solns2out") {
    s2out.opt.flagStandaloneSolns2Out = true;
    flagIsSolns2out = true;
  }
  bool compileSolutionChecker = false;
  int i = 1;
  int j = 1;
  int argc = static_cast<int>(argv.size());
  std::vector<std::string> workingDirs = {""};
  if (argc < 2) {
    throw BadOption();
  }

  // Add params from a file if necessary
  std::vector<std::string> paramFiles;
  for (i = 1; i < argc; ++i) {
    string paramFile;
    bool usedFlag = false;
    bool pushWorkingDir = true;

    if (argv[i] == "--param-file") {
      usedFlag = true;
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --param-file");
      }
      paramFile = argv[i];
    } else if (argv[i] == "--param-file-no-push") {
      usedFlag = true;
      pushWorkingDir = false;
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --param-file-no-push");
      }
      paramFile = argv[i];
    } else if (argv[i] == "--push-working-directory") {
      ++i;
      workingDirs.push_back(argv[i]);
    } else if (argv[i] == "--pop-working-directory") {
      workingDirs.pop_back();
    } else {
      size_t last_dot = argv[i].find_last_of('.');
      if (last_dot != string::npos && argv[i].substr(last_dot, string::npos) == ".mpc") {
        paramFile = argv[i];
      }
    }

    if (!paramFile.empty()) {
      auto paramFilePath = FileUtils::file_path(paramFile, workingDirs.back());
      if (std::find(paramFiles.begin(), paramFiles.end(), paramFilePath) != paramFiles.end()) {
        throw ParamException("Cyclic parameter configuration file");
      }
      // add parameter file arguments
      ParamConfig pc;
      pc.blacklist(
          {"--solvers", "--solvers-json", "--solver-json", "--help", "-h", "--config-dirs"});
      pc.negatedFlag("-i", "-n-i");
      pc.negatedFlag("--intermediate", "--no-intermediate");
      pc.negatedFlag("--intermediate-solutions", "--no-intermediate-solutions");
      pc.negatedFlag("--all-satisfaction", "--disable-all-satisfaction");
      pc.load(paramFilePath);

      // Insert the new options
      auto toInsert = pc.argv();
      auto remove = argv.begin() + (usedFlag ? i - 1 : i);
      auto position = argv.erase(remove, argv.begin() + i + 1);
      if (pushWorkingDir) {
        position =
            argv.insert(position, {"--push-working-directory",
                                   FileUtils::file_path(FileUtils::dir_name(paramFilePath))}) +
            2;
      }
      position = argv.insert(position, toInsert.begin(), toInsert.end()) +
                 static_cast<int>(toInsert.size());
      if (pushWorkingDir) {
        position = argv.insert(position, "--pop-working-directory") + 1;
      }
      paramFiles.push_back(paramFilePath);
      argc = static_cast<int>(argv.size());

      // Have to process the newly added options
      if (usedFlag) {
        i -= 2;
      } else {
        i--;
      }
    }
  }

  // Process any registered factory flags
  const auto& factoryFlags = get_global_solver_registry()->getFactoryFlags();
  std::unordered_map<std::string, std::vector<std::string>> reducedSolverDefaults;
  if (!factoryFlags.empty()) {
    // Process solver default factory flags
    for (const auto& factoryFlag : factoryFlags) {
      auto factoryId = factoryFlag.second->getId();
      if (reducedSolverDefaults.count(factoryId) == 0) {
        reducedSolverDefaults.insert({factoryId, _solverConfigs.defaultOptions(factoryId)});
      }
      auto& defaultArgs = reducedSolverDefaults.find(factoryId)->second;
      std::vector<std::string> keep;
      for (i = 0; i < defaultArgs.size(); i++) {
        if (defaultArgs[i] != factoryFlag.first ||
            !factoryFlag.second->processFactoryOption(i, defaultArgs)) {
          keep.push_back(defaultArgs[i]);
        }
      }
      defaultArgs = keep;
    }
    // Process command line factory flags
    std::vector<std::string> remaining = {argv[0]};
    for (i = 1; i < argc; i++) {
      bool ok = false;
      if (argv[i] == "--push-working-directory") {
        remaining.push_back(argv[i]);
        i++;
        workingDirs.push_back(argv[i]);
      } else if (argv[i] == "--pop-working-directory") {
        workingDirs.pop_back();
      } else {
        for (const auto& factoryFlag : factoryFlags) {
          if (argv[i] == factoryFlag.first &&
              factoryFlag.second->processFactoryOption(i, argv, workingDirs.back())) {
            ok = true;
            break;
          }
        }
      }
      if (!ok) {
        remaining.push_back(argv[i]);
      }
    }
    argv = remaining;
    argc = static_cast<int>(remaining.size());
  }

  // After this point all solver configurations must be available
  _solverConfigs.populate(_log);

  for (i = 1; i < argc; ++i) {
    if (argv[i] == "--backend-flags") {
      // Ensure that we don't consume the argument to --backend-flags
      argv[j++] = argv[i++];
      argv[j++] = argv[i];
      continue;
    }
    if (argv[i] == "-h" || argv[i] == "--help") {
      if (argc > i + 1) {
        printHelp(_os, argv[i + 1]);
      } else {
        printHelp(_os);
      }
      return OPTION_FINISH;
    }
    if (argv[i] == "--version") {
      Flattener::printVersion(_os);
      return OPTION_FINISH;
    }
    if (argv[i] == "--solvers") {
      for (auto* sf : get_global_solver_registry()->getSolverFactories()) {
        sf->finaliseSolverConfigs(_solverConfigs);
      }
      _os << "MiniZinc driver.\nAvailable solver configurations:\n";
      std::vector<std::string> solvers = _solverConfigs.solvers();
      if (solvers.empty()) {
        _os << "  none.\n";
      }
      for (auto& solver : solvers) {
        _os << "  " << solver << endl;
      }
      _os << "Search path for solver configurations:\n";
      for (const string& p : _solverConfigs.solverConfigsPath()) {
        _os << "  " << p << endl;
      }
      return OPTION_FINISH;
    }
    if (argv[i] == "--solvers-json") {
      for (auto* sf : get_global_solver_registry()->getSolverFactories()) {
        sf->finaliseSolverConfigs(_solverConfigs);
      }
      _os << _solverConfigs.solverConfigsJSON();
      return OPTION_FINISH;
    }
    if (argv[i] == "--solver-json") {
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --solver-json");
      }
      if (!solver.empty() && solver != argv[i]) {
        throw BadOption("Only one --solver-json option allowed");
      }
      solver = argv[i];
      const SolverConfig& sc = _solverConfigs.config(solver);
      for (auto* sf : get_global_solver_registry()->getSolverFactories()) {
        if (sf->getId() == sc.id()) {
          sf->finaliseSolverConfigs(_solverConfigs);
        }
      }
      _os << sc.toJSON(_solverConfigs);
      return OPTION_FINISH;
    }
    if (argv[i] == "--config-dirs") {
      GCLock lock;
      _os << "{\n";
      _os << "  \"globalConfigFile\" : \""
          << Printer::escapeStringLit(FileUtils::global_config_file()) << "\",\n";
      _os << "  \"userConfigFile\" : \"" << Printer::escapeStringLit(FileUtils::user_config_file())
          << "\",\n";
      _os << "  \"userSolverConfigDir\" : \""
          << Printer::escapeStringLit(FileUtils::user_config_dir()) << "/solvers\",\n";
      _os << "  \"mznStdlibDir\" : \"" << Printer::escapeStringLit(_solverConfigs.mznlibDir())
          << "\"\n";
      _os << "}\n";
      return OPTION_FINISH;
    }
    if (argv[i] == "--time-limit") {
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --time-limit");
      }
      flagOverallTimeLimit = std::chrono::milliseconds(atoi(argv[i].c_str()));
    } else if (argv[i] == "-t" || argv[i] == "--solver-time-limit" ||
               argv[i] == "--fzn-time-limit") {
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --solver-time-limit");
      }
      flagSolverTimeLimit = std::chrono::milliseconds(atoi(argv[i].c_str()));
    } else if (argv[i] == "--solver") {
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --solver");
      }
      if (!solver.empty() && solver != argv[i]) {
        throw BadOption("Only one --solver option allowed");
      }
      solver = argv[i];
    } else if (argv[i] == "-c" || argv[i] == "--compile") {
      _isMzn2fzn = true;
    } else if (argv[i] == "-v" || argv[i] == "--verbose" || argv[i] == "-l") {
      flagVerbose = true;
      flagCompilerVerbose = true;
    } else if (argv[i] == "--verbose-compilation") {
      flagCompilerVerbose = true;
    } else if (argv[i] == "-s" || argv[i] == "--statistics") {
      flagStatistics = true;
      flagCompilerStatistics = true;
    } else if (argv[i] == "--solver-statistics") {
      flagStatistics = true;
    } else if (argv[i] == "-r" || argv[i] == "--seed" || argv[i] == "--random-seed") {
      ++i;
      if (i == argc) {
        throw BadOption("Argument required for --random-seed");
      }
      flagRandomSeed = true;
      randomSeed = atoi(argv[i].c_str());
    } else if (argv[i] == "--compiler-statistics") {
      flagCompilerStatistics = true;
    } else if (argv[i] == "--json-stream") {
      flagEncapsulateJSON = true;
      s2out.opt.checkerArgs.emplace_back("--json-stream");
    } else if (argv[i] == "--only-sections" || argv[i] == "--not-sections") {
      argv[j++] = argv[i];
      s2out.opt.checkerArgs.push_back(argv[i++]);
      argv[j++] = argv[i];
      s2out.opt.checkerArgs.push_back(argv[i]);
    } else {
      if ((argv[i] == "--fzn-cmd" || argv[i] == "--flatzinc-cmd") && solver.empty()) {
        solver = "org.minizinc.mzn-fzn";
      }
      if (argv[i] == "--compile-solution-checker") {
        compileSolutionChecker = true;
      }
      if (argv[i] == "--ozn-file") {
        flagIsSolns2out = true;
      }
      argv[j++] = argv[i];
    }
  }
  argv.resize(j);
  argc = j;

  if ((mzn2fzn_exe || compileSolutionChecker) && solver.empty()) {
    solver = "org.minizinc.mzn-fzn";
  }

  if (flagVerbose) {
    argv.emplace_back("--verbose-solving");
    argc++;
  }
  if (flagStatistics) {
    argv.emplace_back("--solver-statistics");
    argc++;
  }

  if (ifMzn2Fzn()) {
    _flt.setFlagOutputByDefault(true);
    if (solver.empty()) {
      throw BadOption(
          "Using the --compile (or -c) flag requires that a solver is selected explicitly using "
          "the --solver flag");
    }
  }

  bool isMznMzn = false;
  s2out.opt.flagEncapsulateJSON = flagEncapsulateJSON;

  if (!flagIsSolns2out) {
    SolverConfig& sc = _solverConfigs.config(solver);
    string solverId;
    if (sc.executable().empty()) {
      solverId = sc.id();
    } else {
      switch (sc.inputType()) {
        case SolverConfig::O_FZN:
        case SolverConfig::O_JSON:
          solverId = "org.minizinc.mzn-fzn";
          break;
        case SolverConfig::O_MZN:
          solverId = "org.minizinc.mzn-mzn";
          break;
        case SolverConfig::O_NL:
          solverId = "org.minizinc.mzn-nl";
          break;
      }
    }

    for (auto* it : get_global_solver_registry()->getSolverFactories()) {
      /// TODO: also check version (currently assumes all ids are unique)
      if (it->getId() == solverId) {
        _sf = it;
        _sf->finaliseSolverConfigs(_solverConfigs);
        // Check support of -a and -i
        for (const auto& flag : sc.stdFlags()) {
          if (flag == "-a") {
            _supportsA = true;
          } else if (flag == "-i") {
            _supportsI = true;
          } else if (flag == "--json-stream") {
            _supportsJSONStream = true;
          }
        }
        delete _siOpt;
        _siOpt = _sf->createOptions();
        if (!sc.executable().empty() || solverId == "org.minizinc.mzn-fzn" ||
            solverId == "org.minizinc.mzn-nl" || solverId == "org.minizinc.mzn-sat") {
          std::vector<MZNFZNSolverFlag> acceptedFlags;
          for (const auto& sf : sc.stdFlags()) {
            acceptedFlags.push_back(MZNFZNSolverFlag::std(sf));
          }
          for (const auto& ef : sc.extraFlags()) {
            acceptedFlags.push_back(MZNFZNSolverFlag::extra(ef));
          }

          // Collect arguments required for underlying exe
          vector<string> fzn_mzn_flags;
          if (sc.needsStdlibDir()) {
            fzn_mzn_flags.emplace_back("--stdlib-dir");
            fzn_mzn_flags.push_back(FileUtils::share_directory());
          }
          if (sc.needsMznExecutable()) {
            fzn_mzn_flags.emplace_back("--minizinc-exe");
            fzn_mzn_flags.push_back(FileUtils::progpath() + "/" + _executableName);
          }

          if (sc.inputType() == SolverConfig::O_MZN) {
            isMznMzn = true;
            MZNSolverFactory::setAcceptedFlags(_siOpt, acceptedFlags);
            std::vector<std::string> additionalArgs_s;
            additionalArgs_s.emplace_back("-m");
            if (!sc.executableResolved().empty()) {
              additionalArgs_s.push_back(sc.executableResolved());
            } else {
              additionalArgs_s.push_back(sc.executable());
            }
            if (!sc.passFlags().empty()) {
              add_flags("--mzn-flag", sc.passFlags(), additionalArgs_s);
            }

            if (!fzn_mzn_flags.empty()) {
              add_flags("--mzn-flag", fzn_mzn_flags, additionalArgs_s);
            }

            // This should maybe be moved to fill in fzn_mzn_flags when
            // --find-muses is implemented (these arguments will be passed
            // through to the subsolver of findMUS)
            if (!sc.mznlib().empty()) {
              if (sc.mznlib().substr(0, 2) == "-G") {
                additionalArgs_s.emplace_back("--mzn-flag");
                additionalArgs_s.push_back(sc.mznlib());
              } else {
                additionalArgs_s.emplace_back("--mzn-flag");
                additionalArgs_s.emplace_back("-G");
                additionalArgs_s.emplace_back("--mzn-flag");
                std::string _mznlib;
                if (!sc.mznlibResolved().empty()) {
                  _mznlib = sc.mznlibResolved();
                } else {
                  _mznlib = sc.mznlib();
                }
                additionalArgs_s.push_back(_mznlib);
              }
            }

            for (i = 0; i < additionalArgs_s.size(); ++i) {
              bool success = _sf->processOption(_siOpt, i, additionalArgs_s);
              if (!success) {
                std::stringstream ss;
                ss << "Solver backend " << solverId << " does not recognise option "
                   << additionalArgs_s[i] << "." << endl;
                throw BadOption(ss.str());
              }
            }
          } else {
            // supports fzn, nl, or dimacs
            std::vector<std::string> additionalArgs;

            switch (sc.inputType()) {
              case SolverConfig::O_FZN:
              case SolverConfig::O_JSON:
                FZNSolverFactory::setAcceptedFlags(_siOpt, acceptedFlags, sc.inputType());
                _flt.setFlagOutputJSON(sc.inputType() == SolverConfig::O_JSON);
                additionalArgs.emplace_back("--fzn-cmd");
                break;
              case SolverConfig::O_NL:
                additionalArgs.emplace_back("--nl-cmd");
                break;
              case SolverConfig::O_MZN:
                assert(false);  // unreachable
            }
            if (!sc.executableResolved().empty()) {
              additionalArgs.push_back(sc.executableResolved());
            } else {
              additionalArgs.push_back(sc.executable());
            }
            if (!sc.passFlags().empty()) {
              switch (sc.inputType()) {
                case SolverConfig::O_FZN:
                case SolverConfig::O_JSON:
                  add_flags("--fzn-flag", sc.passFlags(), additionalArgs);
                  break;
                case SolverConfig::O_NL:
                  add_flags("--nl-flag", sc.passFlags(), additionalArgs);
                  break;
                case SolverConfig::O_MZN:
                  assert(false);  // unreachable
              }
            }
            if (!fzn_mzn_flags.empty()) {
              switch (sc.inputType()) {
                case SolverConfig::O_FZN:
                case SolverConfig::O_JSON:
                  add_flags("--fzn-flag", fzn_mzn_flags, additionalArgs);
                  break;
                case SolverConfig::O_NL:
                  add_flags("--nl-flag", fzn_mzn_flags, additionalArgs);
                  break;
                case SolverConfig::O_MZN:
                  assert(false);  // unreachable
              }
            }
            if (sc.needsPathsFile()) {
              // Instruct flattener to hold onto paths
              int i = 0;
              vector<string> args{"--keep-paths"};
              _flt.processOption(i, args);

              // Instruct FznSolverInstance to write a path file
              // and pass it to the executable with --paths arg
              additionalArgs.emplace_back("--fzn-needs-paths");
            }
            if (!sc.needsSolns2Out()) {
              additionalArgs.emplace_back("--fzn-output-passthrough");
            }
            int i = 0;
            for (i = 0; i < additionalArgs.size(); ++i) {
              bool success = _sf->processOption(_siOpt, i, additionalArgs);
              if (!success) {
                std::stringstream ss;
                ss << "Solver backend " << solverId << " does not recognise option "
                   << additionalArgs[i] << "." << endl;
                throw BadOption(ss.str());
              }
            }
          }
        }
        if (!sc.mznlib().empty()) {
          if (sc.mznlib().substr(0, 2) == "-G") {
            std::vector<std::string> additionalArgs({sc.mznlib()});
            int i = 0;
            if (!_flt.processOption(i, additionalArgs)) {
              std::stringstream ss;
              ss << "Flattener does not recognise option " << sc.mznlib() << endl;
              throw BadOption(ss.str());
            }
          } else {
            std::vector<std::string> additionalArgs(2);
            additionalArgs[0] = "-G";
            if (!sc.mznlibResolved().empty()) {
              additionalArgs[1] = sc.mznlibResolved();
            } else {
              additionalArgs[1] = sc.mznlib();
            }
            int i = 0;
            if (!_flt.processOption(i, additionalArgs)) {
              throw BadOption("Flattener does not recognise option -G.");
            }
          }
        }
        auto reducedDefaultFlags = reducedSolverDefaults.find(sc.id());
        const auto& defaultFlags = reducedDefaultFlags == reducedSolverDefaults.end()
                                       ? sc.defaultFlags()
                                       : reducedDefaultFlags->second;
        if (!defaultFlags.empty()) {
          std::vector<std::string> addedArgs;
          addedArgs.push_back(argv[0]);  // excutable name
          for (const auto& df : defaultFlags) {
            addedArgs.push_back(df);
          }
          for (int i = 1; i < argv.size(); i++) {
            addedArgs.push_back(argv[i]);
          }
          argv = addedArgs;
          argc = static_cast<int>(addedArgs.size());
        }
        break;
      }
    }

    if (_sf == nullptr) {
      std::stringstream ss;
      ss << "Solver " << solver << " not found." << endl;
      throw BadOption(ss.str());
    }

    CLOParser cop(i, argv);  // For special handling of -a, -i and -n-i
    for (i = 1; i < argc; ++i) {
      if (argv[i] == "--push-working-directory") {
        i++;
        workingDirs.push_back(argv[i]);
      } else if (argv[i] == "--pop-working-directory") {
        workingDirs.pop_back();
      } else if (!ifMzn2Fzn() ? s2out.processOption(i, argv, workingDirs.back())
                              : false) {  // NOLINT: Allow repeated empty if
        // Processed by Solns2Out
      } else if ((!isMznMzn || _isMzn2fzn) &&
                 _flt.processOption(i, argv,
                                    workingDirs.back())) {  // NOLINT: Allow repeated empty if
        // Processed by Flattener
      } else if ((_supportsA || _supportsI) && cop.get("-a --all --all-solns --all-solutions")) {
        _flagAllSatisfaction = true;
        _flagIntermediate = true;
      } else if ((_supportsA || _supportsI) &&
                 cop.get("-i --intermediate --intermediate-solutions")) {
        _flagIntermediate = true;
      } else if (cop.getOption("-n-i --no-intermediate --no-intermediate-solutions")) {
        _flagIntermediate = false;
      } else if (_supportsA && cop.get("--all-satisfaction")) {
        _flagAllSatisfaction = true;
      } else if (cop.get("--disable-all-satisfaction")) {
        _flagAllSatisfaction = false;
      } else if (_sf != nullptr &&
                 _sf->processOption(_siOpt, i, argv)) {  // NOLINT: Allow repeated empty if
        // Processed by Solver Factory
      } else {
        std::stringstream ss;
        ss << _executableName << ": Unrecognized option or bad format `" << argv[i] << "'";
        throw BadOption(ss.str());
      }
    }

    /// Set the command line string for printing user FlatZinc
    _flt.setCmdLineStr(std::move(cmdline_ss.str()));

    return OPTION_OK;
  }
  for (i = 1; i < argc; ++i) {
    if (argv[i] == "--push-working-directory") {
      i++;
      workingDirs.push_back(argv[i]);
    } else if (argv[i] == "--pop-working-directory") {
      workingDirs.pop_back();
    } else if (s2out.processOption(i, argv, workingDirs.back())) {
      // Processed by Solns2Out
    } else {
      std::stringstream ss;
      ss << _executableName << ": Unrecognized option or bad format `" << argv[i] << "'" << endl;
      throw BadOption(ss.str());
    }
  }
  return OPTION_OK;
}

void MznSolver::flatten(const std::string& modelString, const std::string& modelName) {
  std::exception_ptr exc;
  _flt.setFlagVerbose(flagCompilerVerbose);
  _flt.setFlagStatistics(flagCompilerStatistics);
  _flt.setFlagEncapsulateJSON(flagEncapsulateJSON);
  if (flagRandomSeed) {
    _flt.setRandomSeed(randomSeed);
  }

#ifndef __EMSCRIPTEN__
  // Create timing thread
  std::promise<void> done;
  auto done_future = done.get_future();
  std::packaged_task<std::chrono::milliseconds::rep()> timer([&] {
    auto time_left = std::max(std::chrono::milliseconds(1), flagOverallTimeLimit - _startTime.ms());
    if (flagOverallTimeLimit != std::chrono::milliseconds(0)) {
      auto time_left =
          std::max(std::chrono::milliseconds(1), flagOverallTimeLimit - _startTime.ms());
      if (done_future.wait_for(time_left) == std::future_status::timeout) {
        // Trigger time out!
        _flt.cancel();
      }
    }
    return time_left.count();
  });
  std::thread thr(std::move(timer));
#endif

  // Flattening process in main thread
  try {
    _flt.flatten(modelString, modelName);
  } catch (...) {
    // Capture exception to gracefully handle threads
    exc = std::current_exception();
  }

#ifndef __EMSCRIPTEN__
  // Join timing thread
  done.set_value();
  thr.join();
#endif

  // Rethrow exception if necessary
  if (exc) {
    std::rethrow_exception(exc);
  }
}

SolverInstance::Status MznSolver::solve() {
  {  // To be able to clean up flatzinc after PrcessFlt()
    GCLock lock;
    getSI()->processFlatZinc();
  }

  struct Solve {
    MznSolver& solver;
    SolverInstance::Status status = SolverInstance::Status::UNKNOWN;
    Solve(MznSolver& s) : solver(s) {}
    SolverInstance::Status operator()() {
      status = solver.getSI()->solve();
      return status;
    }
    ~Solve() {
      // Put in destructor so that this still happens when interrupted
      GCLock lock;
      if (!solver.getSI()->getSolns2Out()->fStatusPrinted) {
        solver.getSI()->getSolns2Out()->evalStatus(status);
      }
      if (solver._siOpt->printStatistics) {
        solver.getSI()->printStatistics();
      }
      if (solver.flagStatistics) {
        solver.getSI()->getSolns2Out()->printStatistics(solver._os);
      }
      // Print any statistics left in the buffer
      solver.getSI()->getSolns2Out()->flushStatistics(solver._os);
    }
  };

  Solve s(*this);
  return s();
}

SolverInstance::Status MznSolver::run(const std::vector<std::string>& args0,
                                      const std::string& model, const std::string& exeName,
                                      const std::string& modelName) {
  std::vector<std::string> args = {exeName};
  for (const auto& a : args0) {
    args.push_back(a);
  }
  try {
    switch (processOptions(args)) {
      case OPTION_FINISH:
        return SolverInstance::NONE;
      case OPTION_OK:
        break;
    }
  } catch (BadOption& e) {
    std::stringstream ss;
    printUsage(ss);
    ss << "More info with \"" << exeName << " --help\"";
    e.usage(ss.str());
    throw e;
  }
  if (flagIsSolns2out &&
      (ifMzn2Fzn() || _sf == nullptr || _sf->getId() != "org.minizinc.mzn-mzn") &&
      !_flt.hasInputFiles() && model.empty()) {
    // We are in solns2out mode
    while (std::cin.good()) {
      string line;
      getline(std::cin, line);
      line += '\n';  // need eols as in t=raw stream
      s2out.feedRawDataChunk(line.c_str());
    }
    return SolverInstance::NONE;
  }

  if (!ifMzn2Fzn() && _sf->getId() == "org.minizinc.mzn-mzn") {
    if (_supportsJSONStream && flagEncapsulateJSON) {
      std::vector<std::string> json_stream_flag(1);
      json_stream_flag[0] = "--json-stream";
      int i = 0;
      _sf->processOption(_siOpt, i, json_stream_flag);
    }
    Env env;
    _si = _sf->createSI(env, _log, _siOpt);
    _si->setSolns2Out(&s2out);
    {  // To be able to clean up flatzinc after PrcessFlt()
      GCLock lock;
      _si->options()->verbose = getFlagVerbose();
      _si->options()->printStatistics = getFlagStatistics();
    }
    _si->solve();
    return SolverInstance::NONE;
  }

  try {
    flatten(model, modelName);
  } catch (Timeout&) {
    if (ifMzn2Fzn()) {
      throw;
    }
    s2out.evalStatus(SolverInstance::UNKNOWN);
    return SolverInstance::UNKNOWN;
  }

  if (!ifMzn2Fzn()) {
    if (flagOverallTimeLimit + flagSolverTimeLimit > std::chrono::milliseconds(0)) {
      std::chrono::milliseconds time_left(0);
      if (flagOverallTimeLimit == std::chrono::milliseconds(0)) {
        time_left = flagSolverTimeLimit;
      } else if (flagSolverTimeLimit == std::chrono::milliseconds(0)) {
        time_left = flagOverallTimeLimit - _startTime.ms();
      } else {
        time_left = std::min(flagSolverTimeLimit, flagOverallTimeLimit - _startTime.ms());
      }
      std::vector<std::string> timeoutArgs(
          {"--solver-time-limit", std::to_string(time_left.count())});
      int i = 0;
      _sf->processOption(_siOpt, i, timeoutArgs);
    }
    if (flagRandomSeed) {
      std::vector<std::string> randomArgs({"--random-seed", std::to_string(randomSeed)});
      int i = 0;
      _sf->processOption(_siOpt, i, randomArgs);
    }
  }

  if (SolverInstance::UNKNOWN == getFltStatus()) {
    if (!ifMzn2Fzn()) {  // only then
      // Special handling of basic stdFlags
      auto* solve_item = _flt.getEnv()->flat()->solveItem();
      bool is_sat_problem =
          solve_item != nullptr ? solve_item->st() == SolveI::SolveType::ST_SAT : true;
      if (is_sat_problem && _flagAllSatisfaction) {
        if (_supportsA) {
          std::vector<std::string> a_flag = {"-a"};
          int i = 0;
          _sf->processOption(_siOpt, i, a_flag);
        } else {
          // Solver does not support -a
          _log << "WARNING: Solver does not support all solutions for satisfaction problems."
               << endl;
        }
      }
      if (!is_sat_problem && _flagIntermediate) {
        std::vector<std::string> i_flag(1);
        i_flag[0] = _supportsI ? "-i" : "-a";  // Fallback to -a if -i is not supported
        int i = 0;
        _sf->processOption(_siOpt, i, i_flag);
      }

      // GCLock lock;                  // better locally, to enable cleanup after ProcessFlt()
      addSolverInterface();
      return solve();
    }
    return SolverInstance::NONE;
  }
  if (!ifMzn2Fzn()) {
    addSolverInterface();
    s2out.evalStatus(getFltStatus());
  }
  return getFltStatus();
  //  Add evalOutput() here?   TODO
}
