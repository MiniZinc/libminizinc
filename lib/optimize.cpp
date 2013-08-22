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

#include <unordered_set>

namespace MiniZinc {

  class VarOccurrences {
  public:
    typedef std::unordered_set<Item*> Items;
    ExpressionMap<Items> _m;
    static Items empty;
    
    void add(VarDecl* v, Item* i) {
      ExpressionMap<Items>::iterator vi = _m.find(v);
      if (vi==_m.end()) {
        Items items; items.insert(i);
        _m.insert(v, items);
      } else {
        vi->second.insert(i);
      }
    }
    
    Items::iterator v_begin(VarDecl* v) {
      ExpressionMap<Items>::iterator vi = _m.find(v);
      if (vi==_m.end()) return empty.end();
      return vi->second.begin();
    }
    Items::iterator v_end(VarDecl* v) {
      ExpressionMap<Items>::iterator vi = _m.find(v);
      if (vi==_m.end()) return empty.end();
      return vi->second.end();
    }
    
    int occurrences(VarDecl* v) {
      ExpressionMap<Items>::iterator vi = _m.find(v);
      return (vi==_m.end() ? 0 : vi->second.size());
    }
    
  };
  
  VarOccurrences::Items VarOccurrences::empty;

  class CollectOccurrencesE : public EVisitor {
  public:
    VarOccurrences& vo;
    Item* ci;
    CollectOccurrencesE(VarOccurrences& vo0, Item* ci0)
      : vo(vo0), ci(ci0) {}
    void vId(const Id& id) {
      assert(id._decl);
      vo.add(id._decl,ci);
    }
    
  };

  class CollectOccurrencesI : public ItemVisitor {
  public:
    VarOccurrences& vo;
    CollectOccurrencesI(VarOccurrences& vo0) : vo(vo0) {}
    void vVarDeclI(VarDeclI* v) {
      CollectOccurrencesE ce(vo,v);
      BottomUpIterator<CollectOccurrencesE>(ce).run(v->_e);
    }
    void vConstraintI(ConstraintI* ci) {
      CollectOccurrencesE ce(vo,ci);
      BottomUpIterator<CollectOccurrencesE>(ce).run(ci->_e);
    }
  };

  class AnnotateVardecl : public ItemVisitor {
  public:
    VarOccurrences& vo;
    AnnotateVardecl(VarOccurrences& vo0) : vo(vo0) {}
    void vVarDeclI(VarDeclI* v) {
      std::vector<Expression*> args(1);
      args[0] = IntLit::a(Location(),vo.occurrences(v->_e));
      Call* c = Call::a(Location(),"occ",args);
      v->_e->annotate(Annotation::a(Location(),c));
    }
  };

  void optimize(Model* m) {
    VarOccurrences vo;
    CollectOccurrencesI co(vo);
    iterItems<CollectOccurrencesI>(co,m);
    AnnotateVardecl avd(vo);
    iterItems<AnnotateVardecl>(avd,m);
  }

}
