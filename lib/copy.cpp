/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/copy.hh>

namespace MiniZinc {

  class CopyMap {
  protected:
    std::unordered_map<void*,void*> m;
  public:
    void insert(Expression* e0, Expression* e1) {
      m.insert(std::pair<void*,void*>(e0,e1));
    }
    Expression* find(Expression* e) {
      auto it = m.find(e);
      if (it==m.end()) return NULL;
      return static_cast<Expression*>(it->second);
    }
    void insert(Item* e0, Item* e1) {
      m.insert(std::pair<void*,void*>(e0,e1));
    }
    Item* find(Item* e) {
      auto it = m.find(e);
      if (it==m.end()) return NULL;
      return static_cast<Item*>(it->second);
    }
    void insert(Model* e0, Model* e1) {
      m.insert(std::pair<void*,void*>(e0,e1));
    }
    Model* find(Model* e) {
      auto it = m.find(e);
      if (it==m.end()) return NULL;
      return static_cast<Model*>(it->second);
    }
    void insert(const ASTString& e0, const ASTString& e1) {
      m.insert(std::pair<void*,void*>(e0.aststr(),e1.aststr()));
    }
    ASTStringO* find(const ASTString& e) {
      auto it = m.find(e.aststr());
      if (it==m.end()) return NULL;
      return static_cast<ASTStringO*>(it->second);
    }
    void insert(IntSetVal* e0, IntSetVal* e1) {
      m.insert(std::pair<void*,void*>(e0,e1));
    }
    IntSetVal* find(IntSetVal* e) {
      auto it = m.find(e);
      if (it==m.end()) return NULL;
      return static_cast<IntSetVal*>(it->second);
    }
    template<class T>
    void insert(ASTNodeVec<T>& e0, ASTNodeVec<T>& e1) {
      m.insert(std::pair<void*,void*>(e0.vec(),e1.vec()));
    }
    template<class T>
    ASTNodeVecO<T*>* find(ASTNodeVec<T>& e) {
      auto it = m.find(e.vec());
      if (it==m.end()) return NULL;
      return static_cast<ASTNodeVecO<T*>*>(it->second);
    }
  };

  Location copy_location(CopyMap& m, Location& _loc) {
    Location loc;
    loc.first_line = _loc.first_line;
    loc.first_column = _loc.first_column;
    loc.last_line = _loc.last_line;
    loc.last_column = _loc.last_column;
    if (_loc.filename != "") {
      if (ASTStringO* f = m.find(ASTString(_loc.filename))) {
        loc.filename = ASTString(f);
      } else {
        ASTString fn(_loc.filename.str());
        m.insert(ASTString(_loc.filename), fn);
        loc.filename = fn;
      }
    } else {
      loc.filename = ASTString();
    }
    return loc;
  }
  Location copy_location(CopyMap& m, Expression* e) {
    return copy_location(m,e->_loc);
  }
  Location copy_location(CopyMap& m, Item* i) {
    return copy_location(m,i->_loc);
  }

  Expression* copy(CopyMap& m, Expression* e) {
    if (e==NULL) return NULL;
    if (Expression* cached = m.find(e))
      return cached;
    switch (e->eid()) {
    case Expression::E_INTLIT:
      {
        IntLit* c = IntLit::a(copy_location(m,e),
                              e->cast<IntLit>()->_v);
        m.insert(e,c);
        return c;
      }
    case Expression::E_FLOATLIT:
      {
        FloatLit* c = FloatLit::a(copy_location(m,e),
                                  e->cast<FloatLit>()->_v);
        m.insert(e,c);
        return c;
      }
    case Expression::E_SETLIT:
      {
        SetLit* s = e->cast<SetLit>();
        SetLit* c;
        if (s->_isv) {
          IntSetVal* isv;
          if (IntSetVal* isvc = m.find(s->_isv)) {
            isv = isvc;
          } else {
            IntSetRanges r(s->_isv);
            isv = IntSetVal::ai(r);
            m.insert(s->_isv,isv);
          }
          c = SetLit::a(copy_location(m,e),isv);
        } else {          
          if (ASTNodeVecO<Expression*>* ve = m.find(s->_v)) {
            c = SetLit::a(copy_location(m,e),ASTNodeVec<Expression>(ve));
          } else {
            std::vector<Expression*> elems(s->_v.size());
            for (unsigned int i=s->_v.size(); i--;)
              elems[i] = copy(m,s->_v[i]);
            ASTNodeVec<Expression> ce(elems);
            m.insert(s->_v,ce);
            c = SetLit::a(copy_location(m,e),ce);
          }
        }
        c->_type = s->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_BOOLLIT:
      {
        BoolLit* c = BoolLit::a(copy_location(m,e),
                                e->cast<BoolLit>()->_v);
        m.insert(e,c);
        return c;
      }
    case Expression::E_STRINGLIT:
      {
        StringLit* sl = e->cast<StringLit>();
        StringLit* c;
        if (ASTStringO* cs = m.find(sl->_v)) {
          c = StringLit::a(copy_location(m,e),ASTString(cs));
        } else {
          ASTString s(sl->_v.str());
          m.insert(sl->_v,s);
          c = StringLit::a(copy_location(m,e),s);
        }
        m.insert(e,c);
        return c;
      }
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        ASTString id_v;
        if (ASTStringO* cs = m.find(id->_v)) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(id->_v.str());
          m.insert(id->_v,id_v);
        }
        Id* c = Id::a(copy_location(m,e),id_v,
                      static_cast<VarDecl*>(copy(m,id->_decl)));
        c->_type = id->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_ANON:
      {
        AnonVar* c = AnonVar::a(copy_location(m,e));
        m.insert(e,c);
        return c;
      }
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        ASTNodeVecO<Expression*>* v;
        if (ASTNodeVecO<Expression*>* cv = m.find(al->_v)) {
          v = cv;
        } else {
          std::vector<Expression*> elems(al->_v.size());
          for (unsigned int i=al->_v.size(); i--;)
            elems[i] = copy(m,al->_v[i]);
          ASTNodeVec<Expression> ce(elems);
          m.insert(al->_v,ce);
          v = ce.vec();
        }
        std::vector<std::pair<int,int> > dims(al->dims());
        for (unsigned int i=al->dims(); i--;) {
          dims[i].first = al->min(i);
          dims[i].second = al->max(i);
        }
        ArrayLit* c = ArrayLit::a(copy_location(m,e),
                                  ASTNodeVec<Expression>(v),dims);
        c->_type = al->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        ASTNodeVecO<Expression*>* idx;
        if (ASTNodeVecO<Expression*>* cidx = m.find(aa->_idx)) {
          idx = cidx;
        } else {
          std::vector<Expression*> elems(aa->_idx.size());
          for (unsigned int i=aa->_idx.size(); i--;)
            elems[i] = copy(m,aa->_idx[i]);
          ASTNodeVec<Expression> ce(elems);
          m.insert(aa->_idx,ce);
          idx = ce.vec();
        }
        ArrayAccess* c = 
          ArrayAccess::a(copy_location(m,e),copy(m,aa->_v),idx);
        m.insert(e,c);
        return c;
      }
    case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        Generators g;
        g._w = copy(m,c->_where);
        for (unsigned int i=0; i<c->_g_idx.size()-1; i++) {
          std::vector<VarDecl*> vv;
          for (unsigned int j=c->_g_idx[i]+1; j<c->_g_idx[i+1]; j++)
            vv.push_back(static_cast<VarDecl*>(copy(m,c->_g[j])));
          g._g.push_back(Generator(vv,c->_g[c->_g_idx[i]]));
        }
        Comprehension* cc = 
          Comprehension::a(copy_location(m,e),
                           copy(m,c->_e),g,c->set());
        m.insert(c,cc);
        return cc;
      }
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        std::vector<Expression*> ifthen(ite->_e_if_then.size());
        for (unsigned int i=ite->_e_if_then.size(); i--;) {
          ifthen[i] = copy(m,ite->_e_if_then[i]);
        }
        ITE* c = ITE::a(copy_location(m,e),
                        ifthen,copy(m,ite->_e_else));
        m.insert(e,c);
        return c;
      }
    case Expression::E_BINOP:
      {
        BinOp* b = e->cast<BinOp>();
        BinOp* c = BinOp::a(copy_location(m,e),
                            copy(m,b->_e0),b->op(),
                            copy(m,b->_e1));
        m.insert(e,c);
        return c;
      }
    case Expression::E_UNOP:
      {
        UnOp* b = e->cast<UnOp>();
        UnOp* c = UnOp::a(copy_location(m,e),
                          b->op(),copy(m,b->_e0));
        m.insert(e,c);
        return c;
      }
    case Expression::E_CALL:
      {
        Call* ca = e->cast<Call>();
        std::vector<Expression*> args(ca->_args.size());
        for (unsigned int i=ca->_args.size(); i--;)
          args[i] = copy(m,ca->_args[i]);
        ASTString id_v;
        if (ASTStringO* cs = m.find(ca->_id)) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(ca->_id.str());
          m.insert(ca->_id,id_v);
        }
        Call* c = Call::a(copy_location(m,e),id_v,args);
        c->_decl = ca->_decl;
        m.insert(e,c);
        return c;
      }
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        ASTString id_v;
        if (ASTStringO* cs = m.find(vd->_id)) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(vd->_id.str());
          m.insert(vd->_id,id_v);
        }
        VarDecl* c = VarDecl::a(copy_location(m,e),
          static_cast<TypeInst*>(copy(m,vd->_ti)),
          id_v,copy(m,vd->_e));
        c->toplevel(vd->toplevel());
        c->introduced(vd->introduced());
        c->_type = vd->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        std::vector<Expression*> let(l->_let.size());
        for (unsigned int i=l->_let.size(); i--;)
          let[i] = copy(m,l->_let[i]);
        Let* c = Let::a(copy_location(m,e),let,copy(m,l->_in));
        m.insert(e,c);
        return c;
      }
    case Expression::E_ANN:
      {
        Annotation* a = e->cast<Annotation>();
        Annotation* c = Annotation::a(copy_location(m,e),
                                      copy(m,a->_e),
          static_cast<Annotation*>(copy(m,a->_a)));
        m.insert(e,c);
        return c;
      }
    case Expression::E_TI:
      {
        TypeInst* t = e->cast<TypeInst>();
        ASTNodeVecO<TypeInst*>* r;
        if (t->_ranges.size()==0) {
          r = NULL;
        } else if (ASTNodeVecO<TypeInst*>* cr = m.find(t->_ranges)) {
          r = cr;
        } else {
          std::vector<TypeInst*> rr(t->_ranges.size());
          for (unsigned int i=t->_ranges.size(); i--;)
            rr[i] = static_cast<TypeInst*>(copy(m,t->_ranges[i]));
          r = ASTNodeVecO<TypeInst*>::a(rr);
        }
        TypeInst* c = TypeInst::a(copy_location(m,e),t->_type,
          ASTNodeVec<TypeInst>(r),copy(m,t->_domain));
        m.insert(e,c);
        return c;
      }
    case Expression::E_TIID:
      {
        TIId* t = e->cast<TIId>();
        TIId* c = TIId::a(copy_location(m,e),t->_v.str());
        m.insert(e,c);
        return c;
      }
    }
  }

  Expression* copy(Expression* e) {
    CopyMap m;
    return copy(m,e);
  }

  Model* copy(CopyMap& cm, Model* m);

  Item* copy(CopyMap& m, Item* i) {
    if (i==NULL) return NULL;
    if (Item* cached = m.find(i))
      return cached;
    switch (i->iid()) {
    case Item::II_INC:
      {
        IncludeI* ii = i->cast<IncludeI>();
        IncludeI* c = 
          IncludeI::a(copy_location(m,i),
                      ASTString(ii->_f.str()));
        c->setModel(copy(m,ii->_m),ii->own());
        m.insert(i,c);
        return c;
      }
    case Item::II_VD:
      {
        VarDeclI* v = i->cast<VarDeclI>();
        VarDeclI* c = VarDeclI::a(copy_location(m,i),
          static_cast<VarDecl*>(copy(m,v->_e)));
        m.insert(i,c);
        return c;
      }
    case Item::II_ASN:
      {
        AssignI* a = i->cast<AssignI>();
        AssignI* c = 
          AssignI::a(copy_location(m,i),
                     a->_id.str(),copy(m,a->_e));
        c->_decl = static_cast<VarDecl*>(copy(m,a->_decl));
        m.insert(i,c);
        return c;
      }
    case Item::II_CON:
      {
        ConstraintI* cc = i->cast<ConstraintI>();
        ConstraintI* c = ConstraintI::a(copy_location(m,i),
                                        copy(m,cc->_e));
        m.insert(i,c);
        return c;
      }
    case Item::II_SOL:
      {
        SolveI* s = i->cast<SolveI>();
        SolveI* c;
        switch (s->st()) {
        case SolveI::ST_SAT:
          c = SolveI::sat(Location(),
            static_cast<Annotation*>(copy(m,s->_ann)));
          break;
        case SolveI::ST_MIN:
          c = SolveI::min(Location(),copy(m,s->_e),
            static_cast<Annotation*>(copy(m,s->_ann)));
          break;
        case SolveI::ST_MAX:
          c = SolveI::min(Location(),copy(m,s->_e),
            static_cast<Annotation*>(copy(m,s->_ann)));
          break;
        }
        m.insert(i,c);
        return c;
      }
    case Item::II_OUT:
      {
        OutputI* o = i->cast<OutputI>();
        OutputI* c = OutputI::a(copy_location(m,i),copy(m,o->_e));
        m.insert(i,c);
        return c;
      }
    case Item::II_FUN:
      {
        FunctionI* f = i->cast<FunctionI>();
        std::vector<VarDecl*> params(f->_params.size());
        for (unsigned int j=f->_params.size(); j--;)
          params[j] = static_cast<VarDecl*>(copy(m,f->_params[j]));
        FunctionI* c = FunctionI::a(copy_location(m,i),f->_id.str(),
          static_cast<TypeInst*>(copy(m,f->_ti)),
          params, copy(m,f->_e),
          static_cast<Annotation*>(copy(m,f->_ann)));
        m.insert(i,c);
        return c;
      }
    }
  }

  Item* copy(Item* i) {
    CopyMap m;
    return copy(m,i);
  }

  Model* copy(CopyMap& cm, Model* m) {
    if (m==NULL) return NULL;
    if (Model* cached = cm.find(m))
      return cached;
    Model* c = new Model;
    for (Item* i : m->_items)
      c->addItem(copy(cm,i));
    cm.insert(m,c);
    return c;
  }
  Model* copy(Model* m) {
    CopyMap cm;
    return copy(cm,m);
  }
  
}
