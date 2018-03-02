/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_EXCEPTION_HH__
#define __MINIZINC_EXCEPTION_HH__

#include <exception>

#include <string>

namespace MiniZinc {

  class Exception : public std::exception {
  protected:
    std::string _msg;
  public:
    Exception(const std::string& msg) : _msg(msg) {}
    virtual ~Exception(void) throw() {}
    virtual const char* what(void) const throw()  = 0;
    const std::string& msg(void) const { return _msg; }
  };

  class InternalError : public Exception {
  public:
    InternalError(const std::string& msg) : Exception(msg) {}
    ~InternalError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: internal error";
    }
  };

  class Error : public Exception {
  public:
    Error(const std::string& msg) : Exception(msg) {}
    ~Error(void) throw() {}
    virtual const char* what(void) const throw() { return ""; }
  };
  
  class ArithmeticError : public Exception {
  public:
    ArithmeticError(const std::string& msg)
    : Exception(msg) {}
    virtual ~ArithmeticError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: arithmetic error";
    }
  };
  
}

#endif
