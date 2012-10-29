/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <exception>

#include <minizinc/ast.hh>
#include <string>

namespace MiniZinc {

  class TypeError : public std::exception {
  protected:
    Location _loc;
    std::string _msg;
  public:
    TypeError(const Location& loc, const std::string& msg)
      : _loc(loc), _msg(msg) {}
    ~TypeError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: type error";
    }
    const Location& loc(void) const { return _loc; }
    const std::string& msg(void) const { return _msg; }
  };

  class EvalError : public std::exception {
  protected:
    Location _loc;
    std::string _msg;
  public:
    EvalError(const Location& loc, const std::string& msg)
      : _loc(loc), _msg(msg) {}
    ~EvalError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: evaluation error";
    }
    const Location& loc(void) const { return _loc; }
    const std::string& msg(void) const { return _msg; }
  };

}
