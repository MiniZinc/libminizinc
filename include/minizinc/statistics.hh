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

      virtual void print(std::ostream& os);

      void time(unsigned long long t);
      void nodes(unsigned long long n);
      void failures(unsigned long long f);
      void objective(double o);

      unsigned long long time();
      unsigned long long nodes();
      unsigned long long failures();
      double objective();

      Statistics& operator+=(Statistics& s);
      
      virtual void cleanup() { _time = _nodes = _failures = 0; }
  };
}

#endif
