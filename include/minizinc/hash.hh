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
  
  struct ExpressionHash {
    size_t operator() (const Expression* e) const {
      return Expression::hash(e);
    }
  };
  
  struct ExpressionEq {
    bool operator() (const Expression* e0, const Expression* e1) const {
      return Expression::equal(e0,e1);
    }
  };
  
  template<class T>
  class ExpressionMap {
  protected:
    std::unordered_map<Expression*,T,ExpressionHash,ExpressionEq> _m;
  public:
    typedef typename std::unordered_map<Expression*,T,
      ExpressionHash,ExpressionEq>::iterator iterator;
    void insert(Expression* e, T& t) {
      _m.insert(std::pair<Expression*,T>(e,t));
    }
    iterator find(Expression* e) { return _m.find(e); }
    iterator begin(void) { return _m.begin(); }
    iterator end(void) { return _m.end(); }
  };
  
}

#endif
