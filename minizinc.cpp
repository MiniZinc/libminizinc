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

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

#include <minizinc/solver.hh>

#ifdef FLATTEN_ONLY
#define IS_MZN2FZN true
#else
#define IS_MZN2FZN false
#endif

using namespace std;
using namespace MiniZinc;


int main(int argc, const char** argv) {

  /// Initializing specified solver factories
  /// Gecode has to be 1st for multi-pass
#ifdef HAS_GECODE
  static unique_ptr<SolverFactory>
    pFactoryGECODE( SolverFactory::createF_GECODE() );
#endif
#ifdef HAS_FZN
  static unique_ptr<SolverFactory>
    pFactoryFZN( SolverFactory::createF_FZN() );
#endif
#ifdef HAS_CHUFFED
  static unique_ptr<SolverFactory>
    pFactoryCHUFFED( SolverFactory::createF_CHUFFED() );
#endif
#ifdef HAS_MIP
  static unique_ptr<SolverFactory>
    pFactoryMIP( SolverFactory::createF_MIP() );
#endif

  clock_t starttime = std::clock(), endTime;
  bool fSuccess = false;
  
  MznSolver slv(IS_MZN2FZN);
  try {
    
    slv.addFlattener();
    if (!slv.processOptions(argc, argv, cerr)) {
      slv.printHelp(cerr);
      exit(EXIT_FAILURE);
    }
    slv.flatten();
    
    if (SolverInstance::UNKNOWN == slv.getFlt()->status)
    {
      fSuccess = true;
      if ( !slv.ifMzn2Fzn() ) {          // only then
        // GCLock lock;                  // better locally, to enable cleanup after ProcessFlt()
        slv.addSolverInterface();
        slv.solve();
      }
    } else {
      fSuccess = (SolverInstance::ERROR != slv.getFlt()->status);
      if ( !slv.ifMzn2Fzn() )
        slv.s2out.evalStatus( slv.getFlt()->status );
    }                                   //  Add evalOutput() here?   TODO
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
  
  if ( !slv.ifMzn2Fzn() ) {
    endTime = clock();
    if (slv.get_flag_verbose()) {
      std::cerr << "   Done (";
      cerr << "overall time " << timeDiff(endTime, starttime) << ")." << std::endl;
    }
  }
  return !fSuccess;
}   // int main()
