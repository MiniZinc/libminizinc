/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_ASTEXCEPTION_HH__
#define __MINIZINC_ASTEXCEPTION_HH__

#include <minizinc/exception.hh>
#include <minizinc/ast.hh>
#include <string>

namespace MiniZinc {
  
  class LocationException : public Exception {
  protected:
    Location _loc;
  public:
    LocationException(const Location& loc, const std::string& msg)
      : Exception(msg), _loc(loc) {}
    virtual ~LocationException(void) throw() {}
    const Location& loc(void) const { return _loc; }
  };

  class TypeError : public LocationException {
  public:
    TypeError(const Location& loc, const std::string& msg)
      : LocationException(loc,msg) {}
    ~TypeError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: type error";
    }
  };

  class EvalError : public LocationException {
  public:
    EvalError(const Location& loc, const std::string& msg)
      : LocationException(loc,msg) {}
    EvalError(const Location& loc, const std::string& msg, const ASTString& name)
      : LocationException(loc,msg+" '"+name.str()+"'") {}
    ~EvalError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: evaluation error";
    }
  };

  class FlatteningError : public LocationException {
  public:
    FlatteningError(const Location& loc, const std::string& msg)
      : LocationException(loc,msg) {}
    ~FlatteningError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: flattening error";
    }
  };

}

#endif
