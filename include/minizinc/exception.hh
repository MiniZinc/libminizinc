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
  const char* what() const throw() override { return "MiniZinc: error"; }
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
      os << "MiniZinc: multiple " << _errors[0].what() << "s:\n";
    }
    for (const auto& error : _errors) {
      error.print(os);
    }
  }

  void json(std::ostream& os) const override {
    for (const auto& error : _errors) {
      error.json(os);
    }
  }
};

}  // namespace MiniZinc
