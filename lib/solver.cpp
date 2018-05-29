 
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
  : flt(os0,log0,solver_configs.mznlibDir()), executable_name("<executable>"), os(os0), log(log0), s2out(os0,log0,solver_configs.mznlibDir()) {}

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

void MznSolver::addSolverInterface(SolverFactory* sf)
{
  si = sf->createSI(*flt.getEnv(), log, si_opt);
  assert(si);
  if (s2out.getEnv()==NULL)
    s2out.initFromEnv( flt.getEnv() );
  si->setSolns2Out( &s2out );
  if (get_flag_verbose())
    log
    //     << "  ---------------------------------------------------------------------------\n"
    << "      % SOLVING PHASE\n"
    << sf->getDescription() << endl;
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

void MznSolver::printHelp(const std::string& selectedSolver)
{
  if ( !ifMzn2Fzn() )
  os
    << "MiniZinc driver.\n"
    << "Usage: "  << executable_name
    << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...] or just <flat>.fzn" << std::endl;
  else
  os
    << "MiniZinc to FlatZinc converter.\n"
    << "Usage: "  << executable_name
    << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl;
  os
    << "General options:" << std::endl
    << "  --help, -h\n    Print this help message." << std::endl
    << "  --version\n    Print version information." << std::endl
    << "  --solvers\n    Print list of available solvers." << std::endl
    << "  --solver <solver id>\n    Select solver to use." << std::endl
    << "  -v, -l, --verbose\n    Print progress/log statements. Note that some solvers may log to stdout." << std::endl
    << "  -s, --statistics\n    Print statistics." << std::endl
    << "  -c, --compile\n    Compile only (do not run solver)." << std::endl;

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

MznSolver::OptionStatus MznSolver::processOptions(std::vector<std::string>& argv)
{
  executable_name = argv[0];
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  int i=1, j=1;
  int argc = argv.size();
  if (argc < 2)
    return OPTION_ERROR;
  string solver;
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
    if (argv[i]=="--user-solver-config-dir") {
      cout << FileUtils::user_config_dir()+"/solvers";
      return OPTION_FINISH;
    }
    if (argv[i]=="--solver") {
      ++i;
      if (i==argc) {
        log << "Argument required for --solver" << endl;
        return OPTION_ERROR;
      }
      if (solver.size()>0) {
        log << "Only one --solver option allowed" << endl;
        return OPTION_ERROR;
      }
      solver = argv[i];
    } else if (argv[i]=="-c" || argv[i]=="--compile") {
      is_mzn2fzn = true;
    } else if (argv[i]=="-v" || argv[i]=="--verbose" || argv[i]=="-l") {
      flag_verbose = true;
    } else if (argv[i]=="-s" || argv[i]=="--statistics") {
      flag_statistics = true;                  // is this Flattener's option?
    } else {
      argv[j++] = argv[i];
    }
  }
  argv.resize(j);
  argc = j;

  flt.set_flag_output_by_default(ifMzn2Fzn());

  bool isMznMzn = false;
  
  if (solver.empty()) {
    for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
         it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
      if ((*it)->getId()=="org.minizinc.mzn-fzn") {
        sf = *it;
        si_opt = sf->createOptions();
      }
    }
  } else {

    try {
      const SolverConfig& sc = solver_configs.config(solver);
      string solverId = sc.executable().empty() ? sc.id() : (sc.supportsMzn() ?  string("org.minizinc.mzn-mzn") : string("org.minizinc.mzn-fzn"));
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
           it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
        if ((*it)->getId()==solverId) { /// TODO: also check version (currently assumes all ids are unique)
          sf = *it;
          si_opt = sf->createOptions();
          if (!sc.executable().empty()) {
            if (sc.supportsMzn()) {
              isMznMzn = true;
              std::vector<std::string> additionalArgs_s;
              additionalArgs_s.push_back("-m");
              additionalArgs_s.push_back(sc.executable().c_str());
              if (sc.needsStdlibDir()) {
                additionalArgs_s.push_back("--stdlib-dir");
                additionalArgs_s.push_back(FileUtils::share_directory().c_str());
              }
              if (sc.needsMznExecutable()) {
                additionalArgs_s.push_back("--minizinc-exe");
                additionalArgs_s.push_back(FileUtils::progpath()+"/"+executable_name);
              }
              int i=0;
              bool success = sf->processOption(si_opt, i, additionalArgs_s);
              if (!success) {
                log << "Solver backend " << solverId << " does not recognise option -f." << endl;
                return OPTION_ERROR;
              }
              std::cerr << "created factory\n";
            } else {
              std::vector<std::string> additionalArgs(2);
              additionalArgs[0] = "-f";
              std::string executable = sc.executable();
              additionalArgs[1] = executable;
              int i=0;
              bool success = sf->processOption(si_opt, i, additionalArgs);
              if (!success) {
                log << "Solver backend " << solverId << " does not recognise option -f." << endl;
                return OPTION_ERROR;
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
              std::vector<std::string>  additionalArgs({"-I",sc.mznlib()});
              int i=0;
              if (!flt.processOption(i, additionalArgs)) {
                log << "Flattener does not recognise option -I." << endl;
                return OPTION_ERROR;
              }
            }
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
  }

  for (i=1; i<argc; ++i) {
    if ( !ifMzn2Fzn() ? s2out.processOption( i, argv ) : false ) {
    } else if (!isMznMzn && flt.processOption(i, argv)) {
    } else if (sf != NULL && sf->processOption(si_opt, i, argv)) {
    } else {
      std::string executable_name(argv[0]);
      executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
      log << executable_name << ": Unrecognized option or bad format `" << argv[i] << "'" << endl;
      return OPTION_ERROR;
    }
  }
  return OPTION_OK;
}

void MznSolver::flatten(const std::string& modelString)
{
  flt.set_flag_verbose(get_flag_verbose());
  flt.set_flag_statistics(get_flag_statistics());
  clock_t tm01 = clock();
  flt.flatten(modelString);
  /// The following message tells mzn-test.py that flattening succeeded.
  if (get_flag_verbose())
    log << "  Flattening done, " << timeDiff(clock(), tm01) << std::endl;
}

void MznSolver::solve()
{
  { // To be able to clean up flatzinc after PrcessFlt()
    GCLock lock;
    getSI()->_options->verbose = get_flag_verbose();
    getSI()->_options->printStatistics = get_flag_statistics();
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
}

void MznSolver::printStatistics()
{ // from flattener too?   TODO
  if (si)
    getSI()->printStatisticsLine(1);
}

bool MznSolver::run(int& argc, const char**& argv, const std::string& model) {
  std::vector<std::string> args;
  for (unsigned int i=0; i<argc; i++)
    args.push_back(argv[i]);
  switch (processOptions(args)) {
    case OPTION_FINISH:
      return true;
    case OPTION_ERROR:
      printHelp();
      return false;
    case OPTION_OK:
      break;
  }
  if (sf->getId() == "org.minizinc.mzn-mzn") {
    Env env;
    si = sf->createSI(env, log, si_opt);
    { // To be able to clean up flatzinc after PrcessFlt()
      GCLock lock;
      getSI()->_options->verbose = get_flag_verbose();
      getSI()->_options->printStatistics = get_flag_statistics();
    }
    getSI()->solve();
    return true;
  }
  
  if (!flt.hasInputFiles()) {
    // We are in solns2out mode
    while ( std::cin.good() ) {
      string line;
      getline( std::cin, line );
      line += '\n';                // need eols as in t=raw stream
      s2out.feedRawDataChunk( line.c_str() );
    }
    return true;
  }
  
  flatten(model);
  
  if (SolverInstance::UNKNOWN == getFltStatus())
  {
    if ( !ifMzn2Fzn() ) {          // only then
      // GCLock lock;                  // better locally, to enable cleanup after ProcessFlt()
      addSolverInterface();
      solve();
    }
    return true;
  } else {
    if ( !ifMzn2Fzn() )
      s2out.evalStatus( getFltStatus() );
    return (SolverInstance::ERROR != getFltStatus());
  }                                   //  Add evalOutput() here?   TODO
}
