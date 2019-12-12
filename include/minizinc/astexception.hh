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
#include <minizinc/model.hh>
#include <string>

namespace MiniZinc {
  
  class SyntaxError : public Exception {
  protected:
    Location _loc;
  public:
    SyntaxError(const Location& loc, const std::string& msg)
    : Exception(msg), _loc(loc) {}
    virtual ~SyntaxError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: syntax error";
    }
    const Location& loc(void) const { return _loc; }
  };

  class LocationException : public Exception {
  protected:
    Location _loc;
  public:
    LocationException(EnvI& env, const Location& loc, const std::string& msg);
    virtual ~LocationException(void) throw() {}
    const Location& loc(void) const { return _loc; }
  };

  class TypeError : public LocationException {
  public:
    TypeError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env,loc,msg) {}
    ~TypeError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: type error";
    }
  };

  class EvalError : public LocationException {
  public:
    EvalError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env,loc,msg) {}
    EvalError(EnvI& env, const Location& loc, const std::string& msg, const ASTString& name)
      : LocationException(env,loc,msg+" '"+name.str()+"'") {}
    ~EvalError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: evaluation error";
    }
  };

  class ModelInconsistent : public LocationException {
  public:
    ModelInconsistent(EnvI& env, const Location& loc, const std::string& msg="")
      : LocationException(env,loc,"model inconsistency detected"
                          + (msg.empty() ? msg : ":  ") + msg) {}
    ~ModelInconsistent(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: warning";
    }
  };

  class ResultUndefinedError : public LocationException {
  public:
    ResultUndefinedError(EnvI& env, const Location& loc, const std::string& msg);
    ~ResultUndefinedError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: result of evaluation is undefined";
    }
  };
  
}

#endif
