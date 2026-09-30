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

  std::unordered_map<ASTNode*, ASTNode*> _nodeMap;
  std::unordered_map<ASTNode*, ASTNode*> _revNodeMap;
  /// Owns the nodes on both sides of the maps
  std::vector<Ref<ASTNode>> _owned;

  void insertNode(ASTNode* n0, ASTNode* n1) {
    bool fwd = _nodeMap.emplace(n0, n1).second;
    bool rev = _revNodeMap.emplace(n1, n0).second;
    if (fwd || rev) {
      _owned.emplace_back(n0);
      _owned.emplace_back(n1);
    }
  }
  static ASTNode* findNode(std::unordered_map<ASTNode*, ASTNode*>& m, ASTNode* n) {
    auto it = m.find(n);
    return it == m.end() ? nullptr : it->second;
  }

public:
  void insert(Expression* e0, Expression* e1);
  Expression* find(Expression* e);
  Expression* findOrig(Expression* e);
  void insert(Item* e0, Item* e1);
  Item* find(Item* e);
  Item* findOrig(Item* e);
  void insert(Model* e0, Model* e1);
  Model* find(Model* e);
  void insert(IntSetVal* e0, IntSetVal* e1);
  IntSetVal* find(IntSetVal* e);
  IntSetVal* findOrig(IntSetVal* e);
  void insert(FloatSetVal* e0, FloatSetVal* e1);
  FloatSetVal* find(FloatSetVal* e);
  FloatSetVal* findOrig(FloatSetVal* e);
  template <class T>
  void insert(ASTExprVec<T> e0, ASTExprVec<T> e1) {
    assert(e0.empty() == e1.empty());
    if (!e0.empty()) {
      insertNode(e0.vec(), e1.vec());
    }
  }
  template <class T>
  ASTExprVecO<T*>* find(ASTExprVec<T> e) {
    if (e.empty()) {
      return nullptr;
    }
    ASTNode* n = findNode(_nodeMap, e.vec());
    return static_cast<ASTExprVecO<T*>*>(n);
  }
  template <class T>
  ASTExprVecO<T*>* findOrig(ASTExprVec<T> e) {
    if (e.empty()) {
      return nullptr;
    }
    ASTNode* n = findNode(_revNodeMap, e.vec());
    return static_cast<ASTExprVecO<T*>*>(n);
  }
  void clear() {
    _modelMap.clear();
    _nodeMap.clear();
    _revNodeMap.clear();
    _owned.clear();
  }
};

/// Create a deep copy of expression \a e
Ref<Expression> copy(EnvI& env, Expression* e, bool followIds = false, bool copyFundecls = false,
                     bool isFlatModel = false);
/// Create a deep copy of item \a i
Ref<Item> copy(EnvI& env, Item* i, bool followIds = false, bool copyFundecls = false,
               bool isFlatModel = false);
/// Create a deep copy of model \a m
Model* copy(EnvI& env, Model* m);

/// Create a deep copy of expression \a e
Ref<Expression> copy(EnvI& env, CopyMap& map, Expression* e, bool followIds = false,
                     bool copyFundecls = false, bool isFlatModel = false);
/// Create a deep copy of item \a i
Ref<Item> copy(EnvI& env, CopyMap& map, Item* i, bool followIds = false, bool copyFundecls = false,
               bool isFlatModel = false);
/// Create a deep copy of model \a m
Model* copy(EnvI& env, CopyMap& cm, Model* m, bool isFlatModel = false);

}  // namespace MiniZinc
