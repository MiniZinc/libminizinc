/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/optimize.hh>
#include <minizinc/hash.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/flatten.hh>

namespace MiniZinc {

  void VarOccurrences::add(VarDeclI *i, int idx_i)
  {
    idx.insert(i->e(), idx_i);
  }
  void VarOccurrences::add(VarDecl *e, int idx_i)
  {
    idx.insert(e, idx_i);
  }
  int VarOccurrences::find(VarDecl* vd)
  {
    ExpressionMap<int>::iterator it = idx.find(vd);
    return it==idx.end() ? -1 : it->second;
  }
  void VarOccurrences::remove(VarDecl *vd)
  {
    idx.remove(vd);
  }
  
  void VarOccurrences::add(VarDecl* v, Item* i) {
    ExpressionMap<Items>::iterator vi = _m.find(v);
    if (vi==_m.end()) {
      Items items; items.insert(i);
      _m.insert(v, items);
    } else {
      vi->second.insert(i);
    }
  }
  
  int VarOccurrences::remove(VarDecl* v, Item* i) {
    ExpressionMap<Items>::iterator vi = _m.find(v);
    assert(vi!=_m.end());
    vi->second.erase(i);
    return vi->second.size();
  }
  
  void VarOccurrences::unify(Model* m, Id* id0_0, Id *id1_0) {
    Id* id0 = id0_0->decl()->id();
    Id* id1 = id1_0->decl()->id();
    
    VarDecl* v0 = id0->decl();
    VarDecl* v1 = id1->decl();

    if (v0==v1)
      return;
    
    int v0idx = find(v0);
    assert(v0idx != -1);
    (*m)[v0idx]->remove();

    ExpressionMap<Items>::iterator vi0 = _m.find(v0);
    assert(vi0 != _m.end());
    ExpressionMap<Items>::iterator vi1 = _m.find(v1);
    assert(vi1 != _m.end());
    vi1->second.insert(vi0->second.begin(), vi0->second.end());
    
    id0->redirect(id1);
    
    remove(v0);
  }
  
  void VarOccurrences::clear(void) {
    _m.clear();
    idx.clear();
  }
  
  int VarOccurrences::occurrences(VarDecl* v) {
    ExpressionMap<Items>::iterator vi = _m.find(v);
    return (vi==_m.end() ? 0 : vi->second.size());
  }
  
  void CollectOccurrencesI::vVarDeclI(VarDeclI* v) {
    CollectOccurrencesE ce(vo,v);
    topDown(ce,v->e());
  }
  void CollectOccurrencesI::vConstraintI(ConstraintI* ci) {
    CollectOccurrencesE ce(vo,ci);
    topDown(ce,ci->e());
  }
  void CollectOccurrencesI::vSolveI(SolveI* si) {
    CollectOccurrencesE ce(vo,si);
    topDown(ce,si->e());
    for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++si)
      topDown(ce,*it);
  }

  bool isOutput(VarDecl* vd) {
    for (ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
      if (*it) {
        if (*it==constants().ann.output_var)
          return true;
        if (Call* c = (*it)->dyn_cast<Call>()) {
          if (c->id() == constants().ann.output_array)
            return true;
        }
      }
      
    }
    return false;
  }
  
  void optimize(Env& env) {
  }

}
