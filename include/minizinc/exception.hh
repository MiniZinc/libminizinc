/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace MiniZinc {

class SignalRaised : public std::exception {
protected:
  int _signal;

public:
  SignalRaised(int signal) : _signal(signal) {}
  ~SignalRaised() throw() override {}
  const char* what() const throw() override { return "signal raised"; }
  int signal() const { return _signal; }
  /// Re-raise this signal
  void raise() const;
};

class Exception : public std::exception {
protected:
  std::string _msg;

public:
  Exception(std::string msg) : _msg(std::move(msg)) {}
  ~Exception() throw() override {}
  const char* what() const throw() override = 0;
  const std::string& msg() const { return _msg; }
  /// Print human-readable error message
  virtual void print(std::ostream& os) const;
  /// Print JSON stream formatted error message
  virtual void json(std::ostream& os) const;
};

class ParseException : public Exception {
public:
  ParseException(const std::string& msg) : Exception(msg) {}
  ~ParseException() throw() override {}
  const char* what() const throw() override { return ""; }
};

class InternalError : public Exception {
public:
  InternalError(const std::string& msg) : Exception(msg) {}
  ~InternalError() throw() override {}
  const char* what() const throw() override { return "internal error"; }
  void print(std::ostream& os) const override;
};

class Error : public Exception {
public:
  Error(const std::string& msg) : Exception(msg) {}
  ~Error() throw() override {}
  const char* what() const throw() override { return "error"; }
};

class Timeout : public Exception {
public:
  Timeout() : Exception("time limit reached") {}
  ~Timeout() throw() override {}
  const char* what() const throw() override { return "time out"; }
};

class ArithmeticError : public Exception {
public:
  ArithmeticError(const std::string& msg) : Exception(msg) {}
  ~ArithmeticError() throw() override {}
  const char* what() const throw() override { return "arithmetic error"; }
};

/// Allows throwing multiple errors at once
/// e.g. for SyntaxError and TypeError.
template <class T>
class MultipleErrors : public Exception {
protected:
  std::vector<T> _errors;

public:
  MultipleErrors(std::vector<T> errors) : Exception(""), _errors(std::move(errors)) {}
  ~MultipleErrors() throw() override {}
  const char* what() const throw() override { return "multiple errors"; }

  void print(std::ostream& os) const override {
    if (_errors.size() > 1) {
      os << "Multiple " << _errors[0].what() << "s:\n";
    }
    bool first = true;
    for (const auto& error : _errors) {
      if (first) {
        first = false;
      } else {
        os << "\n";
      }
      error.print(os);
    }
  }

  void json(std::ostream& os) const override {
    for (const auto& error : _errors) {
      error.json(os);
    }
  }
};

class PluginError : public Exception {
public:
  /// Construct with message \a msg
  PluginError(const std::string& msg) : Exception(msg) {}
  /// Destructor
  ~PluginError() throw() override {}
  /// Return description
  const char* what() const throw() override { return "plugin loading error"; }
};

}  // namespace MiniZinc
