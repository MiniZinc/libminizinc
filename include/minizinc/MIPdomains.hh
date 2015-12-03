/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_mipDOMAINS_HH__
#define __MINIZINC_mipDOMAINS_HH__

#include <minizinc/flatten.hh>
#include <minizinc/hash.hh>
#include <minizinc/stl_map_set.hh>
#include <set>

namespace MiniZinc {

  /// Linearize domain constraints in \a env
  void MIPdomains(Env& env);
  
  enum EnumReifType { RIT_None, RIT_Static, RIT_Reif, RIT_Halfreif1, RIT_Halfreif0 };
  enum EnumConstrType { CT_None, CT_Comparison, CT_SetIn, CT_Encode };
  enum EnumCmpType { CMPT_None=0, CMPT_LE=-4, CMPT_GE=4, CMPT_EQ=1, CMPT_NE=3, CMPT_LT=-5, CMPT_GT=5,
                        CMPT_LE_0=-6, CMPT_GE_0=6, CMPT_EQ_0=2, CMPT_LT_0=-7, CMPT_GT_0=7 };
  enum EnumVarType { VT_None, VT_Int, VT_Float };

  /// struct DomainCallType describes & characterizes a possible domain constr call
  struct DCT {
    const char* sFuncName=0;
    const std::vector<Type>& aParams;
//     unsigned iItem;          // call's item number in the flat
    EnumReifType nReifType = RIT_None;   // 0/static/halfreif/reif
    EnumConstrType nConstrType = CT_None;  //
    EnumCmpType nCmpType = CMPT_None;
    EnumVarType nVarType = VT_None;
    FunctionI* &pfi;
//     double dEps = -1.0;
    DCT(const char* fn, const std::vector<Type>& prm,
                  EnumReifType er, EnumConstrType ec, EnumCmpType ecmp, EnumVarType ev,
                  FunctionI* &pfi__
        )
      : sFuncName(fn), aParams(prm), nReifType(er), nConstrType(ec), nCmpType(ecmp),
        nVarType(ev), pfi(pfi__) { }
  };

  template <class N>
  struct Interval {
    constexpr static N infMinus() {
      return ( std::numeric_limits<N>::has_infinity ) ?
        -std::numeric_limits<N>::infinity() :
      std::numeric_limits<N>::lowest();
    }
    constexpr static N infPlus() {
      return ( std::numeric_limits<N>::has_infinity ) ?
        std::numeric_limits<N>::infinity() :
      std::numeric_limits<N>::max();
    }
    N left = infMinus(), right = infPlus();
    Interval(N a=infMinus(), N b=infPlus()) : left(a), right(b) {
      if ( left > right )
        throw std::string("Interval: lb>ub");
    }
    bool operator<( const Interval& intv ) const {
      if ( left < intv.left ) {
//         assert( right <= intv.left );              // assume disjoint
        return true;
      }
      return false;
    }
  };
  typedef Interval<double> IntvReal;

  template <class N>
  std::ostream& operator<< (std::ostream& os, const Interval<N>& ii) {
    os << "[ " << ii.left << ", " << ii.right << " ] ";
    return os;
  }
  
  template <class N>
  class SetOfIntervals : public std::multiset<Interval<N> > {
  public:
    typedef std::multiset<Interval<N> > Base;
    typedef typename Base::iterator iterator;
    SetOfIntervals() : Base() { }
    SetOfIntervals(std::initializer_list<Interval<N> > il) : Base( il )  { }
    template <class Iter>
    SetOfIntervals( Iter i1, Iter i2 ) : Base( i1, i2 )  { }
    template <class N1>
    void intersect(const SetOfIntervals<N1>& s2);
    /// Assumes open intervals to cut out from closed
    template <class N1>
    void cutDeltas( const SetOfIntervals<N1>& s2, N1 delta );
    /// Cut out an open interval from a set of closed ones (except for infinities)
    void cutOut(const Interval<N>& intv);
    typedef std::pair<iterator, iterator> SplitResult;
    SplitResult split(iterator& it, N pos);
    bool checkFiniteBounds();
    bool checkDisjunctStrict();
    Interval<N> getBounds();
  };  // class SetOfIntervals
  typedef SetOfIntervals<double> SetOfIntvReal;
  
  template <class N>
  std::ostream& operator<< (std::ostream& os, const SetOfIntervals<N>& soi) {
    os << "[[ ";
    for ( auto& ii : soi )
      os << "[ " << ii.left << ", " << ii.right << " ] ";
    os << "]]";
    return os;
  }
  
}

#endif
