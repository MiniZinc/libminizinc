/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/memory.hh>

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

namespace MiniZinc {

struct CStringHash {
public:
  // FIXME: This is not an amazing hash function
  size_t operator()(std::string_view s) const {
    size_t result = 0;
    const size_t prime = 31;
    for (char c : s) {
      result = c + (result * prime);
    }
    return result;
  }
};

/**
 * \brief Interned string
 *
 * Equal strings share one object, so two strings are equal if their pointers are equal. The
 * object is reference counted like every other node (see class RC): it leaves the interning map
 * and is freed when its count reaches 0. Use it through ASTString, which holds the reference.
 */
class ASTStringData : public ASTChunk {
protected:
  /// Interning Hash Map
  using Interner = std::unordered_map<std::string_view, ASTStringData*, CStringHash>;
  static Interner& interner();
  /// Constructor
  ASTStringData(std::string_view s);

public:
  /// The interned string for \a s (not counted: the caller takes a reference, see ASTString)
  static ASTStringData* a(std::string_view s);
  /// Remove the string \a n, which is being destroyed, from the interning map
  static void unintern(ASTNode* n);
  /// Return underlying C-style string
  // NOLINTNEXTLINE(readability-identifier-naming)
  const char* c_str() const { return _data + sizeof(size_t); }
  /// Return size of string
  size_t size() const {
    return static_cast<unsigned int>(_size) - static_cast<unsigned int>(sizeof(size_t)) - 1;
  }
  /// Return a view of the string
  std::string_view view() const { return {c_str(), size()}; }
  /// Access character at position \a i
  char operator[](unsigned int i) {
    assert(i < size());
    return _data[sizeof(size_t) + i];
  }
  /// Return hash value of string
  size_t hash() const { return reinterpret_cast<const size_t*>(_data)[0]; }
};

/**
 * \brief Reference counted handle for an interned string
 *
 * An ASTString owns one reference to its ASTStringData: a copy counts the string, and the
 * destructor releases it. The string is freed when its last handle and its last node are gone.
 * A default constructed handle is the empty string (a null pointer, which all members accept).
 *
 * - Pass a `const ASTString&` to avoid a count update.
 * - A pointer from aststr() is borrowed: it is valid only while a handle or a node holds the
 *   string.
 * - A node holds its strings as ASTString members. The destructor of a node never runs, so
 *   RC::release gives those references back.
 * - The strings in Constants are immortal: they are never freed and never counted.
 */
class ASTString {
protected:
  /// String
  ASTStringData* _s = nullptr;

public:
  /// Default constructor
  ASTString() = default;
  /// Constructor
  explicit ASTString(std::string_view s) : _s(ASTStringData::a(s)) { RC::incPlain(_s); }
  /// Constructor
  explicit ASTString(ASTStringData* s) : _s(s) { RC::incPlain(_s); }
  /// Copy constructor
  ASTString(const ASTString& s) : _s(s._s) { RC::incPlain(_s); }
  /// Move constructor
  ASTString(ASTString&& s) noexcept : _s(s._s) { s._s = nullptr; }
  /// Destructor
  ~ASTString() { RC::decPlain(_s); }
  /// Assignment operator
  ASTString& operator=(ASTString s) noexcept {
    std::swap(_s, s._s);
    return *this;
  }
  /// Size of the string
  size_t size() const;
  /// Whether string is empty
  bool empty() const;
  /// Underlying C string object
  const char* c_str() const;  // NOLINT(readability-identifier-naming)
  /// View of the string (borrowed: valid while this handle or a node holds it)
  operator std::string_view() const;  // NOLINT(google-explicit-constructor)
  /// Underlying string implementation (borrowed: valid while this handle or a node holds it)
  ASTStringData* aststr() const { return _s; }

  /// Return if string \a s0 is equal to \a s1
  friend bool operator==(const ASTString& s0, const ASTString& s1);
  /// Return if string \a s0 is equal to \a s1
  friend bool operator==(const ASTString& s0, std::string_view s1);
  /// Return if string \a s0 is equal to \a s1
  friend bool operator==(std::string_view s0, const ASTString& s1);

  /// Return if string \a s0 is not equal to \a s1
  friend bool operator!=(const ASTString& s0, const ASTString& s1);
  /// Return if string \a s0 is not equal to \a s1
  friend bool operator!=(const ASTString& s0, std::string_view s1);
  /// Return if string \a s0 is not equal to \a s1
  friend bool operator!=(std::string_view s0, const ASTString& s1);

  /// Return if string is less than \a s
  bool operator<(const ASTString& s) const;

  /// Return if string ends with \a s
  bool endsWith(std::string_view s) const;

  /// Return if string begins with \a s
  bool beginsWith(std::string_view s) const;

  /// Returns a view of the substring [pos, pos+count) (borrowed, like the conversion to
  /// std::string_view)
  std::string_view substr(size_t pos = 0, size_t count = std::string_view::npos) const&;
  /// The view would outlive a temporary handle
  std::string_view substr(size_t pos = 0, size_t count = std::string_view::npos) const&& = delete;
  /// Position of the last \a ch at or before \a pos, or std::string_view::npos
  size_t findLastOf(char ch, size_t pos = std::string_view::npos) const noexcept;
  /// Position of the first \a ch at or after \a pos, or std::string_view::npos
  size_t find(char ch, size_t pos = 0) const noexcept;

  /// Return Levenshtein distance to \a s
  int levenshteinDistance(const ASTString& other) const;

  /// Compute hash value of string
  size_t hash() const;
};

/**
 * \brief Print String \a s
 */
template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os,
                                             const ASTString& s) {
  return os << std::string_view(s);
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

inline size_t ASTString::size() const { return _s != nullptr ? _s->size() : 0; }
inline bool ASTString::empty() const { return _s == nullptr; }
// NOLINTNEXTLINE(readability-identifier-naming)
inline const char* ASTString::c_str() const { return _s != nullptr ? _s->c_str() : ""; }
inline ASTString::operator std::string_view() const {
  return _s != nullptr ? _s->view() : std::string_view();
}

inline bool operator==(const ASTString& s0, const ASTString& s1) { return s0._s == s1._s; }
inline bool operator!=(const ASTString& s0, const ASTString& s1) { return s0._s != s1._s; }

inline bool operator==(const ASTString& s0, std::string_view s1) {
  return std::string_view(s0) == s1;
}
inline bool operator==(std::string_view s0, const ASTString& s1) {
  return s0 == std::string_view(s1);
}
inline bool operator!=(const ASTString& s0, std::string_view s1) { return !(s0 == s1); }
inline bool operator!=(std::string_view s0, const ASTString& s1) { return !(s0 == s1); }

inline bool ASTString::operator<(const ASTString& s) const {
  return std::string_view(*this) < std::string_view(s);
}
inline bool ASTString::endsWith(std::string_view s) const {
  std::string_view v(*this);
  return v.size() >= s.size() && v.substr(v.size() - s.size()) == s;
}
inline bool ASTString::beginsWith(std::string_view s) const {
  return std::string_view(*this).substr(0, s.size()) == s;
}

inline std::string_view ASTString::substr(size_t pos, size_t count) const& {
  return std::string_view(*this).substr(pos, count);
}
inline size_t ASTString::findLastOf(char ch, size_t pos) const noexcept {
  return std::string_view(*this).rfind(ch, pos);
}
inline size_t ASTString::find(char ch, size_t pos) const noexcept {
  return std::string_view(*this).find(ch, pos);
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
