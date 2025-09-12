/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/exception.hh>

#include <unordered_map>
#include <unordered_set>

namespace MiniZinc {

/// Hash class for expressions
struct ExpressionHash {
  size_t operator()(const Expression* e) const { return Expression::hash(e); }
};

/// Equality test for expressions
struct ExpressionEq {
  bool operator()(const Expression* e0, const Expression* e1) const {
    return Expression::equal(e0, e1);
  }
};

/// Hash map from expression to \a T
template <class T>
class ExpressionMap {
protected:
  /// The underlying map implementation
  std::unordered_map<Expression*, T, ExpressionHash, ExpressionEq> _m;

public:
  /// Iterator type
  typedef
      typename std::unordered_map<Expression*, T, ExpressionHash, ExpressionEq>::iterator iterator;
  /// Insert mapping from \a e to \a t
  iterator insert(Expression* e, const T& t) {
    assert(e != nullptr);
    return _m.insert(std::pair<Expression*, T>(e, t)).first;
  }
  /// Find \a e in map
  iterator find(Expression* e) { return _m.find(e); }
  /// Begin of iterator
  iterator begin() { return _m.begin(); }
  /// End of iterator
  iterator end() { return _m.end(); }
  /// Remove binding of \a e from map
  void remove(Expression* e) { _m.erase(e); }
  /// Remove all elements from the map
  void clear() { _m.clear(); }
};

/// Equality test for identifiers
struct IdEq {
  bool operator()(const Id* e0, const Id* e1) const {
    if (e0->idn() == e1->idn()) {
      if (e0->idn() == -1) {
        return e0->v() == e1->v();
      }
      return true;
    }
    return false;
  }
};

/// Hash map from identifier to \a T
template <class T>
class IdMap {
protected:
  /// The underlying map implementation
  std::unordered_map<Id*, T, ExpressionHash, IdEq> _m;

public:
  /// Iterator type
  typedef typename std::unordered_map<Id*, T, ExpressionHash, IdEq>::iterator iterator;
  /// Insert mapping from \a e to \a t
  void insert(Id* e, const T& t) {
    assert(e != nullptr);
    _m.insert(std::pair<Id*, T>(e, t));
  }
  /// Find \a e in map
  iterator find(Id* e) { return _m.find(e); }
  /// Begin of iterator
  iterator begin() { return _m.begin(); }
  /// End of iterator
  iterator end() { return _m.end(); }
  /// Remove binding of \a e from map
  void remove(Id* e) { _m.erase(e); }
  /// Return number of elements in the map
  int size() const { return _m.size(); }
  /// Return whether map is empty
  bool empty() const { return _m.empty(); }
  /// Remove all elements from the map
  void clear() { _m.clear(); }
  T& get(Id* ident) {
    auto it = find(ident);
    //       assert(it != _m.end());
    if (_m.end() == it) {  // Changing so it stays in Release version
      std::string msg = "Id ";
      //         if (ident)                 // could be a segfault...
      //           msg += ident->v().c_str();
      msg += " not found";
      throw InternalError(msg);
    }
    return it->second;
  }
};

template <class T>
class DenseIdMap {
protected:
  typedef typename std::unordered_map<Id*, T, ExpressionHash, IdEq> string_map;
  /// The underlying map implementation for string-based Ids
  string_map _sm;
  /// The underlying vector for integer-based Ids
  std::vector<T> _im;
  /// Whether an element is present in the vector-based representation
  std::vector<bool> _ip;

public:
  class DenseIdMapIterator : public std::iterator<std::forward_iterator_tag, T> {
  private:
    typename string_map::iterator _smi;
    typename string_map::iterator _smiEnd;
    typename std::vector<T>::iterator _imi;
    typename std::vector<T>::iterator _imiEnd;
    typename std::vector<bool>::iterator _ipi;

  public:
    DenseIdMapIterator(typename string_map::iterator smi, typename string_map::iterator smiEnd,
                       typename std::vector<T>::iterator imi,
                       typename std::vector<T>::iterator imiEnd, std::vector<bool>::iterator ipi)
        : _smi(smi), _smiEnd(smiEnd), _imi(imi), _imiEnd(imiEnd), _ipi(ipi) {
      if (_smi == _smiEnd) {
        while (_imi != _imiEnd && !*_ipi) {
          ++_ipi;
          ++_imi;
        }
      }
    }

    typedef T* pointer;
    typedef T& reference;

    reference operator*() {
      if (_smi != _smiEnd) {
        return _smi->second;
      }
      return *_imi;
    }
    pointer operator->() {
      if (_smi != _smiEnd) {
        return &_smi->second;
      }
      return &*_imi;
    }
    DenseIdMapIterator& operator++() {
      if (_smi != _smiEnd) {
        ++_smi;
      } else {
        while (_imi != _imiEnd && !*_ipi) {
          ++_ipi;
          ++_imi;
        }
        if (_imi != _imiEnd) {
          ++_ipi;
          ++_imi;
        }
      }
      return *this;
    }
    DenseIdMapIterator operator++(int) {
      if (_smi != _smiEnd) {
        return DenseIdMapIterator(_smi++, _smiEnd, _imi, _imiEnd, _ipi);
      }
      auto ret = DenseIdMapIterator(_smi, _smiEnd, _imi, _imiEnd, _ipi);
      while (_imi != _imiEnd && !*_ipi) {
        ++_ipi;
        ++_imi;
      }
      if (_imi != _imiEnd) {
        ++_ipi;
        ++_imi;
      }
      return ret;
    }
    bool operator==(const DenseIdMapIterator& it) { return _smi == it._smi && _imi == it._imi; }
    bool operator!=(const DenseIdMapIterator& it) { return !(*this == it); }
  };

  void insert(Id* e, const T& t) {
    assert(e != nullptr);
    if (e->idn() == -1) {
      _sm.insert(std::pair<Id*, T>(e, t));
    } else {
      if (static_cast<int>(_im.size()) < e->idn() + 1) {
        auto newSize = static_cast<unsigned int>(static_cast<double>(e->idn() + 1) * 1.5);
        _im.resize(newSize);
        _ip.resize(newSize, false);
      }
      _im[e->idn()] = t;
      _ip[e->idn()] = true;
    }
  }
  /// Remove binding of \a e from map
  void remove(Id* e) {
    if (e->idn() == -1) {
      _sm.erase(e);
    } else {
      if (e->idn() < static_cast<int>(_ip.size())) {
        _im[e->idn()] = T();
        _ip[e->idn()] = false;
      }
    }
  }

  /// Remove all elements from the map
  void clear() {
    _sm.clear();
    _im.clear();
    _ip.clear();
  }
  T& get(Id* ident) {
    if (ident->idn() == -1) {
      auto it = _sm.find(ident);
      if (it == _sm.end()) {
        std::string msg = "Id not found";
        throw InternalError(msg);
      }
      return it->second;
    }
    assert(ident->idn() < _ip.size() && _ip[ident->idn()]);
    return _im[ident->idn()];
  }
  std::pair<bool, T*> find(Id* ident) {
    if (ident->idn() == -1) {
      auto it = _sm.find(ident);
      if (it == _sm.end()) {
        return {false, nullptr};
      }
      return {true, &it->second};
    }
    if (ident->idn() >= static_cast<long long int>(_ip.size()) || !_ip[ident->idn()]) {
      return {false, nullptr};
    }
    return {true, &_im[ident->idn()]};
  }

  DenseIdMapIterator begin() {
    return DenseIdMapIterator(_sm.begin(), _sm.end(), _im.begin(), _im.end(), _ip.begin());
  }
  DenseIdMapIterator end() {
    return DenseIdMapIterator(_sm.end(), _sm.end(), _im.end(), _im.end(), _ip.end());
  }
};

/// Hash class for KeepAlive objects
struct KAHash {
  size_t operator()(const Expression* e) const { return Expression::hash(e); }
};

/// Equality test for KeepAlive objects
struct KAEq {
  bool operator()(const Expression* e0, const Expression* e1) const {
    return Expression::equal(e0, e1);
  }
};

/// Hash map from KeepAlive to \a T
template <class T>
class KeepAliveMap : public GCMarker {
protected:
  /// The underlying map implementation
  std::unordered_map<Expression*, T, KAHash, KAEq> _m;

public:
  /// Iterator type
  typedef typename std::unordered_map<Expression*, T, KAHash, KAEq>::iterator iterator;
  /// Insert mapping from \a e to \a t
  void insert(Expression* e, const T& t) {
    assert(e != nullptr);
    _m.insert(std::pair<Expression*, T>(e, t));
  }
  /// Find \a e in map
  iterator find(Expression* e) { return _m.find(e); }
  /// Begin of iterator
  iterator begin() { return _m.begin(); }
  /// End of iterator
  iterator end() { return _m.end(); }
  /// Remove binding of \a e from map
  void remove(Expression* e) { _m.erase(e); }
  void clear() { _m.clear(); }
  template <class D>
  void dump() {
    for (auto i = _m.begin(); i != _m.end(); ++i) {
      std::cerr << D::k(i->first) << ": " << D::d(i->second) << std::endl;
    }
  }
  void mark() override {
    for (auto& it : _m) {
      Expression::mark(it.first);
    }
  }
};

class ExpressionSetIter
    : public std::unordered_set<Expression*, ExpressionHash, ExpressionEq>::iterator {
protected:
  bool _empty;
  typedef std::unordered_set<Expression*, ExpressionHash, ExpressionEq>::iterator Iter;

public:
  ExpressionSetIter() : _empty(false) {}
  ExpressionSetIter(bool /*b*/) : _empty(true) {}
  ExpressionSetIter(const Iter& i) : Iter(i), _empty(false) {}
  bool operator==(const ExpressionSetIter& i) const {
    return (_empty && i._empty) || static_cast<const Iter&>(*this) == static_cast<const Iter&>(i);
  }
  bool operator!=(const ExpressionSetIter& i) const { return !operator==(i); }
};

/// Hash set for expressions
class ExpressionSet {
protected:
  /// The underlying set implementation
  std::unordered_set<Expression*, ExpressionHash, ExpressionEq> _s;

public:
  /// Insert \a e
  void insert(Expression* e) {
    assert(e != nullptr);
    _s.insert(e);
  }
  /// Find \a e in map
  ExpressionSetIter find(Expression* e) { return _s.find(e); }
  /// Begin of iterator
  ExpressionSetIter begin() { return _s.begin(); }
  /// End of iterator
  ExpressionSetIter end() { return _s.end(); }
  /// Remove binding of \a e from map
  void remove(Expression* e) { _s.erase(e); }
  bool contains(Expression* e) { return find(e) != end(); }
  /// Remove all elements from the map
  void clear() { _s.clear(); }
  bool isEmpty() const { return _s.begin() == _s.end(); }
  unsigned int size() const { return static_cast<unsigned int>(_s.size()); }
};

}  // namespace MiniZinc
