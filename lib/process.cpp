/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/process.hh>

namespace MiniZinc {

#ifdef _WIN32
	std::exception_ptr winExceptions[2];

  void TimeOut(HANDLE hProcess, bool* doneStdout, bool* doneStderr, int timeout, std::timed_mutex* mtx) {
    if (timeout > 0) {
      if (!mtx->try_lock_for(std::chrono::milliseconds(timeout))) {
        if ( (!*doneStdout) || (!*doneStderr) ) {
          *doneStdout = true;
          *doneStderr = true;
          TerminateProcess(hProcess,0);
        }
      }
    }
  }
#endif

}
