/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astexception.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

void SyntaxError::print(std::ostream& os) const {
  for (const auto& filename : _includeStack) {
    os << "(included from file '" << filename << "')\n";
  }
  os << loc() << ":\n";
  if (!_currentLine.empty()) {
    os << _currentLine << "\n";
  }
  os << "Error: " << msg() << std::endl;
}

void SyntaxError::json(std::ostream& os) const {
  os << "{\"type\": \"error\", \"what\": \"" << Printer::escapeStringLit(std::string(what()))
     << "\", \"location\": {\"filename\": \"" << Printer::escapeStringLit(loc().filename())
     << "\", \"firstLine\": " << loc().firstLine() << ", \"firstColumn\": " << loc().firstColumn()
     << ", \"lastLine\": " << loc().lastLine() << ", \"lastColumn\": " << loc().lastColumn()
     << "}, ";
  if (!_includeStack.empty()) {
    os << "\"includedFrom\": [";
    bool first = true;
    for (const auto& filename : _includeStack) {
      if (first) {
        first = false;
      } else {
        os << ", ";
      }
      os << "\"" << filename << "\"";
    }
    os << "], ";
  }
  os << "\"message\": \"" << Printer::escapeStringLit(msg()) << "\"}" << std::endl;
}

void CyclicIncludeError::print(std::ostream& os) const {
  Exception::print(os);
  for (const auto& filename : _cycle) {
    os << "  " << filename << "\n";
  }
}

void CyclicIncludeError::json(std::ostream& os) const {
  os << "{\"type\": \"error\", \"what\": \"" << Printer::escapeStringLit(std::string(what()))
     << "\", \"cycle\": [";
  bool first = true;
  for (const auto& filename : _cycle) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << "\"" << Printer::escapeStringLit(filename) << "\"";
  }
  os << "]}\n";
}

LocationException::LocationException(EnvI& env, const Location& loc, const std::string& msg)
    : Exception(msg), _env(env), _loc(loc) {
  env.createErrorStack();
}

void LocationException::print(std::ostream& os) const {
  Exception::print(os);
  if (_dumpStack) {
    _env.dumpStack(os, true);
  } else {
    os << "  " << loc() << "\n";
  }
}

void LocationException::json(std::ostream& os) const {
  // TODO: Allow dumping of stack?
  os << "{\"type\": \"error\", \"what\": \"" << Printer::escapeStringLit(std::string(what()))
     << "\", \"location\": {\"filename\": \"" << Printer::escapeStringLit(loc().filename())
     << "\", \"firstLine\": " << loc().firstLine() << ", \"firstColumn\": " << loc().firstColumn()
     << ", \"lastLine\": " << loc().lastLine() << ", \"lastColumn\": " << loc().lastColumn()
     << "}, \"message\": \"" << Printer::escapeStringLit(msg()) << "\"}" << std::endl;
}

ResultUndefinedError::ResultUndefinedError(EnvI& env, const Location& loc, const std::string& msg)
    : LocationException(env, loc, msg) {
  if (env.inMaybePartial == 0) {
    std::string warning = "undefined result becomes false in Boolean context";
    if (!msg.empty()) {
      warning += "\n  (" + msg + ")";
    }
    env.addWarning(warning);
  }
}

}  // namespace MiniZinc
