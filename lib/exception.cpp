/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/exception.hh>
#include <minizinc/prettyprinter.hh>

#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <csignal>
#include <unistd.h>
#endif

namespace MiniZinc {
void SignalRaised::raise() const {
#ifdef _WIN32
  GenerateConsoleCtrlEvent(signal(), 0);
#else
  kill(getpid(), signal());
#endif
}

void Exception::print(std::ostream& os) const {
  os << "Error: ";
  if (!std::string(what()).empty()) {
    os << what() << ": ";
  }
  os << msg() << '\n';
}

void Exception::json(std::ostream& os) const {
  os << "{\"type\": \"error\", \"what\": \"" << Printer::escapeStringLit(std::string(what()))
     << "\", \"message\": \"" << Printer::escapeStringLit(msg()) << "\"}\n";
}

void InternalError::print(std::ostream& os) const {
  os << "MiniZinc has encountered an internal error. This is a bug.\n"
     << "Please file a bug report using the MiniZinc bug tracker.\n"
     << "The internal error message was: \n"
     << "\"" << msg() << "\"\n";
}

void BadOption::print(std::ostream& os) const {
  os << msg() << '\n';
  if (!usage().empty()) {
    os << usage() << '\n';
  }
}

}  // namespace MiniZinc
