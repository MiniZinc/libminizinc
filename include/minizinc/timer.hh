/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <ratio>
#include <sstream>

namespace MiniZinc {

class Timer {
protected:
  std::chrono::steady_clock::time_point _last;

public:
  /// Construct timer
  Timer() : _last(std::chrono::steady_clock::now()) {}
  /// Reset timer
  void reset() { _last = std::chrono::steady_clock::now(); }
  /// Return milliseconds since timer was last reset
  long long int ms() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                 _last)
        .count();
  }
  /// Return seconds since timer was last reset
  double s() const {
    return std::chrono::duration_cast<std::chrono::duration<double> >(
               std::chrono::steady_clock::now() - _last)
        .count();
  }
  std::string stoptime() const {
    std::ostringstream oss;
    oss << std::setprecision(2) << std::fixed << s() << " s";
    return oss.str();
  }
};

}  // namespace MiniZinc
