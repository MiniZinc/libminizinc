/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/gc.hh>

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

namespace MiniZinc {

class ASTStringData;

/**
 * \brief Handle for an interned garbage collected string
 */
class ASTString {
protected:
  /// String
  ASTStringData* _s = nullptr;

public:
  /// Default constructor
  ASTString() = default;
  /// Constructor
  ASTString(const std::string& s);
  /// Constructor
  ASTString(ASTStringData* s) : _s(s){};
  /// Copy constructor
  ASTString(const ASTString& s) = default;
  /// Assignment operator
  ASTString& operator=(const ASTString& s) = default;

  /// Size of the string
  size_t size() const;
  /// Underlying C string object
  const char* c_str() const;  // NOLINT(readability-identifier-naming)
  /// Underlying string implementation
  ASTStringData* aststr() const { return _s; }

  /// Return if string is equal to \a s
  bool operator==(const ASTString& s) const;
  /// Return if string is not equal to \a s
  bool operator!=(const ASTString& s) const;
  /// Return if string is less than \a s
  bool operator<(const ASTString& s) const;

  /// Return if string is equal to \a s
  bool operator==(const std::string& s) const;
  /// Return if string is not equal to \a s
  bool operator!=(const std::string& s) const;

  /// Return if string ends with \a s
  bool endsWith(const std::string& s) const;

  /// Return if string begins with \a s
  bool beginsWith(const std::string& s) const;

  /// Returns a substring [pos, pos+count).
  std::string substr(size_t pos = 0, size_t count = std::string::npos) const;
  // Finds the last character equal to one of characters in the given character sequence.
  size_t findLastOf(char ch, size_t pos = std::string::npos) const noexcept;
  // Finds the first character equal to the given character sequence.
  size_t find(char ch, size_t pos = 0) const noexcept;

  /// Return Levenshtein distance to \a s
  int levenshteinDistance(const ASTString& other) const;

  /// Compute hash value of string
  size_t hash() const;

  /// Mark string during garbage collection
  void mark() const;
};

/**
 * \brief Print String \a s
 */
template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os,
                                             const ASTString& s) {
  return s.size() == 0 ? os : (os << s.c_str());
}

}  // namespace MiniZinc

namespace std {
template <>
struct hash<MiniZinc::ASTString> {
public:
  size_t operator()(const MiniZinc::ASTString& s) const;
};

template <>
struct equal_to<MiniZinc::ASTString> {
public:
  bool operator()(const MiniZinc::ASTString& s0, const MiniZinc::ASTString& s1) const;
};
template <>
struct less<MiniZinc::ASTString> {
public:
  bool operator()(const MiniZinc::ASTString& s0, const MiniZinc::ASTString& s1) const;
};
}  // namespace std

namespace MiniZinc {

struct CStringHash {
public:
  // FIXME: This is not an amazing hash function
  size_t operator()(const std::pair<const char*, size_t>& s) const {
    size_t result = 0;
    const size_t prime = 31;
    for (size_t i = 0; i < s.second; ++i) {
      result = s.first[i] + (result * prime);
    }
    return result;
  }
};
struct CStringEquals {
public:
  bool operator()(const std::pair<const char*, size_t>& s0,
                  const std::pair<const char*, size_t>& s1) const {
    return s0.second == s1.second && (strncmp(s0.first, s1.first, s0.second) == 0);
  }
};

/**
 * \brief Garbage collected interned string
 */
class ASTStringData : public ASTChunk {
  friend class GC::Heap;

protected:
  /// Interning Hash Map
  using Interner = std::unordered_map<std::pair<const char*, size_t>, ASTStringData*, CStringHash,
                                      CStringEquals>;
  static Interner& interner();
  /// Constructor
  ASTStringData(const std::string& s);

public:
  /// Allocate and initialise as \a s
  static ASTStringData* a(const std::string& s);
  /// Return underlying C-style string
  // NOLINTNEXTLINE(readability-identifier-naming)
  const char* c_str() const { return _data + sizeof(size_t); }
  /// Return size of string
  size_t size() const {
    return static_cast<unsigned int>(_size) - static_cast<unsigned int>(sizeof(size_t)) - 1;
  }
  /// Access character at position \a i
  char operator[](unsigned int i) {
    assert(i < size());
    return _data[sizeof(size_t) + i];
  }
  /// Return hash value of string
  size_t hash() const { return reinterpret_cast<const size_t*>(_data)[0]; }
  /// Mark for garbage collection
  void mark() const { _gcMark = 1; }

protected:
  /// GC Destructor
  void destroy() const {
    assert(interner().find({this->c_str(), this->size()}) != interner().end());
    interner().erase({this->c_str(), this->size()});
  };
};

inline ASTString::ASTString(const std::string& s) : _s(ASTStringData::a(s)) {}

inline size_t ASTString::size() const { return _s != nullptr ? _s->size() : 0; }
// NOLINTNEXTLINE(readability-identifier-naming)
inline const char* ASTString::c_str() const { return _s != nullptr ? _s->c_str() : nullptr; }
inline void ASTString::mark() const {
  if (_s != nullptr) {
    _s->mark();
  }
}

inline bool ASTString::operator==(const ASTString& s) const { return _s == s._s; }
inline bool ASTString::operator!=(const ASTString& s) const { return _s != s._s; }
inline bool ASTString::operator<(const ASTString& s) const {
  if (size() == 0) {
    return 0 < s.size();
  }
  unsigned int size = std::min(_s->size(), s.size());
  int cmp = strncmp(_s->c_str(), s.c_str(), size);
  if (cmp == 0) {
    return _s->size() < s.size();
  }
  return cmp < 0;
}
inline bool ASTString::operator==(const std::string& s) const {
  return size() == s.size() && (size() == 0 || strncmp(_s->c_str(), s.c_str(), size()) == 0);
}
inline bool ASTString::operator!=(const std::string& s) const { return !(*this == s); }
inline bool ASTString::endsWith(const std::string& s) const {
  return size() >= s.size() &&
         (size() == 0 || strncmp(_s->c_str() + size() - s.size(), s.c_str(), s.size()) == 0);
}
inline bool ASTString::beginsWith(const std::string& s) const {
  return size() >= s.size() && (size() == 0 || strncmp(_s->c_str(), s.c_str(), s.size()) == 0);
}

inline std::string ASTString::substr(size_t pos, size_t count) const {
  if (pos > size()) {
    throw std::out_of_range("ASTString::substr pos out of range");
  }
  if (count == std::string::npos) {
    return std::string(c_str() + pos, size() - pos);
  }
  return std::string(c_str() + pos, std::min(size() - pos, count));
}
inline size_t ASTString::findLastOf(char ch, size_t pos) const noexcept {
  const char* str = c_str();
  for (int i = std::min(size() - 1, pos); i >= 0; --i) {
    if (str[i] == ch) {
      return i;
    }
  }
  return std::string::npos;
}

inline size_t ASTString::find(char ch, size_t pos) const noexcept {
  if (pos >= size()) {
    return std::string::npos;
  }
  const char* str = c_str();
  for (int i = pos; i < size(); ++i) {
    if (str[i] == ch) {
      return i;
    }
  }
  return std::string::npos;
}

inline size_t ASTString::hash() const { return _s != nullptr ? _s->hash() : 0; }

}  // namespace MiniZinc

namespace std {
inline size_t hash<MiniZinc::ASTString>::operator()(const MiniZinc::ASTString& s) const {
  return s.hash();
}
inline bool equal_to<MiniZinc::ASTString>::operator()(const MiniZinc::ASTString& s0,
                                                      const MiniZinc::ASTString& s1) const {
  return s0 == s1;
}
inline bool less<MiniZinc::ASTString>::operator()(const MiniZinc::ASTString& s0,
                                                  const MiniZinc::ASTString& s1) const {
  return s0 < s1;
}
}  // namespace std
