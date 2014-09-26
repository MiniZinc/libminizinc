/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_STATISTICS_HH__
#define __MINIZINC_STATISTICS_HH__
#include <iterator>

namespace MiniZinc {
 
  class Statistics {
  protected:
    // time in milliseconds
    unsigned long long _time;
    // search nodes
    unsigned long long _nodes;
    // failures/ backtracks
    unsigned long long _failures;
    // current objective value
    double _objective;
    
  public:
      Statistics() : _time(0), _nodes(0), _failures(0) {}
    
      virtual void print(std::ostream& os) {
        os << "Time(ms):\t"  << _time      << std::endl
           << "Nodes:\t"     << _nodes     << std::endl
           << "Failures:\t"  << _failures  << std::endl
           << "Objective:\t" << _objective << std::endl;
      };
      
      virtual void printLine(std::ostream& os) {
        os << _time     << '\t'
           << _nodes    << '\t'
           << _failures << '\t'
           << _objective << std::endl;
      }
      
      void time(unsigned long long t) { _time = t; }
      void nodes(unsigned long long n) { _nodes = n; }
      void failures(unsigned long long f) { _failures = f; }
      void objective(double o) { _objective = o; }
      
      unsigned long long time() { return _time; };
      unsigned long long nodes() { return _nodes; };
      unsigned long long failures() { return _failures; };
      double objective() { return _objective; };
            
      operator+=(Statistics& s) {
	_time += s.time();
	_nodes += s.nodes();
	_failures += s.failures();
	_objective = s.objective();
      }
  };  
}

#endif