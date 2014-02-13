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

#include <unordered_set>

namespace MiniZinc {

  void VarOccurrences::add(VarDeclI *i, int idx_i)
  {
    idx.insert(i->e(), idx_i);
  }
  int VarOccurrences::find(VarDecl* vd)
  {
    ExpressionMap<int>::iterator it = idx.find(vd);
    return it==idx.end() ? -1 : it->second;
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
    topDown(ce,si->ann());
  }

  class AnnotateVardecl : public ItemVisitor {
  public:
    VarOccurrences& vo;
    AnnotateVardecl(VarOccurrences& vo0) : vo(vo0) {}
    void vVarDeclI(VarDeclI* v) {
      GCLock _gcl;
      std::vector<Expression*> args(1);
      args[0] = new IntLit(Location(),vo.occurrences(v->e()));
      Call* c = new Call(Location(),"occ",args);
      v->e()->addAnnotation(new Annotation(Location(),c));
    }
  };

  bool isOutput(VarDecl* vd) {
    Annotation* a = vd->ann();
    while (a) {
      if (a->e()) {
        if (a->e()==constants().ann.output_var)
          return true;
        if (Call* c = a->e()->dyn_cast<Call>()) {
          if (c->id() == constants().ann.output_array)
            return true;
        }
      }
      a = a->next();
    }
    return false;
  }
  
  void optimize(Env& env) {
  }

}
