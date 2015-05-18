/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/copy.hh>
#include <minizinc/hash.hh>

namespace MiniZinc {

  void CopyMap::insert(Expression* e0, Expression* e1) {
    m.insert(std::pair<void*,void*>(e0,e1));
    m.insert(std::pair<void*,void*>(e1,e1));
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
    loc.is_introduced = _loc.is_introduced;
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

  void copy_ann(EnvI& env, CopyMap& m, Annotation& oldAnn, Annotation& newAnn, bool followIds, bool copyFundecls);
  
  Expression* copy(EnvI& env, CopyMap& m, Expression* e, bool followIds, bool copyFundecls) {
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
        SetLit* c = new SetLit(copy_location(m,e),NULL);
        m.insert(e,c);
        if (s->isv()) {
          IntSetVal* isv;
          if (IntSetVal* isvc = m.find(s->isv())) {
            isv = isvc;
          } else {
            IntSetRanges r(s->isv());
            isv = IntSetVal::ai(r);
            m.insert(s->isv(),isv);
          }
          c->isv(isv);
        } else {          
          if (ASTExprVecO<Expression*>* ve = m.find(s->v())) {
            c->v(ASTExprVec<Expression>(ve));
          } else {
            std::vector<Expression*> elems(s->v().size());
            for (unsigned int i=s->v().size(); i--;)
              elems[i] = copy(env,m,s->v()[i],followIds,copyFundecls);
            ASTExprVec<Expression> ce(elems);
            m.insert(s->v(),ce);
            c->v(ce);
          }
        }
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
        if (e==constants().absent)
          return e;
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
            return cur;
          } else {
            return copy(env,m,cur,false);
          }
        } else {
          Id* c;
          if (id->decl()) {
            VarDecl* vd = static_cast<VarDecl*>(copy(env,m,id->decl(),followIds,copyFundecls));
            c = vd->id();
          } else {
            if (id->idn()!=-1) {
              c = new Id(copy_location(m,e),id->idn(),NULL);
            } else {
              ASTString id_v;
              if (ASTStringO* cs = m.find(id->v())) {
                id_v = ASTString(cs);
              } else {
                id_v = ASTString(id->v().str());
                m.insert(id->v(),id_v);
              }
              c = new Id(copy_location(m,e),id_v,NULL);
            }
          }
          m.insert(e,c);
          ret = c;
        }
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
        std::vector<std::pair<int,int> > dims(al->dims());
        for (unsigned int i=al->dims(); i--;) {
          dims[i].first = al->min(i);
          dims[i].second = al->max(i);
        }
        ArrayLit* c = new ArrayLit(copy_location(m,e),std::vector<Expression*>(),dims);
        m.insert(e,c);

        ASTExprVecO<Expression*>* v;
        if (ASTExprVecO<Expression*>* cv = m.find(al->v())) {
          v = cv;
        } else {
          std::vector<Expression*> elems(al->v().size());
          for (unsigned int i=al->v().size(); i--;)
            elems[i] = copy(env,m,al->v()[i],followIds,copyFundecls);
          ASTExprVec<Expression> ce(elems);
          m.insert(al->v(),ce);
          v = ce.vec();
        }
        c->v(ASTExprVec<Expression>(v));
        ret = c;
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        ArrayAccess* c = new ArrayAccess(copy_location(m,e),NULL,std::vector<Expression*>());
        m.insert(e,c);
        
        ASTExprVecO<Expression*>* idx;
        if (ASTExprVecO<Expression*>* cidx = m.find(aa->idx())) {
          idx = cidx;
        } else {
          std::vector<Expression*> elems(aa->idx().size());
          for (unsigned int i=aa->idx().size(); i--;)
            elems[i] = copy(env,m,aa->idx()[i],followIds,copyFundecls);
          ASTExprVec<Expression> ce(elems);
          m.insert(aa->idx(),ce);
          idx = ce.vec();
        }
        c->v(copy(env,m,aa->v(),followIds,copyFundecls));
        c->idx(ASTExprVec<Expression>(idx));
        ret = c;
      }
      break;
    case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        Generators g;
        Comprehension* cc =
          new Comprehension(copy_location(m,e),NULL,g,c->set());
        m.insert(c,cc);

        g._w = copy(env,m,c->where(),followIds,copyFundecls);
        for (int i=0; i<c->n_generators(); i++) {
          std::vector<VarDecl*> vv;
          for (int j=0; j<c->n_decls(i); j++)
            vv.push_back(static_cast<VarDecl*>(copy(env,m,c->decl(i,j),followIds,copyFundecls)));
          g._g.push_back(Generator(vv,copy(env,m,c->in(i),followIds,copyFundecls)));
        }
        cc->init(copy(env,m,c->e(),followIds,copyFundecls),g);
        ret = cc;
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        ITE* c = new ITE(copy_location(m,e),std::vector<Expression*>(),NULL);
        m.insert(e,c);
        std::vector<Expression*> ifthen(2*ite->size());
        for (unsigned int i=ite->size(); i--;) {
          ifthen[2*i] = copy(env,m,ite->e_if(i),followIds,copyFundecls);
          ifthen[2*i+1] = copy(env,m,ite->e_then(i),followIds,copyFundecls);
        }
        c->init(ifthen,copy(env,m,ite->e_else(),followIds,copyFundecls));
        ret = c;
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* b = e->cast<BinOp>();
        BinOp* c = new BinOp(copy_location(m,e),NULL,b->op(),NULL);
        m.insert(e,c);
        c->lhs(copy(env,m,b->lhs(),followIds,copyFundecls));
        c->rhs(copy(env,m,b->rhs(),followIds,copyFundecls));
        ret = c;
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* b = e->cast<UnOp>();
        UnOp* c = new UnOp(copy_location(m,e),b->op(),NULL);
        m.insert(e,c);
        c->e(copy(env,m,b->e(),followIds,copyFundecls));
        ret = c;
      }
      break;
    case Expression::E_CALL:
      {
        Call* ca = e->cast<Call>();
        ASTString id_v;
        if (ASTStringO* cs = m.find(ca->id())) {
          id_v = ASTString(cs);
        } else {
          id_v = ASTString(ca->id().str());
          m.insert(ca->id(),id_v);
        }
        Call* c = new Call(copy_location(m,e),id_v,std::vector<Expression*>());

        if (copyFundecls) {
          c->decl(Item::cast<FunctionI>(copy(env,m,ca->decl())));
        } else {
          c->decl(ca->decl());
        }
        
        m.insert(e,c);
        std::vector<Expression*> args(ca->args().size());
        for (unsigned int i=ca->args().size(); i--;)
          args[i] = copy(env,m,ca->args()[i],followIds,copyFundecls);
        c->args(args);
        ret = c;
      }
      break;
    case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        VarDecl* c;
        if (vd->id()->idn()==-1) {
          ASTString id_v;
          if (ASTStringO* cs = m.find(vd->id()->v())) {
            id_v = ASTString(cs);
          } else {
            id_v = ASTString(vd->id()->v().str());
            m.insert(vd->id()->v(),id_v);
          }
          c = new VarDecl(copy_location(m,e),NULL,id_v,NULL);
        } else {
          c = new VarDecl(copy_location(m,e),NULL,vd->id()->idn(),NULL);
        }
        c->toplevel(vd->toplevel());
        c->introduced(vd->introduced());
        if (vd->flat()==vd)
          c->flat(c);
        else
          c->flat(vd->flat());
        c->payload(vd->payload());
        m.insert(e,c);
        c->ti(static_cast<TypeInst*>(copy(env,m,vd->ti(),followIds,copyFundecls)));
        c->e(copy(env,m,vd->e(),followIds,copyFundecls));
        c->type(c->ti()->type());
        c->id()->type(c->type());
        ret = c;
      }
      break;
    case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        std::vector<Expression*> let(l->let().size());
        for (unsigned int i=l->let().size(); i--;)
          let[i] = copy(env,m,l->let()[i],followIds,copyFundecls);
        Let* c = new Let(copy_location(m,e),let,copy(env,m,l->in(),followIds,copyFundecls));
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
            rr[i] = static_cast<TypeInst*>(copy(env,m,t->ranges()[i],followIds,copyFundecls));
          r = ASTExprVecO<TypeInst*>::a(rr);
        }
        TypeInst* c = new TypeInst(copy_location(m,e),t->type(),
          ASTExprVec<TypeInst>(r),copy(env,m,t->domain(),followIds,copyFundecls));
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
    if (!ret->isa<Id>() || ret->cast<Id>()->decl()==NULL)
      ret->type(e->type());
    copy_ann(env,m,e->ann(),ret->ann(),followIds,copyFundecls);
    return ret;
  }

  void copy_ann(EnvI& env, CopyMap& m, Annotation& oldAnn, Annotation& newAnn, bool followIds, bool copyFundecls) {
    for (ExpressionSetIter it = oldAnn.begin(); it != oldAnn.end(); ++it)
      newAnn.add(copy(env,m,*it,followIds,copyFundecls));
  }

  Expression* copy(EnvI& env, Expression* e, bool followIds, bool copyFundecls) {
    CopyMap m;
    return copy(env,m,e,followIds,copyFundecls);
  }

  Model* copy(EnvI& env, CopyMap& cm, Model* m, bool);

  Item* copy(EnvI& env, CopyMap& m, Item* i, bool followIds, bool copyFundecls, bool followIncludes) {
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
        m.insert(i,c);
        if (followIncludes) {
          c->m(copy(env,m,ii->m(),followIncludes),ii->own());
        } else
          c->m(ii->m(), false);
        return c;
      }
    case Item::II_VD:
      {
        VarDeclI* v = i->cast<VarDeclI>();
        VarDeclI* c = new VarDeclI(copy_location(m,i),
          NULL);
        m.insert(i,c);
        c->e(static_cast<VarDecl*>(copy(env,m,v->e(),followIds,copyFundecls)));
        return c;
      }
    case Item::II_ASN:
      {
        AssignI* a = i->cast<AssignI>();
        AssignI* c = 
          new AssignI(copy_location(m,i),
                     a->id().str(),NULL);
        m.insert(i,c);
        c->e(copy(env,m,a->e(),followIds,copyFundecls));
        c->decl(static_cast<VarDecl*>(copy(env,m,a->decl(),followIds,copyFundecls)));
        return c;
      }
    case Item::II_CON:
      {
        ConstraintI* cc = i->cast<ConstraintI>();
        ConstraintI* c = new ConstraintI(copy_location(m,i),NULL);
        m.insert(i,c);
        c->e(copy(env,m,cc->e(),followIds,copyFundecls));
        return c;
      }
    case Item::II_SOL:
      {
        SolveI* s = i->cast<SolveI>();
        SolveI* c;
        switch (s->st()) {
        case SolveI::ST_SAT:
          c = SolveI::sat(Location());
          break;
        case SolveI::ST_MIN:
          c = SolveI::min(Location(),copy(env,m,s->e(),followIds,copyFundecls));
          break;
        case SolveI::ST_MAX:
          c = SolveI::max(Location(),copy(env,m,s->e(),followIds,copyFundecls));
          break;
        }
        copy_ann(env,m, s->ann(), c->ann(), followIds,copyFundecls);
        m.insert(i,c);
        return c;
      }
    case Item::II_OUT:
      {
        OutputI* o = i->cast<OutputI>();
        OutputI* c = new OutputI(copy_location(m,i),copy(env,m,o->e(),followIds,copyFundecls));
        m.insert(i,c);
        return c;
      }
    case Item::II_FUN:
      {
        FunctionI* f = i->cast<FunctionI>();
        std::vector<VarDecl*> params(f->params().size());
        for (unsigned int j=f->params().size(); j--;)
          params[j] = static_cast<VarDecl*>(copy(env,m,f->params()[j],followIds,copyFundecls));
        FunctionI* c = new FunctionI(copy_location(m,i),f->id().str(),
          static_cast<TypeInst*>(copy(env,m,f->ti(),followIds,copyFundecls)),
                                     params, copy(env,m,f->e(),followIds,copyFundecls));
        c->_builtins.e = f->_builtins.e;
        c->_builtins.i = f->_builtins.i;
        c->_builtins.f = f->_builtins.f;
        c->_builtins.b = f->_builtins.b;
        c->_builtins.s = f->_builtins.s;
        c->_builtins.str = f->_builtins.str;

        copy_ann(env,m, f->ann(), c->ann(), followIds,copyFundecls);
        m.insert(i,c);
        return c;
      }
    default: assert(false); return NULL;
    }
  }

  Item* copy(EnvI& env, Item* i, bool followIds, bool copyFundecls) {
    CopyMap m;
    return copy(env,m,i,followIds,copyFundecls);
  }

  Model* copy(EnvI& env, CopyMap& cm, Model* m, bool) {
    bool followIncludes = true;
    if (m==NULL) return NULL;
    if (Model* cached = cm.find(m))
      return cached;
    Model* c = new Model;
    for (unsigned int i=0; i<m->size(); i++)
      c->addItem(copy(env,cm,(*m)[i],false,true,followIncludes));

    for (Model::FnMap::iterator it = m->fnmap.begin(); it != m->fnmap.end(); ++it) {
      for (unsigned int i=0; i<it->second.size(); i++)
        c->registerFn(env,copy(env,cm,it->second[i],false,true)->cast<FunctionI>());
    }
    cm.insert(m,c);
    return c;
  }
  Model* copy(EnvI& env, Model* m) {
    CopyMap cm;
    return copy(env,cm,m,true);
  }
  
}
