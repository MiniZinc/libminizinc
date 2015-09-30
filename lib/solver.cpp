 
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
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
  clock_t starttime = std::clock();
  bool fSuccess = false;
  
  MznSolver slv;
  try {
    
    slv.addFlattener();
    if (not slv.processOptions(argc, argv)) {
      slv.printHelp();
      exit(EXIT_FAILURE);
    }
    slv.flatten();
    if (SolverInstance::UNKNOWN == slv.getFlt()->status)
    {
      GCLock lock;
      slv.addSolverInterface();
      slv.solve();
    } else if (SolverInstance::UNSAT == slv.getFlt()->status) {
      std::cout << "=====UNSATISFIABLE=====" << std::endl;
    } else {
      cout << "  Flattening produced status " << slv.getFlt()->status << "  TODO" << endl;
    }   // TODO  Move evalOutput() here
    slv.printStatistics();

    fSuccess = true;
  } catch (const LocationException& e) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << e.loc() << ":" << std::endl;
    std::cerr << e.what() << ": " << e.msg() << std::endl;
  } catch (const Exception& e) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << e.what() << ": " << e.msg() << std::endl;
  }
  catch (const exception& e) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    if (slv.get_flag_verbose())
      std::cerr << std::endl;
    std::cerr << "  UNKNOWN EXCEPTION." << std::endl;
  }

  if (slv.get_flag_verbose())
    std::cerr << "Done (overall time " << stoptime(starttime) << ")." << std::endl;
  return not fSuccess;
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
  if (not pSI) {
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

void MznSolver::addFlattener()
{
  flt = getGlobalFlattener(false);
  assert(flt);
}

void MznSolver::addSolverInterface()
{
  assert(getGlobalSolverRegistry()->getSolverFactories().size());
  si = getGlobalSolverRegistry()->getSolverFactories().front()->createSI(*flt->getEnv());
  assert(si);
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
    if (not getFlt()->processOption(i, argc, argv)) {
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
  cerr << "  Unrecognized option:  " << argv[i] << endl;
  return false;
}

void MznSolver::printHelp()
{
  getFlt()->printHelp(cout);
  cout << endl;
  for (auto it = getGlobalSolverRegistry()->getSolverFactories().begin();
        it != getGlobalSolverRegistry()->getSolverFactories().end(); ++it) {
       (*it)->printHelp(cout);
      cout << endl;
  }
}

void MznSolver::flatten()
{
  getFlt()->flatten();
}

void MznSolver::solve()
{
  GCLock lock;
  getSI()->getOptions().setBoolParam  ("verbose",  get_flag_verbose());
  getSI()->processFlatZinc();
  SolverInstance::Status status = getSI()->solve();
  if (status==SolverInstance::SAT || status==SolverInstance::OPT) {
    getSI()->printSolution(cout);             // What if it's already printed?  TODO
    if (status==SolverInstance::OPT)
      std::cout << "==========" << std::endl;
  }
  else if (status==SolverInstance::UNSAT)
      std::cout << "=====UNSATISFIABLE=====" << std::endl;
  else if (status==SolverInstance::UNBND)
      std::cout << "=====UNBOUNDED=====" << std::endl;
  else if (status==SolverInstance::UNSATorUNBND)
      std::cout << "=====UNSATorUNBOUNDED=====" << std::endl;
  else if (status==SolverInstance::UNKNOWN)
      std::cout << "=====UNKNOWN=====" << std::endl;
  else if (status==SolverInstance::ERROR)
      std::cout << "=====ERROR=====" << std::endl;
  else
    std::cout << "=====UNKNOWN_STATUS=====" << std::endl;
}

void MznSolver::printStatistics()
{ // from both flt and si?

}


