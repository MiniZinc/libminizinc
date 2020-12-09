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

LocationException::LocationException(EnvI& env, const Location& loc, const std::string& msg)
    : Exception(msg), _loc(loc) {
  env.createErrorStack();
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
