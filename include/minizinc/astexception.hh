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
#include <minizinc/gc.hh>
#include <minizinc/model.hh>
#include <minizinc/stackdump.hh>

#include <string>

namespace MiniZinc {

class CyclicIncludeError : public Exception, public GCMarker {
protected:
  std::vector<ASTString> _cycle;

  void mark() override {
    for (auto s : _cycle) {
      s.mark();
    }
  }

public:
  CyclicIncludeError(std::vector<ASTString> cycle) : Exception(""), _cycle(std::move(cycle)) {}
  ~CyclicIncludeError() throw() override {}
  const char* what() const throw() override { return "cyclic include error"; }

  void print(std::ostream& os) const override;
  void json(std::ostream& os) const override;
};

class LocationException : public Exception, public GCMarker {
protected:
  StackDump _stack;
  Location _loc;
  bool _dumpStack = false;

  void mark() override {
    _loc.mark();
    _stack.mark();
  }

public:
  LocationException(const Location& loc, const std::string& msg);
  LocationException(EnvI& env, const Location& loc, const std::string& msg);
  ~LocationException() throw() override {}
  const Location& loc() const { return _loc; }

  bool dumpStack() const { return _dumpStack; }
  void dumpStack(bool dump) { _dumpStack = dump; }

  void print(std::ostream& os) const override;
  void json(std::ostream& os) const override;
};

class SyntaxError : public LocationException {
protected:
  std::string _currentLine;
  std::vector<ASTString> _includeStack;

  void mark() override {
    LocationException::mark();
    for (auto s : _includeStack) {
      s.mark();
    }
  }

public:
  SyntaxError(const Location& loc, const std::string& msg) : LocationException(loc, msg) {}
  SyntaxError(const Location& loc, std::string currentLine, std::vector<ASTString> includeStack,
              const std::string& msg)
      : LocationException(loc, msg),
        _currentLine(std::move(currentLine)),
        _includeStack(std::move(includeStack)) {}
  ~SyntaxError() throw() override {}
  const char* what() const throw() override { return "syntax error"; }

  void print(std::ostream& os) const override;
  void json(std::ostream& os) const override;
};

class IncludeError : public LocationException {
public:
  IncludeError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env, loc, msg) {}
  ~IncludeError() throw() override {}
  const char* what() const throw() override { return "include error"; }
};

class TypeError : public LocationException {
public:
  TypeError(EnvI& env, const Location& loc, const std::string& msg)
      : LocationException(env, loc, msg) {}
  ~TypeError() throw() override {}
  const char* what() const throw() override { return "type error"; }
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
  const char* what() const throw() override { return "evaluation error"; }
};

class AssertionError : public EvalError {
public:
  AssertionError(EnvI& env, const Location& loc, const std::string& msg)
      : EvalError(env, loc, msg) {}
  ~AssertionError() throw() override {}
  const char* what() const throw() override { return "assertion failed"; }
};

class ModelInconsistent : public LocationException {
public:
  ModelInconsistent(EnvI& env, const Location& loc, const std::string& msg = "")
      : LocationException(env, loc,
                          "model inconsistency detected" + (msg.empty() ? msg : ":  ") + msg) {}
  ~ModelInconsistent() throw() override {}
  const char* what() const throw() override { return "warning"; }
};

class ResultUndefinedError : public LocationException {
private:
  int _warningIdx;

public:
  ResultUndefinedError(EnvI& env, const Location& loc, const std::string& msg);
  ~ResultUndefinedError() throw() override {}
  const char* what() const throw() override { return "result of evaluation is undefined"; }
  int warningIdx() const { return _warningIdx; }
};

}  // namespace MiniZinc
