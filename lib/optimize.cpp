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
    
    /// Remove \a i from map and return new number of occurrences
    int remove(VarDecl* v, Item* i) {
      ExpressionMap<Items>::iterator vi = _m.find(v);
      assert(vi!=_m.end());
      vi->second.erase(i);
      return vi->second.size();
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
      if(id._decl)
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
    void vSolveI(SolveI* si) {
      CollectOccurrencesE ce(vo,si);
      BottomUpIterator<CollectOccurrencesE>(ce).run(si->_e);
      BottomUpIterator<CollectOccurrencesE>(ce).run(si->_ann);
    }
  };

  class AnnotateVardecl : public ItemVisitor {
  public:
    VarOccurrences& vo;
    AnnotateVardecl(VarOccurrences& vo0) : vo(vo0) {}
    void vVarDeclI(VarDeclI* v) {
      GCLock _gcl;
      std::vector<Expression*> args(1);
      args[0] = IntLit::a(Location(),vo.occurrences(v->_e));
      Call* c = Call::a(Location(),"occ",args);
      v->_e->annotate(Annotation::a(Location(),c));
    }
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
      if(id._decl) {
        if (vo.remove(id._decl,vdi) == 0) {
          vd.push_back(id._decl);
        }
      }
    }
  };

  void removeUnused(Model* m, VarOccurrences& vo) {
    std::vector<bool> unused(m->_items.size(), true);
    ExpressionMap<int> idx;
    for (unsigned int i=0; i<m->_items.size(); i++) {
      if (VarDeclI* vdi = m->_items[i]->dyn_cast<VarDeclI>()) {
        idx.insert(vdi->_e,i);
      }
    }
    std::vector<VarDecl*> vd;
    for (unsigned int i=0; i<m->_items.size(); i++) {
      VarDeclI* vdi = m->_items[i]->dyn_cast<VarDeclI>();
      if (   vdi==NULL
          || (vo.occurrences(vdi->_e)!=0) ) {
        unused[i] = false;
      } else {
        if (vdi->_e->_e && vdi->_e->_ti->_domain != NULL) {
          if (vdi->_e->_type.isvar() && vdi->_e->_type.isbool() &&
              Expression::equal(vdi->_e->_ti->_domain,constants().lt)) {
            GCLock lock;
            m->_items.push_back(ConstraintI::a(vdi->_loc,vdi->_e->_e));
          } else {
            unused[i] = false;
          }
        } else {
          CollectDecls cd(vo,vd,vdi);
          BottomUpIterator<CollectDecls>(cd).run(vdi->_e->_e);
        }
      }
    }
    while (!vd.empty()) {
      VarDecl* cur = vd.back(); vd.pop_back();
      ExpressionMap<int>::iterator cur_idx = idx.find(cur);
      if (cur_idx != idx.end()) {
        int i = cur_idx->second;
        if (!unused[i]) {
          unused[i] = true;
          CollectDecls cd(vo,vd,m->_items[i]->cast<VarDeclI>());
          BottomUpIterator<CollectDecls>(cd).run(cur->_e);
        }
      }
    }
    unsigned int ci = 0;
    for (unsigned int i=0; i<m->_items.size(); i++) {
      if (!unused[i]) {
        m->_items[ci++] = m->_items[i];
      } else {
      }
    }
    m->_items.resize(ci);
  }

  void optimize(Model* m) {
    VarOccurrences vo;
    CollectOccurrencesI co(vo);
    iterItems<CollectOccurrencesI>(co,m);
    // AnnotateVardecl avd(vo);
    // iterItems<AnnotateVardecl>(avd,m);
    removeUnused(m,vo);
  }

}
