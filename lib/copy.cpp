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
  if (!e0->isUnboxedVal() && !e1->isUnboxedVal()) {
    _nodeMap.insert(e0, e1);
  }
}
Expression* CopyMap::find(Expression* e) { return static_cast<Expression*>(_nodeMap.find(e)); }
void CopyMap::insert(Item* e0, Item* e1) { _nodeMap.insert(e0, e1); }
Item* CopyMap::find(Item* e) { return static_cast<Item*>(_nodeMap.find(e)); }
void CopyMap::insert(Model* e0, Model* e1) { _modelMap.insert(std::make_pair(e0, e1)); }
Model* CopyMap::find(Model* e) {
  auto it = _modelMap.find(e);
  if (it == _modelMap.end()) {
    return nullptr;
  }
  return it->second;
}
void CopyMap::insert(IntSetVal* e0, IntSetVal* e1) { _nodeMap.insert(e0, e1); }
IntSetVal* CopyMap::find(IntSetVal* e) { return static_cast<IntSetVal*>(_nodeMap.find(e)); }
void CopyMap::insert(FloatSetVal* e0, FloatSetVal* e1) { _nodeMap.insert(e0, e1); }
FloatSetVal* CopyMap::find(FloatSetVal* e) { return static_cast<FloatSetVal*>(_nodeMap.find(e)); }

Location copy_location(CopyMap& m, const Location& _loc) { return _loc; }
Location copy_location(CopyMap& m, Expression* e) { return copy_location(m, e->loc()); }
Location copy_location(CopyMap& m, Item* i) { return copy_location(m, i->loc()); }

void copy_ann(EnvI& env, CopyMap& m, Annotation& oldAnn, Annotation& newAnn, bool followIds,
              bool copyFundecls, bool isFlatModel);

Expression* copy(EnvI& env, CopyMap& m, Expression* e, bool followIds, bool copyFundecls,
                 bool isFlatModel) {
  if (e == nullptr) {
    return nullptr;
  }
  if (Expression* cached = m.find(e)) {
    return cached;
  }
  Expression* ret = nullptr;
  switch (e->eid()) {
    case Expression::E_INTLIT: {
      IntLit* c = IntLit::a(e->cast<IntLit>()->v());
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_FLOATLIT: {
      FloatLit* c = FloatLit::a(e->cast<FloatLit>()->v());
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_SETLIT: {
      auto* s = e->cast<SetLit>();
      auto* c = new SetLit(copy_location(m, e), static_cast<IntSetVal*>(nullptr));
      m.insert(e, c);
      if (s->isv() != nullptr) {
        IntSetVal* isv;
        if (IntSetVal* isvc = m.find(s->isv())) {
          isv = isvc;
        } else {
          IntSetRanges r(s->isv());
          isv = IntSetVal::ai(r);
          m.insert(s->isv(), isv);
        }
        c->isv(isv);
      } else if (s->fsv() != nullptr) {
        FloatSetVal* fsv;
        if (FloatSetVal* fsvc = m.find(s->fsv())) {
          fsv = fsvc;
        } else {
          FloatSetRanges r(s->fsv());
          fsv = FloatSetVal::ai(r);
          m.insert(s->fsv(), fsv);
        }
        c->fsv(fsv);
      } else {
        if (ASTExprVecO<Expression*>* ve = m.find(s->v())) {
          c->v(ASTExprVec<Expression>(ve));
        } else {
          std::vector<Expression*> elems(s->v().size());
          for (unsigned int i = s->v().size(); (i--) != 0U;) {
            elems[i] = copy(env, m, s->v()[i], followIds, copyFundecls, isFlatModel);
          }
          ASTExprVec<Expression> ce(elems);
          m.insert(s->v(), ce);
          c->v(ce);
        }
      }
      c->type(s->type());
      ret = c;
    } break;
    case Expression::E_BOOLLIT: {
      ret = e;
    } break;
    case Expression::E_STRINGLIT: {
      auto* sl = e->cast<StringLit>();
      auto* c = new StringLit(copy_location(m, e), sl->v());
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_ID: {
      if (e == constants().absent) {
        return e;
      }
      Id* id = e->cast<Id>();

      if (followIds) {
        Id* prevId = id;
        Expression* cur = e;
        bool done = false;
        do {
          if (cur == nullptr) {
            cur = prevId;
            done = true;
          } else {
            switch (cur->eid()) {
              case Expression::E_ID:
                prevId = cur->cast<Id>();
                cur = prevId->decl();
                break;
              case Expression::E_VARDECL:
                if (cur->cast<VarDecl>()->e() != nullptr) {
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
        if (!cur->isa<Id>()) {
          return copy(env, m, cur, false);
        }
        Id* curId = cur->cast<Id>();
        if (id->decl() != nullptr) {
          if (Expression* cached = m.find(id->decl())) {
            return cached->cast<VarDecl>()->id();
          }
        }
        return curId;
      }
      Id* c;
      if (id->decl() != nullptr) {
        auto* vd =
            static_cast<VarDecl*>(copy(env, m, id->decl(), followIds, copyFundecls, isFlatModel));
        c = vd->id();
      } else {
        if (id->idn() != -1) {
          c = new Id(copy_location(m, e), id->idn(), nullptr);
        } else {
          c = new Id(copy_location(m, e), id->v(), nullptr);
        }
      }
      m.insert(e, c);
      ret = c;

    } break;
    case Expression::E_ANON: {
      auto* c = new AnonVar(copy_location(m, e));
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_ARRAYLIT: {
      auto* al = e->cast<ArrayLit>();
      std::vector<std::pair<int, int>> dims(al->dims());
      for (unsigned int i = 0; i < dims.size(); i++) {
        dims[i].first = al->min(i);
        dims[i].second = al->max(i);
      }
      if (ArrayLit* sliceView = al->getSliceLiteral()) {
        ASTIntVec dimsInternal = al->dimsInternal();
        unsigned int sliceDims = sliceView->dims();
        unsigned int dimsOffset = al->dims() * 2;
        std::vector<std::pair<int, int>> slice(sliceDims);
        for (unsigned int i = 0; i < sliceDims; i++) {
          slice[i].first = dimsInternal[dimsOffset + i * 2];
          slice[i].second = dimsInternal[dimsOffset + i * 2 + 1];
        }
        auto* c = new ArrayLit(
            copy_location(m, e),
            copy(env, m, sliceView, followIds, copyFundecls, isFlatModel)->cast<ArrayLit>(), dims,
            slice);
        m.insert(e, c);
        ret = c;
      } else {
        auto* c = new ArrayLit(copy_location(m, e), std::vector<Expression*>(), dims);
        m.insert(e, c);

        ASTExprVecO<Expression*>* v;
        if (ASTExprVecO<Expression*>* cv = m.find(al->getVec())) {
          v = cv;
        } else {
          std::vector<Expression*> elems(al->size());
          for (unsigned int i = al->size(); (i--) != 0U;) {
            elems[i] = copy(env, m, (*al)[i], followIds, copyFundecls, isFlatModel);
          }
          ASTExprVec<Expression> ce(elems);
          m.insert(al->getVec(), ce);
          v = ce.vec();
        }
        c->setVec(ASTExprVec<Expression>(v));
        ret = c;
      }
    } break;
    case Expression::E_ARRAYACCESS: {
      auto* aa = e->cast<ArrayAccess>();
      auto* c = new ArrayAccess(copy_location(m, e), nullptr, std::vector<Expression*>());
      m.insert(e, c);

      ASTExprVecO<Expression*>* idx;
      if (ASTExprVecO<Expression*>* cidx = m.find(aa->idx())) {
        idx = cidx;
      } else {
        std::vector<Expression*> elems(aa->idx().size());
        for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
          elems[i] = copy(env, m, aa->idx()[i], followIds, copyFundecls, isFlatModel);
        }
        ASTExprVec<Expression> ce(elems);
        m.insert(aa->idx(), ce);
        idx = ce.vec();
      }
      c->v(copy(env, m, aa->v(), followIds, copyFundecls, isFlatModel));
      c->idx(ASTExprVec<Expression>(idx));
      ret = c;
    } break;
    case Expression::E_COMP: {
      auto* c = e->cast<Comprehension>();
      Generators g;
      auto* cc = new Comprehension(copy_location(m, e), nullptr, g, c->set());
      m.insert(c, cc);

      for (int i = 0; i < c->numberOfGenerators(); i++) {
        std::vector<VarDecl*> vv;
        for (int j = 0; j < c->numberOfDecls(i); j++) {
          vv.push_back(static_cast<VarDecl*>(
              copy(env, m, c->decl(i, j), followIds, copyFundecls, isFlatModel)));
          // Comprehension VarDecl should not be assigned to a particular value when copying the
          // full comprehension
          assert(!c->decl(i, j)->e());
        }
        g.g.emplace_back(vv, copy(env, m, c->in(i), followIds, copyFundecls, isFlatModel),
                         copy(env, m, c->where(i), followIds, copyFundecls, isFlatModel));
      }
      cc->init(copy(env, m, c->e(), followIds, copyFundecls, isFlatModel), g);
      ret = cc;
    } break;
    case Expression::E_ITE: {
      ITE* ite = e->cast<ITE>();
      ITE* c = new ITE(copy_location(m, e), std::vector<Expression*>(), nullptr);
      m.insert(e, c);
      std::vector<Expression*> ifthen(2 * ite->size());
      for (unsigned int i = ite->size(); (i--) != 0U;) {
        ifthen[2 * i] = copy(env, m, ite->ifExpr(i), followIds, copyFundecls, isFlatModel);
        ifthen[2 * i + 1] = copy(env, m, ite->thenExpr(i), followIds, copyFundecls, isFlatModel);
      }
      c->init(ifthen, copy(env, m, ite->elseExpr(), followIds, copyFundecls, isFlatModel));
      ret = c;
    } break;
    case Expression::E_BINOP: {
      auto* b = e->cast<BinOp>();
      auto* c = new BinOp(copy_location(m, e), nullptr, b->op(), nullptr);
      if (b->decl() != nullptr) {
        if (copyFundecls) {
          c->decl(Item::cast<FunctionI>(copy(env, m, b->decl())));
        } else {
          c->decl(b->decl());
        }
      }
      m.insert(e, c);
      c->lhs(copy(env, m, b->lhs(), followIds, copyFundecls, isFlatModel));
      c->rhs(copy(env, m, b->rhs(), followIds, copyFundecls, isFlatModel));
      ret = c;
    } break;
    case Expression::E_UNOP: {
      UnOp* b = e->cast<UnOp>();
      UnOp* c = new UnOp(copy_location(m, e), b->op(), nullptr);
      if (b->decl() != nullptr) {
        if (copyFundecls) {
          c->decl(Item::cast<FunctionI>(copy(env, m, b->decl())));
        } else {
          c->decl(b->decl());
        }
      }
      m.insert(e, c);
      c->e(copy(env, m, b->e(), followIds, copyFundecls, isFlatModel));
      ret = c;
    } break;
    case Expression::E_CALL: {
      Call* ca = e->cast<Call>();
      Call* c = new Call(copy_location(m, e), ca->id(), std::vector<Expression*>());

      if (ca->decl() != nullptr) {
        if (copyFundecls) {
          c->decl(Item::cast<FunctionI>(copy(env, m, ca->decl())));
        } else {
          c->decl(ca->decl());
        }
      }

      m.insert(e, c);
      std::vector<Expression*> args(ca->argCount());
      for (auto i = static_cast<unsigned int>(args.size()); (i--) != 0U;) {
        args[i] = copy(env, m, ca->arg(i), followIds, copyFundecls, isFlatModel);
      }
      c->args(args);
      ret = c;
    } break;
    case Expression::E_VARDECL: {
      auto* vd = e->cast<VarDecl>();
      VarDecl* c;
      if (vd->id()->hasStr()) {
        c = new VarDecl(copy_location(m, e), nullptr, vd->id()->v(), nullptr);
      } else {
        c = new VarDecl(copy_location(m, e), nullptr, vd->id()->idn(), nullptr);
      }
      c->toplevel(vd->toplevel());
      c->introduced(vd->introduced());
      if (isFlatModel && vd->flat() == vd) {
        c->flat(c);
      } else {
        c->flat(vd->flat());
      }
      c->payload(vd->payload());
      m.insert(e, c);
      m.insert(c, c);
      c->ti(static_cast<TypeInst*>(copy(env, m, vd->ti(), followIds, copyFundecls, isFlatModel)));
      c->e(copy(env, m, vd->e(), followIds, copyFundecls, isFlatModel));
      c->type(c->ti()->type());
      c->id()->type(c->type());
      ret = c;
    } break;
    case Expression::E_LET: {
      Let* l = e->cast<Let>();
      std::vector<Expression*> let(l->let().size());
      for (unsigned int i = l->let().size(); (i--) != 0U;) {
        let[i] = copy(env, m, l->let()[i], followIds, copyFundecls, isFlatModel);
      }
      Let* c = new Let(copy_location(m, e), let,
                       copy(env, m, l->in(), followIds, copyFundecls, isFlatModel));
      for (unsigned int i = l->_letOrig.size(); (i--) != 0U;) {
        c->_letOrig[i] = copy(env, m, l->_letOrig[i], followIds, copyFundecls, isFlatModel);
      }

      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_TI: {
      auto* t = e->cast<TypeInst>();
      ASTExprVecO<TypeInst*>* r;
      if (t->ranges().size() == 0) {
        r = nullptr;
      } else if (ASTExprVecO<TypeInst*>* cr = m.find(t->ranges())) {
        r = cr;
      } else {
        std::vector<TypeInst*> rr(t->ranges().size());
        for (unsigned int i = t->ranges().size(); (i--) != 0U;) {
          rr[i] = static_cast<TypeInst*>(
              copy(env, m, t->ranges()[i], followIds, copyFundecls, isFlatModel));
        }
        r = ASTExprVecO<TypeInst*>::a(rr);
      }
      auto* c = new TypeInst(copy_location(m, e), t->type(), ASTExprVec<TypeInst>(r),
                             copy(env, m, t->domain(), followIds, copyFundecls, isFlatModel));
      c->setIsEnum(t->isEnum());
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_TIID: {
      TIId* t = e->cast<TIId>();
      TIId* c = new TIId(copy_location(m, e), t->v());
      m.insert(e, c);
      ret = c;
    } break;
    default:
      assert(false);
  }
  if (!ret->isa<Id>() || ret->cast<Id>()->decl() == nullptr) {
    ret->type(e->type());
  }
  copy_ann(env, m, e->ann(), ret->ann(), followIds, copyFundecls, isFlatModel);
  return ret;
}

void copy_ann(EnvI& env, CopyMap& m, Annotation& oldAnn, Annotation& newAnn, bool followIds,
              bool copyFundecls, bool isFlatModel) {
  for (ExpressionSetIter it = oldAnn.begin(); it != oldAnn.end(); ++it) {
    newAnn.add(copy(env, m, *it, followIds, copyFundecls, isFlatModel));
  }
}

Expression* copy(EnvI& env, Expression* e, bool followIds, bool copyFundecls, bool isFlatModel) {
  CopyMap m;
  return copy(env, m, e, followIds, copyFundecls, isFlatModel);
}

Item* copy(EnvI& env, CopyMap& m, Item* i, bool followIds, bool copyFundecls, bool isFlatModel) {
  if (i == nullptr) {
    return nullptr;
  }
  if (Item* cached = m.find(i)) {
    return cached;
  }
  switch (i->iid()) {
    case Item::II_INC: {
      auto* ii = i->cast<IncludeI>();
      auto* c = new IncludeI(copy_location(m, i), ii->f());
      m.insert(i, c);
      c->m(copy(env, m, ii->m()), ii->own());
      return c;
    }
    case Item::II_VD: {
      auto* v = i->cast<VarDeclI>();
      auto* c = new VarDeclI(copy_location(m, i), nullptr);
      m.insert(i, c);
      c->e(static_cast<VarDecl*>(copy(env, m, v->e(), followIds, copyFundecls, isFlatModel)));
      return c;
    }
    case Item::II_ASN: {
      auto* a = i->cast<AssignI>();
      auto* c = new AssignI(copy_location(m, i), a->id(), nullptr);
      m.insert(i, c);
      c->e(copy(env, m, a->e(), followIds, copyFundecls, isFlatModel));
      c->decl(static_cast<VarDecl*>(copy(env, m, a->decl(), followIds, copyFundecls, isFlatModel)));
      return c;
    }
    case Item::II_CON: {
      auto* cc = i->cast<ConstraintI>();
      auto* c = new ConstraintI(copy_location(m, i), nullptr);
      m.insert(i, c);
      c->e(copy(env, m, cc->e(), followIds, copyFundecls, isFlatModel));
      return c;
    }
    case Item::II_SOL: {
      auto* s = i->cast<SolveI>();
      SolveI* c;
      switch (s->st()) {
        case SolveI::ST_SAT:
          c = SolveI::sat(Location());
          break;
        case SolveI::ST_MIN:
          c = SolveI::min(Location(), copy(env, m, s->e(), followIds, copyFundecls, isFlatModel));
          break;
        case SolveI::ST_MAX:
          c = SolveI::max(Location(), copy(env, m, s->e(), followIds, copyFundecls, isFlatModel));
          break;
      }
      copy_ann(env, m, s->ann(), c->ann(), followIds, copyFundecls, isFlatModel);
      m.insert(i, c);
      return c;
    }
    case Item::II_OUT: {
      auto* o = i->cast<OutputI>();
      auto* c = new OutputI(copy_location(m, i),
                            copy(env, m, o->e(), followIds, copyFundecls, isFlatModel));
      m.insert(i, c);
      return c;
    }
    case Item::II_FUN: {
      auto* f = i->cast<FunctionI>();
      std::vector<VarDecl*> params(f->params().size());
      for (unsigned int j = f->params().size(); (j--) != 0U;) {
        params[j] = static_cast<VarDecl*>(
            copy(env, m, f->params()[j], followIds, copyFundecls, isFlatModel));
      }
      auto* c = new FunctionI(
          copy_location(m, i), f->id(),
          static_cast<TypeInst*>(copy(env, m, f->ti(), followIds, copyFundecls, isFlatModel)),
          params, copy(env, m, f->e(), followIds, copyFundecls, isFlatModel));
      c->builtins.e = f->builtins.e;
      c->builtins.i = f->builtins.i;
      c->builtins.f = f->builtins.f;
      c->builtins.b = f->builtins.b;
      c->builtins.s = f->builtins.s;
      c->builtins.str = f->builtins.str;

      copy_ann(env, m, f->ann(), c->ann(), followIds, copyFundecls, isFlatModel);
      m.insert(i, c);
      return c;
    }
    default:
      assert(false);
      return nullptr;
  }
}

Item* copy(EnvI& env, Item* i, bool followIds, bool copyFundecls, bool isFlatModel) {
  CopyMap m;
  return copy(env, m, i, followIds, copyFundecls, isFlatModel);
}

Model* copy(EnvI& env, CopyMap& cm, Model* m, bool isFlatModel) {
  if (m == nullptr) {
    return nullptr;
  }
  if (Model* cached = cm.find(m)) {
    return cached;
  }
  auto* c = new Model;
  for (auto& i : *m) {
    c->addItem(copy(env, cm, i, false, true));
  }

  for (auto& it : m->_fnmap) {
    for (auto& i : it.second) {
      c->registerFn(env, copy(env, cm, i.fi, false, true, isFlatModel)->cast<FunctionI>());
    }
  }
  cm.insert(m, c);
  return c;
}
Model* copy(EnvI& env, Model* m) {
  CopyMap cm;
  return copy(env, cm, m);
}

}  // namespace MiniZinc
