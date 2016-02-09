 
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

int main(int argc, const char** argv) {
  clock_t starttime = std::clock(), endTime;
  bool fSuccess = false;
  
  MznSolver slv;
  try {
    
    slv.addFlattener();
    if (!slv.processOptions(argc, argv)) {
      slv.printHelp();
      exit(EXIT_FAILURE);
    }
    slv.flatten();
    
    if (SolverInstance::UNKNOWN == slv.getFlt()->status)
    {
      fSuccess = true;
      if ( slv.getNSolvers() ) {          // only then
        GCLock lock;
        slv.addSolverInterface();
        slv.solve();
      }
    } else if (SolverInstance::ERROR == slv.getFlt()->status) {
//       slv.s2out.evalStatus( slv.getFlt()->status );
    } else {
      fSuccess = true;
//       slv.s2out.evalStatus( slv.getFlt()->status );
    }   // TODO  Move evalOutput() here?
  } catch (const LocationException& e) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << e.loc() << ":" << std::endl;
    std::cerr << e.what() << ": " << e.msg() << std::endl;
    slv.s2out.evalStatus( SolverInstance::ERROR );
  } catch (const Exception& e) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << e.what() << ": " << e.msg() << std::endl;
    slv.s2out.evalStatus( SolverInstance::ERROR );
  }
  catch (const exception& e) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << e.what() << std::endl;
    slv.s2out.evalStatus( SolverInstance::ERROR );
  }
  catch (...) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << "  UNKNOWN EXCEPTION." << std::endl;
    slv.s2out.evalStatus( SolverInstance::ERROR );
  }

  if ( slv.getNSolvers() ) {
    endTime = clock();
    if (slv.get_flag_verbose()) {
      std::cerr << "   Done (";
      cerr << "overall time " << timeDiff(endTime, starttime) << ")." << std::endl;
    }
  }
  return !fSuccess;
}   // int main()

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


MznSolver::~MznSolver()
{
//   if (si)                         // first the solver
//     CleanupSolverInterface(si);
  // TODO cleanup the used solver interfaces
  si=0;
  if (flt)
    cleanupGlobalFlattener(flt);
  flt=0;
}

bool MznSolver::ifMzn2Fzn() {
#ifdef FLATTEN_ONLY
  return true;
#else
  return 0==getNSolvers();
#endif
}


void MznSolver::addFlattener()
{
  flt = getGlobalFlattener(0==getNSolvers());
  assert(flt);
}

void MznSolver::addSolverInterface()
{
  assert(getGlobalSolverRegistry()->getSolverFactories().size());
  si = getGlobalSolverRegistry()->getSolverFactories().front()->createSI(*flt->getEnv());
  assert(si);
  s2out.initFromEnv( flt->getEnv() );
  si->setSolns2Out( &s2out );
  if (get_flag_verbose())
    cerr
//     << "  ---------------------------------------------------------------------------\n"
    << "      % SOLVING PHASE\n"
    << getGlobalSolverRegistry()->getSolverFactories().front()->getVersion() << endl;  
}

bool MznSolver::processOptions(int argc, const char** argv)
{
  int i=1;
  if (argc < 2)
    return false;
  for (i=1; i<argc; ++i) {
    if (string(argv[i])=="-h" || string(argv[i])=="--help") {
      printHelp();
      std::exit(EXIT_SUCCESS);
    }
    if (string(argv[i])=="--version") {
      getFlt()->printVersion(cout);
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
           it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it)
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
      for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
           it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it)
        if ((*it)->processOption(i, argc, argv))
          goto Found;
      goto NotFound;
    }
Found: { }
  }
  return true;
NotFound:
  cerr << "  Unrecognized option: '" << argv[i] << "'" << endl;
  return false;
}

void MznSolver::printHelp()
{
  if ( !ifMzn2Fzn() )
  cout
    << "NICTA MiniZinc driver.\n"
    << "Usage: <executable>"  //<< argv[0]
    << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...] or just <flat>.fzn" << std::endl;
  else
  cout
    << "NICTA MiniZinc to FlatZinc converter.\n"
    << "Usage: <executable>"  //<< argv[0]
    << "  [<options>] [-I <include path>] <model>.mzn [<data>.dzn ...]" << std::endl;
  cout
    << "Options:" << std::endl
    << "  --help, -h\n    Print this help message." << std::endl
    << "  --version\n    Print version information." << std::endl
    << "  -v, -l, --verbose\n    Print progress/log statements. Note that some solvers may log to stdout." << std::endl
    << "  -s, --statistics\n    Print statistics (to stdout, as comments)." << std::endl;
//   if ( getNSolvers() )
  
  getFlt()->printHelp(cout);
  cout << endl;
  if ( !ifMzn2Fzn() ) {
    s2out.printHelp(cout);
    cout << endl;
  }
  for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
        it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
       (*it)->printHelp(cout);
      cout << endl;
  }
}

void MznSolver::flatten()
{
  getFlt()->set_flag_verbose(get_flag_verbose());
  getFlt()->set_flag_statistics(get_flag_statistics());
  clock_t tm01 = clock();
  getFlt()->flatten();
  if (get_flag_verbose())
    std::cerr << "  Flattening done, " << timeDiff(clock(), tm01) << std::endl;

}

void MznSolver::solve()
{
  GCLock lock;
  getSI()->getOptions().setBoolParam  (constants().opts.verbose.str(),  get_flag_verbose());
  getSI()->getOptions().setBoolParam  (constants().opts.statistics.str(),  get_flag_statistics());
  getSI()->processFlatZinc();
  SolverInstance::Status status = getSI()->solve();
  if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
    getSI()->printSolution();             // What if it's already printed?  TODO
    if ( !getSI()->getSolns2Out()->fStatusPrinted )
      getSI()->getSolns2Out()->evalStatus( status );
  }
  else {
    if ( !getSI()->getSolns2Out()->fStatusPrinted )
      getSI()->getSolns2Out()->evalStatus( status );
    if (get_flag_verbose() || get_flag_statistics())    // it's summary in fact
      printStatistics();
  }
}

void MznSolver::printStatistics()
{ // from flattener too?   TODO
  if (si)
    getSI()->printStatisticsLine(cout, 1);
}


