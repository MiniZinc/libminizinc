/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/chain_compressor.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

void ChainCompressor::removeItem(Item* i) {
  CollectDecls cd(_env.varOccurrences, _deletedVarDecls, i);
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    top_down(cd, ci->e());
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    top_down(cd, vdi->e());
  } else {
    assert(false);  // CURRENTLY NOT SUPPORTED
  }
  i->remove();
}

int ChainCompressor::addItem(Item* i) {
  _env.flatAddItem(i);
  int item_idx = static_cast<int>(_env.flat()->size()) - 1;
  trackItem(i);
  return item_idx;
}

void ChainCompressor::updateCount() {
  for (auto it = _items.begin(); it != _items.end();) {
    if (it->second->removed()) {
      it = _items.erase(it);
    } else {
      ++it;
    }
  }
}

void ChainCompressor::replaceCallArgument(Item* i, Call* c, unsigned int n, Expression* e) {
  CollectDecls cd(_env.varOccurrences, _deletedVarDecls, i);
  top_down(cd, c->arg(n));
  c->arg(n, e);
  CollectOccurrencesE ce(_env.varOccurrences, i);
  top_down(ce, e);
}

bool ImpCompressor::trackItem(Item* i) {
  if (i->removed()) {
    return false;
  }
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    if (auto* c = ci->e()->dynamicCast<Call>()) {
      // clause([y], [x]); i.e. x -> y
      if (c->id() == constants().ids.clause) {
        auto* positive = c->arg(0)->cast<ArrayLit>();
        auto* negative = c->arg(1)->cast<ArrayLit>();
        if (positive->length() == 1 && negative->length() == 1) {
          auto* var = (*negative)[0]->dynamicCast<Id>();
          if (var != nullptr) {
            storeItem(var->decl(), i);
          }
          return true;
        }
      } else if (c->id() == "mzn_reverse_map_var") {
        auto* control = c->arg(0)->cast<Id>();
        assert(control->type().isvarbool());
        storeItem(control->decl(), i);
        return true;
        // pred_imp(..., b); i.e. b -> pred(...)
      } else if (c->id().endsWith("_imp")) {
        auto* control = c->arg(c->argCount() - 1)->dynamicCast<Id>();
        if (control != nullptr) {
          assert(control->type().isvarbool());
          storeItem(control->decl(), i);
        }
        return true;
      }
    }
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    if (vdi->e()->type().isvarbool() && (vdi->e() != nullptr) && (vdi->e()->e() != nullptr)) {
      if (auto* c = vdi->e()->e()->dynamicCast<Call>()) {
        // x = forall([y,z,...]); potentially: x -> (y /\ z /\ ...)
        if (c->id() == constants().ids.forall) {
          storeItem(vdi->e(), i);
          return true;
          // x ::ctx_pos = pred(...); potentially: pred_imp(..., x); i.e. x -> pred(...)
        }
        if (_env.fopts.enableHalfReification && vdi->e()->ann().contains(constants().ctx.pos)) {
          GCLock lock;
          auto cid = EnvI::halfReifyId(c->id());
          std::vector<Type> args;
          args.reserve(c->argCount() + 1);
          for (int j = 0; j < c->argCount(); ++j) {
            args.push_back(c->arg(j)->type());
          }
          args.push_back(Type::varbool());
          FunctionI* decl = _env.model->matchFn(_env, cid, args, false);

          if (decl != nullptr) {
            storeItem(vdi->e(), i);
            return true;
          }
        }
      }
    }
  }
  return false;
}

void ImpCompressor::compress() {
  for (auto it = _items.begin(); it != _items.end();) {
    VarDecl* lhs = nullptr;
    VarDecl* rhs = nullptr;
    // Check if compression is possible
    if (auto* ci = it->second->dynamicCast<ConstraintI>()) {
      auto* c = ci->e()->cast<Call>();
      if (c->id() == constants().ids.clause) {
        auto* positive = c->arg(0)->cast<ArrayLit>();
        auto* var = (*positive)[0]->dynamicCast<Id>();
        if (var != nullptr) {
          bool output_var = var->decl()->ann().contains(constants().ann.output_var);
          auto usages = _env.varOccurrences.usages(var->decl());
          output_var = output_var || usages.second;
          int occurrences = usages.first;
          unsigned long lhs_occurences = count(var->decl());

          // Compress if:
          // - There is one occurrence on the RHS of a clause and the others are on the LHS of a
          // clause
          // - There is one occurrence on the RHS of a clause, that Id is a reified forall that has
          // no other occurrences
          // - There is one occurrence on the RHS of a clause, that Id is a reification in a
          // positive context, and all other occurrences are on the LHS of a clause
          bool compress = !output_var && lhs_occurences > 0;
          if ((var->decl()->e() != nullptr) && (var->decl()->e()->dynamicCast<Call>() != nullptr)) {
            auto* call = var->decl()->e()->cast<Call>();
            if (call->id() == constants().ids.forall) {
              compress = compress && (occurrences == 1 && lhs_occurences == 1);
            } else {
              compress = compress && (occurrences == lhs_occurences);
            }
          } else {
            compress = compress && (occurrences == lhs_occurences + 1);
          }
          if (compress) {
            rhs = var->decl();
            auto* negative = c->arg(1)->cast<ArrayLit>();
            lhs = (*negative)[0]->isa<Id>() ? (*negative)[0]->cast<Id>()->decl() : nullptr;
            if (lhs == rhs) {
              continue;
            }
          }
          // TODO: Detect equivalences for output variables.
        }
      }
    }

    if ((lhs != nullptr) && (rhs != nullptr)) {
      assert(count(rhs) > 0);

      auto range = find(rhs);
      std::vector<Item*> to_process;
      for (auto match = range.first; match != range.second; ++match) {
        to_process.push_back(match->second);
      }
      _items.erase(range.first, range.second);
      for (auto* item : to_process) {
        bool success = compressItem(item, lhs);
        assert(success);
        _env.counters.impDel++;
      }

      assert(!rhs->ann().contains(constants().ann.output_var));
      removeItem(it->second);
      it = _items.erase(it);
    } else {
      ++it;
    }
  }
}

bool ImpCompressor::compressItem(Item* i, VarDecl* newLHS) {
  GCLock lock;
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    auto* c = ci->e()->cast<Call>();
    // Given (x -> y) /\ (y -> z), produce x -> z
    if (c->id() == constants().ids.clause) {
      auto* positive = c->arg(0)->cast<ArrayLit>();
      VarDecl* positiveDecl =
          (*positive)[0]->isa<Id>() ? (*positive)[0]->cast<Id>()->decl() : nullptr;
      if (positiveDecl != newLHS) {
        ConstraintI* nci = constructClause(positive, newLHS->id());
        _boolConstraints.push_back(addItem(nci));
      }
      removeItem(i);
      return true;
      // Given (x -> y) /\ (y -> pred(...)), produce x -> pred(...)
    }
    if (c->id() == "mzn_reverse_map_var") {
      return true;
    }
    if (c->id().endsWith("_imp")) {
      replaceCallArgument(i, c, c->argCount() - 1, newLHS->id());
      trackItem(i);
      return true;
    }
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    auto* c = vdi->e()->e()->dynamicCast<Call>();
    // Given: (x -> y) /\  (y -> (a /\ b /\ ...)), produce (x -> a) /\ (x -> b) /\ ...
    if (c->id() == constants().ids.forall) {
      auto* exprs = c->arg(0)->cast<ArrayLit>();
      for (int j = 0; j < exprs->size(); ++j) {
        VarDecl* rhsDecl = (*exprs)[j]->isa<Id>() ? (*exprs)[j]->cast<Id>()->decl() : nullptr;
        if (rhsDecl != newLHS) {
          ConstraintI* nci = constructClause((*exprs)[j], newLHS->id());
          _boolConstraints.push_back(addItem(nci));
        }
      }
      return true;
      // x ::ctx_pos = pred(...); potentially: pred_imp(..., x); i.e. x -> pred(...)
    }
    if (vdi->e()->ann().contains(constants().ctx.pos)) {
      ConstraintI* nci = constructHalfReif(c, newLHS->id());
      assert(nci);
      addItem(nci);
      return true;
    }
  }
  return false;
}

ConstraintI* ImpCompressor::constructClause(Expression* pos, Expression* neg) {
  assert(GC::locked());
  std::vector<Expression*> args(2);
  if (pos->dynamicCast<ArrayLit>() != nullptr) {
    args[0] = pos;
  } else {
    assert(neg->type().isbool());
    std::vector<Expression*> eVec(1);
    eVec[0] = pos;
    args[0] = new ArrayLit(pos->loc().introduce(), eVec);
    args[0]->type(Type::varbool(1));
  }
  if (neg->dynamicCast<ArrayLit>() != nullptr) {
    args[1] = neg;
  } else {
    assert(neg->type().isbool());
    std::vector<Expression*> eVec(1);
    eVec[0] = neg;
    args[1] = new ArrayLit(neg->loc().introduce(), eVec);
    args[1]->type(Type::varbool(1));
  }
  // NEVER CREATE (a -> a)
  assert(!(*args[0]->cast<ArrayLit>())[0]->isa<Id>() ||
         !(*args[1]->cast<ArrayLit>())[0]->isa<Id>() ||
         (*args[0]->cast<ArrayLit>())[0]->cast<Id>()->decl() !=
             (*args[1]->cast<ArrayLit>())[0]->cast<Id>()->decl());
  auto* nc = new Call(MiniZinc::Location().introduce(), constants().ids.clause, args);
  nc->type(Type::varbool());
  nc->decl(_env.model->matchFn(_env, nc, false));
  assert(nc->decl());

  return new ConstraintI(MiniZinc::Location().introduce(), nc);
}

ConstraintI* ImpCompressor::constructHalfReif(Call* call, Id* control) {
  assert(_env.fopts.enableHalfReification);
  assert(GC::locked());
  auto cid = EnvI::halfReifyId(call->id());
  std::vector<Expression*> args(call->argCount());
  for (int i = 0; i < call->argCount(); ++i) {
    args[i] = call->arg(i);
  }
  args.push_back(control);
  FunctionI* decl = _env.model->matchFn(_env, cid, args, false);
  if (decl != nullptr) {
    auto* nc = new Call(call->loc().introduce(), cid, args);
    nc->decl(decl);
    nc->type(Type::varbool());
    return new ConstraintI(call->loc().introduce(), nc);
  }
  return nullptr;
}

bool LECompressor::trackItem(Item* i) {
  if (i->removed()) {
    return false;
  }
  bool added = false;
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    if (auto* call = ci->e()->dynamicCast<Call>()) {
      // {int,float}_lin_le([c1,c2,...], [x, y,...], 0);
      if (call->id() == constants().ids.int_.lin_le ||
          call->id() == constants().ids.float_.lin_le) {
        auto* as = follow_id(call->arg(0))->cast<ArrayLit>();
        auto* bs = follow_id(call->arg(1))->cast<ArrayLit>();
        assert(as->size() == bs->size());

        for (int j = 0; j < as->size(); ++j) {
          if (as->type().isIntArray()) {
            if (follow_id((*as)[j])->cast<IntLit>()->v() > IntVal(0)) {
              // Check if left hand side is a variable (could be constant)
              if (auto* decl = follow_id_to_decl((*bs)[j])->dynamicCast<VarDecl>()) {
                storeItem(decl, i);
                added = true;
              }
            }
          } else {
            if (follow_id((*as)[j])->cast<FloatLit>()->v() > FloatVal(0)) {
              // Check if left hand side is a variable (could be constant)
              if (auto* decl = follow_id_to_decl((*bs)[j])->dynamicCast<VarDecl>()) {
                storeItem(decl, i);
                added = true;
              }
            }
          }
        }
      }
      assert(call->id() != constants().ids.int2float);
    }
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    assert(vdi->e());
    if (Expression* vde = vdi->e()->e()) {
      if (auto* call = vde->dynamicCast<Call>()) {
        if (call->id() == constants().ids.int2float) {
          if (auto* vd = follow_id_to_decl(call->arg(0))->dynamicCast<VarDecl>()) {
            auto* alias = follow_id_to_decl(vdi->e())->cast<VarDecl>();
            _aliasMap[vd] = alias;
          }
        }
      }
    }
  }
  return added;
}

void LECompressor::compress() {
  for (auto it = _items.begin(); it != _items.end();) {
    VarDecl* lhs = nullptr;
    VarDecl* rhs = nullptr;
    VarDecl* alias = nullptr;

    // Check if compression is possible
    if (auto* ci = it->second->dynamicCast<ConstraintI>()) {
      auto* call = ci->e()->cast<Call>();
      if (call->id() == constants().ids.int_.lin_le) {
        auto* as = follow_id(call->arg(0))->cast<ArrayLit>();
        auto* bs = follow_id(call->arg(1))->cast<ArrayLit>();
        auto* c = follow_id(call->arg(2))->cast<IntLit>();

        if (bs->size() == 2 && c->v() == IntVal(0)) {
          auto a0 = follow_id((*as)[0])->cast<IntLit>()->v();
          auto a1 = follow_id((*as)[1])->cast<IntLit>()->v();
          if (a0 == -a1 && eqBounds((*bs)[0], (*bs)[1])) {
            int i = a0 < a1 ? 0 : 1;
            if (!(*bs)[i]->isa<Id>()) {
              break;
            }
            auto* neg = follow_id_to_decl((*bs)[i])->cast<VarDecl>();
            bool output_var = neg->ann().contains(constants().ann.output_var);

            auto usages = _env.varOccurrences.usages(neg);
            int occurrences = usages.first;
            output_var = output_var || usages.second;
            unsigned long lhs_occurences = count(neg);
            bool compress = !output_var;
            auto search = _aliasMap.find(neg);

            if (search != _aliasMap.end()) {
              alias = search->second;
              auto alias_usages = _env.varOccurrences.usages(alias);
              int alias_occ = alias_usages.first;
              compress = compress && (!alias_usages.second);
              unsigned long alias_lhs_occ = count(alias);
              // neg is only allowed to occur:
              // - once in the "implication"
              // - once in the aliasing
              // - on a lhs of other expressions
              // alias is only allowed to occur on a lhs of an expression.
              compress = compress && (lhs_occurences + alias_lhs_occ > 0) &&
                         (occurrences == lhs_occurences + 2) && (alias_occ == alias_lhs_occ);
            } else {
              // neg is only allowed to occur:
              // - once in the "implication"
              // - on a lhs of other expressions
              compress = compress && (lhs_occurences > 0) && (occurrences == lhs_occurences + 1);
            }

            auto* pos = follow_id_to_decl((*bs)[1 - i])->dynamicCast<VarDecl>();
            if ((pos != nullptr) && compress) {
              rhs = neg;
              lhs = pos;
              assert(lhs != rhs);
            }
            // TODO: Detect equivalences for output variables.
          }
        }
      }
    }

    if ((lhs != nullptr) && (rhs != nullptr)) {
      assert(count(rhs) + count(alias) > 0);

      auto range = find(rhs);

      {
        std::vector<Item*> to_process;
        for (auto match = range.first; match != range.second; ++match) {
          to_process.push_back(match->second);
        }
        _items.erase(range.first, range.second);
        for (auto* item : to_process) {
          leReplaceVar<IntLit>(item, rhs, lhs);
        }
      }
      if (alias != nullptr) {
        VarDecl* i2f_lhs;

        auto search = _aliasMap.find(lhs);
        if (search != _aliasMap.end()) {
          i2f_lhs = search->second;
        } else {
          // Create new int2float
          Call* i2f = new Call(lhs->loc().introduce(), constants().ids.int2float, {lhs->id()});
          i2f->decl(_env.model->matchFn(_env, i2f, false));
          assert(i2f->decl());
          i2f->type(Type::varfloat());
          auto* domain =
              new SetLit(lhs->loc().introduce(), eval_floatset(_env, lhs->ti()->domain()));
          auto* i2f_ti = new TypeInst(lhs->loc().introduce(), Type::varfloat(), domain);
          i2f_lhs = new VarDecl(lhs->loc().introduce(), i2f_ti, _env.genId(), i2f);
          i2f_lhs->type(Type::varfloat());
          addItem(new VarDeclI(lhs->loc().introduce(), i2f_lhs));
        }

        auto arange = find(alias);
        {
          std::vector<Item*> to_process;
          for (auto match = arange.first; match != arange.second; ++match) {
            to_process.push_back(match->second);
          }
          _items.erase(arange.first, arange.second);
          for (auto* item : to_process) {
            leReplaceVar<FloatLit>(item, alias, i2f_lhs);
          }
        }
      }

      assert(!rhs->ann().contains(constants().ann.output_var));
      removeItem(it->second);
      _env.counters.linDel++;
      it = _items.erase(it);
    } else {
      ++it;
    }
  }
}

template <class Lit>
void LECompressor::leReplaceVar(Item* i, VarDecl* oldVar, VarDecl* newVar) {
  typedef typename LinearTraits<Lit>::Val Val;
  GCLock lock;

  auto* ci = i->cast<ConstraintI>();
  auto* call = ci->e()->cast<Call>();
  assert(call->id() == constants().ids.int_.lin_le || call->id() == constants().ids.float_.lin_le);

  // Remove old occurrences
  CollectDecls cd(_env.varOccurrences, _deletedVarDecls, i);
  top_down(cd, ci->e());

  ArrayLit* al_c = eval_array_lit(_env, call->arg(0));
  std::vector<Val> coeffs(al_c->size());
  for (int j = 0; j < al_c->size(); j++) {
    coeffs[j] = LinearTraits<Lit>::eval(_env, (*al_c)[j]);
  }
  ArrayLit* al_x = eval_array_lit(_env, call->arg(1));
  std::vector<KeepAlive> x(al_x->size());
  for (int j = 0; j < al_x->size(); j++) {
    Expression* decl = follow_id_to_decl((*al_x)[j]);
    if (decl && decl->cast<VarDecl>() == oldVar) {
      x[j] = newVar->id();
    } else {
      x[j] = (*al_x)[j];
    }
  }
  Val d = LinearTraits<Lit>::eval(_env, call->arg(2));

  simplify_lin<Lit>(coeffs, x, d);
  if (coeffs.empty()) {
    i->remove();
    _env.counters.linDel++;
    return;
  }
  std::vector<Expression*> coeffs_e(coeffs.size());
  std::vector<Expression*> x_e(coeffs.size());
  for (unsigned int j = 0; j < coeffs.size(); j++) {
    coeffs_e[j] = Lit::a(coeffs[j]);
    x_e[j] = x[j]();
    Expression* decl = follow_id_to_decl(x_e[j]);
    if (decl && decl->cast<VarDecl>() == newVar) {
      storeItem(newVar, i);
    }
  }

  if (auto* arg0 = call->arg(0)->dynamicCast<ArrayLit>()) {
    arg0->setVec(coeffs_e);
  } else {
    auto* al_c_new = new ArrayLit(al_c->loc().introduce(), coeffs_e);
    al_c_new->type(al_c->type());
    call->arg(0, al_c_new);
  }

  if (auto* arg1 = call->arg(1)->dynamicCast<ArrayLit>()) {
    arg1->setVec(x_e);
  } else {
    auto* al_x_new = new ArrayLit(al_x->loc().introduce(), x_e);
    al_x_new->type(al_x->type());
    call->arg(1, al_x_new);
  }

  call->arg(2, Lit::a(d));

  // Add new occurences
  CollectOccurrencesE ce(_env.varOccurrences, i);
  top_down(ce, ci->e());
}

bool LECompressor::eqBounds(Expression* a, Expression* b) {
  // TODO: (To optimise) Check lb(lhs) >= lb(rhs) and enforce ub(lhs) <= ub(rhs)
  IntSetVal* dom_a = nullptr;
  IntSetVal* dom_b = nullptr;

  if (auto* a_decl = follow_id_to_decl(a)->dynamicCast<VarDecl>()) {
    if (a_decl->ti()->domain() != nullptr) {
      dom_a = eval_intset(_env, a_decl->ti()->domain());
    }
  } else {
    assert(a->dynamicCast<IntLit>());
    auto* a_val = a->cast<IntLit>();
    dom_a = IntSetVal::a(a_val->v(), a_val->v());
  }

  if (auto* b_decl = follow_id_to_decl(b)->dynamicCast<VarDecl>()) {
    if (b_decl->ti()->domain() != nullptr) {
      dom_b = eval_intset(_env, b_decl->ti()->domain());
    }
  } else {
    assert(b->dynamicCast<IntLit>());
    auto* b_val = b->cast<IntLit>();
    dom_b = IntSetVal::a(b_val->v(), b_val->v());
  }

  return ((dom_a != nullptr) && (dom_b != nullptr) && (dom_a->min() == dom_b->min()) &&
          (dom_a->max() == dom_b->max())) ||
         ((dom_a == nullptr) && (dom_b == nullptr));
}

}  // namespace MiniZinc
