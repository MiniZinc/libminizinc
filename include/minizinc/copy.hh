/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/model.hh>

namespace MiniZinc {

class CopyMap {
protected:
  typedef std::unordered_map<Model*, Model*> ModelMap;
  ModelMap _modelMap;

  ASTNodeWeakMap _nodeMap;

public:
  void insert(Expression* e0, Expression* e1);
  Expression* find(Expression* e);
  void insert(Item* e0, Item* e1);
  Item* find(Item* e);
  void insert(Model* e0, Model* e1);
  Model* find(Model* e);
  void insert(IntSetVal* e0, IntSetVal* e1);
  IntSetVal* find(IntSetVal* e);
  void insert(FloatSetVal* e0, FloatSetVal* e1);
  FloatSetVal* find(FloatSetVal* e);
  template <class T>
  void insert(ASTExprVec<T> e0, ASTExprVec<T> e1) {
    _nodeMap.insert(e0.vec(), e1.vec());
  }
  template <class T>
  ASTExprVecO<T*>* find(ASTExprVec<T> e) {
    ASTNode* n = _nodeMap.find(e.vec());
    return static_cast<ASTExprVecO<T*>*>(n);
  }
  void clear() {
    _modelMap.clear();
    _nodeMap.clear();
  }
};

/// Create a deep copy of expression \a e
Expression* copy(EnvI& env, Expression* e, bool followIds = false, bool copyFundecls = false,
                 bool isFlatModel = false);
/// Create a deep copy of item \a i
Item* copy(EnvI& env, Item* i, bool followIds = false, bool copyFundecls = false,
           bool isFlatModel = false);
/// Create a deep copy of model \a m
Model* copy(EnvI& env, Model* m);

/// Create a deep copy of expression \a e
Expression* copy(EnvI& env, CopyMap& map, Expression* e, bool followIds = false,
                 bool copyFundecls = false, bool isFlatModel = false);
/// Create a deep copy of item \a i
Item* copy(EnvI& env, CopyMap& map, Item* i, bool followIds = false, bool copyFundecls = false,
           bool isFlatModel = false);
/// Create a deep copy of model \a m
Model* copy(EnvI& env, CopyMap& cm, Model* m, bool isFlatModel = false);

}  // namespace MiniZinc
