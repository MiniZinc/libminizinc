/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_REGEX_HH__
#define __MINIZINC_REGEX_HH__
#ifdef HAS_GECODE

// Regex Parser Requirements
#include <unordered_map>
#include <unordered_set>
#include <gecode/minimodel.hh>
#include <minizinc/values.hh>

// Parser generated header
#include <minizinc/support/regex_parser.tab.hh>

using namespace Gecode;
using namespace MiniZinc;

// Parsing function
std::unique_ptr<REG> regex_from_string(const std::string& expression, const IntSetVal& domain, const std::unordered_map<std::string, int>& identifiers);

#endif //HAS_GECODE
#endif //__MINIZINC_REGEX_HH__
