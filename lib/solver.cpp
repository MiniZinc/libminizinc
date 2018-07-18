 
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

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

#include <minizinc/solver.hh>

using namespace MiniZinc;

#ifdef HAS_GUROBI
#include <minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh>
namespace {
  Gurobi_SolverFactoryInitialiser _gurobi_init;
}
#endif
#ifdef HAS_CPLEX
#include <minizinc/solvers/MIP/MIP_cplex_solverfactory.hh>
namespace {
  Cplex_SolverFactoryInitialiser _cplex_init;
}
#endif
#ifdef HAS_OSICBC
#include <minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh>
namespace {
  OSICBC_SolverFactoryInitialiser _osicbc_init;
}
#endif
#ifdef HAS_XPRESS
#include <minizinc/solvers/MIP/MIP_xpress_solverfactory.hh>
namespace {
  Xpress_SolverFactoryInitialiser _xpress_init;
}
#endif
#ifdef HAS_GECODE
#include <minizinc/solvers/gecode_solverfactory.hh>
namespace {
  Gecode_SolverFactoryInitialiser _gecode_init;
}
#endif
#ifdef HAS_SCIP
#include <minizinc/solvers/MIP/MIP_scip_solverfactory.hh>
namespace {
  SCIP_SolverFactoryInitialiser _scip_init;
}
#endif

#include <minizinc/solvers/fzn_solverfactory.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>
#include <minizinc/solvers/mzn_solverfactory.hh>
#include <minizinc/solvers/mzn_solverinstance.hh>
namespace {
  FZN_SolverFactoryInitialiser _fzn_init;
  MZN_SolverFactoryInitialiser _mzn_init;
}

MZNFZNSolverFlag MZNFZNSolverFlag::std(const std::string& n0) {
  const std::string argFlags("-I -n -p -r");
  if (argFlags.find(n0) != std::string::npos)
    return MZNFZNSolverFlag(FT_ARG,n0);
  return MZNFZNSolverFlag(FT_NOARG,n0);
}

MZNFZNSolverFlag MZNFZNSolverFlag::extra(const std::string& n0, const std::string& t0) {
  return MZNFZNSolverFlag(t0=="bool" ? FT_NOARG : FT_ARG, n0);
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SolverRegistry* MiniZinc::getGlobalSolverRegistry()
{
  static SolverRegistry sr;
  return &sr;
}

void SolverRegistry::addSolverFactory(SolverFactory* pSF)
{
  assert(pSF);
  sfstorage.push_back(pSF);
}

void SolverRegistry::removeSolverFactory(SolverFactory* pSF)
{
  auto it = find(sfstorage.begin(), sfstorage.end(), pSF);
  assert(pSF);
  sfstorage.erase(it);
}

/// Function createSI also adds each SI to the local storage
SolverInstanceBase * SolverFactory::createSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt) {
  SolverInstanceBase *pSI = doCreateSI(env,log,opt);
  if (!pSI) {
    throw InternalError("SolverFactory: failed to initialize solver "+getDescription());
  }
  sistorage.resize(sistorage.size()+1);
  sistorage.back().reset(pSI);
  return pSI;
}

/// also providing a destroy function for a DLL or just special allocator etc.
void SolverFactory::destroySI(SolverInstanceBase * pSI) {
  auto it = sistorage.begin();
  for ( ; it != sistorage.end(); ++ it)
    if (it->get() == pSI)
      break;
  if (sistorage.end() == it) {
    cerr << "  SolverFactory: failed to remove solver at "
      << pSI << endl;
    throw InternalError("  SolverFactory: failed to remove solver");
  }
  sistorage.erase(it);
}

MznSolver::MznSolver(std::ostream& os0, std::ostream& log0)
  : solver_configs(log0), flt(os0,log0,solver_configs.mznlibDir()), executable_name("<executable>"), os(os0), log(log0), s2out(os0,log0,solver_configs.mznlibDir()) {}

MznSolver::~MznSolver()
{
//   if (si)                         // first the solver
//     CleanupSolverInterface(si);
  // TODO cleanup the used solver interfaces
  si=0;
  GC::trigger();
}

bool MznSolver::ifMzn2Fzn() {
  return is_mzn2fzn;
}

bool MznSolver::ifSolns2out() {
  return s2out._opt.flag_standaloneSolns2Out;
}

void MznSolver::addSolverInterface(SolverFactory* sf)
{
  si = sf->createSI(*flt.getEnv(), log, si_opt);
  assert(si);
  if (s2out.getEnv()==NULL)
    s2out.initFromEnv( flt.getEnv() );
  si->setSolns2Out( &s2out );
  if (flag_compiler_verbose)
    log
    //     << "  ---------------------------------------------------------------------------\n"
    << "      % SOLVING PHASE\n"
    << sf->getDescription(si_opt) << endl;
}

void MznSolver::addSolverInterface()
{
  GCLock lock;
  if (sf==NULL) {
    if ( getGlobalSolverRegistry()->getSolverFactories().empty() ) {
      log << " MznSolver: NO SOLVER FACTORIES LINKED." << endl;
      assert( 0 );
    }
    sf = getGlobalSolverRegistry()->getSolverFactories().back();
  }
  addSolverInterface(sf);
}

void MznSolver::printUsage()
{
  os << executable_name << ": ";
  if ( ifMzn2Fzn() ) {
    os
      << "MiniZinc to FlatZinc converter.\n"
      << "Usage: "  << executable_name
      << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl;
  } else if (ifSolns2out()) {
    os
      << "Solutions to output translator.\n"
      << "Usage: "  << executable_name
      << "  [<options>] <model>.ozn" << std::endl;
  } else {
    os
      << "MiniZinc driver.\n"
      << "Usage: "  << executable_name
      << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...] or just <flat>.fzn" << std::endl;
  }
}

void MznSolver::printHelp(const std::string& selectedSolver)
{
  printUsage();
  os
    << "General options:" << std::endl
    << "  --help, -h\n    Print this help message." << std::endl
    << "  --version\n    Print version information." << std::endl
    << "  --solvers\n    Print list of available solvers." << std::endl
    << "  --solver <solver id>, --solver <solver config file>.msc\n    Select solver to use." << std::endl
    << "  --help <solver id>\n    Print help for a particular solver." << std::endl
    << "  -v, -l, --verbose\n    Print progress/log statements. Note that some solvers may log to stdout." << std::endl
    << "  --verbose-compilation\n    Print progress/log statements for compilation." << std::endl
    << "  -s, --statistics\n    Print statistics." << std::endl
    << "  --compiler-statistics\n    Print statistics for compilation." << std::endl
    << "  -c, --compile\n    Compile only (do not run solver)." << std::endl
    << "  --config-dirs\n    Output configuration directories." << std::endl;

  if (selectedSolver.empty()) {
    flt.printHelp(os);
    os << endl;
    if ( !ifMzn2Fzn() ) {
      s2out.printHelp(os);
      os << endl;
    }
    os << "Available solvers (get help using --help <solver id>):" << endl;
    std::vector<std::string> solvers = solver_configs.solvers();
    if (solvers.size()==0)
      cout << "  none.\n";
    for (unsigned int i=0; i<solvers.size(); i++) {
      cout << "  " << solvers[i] << endl;
    }
  } else {
    const SolverConfig& sc = solver_configs.config(selectedSolver);
    string solverId = sc.executable().empty() ? sc.id() : (sc.supportsMzn() ?  string("org.minizinc.mzn-mzn") : string("org.minizinc.mzn-fzn"));
    bool found = false;
    for (auto it = getGlobalSolverRegistry()->getSolverFactories().rbegin();
         it != getGlobalSolverRegistry()->getSolverFactories().rend(); ++it) {
      if ((*it)->getId()==solverId) {
        os << endl;
        (*it)->printHelp(os);
        if (!sc.executable().empty() && !sc.extraFlags().empty()) {
          os << "Extra solver flags (use with ";
          os << (sc.supportsMzn() ? "--mzn-flags" : "--fzn-flags") << ")" << endl;
          for (const SolverConfig::ExtraFlag& ef: sc.extraFlags()) {
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

void addFlags(const std::string& sep,
              const std::vector<std::string>& in_args,
              std::vector<std::string>& out_args) {
  for(const std::string& arg : in_args) {
    out_args.push_back(sep);
    out_args.push_back(arg);
  }
}

MznSolver::OptionStatus MznSolver::processOptions(std::vector<std::string>& argv)
{
  executable_name = argv[0];
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  string solver;
  bool mzn2fzn_exe = (executable_name=="mzn2fzn");
  if (mzn2fzn_exe) {
    is_mzn2fzn=true;
  } else if (executable_name=="solns2out") {
    s2out._opt.flag_standaloneSolns2Out=true;
  }
  int i=1, j=1;
  int argc = static_cast<int>(argv.size());
  if (argc < 2)
    return OPTION_ERROR;
  for (i=1; i<argc; ++i) {
    if (argv[i]=="-h" || argv[i]=="--help") {
      if (argc > i+1) {
        printHelp(argv[i+1]);
      } else {
        printHelp();
      }
      return OPTION_FINISH;
    }
    if (argv[i]=="--version") {
      flt.printVersion(cout);
      return OPTION_FINISH;
    }
    if (argv[i]=="--solvers") {
      cout << "MiniZinc driver.\nAvailable solver configurations:\n";
      std::vector<std::string> solvers = solver_configs.solvers();
      if (solvers.size()==0)
        cout << "  none.\n";
      for (unsigned int i=0; i<solvers.size(); i++) {
        cout << "  " << solvers[i] << endl;
      }
      return OPTION_FINISH;
    }
    if (argv[i]=="--solvers-json") {
      cout << solver_configs.solverConfigsJSON();
      return OPTION_FINISH;
    }
    if (argv[i]=="--config-dirs") {
      GCLock lock;
      cout << "{\n";
      cout << "  \"globalConfigFile\" : \"" << Printer::escapeStringLit(FileUtils::global_config_file()) << "\",\n";
      cout << "  \"userConfigFile\" : \"" << Printer::escapeStringLit(FileUtils::user_config_file()) << "\",\n";
      cout << "  \"userSolverConfigDir\" : \"" << Printer::escapeStringLit(FileUtils::user_config_dir()) << "/solvers\",\n";
      cout << "  \"mznStdlibDir\" : \"" << Printer::escapeStringLit(solver_configs.mznlibDir()) << "\"\n";
      cout << "}\n";
      return OPTION_FINISH;
    }
    if (argv[i]=="--solver") {
      ++i;
      if (i==argc) {
        log << "Argument required for --solver" << endl;
        return OPTION_ERROR;
      }
      if (solver.size()>0 && solver != argv[i]) {
        log << "Only one --solver option allowed" << endl;
        return OPTION_ERROR;
      }
      solver = argv[i];
    } else if (argv[i]=="-c" || argv[i]=="--compile") {
      is_mzn2fzn = true;
    } else if (argv[i]=="-v" || argv[i]=="--verbose" || argv[i]=="-l") {
      flag_verbose = true;
      flag_compiler_verbose = true;
    } else if (argv[i]=="--verbose-compilation") {
      flag_compiler_verbose = true;
    } else if (argv[i]=="-s" || argv[i]=="--statistics") {
      flag_statistics = true;
      flag_compiler_statistics = true;
    } else if (argv[i]=="--compiler-statistics") {
      flag_compiler_statistics = true;
    } else {
      if ((argv[i]=="--fzn-cmd" || argv[i]=="--flatzinc-cmd") && solver.empty()) {
        solver = "org.minizinc.mzn-fzn";
      }
      argv[j++] = argv[i];
    }
  }
  argv.resize(j);
  argc = j;

  if (mzn2fzn_exe && solver.empty()) {
    solver = "org.minizinc.mzn-fzn";
  }
  
  if (flag_verbose) {
    argv.push_back("--verbose-solving");
    argc++;
  }
  if (flag_statistics) {
    argv.push_back("--solver-statistics");
    argc++;
  }
  
  flt.set_flag_output_by_default(ifMzn2Fzn());

  bool isMznMzn = false;
  
  if (!ifSolns2out()) {
    try {
      const SolverConfig& sc = solver_configs.config(solver);
      string solverId = sc.executable().empty() ? sc.id() : (sc.supportsMzn() ?  string("org.minizinc.mzn-mzn") : string("org.minizinc.mzn-fzn"));
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
           it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
        if ((*it)->getId()==solverId) { /// TODO: also check version (currently assumes all ids are unique)
          sf = *it;
          si_opt = sf->createOptions();
          if (!sc.executable().empty() || solverId=="org.minizinc.mzn-fzn") {
            std::vector<MZNFZNSolverFlag> acceptedFlags;
            for (auto& sf : sc.stdFlags())
              acceptedFlags.push_back(MZNFZNSolverFlag::std(sf));
            for (auto& ef : sc.extraFlags())
              acceptedFlags.push_back(MZNFZNSolverFlag::extra(ef.flag,ef.flag_type));

            // Collect arguments required for underlying exe
            vector<string> fzn_mzn_flags;
            if (sc.needsStdlibDir()) {
              fzn_mzn_flags.push_back("--stdlib-dir");
              fzn_mzn_flags.push_back(FileUtils::share_directory());
            }
            if (sc.needsMznExecutable()) {
              fzn_mzn_flags.push_back("--minizinc-exe");
              fzn_mzn_flags.push_back(FileUtils::progpath() + "/" + executable_name);
            }

            if (sc.supportsMzn()) {
              isMznMzn = true;
              static_cast<MZN_SolverFactory*>(sf)->setAcceptedFlags(si_opt, acceptedFlags);
              std::vector<std::string> additionalArgs_s;
              additionalArgs_s.push_back("-m");
              if (sc.executable_resolved().size()) {
                additionalArgs_s.push_back(sc.executable_resolved());
              } else {
                additionalArgs_s.push_back(sc.executable());
              }

              if(!fzn_mzn_flags.empty()) {
                addFlags("--mzn-flag", fzn_mzn_flags, additionalArgs_s);
              }

              // This should maybe be moved to fill in fzn_mzn_flags when
              // --find-muses is implemented (these arguments will be passed
              // through to the subsolver of findMUS)
              if (!sc.mznlib().empty()) {
                if (sc.mznlib().substr(0,2)=="-G") {
                  additionalArgs_s.push_back("--mzn-flag");
                  additionalArgs_s.push_back(sc.mznlib());
                } else {
                  additionalArgs_s.push_back("--mzn-flag");
                  additionalArgs_s.push_back("-I");
                  additionalArgs_s.push_back("--mzn-flag");
                  std::string _mznlib;
                  if (sc.mznlib_resolved().size()) {
                    _mznlib = sc.mznlib_resolved();
                  } else {
                    _mznlib = sc.mznlib();
                  }
                  additionalArgs_s.push_back(_mznlib);
                }
              }

              for (i=0; i<additionalArgs_s.size(); ++i) {
                bool success = sf->processOption(si_opt, i, additionalArgs_s);
                if (!success) {
                  log << "Solver backend " << solverId << " does not recognise option " << additionalArgs_s[i]  << "." << endl;
                  return OPTION_ERROR;
                }
              }
            } else {
              static_cast<FZN_SolverFactory*>(sf)->setAcceptedFlags(si_opt, acceptedFlags);
              std::vector<std::string> additionalArgs;
              additionalArgs.push_back("--fzn-cmd");
              if (sc.executable_resolved().size()) {
                additionalArgs.push_back(sc.executable_resolved());
              } else {
                additionalArgs.push_back(sc.executable());
              }
              if(!fzn_mzn_flags.empty()) {
                addFlags("--fzn-flag", fzn_mzn_flags, additionalArgs);
              }
              if (sc.needsPathsFile()) {
                // Instruct flattener to hold onto paths
                int i=0;
                vector<string> args {"--keep-paths"};
                flt.processOption(i, args);

                // Instruct FznSolverInstance to write a path file
                // and pass it to the executable with --paths arg
                additionalArgs.push_back("--fzn-needs-paths");
              }
              if (!sc.needsSolns2Out()) {
                additionalArgs.push_back("--fzn-output-passthrough");
              }
              int i=0;
              for (i=0; i<additionalArgs.size(); ++i) {
                bool success = sf->processOption(si_opt, i, additionalArgs);
                if (!success) {
                  log << "Solver backend " << solverId << " does not recognise option " << additionalArgs[i]  << "." << endl;
                  return OPTION_ERROR;
                }
              }
            }
          }
          if (!sc.mznlib().empty()) {
            if (sc.mznlib().substr(0,2)=="-G") {
              std::vector<std::string> additionalArgs({sc.mznlib()});
              int i=0;
              if (!flt.processOption(i, additionalArgs)) {
                log << "Flattener does not recognise option " << sc.mznlib() << endl;
                return OPTION_ERROR;
              }
            } else {
              std::vector<std::string>  additionalArgs(2);
              additionalArgs[0] = "-I";
              if (sc.mznlib_resolved().size()) {
                additionalArgs[1] = sc.mznlib_resolved();
              } else {
                additionalArgs[1] = sc.mznlib();
              }
              int i=0;
              if (!flt.processOption(i, additionalArgs)) {
                log << "Flattener does not recognise option -I." << endl;
                return OPTION_ERROR;
              }
            }
          }
          if (!sc.defaultFlags().empty()) {
            std::vector<std::string> addedArgs;
            addedArgs.push_back(argv[0]); // excutable name
            for (auto& df : sc.defaultFlags()) {
              addedArgs.push_back(df);
            }
            for (int i=1; i<argv.size(); i++) {
              addedArgs.push_back(argv[i]);
            }
            argv = addedArgs;
            argc = addedArgs.size();
          }
          break;
        }
      }
      
    } catch (ConfigException& e) {
      log << "Config exception: " << e.msg() << endl;
      return OPTION_ERROR;
    }

    if (sf==NULL) {
      log << "Solver " << solver << " not found." << endl;
      return OPTION_ERROR;
    }
    
    for (i=1; i<argc; ++i) {
      if ( !ifMzn2Fzn() ? s2out.processOption( i, argv ) : false ) {
      } else if ((!isMznMzn || is_mzn2fzn) && flt.processOption(i, argv)) {
      } else if (sf != NULL && sf->processOption(si_opt, i, argv)) {
      } else {
        std::string executable_name(argv[0]);
        executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
        log << executable_name << ": Unrecognized option or bad format `" << argv[i] << "'" << endl;
        return OPTION_ERROR;
      }
    }
    return OPTION_OK;

  } else {
    for (i=1; i<argc; ++i) {
      if ( s2out.processOption( i, argv ) ) {
      } else {
        std::string executable_name(argv[0]);
        executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
        log << executable_name << ": Unrecognized option or bad format `" << argv[i] << "'" << endl;
        return OPTION_ERROR;
      }
    }
    return OPTION_OK;

  }
  
}

void MznSolver::flatten(const std::string& modelString)
{
  flt.set_flag_verbose(flag_compiler_verbose);
  flt.set_flag_statistics(flag_compiler_statistics);
  clock_t tm01 = clock();
  flt.flatten(modelString);
  /// The following message tells mzn-test.py that flattening succeeded.
  if (flag_compiler_verbose)
    log << "  Flattening done, " << timeDiff(clock(), tm01) << std::endl;
}

SolverInstance::Status MznSolver::solve()
{
  { // To be able to clean up flatzinc after PrcessFlt()
    GCLock lock;
    getSI()->processFlatZinc();
  }
  SolverInstance::Status status = getSI()->solve();
  GCLock lock;
  if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
    getSI()->printSolution();             // What if it's already printed?  TODO
    if ( !getSI()->getSolns2Out()->fStatusPrinted )
      getSI()->getSolns2Out()->evalStatus( status );
  }
  else {
    if ( !getSI()->getSolns2Out()->fStatusPrinted )
      getSI()->getSolns2Out()->evalStatus( status );
    if (get_flag_statistics())    // it's summary in fact
      printStatistics();
  }
  return status;
}

void MznSolver::printStatistics()
{ // from flattener too?   TODO
  if (si)
    getSI()->printStatisticsLine(1);
}

SolverInstance::Status MznSolver::run(const std::vector<std::string>& args0, const std::string& model, const std::string& exeName) {
  std::vector<std::string> args = {exeName};
  for (auto a : args0)
    args.push_back(a);
  switch (processOptions(args)) {
    case OPTION_FINISH:
      return SolverInstance::NONE;
    case OPTION_ERROR:
      printUsage();
      os << "More info with \"" << exeName << " --help\"\n";
      return SolverInstance::ERROR;
    case OPTION_OK:
      break;
  }
  if (!(!ifMzn2Fzn() && sf!=NULL && sf->getId() == "org.minizinc.mzn-mzn") && !flt.hasInputFiles()) {
    // We are in solns2out mode
    while ( std::cin.good() ) {
      string line;
      getline( std::cin, line );
      line += '\n';                // need eols as in t=raw stream
      s2out.feedRawDataChunk( line.c_str() );
    }
    return SolverInstance::NONE;
  }

  if (!ifMzn2Fzn() && sf->getId() == "org.minizinc.mzn-mzn") {
    Env env;
    si = sf->createSI(env, log, si_opt);
    si->setSolns2Out( &s2out );
    { // To be able to clean up flatzinc after PrcessFlt()
      GCLock lock;
      getSI()->_options->verbose = get_flag_verbose();
      getSI()->_options->printStatistics = get_flag_statistics();
    }
    getSI()->solve();
    return SolverInstance::NONE;
  }
  
  flatten(model);
  
  if (SolverInstance::UNKNOWN == getFltStatus())
  {
    if ( !ifMzn2Fzn() ) {          // only then
      // GCLock lock;                  // better locally, to enable cleanup after ProcessFlt()
      addSolverInterface();
      return solve();
    }
    return SolverInstance::NONE;
  } else {
    if ( !ifMzn2Fzn() )
      s2out.evalStatus( getFltStatus() );
    return getFltStatus();
  }                                   //  Add evalOutput() here?   TODO
}
