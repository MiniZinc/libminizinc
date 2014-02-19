/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_OPTIMIZE_HH__
#define __MINIZINC_OPTIMIZE_HH__

#include <minizinc/flatten.hh>
#include <minizinc/hash.hh>
#include <unordered_set>

namespace MiniZinc {

  class VarOccurrences {
  public:
    typedef std::unordered_set<Item*> Items;
    ExpressionMap<Items> _m;
    ExpressionMap<int> idx;

    /// Add \a to the index
    void add(VarDeclI* i, int idx_i);
    /// Find index of \a vd
    int find(VarDecl* vd);
    /// Remove index of \a vd
    void remove(VarDecl* vd);
    
    /// Add \a i to the dependencies of \a v
    void add(VarDecl* v, Item* i);
    
    /// Remove \a i from map and return new number of occurrences
    int remove(VarDecl* v, Item* i);
    
    /// Return number of occurrences of \a v
    int occurrences(VarDecl* v);
    
  };
  
  class CollectOccurrencesE : public EVisitor {
  public:
    VarOccurrences& vo;
    Item* ci;
    CollectOccurrencesE(VarOccurrences& vo0, Item* ci0)
    : vo(vo0), ci(ci0) {}
    void vId(const Id& id) {
      if(id.decl())
        vo.add(id.decl(),ci);
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
    VarDeclI* vdi;
    CollectDecls(VarOccurrences& vo0,
                 std::vector<VarDecl*>& vd0,
                 VarDeclI* vdi0)
    : vo(vo0), vd(vd0), vdi(vdi0) {}
    void vId(Id& id) {
      if (id.decl() && vo.remove(id.decl(),vdi) == 0)
        vd.push_back(id.decl());
    }
  };

  bool isOutput(VarDecl* vd);
  
  /// Simplyfy models in \a env
  void optimize(Env& env);
  
}

#endif
