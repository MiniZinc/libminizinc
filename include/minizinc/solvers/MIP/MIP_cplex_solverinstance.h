 
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_MIP_CPLEX_SOLVER_INSTANCE_H__
#define __MINIZINC_MIP_CPLEX_SOLVER_INSTANCE_H__

#include <minizinc/solvers/MIP/MIP_solverinstance.h>
#include <ilcplex/cplex.h>     // add -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 to the 1st call of cmake

namespace MiniZinc {

  class MIP_cplex_solverinstance : public MIP_solverinstance {
    public:
      bool processOption(int& i, int argc, const char** argv);
      void printVersion(ostream& );
      void printHelp(ostream& );
//       Statistics& getStatistics() { return _statistics; }

      void solve();

//      IloConstraintArray *userCuts, *lazyConstraints;

    protected:
            /// SOLVER PARAMS ????
      int nThreads=1;
      string sExportModel;
      double nTimeout=-1;
      double nWorkMemLimit=-1;
      string sReadParams;
      string sWriteParams;
      bool flag_all_solutions = false;

  };

}

#endif  // __MINIZINC_MIP_SOLVER_INSTANCE_H__
