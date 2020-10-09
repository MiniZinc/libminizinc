/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifdef HAS_GECODE

// Regex Parser Requirements
#include <minizinc/astmap.hh>
#include <minizinc/aststring.hh>
#include <minizinc/values.hh>

#include <memory>
#include <set>

#include <gecode/minimodel.hh>
#undef ERROR

// This is a workaround for a bug in flex that only shows up
// with the Microsoft C++ compiler
#if defined(_MSC_VER)
#define YY_NO_UNISTD_H
#ifdef __cplusplus
extern "C" int isatty(int);
#endif
#endif

// The Microsoft C++ compiler marks certain functions as deprecated,
// so let's take the alternative definitions
#if defined(_MSC_VER)
#define strdup _strdup
#define fileno _fileno
#endif

// Anonymous struct for when yyparse is exported
typedef struct REContext REContext;
// Parser generated header
#include <minizinc/support/regex_parser.tab.hh>

using namespace Gecode;
using namespace MiniZinc;

// Parsing function
std::unique_ptr<REG> regex_from_string(const std::string& regex_str, const IntSetVal& domain);

#endif  // HAS_GECODE
