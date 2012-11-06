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
    void insert(const CtxStringH& e0, const CtxStringH& e1) {
      m.insert(std::pair<void*,void*>(e0.ctxstr(),e1.ctxstr()));
    }
    CtxString* find(CtxStringH e) {
      auto it = m.find(e.ctxstr());
      if (it==m.end()) return NULL;
      return static_cast<CtxString*>(it->second);
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
    void insert(CtxVec<T>* e0, CtxVec<T>* e1) {
      m.insert(std::pair<void*,void*>(e0,e1));
    }
    template<class T>
    CtxVec<T>* find(CtxVec<T>* e) {
      auto it = m.find(e);
      if (it==m.end()) return NULL;
      return static_cast<CtxVec<T>*>(it->second);
    }
  };

  Expression* copy(ASTContext& ctx, CopyMap& m,
                   Expression* e) {
    if (e==NULL) return NULL;
    if (Expression* cached = m.find(e))
      return cached;
    switch (e->_eid) {
    case Expression::E_INTLIT:
      {
        IntLit* c = IntLit::a(ctx,Location(),e->cast<IntLit>()->_v);
        m.insert(e,c);
        return c;
      }
    case Expression::E_FLOATLIT:
      {
        FloatLit* c = FloatLit::a(ctx,Location(),e->cast<FloatLit>()->_v);
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
            isv = IntSetVal::ai(ctx,r);
            m.insert(s->_isv,isv);
          }
          c = SetLit::a(ctx,Location(),isv);
        } else {          
          if (CtxVec<Expression*>* e = m.find(s->_v)) {
            c = SetLit::a(ctx,Location(),e);
          } else {
            std::vector<Expression*> elems(s->_v->size());
            for (unsigned int i=s->_v->size(); i--;)
              elems[i] = copy(ctx,m,(*s->_v)[i]);
            CtxVec<Expression*>* ce = CtxVec<Expression*>::a(ctx,elems);
            m.insert(s->_v,ce);
            c = SetLit::a(ctx,Location(),ce);
          }
        }
        c->_type = s->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_BOOLLIT:
      {
        BoolLit* c = BoolLit::a(ctx,Location(),e->cast<BoolLit>()->_v);
        m.insert(e,c);
        return c;
      }
    case Expression::E_STRINGLIT:
      {
        StringLit* sl = e->cast<StringLit>();
        StringLit* c;
        if (CtxString* cs = m.find(sl->_v)) {
          c = StringLit::a(ctx,Location(),CtxStringH(cs));
        } else {
          CtxStringH s(ctx,sl->_v.str());
          m.insert(sl->_v,s);
          c = StringLit::a(ctx,Location(),s);
        }
        m.insert(e,c);
        return c;
      }
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        CtxStringH id_v;
        if (CtxString* cs = m.find(id->_v)) {
          id_v = CtxStringH(cs);
        } else {
          id_v = CtxStringH(ctx,id->_v.str());
          m.insert(id->_v,id_v);
        }
        Id* c = Id::a(ctx,Location(),id_v,
                      static_cast<VarDecl*>(copy(ctx,m,id->_decl)));
        c->_type = id->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_ANON:
      {
        AnonVar* c = AnonVar::a(ctx,Location());
        m.insert(e,c);
        return c;
      }
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        CtxVec<Expression*>* v;
        if (CtxVec<Expression*>* cv = m.find(al->_v)) {
          v = cv;
        } else {
          std::vector<Expression*> elems(al->_v->size());
          for (unsigned int i=al->_v->size(); i--;)
            elems[i] = copy(ctx,m,(*al->_v)[i]);
          CtxVec<Expression*>* ce = CtxVec<Expression*>::a(ctx,elems);
          m.insert(al->_v,ce);
          v = ce;
        }
        std::vector<std::pair<int,int> > dims(al->_dims->size());
        for (unsigned int i=al->_dims->size(); i--;)
          dims[i] = (*al->_dims)[i];
        ArrayLit* c = ArrayLit::a(ctx,Location(),v,dims);
        c->_type = al->_type;
        m.insert(e,c);
        return c;
      }
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        CtxVec<Expression*>* idx;
        if (CtxVec<Expression*>* cidx = m.find(aa->_idx)) {
          idx = cidx;
        } else {
          std::vector<Expression*> elems(aa->_idx->size());
          for (unsigned int i=aa->_idx->size(); i--;)
            elems[i] = copy(ctx,m,(*aa->_idx)[i]);
          CtxVec<Expression*>* ce = CtxVec<Expression*>::a(ctx,elems);
          m.insert(aa->_idx,ce);
          idx = ce;
        }
        ArrayAccess* c = 
          ArrayAccess::a(ctx,Location(),copy(ctx,m,aa->_v),idx);
        m.insert(e,c);
        return c;
      }
    case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        Generators g;
        g._w = copy(ctx,m,c->_where);
        for (Generator* gi : *c->_g) {
          std::vector<VarDecl*> vv(gi->_v->size());
          for (unsigned int i=gi->_v->size(); i--;)
            vv[i] = static_cast<VarDecl*>(copy(ctx,m,(*gi->_v)[i]));
          CtxVec<VarDecl*>* v = CtxVec<VarDecl*>::a(ctx,vv);
          Generator* ng = Generator::a(ctx,v,copy(ctx,m,gi->_in));
          g._g.push_back(ng);
        }
        Comprehension* cc = 
          Comprehension::a(ctx,Location(),copy(ctx,m,c->_e),g,c->_set);
        m.insert(c,cc);
        return cc;
      }
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        std::vector<ITE::IfThen> ifthen(ite->_e_if->size());
        for (unsigned int i=ite->_e_if->size(); i--;) {
          ifthen[i] = ITE::IfThen(copy(ctx,m,(*ite->_e_if)[i].first),
                                  copy(ctx,m,(*ite->_e_if)[i].second));
        }
        ITE* c = ITE::a(ctx,Location(),ifthen,copy(ctx,m,ite->_e_else));
        m.insert(e,c);
        return c;
      }
    case Expression::E_BINOP:
      {
        BinOp* b = e->cast<BinOp>();
        BinOp* c = BinOp::a(ctx,Location(),copy(ctx,m,b->_e0),b->_op,
                            copy(ctx,m,b->_e1));
        m.insert(e,c);
        return c;
      }
    case Expression::E_UNOP:
      {
        UnOp* b = e->cast<UnOp>();
        UnOp* c = UnOp::a(ctx,Location(),b->_op,copy(ctx,m,b->_e0));
        m.insert(e,c);
        return c;
      }
    case Expression::E_CALL:
      {
        Call* ca = e->cast<Call>();
        std::vector<Expression*> args(ca->_args->size());
        for (unsigned int i=ca->_args->size(); i--;)
          args[i] = copy(ctx,m,(*ca->_args)[i]);
        CtxStringH id_v;
        if (CtxString* cs = m.find(ca->_id)) {
          id_v = CtxStringH(cs);
        } else {
          id_v = CtxStringH(ctx,ca->_id.str());
          m.insert(ca->_id,id_v);
        }
        Call* c = Call::a(ctx,Location(),id_v,args);
        m.insert(e,c);
        return c;
      }
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        CtxStringH id_v;
        if (CtxString* cs = m.find(vd->_id)) {
          id_v = CtxStringH(cs);
        } else {
          id_v = CtxStringH(ctx,vd->_id.str());
          m.insert(vd->_id,id_v);
        }
        VarDecl* c = VarDecl::a(ctx,Location(),
          static_cast<TypeInst*>(copy(ctx,m,vd->_ti)),
          id_v,copy(ctx,m,vd->_e));
        c->_toplevel = vd->_toplevel;
        c->_type = vd->_type;
        /// TODO: what about c->_allocator ?
        m.insert(e,c);
        return c;
      }
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        std::vector<Expression*> let(l->_let->size());
        for (unsigned int i=l->_let->size(); i--;)
          let[i] = copy(ctx,m,(*l->_let)[i]);
        Let* c = Let::a(ctx,Location(),let,copy(ctx,m,l->_in));
        m.insert(e,c);
        return c;
      }
    case Expression::E_ANN:
      {
        Annotation* a = e->cast<Annotation>();
        Annotation* c = Annotation::a(ctx,Location(),copy(ctx,m,a->_e),
          static_cast<Annotation*>(copy(ctx,m,a->_a)));
        m.insert(e,c);
        return c;
      }
    case Expression::E_TI:
      {
        TypeInst* t = e->cast<TypeInst>();
        CtxVec<TypeInst*>* r;
        if (t->_ranges==NULL) {
          r = NULL;
        } else if (CtxVec<TypeInst*>* cr = m.find(t->_ranges)) {
          r = cr;
        } else {
          std::vector<TypeInst*> rr(t->_ranges->size());
          for (unsigned int i=t->_ranges->size(); i--;)
            rr[i] = static_cast<TypeInst*>(copy(ctx,m,(*t->_ranges)[i]));
          r = CtxVec<TypeInst*>::a(ctx,rr);
        }
        TypeInst* c = TypeInst::a(ctx,Location(),t->_type,
          copy(ctx,m,t->_domain),r);
        m.insert(e,c);
        return c;
      }
    case Expression::E_TIID:
      {
        TIId* t = e->cast<TIId>();
        TIId* c = TIId::a(ctx,Location(),t->_v.str());
        m.insert(e,c);
        return c;
      }
    }
  }

  Expression* copy(ASTContext& ctx, Expression* e) {
    CopyMap m;
    return copy(ctx,m,e);
  }

  Model* copy(ASTContext& ctx, CopyMap& cm, Model* m);

  Item* copy(ASTContext& ctx, CopyMap& m, Item* i) {
    if (i==NULL) return NULL;
    if (Item* cached = m.find(i))
      return cached;
    switch (i->_iid) {
    case Item::II_INC:
      {
        IncludeI* ii = i->cast<IncludeI>();
        IncludeI* c = 
          IncludeI::a(ctx,Location(),CtxStringH(ctx,ii->_f.str()));
        c->setModel(copy(ctx,m,ii->_m),ii->_own);
        m.insert(i,c);
        return c;
      }
    case Item::II_VD:
      {
        VarDeclI* v = i->cast<VarDeclI>();
        VarDeclI* c = VarDeclI::a(ctx,Location(),
          static_cast<VarDecl*>(copy(ctx,m,v->_e)));
        m.insert(i,c);
        return c;
      }
    case Item::II_ASN:
      {
        AssignI* a = i->cast<AssignI>();
        AssignI* c = 
          AssignI::a(ctx,Location(),a->_id.str(),copy(ctx,m,a->_e));
        c->_decl = static_cast<VarDecl*>(copy(ctx,m,a->_decl));
        m.insert(i,c);
        return c;
      }
    case Item::II_CON:
      {
        ConstraintI* cc = i->cast<ConstraintI>();
        ConstraintI* c = ConstraintI::a(ctx,Location(),copy(ctx,m,cc->_e));
        m.insert(i,c);
        return c;
      }
    case Item::II_SOL:
      {
        SolveI* s = i->cast<SolveI>();
        SolveI* c;
        switch (s->_st) {
        case SolveI::ST_SAT:
          c = SolveI::sat(ctx,Location(),
            static_cast<Annotation*>(copy(ctx,m,s->_ann)));
          break;
        case SolveI::ST_MIN:
          c = SolveI::min(ctx,Location(),copy(ctx,m,s->_e),
            static_cast<Annotation*>(copy(ctx,m,s->_ann)));
          break;
        case SolveI::ST_MAX:
          c = SolveI::min(ctx,Location(),copy(ctx,m,s->_e),
            static_cast<Annotation*>(copy(ctx,m,s->_ann)));
          break;
        }
        m.insert(i,c);
        return c;
      }
    case Item::II_OUT:
      {
        OutputI* o = i->cast<OutputI>();
        OutputI* c = OutputI::a(ctx,Location(),copy(ctx,m,o->_e));
        m.insert(i,c);
        return c;
      }
    case Item::II_FUN:
      {
        FunctionI* f = i->cast<FunctionI>();
        std::vector<VarDecl*> params(f->_params->size());
        for (unsigned int j=f->_params->size(); j--;)
          params[j] = static_cast<VarDecl*>(copy(ctx,m,(*f->_params)[j]));
        FunctionI* c = FunctionI::a(ctx,Location(),f->_id.str(),
          static_cast<TypeInst*>(copy(ctx,m,f->_ti)),
          params, copy(ctx,m,f->_e),
          static_cast<Annotation*>(copy(ctx,m,f->_ann)));
        m.insert(i,c);
        return c;
      }
    }
  }

  Item* copy(ASTContext& ctx, Item* i) {
    CopyMap m;
    return copy(ctx,m,i);
  }

  Model* copy(ASTContext& ctx, CopyMap& cm, Model* m) {
    if (m==NULL) return NULL;
    if (Model* cached = cm.find(m))
      return cached;
    Model* c = new Model;
    for (Item* i : m->_items)
      c->addItem(copy(ctx,cm,i));
    cm.insert(m,c);
    return c;
  }
  Model* copy(ASTContext& ctx, Model* m) {
    CopyMap cm;
    return copy(ctx,cm,m);
  }
  
}
