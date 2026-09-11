/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/chain_compressor.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/iter.hh>
#include <minizinc/optimize.hh>
#include <minizinc/optimize_constraints.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/values.hh>

#include <deque>
#include <vector>

namespace MiniZinc {

void VarOccurrences::addIndex(VarDeclI* i, unsigned int idx_i) { idx.insert(i->e()->id(), idx_i); }
void VarOccurrences::addIndex(VarDecl* e, unsigned int idx_i) {
  assert(find(e) == -1);
  idx.insert(e->id(), idx_i);
}
int VarOccurrences::find(VarDecl* vd) {
  auto it = idx.find(vd->id());
  return it.first ? static_cast<int>(*it.second) : -1;
}
void VarOccurrences::remove(VarDecl* vd) { idx.remove(vd->id()); }

void VarOccurrences::add(VarDecl* v, Item* i) {
  auto vi = itemMap.find(v->id()->decl()->id());
  if (vi.first) {
    vi.second->insert(i);
  } else {
    Items items({i});
    itemMap.insert(v->id()->decl()->id(), items);
  }
}

int VarOccurrences::remove(VarDecl* v, Item* i) {
  auto vi = itemMap.find(v->id()->decl()->id());
  assert(vi.first);
  vi.second->erase(i);
  return static_cast<int>(vi.second->size());
}

void VarOccurrences::removeAllOccurrences(VarDecl* v) {
  auto vi = itemMap.find(v->id()->decl()->id());
  assert(vi.first);
  vi.second->clear();
}

void VarOccurrences::unify(EnvI& env, Model* m, Id* id0_0, Id* id1_0) {
  Id* id0 = id0_0->decl()->id();
  Id* id1 = id1_0->decl()->id();

  VarDecl* v0 = id0->decl();
  VarDecl* v1 = id1->decl();

  if (v0 == v1) {
    return;
  }

  int v0idx = find(v0);
  assert(v0idx != -1);
  (*env.flat())[v0idx]->remove();

  auto vi0 = itemMap.find(v0->id());
  if (vi0.first) {
    auto vi1 = itemMap.find(v1->id());
    if (vi1.first) {
      for (auto* item : *vi0.second) {
        vi1.second->insert(item);
      }
    } else {
      itemMap.insert(v1->id(), *vi0.second);
    }
    itemMap.remove(v0->id());
  }

  remove(v0);
  id0->redirect(id1);
}

void VarOccurrences::clear() {
  itemMap.clear();
  idx.clear();
}

int VarOccurrences::occurrences(VarDecl* v) {
  auto vi = itemMap.find(v->id()->decl()->id());
  return vi.first ? static_cast<int>(vi.second->size()) : 0;
}

std::pair<int, bool> VarOccurrences::usages(VarDecl* v) {
  bool is_output = Expression::ann(v).contains(Constants::constants().ann.output_var) ||
                   Expression::ann(v).containsCall(Constants::constants().ann.output_array);
  auto vi = itemMap.find(v->id()->decl()->id());
  if (!vi.first) {
    return std::make_pair(0, is_output);
  }
  int count = 0;
  for (Item* i : *vi.second) {
    auto* vd = i->dynamicCast<VarDeclI>();
    if ((vd != nullptr) && (vd->e() != nullptr) && (vd->e()->e() != nullptr) &&
        (Expression::isa<ArrayLit>(vd->e()->e()) || Expression::isa<SetLit>(vd->e()->e()))) {
      auto u = usages(vd->e());
      is_output = is_output || u.second;
      count += u.first;
    } else {
      count++;
    }
  }
  return std::make_pair(count, is_output);
}

void CollectOccurrencesE::vId(const Id* id) {
  if (id->decl() == nullptr) {
    return;
  }
  // TODO: Consider a better fix to not count internal annotations!
  for (const auto* ann : env.constants.internalAnn()) {
    if (id->decl() == ann->decl()) {
      return;
    }
  }
  // ENDTODO
  vo.add(id->decl(), ci);
}

void CollectOccurrencesI::vVarDeclI(VarDeclI* v) {
  CollectOccurrencesE ce(env, vo, v);
  top_down(ce, v->e());
}
void CollectOccurrencesI::vConstraintI(ConstraintI* ci) {
  CollectOccurrencesE ce(env, vo, ci);
  top_down(ce, ci->e());
  for (ExpressionSetIter it = Expression::ann(ci->e()).begin();
       it != Expression::ann(ci->e()).end(); ++it) {
    top_down(ce, *it);
  }
}
void CollectOccurrencesI::vSolveI(SolveI* si) {
  CollectOccurrencesE ce(env, vo, si);
  top_down(ce, si->e());
  for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
    top_down(ce, *it);
  }
}

void CollectDecls::vId(Id* id) {
  if (id->decl() == nullptr) {
    return;
  }
  // TODO: Consider a better fix to not count internal annotations!
  for (const auto* ann : env.constants.internalAnn()) {
    if (id->decl() == ann->decl()) {
      return;
    }
  }
  // ENDTODO
  int count = vo.remove(id->decl(), item);
  if (count == 0 && varIsFree(id->decl())) {
    vd.push_back(id->decl());
  }
}

bool is_output(VarDecl* vd) {
  for (ExpressionSetIter it = Expression::ann(vd).begin(); it != Expression::ann(vd).end(); ++it) {
    if (*it != nullptr) {
      if (*it == Constants::constants().ann.output_var) {
        return true;
      }
      if (Call* c = Expression::dynamicCast<Call>(*it)) {
        if (c->id() == Constants::constants().ann.output_array) {
          return true;
        }
      }
    }
  }
  return false;
}

Expression* fixed_output_value(VarDecl* vd) {
  if (vd->type().isbool() && (vd->ti()->domain() != nullptr)) {
    return vd->ti()->domain();
  }
  if (vd->type().isint()) {
    if ((vd->e() != nullptr) && Expression::isa<IntLit>(vd->e())) {
      return vd->e();
    }
    if ((vd->ti()->domain() != nullptr) && Expression::isa<SetLit>(vd->ti()->domain()) &&
        Expression::cast<SetLit>(vd->ti()->domain())->isv()->size() == 1 &&
        Expression::cast<SetLit>(vd->ti()->domain())->isv()->min() ==
            Expression::cast<SetLit>(vd->ti()->domain())->isv()->max()) {
      return IntLit::a(Expression::cast<SetLit>(vd->ti()->domain())->isv()->min());
    }
    return nullptr;
  }
  if (vd->type().isfloat()) {
    if ((vd->e() != nullptr) && Expression::isa<FloatLit>(vd->e())) {
      return vd->e();
    }
    if ((vd->ti()->domain() != nullptr) && Expression::isa<SetLit>(vd->ti()->domain()) &&
        Expression::cast<SetLit>(vd->ti()->domain())->fsv()->size() == 1 &&
        Expression::cast<SetLit>(vd->ti()->domain())->fsv()->min() ==
            Expression::cast<SetLit>(vd->ti()->domain())->fsv()->max()) {
      return FloatLit::a(Expression::cast<SetLit>(vd->ti()->domain())->fsv()->min());
    }
    return nullptr;
  }
  if (vd->type().isIntSet()) {
    return vd->e() != nullptr && Expression::isa<SetLit>(vd->e()) ? vd->e() : nullptr;
  }
  if (vd->type().dim() > 0 && vd->type().isPar()) {
    return vd->e() != nullptr && Expression::isa<ArrayLit>(vd->e()) ? vd->e() : nullptr;
  }
  return nullptr;
}

bool can_remove_fzn_vardecl(EnvI& env, VarDecl* vd) {
  if (env.varOccurrences.occurrences(vd) != 0) {
    return false;
  }
  if (vd->ti()->domain() != nullptr && !vd->ti()->computedDomain() && vd->e() != nullptr &&
      Expression::isa<Call>(vd->e())) {
    return false;
  }
  return !is_output(vd) || fixed_output_value(vd) != nullptr;
}

/// Turn \a vd's defining call into a constraint of its own.
///
/// A variable defined by a call carries it as a right-hand side until the
/// cleanup pass turns it into a constraint with a `defines_var` annotation.
/// Two such variables cannot be unified while both still hold one: `unify`
/// moves a right-hand side across and would overwrite — and so lose — whichever
/// definition it landed on. Demoting one of them first says exactly the same
/// thing, with that definition posted as an ordinary constraint, which is what
/// the cleanup pass would have produced anyway. The variable is then free and
/// the two can be unified.
///
/// Returns false and changes nothing if the definition is not a call this can
/// be done for. `exists`, `forall` and `clause` are excluded: their relational
/// forms are not their own name with the result appended, and the cleanup pass
/// has dedicated cases for them.
bool fznso_par_constraint(EnvI& env, Call* c, bool& holds);

bool demote_definition(EnvI& env, VarDecl* vd) {
  Call* c = Expression::dynamicCast<Call>(vd->e());
  if (c == nullptr || !vd->type().isvarbool() || c->id() == env.constants.ids.exists ||
      c->id() == env.constants.ids.forall || (c->id() == env.constants.ids.clause ||
             c->id() == env.constants.ids.fznso.bool_clause ||
             c->id() == env.constants.ids.bool_.clause)) {
    return false;
  }
  GCLock lock;
  std::vector<Expression*> args(c->argCount() + 1);
  for (unsigned int i = 0; i < c->argCount(); i++) {
    args[i] = c->arg(i);
  }
  args[c->argCount()] = vd->id();
  bool canHalfReify =
      env.fopts.enableHalfReification && Expression::ann(vd).contains(env.constants.ctx.pos);
  FunctionI* decl = env.model->matchReifByNames(env, c, canHalfReify, false);
  if (decl == nullptr) {
    decl = env.model->matchReification(env, c->id(), args, canHalfReify, false);
  }
  if (decl == nullptr) {
    return false;
  }
  Call* nc = Call::a(Expression::loc(c).introduce(), decl->id(), args);
  nc->type(Type::varbool());
  nc->decl(decl);
  Expression::ann(nc).merge(Expression::ann(c));
  vd->e(nullptr);
  Expression::ann(vd).remove(env.constants.ann.is_defined_var);
  env.flatAddItem(new ConstraintI(Expression::loc(c), nc));
  return true;
}

bool fznso_lin_terms(EnvI& env, Call* c, std::vector<std::pair<VarDecl*, IntVal>>& terms,
                     IntVal& rhs);
bool fznso_fix_to(EnvI& env, VarDecl* vd, IntVal v, std::deque<unsigned int>& vardeclQueue,
                  std::deque<Item*>& constraintQueue);

void unify(EnvI& env, std::vector<VarDecl*>& deletedVarDecls, Id* id0, Id* id1) {
  if (id0->decl() != id1->decl()) {
    if (is_output(id0->decl())) {
      std::swap(id0, id1);
    }

    // Once id0 is redirected to id1, an existing RHS alias stored from id0 to
    // id1 would resolve as id1 = id1 after we move it across. We drop that
    // degenerate alias instead of copying it.
    if (auto* alias = Expression::dynamicCast<Id>(id0->decl()->e())) {
      VarDecl* target = alias->decl();
      if (target == id0->decl() || target == id1->decl()) {
        id0->decl()->e(nullptr);
      }
    }
    if (id0->decl()->e() != nullptr) {
      Expression* rhs = id0->decl()->e();

      auto* vdi1 = (*env.flat())[env.varOccurrences.find(id1->decl())]->cast<VarDeclI>();
      CollectOccurrencesE ce(env, env.varOccurrences, vdi1);
      top_down(ce, rhs);

      id1->decl()->e(rhs);
      id0->decl()->e(nullptr);

      auto* vdi0 = (*env.flat())[env.varOccurrences.find(id0->decl())]->cast<VarDeclI>();
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, vdi0);
      top_down(cd, rhs);
    }
    // If id1 already aliases id0 (or itself through an earlier redirect),
    // redirecting id0 to id1 would make that RHS resolve as id1 = id1.
    // Clear the alias and unsubscribe the old RHS occurrences now.
    if (auto* alias = Expression::dynamicCast<Id>(id1->decl()->e())) {
      VarDecl* target = alias->decl();
      if (target == id0->decl() || target == id1->decl()) {
        Expression* rhs = id1->decl()->e();
        id1->decl()->e(nullptr);

        auto* vdi1 = (*env.flat())[env.varOccurrences.find(id1->decl())]->cast<VarDeclI>();
        CollectDecls cd(env, env.varOccurrences, deletedVarDecls, vdi1);
        top_down(cd, rhs);
      }
    }
    if (Expression::equal(id1->decl()->e(), id0->decl()->id())) {
      auto* vdi1 = (*env.flat())[env.varOccurrences.find(id1->decl())]->cast<VarDeclI>();
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, vdi1);
      Expression* rhs = id1->decl()->e();
      top_down(cd, rhs);
      id1->decl()->e(nullptr);
    }
    // Compute intersection of domains
    std::vector<std::pair<TypeInst*, TypeInst*>> todo({{id0->decl()->ti(), id1->decl()->ti()}});
    while (!todo.empty()) {
      auto it = todo.back();
      todo.pop_back();
      auto* ti0 = it.first;
      auto* ti1 = it.second;
      if (ti0->domain() != nullptr) {
        if (ti1->domain() != nullptr) {
          if (ti0->type().structBT()) {
            auto* tis0 = Expression::cast<ArrayLit>(ti0->domain());
            auto* tis1 = Expression::cast<ArrayLit>(ti1->domain());
            for (unsigned int i = 0; i < tis0->size(); i++) {
              todo.emplace_back(Expression::cast<TypeInst>((*tis0)[i]),
                                Expression::cast<TypeInst>((*tis1)[i]));
            }
          } else if (ti0->type().bt() == Type::BT_INT) {
            IntSetVal* isv0 = eval_intset(env, ti0->domain());
            IntSetVal* isv1 = eval_intset(env, ti1->domain());
            IntSetRanges isv0r(isv0);
            IntSetRanges isv1r(isv1);
            Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(isv0r, isv1r);
            IntSetVal* nd = IntSetVal::ai(inter);
            if (nd->empty() && !ti0->type().isSet()) {
              env.fail();
            } else if (!nd->equal(isv1)) {
              ti1->domain(new SetLit(Location(), nd));
              if (nd->equal(isv0)) {
                ti1->setComputedDomain(ti0->computedDomain());
              } else {
                ti1->setComputedDomain(false);
              }
            }
          } else if (ti0->type().bt() == Type::BT_BOOL) {
            if (eval_bool(env, ti0->domain()) != eval_bool(env, ti1->domain())) {
              env.fail();
            }
          } else if (ti0->type().bt() == Type::BT_FLOAT) {
            // float
            FloatSetVal* isv0 = eval_floatset(env, ti0->domain());
            FloatSetVal* isv1 = eval_floatset(env, ti1->domain());
            FloatSetRanges isv0r(isv0);
            FloatSetRanges isv1r(isv1);
            Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> inter(isv0r, isv1r);
            FloatSetVal* nd = FloatSetVal::ai(inter);

            FloatSetRanges nd_r(nd);
            FloatSetRanges isv1r_2(isv1);

            if (nd->empty()) {
              env.fail();
            } else if (!Ranges::equal(nd_r, isv1r_2)) {
              ti1->domain(new SetLit(Location(), nd));
              FloatSetRanges nd_r_2(nd);
              FloatSetRanges isv0r_2(isv0);
              if (Ranges::equal(nd_r_2, isv0r_2)) {
                ti1->setComputedDomain(ti0->computedDomain());
              } else {
                ti1->setComputedDomain(false);
              }
            }
          } else {
            throw InternalError("Failed to unify identifiers during optimisation");
          }
        } else {
          ti1->domain(ti0->domain());
        }
      }
    }

    // If both variables are output variables, unify them in the output model
    if (is_output(id0->decl())) {
      assert(env.outputFlatVarOccurrences.find(id0->decl()) != -1);
      VarDecl* id0_output =
          (*env.output)[env.outputFlatVarOccurrences.find(id0->decl())]->cast<VarDeclI>()->e();
      assert(env.outputFlatVarOccurrences.find(id1->decl()) != -1);
      VarDecl* id1_output =
          (*env.output)[env.outputFlatVarOccurrences.find(id1->decl())]->cast<VarDeclI>()->e();
      auto* decl = Expression::cast<VarDecl>(follow_id_to_decl(id0_output));
      if (decl->e() == nullptr && decl != id1_output) {
        decl->e(id1_output->id());
      }
    }

    // If `id0` is an assumption variable being unified away, move its entry in the
    // unsatisfiable-core reverse map (see `assume`) to the surviving variable `id1`, so the
    // core can still be reported in terms of the original expression.
    auto assumeIt = env.assumptionExprs.find(id0->decl()->id()->str());
    if (assumeIt != env.assumptionExprs.end()) {
      Expression* assumeExpr = assumeIt->second;
      env.assumptionExprs.erase(assumeIt);
      env.assumptionExprs.emplace(id1->decl()->id()->str(), assumeExpr);
    }

    env.varOccurrences.unify(env, env.flat(), id0, id1);
  }
}

void substitute_fixed_vars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls);
void simplify_bool_constraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::deque<unsigned int>& vardeclQueue,
                              std::deque<Item*>& constraintQueue, std::vector<Item*>& toRemove,
                              std::vector<VarDecl*>& deletedVarDecls,
                              std::unordered_map<Expression*, int>& nonFixedLiteralCount,
                              std::vector<std::pair<Id*, Item*>>& toUnsubscribe);

bool simplify_constraint(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls,
                         std::deque<Item*>& constraintQueue,
                         std::deque<unsigned int>& vardeclQueue);

void push_vardecl(EnvI& env, VarDeclI* vdi, unsigned int vd_idx, std::deque<unsigned int>& q) {
  if (!vdi->removed() && !vdi->flag()) {
    vdi->flag(true);
    q.push_back(vd_idx);
  }
}
void push_vardecl(EnvI& env, unsigned int vd_idx, std::deque<unsigned int>& q) {
  push_vardecl(env, (*env.flat())[vd_idx]->cast<VarDeclI>(), vd_idx, q);
}

void push_dependent_constraints(EnvI& env, Id* id, std::deque<Item*>& q) {
  auto it = env.varOccurrences.itemMap.find(id->decl()->id());
  if (it.first) {
    for (auto* item : *it.second) {
      if (auto* ci = item->dynamicCast<ConstraintI>()) {
        if (!ci->removed() && !ci->flag()) {
          ci->flag(true);
          q.push_back(ci);
        }
      } else if (auto* vdi = item->dynamicCast<VarDeclI>()) {
        if (vdi->e()->id()->decl() != vdi->e()) {
          vdi = (*env.flat())[env.varOccurrences.find(vdi->e()->id()->decl())]->cast<VarDeclI>();
        }
        if (!vdi->removed() && !vdi->flag() && (vdi->e()->e() != nullptr)) {
          vdi->flag(true);
          q.push_back(vdi);
        }
      }
    }
  }
}

void remove_deleted_items(EnvI& envi, std::vector<VarDecl*>& deletedVarDecls) {
  // TODO: The delayed deletion could be done eagerly by the creation of
  // env.optRemoveItem() which contains the logic in this while loop.
  auto& m = *envi.flat();
  while (!deletedVarDecls.empty()) {
    envi.checkCancel();
    VarDecl* cur = deletedVarDecls.back();
    deletedVarDecls.pop_back();
    if (can_remove_fzn_vardecl(envi, cur)) {
      auto cur_idx = envi.varOccurrences.idx.find(cur->id());
      if (cur_idx.first && !m[*cur_idx.second]->removed()) {
        auto* vdi = m[*cur_idx.second]->cast<VarDeclI>();
        cur = vdi->e();
        if (is_output(cur)) {
          // We have to change the output model if we remove this variable
          Expression* val = fixed_output_value(cur);
          if (val != nullptr) {
            // Find corresponding variable in output model and fix it
            VarDecl* vd_out =
                (*envi.output)[envi.outputFlatVarOccurrences.find(cur)]->cast<VarDeclI>()->e();
            vd_out->e(val);
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, vdi);
            top_down(cd, cur->e());
            vdi->remove();
          }
        } else {
          CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, vdi);
          top_down(cd, cur->e());
          vdi->remove();
        }
      }
    }
  }
}

/// Replace a Boolean's negation by `1 - b` wherever it is read.
///
/// FlatZinc has no negated literal, so a library that keeps `var bool` states a
/// negation as a variable of its own plus a row `b + nb = 1`. That row is pure
/// overhead: every place `nb` is read can read `b` instead — a linear row by
/// negating its coefficient and shifting its bound, a clause by moving the
/// literal to the other side, a big-M row through the conversion the indicator
/// arrives as. `linear/` never pays this, having no Boolean to begin with.
///
/// Each rewrite is an identity, so this is exact rather than a relaxation. Only
/// applied when *every* use of the negation is one of them, so nothing is left
/// referring to a variable that no longer exists.
void fznso_substitute_negations(EnvI& env, Model& m, std::vector<VarDecl*>& deletedVarDecls) {
  const auto& ids = env.constants.ids;
  auto isConversion = [&](const Call* c) {
    return c->id() == ids.bool2int || c->id() == ids.fznso.bool2int;
  };
  auto isIntLin = [&](const Call* c) {
    return c->id() == ids.fznso.int_lin_le || c->id() == ids.fznso.int_lin_eq ||
           c->id() == ids.fznso.int_lin_ne || c->id() == ids.int_.lin_le ||
           c->id() == ids.int_.lin_eq || c->id() == ids.int_.lin_ne;
  };
  auto isBoolLin = [&](const Call* c) {
    return c->id() == ids.fznso.bool_lin_le || c->id() == ids.fznso.bool_lin_eq;
  };
  auto isClause = [&](const Call* c) {
    return c->id() == ids.clause || c->id() == ids.fznso.bool_clause ||
           c->id() == ids.bool_.clause;
  };

  /// Rewrite one linear row, replacing \a from by \a to with the coefficient
  /// negated and the bound shifted: `c * from` is `c - c * to`.
  auto flipTerm = [&](ConstraintI* ci, VarDecl* from, VarDecl* to) {
    auto* c = Expression::cast<Call>(ci->e());
    auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
    auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
    if (alC == nullptr || alX == nullptr || alC->size() != alX->size()) {
      return false;
    }
    std::vector<Expression*> coeffs(alC->size());
    std::vector<Expression*> vars(alX->size());
    IntVal rhs = eval_int(env, c->arg(2));
    for (unsigned int j = 0; j < alX->size(); j++) {
      coeffs[j] = (*alC)[j];
      vars[j] = (*alX)[j];
      if (Expression::dynamicCast<VarDecl>(follow_id_to_decl((*alX)[j])) != from) {
        continue;
      }
      if (!Expression::isa<IntLit>((*alC)[j])) {
        return false;
      }
      IntVal a = IntLit::v(Expression::cast<IntLit>((*alC)[j]));
      coeffs[j] = IntLit::a(-a);
      vars[j] = to->id();
      rhs -= a;
    }
    auto* nc = new ArrayLit(Location().introduce(), coeffs);
    nc->type(Type::parint(1));
    auto* nv = new ArrayLit(Location().introduce(), vars);
    Type vt = alX->type();
    vt.dim(1);
    nv->type(vt);
    c->arg(0, nc);
    c->arg(1, nv);
    c->arg(2, IntLit::a(rhs));
    return true;
  };

  /// Move \a from from one side of a clause to \a to on the other.
  auto flipLiteral = [&](ConstraintI* ci, VarDecl* from, VarDecl* to) {
    auto* c = Expression::cast<Call>(ci->e());
    std::vector<Expression*> side[2];
    for (int k = 0; k < 2; k++) {
      auto* al = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(k)));
      if (al == nullptr) {
        return false;
      }
      for (unsigned int j = 0; j < al->size(); j++) {
        side[k].push_back((*al)[j]);
      }
    }
    for (int k = 0; k < 2; k++) {
      for (auto it = side[k].begin(); it != side[k].end();) {
        if (Expression::dynamicCast<VarDecl>(follow_id_to_decl(*it)) == from) {
          it = side[k].erase(it);
          side[1 - k].push_back(to->id());
        } else {
          ++it;
        }
      }
    }
    for (int k = 0; k < 2; k++) {
      auto* al = new ArrayLit(Location().introduce(), side[k]);
      al->type(Type::varbool(1));
      c->arg(k, al);
    }
    return true;
  };

  std::unordered_map<VarDecl*, ConstraintI*> conversionOf;
  std::vector<std::pair<ConstraintI*, std::pair<VarDecl*, VarDecl*>>> negations;
  for (auto& item : m) {
    auto* ci = item->dynamicCast<ConstraintI>();
    if (ci == nullptr || ci->removed()) {
      continue;
    }
    auto* c = Expression::dynamicCast<Call>(ci->e());
    if (c == nullptr) {
      continue;
    }
    if (isConversion(c)) {
      auto* bd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(c->arg(0)));
      if (bd != nullptr && bd->type().isvarbool()) {
        conversionOf.emplace(bd, ci);
      }
      continue;
    }
    if (c->id() != ids.fznso.bool_lin_eq || !Expression::equal(c->arg(2), IntLit::a(1))) {
      continue;
    }
    auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
    auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
    if (alC == nullptr || alX == nullptr || alC->size() != 2 || alX->size() != 2 ||
        !Expression::isa<IntLit>((*alC)[0]) || !Expression::isa<IntLit>((*alC)[1]) ||
        IntLit::v(Expression::cast<IntLit>((*alC)[0])) != 1 ||
        IntLit::v(Expression::cast<IntLit>((*alC)[1])) != 1 ||
        !Expression::isa<Id>((*alX)[0]) || !Expression::isa<Id>((*alX)[1])) {
      continue;
    }
    negations.emplace_back(ci, std::make_pair(Expression::cast<Id>((*alX)[0])->decl(),
                                              Expression::cast<Id>((*alX)[1])->decl()));
  }

  for (auto& n : negations) {
    ConstraintI* row = n.first;
    if (row->removed()) {
      continue;
    }
    for (int side = 0; side < 2; side++) {
      VarDecl* keep = side == 0 ? n.second.first : n.second.second;
      VarDecl* drop = side == 0 ? n.second.second : n.second.first;
      if (keep == drop || drop->e() != nullptr || is_output(drop) ||
          env.varOccurrences.usages(drop).second) {
        continue;
      }
      std::vector<ConstraintI*> flipRows;
      std::vector<ConstraintI*> flipClauses;
      std::vector<ConstraintI*> intRows;
      ConstraintI* conversion = nullptr;
      VarDecl* dropInt = nullptr;
      VarDecl* keepInt = nullptr;
      bool ok = true;
      auto occ = env.varOccurrences.itemMap.find(drop->id());
      if (!occ.first) {
        continue;
      }
      for (auto* item : *occ.second) {
        auto* ci = item->dynamicCast<ConstraintI>();
        if (ci == nullptr) {
          ok = false;
          break;
        }
        if (ci->removed() || ci == row) {
          continue;
        }
        auto* c = Expression::dynamicCast<Call>(ci->e());
        if (c == nullptr) {
          ok = false;
          break;
        }
        if (isBoolLin(c) && Expression::type(c->arg(2)).isPar()) {
          flipRows.push_back(ci);
        } else if (isClause(c)) {
          flipClauses.push_back(ci);
        } else if (isConversion(c) && conversion == nullptr) {
          conversion = ci;
        } else {
          ok = false;
          break;
        }
      }
      if (!ok) {
        continue;
      }
      if (conversion != nullptr) {
        auto ck = conversionOf.find(keep);
        auto* cc = Expression::cast<Call>(conversion->e());
        dropInt = Expression::dynamicCast<VarDecl>(follow_id_to_decl(cc->arg(1)));
        if (ck == conversionOf.end() || ck->second->removed() || dropInt == nullptr ||
            dropInt->e() != nullptr || is_output(dropInt) ||
            env.varOccurrences.usages(dropInt).second) {
          continue;
        }
        keepInt = Expression::dynamicCast<VarDecl>(
            follow_id_to_decl(Expression::cast<Call>(ck->second->e())->arg(1)));
        if (keepInt == nullptr || keepInt == dropInt) {
          continue;
        }
        auto iocc = env.varOccurrences.itemMap.find(dropInt->id());
        if (!iocc.first) {
          continue;
        }
        for (auto* item : *iocc.second) {
          auto* ci = item->dynamicCast<ConstraintI>();
          if (ci == nullptr) {
            ok = false;
            break;
          }
          if (ci->removed() || ci == conversion) {
            continue;
          }
          auto* c = Expression::dynamicCast<Call>(ci->e());
          if (c == nullptr || !isIntLin(c) || !Expression::type(c->arg(2)).isPar()) {
            ok = false;
            break;
          }
          intRows.push_back(ci);
        }
        if (!ok) {
          continue;
        }
      }

      GCLock lock;
      bool done = true;
      for (ConstraintI* ci : flipRows) {
        done = done && flipTerm(ci, drop, keep);
      }
      for (ConstraintI* ci : flipClauses) {
        done = done && flipLiteral(ci, drop, keep);
      }
      for (ConstraintI* ci : intRows) {
        done = done && flipTerm(ci, dropInt, keepInt);
      }
      if (!done) {
        break;  // partially rewritten rows stay valid; just stop here
      }
      for (ConstraintI* ci : flipRows) {
        env.varOccurrences.remove(drop, ci);
        env.varOccurrences.add(keep, ci);
      }
      for (ConstraintI* ci : flipClauses) {
        env.varOccurrences.remove(drop, ci);
        env.varOccurrences.add(keep, ci);
      }
      for (ConstraintI* ci : intRows) {
        env.varOccurrences.remove(dropInt, ci);
        env.varOccurrences.add(keepInt, ci);
      }
      if (conversion != nullptr) {
        CollectDecls cdConv(env, env.varOccurrences, deletedVarDecls, conversion);
        top_down(cdConv, conversion->e());
        conversion->remove();
        deletedVarDecls.push_back(dropInt);
      }
      CollectDecls cdRow(env, env.varOccurrences, deletedVarDecls, row);
      top_down(cdRow, row->e());
      row->remove();
      deletedVarDecls.push_back(drop);
      break;
    }
  }
}

/// Point a big-M row's indicator at the Boolean that implies it.
///
/// `b -> c` reaches the flattener as `clause([r], [b])` with `c` half-reified
/// onto a fresh `r`, so a library that states the half-reification as a row
/// gets `r` in the row and a clause tying `r` to `b`. Where `r` is read only as
/// that row's indicator, the row can read `b` instead and the clause goes.
///
/// Sound only because the substitution can only *lower* the indicator — `b`
/// implies `r`, not the other way about — and a row is relaxed by lowering an
/// indicator whose coefficient is non-negative. A `_imp_not` row carries a
/// negative one and is excluded, as is any equality: those are tightened
/// instead, which loses solutions.
/// Substitute an offset alias into the rows that read it.
///
/// `x - y = k` says that `y` is `x - k`, and a row holding `a * y` says the same
/// thing holding `a * x` with `a * k` moved to the other side. The registry
/// types an argument as a variable, so a library naming `p[d] - k` in order to
/// hand it to `int_abs` leaves one of these behind — and `-Glinear` never names
/// it, because for it the shift folds into every row that reads it.
void fznso_substitute_offsets(EnvI& env, Model& m, std::vector<VarDecl*>& deletedVarDecls) {
  const auto& ids = env.constants.ids;
  auto isIntLin = [&](const Call* c) {
    return c->id() == ids.fznso.int_lin_le || c->id() == ids.fznso.int_lin_eq ||
           c->id() == ids.fznso.int_lin_ne || c->id() == ids.int_.lin_le ||
           c->id() == ids.int_.lin_eq || c->id() == ids.int_.lin_ne;
  };

  /// Rewrite one row, replacing \a from by \a to where `from = to + shift`.
  auto shiftTerm = [&](ConstraintI* ci, VarDecl* from, VarDecl* to, IntVal shift) {
    auto* c = Expression::cast<Call>(ci->e());
    auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
    auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
    if (alC == nullptr || alX == nullptr || alC->size() != alX->size()) {
      return false;
    }
    std::vector<Expression*> coeffs(alC->size());
    std::vector<Expression*> vars(alX->size());
    IntVal rhs = eval_int(env, c->arg(2));
    for (unsigned int j = 0; j < alX->size(); j++) {
      coeffs[j] = (*alC)[j];
      vars[j] = (*alX)[j];
      if (Expression::dynamicCast<VarDecl>(follow_id_to_decl((*alX)[j])) != from) {
        continue;
      }
      if (!Expression::isa<IntLit>((*alC)[j])) {
        return false;
      }
      vars[j] = to->id();
      rhs -= IntLit::v(Expression::cast<IntLit>((*alC)[j])) * shift;
    }
    auto* nc = new ArrayLit(Location().introduce(), coeffs);
    nc->type(Type::parint(1));
    auto* nv = new ArrayLit(Location().introduce(), vars);
    Type vt = alX->type();
    vt.dim(1);
    nv->type(vt);
    c->arg(0, nc);
    c->arg(1, nv);
    c->arg(2, IntLit::a(rhs));
    return true;
  };

  struct Alias {
    ConstraintI* row;
    VarDecl* v[2];
    IntVal shift[2];  // v[i] = v[1 - i] + shift[i]
  };
  std::vector<Alias> aliases;
  for (auto& item : m) {
    auto* ci = item->dynamicCast<ConstraintI>();
    if (ci == nullptr || ci->removed()) {
      continue;
    }
    auto* c = Expression::dynamicCast<Call>(ci->e());
    if (c == nullptr || (c->id() != ids.fznso.int_lin_eq && c->id() != ids.int_.lin_eq) ||
        !Expression::type(c->arg(2)).isPar()) {
      continue;
    }
    auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
    auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
    if (alC == nullptr || alX == nullptr || alC->size() != 2 || alX->size() != 2 ||
        !Expression::isa<IntLit>((*alC)[0]) || !Expression::isa<IntLit>((*alC)[1])) {
      continue;
    }
    IntVal a0 = IntLit::v(Expression::cast<IntLit>((*alC)[0]));
    IntVal a1 = IntLit::v(Expression::cast<IntLit>((*alC)[1]));
    if (a0 + a1 != 0 || (a0 != 1 && a0 != -1)) {
      continue;
    }
    auto* v0 = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*alX)[0]));
    auto* v1 = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*alX)[1]));
    if (v0 == nullptr || v1 == nullptr || v0 == v1 || !v0->type().isvarint() ||
        !v1->type().isvarint()) {
      continue;
    }
    // `a0 * v0 - a0 * v1 = k`, so `v0 = v1 + k / a0` and `v1 = v0 - k / a0`.
    IntVal k = eval_int(env, c->arg(2)) / a0;
    aliases.push_back({ci, {v0, v1}, {k, -k}});
  }

  for (auto& alias : aliases) {
    if (alias.row->removed()) {
      continue;
    }
    for (int side = 0; side < 2; side++) {
      VarDecl* drop = alias.v[side];
      VarDecl* keep = alias.v[1 - side];
      IntVal shift = alias.shift[side];
      if (drop->e() != nullptr || is_output(drop) || keep->ti()->domain() == nullptr ||
          drop->ti()->domain() == nullptr || env.varOccurrences.usages(drop).second) {
        continue;
      }
      // Dropping the row would lose whatever `drop`'s own domain said about
      // `keep`, so say it on `keep` first: `keep` is `drop - shift`.
      IntSetVal* keepDom = eval_intset(env, keep->ti()->domain());
      IntSetVal* dropDom = eval_intset(env, drop->ti()->domain());
      if (keepDom->empty() || dropDom->empty()) {
        continue;
      }
      std::vector<IntSetVal::Range> moved;
      for (unsigned int r = 0; r < dropDom->size(); r++) {
        if (!dropDom->min(r).isFinite() || !dropDom->max(r).isFinite()) {
          moved.clear();
          break;
        }
        moved.emplace_back(dropDom->min(r) - shift, dropDom->max(r) - shift);
      }
      if (moved.empty()) {
        continue;
      }
      IntSetVal* narrowed = LinearTraits<IntLit>::intersectDomain(keepDom, IntSetVal::a(moved));
      if (narrowed->empty()) {
        continue;  // let the ordinary propagation report this
      }
      std::vector<ConstraintI*> rows;
      bool ok = true;
      auto occ = env.varOccurrences.itemMap.find(drop->id());
      if (!occ.first) {
        continue;
      }
      for (auto* used : *occ.second) {
        auto* ci = used->dynamicCast<ConstraintI>();
        if (ci == nullptr) {
          ok = false;
          break;
        }
        if (ci->removed() || ci == alias.row) {
          continue;
        }
        auto* c = Expression::dynamicCast<Call>(ci->e());
        if (c == nullptr || !isIntLin(c) || !Expression::type(c->arg(2)).isPar()) {
          ok = false;
          break;
        }
        // A row already naming `keep` would end up naming it twice.
        auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
        if (alX == nullptr) {
          ok = false;
          break;
        }
        for (unsigned int j = 0; j < alX->size() && ok; j++) {
          ok = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*alX)[j])) != keep;
        }
        if (!ok) {
          break;
        }
        rows.push_back(ci);
      }
      if (!ok) {
        continue;
      }
      GCLock lock;
      bool done = true;
      for (ConstraintI* ci : rows) {
        done = done && shiftTerm(ci, drop, keep, shift);
      }
      if (!done) {
        break;  // partially rewritten rows stay valid; just stop here
      }
      for (ConstraintI* ci : rows) {
        env.varOccurrences.remove(drop, ci);
        env.varOccurrences.add(keep, ci);
      }
      if (!narrowed->equal(keepDom)) {
        keep->ti()->domain(new SetLit(Location().introduce(), narrowed));
        keep->ti()->setComputedDomain(false);
      }
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, alias.row);
      top_down(cd, alias.row->e());
      alias.row->remove();
      deletedVarDecls.push_back(drop);
      break;
    }
  }
}

void fznso_retarget_indicators(EnvI& env, Model& m, std::vector<VarDecl*>& deletedVarDecls) {
  const auto& ids = env.constants.ids;
  auto isConversion = [&](const Call* c) {
    return c->id() == ids.bool2int || c->id() == ids.fznso.bool2int;
  };

  /// Whether every row reading \a iv is relaxed by lowering it.
  auto loweringIsSafe = [&](VarDecl* iv, ConstraintI* conversion) {
    if (is_output(iv) || env.varOccurrences.usages(iv).second) {
      return false;
    }
    auto occ = env.varOccurrences.itemMap.find(iv->id());
    if (!occ.first) {
      return false;
    }
    for (auto* item : *occ.second) {
      auto* ci = item->dynamicCast<ConstraintI>();
      if (ci == nullptr) {
        return false;
      }
      if (ci->removed() || ci == conversion) {
        continue;
      }
      auto* c = Expression::dynamicCast<Call>(ci->e());
      if (c == nullptr ||
          !(c->id() == ids.int_.lin_le || c->id() == ids.fznso.int_lin_le ||
            c->id() == ids.float_.lin_le || c->id() == ids.fznso.float_lin_le)) {
        return false;
      }
      auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
      auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
      if (alC == nullptr || alX == nullptr || alC->size() != alX->size()) {
        return false;
      }
      for (unsigned int j = 0; j < alX->size(); j++) {
        if (Expression::dynamicCast<VarDecl>(follow_id_to_decl((*alX)[j])) != iv) {
          continue;
        }
        if (Expression::isa<IntLit>((*alC)[j])) {
          if (IntLit::v(Expression::cast<IntLit>((*alC)[j])) < 0) {
            return false;
          }
        } else if (Expression::isa<FloatLit>((*alC)[j])) {
          if (FloatLit::v(Expression::cast<FloatLit>((*alC)[j])) < 0.0) {
            return false;
          }
        } else {
          return false;
        }
      }
    }
    return true;
  };

  // `bool2int(b, i)`, keyed by the Boolean.
  std::unordered_map<VarDecl*, ConstraintI*> conversionOf;
  std::vector<ConstraintI*> implications;
  for (auto& item : m) {
    auto* ci = item->dynamicCast<ConstraintI>();
    if (ci == nullptr || ci->removed()) {
      continue;
    }
    auto* c = Expression::dynamicCast<Call>(ci->e());
    if (c == nullptr) {
      continue;
    }
    if (isConversion(c)) {
      auto* bd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(c->arg(0)));
      if (bd != nullptr && bd->type().isvarbool()) {
        conversionOf.emplace(bd, ci);
      }
    } else if (c->id() == ids.clause || c->id() == ids.fznso.bool_clause) {
      implications.push_back(ci);
    }
  }

  for (ConstraintI* ci : implications) {
    if (ci->removed()) {
      continue;
    }
    auto* c = Expression::cast<Call>(ci->e());
    auto* pos = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
    auto* neg = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
    if (pos == nullptr || neg == nullptr || pos->size() != 1 || neg->size() != 1 ||
        !Expression::isa<Id>((*pos)[0]) || !Expression::isa<Id>((*neg)[0])) {
      continue;
    }
    VarDecl* implied = Expression::cast<Id>((*pos)[0])->decl();
    VarDecl* antecedent = Expression::cast<Id>((*neg)[0])->decl();
    if (implied == antecedent || implied->e() != nullptr || is_output(implied) ||
        env.varOccurrences.usages(implied).second) {
      continue;
    }
    auto conv = conversionOf.find(implied);
    if (conv == conversionOf.end() || conv->second->removed()) {
      continue;
    }
    // The implied Boolean may be read only by this clause and its conversion.
    if (env.varOccurrences.usages(implied).first != 2) {
      continue;
    }
    auto* cc = Expression::cast<Call>(conv->second->e());
    auto* iv = Expression::dynamicCast<VarDecl>(follow_id_to_decl(cc->arg(1)));
    if (iv == nullptr || !loweringIsSafe(iv, conv->second)) {
      continue;
    }
    GCLock lock;
    env.varOccurrences.remove(implied, conv->second);
    cc->arg(0, antecedent->id());
    env.varOccurrences.add(antecedent, conv->second);
    CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ci);
    top_down(cd, ci->e());
    ci->remove();
    deletedVarDecls.push_back(implied);
  }
}

void optimize(Env& env, bool chain_compression) {
  env.envi().checkCancel();

  if (env.envi().failed()) {
    return;
  }
  try {
    EnvI& envi = env.envi();
    Model& m = *envi.flat();
    std::vector<unsigned int> toAssignBoolVars;
    std::vector<unsigned int> toRemoveConstraints;
    std::vector<VarDecl*> deletedVarDecls;
    // The negation of each variable seen so far, from a row saying `x + y = 1`.
    // A second such row about the same `x` names the same negation again, and
    // the two names are unified rather than both kept.
    std::unordered_map<VarDecl*, VarDecl*> negationOf;
    // The Boolean each integer conversion was first seen for. Two conversions
    // onto one integer say their Booleans are equal, which is how a second name
    // for one Boolean survives: the integers were unified but the Booleans were
    // not, so every negation and every row stated about either is kept twice.
    std::unordered_map<VarDecl*, VarDecl*> boolOfInt;

    // Queue of constraint and variable items that still need to be optimised
    std::deque<Item*> constraintQueue;
    // Queue of variable declarations (indexes into the model) that still need to be optimised
    std::deque<unsigned int> vardeclQueue;

    std::vector<unsigned int> boolConstraints;

    GCLock lock;

    // Phase 0: clean up
    // - clear flags for all constraint and variable declaration items
    //   (flags are used to indicate whether an item is already queued or not)
    for (auto& i : m) {
      if (!i->removed()) {
        if (auto* ci = i->dynamicCast<ConstraintI>()) {
          ci->flag(false);
        } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
          vdi->flag(false);
        }
      }
    }

    envi.checkCancel();

    // `a \/ not b` by the pair of variables it names, so that meeting the
    // converse identifies the two.
    std::map<std::pair<VarDecl*, VarDecl*>, ConstraintI*> binaryClauses;


    // Phase 1: initialise queues
    //  - remove equality constraints between identifiers
    //  - remove toplevel forall constraints
    //  - collect exists, clauses and reified foralls in boolConstraints
    //  - remove "constraint x" where x is a bool var
    //  - unify variables that are assigned to an identifier
    //  - push bool vars that are fixed and have a RHS (to propagate the RHS constraint)
    //  - push int/float vars that are fixed (either have a RHS or a singleton domain)
    for (unsigned int i = 0; i < m.size(); i++) {
      env.envi().checkCancel();
      if (m[i]->removed()) {
        continue;
      }

      if (auto* ci = m[i]->dynamicCast<ConstraintI>()) {
        ci->flag(false);
        if (!ci->removed()) {
          if (Call* c = Expression::dynamicCast<Call>(ci->e())) {
            bool parHolds = false;
            const bool decidedPar = fznso_par_constraint(envi, c, parHolds);
            if (decidedPar) {
              // Born with every argument already a literal, so nothing will
              // ever queue it: decide it here or it reaches the solver as a row
              // over nothing but constants.
              if (!parHolds) {
                env.envi().fail();
              }
              toRemoveConstraints.push_back(i);
            } else if ((c->id() == envi.constants.ids.int_.eq ||
                        c->id() == envi.constants.ids.bool_.eq ||
                 c->id() == envi.constants.ids.float_.eq ||
                 c->id() == envi.constants.ids.set_.eq ||
                 c->id() == envi.constants.ids.fznso.set_eq) &&
                Expression::isa<Id>(c->arg(0)) && Expression::isa<Id>(c->arg(1)) &&
                (Expression::cast<Id>(c->arg(0))->decl()->e() == nullptr ||
                 Expression::cast<Id>(c->arg(1))->decl()->e() == nullptr ||
                 // Both defined: one definition becomes a constraint so that
                 // the variables can still be unified. Evaluated last, so it
                 // only runs when neither side is already free.
                 demote_definition(envi, Expression::cast<Id>(c->arg(0))->decl()))) {
              // Equality constraint between two identifiers: unify

              if (Call* defVar = Expression::ann(c).getCall(envi.constants.ann.defines_var)) {
                // First, remove defines_var/is_defined_var annotations if present
                if (Expression::equal(defVar->arg(0), c->arg(0))) {
                  Expression::ann(Expression::cast<Id>(c->arg(0))->decl())
                      .remove(envi.constants.ann.is_defined_var);
                } else {
                  Expression::ann(Expression::cast<Id>(c->arg(1))->decl())
                      .remove(envi.constants.ann.is_defined_var);
                }
              }
              unify(envi, deletedVarDecls, Expression::cast<Id>(c->arg(0)),
                    Expression::cast<Id>(c->arg(1)));
              {
                VarDecl* vd = Expression::cast<Id>(c->arg(0))->decl();
                int v0idx = envi.varOccurrences.find(vd);
                push_vardecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
              }

              push_dependent_constraints(envi, Expression::cast<Id>(c->arg(0)), constraintQueue);
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
              top_down(cd, c);
              ci->e(envi.constants.literalTrue);
              ci->remove();
            } else if ((c->id() == envi.constants.ids.int_.eq ||
                        c->id() == envi.constants.ids.bool_.eq ||
                        c->id() == envi.constants.ids.float_.eq ||
                        c->id() == envi.constants.ids.set_.eq ||
                        c->id() == envi.constants.ids.fznso.set_eq) &&
                       ((Expression::isa<Id>(c->arg(0)) &&
                         Expression::cast<Id>(c->arg(0))->decl()->e() == nullptr &&
                         Expression::type(c->arg(1)).isPar()) ||
                        (Expression::isa<Id>(c->arg(1)) &&
                         Expression::cast<Id>(c->arg(1))->decl()->e() == nullptr &&
                         Expression::type(c->arg(0)).isPar()))) {
              // equality constraint with one fixed var can be resolved later by
              // simplify_bool_constraint
              auto* id = Expression::cast<Id>(c->arg(Expression::isa<Id>(c->arg(0)) ? 0 : 1));
              int idx = envi.varOccurrences.find(id->decl());
              push_vardecl(envi, m[idx]->cast<VarDeclI>(), idx, vardeclQueue);
              push_dependent_constraints(envi, id, constraintQueue);
            } else if ((c->id() == envi.constants.ids.int_.lin_eq ||
                        c->id() == envi.constants.ids.fznso.int_lin_eq ||
                        // A library that says `a = b` over two Booleans in the
                        // registry's vocabulary says it with this, and two
                        // names for one variable should be unified rather than
                        // held equal by a row.
                        c->id() == envi.constants.ids.fznso.bool_lin_eq) &&
                       Expression::equal(c->arg(2), IntLit::a(0))) {
              auto* al_c = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
              if (al_c->size() == 2 && IntLit::v(Expression::cast<IntLit>((*al_c)[0])) ==
                                           -IntLit::v(Expression::cast<IntLit>((*al_c)[1]))) {
                auto* al_x = Expression::cast<ArrayLit>(follow_id(c->arg(1)));
                if (Expression::isa<Id>((*al_x)[0]) && Expression::isa<Id>((*al_x)[1]) &&
                    (Expression::cast<Id>((*al_x)[0])->decl()->e() == nullptr ||
                     Expression::cast<Id>((*al_x)[1])->decl()->e() == nullptr)) {
                  // Equality constraint between two identifiers: unify

                  if (Call* defVar = Expression::ann(c).getCall(envi.constants.ann.defines_var)) {
                    // First, remove defines_var/is_defined_var annotations if present
                    if (Expression::equal(defVar->arg(0), (*al_x)[0])) {
                      Expression::ann(Expression::cast<Id>((*al_x)[0])->decl())
                          .remove(envi.constants.ann.is_defined_var);
                    } else {
                      Expression::ann(Expression::cast<Id>((*al_x)[1])->decl())
                          .remove(envi.constants.ann.is_defined_var);
                    }
                  }
                  unify(envi, deletedVarDecls, Expression::cast<Id>((*al_x)[0]),
                        Expression::cast<Id>((*al_x)[1]));
                  {
                    VarDecl* vd = Expression::cast<Id>((*al_x)[0])->decl();
                    int v0idx = envi.varOccurrences.find(vd);
                    push_vardecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
                  }

                  push_dependent_constraints(envi, Expression::cast<Id>((*al_x)[0]),
                                             constraintQueue);
                  CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
                  top_down(cd, c);
                  ci->e(envi.constants.literalTrue);
                  ci->remove();
                }
              }
            } else if ((c->id() == envi.constants.ids.bool2int ||
                        c->id() == envi.constants.ids.fznso.bool2int) &&
                       (Expression::type(c->arg(0)).isPar() ||
                        Expression::type(c->arg(1)).isPar())) {
              // `bool2int(true, x)` says `x` is 1 and `bool2int(b, 1)` says `b`
              // holds, but nothing else here queues either: neither has an
              // argument that anything is about to decide. Left alone the row
              // reaches the solver, and so does every other row that mentions
              // the variable — with a term for a constant.
              ci->flag(true);
              constraintQueue.push_back(ci);
            } else if (c->id() == envi.constants.ids.bool2int ||
                       c->id() == envi.constants.ids.fznso.bool2int) {
              auto* bd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(c->arg(0)));
              auto* id = Expression::dynamicCast<VarDecl>(follow_id_to_decl(c->arg(1)));
              if (bd != nullptr && id != nullptr && bd->type().isvarbool()) {
                auto seen = boolOfInt.find(id);
                if (seen == boolOfInt.end()) {
                  boolOfInt.emplace(id, bd);
                } else if (seen->second != bd && seen->second->e() == nullptr &&
                           bd->e() == nullptr) {
                  unify(envi, deletedVarDecls, bd->id(), seen->second->id());
                  push_dependent_constraints(envi, seen->second->id(), constraintQueue);
                  CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
                  top_down(cd, c);
                  ci->e(envi.constants.literalTrue);
                  ci->remove();
                }
              }
            } else if ((c->id() == envi.constants.ids.int_.lin_eq ||
                        c->id() == envi.constants.ids.fznso.int_lin_eq ||
                        c->id() == envi.constants.ids.fznso.bool_lin_eq) &&
                       Expression::equal(c->arg(2), IntLit::a(1))) {
              // `x + y = 1` over two 0/1 variables says `y` is the negation of
              // `x`. FlatZinc has no negated literal, so a library that needs
              // one states this row — and a model that needs the same negation
              // in six places states it six times, over six variables that are
              // all the same. Keep the first and unify the rest onto it.
              auto* al_c = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
              auto* al_x = al_c != nullptr && al_c->size() == 2
                               ? Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)))
                               : nullptr;
              if (al_x != nullptr && al_x->size() == 2 &&
                  Expression::isa<IntLit>((*al_c)[0]) && Expression::isa<IntLit>((*al_c)[1]) &&
                  IntLit::v(Expression::cast<IntLit>((*al_c)[0])) == 1 &&
                  IntLit::v(Expression::cast<IntLit>((*al_c)[1])) == 1 &&
                  Expression::isa<Id>((*al_x)[0]) && Expression::isa<Id>((*al_x)[1])) {
                auto* xd = Expression::cast<Id>((*al_x)[0])->decl();
                auto* yd = Expression::cast<Id>((*al_x)[1])->decl();
                // Both sides have to be free 0/1 variables of the same type:
                // one may be substituted for the other.
                auto zeroOne = [&](VarDecl* vd) {
                  if (vd->e() != nullptr) {
                    return false;
                  }
                  if (vd->type().isvarbool()) {
                    return vd->ti()->domain() == nullptr;
                  }
                  if (!vd->type().isvarint() || vd->ti()->domain() == nullptr) {
                    return false;
                  }
                  IntSetVal* d = eval_intset(envi, vd->ti()->domain());
                  return !d->empty() && d->min() == 0 && d->max() == 1;
                };
                if (xd != yd && xd->type() == yd->type() && zeroOne(xd) && zeroOne(yd)) {
                  auto seen = negationOf.find(xd);
                  auto other = seen != negationOf.end() ? seen->second : nullptr;
                  if (other == nullptr) {
                    seen = negationOf.find(yd);
                    if (seen != negationOf.end()) {
                      other = seen->second;
                      std::swap(xd, yd);
                    }
                  }
                  if (other != nullptr && other != yd) {
                    unify(envi, deletedVarDecls, yd->id(), other->id());
                    push_dependent_constraints(envi, other->id(), constraintQueue);
                    CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
                    top_down(cd, c);
                    ci->e(envi.constants.literalTrue);
                    ci->remove();
                  } else if (other == nullptr) {
                    negationOf.emplace(xd, yd);
                    negationOf.emplace(yd, xd);
                  }
                }
              }
            } else if (c->id() == envi.constants.ids.forall) {
              // Remove forall constraints, assign variables inside the forall to true

              auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
              for (unsigned int j = al->size(); (j--) != 0U;) {
                if (Id* id = Expression::dynamicCast<Id>((*al)[j])) {
                  if (id->decl()->ti()->domain() == nullptr) {
                    toAssignBoolVars.push_back(
                        *envi.varOccurrences.idx.find(id->decl()->id()).second);
                  } else if (id->decl()->ti()->domain() == envi.constants.literalFalse) {
                    env.envi().fail();
                    id->decl()->e(envi.constants.literalTrue);
                  }
                }  // todo: check else case (fixed bool inside a forall at this stage)
              }
              toRemoveConstraints.push_back(i);
            } else if (c->id() == envi.constants.ids.exists ||
                       (c->id() == envi.constants.ids.clause ||
             c->id() == envi.constants.ids.fznso.bool_clause ||
             c->id() == envi.constants.ids.bool_.clause)) {
              // Add disjunctive constraints to the boolConstraints list

              boolConstraints.push_back(i);
            }
            // A constraint is queued when a variable it mentions is touched, so
            // a row that decides its own variables outright is never looked at.
            // A library states `x = k` as one of these, and left unread it
            // reaches the solver as a row rather than narrowing the variable
            // and folding everything that mentions it. Queue it once; whatever
            // it decides re-queues the rest by the usual route.
            //
            // Outside the chain above, because a branch of it matches the same
            // idents on a shape it then rejects, and would otherwise consume
            // them.
            if (!decidedPar && !ci->flag() &&
                (c->id() == envi.constants.ids.fznso.bool_lin_le ||
                 c->id() == envi.constants.ids.fznso.bool_lin_eq ||
                 c->id() == envi.constants.ids.fznso.int_lin_le ||
                 c->id() == envi.constants.ids.fznso.int_lin_eq ||
                 c->id() == envi.constants.ids.int_.lin_le ||
                 c->id() == envi.constants.ids.int_.lin_eq ||
                 c->id() == envi.constants.ids.bool_.lin_le ||
                 c->id() == envi.constants.ids.bool_.lin_eq)) {
              ci->flag(true);
              constraintQueue.push_back(ci);
            }
          } else if (Id* id = Expression::dynamicCast<Id>(ci->e())) {
            if (id->decl()->ti()->domain() == envi.constants.literalFalse) {
              env.envi().fail();
              ci->e(envi.constants.literalFalse);
            } else {
              if (id->decl()->ti()->domain() == nullptr) {
                toAssignBoolVars.push_back(*envi.varOccurrences.idx.find(id->decl()->id()).second);
              }
              toRemoveConstraints.push_back(i);
            }
          }
        }
      } else if (auto* vdi = m[i]->dynamicCast<VarDeclI>()) {
        vdi->flag(false);
        if ((vdi->e()->e() != nullptr) && Expression::isa<Id>(vdi->e()->e()) &&
            Expression::type(vdi->e()).dim() == 0) {
          // unify variable with the identifier it's assigned to
          Id* id1 = Expression::cast<Id>(vdi->e()->e());
          vdi->e()->e(nullptr);

          // Transfer is_defined_var annotation
          if (Expression::ann(id1->decl()).contains(envi.constants.ann.is_defined_var)) {
            Expression::addAnnotation(vdi->e(), envi.constants.ann.is_defined_var);
          } else if (Expression::ann(vdi->e()).contains(envi.constants.ann.is_defined_var)) {
            Expression::addAnnotation(id1->decl(), envi.constants.ann.is_defined_var);
          }

          unify(envi, deletedVarDecls, vdi->e()->id(), id1);
          push_dependent_constraints(envi, id1, constraintQueue);
        }
        if (vdi->e()->type().isbool() && vdi->e()->type().dim() == 0 &&
            (vdi->e()->ti()->domain() == envi.constants.literalTrue ||
             vdi->e()->ti()->domain() == envi.constants.literalFalse ||
             (vdi->e()->e() != nullptr && Expression::isa<BoolLit>(vdi->e()->e())))) {
          // push RHS onto constraint queue since this bool var is fixed
          push_vardecl(envi, vdi, i, vardeclQueue);
          push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
        }
        if (Call* c = Expression::dynamicCast<Call>(vdi->e()->e())) {
          if (c->id() == envi.constants.ids.forall || c->id() == envi.constants.ids.exists ||
              (c->id() == envi.constants.ids.clause ||
             c->id() == envi.constants.ids.fznso.bool_clause ||
             c->id() == envi.constants.ids.bool_.clause)) {
            // push reified foralls, exists, clauses
            boolConstraints.push_back(i);
          } else if ((c->id() == envi.constants.ids.bool2int ||
                      c->id() == envi.constants.ids.fznso.bool2int) &&
                     Expression::type(c->arg(0)).isPar()) {
            // `bool2int(true)` defines a 0/1 variable that is simply 1, and
            // nothing else here queues it: its domain is not a singleton and
            // its right-hand side is a call rather than a literal. Left alone
            // it reaches the solver as a column, and every row that mentions
            // it keeps a term for a constant.
            vdi->flag(true);
            constraintQueue.push_back(vdi);
          }
        }
        if (vdi->e()->type().isint()) {
          if (((vdi->e()->e() != nullptr) && Expression::isa<IntLit>(vdi->e()->e())) ||
              ((vdi->e()->ti()->domain() != nullptr) &&
               Expression::isa<SetLit>(vdi->e()->ti()->domain()) &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->isv()->size() == 1 &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->isv()->min() ==
                   Expression::cast<SetLit>(vdi->e()->ti()->domain())->isv()->max())) {
            // Variable is assigned an integer, or has a singleton domain
            push_vardecl(envi, vdi, i, vardeclQueue);
            push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
          }
        }
        if (vdi->e()->type().isfloat()) {
          if (((vdi->e()->e() != nullptr) && Expression::isa<FloatLit>(vdi->e()->e())) ||
              ((vdi->e()->ti()->domain() != nullptr) &&
               Expression::isa<SetLit>(vdi->e()->ti()->domain()) &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->fsv()->size() == 1 &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->fsv()->min() ==
                   Expression::cast<SetLit>(vdi->e()->ti()->domain())->fsv()->max())) {
            // Variable is assigned a float, or has a singleton domain
            push_vardecl(envi, vdi, i, vardeclQueue);
            push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
          }
        }
        if (vdi->e()->type().isIntSet()) {
          if (vdi->e()->e() != nullptr && Expression::isa<SetLit>(vdi->e()->e())) {
            // Set variable is assigned a literal
            push_vardecl(envi, vdi, i, vardeclQueue);
            push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
          }
        }
      }
    }

    // Phase 2: handle boolean constraints
    //  - check if any boolean constraint is subsumed (e.g. a fixed false in a forall, or a fixed
    //  true in a disjunction)
    //  - check if any boolean constraint has a single non-fixed literal left, then fix that literal
    for (auto i = static_cast<unsigned int>(boolConstraints.size()); (i--) != 0U;) {
      env.envi().checkCancel();

      Item* bi = m[boolConstraints[i]];
      if (bi->removed()) {
        continue;
      }
      Call* c;

      if (bi->isa<ConstraintI>()) {
        c = Expression::dynamicCast<Call>(bi->cast<ConstraintI>()->e());
      } else {
        c = Expression::dynamicCast<Call>(bi->cast<VarDeclI>()->e()->e());
      }
      if (c == nullptr) {
        continue;
      }
      bool isConjunction = (c->id() == envi.constants.ids.forall);
      bool subsumed = false;
      Id* finalId = nullptr;
      bool finalIdNeg = false;
      int idCount = 0;
      std::vector<VarDecl*> pos;
      std::vector<VarDecl*> neg;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        bool unit = (j == 0 ? isConjunction : !isConjunction);
        auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(j)));
        for (unsigned int k = 0; k < al->size(); k++) {
          if (Id* ident = Expression::dynamicCast<Id>((*al)[k])) {
            if ((ident->decl()->ti()->domain() != nullptr) ||
                ((ident->decl()->e() != nullptr) && Expression::type(ident->decl()->e()).isPar())) {
              bool identValue = ident->decl()->ti()->domain() != nullptr
                                    ? eval_bool(envi, ident->decl()->ti()->domain())
                                    : eval_bool(envi, ident->decl()->e());
              if (identValue != unit) {
                subsumed = true;
                goto subsumed_check_done;
              }
            } else {
              idCount++;
              finalId = ident;
              finalIdNeg = (j == 1);
              if (j == 0) {
                pos.push_back(ident->decl());
              } else {
                neg.push_back(ident->decl());
              }
            }
          } else {
            if (Expression::cast<BoolLit>((*al)[k])->v() != unit) {
              subsumed = true;
              goto subsumed_check_done;
            }
          }
        }
      }
      if (!pos.empty() && !neg.empty()) {
        std::sort(pos.begin(), pos.end());
        std::sort(neg.begin(), neg.end());
        unsigned int ix = 0;
        unsigned int iy = 0;
        for (;;) {
          if (pos[ix] == neg[iy]) {
            subsumed = true;
            break;
          }
          if (pos[ix] < neg[iy]) {
            ix++;
          } else {
            iy++;
          }
          if (ix == pos.size() || iy == neg.size()) {
            break;
          }
        }
      }

    subsumed_check_done:
      if (subsumed) {
        if (isConjunction) {
          if (bi->isa<ConstraintI>()) {
            env.envi().fail();
          } else {
            if (bi->cast<VarDeclI>()->e()->ti()->domain() != nullptr) {
              if (eval_bool(envi, bi->cast<VarDeclI>()->e()->ti()->domain())) {
                envi.fail();
              }
            } else {
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
              top_down(cd, bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(envi.constants.literalFalse);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(envi.constants.literalFalse);
              push_vardecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              push_dependent_constraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        } else {
          if (bi->isa<ConstraintI>()) {
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<ConstraintI>()->e());
            bi->remove();
          } else {
            if (bi->cast<VarDeclI>()->e()->ti()->domain() != nullptr) {
              if (!eval_bool(envi, bi->cast<VarDeclI>()->e()->ti()->domain())) {
                envi.fail();
              }
            } else {
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
              top_down(cd, bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(envi.constants.literalTrue);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(envi.constants.literalTrue);
              push_vardecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              push_dependent_constraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        }
      } else if (idCount == 1 && bi->isa<ConstraintI>()) {
        assert(finalId->decl()->ti()->domain() == nullptr);
        finalId->decl()->ti()->domain(envi.constants.boollit(!finalIdNeg));
        if (finalId->decl()->e() == nullptr) {
          finalId->decl()->e(envi.constants.boollit(!finalIdNeg));
        }
        CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
        top_down(cd, bi->cast<ConstraintI>()->e());
        bi->remove();
        push_vardecl(envi, *envi.varOccurrences.idx.find(finalId->decl()->id()).second,
                     vardeclQueue);
        push_dependent_constraints(envi, finalId, constraintQueue);
      }  // todo: for var decls, we could unify the variable with the remaining finalId (the RHS)
    }

    // Fix all bool vars in toAssignBoolVars to true and push their declarations and constraints
    for (unsigned int i = static_cast<int>(toAssignBoolVars.size()); (i--) != 0U;) {
      if (m[toAssignBoolVars[i]]->removed()) {
        continue;
      }
      auto* vdi = m[toAssignBoolVars[i]]->cast<VarDeclI>();
      if (vdi->e()->ti()->domain() == nullptr) {
        vdi->e()->ti()->domain(envi.constants.literalTrue);
        push_vardecl(envi, vdi, toAssignBoolVars[i], vardeclQueue);
        push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
      }
    }

    // Phase 3: fixpoint of constraint and variable simplification

    std::unordered_map<Expression*, int> nonFixedLiteralCount;
    while (!vardeclQueue.empty() || !constraintQueue.empty()) {
      while (!vardeclQueue.empty()) {
        env.envi().checkCancel();

        unsigned int var_idx = vardeclQueue.front();
        vardeclQueue.pop_front();
        m[var_idx]->cast<VarDeclI>()->flag(false);
        VarDecl* vd = m[var_idx]->cast<VarDeclI>()->e();

        if (vd->type().isbool() && (vd->ti()->domain() != nullptr)) {
          bool isTrue = vd->ti()->domain() == envi.constants.literalTrue;
          bool remove = false;
          if (vd->e() != nullptr) {
            if (Id* id = Expression::dynamicCast<Id>(vd->e())) {
              // Variable assigned to id, so fix id
              if (id->decl()->ti()->domain() == nullptr) {
                id->decl()->ti()->domain(vd->ti()->domain());
                push_vardecl(envi, envi.varOccurrences.idx.get(id->decl()->id()), vardeclQueue);
              } else if (id->decl()->ti()->domain() != vd->ti()->domain()) {
                env.envi().fail();
              }
              remove = true;
            } else if (Call* c = Expression::dynamicCast<Call>(vd->e())) {
              if (isTrue && c->id() == envi.constants.ids.forall) {
                // Reified forall is now fixed to true, so make all elements of the conjunction true
                remove = true;
                auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
                for (unsigned int i = 0; i < al->size(); i++) {
                  if (Id* id = Expression::dynamicCast<Id>((*al)[i])) {
                    if (id->decl()->ti()->domain() == nullptr) {
                      id->decl()->ti()->domain(envi.constants.literalTrue);
                      push_vardecl(envi, envi.varOccurrences.idx.get(id->decl()->id()),
                                   vardeclQueue);
                    } else if (id->decl()->ti()->domain() == envi.constants.literalFalse) {
                      env.envi().fail();
                      remove = true;
                    }
                  }
                }
              } else if (!isTrue && (c->id() == envi.constants.ids.exists ||
                                     (c->id() == envi.constants.ids.clause ||
             c->id() == envi.constants.ids.fznso.bool_clause ||
             c->id() == envi.constants.ids.bool_.clause))) {
                // Reified disjunction is now fixed to false, so make all elements of the
                // disjunction false
                remove = true;
                for (unsigned int i = 0; i < c->argCount(); i++) {
                  bool ispos = i == 0;
                  auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(i)));
                  for (unsigned int j = 0; j < al->size(); j++) {
                    if (Id* id = Expression::dynamicCast<Id>((*al)[j])) {
                      if (id->decl()->ti()->domain() == nullptr) {
                        id->decl()->ti()->domain(envi.constants.boollit(!ispos));
                        push_vardecl(envi, envi.varOccurrences.idx.get(id->decl()->id()),
                                     vardeclQueue);
                      } else if (id->decl()->ti()->domain() == envi.constants.boollit(ispos)) {
                        env.envi().fail();
                        remove = true;
                      }
                    }
                  }
                }
              }
            }
          } else {
            // If bool variable doesn't have a RHS, just remove it
            remove = true;
          }
          push_dependent_constraints(envi, vd->id(), constraintQueue);
          std::vector<Item*> toRemove;
          auto it = envi.varOccurrences.itemMap.find(vd->id()->decl()->id());

          // Handle all boolean constraints that involve this variable
          if (it.first) {
            std::vector<std::pair<Id*, Item*>> toUnsubscribe;
            for (auto* item : *it.second) {
              if (item->removed()) {
                continue;
              }
              if (auto* vdi = item->dynamicCast<VarDeclI>()) {
                // The variable occurs in the RHS of another variable, so
                // if that is an array variable, push it onto the stack for processing
                if (vdi->e()->e() != nullptr && Expression::isa<ArrayLit>(vdi->e()->e())) {
                  push_vardecl(envi, envi.varOccurrences.idx.get(vdi->e()->id()), vardeclQueue);
                  continue;
                }
              }
              // Simplify the constraint *item (which depends on this variable)
              simplify_bool_constraint(envi, item, vd, remove, vardeclQueue, constraintQueue,
                                       toRemove, deletedVarDecls, nonFixedLiteralCount,
                                       toUnsubscribe);
            }
            for (auto& unsubscribe : toUnsubscribe) {
              auto it = envi.varOccurrences.itemMap.find(unsubscribe.first);
              if (it.first) {
                it.second->erase(unsubscribe.second);
              }
            }
          }
          // Actually remove all items that have become unnecessary in the step above
          for (auto i = static_cast<unsigned int>(toRemove.size()); (i--) != 0U;) {
            if (auto* ci = toRemove[i]->dynamicCast<ConstraintI>()) {
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
              top_down(cd, ci->e());
              ci->remove();
            } else {
              auto* vdi = toRemove[i]->cast<VarDeclI>();
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, vdi);
              top_down(cd, vdi->e()->e());
              vdi->e()->e(nullptr);
            }
          }
          if (remove) {
            deletedVarDecls.push_back(vd);
          } else {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        } else if (vd->type().isint() && (vd->ti()->domain() != nullptr)) {
          IntSetVal* isv = eval_intset(envi, vd->ti()->domain());
          if (auto* il = Expression::dynamicCast<IntLit>(vd->e())) {
            auto iv = IntLit::v(il);
            if (!isv->contains(iv)) {
              env.envi().fail();
            } else if (isv->size() != 1 || isv->card() != 1) {
              isv = IntSetVal::a(iv, iv);
              vd->ti()->domain(new SetLit(Location().introduce(), isv));
              push_dependent_constraints(envi, vd->id(), constraintQueue);
            }
          }
          if (isv->size() == 1 && isv->card() == 1) {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        } else if (vd->type().isfloat() && (vd->ti()->domain() != nullptr)) {
          FloatSetVal* fsv = eval_floatset(envi, vd->ti()->domain());
          if (fsv->size() == 1 && fsv->card() == 1) {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        } else if (vd->type().isIntSet() && (vd->e() != nullptr)) {
          simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
        }
      }  // end of processing of variable queue

      // Now handle all non-boolean constraints (i.e. anything except forall, clause, exists)
      bool handledConstraint = false;
      while (!handledConstraint && !constraintQueue.empty()) {
        envi.checkCancel();

        Item* item = constraintQueue.front();
        constraintQueue.pop_front();
        Call* c;
        ArrayLit* al = nullptr;
        if (auto* ci = item->dynamicCast<ConstraintI>()) {
          ci->flag(false);
          c = Expression::dynamicCast<Call>(ci->e());
        } else {
          if (item->removed()) {
            // This variable was removed because of unification, so we look up the
            // variable it was unified to
            item = m[envi.varOccurrences.find(item->cast<VarDeclI>()->e()->id()->decl())]
                       ->cast<VarDeclI>();
          }
          item->cast<VarDeclI>()->flag(false);
          c = Expression::dynamicCast<Call>(item->cast<VarDeclI>()->e()->e());
          al = Expression::dynamicCast<ArrayLit>(item->cast<VarDeclI>()->e()->e());
        }
        if (!item->removed()) {
          if (al != nullptr && !al->type().structBT()) {
            // Substitute all fixed variables by their values in array literals, then
            // push all constraints that depend on the array
            substitute_fixed_vars(envi, item, deletedVarDecls);
            push_dependent_constraints(envi, item->cast<VarDeclI>()->e()->id(), constraintQueue);
          } else {
            // For any constraint that is not forall, exists or clause,
            // substitute fixed arguments, then simplify it
            substitute_fixed_vars(envi, item, deletedVarDecls);
            handledConstraint =
                simplify_constraint(envi, item, deletedVarDecls, constraintQueue, vardeclQueue);
          }
        }
      }
    }

    // `t <= k` and `-t <= -k` together are `t = k`, which the backend takes in
    // one row and which the unification in phase 1 can read. A library states
    // an equality under an indicator as two big-M rows, and each collapses to
    // one of these only once the indicator is decided — separately, by which
    // time nothing is left to put them back together. So this runs after the
    // queue has drained, and feeds what it merges back into it.
    // `a -> b` and `b -> a` together say the two are one variable, but as a pair
    // of clauses neither half says anything on its own and nothing above matches
    // the pair. A library that writes an equivalence in clauses — which is what
    // keeping `var bool` means — leaves one of these behind every time. Here
    // rather than in phase 1 because the two halves often name *different*
    // variables until something above unifies them: the pair is only a pair once
    // the queue has drained.
    {
      std::map<std::pair<VarDecl*, VarDecl*>, ConstraintI*> binaryClauses;
      for (unsigned int i = 0; i < m.size(); i++) {
        auto* ci = m[i]->dynamicCast<ConstraintI>();
        if (ci == nullptr || ci->removed()) {
          continue;
        }
        auto* c = Expression::dynamicCast<Call>(ci->e());
        if (c == nullptr) {
          continue;
        }
        // The consequent and the antecedent of `b -> a`, however it is written.
        // A library that keeps `var bool` states one direction as a clause and
        // the other as a row over the same two Booleans just as readily, and a
        // pair split across the two forms is still a pair.
        Expression* consequent = nullptr;
        Expression* antecedent = nullptr;
        if (c->id() == envi.constants.ids.clause ||
            c->id() == envi.constants.ids.fznso.bool_clause ||
            c->id() == envi.constants.ids.bool_.clause) {
          auto* pos = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
          auto* neg = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
          if (pos == nullptr || neg == nullptr || pos->size() != 1 || neg->size() != 1) {
            continue;
          }
          consequent = (*pos)[0];
          antecedent = (*neg)[0];
        } else if (c->id() == envi.constants.ids.fznso.bool_lin_le ||
                   c->id() == envi.constants.ids.bool_.lin_le) {
          // `x - y <= 0` is `x -> y`.
          auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
          auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
          if (alC == nullptr || alX == nullptr || alC->size() != 2 || alX->size() != 2 ||
              !Expression::type(c->arg(2)).isPar() || eval_int(envi, c->arg(2)) != 0 ||
              !Expression::isa<IntLit>((*alC)[0]) || !Expression::isa<IntLit>((*alC)[1])) {
            continue;
          }
          IntVal a0 = IntLit::v(Expression::cast<IntLit>((*alC)[0]));
          IntVal a1 = IntLit::v(Expression::cast<IntLit>((*alC)[1]));
          if (a0 == 1 && a1 == -1) {
            consequent = (*alX)[1];
            antecedent = (*alX)[0];
          } else if (a0 == -1 && a1 == 1) {
            consequent = (*alX)[0];
            antecedent = (*alX)[1];
          } else {
            continue;
          }
        } else {
          continue;
        }
        if (!Expression::isa<Id>(consequent) || !Expression::isa<Id>(antecedent)) {
          continue;
        }
        // Resolved, not as written: unifying two names that already resolve to
        // one declaration would point it at itself, and every
        // `follow_id_to_decl` after that never returns.
        auto* a = Expression::dynamicCast<VarDecl>(follow_id_to_decl(consequent));
        auto* b = Expression::dynamicCast<VarDecl>(follow_id_to_decl(antecedent));
        if (a == nullptr || b == nullptr || a == b) {
          continue;
        }
        auto converse = binaryClauses.find({b, a});
        if (converse == binaryClauses.end() || converse->second->removed() ||
            (a->e() != nullptr && b->e() != nullptr)) {
          binaryClauses.emplace(std::make_pair(a, b), ci);
          continue;
        }
        if (a->e() != nullptr) {
          std::swap(a, b);
        }
        GCLock lock;
        unify(envi, deletedVarDecls, a->id(), b->id());
        push_dependent_constraints(envi, a->id(), constraintQueue);
        for (ConstraintI* dead : {ci, converse->second}) {
          CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, dead);
          top_down(cd, dead->e());
          dead->e(envi.constants.literalTrue);
          dead->remove();
        }
        binaryClauses.erase(converse);
      }
    }

    // Two equalities over the same two variables determine both of them, and
    // nothing above solves a pair of rows together: each is under-determined on
    // its own, so both reach the solver holding variables that were never in
    // doubt.
    {
      std::map<std::pair<VarDecl*, VarDecl*>, ConstraintI*> pairRows;
      for (unsigned int i = 0; i < m.size(); i++) {
        auto* ci = m[i]->dynamicCast<ConstraintI>();
        if (ci == nullptr || ci->removed()) {
          continue;
        }
        auto* c = Expression::dynamicCast<Call>(ci->e());
        if (c == nullptr || (c->id() != envi.constants.ids.fznso.int_lin_eq &&
                             c->id() != envi.constants.ids.int_.lin_eq &&
                             c->id() != envi.constants.ids.fznso.bool_lin_eq &&
                             c->id() != envi.constants.ids.bool_.lin_eq)) {
          continue;
        }
        std::vector<std::pair<VarDecl*, IntVal>> terms;
        IntVal rhs = 0;
        if (!fznso_lin_terms(envi, c, terms, rhs) || terms.size() != 2) {
          continue;
        }
        auto seen = pairRows.find({terms[0].first, terms[1].first});
        if (seen == pairRows.end() || seen->second->removed()) {
          pairRows[{terms[0].first, terms[1].first}] = ci;
          continue;
        }
        std::vector<std::pair<VarDecl*, IntVal>> other;
        IntVal otherRhs = 0;
        auto* oc = Expression::cast<Call>(seen->second->e());
        if (!fznso_lin_terms(envi, oc, other, otherRhs) || other.size() != 2 ||
            other[0].first != terms[0].first || other[1].first != terms[1].first) {
          continue;
        }
        IntVal det = terms[0].second * other[1].second - other[0].second * terms[1].second;
        if (det == 0) {
          continue;  // the same fact twice, or a contradiction; leave both
        }
        IntVal xNum = rhs * other[1].second - otherRhs * terms[1].second;
        IntVal yNum = terms[0].second * otherRhs - other[0].second * rhs;
        if (xNum % det != 0 || yNum % det != 0) {
          env.envi().fail();  // no integer point satisfies both
          break;
        }
        IntVal value[2] = {xNum / det, yNum / det};
        bool ok = true;
        for (int k = 0; k < 2 && ok; k++) {
          ok = fznso_fix_to(envi, terms[k].first, value[k], vardeclQueue, constraintQueue);
        }
        if (!ok) {
          env.envi().fail();
          break;
        }
        for (ConstraintI* dead : {ci, seen->second}) {
          CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, dead);
          top_down(cd, dead->e());
          dead->e(envi.constants.literalTrue);
          dead->remove();
        }
        pairRows.erase(seen);
      }
    }

    {
      std::map<std::pair<std::vector<std::pair<VarDecl*, IntVal>>, IntVal>, ConstraintI*> linRows;
      for (unsigned int i = 0; i < m.size(); i++) {
        auto* ci = m[i]->dynamicCast<ConstraintI>();
        if (ci == nullptr || ci->removed()) {
          continue;
        }
        auto* c = Expression::dynamicCast<Call>(ci->e());
        if (c == nullptr || (c->id() != envi.constants.ids.fznso.int_lin_le &&
                             c->id() != envi.constants.ids.int_.lin_le)) {
          continue;
        }
        std::vector<std::pair<VarDecl*, IntVal>> terms;
        IntVal rhs = 0;
        if (!fznso_lin_terms(envi, c, terms, rhs) || terms.empty()) {
          continue;
        }
        auto flip = terms;
        for (auto& t : flip) {
          t.second = -t.second;
        }
        auto converse = linRows.find({flip, -rhs});
        if (converse == linRows.end() || converse->second->removed()) {
          linRows[{terms, rhs}] = ci;
          continue;
        }
        GCLock lock;
        c->id(c->id() == envi.constants.ids.int_.lin_le ? envi.constants.ids.int_.lin_eq
                                                        : envi.constants.ids.fznso.int_lin_eq);
        c->decl(envi.model->matchFn(envi, c, false));
        ConstraintI* dead = converse->second;
        CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, dead);
        top_down(cd, dead->e());
        dead->e(envi.constants.literalTrue);
        dead->remove();
        linRows.erase(converse);
        ci->flag(true);
        constraintQueue.push_back(ci);
      }
    }

    // Clean up constraints that have been removed in the previous phase
    for (auto i = static_cast<unsigned int>(toRemoveConstraints.size()); (i--) != 0U;) {
      auto* ci = m[toRemoveConstraints[i]]->cast<ConstraintI>();
      CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
      top_down(cd, ci->e());
      ci->remove();
    }

    // Phase 4: handle boolean constraints again (todo: check if we can
    // refactor this into a separate function)
    //
    // Difference to phase 2: constraint argument arrays are actually shortened here if possible
    for (auto i = static_cast<unsigned int>(boolConstraints.size()); (i--) != 0U;) {
      env.envi().checkCancel();
      Item* bi = m[boolConstraints[i]];
      if (bi->removed()) {
        continue;
      }
      Call* c;
      std::vector<VarDecl*> removedVarDecls;

      if (bi->isa<ConstraintI>()) {
        c = Expression::dynamicCast<Call>(bi->cast<ConstraintI>()->e());
      } else {
        c = Expression::dynamicCast<Call>(bi->cast<VarDeclI>()->e()->e());
      }
      if (c == nullptr ||
          !(c->id() == envi.constants.ids.forall || c->id() == envi.constants.ids.exists ||
            (c->id() == envi.constants.ids.clause ||
             c->id() == envi.constants.ids.fznso.bool_clause ||
             c->id() == envi.constants.ids.bool_.clause))) {
        continue;
      }
      bool isConjunction = (c->id() == envi.constants.ids.forall);
      bool subsumed = false;
      bool empty = true;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        bool unit = (j == 0 ? isConjunction : !isConjunction);
        auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(j)));
        std::vector<Expression*> compactedAl;
        for (unsigned int k = 0; k < al->size(); k++) {
          if (Id* ident = Expression::dynamicCast<Id>((*al)[k])) {
            if (ident->decl()->ti()->domain() != nullptr) {
              if (!(ident->decl()->ti()->domain() == envi.constants.boollit(unit))) {
                subsumed = true;
              }
              removedVarDecls.push_back(ident->decl());
            } else {
              compactedAl.push_back(ident);
            }
          } else {
            if (Expression::cast<BoolLit>((*al)[k])->v() != unit) {
              subsumed = true;
            }
          }
        }
        if (compactedAl.size() < al->size()) {
          c->arg(j, new ArrayLit(Expression::loc(al), compactedAl));
          Expression::type(c->arg(j), Type::varbool(1));
        }
        empty = empty && compactedAl.empty();
      }
      if (subsumed) {
        if (bi->isa<ConstraintI>()) {
          if (isConjunction) {
            env.envi().fail();
          } else {
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<ConstraintI>()->e());
            bi->remove();
          }
        } else {
          if (isConjunction) {
            auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
            for (unsigned int j = 0; j < al->size(); j++) {
              removedVarDecls.push_back(Expression::cast<Id>((*al)[j])->decl());
            }
          } else {
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<VarDeclI>()->e()->e());
          }
          bool result = !isConjunction;
          auto* ti = bi->cast<VarDeclI>()->e()->ti();
          if (ti->domain() != nullptr) {
            if (Expression::equal(ti->domain(), env.envi().constants.boollit(!result))) {
              env.envi().fail();
            }
          } else {
            ti->domain(envi.constants.boollit(result));
          }
          ti->setComputedDomain(true);
          bi->cast<VarDeclI>()->e()->e(envi.constants.boollit(result));
        }
      } else if (!empty && bi->isa<ConstraintI>() && !isConjunction &&
                 c->arg(0) != nullptr && c->argCount() == 2 &&
                 Expression::cast<ArrayLit>(follow_id(c->arg(0)))->size() +
                         Expression::cast<ArrayLit>(follow_id(c->arg(1)))->size() ==
                     1) {
        // Shortening left one literal, and a root disjunction of one literal
        // says what that literal is. Phase 2 fixes these where the clause was
        // born unit; only here can it have *become* unit, which is what happens
        // whenever propagation decides all but one of a clause's variables.
        auto* pos = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
        bool value = pos->size() == 1;
        Id* last = Expression::cast<Id>((*Expression::cast<ArrayLit>(
            follow_id(c->arg(value ? 0 : 1))))[0]);
        if (last->decl()->ti()->domain() == nullptr) {
          last->decl()->ti()->domain(envi.constants.boollit(value));
          if (last->decl()->e() == nullptr) {
            last->decl()->e(envi.constants.boollit(value));
          }
          CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
          top_down(cd, bi->cast<ConstraintI>()->e());
          bi->remove();
          push_vardecl(envi, *envi.varOccurrences.idx.find(last->decl()->id()).second,
                       vardeclQueue);
          push_dependent_constraints(envi, last, constraintQueue);
        } else if (eval_bool(envi, last->decl()->ti()->domain()) != value) {
          env.envi().fail();
        } else {
          CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
          top_down(cd, bi->cast<ConstraintI>()->e());
          bi->remove();
        }
      } else if (empty) {
        bool result = isConjunction;
        if (bi->isa<ConstraintI>()) {
          if (result) {
            bi->remove();
          } else {
            env.envi().fail();
          }
        } else {
          auto* ti = bi->cast<VarDeclI>()->e()->ti();
          if (ti->domain() != nullptr) {
            if (Expression::equal(ti->domain(), env.envi().constants.boollit(!result))) {
              env.envi().fail();
            }
          } else {
            ti->domain(envi.constants.boollit(result));
          }
          ti->setComputedDomain(true);
          bi->cast<VarDeclI>()->e()->e(envi.constants.boollit(result));
        }
      }

      for (auto& removedVarDecl : removedVarDecls) {
        if (env.envi().varOccurrences.remove(removedVarDecl, bi) == 0) {
          if (can_remove_fzn_vardecl(envi, removedVarDecl)) {
            deletedVarDecls.push_back(removedVarDecl);
          }
        }
      }
      if (auto* vdi = bi->dynamicCast<VarDeclI>()) {
        if (can_remove_fzn_vardecl(envi, vdi->e())) {
          deletedVarDecls.push_back(vdi->e());
        }
      }
    }
    // Phase 5: Chain Breaking
    env.envi().checkCancel();
    if (chain_compression) {
      ImpCompressor imp(envi, m, deletedVarDecls, boolConstraints);
      LECompressor le(envi, m, deletedVarDecls);
      for (auto& item : m) {
        imp.trackItem(item);
        le.trackItem(item);
      }
      envi.checkCancel();
      imp.compress();

      envi.checkCancel();
      le.compress();
    }

    // Phase 6: a negation that is only ever read as a number
    env.envi().checkCancel();
    fznso_substitute_negations(envi, m, deletedVarDecls);
    fznso_substitute_offsets(envi, m, deletedVarDecls);
    fznso_retarget_indicators(envi, m, deletedVarDecls);

    // Phase 7: remove deleted variables if possible
    remove_deleted_items(envi, deletedVarDecls);
  } catch (ModelInconsistent&) { /* NOLINT(bugprone-empty-catch) */
  }
}

class SubstitutionVisitor : public EVisitor {
protected:
  std::vector<VarDecl*> _removed;

  static bool inlineLiteral(Expression* e) {
    switch (Expression::eid(e)) {
      case BoxedExpression::E_BOOLLIT:
      case BoxedExpression::E_INTLIT:
      case BoxedExpression::E_FLOATLIT:
      case BoxedExpression::E_SETLIT:
      case BoxedExpression::E_STRINGLIT:
        return true;
      default:
        return false;
    }
  }

public:
  Expression* subst(Expression* e) {
    if (auto* vd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(e))) {
      if ((vd->e() != nullptr) && inlineLiteral(vd->e())) {
        _removed.push_back(vd);
        return vd->e();
      }
      if (vd->ti()->domain() != nullptr) {
        if (vd->type().isbool()) {
          _removed.push_back(vd);
          return vd->ti()->domain();
        }
        if (vd->type().isint() && Expression::isa<SetLit>(vd->ti()->domain()) &&
            Expression::cast<SetLit>(vd->ti()->domain())->isv()->size() == 1 &&
            Expression::cast<SetLit>(vd->ti()->domain())->isv()->min() ==
                Expression::cast<SetLit>(vd->ti()->domain())->isv()->max()) {
          _removed.push_back(vd);
          return IntLit::a(Expression::cast<SetLit>(vd->ti()->domain())->isv()->min());
        }
        if (vd->type().isfloat() && Expression::isa<SetLit>(vd->ti()->domain()) &&
            Expression::cast<SetLit>(vd->ti()->domain())->fsv()->size() == 1 &&
            Expression::cast<SetLit>(vd->ti()->domain())->fsv()->min() ==
                Expression::cast<SetLit>(vd->ti()->domain())->fsv()->max()) {
          _removed.push_back(vd);
          return FloatLit::a(Expression::cast<SetLit>(vd->ti()->domain())->fsv()->min());
        }
      }
    }
    return e;
  }
  /// Visit array literal
  void vArrayLit(ArrayLit* al) {
    for (unsigned int i = 0; i < al->size(); i++) {
      al->set(i, subst((*al)[i]));
    }
  }
  /// Visit call
  void vCall(Call* c) {
    for (unsigned int i = 0; i < c->argCount(); i++) {
      c->arg(i, subst(c->arg(i)));
    }
  }
  /// Determine whether to enter node
  static bool enter(Expression* e) { return !Expression::isa<Id>(e); }
  void remove(EnvI& env, Item* item, std::vector<VarDecl*>& deletedVarDecls) {
    for (auto& i : _removed) {
      Expression::ann(i).remove(env.constants.ann.is_defined_var);
      if (env.varOccurrences.remove(i, item) == 0) {
        if (can_remove_fzn_vardecl(env, i)) {
          deletedVarDecls.push_back(i);
        }
      }
    }
    // If we are replacing in an array and it is now all par, then remove it (or
    // move to output model)
    if (auto* vdi = Item::dynamicCast<VarDeclI>(item)) {
      if (env.varOccurrences.occurrences(vdi->e()) == 0) {
        if (auto* al = Expression::dynamicCast<ArrayLit>(vdi->e()->e())) {
          for (unsigned int i = 0; i < al->size(); i++) {
            if (!Expression::type((*al)[i]).isPar()) {
              return;
            }
          }
          Type nt = vdi->e()->type();
          nt.mkPar(env);
          vdi->e()->ti()->type(nt);
          vdi->e()->type(nt);
          Expression::type(vdi->e()->e(), nt);
          deletedVarDecls.push_back(vdi->e());
        }
      }
    }
  }
};

void substitute_fixed_vars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls) {
  SubstitutionVisitor sv;
  if (auto* ci = ii->dynamicCast<ConstraintI>()) {
    top_down(sv, ci->e());
    for (ExpressionSetIter it = Expression::ann(ci->e()).begin();
         it != Expression::ann(ci->e()).end(); ++it) {
      top_down(sv, *it);
    }
  } else if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
    top_down(sv, vdi->e());
    for (ExpressionSetIter it = Expression::ann(vdi->e()).begin();
         it != Expression::ann(vdi->e()).end(); ++it) {
      top_down(sv, *it);
    }
  } else {
    auto* si = ii->cast<SolveI>();
    if (si->e() != nullptr) {
      si->e(sv.subst(si->e()));
      top_down(sv, si->e());
    }
    for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
      top_down(sv, *it);
    }
  }
  sv.remove(env, ii, deletedVarDecls);
}

/// Whether every assignment the domains still allow satisfies
/// `sum(coeffs .* xs) <= bound`.
///
/// A linear library states a half-reified constraint as a big-M row, and fixing
/// the indicator leaves the row trivially satisfied rather than removing it.
/// `int_le` gets that for free from its domain tightening; a linear form needs
/// the bound computed. Conservative: any term whose variable has no domain, or
/// whose value is not an integer literal, gives up.
/// Whether a linear equality only *defines* a variable that nothing else reads.
///
/// A row like `n + b = 1`, where `n` is a fresh 0/1 column nobody looks at, says
/// what `n` is and nothing about anything else. The flattener writes them for
/// negations it turns out not to need, and they reach the solver as columns and
/// rows because nothing annotated them as definitions. Sound only where the
/// coefficient is one — otherwise the row also says the rest is divisible by it
/// — and where the variable's domain holds every value the rest can force on
/// it, which is what makes the row say nothing about the rest.
bool fznso_lin_defines_unread(EnvI& env, Call* c) {
  const auto& ids = env.constants.ids;
  if (c->id() != ids.fznso.int_lin_eq && c->id() != ids.int_.lin_eq &&
      c->id() != ids.fznso.bool_lin_eq && c->id() != ids.bool_.lin_eq) {
    return false;
  }
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size()) {
    return false;
  }
  const bool isBool = Expression::type(c->arg(1)).bt() == Type::BT_BOOL;
  // `bool_lin_eq` names its total as a *variable*, which is the shape a weighted
  // count of Booleans arrives in — and the commonest thing nothing goes on to
  // read. Treated as one more term, with the row's own right-hand side zero.
  Expression* total = c->arg(2);
  const bool varTotal = !Expression::type(total).isPar();
  if (varTotal && !Expression::isa<Id>(total)) {
    return false;
  }
  IntVal rhs = varTotal ? IntVal(0) : eval_int(env, total);
  IntVal low = 0;   // least the other terms can sum to
  IntVal high = 0;  // most they can
  VarDecl* only = nullptr;
  IntVal onlyCoeff = 0;
  for (unsigned int i = 0; i < alX->size() + static_cast<unsigned int>(varTotal); i++) {
    const bool isTotal = i == alX->size();
    if (!isTotal && !Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    IntVal a = isTotal ? IntVal(-1) : IntLit::v(Expression::cast<IntLit>((*alC)[i]));
    Expression* x = isTotal ? total : (*alX)[i];
    IntVal lo;
    IntVal hi;
    if (Expression::type(x).isPar()) {
      lo = hi = Expression::type(x).isbool() ? IntVal(eval_bool(env, x) ? 1 : 0) : eval_int(env, x);
    } else if (auto* id = Expression::dynamicCast<Id>(x)) {
      VarDecl* vd = id->decl();
      if (isBool && !isTotal) {
        if (vd->ti()->domain() == nullptr) {
          lo = 0;
          hi = 1;
        } else {
          lo = hi = vd->ti()->domain() == env.constants.literalTrue ? 1 : 0;
        }
      } else {
        if (vd->ti()->domain() == nullptr) {
          return false;
        }
        IntSetVal* dom = eval_intset(env, vd->ti()->domain());
        // A hole would let the row rule a value of the rest out, so only a
        // single range is safe to drop the row over.
        if (dom->empty() || dom->size() != 1 || !dom->min().isFinite() || !dom->max().isFinite()) {
          return false;
        }
        lo = dom->min();
        hi = dom->max();
      }
      if (only == nullptr && (a == 1 || a == -1) && lo != hi && vd->e() == nullptr &&
          !is_output(vd) && env.varOccurrences.occurrences(vd) == 1) {
        only = vd;
        onlyCoeff = a;
        continue;
      }
    } else {
      return false;
    }
    low += a > 0 ? a * lo : a * hi;
    high += a > 0 ? a * hi : a * lo;
  }
  if (only == nullptr) {
    return false;
  }
  // `onlyCoeff * only = rhs - rest`, so `only` ranges over this as the rest does.
  IntVal needLow = onlyCoeff * (rhs - high);
  IntVal needHigh = onlyCoeff * (rhs - low);
  if (needLow > needHigh) {
    std::swap(needLow, needHigh);
  }
  // What the dropped variable can hold, which is what makes the row say nothing
  // about the rest. Keyed on its own type: in a Boolean row the total is still
  // an integer.
  if (only->type().isvarbool()) {
    return needLow >= 0 && needHigh <= 1;
  }
  IntSetVal* dom = eval_intset(env, only->ti()->domain());
  return dom->min() <= needLow && dom->max() >= needHigh;
}

/// The terms of an integer linear row, as the declarations they resolve to with
/// their coefficients, sorted so that two rows over the same terms give the
/// same vector. Par terms are folded into \a rhs. False if the row is not of
/// that shape, or if a variable appears twice.
/// Fix \a vd to \a v, whichever of the two 0/1 spellings it is, and wake up
/// everything that reads it. False if its domain rules the value out.
bool fznso_fix_to(EnvI& env, VarDecl* vd, IntVal v, std::deque<unsigned int>& vardeclQueue,
                  std::deque<Item*>& constraintQueue) {
  GCLock lock;
  if (vd->type().isvarbool()) {
    if (v < 0 || v > 1) {
      return false;
    }
    Expression* want = env.constants.boollit(v == 1);
    if (vd->ti()->domain() != nullptr) {
      return vd->ti()->domain() == want;
    }
    vd->ti()->domain(want);
  } else {
    if (vd->ti()->domain() != nullptr) {
      IntSetVal* dom = eval_intset(env, vd->ti()->domain());
      if (!dom->contains(v)) {
        return false;
      }
      if (dom->min() == dom->max()) {
        return true;
      }
    }
    vd->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(v, v)));
    vd->ti()->setComputedDomain(false);
  }
  vardeclQueue.push_back(env.varOccurrences.idx.get(vd->id()));
  push_dependent_constraints(env, vd->id(), constraintQueue);
  return true;
}

bool fznso_lin_terms(EnvI& env, Call* c, std::vector<std::pair<VarDecl*, IntVal>>& terms,
                     IntVal& rhs) {
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size() ||
      !Expression::type(c->arg(2)).isPar()) {
    return false;
  }
  rhs = eval_int(env, c->arg(2));
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    IntVal a = IntLit::v(Expression::cast<IntLit>((*alC)[i]));
    Expression* x = (*alX)[i];
    if (Expression::type(x).isPar()) {
      rhs -= a * (Expression::type(x).isbool() ? IntVal(eval_bool(env, x) ? 1 : 0)
                                               : eval_int(env, x));
      continue;
    }
    auto* vd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(x));
    if (vd == nullptr || a == 0) {
      return false;
    }
    terms.emplace_back(vd, a);
  }
  std::sort(terms.begin(), terms.end(),
            [](const std::pair<VarDecl*, IntVal>& l, const std::pair<VarDecl*, IntVal>& r) {
              return l.first < r.first;
            });
  for (size_t i = 1; i < terms.size(); i++) {
    if (terms[i].first == terms[i - 1].first) {
      return false;
    }
  }
  return true;
}

bool fznso_lin_le_entailed(EnvI& env, Call* c) {
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size() ||
      !Expression::type(c->arg(2)).isPar()) {
    return false;
  }
  IntVal most = 0;
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    IntVal a = IntLit::v(Expression::cast<IntLit>((*alC)[i]));
    Expression* x = (*alX)[i];
    if (Expression::type(x).isPar()) {
      most += a * eval_int(env, x);
      continue;
    }
    auto* id = Expression::dynamicCast<Id>(x);
    if (id == nullptr || id->decl()->ti()->domain() == nullptr) {
      return false;
    }
    IntSetVal* dom = eval_intset(env, id->decl()->ti()->domain());
    if (dom->empty()) {
      return false;
    }
    most += a > 0 ? a * dom->max() : a * dom->min();
  }
  return most <= eval_int(env, c->arg(2));
}

/// Recover the single-variable form of a linear constraint: the one variable
/// term's coefficient, and what the fixed terms leave on the right.
///
/// A library that lowers comparisons onto the FZnSO registry states `int_le(x,
/// k)` as `fzn_int_lin_le([1], [x], k)`, so the domain tightening below would
/// otherwise never match one. That tightening is what seeds the simplification
/// fixpoint — each one fixes a variable, which re-queues everything mentioning
/// it — so losing it costs far more than the single constraint.
///
/// Fixed terms are folded into \a rhs here, because the flattener leaves them in
/// the coefficient array. The coefficient is returned rather than required to
/// be a unit: a big-M row whose operands have all become constants is exactly
/// this shape with the big-M as the coefficient, and it decides its indicator.
bool fznso_single_var_lin(EnvI& env, Call* c, Id*& ident, IntVal& coeff, IntVal& rhs) {
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size() ||
      !Expression::type(c->arg(2)).isPar()) {
    return false;
  }
  rhs = eval_int(env, c->arg(2));
  ident = nullptr;
  coeff = 0;
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    IntVal a = IntLit::v(Expression::cast<IntLit>((*alC)[i]));
    Expression* x = (*alX)[i];
    if (Expression::type(x).isPar()) {
      rhs -= a * eval_int(env, x);
    } else if (Expression::isa<Id>(x)) {
      if (ident != nullptr || a == 0) {
        return false;
      }
      ident = Expression::cast<Id>(x);
      coeff = a;
    } else {
      return false;
    }
  }
  return ident != nullptr;
}

/// `floor(a / b)` and `ceil(a / b)`, which C++ integer division is neither of
/// when the signs differ.
IntVal floor_div(IntVal a, IntVal b) {
  IntVal q = a / b;
  if (a % b != 0 && ((a < 0) != (b < 0))) {
    q -= 1;
  }
  return q;
}

IntVal ceil_div(IntVal a, IntVal b) {
  IntVal q = a / b;
  if (a % b != 0 && ((a < 0) == (b < 0))) {
    q += 1;
  }
  return q;
}

/// Replace \a ident's domain and retire the constraint it came from. Shared by
/// the `int_le` case and the FZnSO spellings of it.
void apply_tightened_domain(EnvI& env, Item* ii, Call* c, Id* ident, IntSetVal* newDomain,
                            bool isTrue, std::vector<VarDecl*>& deletedVarDecls,
                            std::deque<Item*>& constraintQueue) {
  if (newDomain->empty()) {
    env.fail();
    return;
  }
  ident->decl()->ti()->domain(new SetLit(Location().introduce(), newDomain));
  ident->decl()->ti()->setComputedDomain(false);

  if (newDomain->min() == newDomain->max()) {
    push_dependent_constraints(env, ident, constraintQueue);
  }
  CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
  top_down(cd, c);

  if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
    vdi->e()->e(env.constants.boollit(isTrue));
    push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
    if (env.varOccurrences.occurrences(vdi->e()) == 0) {
      if (is_output(vdi->e())) {
        VarDecl* vdOut =
            (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]->cast<VarDeclI>()->e();
        vdOut->e(env.constants.boollit(isTrue));
      }
      vdi->remove();
    }
  } else {
    ii->remove();
  }
}

/// Decide an FZnSO constraint all of whose arguments have become fixed.
///
/// Substitution replaces a fixed variable with its literal but does not
/// re-evaluate what it appears in, and the registry's linear forms are calls
/// rather than builtins, so nothing else here would look at them again. A model
/// that fixes its variables late -- a set decomposed to bits, say -- otherwise
/// reaches the solver as hundreds of rows over nothing but literals.
///
/// Returns false, leaving \a holds untouched, when the constraint is not one of
/// these or still mentions a variable.
/// Bound propagation over `fzn_bool_lin_le` / `fzn_bool_lin_eq`.
///
/// Every term is worth 0 or its coefficient, so the reachable range of the sum
/// is exact, and a term whose only remaining value would take the sum outside
/// the bound is decided. This is what `int_lin_le` over a `var 0..1` column
/// gets from the machinery below; the registry states the same row over
/// Booleans, where nothing else here would look at it.
///
/// A chain — `lex` is the clearest case — collapses entirely through this: one
/// row fixes an indicator, which fixes its neighbour, and so on. Without it the
/// whole chain reaches the solver even when both operands are constants.
///
/// Returns false, touching nothing, if this is not one of those or a term is
/// not a literal. Otherwise \a fixings holds the decided Booleans, and
/// \a infeasible says the row cannot hold at all, and \a entailed that it
/// always does.
bool fznso_bool_lin_fix(EnvI& env, Call* c, std::vector<std::pair<VarDecl*, bool>>& fixings,
                        bool& infeasible, bool& entailed) {
  // The registry spellings, plus the `bool_lin_*` builtins the rewrite layer
  // leaves behind. `fzn_int_lin_*` is included because a row over `bool2int`
  // terms is the same row over 0/1 columns, and an encoding's channel is
  // written that way. The *builtin* `int_lin_*` are deliberately absent: they
  // are MiniZinc's own and reach libraries this pass knows nothing about.
  const auto& ids = env.constants.ids;
  const bool isEq = c->id() == ids.fznso.bool_lin_eq || c->id() == ids.bool_.lin_eq ||
                    c->id() == ids.fznso.int_lin_eq;
  if (!isEq && c->id() != ids.fznso.bool_lin_le && c->id() != ids.bool_.lin_le &&
      c->id() != ids.fznso.int_lin_le) {
    return false;
  }
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size() ||
      !Expression::type(c->arg(2)).isPar()) {
    return false;
  }
  IntVal rhs = eval_int(env, c->arg(2));
  IntVal low = 0;   // least the sum can be
  IntVal high = 0;  // most the sum can be
  std::vector<std::pair<IntVal, VarDecl*>> free;
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    IntVal a = IntLit::v(Expression::cast<IntLit>((*alC)[i]));
    Expression* x = (*alX)[i];
    if (Expression::type(x).isPar()) {
      IntVal v = Expression::type(x).isbool() ? IntVal(eval_bool(env, x) ? 1 : 0)
                                              : eval_int(env, x);
      if (v < 0 || v > 1) {
        return false;
      }
      low += a * v;
      high += a * v;
      continue;
    }
    auto* id = Expression::dynamicCast<Id>(x);
    if (id == nullptr) {
      return false;
    }
    // The declaration the name *resolves* to: unifying two variables redirects
    // one at its declaration rather than rewriting the rows, so reading the one
    // the row names gives a stale domain, and fixing it leaves the survivor
    // untouched.
    auto* decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl(x));
    if (decl == nullptr) {
      return false;
    }
    // A `var 0..1` is a Boolean as far as this reasoning goes, which is how the
    // same row survives being written over `bool2int` terms rather than over
    // the Booleans themselves.
    if (decl->type().isvarint()) {
      if (decl->ti()->domain() == nullptr) {
        return false;
      }
      IntSetVal* dom = eval_intset(env, decl->ti()->domain());
      if (dom->empty()) {
        return false;
      }
      if (dom->min() == dom->max()) {
        low += a * dom->min();
        high += a * dom->min();
        continue;
      }
      if (dom->min() < 0 || dom->max() > 1) {
        // A wider column cannot be fixed to a truth value, but what it can
        // reach is what makes the indicator beside it decidable — and a big-M
        // row is exactly one of each. Without this the row survives holding an
        // indicator its own bound has already decided.
        if (!dom->min().isFinite() || !dom->max().isFinite()) {
          return false;
        }
        low += a > 0 ? a * dom->min() : a * dom->max();
        high += a > 0 ? a * dom->max() : a * dom->min();
        continue;
      }
    } else if (decl->ti()->domain() != nullptr) {
      bool v = decl->ti()->domain() == env.constants.literalTrue;
      if (v) {
        low += a;
        high += a;
      }
      continue;
    }
    low += std::min(IntVal(0), a);
    high += std::max(IntVal(0), a);
    free.emplace_back(a, decl);
  }

  infeasible = low > rhs || (isEq && high < rhs);
  if (infeasible) {
    return true;
  }
  // Every remaining assignment satisfies it, so it constrains nothing. Common
  // once the fixed terms are folded out: a big-M row left holding one Boolean.
  entailed = isEq ? (low == rhs && high == rhs) : high <= rhs;
  if (entailed) {
    return true;
  }
  for (const auto& t : free) {
    IntVal a = t.first;
    // What the rest of the sum can be once this term is set aside.
    IntVal restLow = low - std::min(IntVal(0), a);
    IntVal restHigh = high - std::max(IntVal(0), a);
    // `<= rhs` rules out whichever value pushes the least achievable sum past
    // it; `= rhs` additionally rules out one that keeps the most achievable sum
    // short of it.
    bool trueOut = restLow + a > rhs || (isEq && restHigh + a < rhs);
    bool falseOut = restLow > rhs || (isEq && restHigh < rhs);
    if (trueOut && falseOut) {
      infeasible = true;
      return true;
    }
    if (trueOut) {
      fixings.emplace_back(t.second, false);
    } else if (falseOut) {
      fixings.emplace_back(t.second, true);
    }
  }
  return true;
}

/// Drop the terms of an FZnSO linear row whose variables have become fixed,
/// folding them into the right-hand side.
///
/// Substitution turns a fixed variable into a literal but leaves it in the
/// array, so a row keeps its width however much of it is decided. That hides
/// the row from everything that matches on shape — the single-variable
/// tightening, the two-name unification — and hands the solver a column of
/// zeroes. MiniZinc folds its own `int_lin_*` this way; the registry spellings
/// are calls, so nothing else here does it for them.
///
/// Returns false when there is nothing to fold or the row is not one of these.
bool fznso_fold_lin(EnvI& env, Call* c) {
  const auto& ids = env.constants.ids.fznso;
  const bool isBool = c->id() == ids.bool_lin_eq || c->id() == ids.bool_lin_le ||
                      c->id() == env.constants.ids.bool_.lin_eq ||
                      c->id() == env.constants.ids.bool_.lin_le;
  const bool isInt = c->id() == ids.int_lin_eq || c->id() == ids.int_lin_le ||
                     c->id() == ids.int_lin_ne || c->id() == env.constants.ids.int_.lin_eq ||
                     c->id() == env.constants.ids.int_.lin_le ||
                     c->id() == env.constants.ids.int_.lin_ne;
  if (!isBool && !isInt) {
    return false;
  }
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size() ||
      !Expression::type(c->arg(2)).isPar()) {
    return false;
  }
  IntVal rhs = eval_int(env, c->arg(2));
  std::vector<Expression*> coeffs;
  std::vector<Expression*> vars;
  std::vector<VarDecl*> decls;  // what each name in `vars` resolves to
  bool folded = false;
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    IntVal a = IntLit::v(Expression::cast<IntLit>((*alC)[i]));
    Expression* x = (*alX)[i];
    IntVal fixed = 0;
    bool isFixed = false;
    if (Expression::type(x).isPar()) {
      fixed = isBool ? IntVal(eval_bool(env, x) ? 1 : 0) : eval_int(env, x);
      isFixed = true;
    } else if (auto* id = Expression::dynamicCast<Id>(x)) {
      if (isBool) {
        if (id->decl()->ti()->domain() != nullptr) {
          fixed = id->decl()->ti()->domain() == env.constants.literalTrue ? 1 : 0;
          isFixed = true;
        }
      } else if (id->decl()->ti()->domain() != nullptr) {
        IntSetVal* dom = eval_intset(env, id->decl()->ti()->domain());
        if (!dom->empty() && dom->min() == dom->max()) {
          fixed = dom->min();
          isFixed = true;
        }
      }
    } else {
      return false;
    }
    if (isFixed || a == 0) {
      rhs -= a * fixed;
      folded = true;
      continue;
    }
    // Unifying two variables leaves the row holding one of them twice, and the
    // two terms may well cancel: `x - y <= -1` becomes `0 <= -1`, which decides
    // the indicator beside it. Left unmerged the row says nothing and survives.
    //
    // Compared by the declaration each name *resolves* to: unifying redirects a
    // variable at its declaration rather than rewriting the rows that mention
    // it, so the two names are still two names right up until the FlatZinc is
    // written.
    auto* xd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(x));
    auto seen = xd == nullptr ? decls.end() : std::find(decls.begin(), decls.end(), xd);
    if (seen != decls.end()) {
      auto at = static_cast<size_t>(seen - decls.begin());
      IntVal merged = IntLit::v(Expression::cast<IntLit>(coeffs[at])) + a;
      folded = true;
      if (merged == 0) {
        coeffs.erase(coeffs.begin() + static_cast<long>(at));
        vars.erase(vars.begin() + static_cast<long>(at));
        decls.erase(decls.begin() + static_cast<long>(at));
      } else {
        GCLock lock;
        coeffs[at] = IntLit::a(merged);
      }
      continue;
    }
    coeffs.push_back((*alC)[i]);
    vars.push_back(x);
    decls.push_back(xd);
  }
  if (!folded) {
    return false;
  }
  GCLock lock;
  auto* nc = new ArrayLit(Location().introduce(), coeffs);
  nc->type(Type::parint(1));
  auto* nv = new ArrayLit(Location().introduce(), vars);
  Type vt = alX->type();
  vt.dim(1);
  nv->type(vt);
  c->arg(0, nc);
  c->arg(1, nv);
  c->arg(2, IntLit::a(rhs));
  return true;
}

/// A linear row whose terms are all fixed but whose right-hand side is a
/// variable: `sum(coeffs .* xs) = y` says what `y` is.
///
/// The registry's `bool_lin_eq` takes a `var int` total, which is how `card` of
/// a set arrives, so a set that has become fixed leaves its cardinality stated
/// as a row over constants rather than as the number it is.
///
/// Returns false unless every term is fixed and the total is a free variable;
/// otherwise \a ident is that variable and \a value what it must take.
bool fznso_lin_defines_total(EnvI& env, Call* c, Id*& ident, IntVal& value) {
  const auto& ids = env.constants.ids;
  const bool isBool = c->id() == ids.fznso.bool_lin_eq || c->id() == ids.bool_.lin_eq;
  if (!isBool && c->id() != ids.fznso.int_lin_eq && c->id() != ids.int_.lin_eq) {
    return false;
  }
  ident = Expression::dynamicCast<Id>(c->arg(2));
  if (ident == nullptr || ident->decl()->e() != nullptr) {
    return false;
  }
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size()) {
    return false;
  }
  value = 0;
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::isa<IntLit>((*alC)[i])) {
      return false;
    }
    Expression* x = (*alX)[i];
    IntVal v;
    if (Expression::type(x).isPar()) {
      v = isBool ? IntVal(eval_bool(env, x) ? 1 : 0) : eval_int(env, x);
    } else if (auto* id = Expression::dynamicCast<Id>(x)) {
      if (id->decl()->ti()->domain() == nullptr) {
        return false;
      }
      if (isBool) {
        v = id->decl()->ti()->domain() == env.constants.literalTrue ? 1 : 0;
      } else {
        IntSetVal* dom = eval_intset(env, id->decl()->ti()->domain());
        if (dom->empty() || dom->min() != dom->max()) {
          return false;
        }
        v = dom->min();
      }
    } else {
      return false;
    }
    value += IntLit::v(Expression::cast<IntLit>((*alC)[i])) * v;
  }
  return true;
}

bool fznso_par_constraint(EnvI& env, Call* c, bool& holds) {
  const auto& ids = env.constants.ids.fznso;
  if (c->id() == ids.bool_clause || c->id() == env.constants.ids.bool_.clause) {
    for (unsigned int side = 0; side < 2; side++) {
      auto* al = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(side)));
      if (al == nullptr) {
        return false;
      }
      for (unsigned int i = 0; i < al->size(); i++) {
        if (!Expression::type((*al)[i]).isPar()) {
          return false;
        }
      }
    }
    holds = false;
    for (unsigned int side = 0; side < 2 && !holds; side++) {
      auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(side)));
      for (unsigned int i = 0; i < al->size(); i++) {
        if (eval_bool(env, (*al)[i]) == (side == 0)) {
          holds = true;
          break;
        }
      }
    }
    return true;
  }

  const bool isInt = c->id() == ids.int_lin_eq || c->id() == ids.int_lin_le ||
                     c->id() == ids.int_lin_ne || c->id() == ids.bool_lin_eq ||
                     c->id() == ids.bool_lin_le ||
                     c->id() == env.constants.ids.bool_.lin_eq ||
                     c->id() == env.constants.ids.bool_.lin_le;
  const bool isFloat = c->id() == ids.float_lin_eq || c->id() == ids.float_lin_le ||
                       c->id() == ids.float_lin_lt;
  if (!isInt && !isFloat) {
    return false;
  }
  auto* alC = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(0)));
  auto* alX = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  if (alC == nullptr || alX == nullptr || alC->size() != alX->size() ||
      !Expression::type(c->arg(2)).isPar()) {
    return false;
  }
  for (unsigned int i = 0; i < alX->size(); i++) {
    if (!Expression::type((*alC)[i]).isPar() || !Expression::type((*alX)[i]).isPar()) {
      return false;
    }
  }
  const bool isBool = c->id() == ids.bool_lin_eq || c->id() == ids.bool_lin_le ||
                      c->id() == env.constants.ids.bool_.lin_eq ||
                      c->id() == env.constants.ids.bool_.lin_le;
  if (isInt) {
    IntVal sum = 0;
    for (unsigned int i = 0; i < alX->size(); i++) {
      IntVal x = isBool ? IntVal(eval_bool(env, (*alX)[i]) ? 1 : 0) : eval_int(env, (*alX)[i]);
      sum += eval_int(env, (*alC)[i]) * x;
    }
    IntVal rhs = eval_int(env, c->arg(2));
    if (c->id() == ids.int_lin_ne) {
      holds = sum != rhs;
    } else if (c->id() == ids.int_lin_le || c->id() == ids.bool_lin_le ||
               c->id() == env.constants.ids.bool_.lin_le) {
      holds = sum <= rhs;
    } else {
      holds = sum == rhs;
    }
    return true;
  }
  FloatVal sum = 0.0;
  for (unsigned int i = 0; i < alX->size(); i++) {
    sum += eval_float(env, (*alC)[i]) * eval_float(env, (*alX)[i]);
  }
  FloatVal rhs = eval_float(env, c->arg(2));
  if (c->id() == ids.float_lin_eq) {
    holds = sum == rhs;
  } else if (c->id() == ids.float_lin_lt) {
    holds = sum < rhs;
  } else {
    holds = sum <= rhs;
  }
  return true;
}

bool simplify_constraint(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls,
                         std::deque<Item*>& constraintQueue,
                         std::deque<unsigned int>& vardeclQueue) {
  Expression* con_e;
  bool is_true;
  bool is_false;
  if (auto* ci = ii->dynamicCast<ConstraintI>()) {
    con_e = ci->e();
    is_true = true;
    is_false = false;
  } else {
    auto* vdi = ii->cast<VarDeclI>();
    con_e = vdi->e()->e();
    is_true = (vdi->e()->type().isbool() && vdi->e()->ti()->domain() == env.constants.literalTrue);
    is_false =
        (vdi->e()->type().isbool() && vdi->e()->ti()->domain() == env.constants.literalFalse);
    assert(is_true || is_false || !vdi->e()->type().isbool() ||
           vdi->e()->ti()->domain() == nullptr);
  }
  if (Call* c = Expression::dynamicCast<Call>(con_e)) {
    bool parHolds = false;
    if (is_true) {
      Id* total = nullptr;
      IntVal value = 0;
      if (fznso_lin_defines_total(env, c, total, value)) {
        IntSetVal* domain = total->decl()->ti()->domain() != nullptr
                                ? eval_intset(env, total->decl()->ti()->domain())
                                : IntSetVal::a(IntVal::minint(), IntVal::maxint());
        IntSetVal* d = LinearTraits<IntLit>::limitDomain(BOT_LQ, domain, value);
        if (!d->empty()) {
          d = LinearTraits<IntLit>::limitDomain(BOT_GQ, d, value);
        }
        apply_tightened_domain(env, ii, c, total, d, is_true, deletedVarDecls, constraintQueue);
        return true;
      }
    }
    if (is_true && fznso_fold_lin(env, c)) {
      constraintQueue.push_back(ii);
      return true;
    }
    if (is_true) {
      std::vector<std::pair<VarDecl*, bool>> fixings;
      bool infeasible = false;
      bool entailed = false;
      if (fznso_bool_lin_fix(env, c, fixings, infeasible, entailed)) {
        if (infeasible) {
          env.fail();
          return true;
        }
        if (entailed) {
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
            vdi->e()->e(env.constants.literalTrue);
            push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
          } else {
            ii->remove();
          }
          return true;
        }
        bool changed = false;
        for (const auto& f : fixings) {
          if (f.first->type().isvarint()) {
            GCLock lock;
            IntSetVal* was = f.first->ti()->domain() != nullptr
                                 ? eval_intset(env, f.first->ti()->domain())
                                 : nullptr;
            IntVal v = f.second ? 1 : 0;
            if (was != nullptr && (was->min() > v || was->max() < v)) {
              env.fail();
              return true;
            }
            if (was == nullptr || was->min() != was->max()) {
              f.first->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(v, v)));
              f.first->ti()->setComputedDomain(false);
              vardeclQueue.push_back(env.varOccurrences.idx.get(f.first->id()));
              push_dependent_constraints(env, f.first->id(), constraintQueue);
              changed = true;
            }
            continue;
          }
          Expression* want = env.constants.boollit(f.second);
          if (f.first->ti()->domain() == nullptr) {
            f.first->ti()->domain(want);
            vardeclQueue.push_back(env.varOccurrences.idx.get(f.first->id()));
            push_dependent_constraints(env, f.first->id(), constraintQueue);
            changed = true;
          } else if (f.first->ti()->domain() != want) {
            env.fail();
            return true;
          }
        }
        if (changed) {
          return true;
        }
      }
    }
    if (fznso_par_constraint(env, c, parHolds)) {
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
      top_down(cd, c);
      if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
        if (vdi->e()->ti()->domain() != nullptr &&
            vdi->e()->ti()->domain() != env.constants.boollit(parHolds)) {
          env.fail();
        }
        vdi->e()->e(env.constants.boollit(parHolds));
        push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
      } else {
        if (!parHolds) {
          env.fail();
        }
        ii->remove();
      }
      return true;
    }
    if (c->id() == env.constants.ids.int_.eq || c->id() == env.constants.ids.bool_.eq ||
        c->id() == env.constants.ids.float_.eq || c->id() == env.constants.ids.set_.eq ||
        c->id() == env.constants.ids.fznso.set_eq) {
      if (is_true && Expression::isa<Id>(c->arg(0)) && Expression::isa<Id>(c->arg(1)) &&
          (Expression::cast<Id>(c->arg(0))->decl()->e() == nullptr ||
           Expression::cast<Id>(c->arg(1))->decl()->e() == nullptr)) {
        if (Call* defVar = Expression::ann(c).getCall(env.constants.ann.defines_var)) {
          // First, remove defines_var/is_defined_var annotations if present
          if (Expression::equal(defVar->arg(0), c->arg(0))) {
            Expression::ann(Expression::cast<Id>(c->arg(0))->decl())
                .remove(env.constants.ann.is_defined_var);
          } else {
            Expression::ann(Expression::cast<Id>(c->arg(1))->decl())
                .remove(env.constants.ann.is_defined_var);
          }
        }
        unify(env, deletedVarDecls, Expression::cast<Id>(c->arg(0)),
              Expression::cast<Id>(c->arg(1)));
        push_dependent_constraints(env, Expression::cast<Id>(c->arg(0)), constraintQueue);
        CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
        top_down(cd, c);
        if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
          vdi->e()->e(env.constants.literalTrue);
          deletedVarDecls.push_back(vdi->e());
        } else {
          ii->remove();
        }
      } else if (Expression::type(c->arg(0)).isPar() && Expression::type(c->arg(1)).isPar()) {
        Expression* e0 = eval_par(env, c->arg(0));
        Expression* e1 = eval_par(env, c->arg(1));
        bool is_equal = Expression::equal(e0, e1);
        if ((is_true && is_equal) || (is_false && !is_equal)) {
          // do nothing
        } else if ((is_true && !is_equal) || (is_false && is_equal)) {
          env.fail();
        } else {
          auto* vdi = ii->cast<VarDeclI>();
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          vdi->e()->e(env.constants.boollit(is_equal));
          vdi->e()->ti()->domain(env.constants.boollit(is_equal));
          vdi->e()->ti()->setComputedDomain(true);
          push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
          push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
        }
        if (ii->isa<ConstraintI>()) {
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          ii->remove();
        }
      } else if (is_true &&
                 ((Expression::isa<Id>(c->arg(0)) && Expression::type(c->arg(1)).isPar()) ||
                  (Expression::isa<Id>(c->arg(1)) && Expression::type(c->arg(0)).isPar()))) {
        Id* ident = Expression::isa<Id>(c->arg(0)) ? Expression::cast<Id>(c->arg(0))
                                                   : Expression::cast<Id>(c->arg(1));
        Expression* arg = Expression::isa<Id>(c->arg(0)) ? c->arg(1) : c->arg(0);
        bool canRemove = false;
        TypeInst* ti = ident->decl()->ti();
        switch (ident->type().bt()) {
          case Type::BT_BOOL:
            if (ti->domain() == nullptr) {
              ti->domain(env.constants.boollit(eval_bool(env, arg)));
              ti->setComputedDomain(false);
              canRemove = true;
            } else {
              if (eval_bool(env, ti->domain()) == eval_bool(env, arg)) {
                canRemove = true;
              } else {
                env.fail();
                canRemove = true;
              }
            }
            break;
          case Type::BT_INT: {
            if (ident->type().st() == Type::ST_SET) {
              GCLock lock;
              IntSetVal* isv = eval_intset(env, arg);
              if (ti->domain() != nullptr) {
                IntSetVal* dom = eval_intset(env, ti->domain());
                IntSetRanges domr(dom);
                IntSetRanges slr(isv);
                if (!Ranges::subset(slr, domr)) {
                  env.fail();
                  canRemove = true;
                }
              }
              if (ident->decl()->e() == nullptr) {
                ident->decl()->e(new SetLit(Expression::loc(arg), isv));
                canRemove = true;
              } else if (auto* call = Expression::dynamicCast<Call>(ident->decl()->e())) {
                // Remove call from RHS and add it as new constraint with the literal
                auto* sl = new SetLit(Expression::loc(arg), isv);
                std::vector<Expression*> args(call->argCount() + 1);
                for (unsigned int i = 0; i < call->argCount(); ++i) {
                  args[i] = call->arg(i);
                }
                args[call->argCount()] = sl;
                auto* nc = Call::a(Expression::loc(call), call->id(), args);
                nc->type(Type::varbool());
                nc->decl(env.model->matchFn(env, nc, false));
                env.flatAddItem(new ConstraintI(Expression::loc(call), nc));

                // Add literal as new RHS
                ident->decl()->e(sl);
                canRemove = true;
              } else {
                IntSetVal* rhs = eval_intset(env, ident->decl()->e());
                if (!rhs->equal(isv)) {
                  env.fail();
                }
                canRemove = true;
              }
            } else {
              IntVal d = eval_int(env, arg);
              if (ti->domain() == nullptr) {
                ti->domain(new SetLit(Location().introduce(), IntSetVal::a(d, d)));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                IntSetVal* isv = eval_intset(env, ti->domain());
                if (isv->contains(d)) {
                  ident->decl()->ti()->domain(
                      new SetLit(Location().introduce(), IntSetVal::a(d, d)));
                  ident->decl()->ti()->setComputedDomain(false);
                  canRemove = true;
                } else {
                  env.fail();
                  canRemove = true;
                }
              }
            }
          } break;
          case Type::BT_FLOAT: {
            if (ti->domain() == nullptr) {
              ti->domain(new BinOp(Location().introduce(), arg, BOT_DOTDOT, arg));
              ti->setComputedDomain(false);
              canRemove = true;
            } else {
              FloatVal value = eval_float(env, arg);
              if (LinearTraits<FloatLit>::domainContains(eval_floatset(env, ti->domain()), value)) {
                ti->domain(new BinOp(Location().introduce(), arg, BOT_DOTDOT, arg));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                env.fail();
                canRemove = true;
              }
            }
          } break;
          default:
            break;
        }
        if (ident->decl()->e() == nullptr) {
          ident->decl()->e(Expression::isa<Id>(c->arg(0)) ? c->arg(1) : c->arg(0));
          ti->setComputedDomain(true);
          canRemove = true;
        }

        if (Expression::isa<Call>(ident->decl()->e())) {
          constraintQueue.push_back((*env.flat())[env.varOccurrences.find(ident->decl())]);
        }
        push_dependent_constraints(env, ident, constraintQueue);
        if (canRemove) {
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
            if (env.varOccurrences.occurrences(vdi->e()) == 0) {
              if (is_output(vdi->e())) {
                VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                      ->cast<VarDeclI>()
                                      ->e();
                vd_out->e(env.constants.boollit(is_true));
              }
              vdi->remove();
            } else {
              vdi->e()->e(env.constants.boollit(is_true));
            }
          } else {
            ii->remove();
          }
        }
      }
    } else if (is_true && fznso_lin_defines_unread(env, c)) {
      // It says what an unread variable is, and nothing else.
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
      top_down(cd, c);
      if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
        vdi->e()->e(env.constants.literalTrue);
        push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
      } else {
        ii->remove();
      }
    } else if (is_true &&
               (c->id() == env.constants.ids.fznso.int_lin_le ||
                c->id() == env.constants.ids.int_.lin_le) &&
               fznso_lin_le_entailed(env, c)) {
      // Every remaining assignment satisfies it, so it constrains nothing.
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
      top_down(cd, c);
      if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
        vdi->e()->e(env.constants.literalTrue);
        push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
      } else {
        ii->remove();
      }
    } else if ((is_true || is_false) && c->id() == env.constants.ids.fznso.int_lin_ne) {
      Id* ident = nullptr;
      IntVal coeff = 0;
      IntVal rhs = 0;
      if (fznso_single_var_lin(env, c, ident, coeff, rhs) &&
          ident->decl()->ti()->domain() != nullptr) {
        // `x != k` is not an interval, but a domain is not an interval either:
        // it is the set the variable may still take, and one value can simply
        // come out of it. Without this the row survives to the solver even
        // where it decides the variable — and every reification against that
        // variable was lowered while the value was still in its domain.
        IntSetVal* domain = eval_intset(env, ident->decl()->ti()->domain());
        IntSetVal* narrowed = domain;
        if (rhs % coeff == 0) {
          IntVal v = rhs / coeff;
          if (is_true) {
            std::vector<IntSetVal::Range> ranges;
            for (unsigned int i = 0; i < domain->size(); i++) {
              if (domain->min(i) < v) {
                ranges.emplace_back(domain->min(i), std::min(domain->max(i), v - 1));
              }
              if (domain->max(i) > v) {
                ranges.emplace_back(std::max(domain->min(i), v + 1), domain->max(i));
              }
            }
            narrowed = IntSetVal::a(ranges);
          } else {
            narrowed = domain->contains(v) ? IntSetVal::a(v, v) : IntSetVal::a();
          }
        } else if (!is_true) {
          // `coeff * x = rhs` has no integer solution, so `x != rhs/coeff`
          // cannot be false.
          narrowed = IntSetVal::a();
        }
        apply_tightened_domain(env, ii, c, ident, narrowed, is_true, deletedVarDecls,
                               constraintQueue);
      }
    } else if ((is_true || is_false) &&
               (c->id() == env.constants.ids.fznso.int_lin_le ||
                c->id() == env.constants.ids.fznso.int_lin_eq ||
                c->id() == env.constants.ids.int_.lin_le ||
                c->id() == env.constants.ids.int_.lin_eq)) {
      Id* ident = nullptr;
      IntVal coeff = 0;
      IntVal rhs = 0;
      if (fznso_single_var_lin(env, c, ident, coeff, rhs)) {
        // An unbounded variable still has a domain to narrow — it is just the
        // whole of it. Without this, `x = 1` on a `var int` reaches the solver
        // as a row, because there was no interval to intersect.
        IntSetVal* domain = ident->decl()->ti()->domain() != nullptr
                                ? eval_intset(env, ident->decl()->ti()->domain())
                                : IntSetVal::a(IntVal::minint(), IntVal::maxint());
        if (c->id() == env.constants.ids.fznso.int_lin_eq ||
            c->id() == env.constants.ids.int_.lin_eq) {
          // `x != k` is not an interval, so only the positive case narrows.
          if (is_true) {
            IntSetVal* d;
            if (rhs % coeff != 0) {
              d = IntSetVal::a();  // no integer solves `coeff * x = rhs`
            } else {
              IntVal v = rhs / coeff;
              d = LinearTraits<IntLit>::limitDomain(BOT_LQ, domain, v);
              if (!d->empty()) {
                d = LinearTraits<IntLit>::limitDomain(BOT_GQ, d, v);
              }
            }
            apply_tightened_domain(env, ii, c, ident, d, is_true, deletedVarDecls,
                                   constraintQueue);
          }
        } else {
          // `coeff * x <= rhs` when the constraint holds, `coeff * x >= rhs + 1`
          // when it does not. Dividing through rounds towards whichever side
          // keeps every integer the inequality allows.
          IntVal side = is_true ? rhs : rhs + 1;
          BinOpType bot;
          IntVal bound;
          if ((coeff > 0) == is_true) {
            bot = BOT_LQ;
            bound = floor_div(side, coeff);
          } else {
            bot = BOT_GQ;
            bound = ceil_div(side, coeff);
          }
          apply_tightened_domain(env, ii, c, ident,
                                 LinearTraits<IntLit>::limitDomain(bot, domain, bound), is_true,
                                 deletedVarDecls, constraintQueue);
        }
      }
    } else if ((is_true || is_false) && c->id() == env.constants.ids.int_.le &&
               ((Expression::isa<Id>(c->arg(0)) && Expression::type(c->arg(1)).isPar()) ||
                (Expression::isa<Id>(c->arg(1)) && Expression::type(c->arg(0)).isPar()))) {
      Id* ident = Expression::isa<Id>(c->arg(0)) ? Expression::cast<Id>(c->arg(0))
                                                 : Expression::cast<Id>(c->arg(1));
      Expression* arg = Expression::isa<Id>(c->arg(0)) ? c->arg(1) : c->arg(0);
      IntSetVal* domain = ident->decl()->ti()->domain() != nullptr
                              ? eval_intset(env, ident->decl()->ti()->domain())
                              : nullptr;
      if (domain != nullptr) {
        BinOpType bot = Expression::isa<Id>(c->arg(0)) ? (is_true ? BOT_LQ : BOT_GR)
                                                       : (is_true ? BOT_GQ : BOT_LE);
        IntSetVal* newDomain = LinearTraits<IntLit>::limitDomain(bot, domain, eval_int(env, arg));
        if (newDomain->empty()) {
          env.fail();
        } else {
          ident->decl()->ti()->domain(new SetLit(Location().introduce(), newDomain));
          ident->decl()->ti()->setComputedDomain(false);

          if (newDomain->min() == newDomain->max()) {
            push_dependent_constraints(env, ident, constraintQueue);
          }
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);

          if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
            vdi->e()->e(env.constants.boollit(is_true));
            push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
            if (env.varOccurrences.occurrences(vdi->e()) == 0) {
              if (is_output(vdi->e())) {
                VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                      ->cast<VarDeclI>()
                                      ->e();
                vd_out->e(env.constants.boollit(is_true));
              }
              vdi->remove();
            }
          } else {
            ii->remove();
          }
        }
      }
    } else if (c->id() == env.constants.ids.bool2int ||
               c->id() == env.constants.ids.fznso.bool2int) {
      auto* vdi = ii->dynamicCast<VarDeclI>();
      VarDecl* vd;
      bool fixed = false;
      bool b_val = false;
      if (vdi != nullptr) {
        vd = vdi->e();
      } else if (Id* ident = Expression::dynamicCast<Id>(c->arg(1))) {
        vd = ident->decl();
      } else {
        vd = nullptr;
      }
      IntSetVal* vd_dom = nullptr;
      if (vd != nullptr) {
        if (vd->ti()->domain() != nullptr) {
          vd_dom = eval_intset(env, vd->ti()->domain());
          assert(!vd_dom->empty());
          if (vd_dom->max() < 0 || vd_dom->min() > 1) {
            env.fail();
            return true;
          }
          fixed = vd_dom->min() == vd_dom->max();
          b_val = (vd_dom->min() == 1);
        }
      } else {
        fixed = true;
        b_val = (eval_int(env, c->arg(1)) == 1);
      }
      if (fixed) {
        if (Expression::type(c->arg(0)).isPar()) {
          bool b2i_val = eval_bool(env, c->arg(0));
          if (b2i_val != b_val) {
            env.fail();
          } else {
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
              auto* v = IntLit::a(b2i_val ? 1 : 0);
              if (env.varOccurrences.occurrences(vdi->e()) == 0) {
                if (is_output(vdi->e())) {
                  VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                        ->cast<VarDeclI>()
                                        ->e();
                  vd_out->e(v);
                }
                vdi->remove();
              } else {
                vdi->e()->e(v);
              }
            } else {
              ii->remove();
            }
          }
        } else {
          Id* ident = Expression::cast<Id>(c->arg(0));
          TypeInst* ti = ident->decl()->ti();
          if (ti->domain() == nullptr) {
            ti->domain(env.constants.boollit(b_val));
            ti->setComputedDomain(false);
          } else if (eval_bool(env, ti->domain()) != b_val) {
            env.fail();
          }
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          if (vd != nullptr) {
            vd->e(IntLit::a(static_cast<long long>(b_val)));
            vd->ti()->setComputedDomain(true);
          }
          push_dependent_constraints(env, ident, constraintQueue);
          if (vdi != nullptr) {
            if (env.varOccurrences.occurrences(vd) == 0) {
              deletedVarDecls.push_back(vdi->e());
            }
          } else {
            ii->remove();
          }
        }
      } else {
        IntVal v = -1;
        if (auto* bl = Expression::dynamicCast<BoolLit>(c->arg(0))) {
          v = bl->v() ? 1 : 0;
        } else if (Id* ident = Expression::dynamicCast<Id>(c->arg(0))) {
          if (ident->decl()->ti()->domain() != nullptr) {
            v = eval_bool(env, ident->decl()->ti()->domain()) ? 1 : 0;
          }
        }
        if (v != -1) {
          if ((vd_dom != nullptr) && !vd_dom->contains(v)) {
            env.fail();
          } else {
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vd->e(IntLit::a(v));
            vd->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(v, v)));
            vd->ti()->setComputedDomain(true);
            // The conversion has said all it had to say, and `top_down` above
            // has already stopped counting what it named. Leaving the item
            // behind leaves a row naming a variable whose declaration is then
            // removed for having no uses left.
            if (auto* rowI = ii->dynamicCast<ConstraintI>()) {
              rowI->e(env.constants.literalTrue);
              rowI->remove();
            }
            push_vardecl(env, env.varOccurrences.find(vd), vardeclQueue);
            push_dependent_constraints(env, vd->id(), constraintQueue);
          }
        }
      }

    } else {
      // General propagation: call a propagator registered for this constraint type
      Expression* rewrite = nullptr;
      GCLock lock;
      switch (OptimizeRegistry::registry().process(env, ii, c, rewrite)) {
        case OptimizeRegistry::CS_NONE:
          return false;
        case OptimizeRegistry::CS_OK:
          return true;
        case OptimizeRegistry::CS_FAILED:
          if (is_true) {
            env.fail();
            return true;
          } else if (is_false) {
            if (ii->isa<ConstraintI>()) {
              CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
              top_down(cd, c);
              ii->remove();
            } else {
              deletedVarDecls.push_back(ii->cast<VarDeclI>()->e());
            }
            return true;
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->ti()->domain(env.constants.literalFalse);
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vdi->e()->e(env.constants.literalFalse);
            push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            return true;
          }
        case OptimizeRegistry::CS_ENTAILED:
          if (is_true) {
            if (ii->isa<ConstraintI>()) {
              CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
              top_down(cd, c);
              ii->remove();
            } else {
              deletedVarDecls.push_back(ii->cast<VarDeclI>()->e());
            }
            return true;
          } else if (is_false) {
            env.fail();
            return true;
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->ti()->domain(env.constants.literalTrue);
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vdi->e()->e(env.constants.literalTrue);
            push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            return true;
          }
        case OptimizeRegistry::CS_REWRITE: {
          std::vector<VarDecl*> tdv;
          CollectDecls cd(env, env.varOccurrences, tdv, ii);
          top_down(cd, c);

          CollectOccurrencesE ce(env, env.varOccurrences, ii);
          top_down(ce, rewrite);

          for (auto& i : tdv) {
            if (env.varOccurrences.occurrences(i) == 0) {
              deletedVarDecls.push_back(i);
            }
          }

          assert(rewrite != nullptr);
          if (auto* ci = ii->dynamicCast<ConstraintI>()) {
            ci->e(rewrite);
            constraintQueue.push_back(ii);
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->e(rewrite);
            if ((vdi->e()->e() != nullptr) && Expression::isa<Id>(vdi->e()->e()) &&
                vdi->e()->type().dim() == 0) {
              Id* id1 = Expression::cast<Id>(vdi->e()->e());
              vdi->e()->e(nullptr);
              // Transfer is_defined_var annotation
              if (Expression::ann(id1->decl()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(vdi->e(), env.constants.ann.is_defined_var);
              } else if (Expression::ann(vdi->e()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(id1->decl(), env.constants.ann.is_defined_var);
              }
              unify(env, deletedVarDecls, vdi->e()->id(), id1);
              push_dependent_constraints(env, id1, constraintQueue);
            }
            if ((vdi->e()->e() != nullptr) && Expression::type(vdi->e()->e()).isPar() &&
                (vdi->e()->ti()->domain() != nullptr)) {
              if (Expression::type(vdi->e()->e()).isint()) {
                IntVal iv = eval_int(env, vdi->e()->e());
                IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                if (!dom->contains(iv)) {
                  env.fail();
                }
              } else if (Expression::type(vdi->e()->e()).isIntSet()) {
                IntSetVal* isv = eval_intset(env, vdi->e()->e());
                IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                IntSetRanges isv_r(isv);
                IntSetRanges dom_r(dom);
                if (!Ranges::subset(isv_r, dom_r)) {
                  env.fail();
                }
              } else if (Expression::type(vdi->e()->e()).isfloat()) {
                FloatVal fv = eval_float(env, vdi->e()->e());
                FloatSetVal* dom = eval_floatset(env, vdi->e()->ti()->domain());
                if (!dom->contains(fv)) {
                  env.fail();
                }
              } else if (Expression::type(vdi->e()->e()).isFloatSet()) {
                FloatSetVal* fsv = eval_floatset(env, vdi->e()->e());
                FloatSetVal* dom = eval_floatset(env, vdi->e()->ti()->domain());
                FloatSetRanges fsv_r(fsv);
                FloatSetRanges dom_r(dom);
                if (!Ranges::subset(fsv_r, dom_r)) {
                  env.fail();
                }
              }
            }
            if (vdi->e()->ti()->type() != Type::varbool() || vdi->e()->ti()->domain() == nullptr) {
              push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            }

            if (is_true) {
              constraintQueue.push_back(ii);
            }
          }
          return true;
        }
      }
    }
  }
  return false;
}

int bool_state(EnvI& env, Expression* e) {
  if (Expression::type(e).isPar()) {
    return static_cast<int>(eval_bool(env, e));
  }
  Id* id = Expression::cast<Id>(e);
  if (id->decl()->ti()->domain() == nullptr) {
    return 2;
  }
  return static_cast<int>(id->decl()->ti()->domain() == env.constants.literalTrue);
}

int decrement_non_fixed_vars(EnvI& env, Item* ii, std::vector<std::pair<Id*, Item*>>& toUnsubscribe,
                             std::unordered_map<Expression*, int>& nonFixedLiteralCount, Call* c) {
  auto it = nonFixedLiteralCount.find(c);
  if (it == nonFixedLiteralCount.end()) {
    int nonFixedVars = 0;
    for (unsigned int i = 0; i < c->argCount(); i++) {
      auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(i)));
      nonFixedVars += static_cast<int>(al->size());
      for (unsigned int j = al->size(); (j--) != 0U;) {
        if (Expression::type((*al)[j]).isPar() ||
            (Expression::isa<Id>((*al)[j]) &&
             Expression::cast<Id>((*al)[j])->decl()->ti()->domain() != nullptr)) {
          nonFixedVars--;
          if (Expression::isa<Id>((*al)[j])) {
            toUnsubscribe.emplace_back(Expression::cast<Id>((*al)[j]), ii);
          }
        }
      }
    }
    nonFixedLiteralCount.insert(std::make_pair(c, nonFixedVars));
    return nonFixedVars;
  }
  it->second--;
  return it->second;
}

void simplify_bool_constraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::deque<unsigned int>& vardeclQueue,
                              std::deque<Item*>& constraintQueue, std::vector<Item*>& toRemove,
                              std::vector<VarDecl*>& deletedVarDecls,
                              std::unordered_map<Expression*, int>& nonFixedLiteralCount,
                              std::vector<std::pair<Id*, Item*>>& toUnsubscribe) {
  if (ii->isa<SolveI>()) {
    remove = false;
    return;
  }
  bool isTrue = vd->ti()->domain() == env.constants.literalTrue;
  Expression* e = nullptr;
  auto* ci = ii->dynamicCast<ConstraintI>();
  auto* vdi = ii->dynamicCast<VarDeclI>();

  Call* call;
  if (ci != nullptr) {
    call = Expression::dynamicCast<Call>(ci->e());
  } else {
    call = Expression::dynamicCast<Call>(vdi->e()->e());
  }

  if (call != nullptr) {
    // Check that the vd actually occurs in the arguments of the call,
    // and not just in an annotation
    bool foundVd = false;
    for (unsigned int i = 0; !foundVd && i < call->argCount(); i++) {
      if (call->arg(i) == vd->id()) {
        foundVd = true;
        break;
      }
      auto* a = Expression::dynamicCast<ArrayLit>(call->arg(i));
      if (a != nullptr) {
        for (unsigned int j = 0; j < a->size(); j++) {
          if (Expression::equal((*a)[j], vd->id())) {
            foundVd = true;
            break;
          }
        }
      }
    }

    if (!foundVd) {
      remove = false;
      return;
    }
  }

  if (ci != nullptr) {
    e = ci->e();

    if (vd->ti()->domain() != nullptr) {
      if (Call* definedVarCall = Expression::ann(e).getCall(env.constants.ann.defines_var)) {
        if (Expression::equal(definedVarCall->arg(0), vd->id())) {
          Expression::ann(e).removeCall(env.constants.ann.defines_var);
          Expression::ann(vd).remove(env.constants.ann.is_defined_var);
        }
      }
    }

  } else if (vdi != nullptr) {
    e = vdi->e()->e();
    if (e == nullptr) {
      return;
    }
    if (Id* id = Expression::dynamicCast<Id>(e)) {
      assert(id->decl() == vd);
      if (vdi->e()->ti()->domain() == nullptr) {
        vdi->e()->ti()->domain(env.constants.boollit(isTrue));
        vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
      } else if (id->decl()->ti()->domain() == env.constants.boollit(!isTrue)) {
        env.fail();
        remove = false;
      }
      return;
    }
  }
  if (Id* ident = Expression::dynamicCast<Id>(e)) {
    assert(ident->decl() == vd);
    return;
  }
  if (Expression::isa<BoolLit>(e)) {
    if (e == env.constants.literalTrue && (ci != nullptr)) {
      toRemove.push_back(ci);
    }
    return;
  }
  Call* c = Expression::cast<Call>(e);
  if (c->id() == env.constants.ids.bool_.eq) {
    Expression* b0 = c->arg(0);
    Expression* b1 = c->arg(1);
    int b0s = bool_state(env, b0);
    int b1s = bool_state(env, b1);
    if (b0s == 2) {
      std::swap(b0, b1);
      std::swap(b0s, b1s);
    }
    assert(b0s != 2);
    if (b0s == 2) {
      // Should never happen, but to be safe, just do nothing in release mode
      remove = false;
      return;
    }
    if ((ci != nullptr) || vdi->e()->ti()->domain() == env.constants.literalTrue) {
      if (b0s != b1s) {
        if (b1s == 2) {
          /// b0 is fixed, b1 is not fixed, so make them equal so that the ci/vdi is true
          Expression::cast<Id>(b1)->decl()->ti()->domain(env.constants.boollit(b0s == 1));
          vardeclQueue.push_back(
              env.varOccurrences.idx.get(Expression::cast<Id>(b1)->decl()->id()));
          if (ci != nullptr) {
            toRemove.push_back(ci);
          }
        } else {
          env.fail();
          remove = false;
        }
      } else {
        if (ci != nullptr) {
          toRemove.push_back(ci);
        }
      }
    } else if ((vdi != nullptr) && vdi->e()->ti()->domain() == env.constants.literalFalse) {
      if (b0s != b1s) {
        if (b1s == 2) {
          /// b0 is fixed, b1 is not fixed, so make them different so that vdi is false
          Expression::cast<Id>(b1)->decl()->ti()->domain(env.constants.boollit(b0s == 0));
          vardeclQueue.push_back(
              env.varOccurrences.idx.get(Expression::cast<Id>(b1)->decl()->id()));
        }
      } else {
        env.fail();
        remove = false;
      }
    } else {
      remove = false;
    }
  } else if (c->id() == env.constants.ids.forall || c->id() == env.constants.ids.exists ||
             (c->id() == env.constants.ids.clause ||
             c->id() == env.constants.ids.fznso.bool_clause ||
             c->id() == env.constants.ids.bool_.clause)) {
    if (isTrue && c->id() == env.constants.ids.exists) {
      if (ci != nullptr) {
        toRemove.push_back(ci);
      } else {
        if (vdi->e()->ti()->domain() == nullptr) {
          vdi->e()->ti()->domain(env.constants.literalTrue);
          vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
        } else if (vdi->e()->ti()->domain() != env.constants.literalTrue) {
          env.fail();
          vdi->e()->e(env.constants.literalTrue);
        }
      }
    } else if (!isTrue && c->id() == env.constants.ids.forall) {
      if (ci != nullptr) {
        env.fail();
        toRemove.push_back(ci);
      } else {
        if (vdi->e()->ti()->domain() == nullptr) {
          vdi->e()->ti()->domain(env.constants.literalFalse);
          vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
        } else if (vdi->e()->ti()->domain() != env.constants.literalFalse) {
          env.fail();
          vdi->e()->e(env.constants.literalFalse);
        }
      }
    } else {
      int nonfixed = decrement_non_fixed_vars(env, ii, toUnsubscribe, nonFixedLiteralCount, c);
      bool isConjunction = (c->id() == env.constants.ids.forall);
      assert(nonfixed >= 0);
      if (nonfixed <= 1) {
        bool subsumed = false;
        int nonfixed_i = -1;
        int nonfixed_j = -1;
        int realNonFixed = 0;
        for (unsigned int i = 0; i < c->argCount(); i++) {
          bool unit = (i == 0 ? isConjunction : !isConjunction);
          auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(i)));
          realNonFixed += static_cast<int>(al->size());
          for (unsigned int j = al->size(); (j--) != 0U;) {
            if (Expression::type((*al)[j]).isPar() ||
                (Expression::cast<Id>((*al)[j])->decl()->ti()->domain() != nullptr)) {
              realNonFixed--;
            }
            if (Expression::type((*al)[j]).isPar() && eval_bool(env, (*al)[j]) != unit) {
              subsumed = true;
              i = 2;  // break out of outer loop
              break;
            }
            if (Id* id = Expression::dynamicCast<Id>((*al)[j])) {
              if (id->decl()->ti()->domain() != nullptr) {
                bool idv = (id->decl()->ti()->domain() == env.constants.literalTrue);
                if (unit != idv) {
                  subsumed = true;
                  i = 2;  // break out of outer loop
                  break;
                }
              } else {
                nonfixed_i = static_cast<int>(i);
                nonfixed_j = static_cast<int>(j);
              }
            }
          }
        }

        if (subsumed) {
          if (ci != nullptr) {
            if (isConjunction) {
              env.fail();
              ci->e(env.constants.literalFalse);
            } else {
              toRemove.push_back(ci);
            }
          } else {
            if (vdi->e()->ti()->domain() == nullptr) {
              vdi->e()->ti()->domain(env.constants.boollit(!isConjunction));
              vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
            } else if (vdi->e()->ti()->domain() != env.constants.boollit(!isConjunction)) {
              env.fail();
              vdi->e()->e(env.constants.boollit(!isConjunction));
            }
          }
        } else if (realNonFixed == 0) {
          if (ci != nullptr) {
            if (isConjunction) {
              toRemove.push_back(ci);
            } else {
              env.fail();
              ci->e(env.constants.literalFalse);
            }
          } else {
            if (vdi->e()->ti()->domain() == nullptr) {
              vdi->e()->ti()->domain(env.constants.boollit(isConjunction));
              vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
            } else if (vdi->e()->ti()->domain() != env.constants.boollit(isConjunction)) {
              env.fail();
              vdi->e()->e(env.constants.boollit(isConjunction));
            }
            toRemove.push_back(vdi);
          }
        } else if (realNonFixed == 1) {
          // not subsumed, nonfixed==1
          assert(nonfixed_i != -1);
          auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(nonfixed_i)));
          Id* ident = Expression::cast<Id>((*al)[nonfixed_j]);
          if ((ci != nullptr) || (vdi->e()->ti()->domain() != nullptr)) {
            bool result = nonfixed_i == 0;
            if ((vdi != nullptr) && vdi->e()->ti()->domain() == env.constants.literalFalse) {
              result = !result;
            }
            VarDecl* decl = ident->decl();
            if (decl->ti()->domain() == nullptr) {
              decl->ti()->domain(env.constants.boollit(result));
              vardeclQueue.push_back(env.varOccurrences.idx.get(decl->id()));
            } else if (vd->ti()->domain() != env.constants.boollit(result)) {
              env.fail();
              decl->e(env.constants.literalTrue);
            }
          } else {
            if (nonfixed_i == 0) {
              // this is a clause, exists or forall with a single non-fixed variable,
              // assigned to a non-fixed variable => turn into simple equality
              vdi->e()->e(nullptr);
              // Transfer is_defined_var annotation
              if (Expression::ann(ident->decl()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(vdi->e(), env.constants.ann.is_defined_var);
              } else if (Expression::ann(vdi->e()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(ident->decl(), env.constants.ann.is_defined_var);
              }
              unify(env, deletedVarDecls, vdi->e()->id(), ident);
              push_dependent_constraints(env, ident, constraintQueue);
            } else {
              remove = false;
            }
          }
        } else {
          remove = false;
        }

      } else if ((c->id() == env.constants.ids.clause ||
             c->id() == env.constants.ids.fznso.bool_clause ||
             c->id() == env.constants.ids.bool_.clause)) {
        int posOrNeg = isTrue ? 0 : 1;
        auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(posOrNeg)));
        auto* al_other = Expression::cast<ArrayLit>(follow_id(c->arg(1 - posOrNeg)));

        if ((ci != nullptr) && al->size() == 1 && (*al)[0] != vd->id() && al_other->size() == 1) {
          // simple implication
          assert((*al_other)[0] == vd->id());
          if (ci != nullptr) {
            if (Expression::type((*al)[0]).isPar()) {
              if (eval_bool(env, (*al)[0]) == isTrue) {
                toRemove.push_back(ci);
              } else {
                env.fail();
                remove = false;
              }
            } else {
              Id* id = Expression::cast<Id>((*al)[0]);
              if (id->decl()->ti()->domain() == nullptr) {
                id->decl()->ti()->domain(env.constants.boollit(isTrue));
                vardeclQueue.push_back(env.varOccurrences.idx.get(id->decl()->id()));
              } else {
                if (id->decl()->ti()->domain() == env.constants.boollit(isTrue)) {
                  toRemove.push_back(ci);
                } else {
                  env.fail();
                  remove = false;
                }
              }
            }
          }
        } else {
          // proper clause
          for (unsigned int i = 0; i < al->size(); i++) {
            if ((*al)[i] == vd->id()) {
              if (ci != nullptr) {
                toRemove.push_back(ci);
              } else {
                if (vdi->e()->ti()->domain() == nullptr) {
                  vdi->e()->ti()->domain(env.constants.literalTrue);
                  vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
                } else if (vdi->e()->ti()->domain() != env.constants.literalTrue) {
                  env.fail();
                  vdi->e()->e(env.constants.literalTrue);
                }
              }
              break;
            }
          }
        }
      }
    }
  } else {
    remove = false;
  }
}

void substitute_fixed_vars(Env& env) {
  env.envi().checkCancel();
  if (env.envi().failed()) {
    return;
  }
  try {
    EnvI& envi = env.envi();
    Model& m = *envi.flat();

    std::vector<VarDecl*> deletedVarDecls;
    VarOccurrences rebuilt;
    CollectOccurrencesI coi(envi, rebuilt);

    for (unsigned int i = 0; i < m.size(); i++) {
      if (auto* vdi = m[i]->dynamicCast<VarDeclI>()) {
        if (!vdi->removed()) {
          rebuilt.addIndex(vdi, i);
        }
      }
    }

    for (auto* item : m) {
      if (!item->removed()) {
        substitute_fixed_vars(envi, item, deletedVarDecls);

        // Update the occurrences
        if (auto* vdi = item->dynamicCast<VarDeclI>()) {
          coi.vVarDeclI(vdi);
        } else if (auto* ci = item->dynamicCast<ConstraintI>()) {
          coi.vConstraintI(ci);
        } else if (auto* si = item->dynamicCast<SolveI>()) {
          coi.vSolveI(si);
        }
      }
    }
    env.envi().varOccurrences = std::move(rebuilt);
    remove_deleted_items(envi, deletedVarDecls);
  } catch (ModelInconsistent&) { /* NOLINT(bugprone-empty-catch) */
  }
}

}  // namespace MiniZinc
