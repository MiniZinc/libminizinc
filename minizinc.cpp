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
#ifdef HAS_FZN
#include <minizinc/solvers/fzn_solverfactory.hh>
#include <minizinc/solvers/mzn_solverfactory.hh>
namespace {
  FZN_SolverFactoryInitialiser _fzn_init;
  MZN_SolverFactoryInitialiser _mzn_init;
}
#endif

int main(int argc, const char** argv) {

  clock_t starttime = std::clock(), endTime;
  bool fSuccess = false;

  try {
    MznSolver slv(std::cout,std::cerr);
    try {
      fSuccess = slv.run(argc,argv);
    } catch (const LocationException& e) {
      if (slv.get_flag_verbose())
        std::cerr << std::endl;
      std::cerr << e.loc() << ":" << std::endl;
      std::cerr << e.what() << ": " << e.msg() << std::endl;
    } catch (const Exception& e) {
      if (slv.get_flag_verbose())
        std::cerr << std::endl;
      std::string what = e.what();
      std::cerr << what << (what.empty() ? "" : ": ") << e.msg() << std::endl;
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
    
    endTime = clock();
    if (slv.get_flag_verbose()) {
      std::cerr << "   Done (";
      cerr << "overall time " << timeDiff(endTime, starttime) << ")." << std::endl;
    }
    return !fSuccess;
  } catch (const Exception& e) {
    std::string what = e.what();
    std::cerr << what << (what.empty() ? "" : ": ") << e.msg() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}   // int main()
