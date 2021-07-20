/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/prettyprinter.hh>
#include <minizinc/statistics.hh>

#include <iostream>
#include <unordered_set>

namespace MiniZinc {

StatisticsStream::StatisticsStream(std::ostream& os, bool json)
    : _os(os), _json(json), _ios(nullptr) {
  _ios.copyfmt(os);
  if (_json) {
    _os << "{\"type\": \"statistics\", \"statistics\": {";
  }
}

StatisticsStream::~StatisticsStream() {
  if (_json) {
    _os << "}}\n";
  } else {
    _os << "%%%mzn-stat-end\n";
  }
  _os.copyfmt(_ios);
}

void StatisticsStream::precision(std::streamsize prec, bool fixed) {
  _os.precision(prec);
  if (fixed) {
    _os.setf(std::ios::fixed);
  } else {
    _os.unsetf(std::ios::fixed);
  }
}

void StatisticsStream::add(const std::string& stat, const Expression& value) {
  addInternal(stat, value);
}
void StatisticsStream::add(const std::string& stat, int value) { addInternal(stat, value); }
void StatisticsStream::add(const std::string& stat, unsigned int value) {
  addInternal(stat, value);
}
void StatisticsStream::add(const std::string& stat, long value) { addInternal(stat, value); }
void StatisticsStream::add(const std::string& stat, unsigned long value) {
  addInternal(stat, value);
}
void StatisticsStream::add(const std::string& stat, long long value) { addInternal(stat, value); }
void StatisticsStream::add(const std::string& stat, unsigned long long value) {
  addInternal(stat, value);
}
void StatisticsStream::add(const std::string& stat, double value) { addInternal(stat, value); }
void StatisticsStream::add(const std::string& stat, const std::string& value) {
  addInternal(stat, "\"" + Printer::escapeStringLit(value) + "\"");
}

/// TODO all key words should be standard and defined in 1 place
void Statistics::print(std::ostream& os) {
  // TODO When is this ever called???
  StatisticsStream stats(os);
  stats.add("solveTime", _time);
  stats.add("nodes", _nodes);
  stats.add("failures", _failures);
  stats.add("objective", _objective);
};

void Statistics::time(unsigned long long t) { _time = t; }
void Statistics::nodes(unsigned long long n) { _nodes = n; }
void Statistics::failures(unsigned long long f) { _failures = f; }
void Statistics::objective(double o) { _objective = o; }

unsigned long long Statistics::time() const { return _time; };
unsigned long long Statistics::nodes() const { return _nodes; };
unsigned long long Statistics::failures() const { return _failures; };
double Statistics::objective() const { return _objective; };

Statistics& Statistics::operator+=(Statistics& s) {
  _time += s.time();
  _nodes += s.nodes();
  _failures += s.failures();
  _objective = s.objective();
  return *this;
}

}  // namespace MiniZinc
