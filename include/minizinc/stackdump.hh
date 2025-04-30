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

#include <iostream>
#include <vector>

namespace MiniZinc {
class EnvI;
class Expression;

// IMPORTANT: StackDump currently depends on the warning/error keeping the expression in _stack
// alive from their marking member function.
class StackDump {
public:
  StackDump(EnvI& env);
  StackDump() : _env(nullptr) {}
  void print(std::ostream& os) const;
  void json(std::ostream& os) const;
  bool empty() const { return _stack.empty(); }

  inline void mark() {
    for (auto pair : _stack) {
      Expression::mark(pair.first);
    }
  }

private:
  EnvI* _env;
  std::vector<std::pair<Expression*, bool>> _stack;
};

}  // namespace MiniZinc
