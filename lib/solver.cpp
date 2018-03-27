 
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
SolverInstanceBase * SolverFactory::createSI(Env& env, std::ostream& log) {
  SolverInstanceBase *pSI = doCreateSI(env,log);
  if (!pSI) {
    throw InternalError("SolverFactory: failed to initialize solver "+getVersion());
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

MznSolver::MznSolver(std::ostream& os0, std::ostream& log0, bool ism2f)
  : flt(os0,log0,ism2f), is_mzn2fzn(ism2f), solver_configs(""), executable_name("<executable>"), os(os0), log(log0), s2out(os0,log0) {}

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
  si = sf->createSI(*flt.getEnv(), log);
  assert(si);
  s2out.initFromEnv( flt.getEnv() );
  si->setSolns2Out( &s2out );
  if (get_flag_verbose())
    log
    //     << "  ---------------------------------------------------------------------------\n"
    << "      % SOLVING PHASE\n"
    << sf->getVersion() << endl;
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

void MznSolver::printHelp()
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
    << "Options:" << std::endl
    << "  --help, -h\n    Print this help message." << std::endl
    << "  --version\n    Print version information." << std::endl
    << "  --solvers\n    Print available solvers." << std::endl
    << "  -v, -l, --verbose\n    Print progress/log statements. Note that some solvers may log to stdout." << std::endl
    << "  -s, --statistics\n    Print statistics." << std::endl;
//   if ( getNSolvers() )
  
  flt.printHelp(os);
  os << endl;
  if ( !ifMzn2Fzn() ) {
    s2out.printHelp(os);
    os << endl;
  }
  for (auto it = getGlobalSolverRegistry()->getSolverFactories().rbegin();
        it != getGlobalSolverRegistry()->getSolverFactories().rend(); ++it) {
       (*it)->printHelp(os);
      os << endl;
  }
}

bool MznSolver::processOptions(int& argc, const char**& argv)
{
  executable_name = argv[0];
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  int i=1, j=1;
  if (argc < 2)
    return false;
  string solver;
  for (i=1; i<argc; ++i) {
    if (string(argv[i])=="-h" || string(argv[i])=="--help") {
      printHelp();
      std::exit(EXIT_SUCCESS);
    }
    if (string(argv[i])=="--version") {
      flt.printVersion(cout);
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().rbegin();
           it != getGlobalSolverRegistry()->getSolverFactories().rend(); ++it)
        cout << (*it)->getVersion() << endl;
      std::exit(EXIT_SUCCESS);
    }
    if (string(argv[i])=="--solvers") {
      cout << "MiniZinc driver.\nAvailable solver configurations:\n";
      std::vector<std::string> solvers = solver_configs.solvers();
      if (solvers.size()==0)
        cout << "  none.\n";
      for (unsigned int i=0; i<solvers.size(); i++) {
        cout << "  " << solvers[i] << endl;
      }
      std::exit(EXIT_SUCCESS);
    }
    if (string(argv[i])=="--solver") {
      ++i;
      if (i==argc) {
        log << "Argument required for --solver" << endl;
        return false;
      }
      if (solver.size()>0) {
        log << "Only one --solver option allowed" << endl;
        return false;
      }
      solver = argv[i];
    } else if ((argv[i])==string("-v") || (argv[i])==string("--verbose") || (argv[i])==string("-l")) {
      flag_verbose = true;
    } else if (string(argv[i])=="-s" || string(argv[i])=="--statistics") {
      flag_statistics = true;                  // is this Flattener's option?
    } else {
      argv[j++] = argv[i];
    }
  }
  argc = j;

  if (!ifMzn2Fzn()) {
    if (solver.empty()) {
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
           it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
        if ((*it)->getId()=="org.minizinc.mzn-fzn") {
          sf = *it;
        }
      }
    } else {

      try {
        const SolverConfig& sc = solver_configs.config(solver);
        string solverId = sc.executable().empty() ? sc.id() : string("org.minizinc.mzn-fzn");
        for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
             it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
          if ((*it)->getId()==solverId) {
            sf = *it;
            if (!sc.executable().empty()) {
              const char* additionalArgs[2];
              additionalArgs[0] = "-f";
              std::string executable = sc.executable();
              additionalArgs[1] = executable.c_str();
              int i=0;
              bool success = sf->processOption(i, 2, additionalArgs);
              if (!success) {
                log << "Solver backend " << solverId << " does not recognise option -f." << endl;
                return false;
              }
            }
            if (!sc.mznlib().empty()) {
              if (sc.mznlib().substr(0,2)=="-G") {
                const char* additionalArgs[1];
                std::string mznlib = sc.mznlib();
                additionalArgs[0] = mznlib.c_str();
                int i=0;
                if (!flt.processOption(i, 1, additionalArgs)) {
                  log << "Flattener does not recognise option " << sc.mznlib() << endl;
                  return false;
                }
              } else {
                const char* additionalArgs[2];
                additionalArgs[0] = "-I";
                std::string mznlib = sc.mznlib();
                additionalArgs[1] = mznlib.c_str();
                int i=0;
                if (!flt.processOption(i, 2, additionalArgs)) {
                  log << "Flattener does not recognise option -I." << endl;
                  return false;
                }
              }
            }
            break;
          }
        }
        
      } catch (ConfigException& e) {
        log << "Config exception: " << e.what() << endl;
        return false;
      }
    }
    
    if (sf==NULL) {
      log << "Solver " << solver << " not found." << endl;
      return false;
    }
  }
  
  for (i=1; i<argc; ++i) {
    if ( !ifMzn2Fzn() ? s2out.processOption( i, argc, argv ) : false ) {
    } else if (flt.processOption(i, argc, argv)) {
    } else if (sf->processOption(i, argc, argv)) {
    } else {
      std::string executable_name(argv[0]);
      executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
      log << executable_name << ": Unrecognized option or bad format `" << argv[i] << "'" << endl;
      return false;
    }
  }
  return true;
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
    getSI()->getOptions().setBoolParam  (constants().opts.verbose.str(),  get_flag_verbose());
    getSI()->getOptions().setBoolParam  (constants().opts.statistics.str(),  get_flag_statistics());
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


