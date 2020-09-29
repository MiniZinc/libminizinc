/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flatten.hh>
#include <minizinc/hash.hh>

namespace MiniZinc {

class VarOccurrences {
public:
  typedef std::unordered_set<Item*> Items;
  IdMap<Items> itemMap;
  IdMap<int> idx;

  /// Add \a to the index
  void addIndex(VarDeclI* i, int idx_i);
  /// Add \a to the index
  void addIndex(VarDecl* e, int idx_i);
  /// Find index of \a vd
  int find(VarDecl* vd);
  /// Remove index of \a vd
  void remove(VarDecl* vd);

  /// Add \a i to the dependencies of \a v
  void add(VarDecl* v, Item* i);

  /// Remove \a i from map and return new number of occurrences
  int remove(VarDecl* v, Item* i);

  /// Remove all occurrences from map and return new number of occurrences
  void removeAllOccurrences(VarDecl* v);

  /// Return number of occurrences of \a v
  int occurrences(VarDecl* v);

  /// Return number of constraint usages of \a v and whether \a v is an output variable
  std::pair<int, bool> usages(VarDecl* v);

  /// Unify \a v0 and \a v1 (removing \a v0)
  void unify(EnvI& env, Model* m, Id* id0, Id* id1);

  /// Clear all entries
  void clear();
};

class CollectOccurrencesE : public EVisitor {
public:
  VarOccurrences& vo;
  Item* ci;
  CollectOccurrencesE(VarOccurrences& vo0, Item* ci0) : vo(vo0), ci(ci0) {}
  void vId(const Id& id) {
    if (id.decl() != nullptr) {
      vo.add(id.decl(), ci);
    }
  }
};

class CollectOccurrencesI : public ItemVisitor {
public:
  VarOccurrences& vo;
  CollectOccurrencesI(VarOccurrences& vo0) : vo(vo0) {}
  void vVarDeclI(VarDeclI* v);
  void vConstraintI(ConstraintI* ci);
  void vSolveI(SolveI* si);
};

class CollectDecls : public EVisitor {
public:
  VarOccurrences& vo;
  std::vector<VarDecl*>& vd;
  Item* item;
  CollectDecls(VarOccurrences& vo0, std::vector<VarDecl*>& vd0, Item* item0)
      : vo(vo0), vd(vd0), item(item0) {}

  static bool varIsFree(VarDecl* vd) {
    if (vd->e() == nullptr || vd->ti()->domain() == nullptr || vd->ti()->computedDomain()) {
      return true;
    }  /// TODO: test if id's domain is a superset of the right hand side
    /// this currently only tests for equality, and for Boolean domains
    if (Id* ident = vd->e()->dynamicCast<Id>()) {
      if (Expression::equal(ident->decl()->ti()->domain(), vd->ti()->domain())) {
        return true;
      }
    } else if (vd->e() == vd->ti()->domain()) {
      return true;
    }

    return false;
  }

  void vId(Id& id) {
    if ((id.decl() != nullptr) && vo.remove(id.decl(), item) == 0) {
      if (varIsFree(id.decl())) {
        vd.push_back(id.decl());
      }
    }
  }
};

bool is_output(VarDecl* vd);

/// Simplyfy models in \a env
void optimize(Env& env, bool chain_compression = true);

}  // namespace MiniZinc
