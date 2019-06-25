/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/statistics.hh>
#include <iostream>

namespace MiniZinc {

  /// TODO all key words should be standard and defined in 1 place
  void Statistics::print(std::ostream& os) {
    os << "%%%mzn-stat: solveTime="  << _time      << std::endl
    << "%%%mzn-stat: nodes="     << _nodes     << std::endl
    << "%%%mzn-stat: failures="  << _failures  << std::endl
    << "%%%mzn-stat: objective=" << _objective << std::endl;
  };

  void Statistics::time(unsigned long long t) { _time = t; }
  void Statistics::nodes(unsigned long long n) { _nodes = n; }
  void Statistics::failures(unsigned long long f) { _failures = f; }
  void Statistics::objective(double o) { _objective = o; }
  
  unsigned long long Statistics::time() { return _time; };
  unsigned long long Statistics::nodes() { return _nodes; };
  unsigned long long Statistics::failures() { return _failures; };
  double Statistics::objective() { return _objective; };
  
  Statistics&
  Statistics::operator+=(Statistics& s) {
    _time += s.time();
    _nodes += s.nodes();
    _failures += s.failures();
    _objective = s.objective();
    return *this;
  }

}
