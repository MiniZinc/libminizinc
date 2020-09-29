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

#include <vector>

namespace MiniZinc {

class ASTIntVecO;

/**
 * \brief Handler for ASTIntVecO objects
 */
class ASTIntVec {
protected:
  /// Vector
  ASTIntVecO* _v;

public:
  /// Default constructor
  ASTIntVec() : _v(nullptr) {}
  /// Constructor
  ASTIntVec(ASTIntVecO* v) : _v(v) {}
  /// Constructor
  ASTIntVec(const std::vector<int>& v);
  /// Copy constructor
  ASTIntVec(const ASTIntVec& v);
  /// Assignment operator
  ASTIntVec& operator=(const ASTIntVec& v);

  /// Size of vector
  unsigned int size() const;
  /// Element access
  int& operator[](unsigned int i);
  /// Element access
  int operator[](unsigned int i) const;
  /// Iterator begin
  int* begin();
  /// Iterator end
  int* end();
  /// Mark as alive for garbage collection
  void mark() const;
};

template <class>
class ASTExprVecO;

/**
 * \brief Handler for ASTExprVecO objects
 */
template <class T>
class ASTExprVec {
protected:
  /// Vector
  ASTExprVecO<T*>* _v;

public:
  /// Default constructor
  ASTExprVec() : _v(nullptr) {}
  /// Constructor
  ASTExprVec(ASTExprVecO<T*>* v) : _v(v) {}
  /// Constructor
  ASTExprVec(const std::vector<T*>& v);
  /// Copy constructor
  ASTExprVec(const ASTExprVec& v);
  /// Assignment operator
  // NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
  ASTExprVec& operator=(const ASTExprVec& v);

  /// Size of vector
  unsigned int size() const;
  /// Element access
  T*& operator[](unsigned int i);
  /// Element access
  T* operator[](unsigned int i) const;
  /// Iterator begin
  T** begin();
  /// Iterator end
  T** end();

  /// Return vector object
  ASTExprVecO<T*>* vec() const;
  /// Mark as alive for garbage collection
  void mark() const;
};

/// Garbage collected integer vector
class ASTIntVecO : public ASTChunk {
protected:
  /// Constructor
  ASTIntVecO(const std::vector<int>& v);

public:
  /// Allocate and initialise from \a v
  static ASTIntVecO* a(const std::vector<int>& v);
  /// Return size
  unsigned int size() const { return static_cast<unsigned int>(_size / sizeof(int)); }
  /// Return element at position \a i
  int& operator[](unsigned int i) {
    assert(i < size());
    return reinterpret_cast<int*>(_data)[i];
  }
  /// Return element at position \a i
  int operator[](unsigned int i) const {
    assert(i < size());
    return reinterpret_cast<const int*>(_data)[i];
  }
  /// Iterator begin
  int* begin() { return reinterpret_cast<int*>(_data); }
  /// Iterator end
  int* end() { return begin() + size(); }
  /// Mark as alive for garbage collection
  void mark() const { _gcMark = 1; }
};

/// Garbage collected vector of expressions
template <class T>
class ASTExprVecO : public ASTVec {
protected:
  /// Constructor
  ASTExprVecO(const std::vector<T>& v);

public:
  /// Allocate and initialise from \a v
  static ASTExprVecO* a(const std::vector<T>& v);
  unsigned int size() const { return static_cast<unsigned int>(_size); }
  bool empty() const { return size() == 0; }
  T& operator[](unsigned int i) {
    assert(i < static_cast<int>(size()));
    return reinterpret_cast<T&>(_data[i]);
  }
  T operator[](unsigned int i) const {
    assert(i < static_cast<int>(size()));
    return reinterpret_cast<T>(_data[i]);
  }
  /// Iterator begin
  T* begin() { return reinterpret_cast<T*>(_data); }
  /// Iterator end
  T* end() { return begin() + size(); }
  /// Mark as alive for garbage collection
  void mark() const { _gcMark = 1; }
  /// Check if flag is set
  bool flag() const { return _flag1; }
  /// Set flag
  void flag(bool f) { _flag1 = f; }
};

template <class T>
ASTExprVecO<T>::ASTExprVecO(const std::vector<T>& v) : ASTVec(v.size()) {
  _flag1 = false;
  for (auto i = static_cast<unsigned int>(v.size()); (i--) != 0U;) {
    (*this)[i] = v[i];
  }
}
template <class T>
ASTExprVecO<T>* ASTExprVecO<T>::a(const std::vector<T>& v) {
  auto* ao = static_cast<ASTExprVecO<T>*>(alloc(v.size()));
  new (ao) ASTExprVecO<T>(v);
  return ao;
}

inline ASTIntVec::ASTIntVec(const std::vector<int>& v) : _v(ASTIntVecO::a(v)) {}
inline ASTIntVec::ASTIntVec(const ASTIntVec& v) : _v(v._v) {}
// NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
inline ASTIntVec& ASTIntVec::operator=(const ASTIntVec& v) {
  _v = v._v;
  return *this;
}

inline unsigned int ASTIntVec::size() const { return _v != nullptr ? _v->size() : 0; }
inline int& ASTIntVec::operator[](unsigned int i) { return (*_v)[i]; }
inline int ASTIntVec::operator[](unsigned int i) const { return (*_v)[i]; }
inline int* ASTIntVec::begin() { return _v != nullptr ? _v->begin() : nullptr; }
inline int* ASTIntVec::end() { return _v != nullptr ? _v->end() : nullptr; }
inline void ASTIntVec::mark() const {
  if (_v != nullptr) {
    _v->mark();
  }
}

template <class T>
ASTExprVec<T>::ASTExprVec(const std::vector<T*>& v) : _v(ASTExprVecO<T*>::a(v)) {}
template <class T>
inline ASTExprVec<T>::ASTExprVec(const ASTExprVec<T>& v) : _v(v._v) {}
template <class T>
// NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
inline ASTExprVec<T>& ASTExprVec<T>::operator=(const ASTExprVec<T>& v) {
  _v = v._v;
  return *this;
}
template <class T>
inline unsigned int ASTExprVec<T>::size() const {
  return _v ? _v->size() : 0;
}
template <class T>
inline T*& ASTExprVec<T>::operator[](unsigned int i) {
  return (*_v)[i];
}
template <class T>
inline T* ASTExprVec<T>::operator[](unsigned int i) const {
  return (*_v)[i];
}
template <class T>
inline T** ASTExprVec<T>::begin() {
  return _v ? _v->begin() : nullptr;
}
template <class T>
inline T** ASTExprVec<T>::end() {
  return _v ? _v->end() : nullptr;
}
template <class T>
inline ASTExprVecO<T*>* ASTExprVec<T>::vec() const {
  return _v;
}
template <class T>
inline void ASTExprVec<T>::mark() const {
  if (_v) {
    _v->mark();
  }
}

}  // namespace MiniZinc
