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
#include <string>
#include <utility>

namespace MiniZinc {

class Exception : public std::exception {
protected:
  std::string _msg;

public:
  Exception(std::string msg) : _msg(std::move(msg)) {}
  virtual ~Exception() throw() {}
  virtual const char* what() const throw() = 0;
  const std::string& msg() const { return _msg; }
};

class ParseException : public Exception {
public:
  ParseException(const std::string& msg) : Exception(msg) {}
  ~ParseException() throw() {}
  virtual const char* what() const throw() { return ""; }
};

class InternalError : public Exception {
public:
  InternalError(const std::string& msg) : Exception(msg) {}
  ~InternalError() throw() {}
  virtual const char* what() const throw() { return "MiniZinc: internal error"; }
};

class Error : public Exception {
public:
  Error(const std::string& msg) : Exception(msg) {}
  ~Error() throw() {}
  virtual const char* what() const throw() { return ""; }
};

class Timeout : public Exception {
public:
  Timeout() : Exception("time limit reached") {}
  ~Timeout() throw() {}
  virtual const char* what() const throw() { return "MiniZinc: time out"; }
};

class ArithmeticError : public Exception {
public:
  ArithmeticError(const std::string& msg) : Exception(msg) {}
  virtual ~ArithmeticError() throw() {}
  virtual const char* what() const throw() { return "MiniZinc: arithmetic error"; }
};

}  // namespace MiniZinc
