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

class StackDump {
public:
  StackDump(EnvI& env);
  StackDump() : _env(nullptr) {}
  void print(std::ostream& os) const;
  void json(std::ostream& os) const;
  bool empty() const { return _stack.empty(); }

private:
  struct Item {
    Ref<Expression> e;
    bool isCompIter;
    /// The value of a comprehension variable when the dump was taken: the trail restores the
    /// variable while the exception unwinds, before the dump is printed
    Ref<Expression> value;
  };
  EnvI* _env;
  std::vector<Item> _stack;
  /// Print the binding of comprehension variable \a e to \a value
  void printBinding(std::ostream& os, Expression* e, Expression* value) const;
};

}  // namespace MiniZinc
