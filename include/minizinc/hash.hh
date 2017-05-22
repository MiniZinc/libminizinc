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

#include <minizinc/stl_map_set.hh>
#include <minizinc/exception.hh>

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
    UNORDERED_NAMESPACE::unordered_map<Expression*,T,ExpressionHash,ExpressionEq> _m;
  public:
    /// Iterator type
    typedef typename UNORDERED_NAMESPACE::unordered_map<Expression*,T,
      ExpressionHash,ExpressionEq>::iterator iterator;
    /// Insert mapping from \a e to \a t
    iterator insert(Expression* e, const T& t) {
      assert(e != NULL);
      return _m.insert(std::pair<Expression*,T>(e,t)).first;
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
  
  /// Equality test for identifiers
  struct IdEq {
    bool operator() (const Id* e0, const Id* e1) const {
      if (e0->idn() == e1->idn()) {
        if (e0->idn() == -1)
          return e0->v() == e1->v();
        return true;
      }
      return false;
    }
  };

  /// Hash map from identifier to \a T
  template<class T>
  class IdMap {
  protected:
    /// The underlying map implementation
    UNORDERED_NAMESPACE::unordered_map<Id*,T,ExpressionHash,IdEq> _m;
  public:
    /// Iterator type
    typedef typename UNORDERED_NAMESPACE::unordered_map<Id*,T,
    ExpressionHash,IdEq>::iterator iterator;
    /// Insert mapping from \a e to \a t
    void insert(Id* e, const T& t) {
      assert(e != NULL);
      _m.insert(std::pair<Id*,T>(e,t));
    }
    /// Find \a e in map
    iterator find(Id* e) { return _m.find(e); }
    /// Begin of iterator
    iterator begin(void) { return _m.begin(); }
    /// End of iterator
    iterator end(void) { return _m.end(); }
    /// Remove binding of \a e from map
    void remove(Id* e) {
      _m.erase(e);
    }
    /// Remove all elements from the map
    void clear(void) {
      _m.clear();
    }
    T& get(Id* ident) {
      iterator it = find(ident);
//       assert(it != _m.end());
      if (_m.end() == it)   { // Changing so it stays in Release version
        std::string msg = "Id ";
//         if (ident)                 // could be a segfault...
//           msg += ident->v().c_str();
        msg += " not found";
        throw InternalError(msg);
      }
      return it->second;
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
    UNORDERED_NAMESPACE::unordered_map<KeepAlive,T,KAHash,KAEq> _m;
  public:
    /// Iterator type
    typedef typename UNORDERED_NAMESPACE::unordered_map<KeepAlive,T,
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
    void clear() { _m.clear(); }
    template <class D> void dump(void) {
      for (iterator i = _m.begin(); i != _m.end(); ++i) {
        std::cerr << i->first() << ": " << D::d(i->second) << std::endl;
      }
    }
  };
  
  class ExpressionSetIter : public UNORDERED_NAMESPACE::unordered_set<Expression*,ExpressionHash,ExpressionEq>::iterator {
  protected:
    bool _empty;
    typedef UNORDERED_NAMESPACE::unordered_set<Expression*,ExpressionHash,ExpressionEq>::iterator Iter;
  public:
    ExpressionSetIter(void) : _empty(false) {}
    ExpressionSetIter(bool) : _empty(true) {}
    ExpressionSetIter(const Iter& i) : Iter(i), _empty(false) {}
    bool operator ==(const ExpressionSetIter& i) const {
      return (_empty && i._empty) || static_cast<const Iter&>(*this)==static_cast<const Iter&>(i);
    }
    bool operator !=(const ExpressionSetIter& i) const {
      return !operator ==(i);
    }
  };

  /// Hash set for expressions
  class ExpressionSet {
  protected:
    /// The underlying set implementation
    UNORDERED_NAMESPACE::unordered_set<Expression*,ExpressionHash,ExpressionEq> _s;
  public:
    /// Insert \a e
    void insert(Expression* e) {
      assert(e != NULL);
      _s.insert(e);
    }
    /// Find \a e in map
    ExpressionSetIter find(Expression* e) { return _s.find(e); }
    /// Begin of iterator
    ExpressionSetIter begin(void) { return _s.begin(); }
    /// End of iterator
    ExpressionSetIter end(void) { return _s.end(); }
    /// Remove binding of \a e from map
    void remove(Expression* e) {
      _s.erase(e);
    }
    bool contains(Expression* e) { return find(e) != end(); }
    /// Remove all elements from the map
    void clear(void) {
      _s.clear();
    }
    bool isEmpty(void) const { return _s.begin() == _s.end(); }
  };
  
}

#endif
