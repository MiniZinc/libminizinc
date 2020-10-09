/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

namespace MiniZinc {

class SolverInstanceBase;

class SolverInstance {
protected:
  SolverInstanceBase* _si;

public:
  enum Status {
    OPT,  // For SAT problems this means "search complete"
    SAT,
    UNSAT,
    UNBND,
    UNSATorUNBND,
    UNKNOWN,
    ERROR,
    NONE
  };
  enum StatusReason { SR_OK = -5, SR_TIME, SR_MEMORY, SR_LIMIT, SR_ERROR };
};

const SolverInstance::Status SolverInstance__ERROR = SolverInstance::ERROR;  // just in case...

}  // namespace MiniZinc
