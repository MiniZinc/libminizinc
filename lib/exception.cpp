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
  os << _msg << std::endl;
}

void Exception::json(std::ostream& os) const {
  os << "{\"type\": \"error\", \"what\": \"" << Printer::escapeStringLit(std::string(what()))
     << "\", \"message\": \"" << Printer::escapeStringLit(_msg) << "\"}" << std::endl;
}

void InternalError::print(std::ostream& os) const {
  os << "MiniZinc has encountered an internal error. This is a bug." << std::endl
     << "Please file a bug report using the MiniZinc bug tracker." << std::endl
     << "The internal error message was: " << std::endl
     << "\"" << msg() << "\"" << std::endl;
}

void BadOption::print(std::ostream& os) const {
  os << msg() << std::endl;
  if (!_usage.empty()) {
    os << _usage << std::endl;
  }
}

}  // namespace MiniZinc
