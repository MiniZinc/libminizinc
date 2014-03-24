/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_HASH_HH__
#define __MINIZINC_HASH_HH__

#include <minizinc/ast.hh>

#include <unordered_map>

namespace MiniZinc {
  
  /// Hash class for expressions
  struct ExpressionHash {
    size_t operator() (const Expression* e) const {
      return Expression::hash(e);
    }
  };
  
  /// Equality test for expressions
  struct ExpressionEq {
    bool operator() (const Expression* e0, const Expression* e1) const {
      return Expression::equal(e0,e1);
    }
  };
  
  /// Hash map from expression to \a T
  template<class T>
  class ExpressionMap {
  protected:
    /// The underlying map implementation
    std::unordered_map<Expression*,T,ExpressionHash,ExpressionEq> _m;
  public:
    /// Iterator type
    typedef typename std::unordered_map<Expression*,T,
      ExpressionHash,ExpressionEq>::iterator iterator;
    /// Insert mapping from \a e to \a t
    void insert(Expression* e, const T& t) {
      assert(e != NULL);
      _m.insert(std::pair<Expression*,T>(e,t));
    }
    /// Find \a e in map
    iterator find(Expression* e) { return _m.find(e); }
    /// Begin of iterator
    iterator begin(void) { return _m.begin(); }
    /// End of iterator
    iterator end(void) { return _m.end(); }
    /// Remove binding of \a e from map
    void remove(Expression* e) {
      _m.erase(e);
    }
    /// Remove all elements from the map
    void clear(void) {
      _m.clear();
    }
  };


  /// Hash class for KeepAlive objects
  struct KAHash {
    size_t operator() (const KeepAlive& e) const {
      return Expression::hash(e());
    }
  };
  
  /// Equality test for KeepAlive objects
  struct KAEq {
    bool operator() (const KeepAlive& e0, const KeepAlive& e1) const {
      return Expression::equal(e0(),e1());
    }
  };


  /// Hash map from KeepAlive to \a T
  template<class T>
  class KeepAliveMap {
  protected:
    /// The underlying map implementation
    std::unordered_map<KeepAlive,T,KAHash,KAEq> _m;
  public:
    /// Iterator type
    typedef typename std::unordered_map<KeepAlive,T,
      KAHash,KAEq>::iterator iterator;
    /// Insert mapping from \a e to \a t
    void insert(KeepAlive& e, const T& t) {
      assert(e() != NULL);
      _m.insert(std::pair<KeepAlive,T>(e,t));
    }
    /// Find \a e in map
    iterator find(KeepAlive& e) { return _m.find(e); }
    /// Begin of iterator
    iterator begin(void) { return _m.begin(); }
    /// End of iterator
    iterator end(void) { return _m.end(); }
    /// Remove binding of \a e from map
    void remove(KeepAlive& e) {
      _m.erase(e);
    }
    template <class D> void dump(void) {
      for (auto i: _m) {
        std::cerr << i.first() << ": " << D::d(i.second) << std::endl;
      }
    }
  };
  
}

#endif
