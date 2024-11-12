/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/gecode/fzn_space.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

using namespace Gecode;

namespace MiniZinc {

FznSpace::FznSpace(FznSpace& f) : Space(f) {
  // integer variables
  iv.resize(f.iv.size());
  for (unsigned int i = 0; i < iv.size(); i++) {
    iv[i].update(*this, f.iv[i]);
  }

  ivIntroduced = f.ivIntroduced;
  ivDefined = f.ivDefined;
  
  if (f.copyAuxVars) {
    IntVarArgs iva;
    for (auto& i : f.ivAux) {
      if (!i.assigned()) {
        iva << IntVar();
        iva[iva.size() - 1].update(*this, i);
      }
    }
    ivAux = IntVarArray(*this, iva);
  }

  // boolean variables
  bv.resize(f.bv.size());
  for (unsigned int i = 0; i < bv.size(); i++) {
    bv[i].update(*this, f.bv[i]);
  }
  if (f.copyAuxVars) {
    BoolVarArgs bva;
    for (auto& i : f.bvAux) {
      if (!i.assigned()) {
        bva << BoolVar();
        bva[bva.size() - 1].update(*this, i);
      }
    }
    bvAux = BoolVarArray(*this, bva);
  }
  bvIntroduced = f.bvIntroduced;

#ifdef GECODE_HAS_SET_VARS
  sv.resize(f.sv.size());
  for (unsigned int i = 0; i < sv.size(); i++) {
    sv[i].update(*this, f.sv[i]);
  }
  if (f.copyAuxVars) {
    SetVarArgs sva;
    for (auto& i : f.svAux) {
      if (!i.assigned()) {
        sva << SetVar();
        sva[sva.size() - 1].update(*this, i);
      }
    }
    svAux = SetVarArray(*this, sva);
  }

  svIntroduced = f.svIntroduced;

#ifdef GECODE_HAS_FLOAT_VARS
  fv.resize(f.fv.size());
  for (unsigned int i = 0; i < fv.size(); i++) {
    fv[i].update(*this, f.fv[i]);
  }
  if (f.copyAuxVars) {
    FloatVarArgs fva;
    for (auto& i : f.fvAux) {
      if (!i.assigned()) {
        fva << FloatVar();
        fva[fva.size() - 1].update(*this, i);
      }
    }
    fvAux = FloatVarArray(*this, fva);
  }
#endif

  optVarIsInt = f.optVarIsInt;
  optVarIdx = f.optVarIdx;
  copyAuxVars = f.copyAuxVars;
  solveType = f.solveType;
}

Gecode::Space* FznSpace::copy() { return new FznSpace(*this); }

void FznSpace::constrain(const Space& s) {
  if (optVarIsInt) {
    if (solveType == MiniZinc::SolveI::SolveType::ST_MIN) {
      rel(*this, iv[optVarIdx], IRT_LE, static_cast<const FznSpace*>(&s)->iv[optVarIdx].val());
    } else if (solveType == MiniZinc::SolveI::SolveType::ST_MAX) {
      rel(*this, iv[optVarIdx], IRT_GR, static_cast<const FznSpace*>(&s)->iv[optVarIdx].val());
    }
  } else {
#ifdef GECODE_HAS_FLOAT_VARS
    if (solveType == MiniZinc::SolveI::SolveType::ST_MIN) {
      rel(*this, fv[optVarIdx], FRT_LE, static_cast<const FznSpace*>(&s)->fv[optVarIdx].val());
    } else if (solveType == MiniZinc::SolveI::SolveType::ST_MAX) {
      rel(*this, fv[optVarIdx], FRT_GR, static_cast<const FznSpace*>(&s)->fv[optVarIdx].val());
    }
#endif
  }
}

}  // namespace MiniZinc
