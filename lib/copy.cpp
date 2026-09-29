/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/copy.hh>
#include <minizinc/hash.hh>

namespace MiniZinc {

void CopyMap::insert(Expression* e0, Expression* e1) {
  if (!Expression::isUnboxedVal(e0) && !Expression::isUnboxedVal(e1)) {
    insertNode(e0, e1);
  }
}
Expression* CopyMap::find(Expression* e) { return static_cast<Expression*>(findNode(_nodeMap, e)); }
Expression* CopyMap::findOrig(Expression* e) {
  return static_cast<Expression*>(findNode(_revNodeMap, e));
}
void CopyMap::insert(Item* e0, Item* e1) { insertNode(e0, e1); }
Item* CopyMap::find(Item* e) { return static_cast<Item*>(findNode(_nodeMap, e)); }
Item* CopyMap::findOrig(Item* e) { return static_cast<Item*>(findNode(_revNodeMap, e)); }
void CopyMap::insert(Model* e0, Model* e1) { _modelMap.insert(std::make_pair(e0, e1)); }
Model* CopyMap::find(Model* e) {
  auto it = _modelMap.find(e);
  if (it == _modelMap.end()) {
    return nullptr;
  }
  return it->second;
}
void CopyMap::insert(IntSetVal* e0, IntSetVal* e1) { insertNode(e0, e1); }
IntSetVal* CopyMap::find(IntSetVal* e) { return static_cast<IntSetVal*>(findNode(_nodeMap, e)); }
IntSetVal* CopyMap::findOrig(IntSetVal* e) {
  return static_cast<IntSetVal*>(findNode(_revNodeMap, e));
}
void CopyMap::insert(FloatSetVal* e0, FloatSetVal* e1) { insertNode(e0, e1); }
FloatSetVal* CopyMap::find(FloatSetVal* e) {
  return static_cast<FloatSetVal*>(findNode(_nodeMap, e));
}
FloatSetVal* CopyMap::findOrig(FloatSetVal* e) {
  return static_cast<FloatSetVal*>(findNode(_revNodeMap, e));
}

Location copy_location(CopyMap& m, const Location& _loc) { return _loc; }
Location copy_location(CopyMap& m, Expression* e) { return copy_location(m, Expression::loc(e)); }
Location copy_location(CopyMap& m, Item* i) { return copy_location(m, i->loc()); }

void copy_ann(EnvI& env, CopyMap& m, Annotation& oldAnn, Annotation& newAnn, bool followIds,
              bool copyFundecls, bool isFlatModel);

Ref<Expression> copy(EnvI& env, CopyMap& m, Expression* e, bool followIds, bool copyFundecls,
                     bool isFlatModel) {
  if (e == nullptr) {
    return nullptr;
  }
  if (Expression* cached = m.find(e)) {
    if (!Expression::isa<VarDecl>(cached) ||
        !VarDeclI::a(Location(), Expression::cast<VarDecl>(cached))->removed()) {
      return cached;
    }
  }
  Ref<Expression> ret;
  switch (Expression::eid(e)) {
    case Expression::E_INTLIT: {
      Ref<IntLit> c = IntLit::a(IntLit::v(Expression::cast<IntLit>(e)));
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_FLOATLIT: {
      Ref<FloatLit> c = FloatLit::a(FloatLit::v(Expression::cast<FloatLit>(e)));
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_SETLIT: {
      auto* s = Expression::cast<SetLit>(e);
      auto c = make<SetLit>(copy_location(m, e), static_cast<IntSetVal*>(nullptr));
      m.insert(e, c);
      if (s->isv() != nullptr) {
        Ref<IntSetVal> isv;
        if (IntSetVal* isvc = m.find(s->isv())) {
          isv = isvc;
        } else {
          IntSetRanges r(s->isv());
          isv = IntSetVal::ai(r);
          m.insert(s->isv(), isv);
        }
        c->isv(isv);
      } else if (s->fsv() != nullptr) {
        Ref<FloatSetVal> fsv;
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
          std::vector<Ref<Expression>> elems(s->v().size());
          for (unsigned int i = s->v().size(); (i--) != 0U;) {
            elems[i] = copy(env, m, s->v()[i], followIds, copyFundecls, isFlatModel);
          }
          ASTExprVec<Expression> ce(raw(elems));
          m.insert(s->v(), ce);
          c->v(ce);
        }
      }
      Expression::type(c, s->type());
      ret = c;
    } break;
    case Expression::E_BOOLLIT: {
      ret = e;
    } break;
    case Expression::E_STRINGLIT: {
      auto* sl = Expression::cast<StringLit>(e);
      auto c = make<StringLit>(copy_location(m, e), sl->v());
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_ID: {
      if (e == Constants::constants().absent) {
        return e;
      }
      Id* id = Expression::cast<Id>(e);

      if (followIds) {
        Id* prevId = id;
        Expression* cur = e;
        bool done = false;
        do {
          if (cur == nullptr) {
            cur = prevId;
            done = true;
          } else {
            switch (Expression::eid(cur)) {
              case Expression::E_ID:
                prevId = Expression::cast<Id>(cur);
                cur = prevId->decl();
                break;
              case Expression::E_VARDECL:
                if (Expression::cast<VarDecl>(cur)->e() != nullptr) {
                  cur = Expression::cast<VarDecl>(cur)->e();
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
        if (!Expression::isa<Id>(cur)) {
          return copy(env, m, cur, false);
        }
        Id* curId = Expression::cast<Id>(cur);
        if (id->decl() != nullptr) {
          if (Expression* cached = m.find(id->decl())) {
            return Expression::cast<VarDecl>(cached)->id();
          }
        }
        return curId;
      }
      Ref<Id> c;
      if (id->decl() != nullptr) {
        auto vd = copy(env, m, id->decl(), followIds, copyFundecls, isFlatModel).cast<VarDecl>();
        c = vd->id();
      } else {
        if (id->idn() != -1) {
          c = make<Id>(copy_location(m, e), id->idn(), nullptr);
        } else {
          c = make<Id>(copy_location(m, e), id->v(), nullptr);
        }
      }
      m.insert(e, c);
      ret = c;

    } break;
    case Expression::E_ANON: {
      auto c = make<AnonVar>(copy_location(m, e));
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
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
        auto sliceCopy =
            copy(env, m, sliceView, followIds, copyFundecls, isFlatModel).cast<ArrayLit>();
        auto c = make<ArrayLit>(copy_location(m, e), sliceCopy, dims, slice);
        m.insert(e, c);
        ret = c;
      } else {
        Ref<ArrayLit> c;
        if (al->isTuple()) {
          c = ArrayLit::constructTuple(copy_location(m, e), std::vector<Expression*>());
        } else {
          c = make<ArrayLit>(copy_location(m, e), std::vector<Expression*>(), dims);
        }
        m.insert(e, c);

        ASTExprVecO<Expression*>* v;
        if (ASTExprVecO<Expression*>* cv = m.find(al->getVec())) {
          v = cv;
        } else {
          std::vector<Ref<Expression>> elems(al->size());
          for (unsigned int i = al->size(); (i--) != 0U;) {
            elems[i] = copy(env, m, (*al)[i], followIds, copyFundecls, isFlatModel);
          }
          ASTExprVec<Expression> ce(raw(elems));
          m.insert(al->getVec(), ce);
          v = ce.vec();
        }
        c->setVec(ASTExprVec<Expression>(v));
        c->flat(al->flat());
        ret = c;
      }
    } break;
    case Expression::E_ARRAYACCESS: {
      auto* aa = Expression::cast<ArrayAccess>(e);
      auto c = make<ArrayAccess>(copy_location(m, e), nullptr, std::vector<Expression*>());
      m.insert(e, c);

      ASTExprVecO<Expression*>* idx;
      if (ASTExprVecO<Expression*>* cidx = m.find(aa->idx())) {
        idx = cidx;
      } else {
        std::vector<Ref<Expression>> elems(aa->idx().size());
        for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
          elems[i] = copy(env, m, aa->idx()[i], followIds, copyFundecls, isFlatModel);
        }
        ASTExprVec<Expression> ce(raw(elems));
        m.insert(aa->idx(), ce);
        idx = ce.vec();
      }
      c->v(copy(env, m, aa->v(), followIds, copyFundecls, isFlatModel));
      c->idx(ASTExprVec<Expression>(idx));
      ret = c;
    } break;
    case Expression::E_FIELDACCESS: {
      auto* fa = Expression::cast<FieldAccess>(e);
      auto c = make<FieldAccess>(copy_location(m, e),
                                 copy(env, m, fa->v(), followIds, copyFundecls, isFlatModel),
                                 copy(env, m, fa->field(), followIds, copyFundecls, isFlatModel));
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_COMP: {
      auto* c = Expression::cast<Comprehension>(e);
      Generators g;
      auto cc = make<Comprehension>(copy_location(m, e), nullptr, g, c->set());
      m.insert(c, cc);

      for (unsigned int i = 0; i < c->numberOfGenerators(); i++) {
        std::vector<Ref<VarDecl>> vv;
        for (unsigned int j = 0; j < c->numberOfDecls(i); j++) {
          vv.push_back(
              copy(env, m, c->decl(i, j), followIds, copyFundecls, isFlatModel).cast<VarDecl>());
          // Comprehension VarDecl should not be assigned to a particular value when copying the
          // full comprehension
          assert(!c->decl(i, j)->e());
        }
        auto in = copy(env, m, c->in(i), followIds, copyFundecls, isFlatModel);
        auto where = copy(env, m, c->where(i), followIds, copyFundecls, isFlatModel);
        g.g.emplace_back(raw(vv), in, where);
      }
      auto body = copy(env, m, c->e(), followIds, copyFundecls, isFlatModel);
      RC::replaceChildren(cc.get(), [&] { cc->init(body, g); });
      ret = cc;
    } break;
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      Ref<ITE> c = make<ITE>(copy_location(m, e), std::vector<Expression*>(), nullptr);
      m.insert(e, c);
      std::vector<Ref<Expression>> ifthen(2 * static_cast<size_t>(ite->size()));
      for (unsigned int i = ite->size(); (i--) != 0U;) {
        ifthen[2 * static_cast<size_t>(i)] =
            copy(env, m, ite->ifExpr(i), followIds, copyFundecls, isFlatModel);
        ifthen[2 * static_cast<size_t>(i) + 1] =
            copy(env, m, ite->thenExpr(i), followIds, copyFundecls, isFlatModel);
      }
      auto elseCopy = copy(env, m, ite->elseExpr(), followIds, copyFundecls, isFlatModel);
      RC::replaceChildren(c.get(), [&] { c->init(raw(ifthen), elseCopy); });
      ret = c;
    } break;
    case Expression::E_BINOP: {
      auto* b = Expression::cast<BinOp>(e);
      auto c = make<BinOp>(copy_location(m, e), nullptr, b->op(), nullptr);
      if (b->decl() != nullptr) {
        if (copyFundecls) {
          c->decl(copy(env, m, b->decl()).cast<FunctionI>());
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
      UnOp* b = Expression::cast<UnOp>(e);
      Ref<UnOp> c = make<UnOp>(copy_location(m, e), b->op(), nullptr);
      if (b->decl() != nullptr) {
        if (copyFundecls) {
          c->decl(copy(env, m, b->decl()).cast<FunctionI>());
        } else {
          c->decl(b->decl());
        }
      }
      m.insert(e, c);
      c->e(copy(env, m, b->e(), followIds, copyFundecls, isFlatModel));
      ret = c;
    } break;
    case Expression::E_CALL: {
      Call* ca = Expression::cast<Call>(e);
      std::vector<Expression*> emptyArgs(ca->argCount(), nullptr);
      Ref<Call> c = Call::a(copy_location(m, e), ca->id(), emptyArgs);

      if (ca->decl() != nullptr) {
        if (copyFundecls) {
          c->decl(copy(env, m, ca->decl(), followIds, copyFundecls, isFlatModel).cast<FunctionI>());
        } else {
          c->decl(ca->decl());
        }
      }

      m.insert(e, c);
      for (auto i = c->argCount(); (i--) != 0U;) {
        c->arg(i, copy(env, m, ca->arg(i), followIds, copyFundecls, isFlatModel));
      }
      ret = c;
    } break;
    case Expression::E_VARDECL: {
      auto* vd = Expression::cast<VarDecl>(e);
      Ref<VarDecl> c;
      if (vd->id()->hasStr()) {
        c = make<VarDecl>(copy_location(m, e), nullptr, vd->id()->v());
      } else {
        c = make<VarDecl>(copy_location(m, e), nullptr, vd->id()->idn());
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
      c->ti(copy(env, m, vd->ti(), followIds, copyFundecls, isFlatModel).cast<TypeInst>());
      c->e(copy(env, m, vd->e(), followIds, copyFundecls, isFlatModel));
      if (c->ti() != nullptr) {
        Expression::type(c, c->ti()->type());
      }
      Expression::type(c->id(), c->type());
      ret = c;
    } break;
    case Expression::E_LET: {
      Let* l = Expression::cast<Let>(e);
      std::vector<Ref<Expression>> let(l->let().size());
      for (unsigned int i = l->let().size(); (i--) != 0U;) {
        let[i] = copy(env, m, l->let()[i], followIds, copyFundecls, isFlatModel);
      }
      Ref<Let> c = make<Let>(copy_location(m, e), let,
                             copy(env, m, l->in(), followIds, copyFundecls, isFlatModel));
      for (unsigned int i = l->_letOrig.size(); (i--) != 0U;) {
        c->_letOrig[i] = copy(env, m, l->_letOrig[i], followIds, copyFundecls, isFlatModel);
      }

      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_TI: {
      auto* t = Expression::cast<TypeInst>(e);
      ASTExprVec<TypeInst> r;
      if (t->ranges().empty()) {
        r = nullptr;
      } else if (ASTExprVecO<TypeInst*>* cr = m.find(t->ranges())) {
        r = cr;
      } else {
        std::vector<Ref<TypeInst>> rr(t->ranges().size());
        for (unsigned int i = t->ranges().size(); (i--) != 0U;) {
          rr[i] =
              copy(env, m, t->ranges()[i], followIds, copyFundecls, isFlatModel).cast<TypeInst>();
        }
        r = ASTExprVec<TypeInst>(raw(rr));
      }
      auto c = make<TypeInst>(copy_location(m, e), t->type(), r,
                              copy(env, m, t->domain(), followIds, copyFundecls, isFlatModel));
      c->setIsEnum(t->isEnum());
      m.insert(e, c);
      ret = c;
    } break;
    case Expression::E_TIID: {
      TIId* t = Expression::cast<TIId>(e);
      Ref<TIId> c = make<TIId>(copy_location(m, e), t->v());
      m.insert(e, c);
      ret = c;
    } break;
    default:
      assert(false);
  }
  if (!Expression::isa<Id>(ret) || Expression::cast<Id>(ret)->decl() == nullptr) {
    Expression::type(ret, Expression::type(e));
  }
  copy_ann(env, m, Expression::ann(e), Expression::ann(ret), followIds, copyFundecls, isFlatModel);
  return ret;
}

void copy_ann(EnvI& env, CopyMap& m, Annotation& oldAnn, Annotation& newAnn, bool followIds,
              bool copyFundecls, bool isFlatModel) {
  for (ExpressionSetIter it = oldAnn.begin(); it != oldAnn.end(); ++it) {
    newAnn.add(copy(env, m, *it, followIds, copyFundecls, isFlatModel));
  }
}

Ref<Expression> copy(EnvI& env, Expression* e, bool followIds, bool copyFundecls,
                     bool isFlatModel) {
  CopyMap m;
  return copy(env, m, e, followIds, copyFundecls, isFlatModel);
}

Ref<Item> copy(EnvI& env, CopyMap& m, Item* i, bool followIds, bool copyFundecls,
               bool isFlatModel) {
  if (i == nullptr) {
    return nullptr;
  }
  if (Item* cached = m.find(i)) {
    return cached;
  }
  switch (i->iid()) {
    case Item::II_INC: {
      auto* ii = i->cast<IncludeI>();
      auto c = make<IncludeI>(copy_location(m, i), ii->f());
      m.insert(i, c);
      c->m(copy(env, m, ii->m()), ii->own());
      return c;
    }
    case Item::II_VD: {
      auto* v = i->cast<VarDeclI>();
      auto vc = copy(env, m, v->e(), followIds, copyFundecls, isFlatModel).cast<VarDecl>();
      return VarDeclI::a(Location(), vc);
    }
    case Item::II_ASN: {
      auto* a = i->cast<AssignI>();
      auto c = make<AssignI>(copy_location(m, i), a->id(), nullptr);
      m.insert(i, c);
      c->e(copy(env, m, a->e(), followIds, copyFundecls, isFlatModel));
      c->decl(copy(env, m, a->decl(), followIds, copyFundecls, isFlatModel).cast<VarDecl>());
      return c;
    }
    case Item::II_CON: {
      auto* cc = i->cast<ConstraintI>();
      auto c = make<ConstraintI>(copy_location(m, i), nullptr);
      m.insert(i, c);
      c->e(copy(env, m, cc->e(), followIds, copyFundecls, isFlatModel));
      return c;
    }
    case Item::II_SOL: {
      auto* s = i->cast<SolveI>();
      Ref<SolveI> c;
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
      auto c = make<OutputI>(copy_location(m, i),
                             copy(env, m, o->e(), followIds, copyFundecls, isFlatModel));
      copy_ann(env, m, o->ann(), c->ann(), followIds, copyFundecls, isFlatModel);
      m.insert(i, c);
      return c;
    }
    case Item::II_FUN: {
      auto* f = i->cast<FunctionI>();
      auto c = make<FunctionI>(copy_location(m, i), f->id(), nullptr, std::vector<VarDecl*>(),
                               nullptr, f->fromStdLib(), f->capturedAnnotationsVar() != nullptr);
      m.insert(i, c);
      std::vector<Ref<VarDecl>> params(f->paramCount());
      for (unsigned int j = f->paramCount(); (j--) != 0U;) {
        params[j] = copy(env, m, f->param(j), followIds, copyFundecls, isFlatModel).cast<VarDecl>();
      }
      if (f->capturedAnnotationsVar() != nullptr) {
        params.push_back(
            copy(env, m, f->capturedAnnotationsVar(), followIds, copyFundecls, isFlatModel)
                .cast<VarDecl>());
      }
      c->init(raw(params));
      c->ti(copy(env, m, f->ti(), followIds, copyFundecls, isFlatModel).cast<TypeInst>());
      c->e(copy(env, m, f->e(), followIds, copyFundecls, isFlatModel));
      c->builtins.e = f->builtins.e;
      c->builtins.i = f->builtins.i;
      c->builtins.f = f->builtins.f;
      c->builtins.b = f->builtins.b;
      c->builtins.s = f->builtins.s;
      c->builtins.fs = f->builtins.fs;
      c->builtins.str = f->builtins.str;
      c->isMonomorphised(f->isMonomorphised());

      copy_ann(env, m, f->ann(), c->ann(), followIds, copyFundecls, isFlatModel);
      return c;
    }
    default:
      assert(false);
      return nullptr;
  }
}

Ref<Item> copy(EnvI& env, Item* i, bool followIds, bool copyFundecls, bool isFlatModel) {
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
