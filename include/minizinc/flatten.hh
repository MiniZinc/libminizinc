/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FLATTEN_HH__
#define __MINIZINC_FLATTEN_HH__

#include <minizinc/model.hh>
#include <minizinc/astexception.hh>
#include <minizinc/copy.hh> // for the extended Env constructor
#include <minizinc/hash.hh> // for the extended Env constructor
//#include <minizinc/optimize.hh> // for VarOccurrences in extended Env constructor

namespace MiniZinc {

  /// Exception thrown for errors during flattening
  class FlatteningError : public LocationException {
  public:
    FlatteningError(EnvI& env, const Location& loc, const std::string& msg);
    ~FlatteningError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: flattening error";
    }
  };

  /// Options for the flattener
  struct FlatteningOptions {
    /// Variables support only ranges, convert holes into != constraints
    bool onlyRangeDomains;
    /// Keep output in resulting flat model
    bool keepOutputInFzn;
    /// Default constructor
    FlatteningOptions(void) : onlyRangeDomains(false), keepOutputInFzn(false) {}
  };
  
  /// Flatten model \a m
  void flatten(Env& m, FlatteningOptions opt = FlatteningOptions());

  /// Translate \a m into old FlatZinc syntax
  void oldflatzinc(Env& m);
  
}

#endif
