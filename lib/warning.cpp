/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten_internal.hh>
#include <minizinc/warning.hh>

namespace MiniZinc {

void Warning::createStack(EnvI& env) {
  std::stringstream s;
  env.createErrorStack();
  env.dumpStack(s, true);
  _stack = s.str();
}

void Warning::print(std::ostream& os, bool werror) const {
  os << (werror ? "ERROR" : "WARNING") << ": " << _msg << "\n";
  if (!_loc.filename().empty()) {
    os << "  " << _loc << ":\n  ";
  }
  os << _stack << std::endl;
}

void Warning::json(std::ostream& os, bool werror) const {
  os << "{\"type\": \"";
  if (werror) {
    os << "error\", \"what\": \"";
  }
  os << "warning\", ";
  if (!_loc.filename().empty()) {
    os << "\"location\": {\"firstLine\": " << _loc.firstLine()
       << ", \"firstColumn\": " << _loc.firstColumn() << ", \"lastLine\": " << _loc.lastLine()
       << ", \"lastColumn\": " << _loc.lastColumn() << "}, ";
  }
  os << "\"message\": \"" << Printer::escapeStringLit(_msg) << "\"}" << std::endl;
}

}  // namespace MiniZinc
