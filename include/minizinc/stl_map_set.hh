/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
