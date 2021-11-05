/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <iostream>
#include <vector>

namespace MiniZinc {
class EnvI;
class KeepAlive;

class StackDump {
public:
  StackDump(EnvI& env);
  void print(std::ostream& os) const;
  void json(std::ostream& os) const;

private:
  std::vector<std::pair<KeepAlive, bool>> _stack;
};

}  // namespace MiniZinc
