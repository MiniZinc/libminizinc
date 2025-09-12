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
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    _env.flatRemoveItem(ci);
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    _env.flatRemoveItem(vdi);
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
  CollectDecls cd(_env, _env.varOccurrences, _deletedVarDecls, i);
  top_down(cd, c->arg(n));
  c->arg(n, e);
  CollectOccurrencesE ce(_env, _env.varOccurrences, i);
  top_down(ce, e);
}

bool ImpCompressor::trackItem(Item* i) {
  if (i->removed()) {
    return false;
  }
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    if (auto* c = Expression::dynamicCast<Call>(ci->e())) {
      // clause([...], [...]); e.g. x -> y
      if (c->id() == _env.constants.ids.clause) {
        ArrayLit* negative = eval_array_lit(_env, c->arg(1));
        for (unsigned int j = 0; j < negative->size(); ++j) {
          auto* var = Expression::dynamicCast<Id>((*negative)[j]);
          if (var != nullptr) {
            storeItem(var->decl(), i);
          }
        }
        return true;
      }
      if (c->id() == _env.constants.ids.mzn_reverse_map_var) {
        auto* control = Expression::dynamicCast<VarDecl>(follow_id_to_decl(c->arg(0)));
        if (control != nullptr) {
          assert(control->type().isvarbool());
          storeItem(control, i);
        }
        return true;
        // pred_imp(..., b); i.e. b -> pred(...)
      }
      if (c->id().endsWith("_imp")) {
        auto* control =
            Expression::dynamicCast<VarDecl>(follow_id_to_decl(c->arg(c->argCount() - 1)));
        if (control != nullptr) {
          assert(control->type().isvarbool());
          storeItem(control, i);
        }
        return true;
      }
    }
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    if (vdi->e()->type().isvarbool() && (vdi->e() != nullptr) && (vdi->e()->e() != nullptr)) {
      if (auto* c = Expression::dynamicCast<Call>(vdi->e()->e())) {
        // x = forall([y,z,...]); potentially: x -> (y /\ z /\ ...)
        if (c->id() == _env.constants.ids.forall) {
          storeItem(vdi->e(), i);
          return true;
          // x ::ctx_pos = pred(...); potentially: pred_imp(..., x); i.e. x -> pred(...)
        }
        if (_env.fopts.enableHalfReification &&
            Expression::ann(vdi->e()).contains(_env.constants.ctx.pos)) {
          if (c->id() == _env.constants.ids.exists) {
            storeItem(vdi->e(), i);
          } else {
            GCLock lock;
            std::vector<Type> args;
            args.reserve(c->argCount() + 1);
            for (unsigned int j = 0; j < c->argCount(); ++j) {
              args.push_back(Expression::type(c->arg(j)));
            }
            args.push_back(Type::varbool());
            FunctionI* decl = _env.model->matchReification(_env, c->id(), args, true, false);
            if (decl != nullptr && decl->id() == EnvI::halfReifyId(c->id())) {
              storeItem(vdi->e(), i);
              return true;
            }
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
      auto* c = Expression::cast<Call>(ci->e());
      if (c->id() == _env.constants.ids.clause) {
        auto* positive = eval_array_lit(_env, c->arg(0));
        auto* negative = eval_array_lit(_env, c->arg(1));
        if (positive->size() == 1 && negative->size() == 1) {
          auto* var = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*positive)[0]));
          if (var != nullptr) {
            bool output_var = Expression::ann(var).contains(_env.constants.ann.output_var);
            auto usages = _env.varOccurrences.usages(var);
            output_var = output_var || usages.second;
            int occurrences = usages.first;
            unsigned long lhs_occurences = count(var);
            bool is_fixed = var->ti()->domain() != nullptr;
#ifndef NDEBUG
            if (is_fixed) {
              std::cerr << "ERROR: We expect propagation to have taken care of all fixed variables "
                           "before chain propagation. This can be ignored in release builds, but "
                           "should be investigated by the MiniZinc Team";
              assert(!is_fixed);
            }
#endif

            // Compress if:
            // - There is one occurrence on the RHS of a clause and the others are on the LHS of a
            // clause
            // - There is one occurrence on the RHS of a clause, that Id is a reified forall that
            // has no other occurrences
            // - There is one occurrence on the RHS of a clause, that Id is a reification in a
            // positive context, and all other occurrences are on the LHS of a clause
            bool compress = !is_fixed && !output_var && lhs_occurences > 0;
            if ((var->e() != nullptr) && Expression::isa<Call>(var->e())) {
              auto* call = Expression::cast<Call>(var->e());
              if (call->id() == _env.constants.ids.forall) {
                compress = compress && (occurrences == 1 && lhs_occurences == 1);
              } else {
                compress = compress && (occurrences == lhs_occurences);
              }
            } else {
              compress = compress && (occurrences == lhs_occurences + 1);
            }
            if (compress) {
              rhs = var;
              lhs = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*negative)[0]));
              if (lhs == rhs) {
                continue;
              }
            }
            // TODO: Detect equivalences for output variables.
          }
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
        bool success = compressItem(item, rhs, lhs);
        assert(success);
        _env.counters.impDel++;
      }

      assert(!Expression::ann(rhs).contains(_env.constants.ann.output_var));
      removeItem(it->second);
      it = _items.erase(it);
    } else {
      ++it;
    }
  }
}

bool ImpCompressor::compressItem(Item* i, VarDecl* oldLHS, VarDecl* newLHS) {
  GCLock lock;
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    auto* c = Expression::cast<Call>(ci->e());
    // Given (x -> y) /\ (y -> z), produce x -> z
    if (c->id() == _env.constants.ids.clause) {
      // Get clause array literals to be changed
      auto* positive = eval_array_lit(_env, c->arg(0));
      auto* negative = eval_array_lit(_env, c->arg(1));
      // Avoid creating a -> a (constraint can just be removed)
      if (positive->size() == 1 && negative->size() == 1) {
        auto* positiveDecl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*positive)[0]));
        if (positiveDecl == newLHS) {
          removeItem(i);
          return true;
        }
      }

      // Create new negative array
      std::vector<Expression*> contents = std::vector<Expression*>(negative->size());
      for (unsigned int i = 0; i < negative->size(); ++i) {
        auto* vd = Expression::cast<VarDecl>(follow_id_to_decl((*negative)[i]));
        if (vd == oldLHS) {
          contents[i] = newLHS->id();
        } else {
          contents[i] = vd->id();
          // Remove ci from multimap for other negative variables
          auto range = find(vd);
          for (auto it = range.first; it != range.second; ++it) {
            if (it->second == ci) {
              _items.erase(it);
              break;
            }
          }
        }
      }
      negative = new ArrayLit(Expression::loc(negative).introduce(), contents);
      negative->type(Type::varbool(1));

      negative = arrayLitCopyReplace(negative, oldLHS, newLHS);
      auto* nci = constructClause(positive, negative);

      _boolConstraints.push_back(addItem(nci));
      removeItem(i);
      return true;
    }
    if (c->id() == _env.constants.ids.mzn_reverse_map_var) {
      return true;
    }
    // Given (x -> y) /\ (y -> pred(...)), produce x -> pred(...)
    if (c->id().endsWith("_imp")) {
      replaceCallArgument(i, c, c->argCount() - 1, newLHS->id());
      trackItem(i);
      return true;
    }
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    auto* c = Expression::cast<Call>(vdi->e()->e());
    // Given: (x -> y) /\  (y -> (a /\ b /\ ...)), produce (x -> a) /\ (x -> b) /\ ...
    if (c->id() == _env.constants.ids.forall) {
      auto* exprs = eval_array_lit(_env, c->arg(0));
      for (unsigned int j = 0; j < exprs->size(); ++j) {
        auto* rhsDecl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*exprs)[j]));
        if (rhsDecl != newLHS) {
          ConstraintI* nci = constructClause((*exprs)[j], newLHS->id());
          _boolConstraints.push_back(addItem(nci));
        }
      }
      return true;
      // x ::ctx_pos = pred(...); potentially: pred_imp(..., x); i.e. x -> pred(...)
    }
    if (Expression::ann(vdi->e()).contains(_env.constants.ctx.pos)) {
      if (c->id() == _env.constants.ids.exists) {
        auto* positive = eval_array_lit(_env, c->arg(0));
        auto* positiveDecl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*positive)[0]));
        if (positiveDecl != newLHS) {
          ConstraintI* nci = constructClause(positive, newLHS->id());
          _boolConstraints.push_back(addItem(nci));
        }
        removeItem(i);
        return true;
      }
      ConstraintI* nci = constructHalfReif(c, newLHS->id());
      assert(nci);
      addItem(nci);
      return true;
    }
  }
  return false;
}

ArrayLit* ImpCompressor::arrayLitCopyReplace(ArrayLit* arr, VarDecl* oldVar, VarDecl* newVar) {
  assert(GC::locked());

  std::vector<Expression*> contents = std::vector<Expression*>(arr->size());
  for (unsigned int i = 0; i < arr->size(); ++i) {
    auto* vd = Expression::cast<VarDecl>(follow_id_to_decl((*arr)[i]));
    if (vd == oldVar) {
      contents[i] = newVar->id();
    } else {
      contents[i] = vd->id();
    }
  }
  auto* ret = new ArrayLit(Expression::loc(arr).introduce(), contents);
  ret->type(arr->type());
  return ret;
}
ConstraintI* ImpCompressor::constructClause(Expression* pos, Expression* neg) {
  assert(GC::locked());
  std::vector<Expression*> args(2);
  if (Expression::dynamicCast<ArrayLit>(pos) != nullptr) {
    args[0] = pos;
  } else {
    assert(Expression::type(neg).isbool());
    std::vector<Expression*> eVec(1);
    eVec[0] = pos;
    args[0] = new ArrayLit(Expression::loc(pos).introduce(), eVec);
    Expression::type(args[0], Type::varbool(1));
  }
  if (Expression::dynamicCast<ArrayLit>(neg) != nullptr) {
    args[1] = neg;
  } else {
    assert(Expression::type(neg).isbool());
    std::vector<Expression*> eVec(1);
    eVec[0] = neg;
    args[1] = new ArrayLit(Expression::loc(neg).introduce(), eVec);
    Expression::type(args[1], Type::varbool(1));
  }
  // NEVER CREATE (a -> a)
  assert(Expression::cast<ArrayLit>(args[0])->size() != 1 ||
         Expression::cast<ArrayLit>(args[1])->size() != 1 ||
         !Expression::isa<Id>((*Expression::cast<ArrayLit>(args[0]))[0]) ||
         !Expression::isa<Id>((*Expression::cast<ArrayLit>(args[1]))[0]) ||
         Expression::cast<Id>((*Expression::cast<ArrayLit>(args[0]))[0])->decl() !=
             Expression::cast<Id>((*Expression::cast<ArrayLit>(args[1]))[0])->decl());
  auto* nc = Call::a(MiniZinc::Location().introduce(), _env.constants.ids.clause, args);
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
  for (unsigned int i = 0; i < call->argCount(); ++i) {
    args[i] = call->arg(i);
  }
  args.push_back(control);
  FunctionI* decl = _env.model->matchFn(_env, cid, args, false);
  if (decl != nullptr) {
    auto* nc = Call::a(Expression::loc(call).introduce(), cid, args);
    nc->decl(decl);
    nc->type(Type::varbool());
    return new ConstraintI(Expression::loc(call).introduce(), nc);
  }
  return nullptr;
}

bool LECompressor::trackItem(Item* i) {
  if (i->removed()) {
    return false;
  }
  bool added = false;
  if (auto* ci = i->dynamicCast<ConstraintI>()) {
    if (auto* call = Expression::dynamicCast<Call>(ci->e())) {
      // {int,float}_lin_le([c1,c2,...], [x, y,...], 0);
      if (call->id() == _env.constants.ids.int_.lin_le ||
          call->id() == _env.constants.ids.float_.lin_le) {
        ArrayLit* as = eval_array_lit(_env, call->arg(0));
        ArrayLit* bs = eval_array_lit(_env, call->arg(1));
        assert(as->size() == bs->size());

        for (unsigned int j = 0; j < as->size(); ++j) {
          if (as->type().isIntArray()) {
            if (eval_int(_env, (*as)[j]) > IntVal(0)) {
              // Check if left hand side is a variable (could be constant)
              if (auto* decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*bs)[j]))) {
                storeItem(decl, i);
                added = true;
              }
            }
          } else {
            if (eval_float(_env, (*as)[j]) > FloatVal(0)) {
              // Check if left hand side is a variable (could be constant)
              if (auto* decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*bs)[j]))) {
                storeItem(decl, i);
                added = true;
              }
            }
          }
        }
      }
      assert(call->id() != _env.constants.ids.int2float);
    }
  } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
    assert(vdi->e());
    if (Expression* vde = vdi->e()->e()) {
      if (auto* call = Expression::dynamicCast<Call>(vde)) {
        if (call->id() == _env.constants.ids.int2float) {
          if (auto* vd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(call->arg(0)))) {
            auto* alias = Expression::dynamicCast<VarDecl>(follow_id_to_decl(vdi->e()));
            if (alias != nullptr) {
              _aliasMap[vd] = alias;
            }
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
      auto* call = Expression::cast<Call>(ci->e());
      if (call->id() == _env.constants.ids.int_.lin_le) {
        ArrayLit* as = eval_array_lit(_env, call->arg(0));
        ArrayLit* bs = eval_array_lit(_env, call->arg(1));
        IntVal c = eval_int(_env, call->arg(2));

        if (bs->size() == 2 && c == IntVal(0)) {
          IntVal a0 = eval_int(_env, (*as)[0]);
          IntVal a1 = eval_int(_env, (*as)[1]);
          if (a0 == -a1 && eqBounds((*bs)[0], (*bs)[1])) {
            int i = a0 < a1 ? 0 : 1;
            if (!Expression::isa<Id>((*bs)[i])) {
              break;
            }
            auto* neg = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*bs)[i]));
            if (neg == nullptr) {
              continue;
            }
            bool output_var = Expression::ann(neg).contains(_env.constants.ann.output_var);

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

            auto* pos = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*bs)[1 - i]));
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
          Call* i2f =
              Call::a(Expression::loc(lhs).introduce(), _env.constants.ids.int2float, {lhs->id()});
          i2f->decl(_env.model->matchFn(_env, i2f, false));
          assert(i2f->decl());
          i2f->type(Type::varfloat());
          auto* domain = new SetLit(Expression::loc(lhs).introduce(),
                                    eval_floatset(_env, lhs->ti()->domain()));
          auto* i2f_ti = new TypeInst(Expression::loc(lhs).introduce(), Type::varfloat(), domain);
          i2f_lhs = new VarDecl(Expression::loc(lhs).introduce(), i2f_ti, _env.genId(), i2f);
          i2f_lhs->type(Type::varfloat());
          addItem(VarDeclI::a(Expression::loc(lhs).introduce(), i2f_lhs));
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

      assert(!Expression::ann(rhs).contains(_env.constants.ann.output_var));
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
  auto* call = Expression::cast<Call>(ci->e());
  assert(call->id() == _env.constants.ids.int_.lin_le ||
         call->id() == _env.constants.ids.float_.lin_le);

  // Remove old occurrences
  CollectDecls cd(_env, _env.varOccurrences, _deletedVarDecls, i);
  top_down(cd, ci->e());

  ArrayLit* al_c = eval_array_lit(_env, call->arg(0));
  std::vector<Val> coeffs(al_c->size());
  for (unsigned int j = 0; j < al_c->size(); j++) {
    coeffs[j] = LinearTraits<Lit>::eval(_env, (*al_c)[j]);
  }
  ArrayLit* al_x = eval_array_lit(_env, call->arg(1));
  std::vector<KeepAlive> x(al_x->size());
  for (unsigned int j = 0; j < al_x->size(); j++) {
    Expression* decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*al_x)[j]));
    if (decl && decl == oldVar) {
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
    Expression* decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl(x_e[j]));
    if (decl && Expression::cast<VarDecl>(decl) == newVar) {
      storeItem(newVar, i);
    }
  }

  auto* al_c_new = new ArrayLit(Expression::loc(al_c).introduce(), coeffs_e);
  al_c_new->type(al_c->type());
  call->arg(0, al_c_new);

  auto* al_x_new = new ArrayLit(Expression::loc(al_x).introduce(), x_e);
  al_x_new->type(al_x->type());
  call->arg(1, al_x_new);

  call->arg(2, Lit::a(d));

  // Add new occurences
  CollectOccurrencesE ce(_env, _env.varOccurrences, i);
  top_down(ce, ci->e());
}

bool LECompressor::eqBounds(Expression* a, Expression* b) {
  // TODO: (To optimise) Check lb(lhs) >= lb(rhs) and enforce ub(lhs) <= ub(rhs)
  IntSetVal* dom_a = nullptr;
  IntSetVal* dom_b = nullptr;

  if (auto* a_decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl(a))) {
    if (a_decl->ti()->domain() != nullptr) {
      dom_a = eval_intset(_env, a_decl->ti()->domain());
    }
  } else {
    IntVal a_val = eval_int(_env, a);
    dom_a = IntSetVal::a(a_val, a_val);
  }

  if (auto* b_decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl(b))) {
    if (b_decl->ti()->domain() != nullptr) {
      dom_b = eval_intset(_env, b_decl->ti()->domain());
    }
  } else {
    IntVal b_val = eval_int(_env, b);
    dom_b = IntSetVal::a(b_val, b_val);
  }

  return (dom_a != nullptr && dom_b != nullptr && !dom_a->empty() && !dom_b->empty() &&
          dom_a->min() == dom_b->min() && dom_a->max() == dom_b->max()) ||
         (dom_a == nullptr && dom_b == nullptr);
}

}  // namespace MiniZinc
