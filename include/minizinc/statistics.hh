/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <iterator>

namespace MiniZinc {

/// Stream for writing statistics to.
class StatisticsStream {
private:
  std::ostream& _os;
  bool _json;
  bool _first = true;
  std::ios _ios;

  template <class T>
  void addInternal(const std::string& stat, const T& value) {
    if (_json) {
      if (_first) {
        _first = false;
      } else {
        _os << ", ";
      }
      _os << "\"" << Printer::escapeStringLit(stat) << "\": " << value;
    } else {
      _os << "%%%mzn-stat: " << stat << "=" << value << "\n";
    }
  }

public:
  StatisticsStream(std::ostream& os, bool json = false);
  ~StatisticsStream();

  void precision(std::streamsize prec, bool fixed = false);
  void add(const std::string& stat, const Expression& value);
  void add(const std::string& stat, int value);
  void add(const std::string& stat, unsigned int value);
  void add(const std::string& stat, long value);
  void add(const std::string& stat, unsigned long value);
  void add(const std::string& stat, long long value);
  void add(const std::string& stat, unsigned long long value);
  void add(const std::string& stat, double value);
  void add(const std::string& stat, const std::string& value);
};

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

  unsigned long long time() const;
  unsigned long long nodes() const;
  unsigned long long failures() const;
  double objective() const;

  Statistics& operator+=(Statistics& s);

  virtual void cleanup() { _time = _nodes = _failures = 0; }
};
}  // namespace MiniZinc
