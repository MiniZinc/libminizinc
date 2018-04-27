/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_STL_MAP_SET_HH__
#define __MINIZINC_STL_MAP_SET_HH__

#include <minizinc/config.hh>

#ifdef MZN_NEED_TR1

#include <tr1/unordered_map>
#include <tr1/unordered_set>

#define HASH_NAMESPACE std::tr1
#define OPEN_HASH_NAMESPACE namespace std { namespace tr1
#define CLOSE_HASH_NAMESPACE }
#define UNORDERED_NAMESPACE std::tr1

#else

#include <unordered_map>
#include <unordered_set>

#define HASH_NAMESPACE std
#define OPEN_HASH_NAMESPACE namespace std
#define CLOSE_HASH_NAMESPACE
#define UNORDERED_NAMESPACE std

#endif

#ifdef MZN_HAS_LLROUND
#include <cmath>
namespace MiniZinc {
  inline
  long long int round_to_longlong(double v) {
    return ::llround(v);
  }
}
#else
namespace MiniZinc {
  inline
  long long int round_to_longlong(double v) {
    return static_cast<long long int>(v < 0 ? v-0.5 : v+0.5);
  }
}
#endif

#endif
