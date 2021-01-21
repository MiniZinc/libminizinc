/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flatten.hh>
#include <minizinc/hash.hh>
#include <minizinc/utils.hh>

#include <array>
#include <set>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MZN_MIPD_assert_soft(c, e)                                                         \
  do {                                                                                     \
    static int nn = 0;                                                                     \
    if (!(c))                                                                              \
      if (++nn <= 1) std::cerr << e << std::endl; /* NOLINT(bugprone-macro-parentheses) */ \
  } while (0)
#define MZN_MIPD_assert_hard(c) MZN_ASSERT_HARD(c)
#define MZN_MIPD_assert_hard_msg(c, e) MZN_ASSERT_HARD_MSG(c, e)
#define MZN_MIPD_FLATTENING_ERROR_IF_NOT(cond, envi, loc, msg) \
  do {                                                         \
    if (!(cond)) {                                             \
      std::ostringstream oss;                                  \
      oss << msg; /* NOLINT(bugprone-macro-parentheses) */     \
      throw FlatteningError(envi, loc, oss.str());             \
    }                                                          \
  } while (0)
#define MZN_MIPD_ASSERT_FOR_SAT(cond, envi, loc, msg)                             \
  do {                                                                            \
    if (!(cond)) {                                                                \
      std::ostringstream oss;                                                     \
      oss << "from MIPDomains: " << msg; /* NOLINT(bugprone-macro-parentheses) */ \
      throw ModelInconsistent(envi, loc, oss.str());                              \
    }                                                                             \
  } while (0)

//( c, e ) \
//   do { if ( !(c) ) { std::ostringstream oss; oss << e; throw MIPD_Infeasibility_Exception(oss.str()); } } while (0)

namespace MiniZinc {

/// Linearize domain constraints in \a env
void mip_domains(Env& env, bool fVerbose = false, int nmi = 0, double dmd = 3.0);

}  // namespace MiniZinc
