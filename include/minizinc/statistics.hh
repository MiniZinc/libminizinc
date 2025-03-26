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
#include <map>

namespace MiniZinc {

/// Stream for writing statistics to.
class StatisticsStream {
private:
  std::ostream& _os;
  bool _json;
  bool _first = true;
  std::ios _ios;
  std::string _jsonType;
  std::string _prefix;
  std::string _endMarker;

  template <typename T>
  void serializeValue(std::ostream& os, const T& value) {
    if constexpr (std::is_arithmetic<T>::value) {
      os << value;
    } else {
      os << "\"" << value << "\"";
    }
  }

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
      _os << _prefix << stat << "=" << value << "\n";
    }
  }

  template <class T>
  void addArrayInternal(const std::string& stat, const std::vector<T>& value) {
    if (_json) {
      if (_first) {
        _first = false;
      } else {
        _os << ", ";
      }
      _os << "\"" << Printer::escapeStringLit(stat) << "\": [";
      for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) {
          _os << ", ";
        }
        serializeValue(_os, value[i]);
      }
      _os << "]";
    } else {
      _os << _prefix << stat << "=[";
      for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) {
          _os << ", ";
        }
        _os << value[i];
      }
      _os << "]\n";
    }
  }

  template <class K, class V>
  void addMapInternal(const std::string& stat, const std::map<K, V>& value) {
    if (_json) {
      if (_first) {
        _first = false;
      } else {
        _os << ", ";
      }
      _os << "\"" << Printer::escapeStringLit(stat) << "\": {";
      bool firstElem = true;
      for (const auto& pair : value) {
        if (!firstElem) {
          _os << ", ";
        }
        firstElem = false;
        _os << "\"" << pair.first << "\": ";
        serializeValue(_os, pair.second);
      }
      _os << "}";
    } else {
      _os << _prefix << stat << "={";
      bool firstElem = true;
      for (const auto& pair : value) {
        if (!firstElem) {
          _os << ", ";
        }
        firstElem = false;
        _os << pair.first << ": " << pair.second;
      }
      _os << "}\n";
    }
  }

public:
  StatisticsStream(std::ostream& os, bool json = false,
                   std::string jsonType = "statistics", 
                   std::string linePrefix = "%%%mzn-stat: ",
                   std::string outputEndMarker = "%%%mzn-stat-end");
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
  void addRaw(const std::string& stat, const std::string& value);
  
  template <class T>
  void addArray(const std::string& stat, const std::vector<T>& value) {
    addArrayInternal(stat, value);
  }

  template <class K, class V>
  void addMap(const std::string& stat, const std::map<K, V>& value) {
    addMapInternal(stat, value);
  }
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
