// * -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This is the main file for a mzn-cplex solver using a unified
 * linearization module && a flexible flattener-to-solver interface
 */

/// TODO Quadratic terms, even CBC

#ifdef _MSC_VER 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include <chrono>

using namespace std;

#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
  namespace SCIPConstraints {
    
    bool CheckAnnUserCut(const Call* call) {
      if(!call->ann().isEmpty()) {
        if(call->ann().contains(constants().ann.user_cut)) {
          return true;
        }
      }
      return false;
    }
    bool CheckAnnLazyConstraint(const Call* call) {
      if(!call->ann().isEmpty()) {
        if(call->ann().contains(constants().ann.lazy_constraint)) {
          return true;
        }
      }
      return false;
    }
    int GetMaskConsType(const Call* call) {
      int mask=0;
      const bool fUC = CheckAnnUserCut(call);
      const bool fLC = CheckAnnLazyConstraint(call);
      if (fUC) {
        mask |= MIP_wrapper::MaskConsType_Usercut;
      }
      if (fLC) {
        mask |= MIP_wrapper::MaskConsType_Lazy;
      }
      if (!fUC && !fLC)
        mask |= MIP_wrapper::MaskConsType_Normal;
      return mask;
      //       return MIP_wrapper::MaskConsType_Normal;    // recognition fails
    }
  }
}

using namespace MiniZinc;



void XBZCutGen::generate(const MIP_wrapper::Output& slvOut, MIP_wrapper::CutInput& cutsIn) {
  assert( pMIP );
  const int n = varX.size();
  assert( n==varB.size() );
  MIP_wrapper::CutDef cut( MIP_wrapper::GQ, MIP_wrapper::MaskConsType_Usercut );
  cut.addVar( varZ, -1.0 );
  for ( int i=0; i<n; ++i ) {
    const int ix = varX[ i ];
    const int ib = varB[ i ];
    assert( ix>=0 && ix<slvOut.nCols );
    assert( ib>=0 && ib<slvOut.nCols );
    const double theXi = slvOut.x[ ix ];
    const double theBi = slvOut.x[ ib ];
    const double LBXi = pMIP->colLB[ ix ];
    const double UBXi = pMIP->colUB[ ix ];  // tighter bounds from presolve?  TODO
    bool fi = ( theXi + LBXi * ( theBi - 1.0 ) - UBXi * theBi < 0.0 );
    if ( fi ) {
      cut.addVar( ix, 1.0 );
      cut.addVar( ib, LBXi );
      cut.rhs += LBXi;
    } else {
      cut.addVar( ib, UBXi );
    }
  }
  double dViol = cut.computeViol( slvOut.x, slvOut.nCols );
  if ( dViol > 0.01 ) {   // ?? PARAM?  TODO
    cutsIn.push_back( cut );
    cerr << " vi" << dViol << flush;
//     cout << cut.rmatind.size() << ' '
//       << cut.rhs << "  cutlen, rhs. (Sense fixed to GQ) " << endl;
//     for ( int i=0; i<cut.rmatind.size(); ++i )
//       cout << cut.rmatind[i] << ' ';
//     cout << endl;
//     for ( int i=0; i<cut.rmatind.size(); ++i )
//       cout << cut.rmatval[i] << ' ';
//     cout << endl;
  }
}

void XBZCutGen::print( ostream& os )
{
  os
    << varZ << '\n'
    << varX.size() << '\n';
  for ( int i=0; i<varX.size(); ++i )
    os << varX[i] << ' ';
  os << endl;
  for ( int i=0; i<varB.size(); ++i )
    os << varB[i] << ' ';
  os << endl;
}
