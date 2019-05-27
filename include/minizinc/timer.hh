/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_TIMER_HH__
#define __MINIZINC_TIMER_HH__

#include <ctime>
#include <chrono>
#include <ratio>
#include <iomanip>
#include <sstream>

namespace MiniZinc {
  
  class Timer {
  protected:
    std::chrono::steady_clock::time_point last;
  public:
    /// Construct timer
    Timer(void) : last(std::chrono::steady_clock::now()) {}
    /// Reset timer
    void reset(void) {
      last = std::chrono::steady_clock::now();
    }
    /// Return milliseconds since timer was last reset
    long long int ms(void) const {
      return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-last).count();
    }
    /// Return seconds since timer was last reset
    double s(void) const {
      return std::chrono::duration_cast<std::chrono::duration<double> >(std::chrono::steady_clock::now()-last).count();
    }
    std::string stoptime(void) const {
      std::ostringstream oss;
      oss << std::setprecision(2) << std::fixed << s() << " s";
      return oss.str();
    }

  };
  
}

#endif
