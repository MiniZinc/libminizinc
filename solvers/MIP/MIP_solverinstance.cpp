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

#include <minizinc/algorithms/min_cut.h>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

std::string MIPWrapper::getMznLib() { return "-Glinear"; }

namespace MiniZinc {
namespace SCIPConstraints {

bool check_ann_user_cut(const Call* call) {
  if (!call->ann().isEmpty()) {
    if (call->ann().contains(constants().ann.user_cut)) {
      return true;
    }
  }
  return false;
}
bool check_ann_lazy_constraint(const Call* call) {
  if (!call->ann().isEmpty()) {
    if (call->ann().contains(constants().ann.lazy_constraint)) {
      return true;
    }
  }
  return false;
}
int get_mask_cons_type(const Call* call) {
  int mask = 0;
  const bool fUC = check_ann_user_cut(call);
  const bool fLC = check_ann_lazy_constraint(call);
  if (fUC) {
    mask |= MIPWrapper::MaskConsType_Usercut;
  }
  if (fLC) {
    mask |= MIPWrapper::MaskConsType_Lazy;
  }
  if (!fUC && !fLC) {
    mask |= MIPWrapper::MaskConsType_Normal;
  }
  return mask;
  //       return MIPWrapper::MaskConsType_Normal;    // recognition fails
}
}  // namespace SCIPConstraints
}  // namespace MiniZinc

using namespace MiniZinc;

void XBZCutGen::generate(const MIPWrapper::Output& slvOut, MIPWrapper::CutInput& cutsIn) {
  assert(_pMIP);
  const int n = static_cast<int>(varX.size());
  assert(n == varB.size());
  MIPWrapper::CutDef cut(MIPWrapper::GQ, MIPWrapper::MaskConsType_Usercut);
  cut.addVar(varZ, -1.0);
  for (int i = 0; i < n; ++i) {
    const int ix = varX[i];
    const int ib = varB[i];
    assert(ix >= 0 && ix < slvOut.nCols);
    assert(ib >= 0 && ib < slvOut.nCols);
    const double theXi = slvOut.x[ix];
    const double theBi = slvOut.x[ib];
    const double LBXi = _pMIP->colLB[ix];
    const double UBXi = _pMIP->colUB[ix];  // tighter bounds from presolve?  TODO
    bool fi = (theXi + LBXi * (theBi - 1.0) - UBXi * theBi < 0.0);
    if (fi) {
      cut.addVar(ix, 1.0);
      cut.addVar(ib, LBXi);
      cut.rhs += LBXi;
    } else {
      cut.addVar(ib, UBXi);
    }
  }
  double dViol = cut.computeViol(slvOut.x, slvOut.nCols);
  if (dViol > 0.01) {  // ?? PARAM?  TODO
    cutsIn.push_back(cut);
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

void XBZCutGen::print(ostream& os) {
  os << varZ << '\n' << varX.size() << '\n';
  for (int i : varX) {
    os << i << ' ';
  }
  os << endl;
  for (int i : varB) {
    os << i << ' ';
  }
  os << endl;
}

std::string SECCutGen::validate() const {
  std::ostringstream oss;
  /// Check that diagonal flows are 0
  for (int i = 0; i < nN; ++i) {
    if (_pMIP->colUB[varXij[i * nN + i]] > 0.0) {
      oss << "SECutGen with " << nN << " cities: diagonal flow " << (i + 1)
          << " has UB=" << _pMIP->colUB[varXij[i * nN + i]] << "\n";
    }
  }
  return oss.str();
}

void SECCutGen::generate(const MIPWrapper::Output& slvOut, MIPWrapper::CutInput& cutsIn) {
  assert(_pMIP);
  /// Extract graph, converting to undirected
  typedef map<pair<int, int>, double> TMapFlow;
  TMapFlow mapFlow;
  for (int i = 0; i < nN; ++i) {
    for (int j = 0; j < nN; ++j) {
      const double xij = slvOut.x[varXij[nN * i + j]];
      if (i == j) {
        MZN_ASSERT_HARD_MSG(1e-4 > fabs(xij),
                            "circuit: X[" << (i + 1) << ", " << (j + 1) << "]==" << xij);
      }
      MZN_ASSERT_HARD_MSG(
          -1e-4 < xij && 1.0 + 1e-4 > xij,  // adjusted from 1e-6 to 1e-4 for CBC. 7.8.19
          "circuit: X[" << (i + 1) << ", " << (j + 1) << "]==" << xij);
      if (1e-4 <= xij) {
        mapFlow[make_pair(min(i, j), max(i, j))] += xij;
      }
    }
  }
  /// Invoking Min Cut
  //   cerr << "  MIN CUT... " << flush;
  Algorithms::MinCut mc;
  mc.nNodes = nN;
  mc.edges.reserve(mapFlow.size());
  mc.weights.reserve(mapFlow.size());
  for (const auto& mf : mapFlow) {
    mc.edges.push_back(mf.first);
    mc.weights.push_back(mf.second);
  }
  Algorithms::MinCut::solve();
  /// Check if violation
  if (mc.wMinCut <= 1.999) {
    MIPWrapper::CutDef cut(MIPWrapper::GQ,
                           MIPWrapper::MaskConsType_Lazy | MIPWrapper::MaskConsType_Usercut);
    cut.rhs = 1.0;
    int nCutSize = 0;
    constexpr int nElemPrint = 20;
    // cerr << "  CUT: [ ";
    for (int i = 0; i < nN; ++i) {
      if (mc.parities[i]) {
        ++nCutSize;
        // if ( nCutSize<=nElemPrint )
        //  cerr << (i+1) << ", ";
        // else if ( nCutSize==nElemPrint+1 )
        //  cerr << "...";
        for (int j = 0; j < nN; ++j) {
          if (!mc.parities[j]) {
            cut.addVar(varXij[nN * i + j], 1.0);
          }
        }
      }
    }
    // cerr << "]. " << flush;
    double dViol = cut.computeViol(slvOut.x, slvOut.nCols);
    if (dViol > 0.0001) {  // ?? PARAM?  TODO. See also min cut value required
      cutsIn.push_back(cut);
      /* cerr << "  SEC: viol=" << dViol
        << "  N NODES: " << nN
        << "  |X|: : " << nCutSize
        << flush; */
    } else {
      MZN_ASSERT_HARD_MSG(0, "  SEC cut: N nodes = " << nN << ": violation = " << dViol
                                                     << ": too small compared to the min-cut value "
                                                     << (2.0 - mc.wMinCut));
    }
  }
}

void SECCutGen::print(ostream& /*os*/) {}
