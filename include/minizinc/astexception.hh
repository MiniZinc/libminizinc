/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/exception.hh>
#include <minizinc/model.hh>

#include <string>

namespace MiniZinc {

class SyntaxError : public Exception {
protected:
  Location _loc;

public:
  SyntaxError(const Location& loc, const std::string& msg) : Exception(msg), _loc(loc) {}
  ~SyntaxError() throw() override {}
  const char* what() const throw() override { return "MiniZinc: syntax error"; }
  const Location& loc() const { return _loc; }
};

class LocationException : public Exception {
protected:
  Location _loc;

public:
  LocationException(EnvI& env, const Location& loc, const std::string& msg);
  ~LocationException() throw() override {}
  const Location& loc() const { return _loc; }
};

class TypeError : public LocationException {
public:
  TypeError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env, loc, msg) {}
  ~TypeError() throw() override {}
  const char* what() const throw() override { return "MiniZinc: type error"; }
};

class EvalError : public LocationException {
public:
  EvalError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env, loc, msg) {}
  EvalError(EnvI& env, const Location& loc, const std::string& msg, const ASTString& name)
      : LocationException(env, loc, "") {
    std::ostringstream ss;
    ss << msg << " '" << name << "'";
    _msg = ss.str();
  }
  ~EvalError() throw() override {}
  const char* what() const throw() override { return "MiniZinc: evaluation error"; }
};

class ModelInconsistent : public LocationException {
public:
  ModelInconsistent(EnvI& env, const Location& loc, const std::string& msg = "")
      : LocationException(env, loc,
                          "model inconsistency detected" + (msg.empty() ? msg : ":  ") + msg) {}
  ~ModelInconsistent() throw() override {}
  const char* what() const throw() override { return "MiniZinc: warning"; }
};

class ResultUndefinedError : public LocationException {
public:
  ResultUndefinedError(EnvI& env, const Location& loc, const std::string& msg);
  ~ResultUndefinedError() throw() override {}
  const char* what() const throw() override {
    return "MiniZinc: result of evaluation is undefined";
  }
};

}  // namespace MiniZinc
