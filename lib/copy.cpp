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

  void CopyMap::insert(Expression* e0, Expression* e1) {
    m.insert(std::pair<void*,void*>(e0,e1));
  }
  Expression* CopyMap::find(Expression* e) {
    MyMap::iterator it = m.find(e);
    if (it==m.end()) return NULL;
    return static_cast<Expression*>(it->second);
  }
  void CopyMap::insert(Item* e0, Item* e1) {
    m.insert(std::pair<void*,void*>(e0,e1));
  }
  Item* CopyMap::find(Item* e) {
    MyMap::iterator it = m.find(e);
    if (it==m.end()) return NULL;
    return static_cast<Item*>(it->second);
  }
  void CopyMap::insert(Model* e0, Model* e1) {
    m.insert(std::pair<void*,void*>(e0,e1));
  }
  Model* CopyMap::find(Model* e) {
    MyMap::iterator it = m.find(e);
    if (it==m.end()) return NULL;
    return static_cast<Model*>(it->second);
  }
  void CopyMap::insert(const ASTString& e0, const ASTString& e1) {
    m.insert(std::pair<void*,void*>(e0.aststr(),e1.aststr()));
  }
  ASTStringO* CopyMap::find(const ASTString& e) {
    MyMap::iterator it = m.find(e.aststr());
    if (it==m.end()) return NULL;
    return static_cast<ASTStringO*>(it->second);
  }
  void CopyMap::insert(IntSetVal* e0, IntSetVal* e1) {
    m.insert(std::pair<void*,void*>(e0,e1));
  }
  IntSetVal* CopyMap::find(IntSetVal* e) {
    MyMap::iterator it = m.find(e);
    if (it==m.end()) return NULL;
    return static_cast<IntSetVal*>(it->second);
  }

  Location copy_location(CopyMap& m, const Location& _loc) {
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
    return copy_location(m,e->loc());
  }
  Location copy_location(CopyMap& m, Item* i) {
    return copy_location(m,i->loc());
  }

  Expression* copy(CopyMap& m, Expression* e, bool followIds) {
    if (e==NULL) return NULL;
    if (Expression* cached = m.find(e))
      return cached;
    Expression* ret = NULL;
    switch (e->eid()) {
    case Expression::E_INTLIT:
      {
        IntLit* c = new IntLit(copy_location(m,e),
                              e->cast<IntLit>()->v());
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_FLOATLIT:
      {
        FloatLit* c = new FloatLit(copy_location(m,e),
                                   e->cast<FloatLit>()->v());
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_SETLIT:
      {
        SetLit* s = e->cast<SetLit>();
        SetLit* c;
        if (s->isv()) {
          IntSetVal* isv;
          if (IntSetVal* isvc = m.find(s->isv())) {
            isv = isvc;
          } else {
            IntSetRanges r(s->isv());
            isv = IntSetVal::ai(r);
            m.insert(s->isv(),isv);
          }
          c = new SetLit(copy_location(m,e),isv);
        } else {          
          if (ASTExprVecO<Expression*>* ve = m.find(s->v())) {
            c = new SetLit(copy_location(m,e),ASTExprVec<Expression>(ve));
          } else {
            std::vector<Expression*> elems(s->v().size());
            for (unsigned int i=s->v().size(); i--;)
              elems[i] = copy(m,s->v()[i],followIds);
            ASTExprVec<Expression> ce(elems);
            m.insert(s->v(),ce);
            c = new SetLit(copy_location(m,e),ce);
          }
        }
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_BOOLLIT:
      {
        BoolLit* c = new BoolLit(copy_location(m,e),
                                e->cast<BoolLit>()->v());
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_STRINGLIT:
      {
        StringLit* sl = e->cast<StringLit>();
        StringLit* c;
        if (ASTStringO* cs = m.find(sl->v())) {
          c = new StringLit(copy_location(m,e),ASTString(cs));
        } else {
          ASTString s(sl->v().str());
          m.insert(sl->v(),s);
          c = new StringLit(copy_location(m,e),s);
        }
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        
        if (followIds) {
          Id* prevId = id;
          Expression* cur = e;
          bool done = false;
          do {
            if (cur==NULL) {
              cur = prevId;
              done = true;
            } else {
              switch (cur->eid()) {
                case Expression::E_ID:
                  prevId = cur->cast<Id>();
                  cur = prevId->decl();
                  break;
                case Expression::E_VARDECL:
                  if (cur->cast<VarDecl>()->e()) {
                    cur = cur->cast<VarDecl>()->e();
                  } else {
                    cur = prevId;
                    done = true;
                  }
                  break;
                default:
                  done = true;
              }
            }
          } while (!done);
          if (cur->isa<Id>()) {
            id = cur->cast<Id>();
          } else {
            return copy(m,cur,false);
          }
        }
        ASTString id_v;
        if (ASTStringO* cs = m.find(id->v())) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(id->v().str());
          m.insert(id->v(),id_v);
        }
        Id* c = new Id(copy_location(m,e),id_v,
                       static_cast<VarDecl*>(copy(m,id->decl(),followIds)));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_ANON:
      {
        AnonVar* c = new AnonVar(copy_location(m,e));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        ASTExprVecO<Expression*>* v;
        if (ASTExprVecO<Expression*>* cv = m.find(al->v())) {
          v = cv;
        } else {
          std::vector<Expression*> elems(al->v().size());
          for (unsigned int i=al->v().size(); i--;)
            elems[i] = copy(m,al->v()[i],followIds);
          ASTExprVec<Expression> ce(elems);
          m.insert(al->v(),ce);
          v = ce.vec();
        }
        std::vector<std::pair<int,int> > dims(al->dims());
        for (unsigned int i=al->dims(); i--;) {
          dims[i].first = al->min(i);
          dims[i].second = al->max(i);
        }
        ArrayLit* c = new ArrayLit(copy_location(m,e),
                                  ASTExprVec<Expression>(v),dims);
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        ASTExprVecO<Expression*>* idx;
        if (ASTExprVecO<Expression*>* cidx = m.find(aa->idx())) {
          idx = cidx;
        } else {
          std::vector<Expression*> elems(aa->idx().size());
          for (unsigned int i=aa->idx().size(); i--;)
            elems[i] = copy(m,aa->idx()[i],followIds);
          ASTExprVec<Expression> ce(elems);
          m.insert(aa->idx(),ce);
          idx = ce.vec();
        }
        ArrayAccess* c = 
          new ArrayAccess(copy_location(m,e),copy(m,aa->v(),followIds),idx);
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        Generators g;
        g._w = copy(m,c->where(),followIds);
        for (unsigned int i=0; i<c->n_generators(); i++) {
          std::vector<VarDecl*> vv;
          for (unsigned int j=0; j<c->n_decls(i); j++)
            vv.push_back(static_cast<VarDecl*>(copy(m,c->decl(i,j),followIds)));
          g._g.push_back(Generator(vv,copy(m,c->in(i),followIds)));
        }
        Comprehension* cc = 
          new Comprehension(copy_location(m,e),
                            copy(m,c->e(),followIds),g,c->set());
        m.insert(c,cc);
        ret = cc;
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        std::vector<Expression*> ifthen(2*ite->size());
        for (unsigned int i=ite->size(); i--;) {
          ifthen[2*i] = copy(m,ite->e_if(i),followIds);
          ifthen[2*i+1] = copy(m,ite->e_then(i),followIds);
        }
        ITE* c = new ITE(copy_location(m,e),
                        ifthen,copy(m,ite->e_else(),followIds));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* b = e->cast<BinOp>();
        BinOp* c = new BinOp(copy_location(m,e),
                            copy(m,b->lhs(),followIds),b->op(),
                            copy(m,b->rhs(),followIds));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* b = e->cast<UnOp>();
        UnOp* c = new UnOp(copy_location(m,e),
                          b->op(),copy(m,b->e(),followIds));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_CALL:
      {
        Call* ca = e->cast<Call>();
        std::vector<Expression*> args(ca->args().size());
        for (unsigned int i=ca->args().size(); i--;)
          args[i] = copy(m,ca->args()[i],followIds);
        ASTString id_v;
        if (ASTStringO* cs = m.find(ca->id())) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(ca->id().str());
          m.insert(ca->id(),id_v);
        }
        Call* c = new Call(copy_location(m,e),id_v,args);
        c->decl(ca->decl());
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        ASTString id_v;
        if (ASTStringO* cs = m.find(vd->id()->v())) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(vd->id()->v().str());
          m.insert(vd->id()->v(),id_v);
        }
        VarDecl* c = new VarDecl(copy_location(m,e),
          static_cast<TypeInst*>(copy(m,vd->ti(),followIds)),
          id_v,copy(m,vd->e(),followIds));
        c->toplevel(vd->toplevel());
        c->introduced(vd->introduced());
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        std::vector<Expression*> let(l->let().size());
        for (unsigned int i=l->let().size(); i--;)
          let[i] = copy(m,l->let()[i],followIds);
        Let* c = new Let(copy_location(m,e),let,copy(m,l->in(),followIds));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_ANN:
      {
        Annotation* a = e->cast<Annotation>();
        Annotation* c = new Annotation(copy_location(m,e),copy(m,a->e(),followIds),
                                       static_cast<Annotation*>(copy(m,a->next(),followIds)));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_TI:
      {
        TypeInst* t = e->cast<TypeInst>();
        ASTExprVecO<TypeInst*>* r;
        if (t->ranges().size()==0) {
          r = NULL;
        } else if (ASTExprVecO<TypeInst*>* cr = m.find(t->ranges())) {
          r = cr;
        } else {
          std::vector<TypeInst*> rr(t->ranges().size());
          for (unsigned int i=t->ranges().size(); i--;)
            rr[i] = static_cast<TypeInst*>(copy(m,t->ranges()[i],followIds));
          r = ASTExprVecO<TypeInst*>::a(rr);
        }
        TypeInst* c = new TypeInst(copy_location(m,e),t->type(),
          ASTExprVec<TypeInst>(r),copy(m,t->domain(),followIds));
        m.insert(e,c);
        ret = c;
      }
      break;
    case Expression::E_TIID:
      {
        TIId* t = e->cast<TIId>();
        TIId* c = new TIId(copy_location(m,e),t->v().str());
        m.insert(e,c);
        ret = c;
      }
      break;
    default:
        assert(false);
    }
    ret->type(e->type());
    if (e->ann()) {
      ret->ann(copy(m,e->ann(),followIds)->cast<Annotation>());
    }
    return ret;
  }

  Expression* copy(Expression* e, bool followIds) {
    CopyMap m;
    return copy(m,e,followIds);
  }

  Model* copy(CopyMap& cm, Model* m);

  Item* copy(CopyMap& m, Item* i, bool followIds) {
    if (i==NULL) return NULL;
    if (Item* cached = m.find(i))
      return cached;
    switch (i->iid()) {
    case Item::II_INC:
      {
        IncludeI* ii = i->cast<IncludeI>();
        IncludeI* c = 
          new IncludeI(copy_location(m,i),
                      ASTString(ii->f().str()));
        c->m(copy(m,ii->m()),ii->own());
        m.insert(i,c);
        return c;
      }
    case Item::II_VD:
      {
        VarDeclI* v = i->cast<VarDeclI>();
        VarDeclI* c = new VarDeclI(copy_location(m,i),
          static_cast<VarDecl*>(copy(m,v->e(),followIds)));
        m.insert(i,c);
        return c;
      }
    case Item::II_ASN:
      {
        AssignI* a = i->cast<AssignI>();
        AssignI* c = 
          new AssignI(copy_location(m,i),
                     a->id().str(),copy(m,a->e(),followIds));
        c->decl(static_cast<VarDecl*>(copy(m,a->decl(),followIds)));
        m.insert(i,c);
        return c;
      }
    case Item::II_CON:
      {
        ConstraintI* cc = i->cast<ConstraintI>();
        ConstraintI* c = new ConstraintI(copy_location(m,i),
                                         copy(m,cc->e(),followIds));
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
            static_cast<Annotation*>(copy(m,s->ann(),followIds)));
          break;
        case SolveI::ST_MIN:
          c = SolveI::min(Location(),copy(m,s->e(),followIds),
            static_cast<Annotation*>(copy(m,s->ann(),followIds)));
          break;
        case SolveI::ST_MAX:
          c = SolveI::min(Location(),copy(m,s->e(),followIds),
            static_cast<Annotation*>(copy(m,s->ann(),followIds)));
          break;
        }
        m.insert(i,c);
        return c;
      }
    case Item::II_OUT:
      {
        OutputI* o = i->cast<OutputI>();
        OutputI* c = new OutputI(copy_location(m,i),copy(m,o->e(),followIds));
        m.insert(i,c);
        return c;
      }
    case Item::II_FUN:
      {
        FunctionI* f = i->cast<FunctionI>();
        std::vector<VarDecl*> params(f->params().size());
        for (unsigned int j=f->params().size(); j--;)
          params[j] = static_cast<VarDecl*>(copy(m,f->params()[j],followIds));
        FunctionI* c = new FunctionI(copy_location(m,i),f->id().str(),
          static_cast<TypeInst*>(copy(m,f->ti(),followIds)),
          params, copy(m,f->e(),followIds),
          static_cast<Annotation*>(copy(m,f->ann(),followIds)));
        m.insert(i,c);
        return c;
      }
    }
  }

  Item* copy(Item* i, bool followIds) {
    CopyMap m;
    return copy(m,i,followIds);
  }

  Model* copy(CopyMap& cm, Model* m) {
    if (m==NULL) return NULL;
    if (Model* cached = cm.find(m))
      return cached;
    Model* c = new Model;
    for (unsigned int i=0; i<m->_items.size(); i++)
      c->addItem(copy(cm,m->_items[i]));
    cm.insert(m,c);
    return c;
  }
  Model* copy(Model* m) {
    CopyMap cm;
    return copy(cm,m);
  }
  
}
