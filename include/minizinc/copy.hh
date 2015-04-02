/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_COPY_HH__
#define __MINIZINC_COPY_HH__

#include <minizinc/model.hh>

namespace MiniZinc {

  class CopyMap {
  protected:
    typedef UNORDERED_NAMESPACE::unordered_map<void*,void*> MyMap;
    MyMap m;
  public:
    void insert(Expression* e0, Expression* e1);
    Expression* find(Expression* e);
    void insert(Item* e0, Item* e1);
    Item* find(Item* e);
    void insert(Model* e0, Model* e1);
    Model* find(Model* e);
    void insert(const ASTString& e0, const ASTString& e1);
    ASTStringO* find(const ASTString& e);
    void insert(IntSetVal* e0, IntSetVal* e1);
    IntSetVal* find(IntSetVal* e);
    template<class T>
    void insert(ASTExprVec<T> e0, ASTExprVec<T> e1) {
      m.insert(std::pair<void*,void*>(e0.vec(),e1.vec()));
    }
    template<class T>
    ASTExprVecO<T*>* find(ASTExprVec<T> e) {
      MyMap::iterator it = m.find(e.vec());
      if (it==m.end()) return NULL;
      return static_cast<ASTExprVecO<T*>*>(it->second);
    }
  };

  /// Create a deep copy of expression \a e
  Expression* copy(EnvI& env, Expression* e, bool followIds=false, bool copyFundecls=false);
  /// Create a deep copy of item \a i
  Item* copy(EnvI& env, Item* i, bool followIds=false, bool copyFundecls=false);
  /// Create a deep copy of model \a m
  Model* copy(EnvI& env, Model* m);

  /// Create a deep copy of expression \a e
  Expression* copy(EnvI& env, CopyMap& map, Expression* e, bool followIds=false, bool copyFundecls=false);
  /// Create a deep copy of item \a i
  Item* copy(EnvI& env, CopyMap& map, Item* i, bool followIds=false, bool copyFundecls=false);
  /// Create a deep copy of model \a m
  Model* copy(EnvI& env, CopyMap& map, Model* m);

}

#endif
