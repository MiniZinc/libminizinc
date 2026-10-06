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
#include <minizinc/memory.hh>
#include <minizinc/stackdump.hh>

#include <exception>
#include <memory>

namespace MiniZinc {
class Warning {
protected:
  Location _loc;
  std::string _msg;
  std::unique_ptr<StackDump> _stack;

public:
  /// Create a warning that does not have a stack dump
  Warning(std::string msg) : _msg(std::move(msg)) {}
  /// Create a warning with a location
  Warning(Location loc, std::string msg) : _loc(std::move(loc)), _msg(std::move(msg)) {}
  /// Create a warning with a stack dump and location
  Warning(EnvI& env, Location loc, std::string msg)
      : _loc(std::move(loc)), _msg(std::move(msg)), _stack(new StackDump(env)) {}

  /// Access the warning message
  const std::string& msg() const { return _msg; }
  /// Access the location the warning was raised at
  const Location& loc() const { return _loc; }

  /// Print human-readable warning/error message
  void print(std::ostream& os, bool werror) const;
  /// Print JSON stream formatted warning/error message
  void json(std::ostream& os, bool werror) const;

  virtual ~Warning() {}
};

}  // namespace MiniZinc
