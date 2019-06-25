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
#include <minizinc/utils.hh>
#include <array>
#include <set>

#ifdef _MSC_VER 
#define _CRT_SECURE_NO_WARNINGS
#undef ERROR    // MICROsoft.
#undef min
#undef max
#endif

#define MZN_MIPD__assert_soft( c, e ) \
  do { static int nn=0; \
 if ( !(c) ) if ( ++nn<=1 ) std::cerr << e << std::endl; } while (0)
#define MZN_MIPD__assert_hard( c ) MZN_ASSERT_HARD(	c )
#define MZN_MIPD__assert_hard_msg( c, e ) MZN_ASSERT_HARD_MSG( c, e )
struct MIPD_Infeasibility_Exception {
  std::string msg;
  MIPD_Infeasibility_Exception(const std::string& s) : msg(s) { }
};
#define MZN_MIPD__assert_for_feas( c, e ) \
   do { if ( !(c) ) { std::ostringstream oss; oss << e; throw MIPD_Infeasibility_Exception(oss.str()); } } while (0)


namespace MiniZinc {

  /// Linearize domain constraints in \a env
  void MIPdomains(Env& env, bool fVerbose = false, int=0, double=3.0);
  
}

#endif
