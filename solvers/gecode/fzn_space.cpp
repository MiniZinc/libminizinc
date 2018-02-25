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
    for(unsigned int i=0; i<iv.size(); i++)
      iv[i].update(*this, f.iv[i]);
    for(unsigned int i=0; i<f.iv_introduced.size(); i++)
      iv_introduced.push_back(f.iv_introduced[i]);
    for(unsigned int i=0; i<f.iv_defined.size(); i++)
      iv_defined.push_back(f.iv_defined[i]);
    if(f._copyAuxVars) {
      IntVarArgs iva;
      for (int i=0; i<f.iv_aux.size(); i++) {
        if (!f.iv_aux[i].assigned()) {
          iva << IntVar();
          iva[iva.size()-1].update(*this, f.iv_aux[i]);
        }
      }
      iv_aux = IntVarArray(*this, iva);
    }

    // boolean variables
    bv.resize(f.bv.size());
    for(unsigned int i=0; i<bv.size(); i++)
      bv[i].update(*this, f.bv[i]);
    if (f._copyAuxVars) {
      BoolVarArgs bva;
      for (int i=0; i<f.bv_aux.size(); i++) {
        if (!f.bv_aux[i].assigned()) {
          bva << BoolVar();
          bva[bva.size()-1].update(*this, f.bv_aux[i]);
        }
      }
      bv_aux = BoolVarArray(*this, bva);
    }
    for(unsigned int i=0; i<f.bv_introduced.size(); i++)
      bv_introduced.push_back(f.bv_introduced[i]);


#ifdef GECODE_HAS_SET_VARS
    sv.resize(f.sv.size());
    for(unsigned int i=0; i<sv.size(); i++)
      sv[i].update(*this, f.sv[i]);
    if (f._copyAuxVars) {
      SetVarArgs sva;
      for (int i=0; i<f.sv_aux.size(); i++) {
        if (!f.sv_aux[i].assigned()) {
          sva << SetVar();
          sva[sva.size()-1].update(*this, f.sv_aux[i]);
        }
      }
      sv_aux = SetVarArray(*this, sva);
    }
    for(unsigned int i=0; i<f.sv_introduced.size(); i++)
      sv_introduced.push_back(f.sv_introduced[i]);
#endif

#ifdef GECODE_HAS_FLOAT_VARS
    fv.resize(f.fv.size());
    for(unsigned int i=0; i<fv.size(); i++)
      fv[i].update(*this, f.fv[i]);
    if (f._copyAuxVars) {
      FloatVarArgs fva;
      for (int i=0; i<f.fv_aux.size(); i++) {
        if (!f.fv_aux[i].assigned()) {
          fva << FloatVar();
          fva[fva.size()-1].update(*this, f.fv_aux[i]);
        }
      }
      fv_aux = FloatVarArray(*this, fva);
    }
#endif

    _optVarIsInt = f._optVarIsInt;
    _optVarIdx = f._optVarIdx;
    _copyAuxVars = f._copyAuxVars;
    _solveType = f._solveType;
  }


  Gecode::Space*
  FznSpace::copy(void) {
    return new FznSpace(*this);
  }


  void
    FznSpace::constrain(const Space& s) {
      if (_optVarIsInt) {
        if (_solveType == MiniZinc::SolveI::SolveType::ST_MIN)
          rel(*this, iv[_optVarIdx], IRT_LE,
              static_cast<const FznSpace*>(&s)->iv[_optVarIdx].val());
        else if (_solveType == MiniZinc::SolveI::SolveType::ST_MAX)
          rel(*this, iv[_optVarIdx], IRT_GR,
              static_cast<const FznSpace*>(&s)->iv[_optVarIdx].val());
      } else {
#ifdef GECODE_HAS_FLOAT_VARS
        if (_solveType == MiniZinc::SolveI::SolveType::ST_MIN)
          rel(*this, fv[_optVarIdx], FRT_LE,
              static_cast<const FznSpace*>(&s)->fv[_optVarIdx].val());
        else if (_solveType == MiniZinc::SolveI::SolveType::ST_MAX)
          rel(*this, fv[_optVarIdx], FRT_GR,
              static_cast<const FznSpace*>(&s)->fv[_optVarIdx].val());
#endif
      }
    }

}
