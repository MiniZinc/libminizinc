/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/exception.hh>
#include <minizinc/timer.hh>

#include <cassert>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ratio>
#include <sstream>
#include <string>
#include <vector>

#ifdef MZN_HAS_LLROUND
#include <cmath>
namespace MiniZinc {
inline long long int round_to_longlong(double v) { return ::llround(v); }
}  // namespace MiniZinc
#else
namespace MiniZinc {
inline long long int round_to_longlong(double v) {
  return static_cast<long long int>(v < 0 ? v - 0.5 : v + 0.5);
}
}  // namespace MiniZinc
#endif

namespace MiniZinc {

// #define MZN_PRINTATONCE_
#ifdef MZN_PRINTATONCE_
#define MZN_PRINT_SRCLOC(e1, e2)                                                            \
  std::cerr << '\n'                                                                         \
            << __FILE__ << ": " << __LINE__ << " (" << __func__ << "): not " << e1 << ":  " \
            << std::flush;                                                                  \
  std::cerr << e2 << std::endl
#else
#define MZN_PRINT_SRCLOC(e1, e2)
#endif
#define MZN_ASSERT_HARD(c)      \
  do {                          \
    if (!(c)) {                 \
      MZN_PRINT_SRCLOC(#c, ""); \
      throw InternalError(#c);  \
    }                           \
  } while (0)
#define MZN_ASSERT_HARD_MSG(c, e)                                                 \
  do {                                                                            \
    if (!(c)) {                                                                   \
      MZN_PRINT_SRCLOC(#c, e);                                                    \
      std::ostringstream oss;                                                     \
      oss << "not " << #c << ":  " << e; /* NOLINT(bugprone-macro-parentheses) */ \
      throw MiniZinc::InternalError(oss.str());                                   \
    }                                                                             \
  } while (0)

inline bool beginswith(const std::string& s, const std::string& t) {
  return s.compare(0, t.length(), t) == 0;
}

inline void check_io_status(bool fOk, const std::string& msg, bool fHard = true) {
  if (!fOk) {
#ifdef _MSC_VER
    char errBuf[1024];
    strerror_s(errBuf, sizeof(errBuf), errno);
#else
    char* errBuf = strerror(errno);
#endif
    std::cerr << "\n  " << msg << ":   " << errBuf << "." << std::endl;
    MZN_ASSERT_HARD_MSG(!fHard, msg << ": " << errBuf);
  }
}

template <class T>
inline bool assign_string(T* /*t*/, const std::string& /*s*/) {
  return false;
}
template <>
inline bool assign_string(std::string* pS, const std::string& s) {
  *pS = s;
  return true;
}

/// A simple per-cmdline option parser
class CLOParser {
  int& _i;  // current item
  std::vector<std::string>& _argv;

public:
  CLOParser(int& ii, std::vector<std::string>& av) : _i(ii), _argv(av) {}
  template <class Value = int>
  inline bool get(const char* names,           // space-separated option list
                  Value* pResult = nullptr,    // pointer to value storage
                  bool fValueOptional = false  // if pResult, for non-string values
  ) {
    return getOption(names, pResult, fValueOptional);
  }
  template <class Value = int>
  inline bool getOption(const char* names,           // space-separated option list
                        Value* pResult = nullptr,    // pointer to value storage
                        bool fValueOptional = false  // if pResult, for non-string values
  ) {
    assert(nullptr == strchr(names, ','));
    assert(nullptr == strchr(names, ';'));
    if (_i >= _argv.size()) {
      return false;
    }
    std::string arg(_argv[_i]);
    /// Separate keywords
    std::string keyword;
    std::istringstream iss(names);
    while (iss >> keyword) {
      if (((2 < keyword.size() || nullptr == pResult) && arg != keyword) ||  // exact cmp
          (0 != arg.compare(0, keyword.size(), keyword))) {                  // truncated cmp
        continue;
      }
      /// Process it
      bool combinedArg = false;  // whether arg and value are combined in one string (like -Ggecode)
      if (keyword.size() < arg.size()) {
        if (nullptr == pResult) {
          continue;
        }
        combinedArg = true;
        arg.erase(0, keyword.size());
      } else {
        if (nullptr == pResult) {
          return true;
        }
        _i++;
        if (_i >= _argv.size()) {
          --_i;
          return fValueOptional;
        }
        arg = _argv[_i];
      }
      assert(pResult);
      if (assign_string(pResult, arg)) {
        return true;
      }
      std::istringstream iss(arg);
      Value tmp;
      if (!(iss >> tmp)) {
        if (!combinedArg) {
          --_i;
        }
        return fValueOptional;
      }
      *pResult = tmp;
      return true;
    }
    return false;
  }
};  // class CLOParser

/// This class prints a value if non-0 and adds comma if not 1st time
class HadOne {
  bool _fHadOne = false;

public:
  template <class N>
  std::string operator()(const N& val, const char* descr = nullptr) {
    std::ostringstream oss;
    if (val) {
      if (_fHadOne) {
        oss << ", ";
      }
      _fHadOne = true;
      oss << val;
      if (descr) {
        oss << descr;
      }
    }
    return oss.str();
  }
  void reset() { _fHadOne = false; }
  operator bool() const { return _fHadOne; }
  bool operator!() const { return !_fHadOne; }
};

/// Split a string into words
/// Add the words into the given vector
inline void split(const std::string& str, std::vector<std::string>& words) {
  std::istringstream iss(str);
  std::string buf;
  while (iss) {
    iss >> buf;
    words.push_back(buf);
  }
}

/// Puts the strings' c_str()s into the 2nd argument.
/// The latter is only valid as long as the former isn't changed.
inline void vec_string2vec_pchar(const std::vector<std::string>& vS,
                                 std::vector<const char*>& vPC) {
  vPC.resize(vS.size());
  for (size_t i = 0; i < vS.size(); ++i) {
    vPC[i] = vS[i].c_str();
  }
}

}  // namespace MiniZinc
