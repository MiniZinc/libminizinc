/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>

#include <exception>

namespace MiniZinc {
class Warning {
protected:
  Location _loc;
  std::string _msg;
  std::string _stack;

  void createStack(EnvI& env);

public:
  /// Create a warning that does not have a stack dump
  Warning(std::string msg) : _msg(std::move(msg)) {}
  /// Create a warning with a stack dump
  Warning(EnvI& env, std::string msg) : _msg(std::move(msg)) { createStack(env); }
  /// Create a warning with a stack dump and location
  Warning(EnvI& env, const Location& loc, std::string msg) : _loc(loc), _msg(std::move(msg)) {
    createStack(env);
  }

  /// Print human-readable warning/error message
  void print(std::ostream& os, bool werror) const;
  /// Print JSON stream formatted warning/error message
  void json(std::ostream& os, bool werror) const;
};

}  // namespace MiniZinc
