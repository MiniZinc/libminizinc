 
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
SolverInstanceBase * SolverFactory::createSI(Env& env) {
  SolverInstanceBase *pSI = doCreateSI(env);
  if (!pSI) {
    cerr << "  SolverFactory: failed to initialize solver "
      << getVersion() << endl;
    throw InternalError("  SolverFactory: failed to initialize solver");
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

MznSolver::MznSolver(bool ism2f) : is_mzn2fzn(ism2f), executable_name("<executable>") { }

MznSolver::~MznSolver()
{
//   if (si)                         // first the solver
//     CleanupSolverInterface(si);
  // TODO cleanup the used solver interfaces
  si=0;
  if (flt)
    cleanupGlobalFlattener(flt);
  flt=0;
  GC::trigger();
}

bool MznSolver::ifMzn2Fzn() {
  return is_mzn2fzn;
}

void MznSolver::addFlattener()
{
  flt = getGlobalFlattener(ifMzn2Fzn());
  assert(flt);
}

void MznSolver::addSolverInterface()
{
  GCLock lock;
  if ( getGlobalSolverRegistry()->getSolverFactories().empty() ) {
    cerr << " MznSolver: NO SOLVER FACTORIES LINKED." << endl;
    assert( 0 );
  }
  si = getGlobalSolverRegistry()->getSolverFactories().back()->createSI(*flt->getEnv());
  assert(si);
  s2out.initFromEnv( flt->getEnv() );
  si->setSolns2Out( &s2out );
  if (get_flag_verbose())
    cerr
//     << "  ---------------------------------------------------------------------------\n"
    << "      % SOLVING PHASE\n"
    << getGlobalSolverRegistry()->getSolverFactories().back()->getVersion() << endl;  
}

void MznSolver::printHelp(std::ostream& os)
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
    << "  -v, -l, --verbose\n    Print progress/log statements. Note that some solvers may log to stdout." << std::endl
    << "  -s, --statistics\n    Print statistics." << std::endl;
//   if ( getNSolvers() )
  
  getFlt()->printHelp(os);
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

bool MznSolver::processOptions(int argc, const char** argv, std::ostream& os)
{
  executable_name = argv[0];
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  int i=1;
  if (argc < 2)
    return false;
  for (i=1; i<argc; ++i) {
    if (string(argv[i])=="-h" || string(argv[i])=="--help") {
      printHelp(os);
      std::exit(EXIT_SUCCESS);
    }
    if (string(argv[i])=="--version") {
      getFlt()->printVersion(cout);
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().rbegin();
           it != getGlobalSolverRegistry()->getSolverFactories().rend(); ++it)
        cout << (*it)->getVersion() << endl;
      std::exit(EXIT_SUCCESS);
    }
    //  moving --verbose here:
    if ((argv[i])==string("-v") || (argv[i])==string("--verbose") || (argv[i])==string("-l")) {
      flag_verbose = true;
    } else if (string(argv[i])=="-s" || string(argv[i])=="--statistics") {
      flag_statistics = true;                  // is this Flattener's option?
    } else if ( !ifMzn2Fzn() ? s2out.processOption( i, argc, argv ) : false ) {
    } else if (!getFlt()->processOption(i, argc, argv)) {
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().rbegin();
           it != getGlobalSolverRegistry()->getSolverFactories().rend(); ++it)
        if ((*it)->processOption(i, argc, argv))
          goto Found;
      goto NotFound;
    }
Found: { }
  }
  return true;
NotFound:
  std::string executable_name(argv[0]);
  executable_name = executable_name.substr(executable_name.find_last_of("/\\") + 1);
  os << executable_name << ": Unrecognized option or bad format `" << argv[i] << "'" << endl;
  return false;
}

void MznSolver::flatten()
{
  getFlt()->set_flag_verbose(get_flag_verbose());
  getFlt()->set_flag_statistics(get_flag_statistics());
  clock_t tm01 = clock();
  getFlt()->flatten();
  /// The following message tells mzn-test.py that flattening succeeded.
  if (get_flag_verbose())
    std::cerr << "  Flattening done, " << timeDiff(clock(), tm01) << std::endl;
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
    getSI()->printStatisticsLine(cout, 1);
}


