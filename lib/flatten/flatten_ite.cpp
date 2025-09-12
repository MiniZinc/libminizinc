/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/gc.hh>

#include <vector>

namespace MiniZinc {

std::vector<Expression*> get_conjuncts(Expression* start) {
  std::vector<Expression*> conj_stack;
  std::vector<Expression*> conjuncts;
  conj_stack.push_back(start);
  while (!conj_stack.empty()) {
    Expression* e = conj_stack.back();
    conj_stack.pop_back();
    if (auto* bo = Expression::dynamicCast<BinOp>(e)) {
      if (bo->op() == BOT_AND) {
        conj_stack.push_back(bo->rhs());
        conj_stack.push_back(bo->lhs());
      } else {
        conjuncts.push_back(e);
      }
    } else {
      conjuncts.push_back(e);
    }
  }
  return conjuncts;
}

void classify_conjunct(EnvI& env, Expression* e, IdMap<int>& eq_occurrences,
                       IdMap<std::pair<Expression*, Expression*>>& eq_branches,
                       std::vector<Expression*>& other_branches) {
  if (auto* bo = Expression::dynamicCast<BinOp>(e)) {
    if (bo->op() == BOT_EQ && Expression::type(bo->lhs()).dim() == 0 &&
        !Expression::type(bo->lhs()).structBT()) {
      auto* ident = Expression::dynamicCast<Id>(bo->lhs());
      auto* other = bo->rhs();
      if (ident == nullptr || ident == env.constants.absent) {
        ident = Expression::dynamicCast<Id>(bo->rhs());
        other = bo->lhs();
      }
      if (ident != nullptr && ident != env.constants.absent &&
          eq_branches.find(ident) == eq_branches.end()) {
        auto it = eq_occurrences.find(ident);
        if (it == eq_occurrences.end()) {
          eq_occurrences.insert(ident, 1);
        } else {
          eq_occurrences.get(ident)++;
        }
        eq_branches.insert(ident, std::make_pair(other, bo));
        return;
      }
    }
  }
  other_branches.push_back(e);
}

Expression* ite_struct_split(EnvI& env, Type ty, const std::vector<Expression*>& then_in,
                             Expression* else_in, std::vector<KeepAlive>& results,
                             std::vector<std::vector<KeepAlive>>& e_then,
                             std::vector<KeepAlive>& e_else) {
  StructType* st = env.getStructType(ty);
  GCLock lock;
  std::vector<Expression*> tupleResult(st->size());

  std::vector<VarDecl*> then_decl(then_in.size());
  for (int i = 0; i < then_in.size(); ++i) {
    then_decl[i] =
        new VarDecl(Location().introduce(),
                    new TypeInst(Location().introduce(), Expression::type(then_in[i]), nullptr),
                    env.genId(), then_in[i]);
    then_decl[i]->ti()->setStructDomain(env, Expression::type(then_in[i]));
  }
  auto* else_decl =
      new VarDecl(Location().introduce(),
                  new TypeInst(Location().introduce(), Expression::type(else_in), nullptr),
                  env.genId(), else_in);
  else_decl->ti()->setStructDomain(env, Expression::type(else_in));

  for (unsigned int i = 0; i < st->size(); ++i) {
    Type field = (*st)[i];
    if (field.structBT()) {
      std::vector<Expression*> f_then_in(then_decl.size());
      for (int j = 0; j < then_decl.size(); ++j) {
        f_then_in[j] =
            new FieldAccess(Location().introduce(), then_decl[i]->id(), IntLit::a(i + 1));
        Expression::type(f_then_in[j], field);
      }
      Expression* f_else_in =
          new FieldAccess(Location().introduce(), else_decl->id(), IntLit::a(i + 1));
      Expression::type(f_else_in, field);
      tupleResult[i] = ite_struct_split(env, field, f_then_in, f_else_in, results, e_then, e_else);
    } else {
      VarDecl* fieldRes =
          new_vardecl(env, Ctx(), new TypeInst(Location().introduce(), field, nullptr), nullptr,
                      nullptr, nullptr);
      tupleResult[i] = fieldRes->id();

      results.emplace_back(fieldRes);
      e_then.emplace_back();
      e_then.back().reserve(then_decl.size());
      for (auto& decl : then_decl) {
        e_then.back().push_back(
            new FieldAccess(Location().introduce(), decl->id(), IntLit::a(i + 1)));
        Expression::type(e_then.back().back()(), field);
      }
      e_else.emplace_back(
          new FieldAccess(Location().introduce(), else_decl->id(), IntLit::a(i + 1)));
      Expression::type(e_else.back()(), field);
    }
    assert(results.size() == e_then.size() && results.size() == e_else.size());
  }
  ArrayLit* tuple = ArrayLit::constructTuple(Location().introduce(), tupleResult);
  tuple->type(ty);
  return tuple;
}

EE flatten_ite(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  ITE* ite = Expression::cast<ITE>(e);

  // The conditions of each branch of the if-then-else
  std::vector<KeepAlive> conditions;
  // Whether the right hand side of each branch is defined
  std::vector<std::vector<KeepAlive>> defined;
  // The right hand side of each branch
  std::vector<std::vector<KeepAlive>> branches;
  // Whether all branches are fixed
  std::vector<bool> allBranchesPar;

  // Compute bounds of result as union bounds of all branches
  std::vector<std::vector<IntBounds>> r_bounds_int;
  std::vector<bool> r_bounds_valid_int;
  std::vector<std::vector<IntSetVal*>> r_bounds_set;
  std::vector<bool> r_bounds_valid_set;
  std::vector<std::vector<FloatBounds>> r_bounds_float;
  std::vector<bool> r_bounds_valid_float;

  bool allConditionsPar = true;
  bool allDefined = true;

  // The result variables of each generated conditional
  std::vector<KeepAlive> results;
  KeepAlive tupleResult;
  // The then-expressions of each generated conditional
  std::vector<std::vector<KeepAlive>> e_then;
  // The else-expressions of each generated conditional
  std::vector<KeepAlive> e_else;

  bool noOtherBranches = true;
  if (ite->type() == Type::varbool() && ctx.b == C_ROOT && r == env.constants.varTrue) {
    // Check if all branches are of the form x1=e1 /\ ... /\ xn=en
    IdMap<int> eq_occurrences;
    std::vector<IdMap<std::pair<Expression*, Expression*>>> eq_branches(ite->size() + 1);
    std::vector<std::vector<Expression*>> other_branches(ite->size() + 1);
    for (unsigned int i = 0; i < ite->size(); i++) {
      auto conjuncts = get_conjuncts(ite->thenExpr(i));
      for (auto* c : conjuncts) {
        classify_conjunct(env, c, eq_occurrences, eq_branches[i], other_branches[i]);
      }
      noOtherBranches = noOtherBranches && other_branches[i].empty();
    }
    {
      auto conjuncts = get_conjuncts(ite->elseExpr());
      for (auto* c : conjuncts) {
        classify_conjunct(env, c, eq_occurrences, eq_branches[ite->size()],
                          other_branches[ite->size()]);
      }
      noOtherBranches = noOtherBranches && other_branches[ite->size()].empty();
    }
    for (auto& eq : eq_occurrences) {
      if (eq.second >= static_cast<int>(ite->size())) {
        // Any identifier that occurs in all or all but one branch gets its own conditional
        results.emplace_back(eq.first->decl());
        e_then.emplace_back();
        for (unsigned int i = 0; i < ite->size(); i++) {
          auto it = eq_branches[i].find(eq.first);
          if (it == eq_branches[i].end()) {
            // not found, simply push x=x
            e_then.back().emplace_back(eq.first);
          } else {
            e_then.back().emplace_back(it->second.first);
          }
        }
        {
          auto it = eq_branches[ite->size()].find(eq.first);
          if (it == eq_branches[ite->size()].end()) {
            // not found, simply push x=x
            e_else.emplace_back(eq.first);
          } else {
            e_else.emplace_back(it->second.first);
          }
        }
      } else {
        // All other identifiers are put in the vector of "other" branches
        for (unsigned int i = 0; i <= ite->size(); i++) {
          auto it = eq_branches[i].find(eq.first);
          if (it != eq_branches[i].end()) {
            other_branches[i].push_back(it->second.second);
            noOtherBranches = false;
            eq_branches[i].remove(eq.first);
          }
        }
      }
    }
    if (!noOtherBranches) {
      results.emplace_back(r);
      e_then.emplace_back();
      for (unsigned int i = 0; i < ite->size(); i++) {
        if (eq_branches[i].empty()) {
          e_then.back().emplace_back(ite->thenExpr(i));
        } else if (other_branches[i].empty()) {
          e_then.back().emplace_back(env.constants.literalTrue);
        } else if (other_branches[i].size() == 1) {
          e_then.back().emplace_back(other_branches[i][0]);
        } else {
          GCLock lock;
          auto* al = new ArrayLit(Location().introduce(), other_branches[i]);
          al->type(Type::varbool(1));
          Call* forall = Call::a(Location().introduce(), env.constants.ids.forall, {al});
          forall->decl(env.model->matchFn(env, forall, false));
          forall->type(forall->decl()->rtype(env, {al}, nullptr, false));
          e_then.back().emplace_back(forall);
        }
      }
      {
        if (eq_branches[ite->size()].empty()) {
          e_else.emplace_back(ite->elseExpr());
        } else if (other_branches[ite->size()].empty()) {
          e_else.emplace_back(env.constants.literalTrue);
        } else if (other_branches[ite->size()].size() == 1) {
          e_else.emplace_back(other_branches[ite->size()][0]);
        } else {
          GCLock lock;
          auto* al = new ArrayLit(Location().introduce(), other_branches[ite->size()]);
          al->type(Type::varbool(1));
          Call* forall = Call::a(Location().introduce(), env.constants.ids.forall, {al});
          forall->decl(env.model->matchFn(env, forall, false));
          forall->type(forall->decl()->rtype(env, {al}, nullptr, false));
          e_else.emplace_back(forall);
        }
      }
    }
  } else if (ite->type().istuple() || ite->type().isrecord()) {
    noOtherBranches = false;
    std::vector<Expression*> then_in(ite->size());
    for (unsigned int i = 0; i < ite->size(); ++i) {
      then_in[i] = ite->thenExpr(i);
    }
    tupleResult =
        ite_struct_split(env, ite->type(), then_in, ite->elseExpr(), results, e_then, e_else);
  } else {
    noOtherBranches = false;
    results.emplace_back(r);
    e_then.emplace_back();
    for (unsigned int i = 0; i < ite->size(); i++) {
      e_then.back().emplace_back(ite->thenExpr(i));
    }
    e_else.emplace_back(ite->elseExpr());
  }
  allBranchesPar.resize(results.size());
  r_bounds_valid_int.resize(results.size());
  r_bounds_int.resize(results.size());
  r_bounds_valid_float.resize(results.size());
  r_bounds_float.resize(results.size());
  r_bounds_valid_set.resize(results.size());
  r_bounds_set.resize(results.size());
  defined.resize(results.size());
  branches.resize(results.size());
  for (unsigned int i = 0; i < results.size(); i++) {
    allBranchesPar[i] = true;
    r_bounds_valid_int[i] = true;
    r_bounds_valid_float[i] = true;
    r_bounds_valid_set[i] = true;
  }

  Ctx cmix;
  cmix.b = C_MIX;
  cmix.i = C_MIX;
  cmix.neg = ctx.neg;

  bool foundTrueBranch = false;
  for (unsigned int i = 0; i < ite->size() && !foundTrueBranch; i++) {
    bool cond = true;
    EE e_if;
    Ctx cmix_not_negated;
    cmix_not_negated.b = C_MIX;
    cmix_not_negated.i = C_MIX;
    e_if = flat_exp(env, cmix_not_negated, ite->ifExpr(i), nullptr, env.constants.varTrue);
    if (Expression::type(e_if.r()) == Type::parbool()) {
      {
        GCLock lock;
        cond = eval_bool(env, e_if.r());
      }
      if (cond) {
        if (allConditionsPar) {
          // no var conditions before this one, so we can simply emit
          // the then branch
          return flat_exp(env, ctx, ite->thenExpr(i), r, b);
        }
        // had var conditions, so we have to take them into account
        // and emit new conditional clause
        // add another condition and definedness variable
        conditions.emplace_back(env.constants.literalTrue);
        for (unsigned int j = 0; j < results.size(); j++) {
          EE ethen = flat_exp(env, cmix, e_then[j][i](), nullptr, cmix.partialityVar(env));
          assert(ethen.b());
          defined[j].push_back(ethen.b);
          allDefined = allDefined && (ethen.b() == env.constants.literalTrue);
          branches[j].push_back(ethen.r);
          if (Expression::type(ethen.r()).isvar()) {
            allBranchesPar[j] = false;
          }
        }
        foundTrueBranch = true;
      } else {
        GCLock lock;
        conditions.emplace_back(env.constants.literalFalse);
        for (unsigned int j = 0; j < results.size(); j++) {
          defined[j].emplace_back(env.constants.literalTrue);
          branches[j].emplace_back(create_dummy_value(env, Expression::type(e_then[j][i]())));
        }
      }
    } else {
      allConditionsPar = false;
      // add current condition and definedness variable
      conditions.push_back(e_if.r);

      for (unsigned int j = 0; j < results.size(); j++) {
        // flatten the then branch
        EE ethen = flat_exp(env, cmix, e_then[j][i](), nullptr, cmix.partialityVar(env));

        assert(ethen.b());
        defined[j].push_back(ethen.b);
        allDefined = allDefined && (ethen.b() == env.constants.literalTrue);
        branches[j].push_back(ethen.r);
        if (Expression::type(ethen.r()).isvar()) {
          allBranchesPar[j] = false;
        }
      }
    }
    // update bounds

    if (cond) {
      for (unsigned int j = 0; j < results.size(); j++) {
        if (r_bounds_valid_int[j] && Expression::type(e_then[j][i]()).isint()) {
          GCLock lock;
          IntBounds ib_then = compute_int_bounds(env, branches[j][i]());
          if (ib_then.valid) {
            r_bounds_int[j].push_back(ib_then);
          }
          r_bounds_valid_int[j] = r_bounds_valid_int[j] && ib_then.valid;
        } else if (r_bounds_valid_set[j] && Expression::type(e_then[j][i]()).isIntSet()) {
          GCLock lock;
          IntSetVal* isv = compute_intset_bounds(env, branches[j][i]());
          if (isv != nullptr) {
            r_bounds_set[j].push_back(isv);
          }
          r_bounds_valid_set[j] = r_bounds_valid_set[j] && (isv != nullptr);
        } else if (r_bounds_valid_float[j] && Expression::type(e_then[j][i]()).isfloat()) {
          GCLock lock;
          FloatBounds fb_then = compute_float_bounds(env, branches[j][i]());
          if (fb_then.valid) {
            r_bounds_float[j].push_back(fb_then);
          }
          r_bounds_valid_float[j] = r_bounds_valid_float[j] && fb_then.valid;
        }
      }
    }
  }

  if (allConditionsPar) {
    // no var condition, and all par conditions were false,
    // so simply emit else branch
    return flat_exp(env, ctx, ite->elseExpr(), r, b);
  }

  for (auto& result : results) {
    if (result() == nullptr) {
      // need to introduce new result variable
      GCLock lock;
      auto* ti = new TypeInst(Location().introduce(), ite->type(), nullptr);
      result = new_vardecl(env, Ctx(), ti, nullptr, nullptr, nullptr);
    }
  }

  if (conditions.back()() != env.constants.literalTrue) {
    // The last condition wasn't fixed to true, we need to look at the else branch
    conditions.emplace_back(env.constants.literalTrue);

    for (unsigned int j = 0; j < results.size(); j++) {
      // flatten else branch
      EE eelse = flat_exp(env, cmix, e_else[j](), nullptr, cmix.partialityVar(env));
      assert(eelse.b());
      defined[j].push_back(eelse.b);
      allDefined = allDefined && (eelse.b() == env.constants.literalTrue);
      branches[j].push_back(eelse.r);
      if (Expression::type(eelse.r()).isvar()) {
        allBranchesPar[j] = false;
      }

      if (r_bounds_valid_int[j] && Expression::type(e_else[j]()).isint()) {
        GCLock lock;
        IntBounds ib_else = compute_int_bounds(env, eelse.r());
        if (ib_else.valid) {
          r_bounds_int[j].push_back(ib_else);
        }
        r_bounds_valid_int[j] = r_bounds_valid_int[j] && ib_else.valid;
      } else if (r_bounds_valid_set[j] && Expression::type(e_else[j]()).isIntSet()) {
        GCLock lock;
        IntSetVal* isv = compute_intset_bounds(env, eelse.r());
        if (isv != nullptr) {
          r_bounds_set[j].push_back(isv);
        }
        r_bounds_valid_set[j] = r_bounds_valid_set[j] && (isv != nullptr);
      } else if (r_bounds_valid_float[j] && Expression::type(e_else[j]()).isfloat()) {
        GCLock lock;
        FloatBounds fb_else = compute_float_bounds(env, eelse.r());
        if (fb_else.valid) {
          r_bounds_float[j].push_back(fb_else);
        }
        r_bounds_valid_float[j] = r_bounds_valid_float[j] && fb_else.valid;
      }
    }
  }

  // update domain of result variable with bounds from all branches
  for (unsigned int j = 0; j < results.size(); j++) {
    auto* nr = Expression::cast<VarDecl>(results[j]());
    GCLock lock;
    if (r_bounds_valid_int[j] && ite->type().isint()) {
      IntVal lb = IntVal::infinity();
      IntVal ub = -IntVal::infinity();
      for (auto& i : r_bounds_int[j]) {
        lb = std::min(lb, i.l);
        ub = std::max(ub, i.u);
      }
      if (nr->ti()->domain() != nullptr) {
        IntSetVal* isv = eval_intset(env, nr->ti()->domain());
        Ranges::Const<IntVal> ite_r(lb, ub);
        IntSetRanges isv_r(isv);
        Ranges::Inter<IntVal, Ranges::Const<IntVal>, IntSetRanges> inter(ite_r, isv_r);
        IntSetVal* isv_new = IntSetVal::ai(inter);
        if (!isv_new->equal(isv)) {
          auto* r_dom = new SetLit(Location().introduce(), isv_new);
          nr->ti()->domain(r_dom);
        }
      } else {
        auto* r_dom = new SetLit(Location().introduce(), IntSetVal::a(lb, ub));
        nr->ti()->domain(r_dom);
        nr->ti()->setComputedDomain(true);
      }
    } else if (r_bounds_valid_set[j] && ite->type().isIntSet()) {
      IntSetVal* isv_branches = IntSetVal::a();
      for (auto& i : r_bounds_set[j]) {
        IntSetRanges i0(isv_branches);
        IntSetRanges i1(i);
        Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(i0, i1);
        isv_branches = IntSetVal::ai(u);
      }
      if (nr->ti()->domain() != nullptr) {
        IntSetVal* isv = eval_intset(env, nr->ti()->domain());
        IntSetRanges isv_r(isv);
        IntSetRanges isv_branches_r(isv_branches);
        Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(isv_branches_r, isv_r);
        IntSetVal* isv_new = IntSetVal::ai(inter);
        if (!isv_new->equal(isv)) {
          auto* r_dom = new SetLit(Location().introduce(), isv_new);
          nr->ti()->domain(r_dom);
        }
      } else {
        auto* r_dom = new SetLit(Location().introduce(), isv_branches);
        nr->ti()->domain(r_dom);
        nr->ti()->setComputedDomain(true);
      }
    } else if (r_bounds_valid_float[j] && ite->type().isfloat()) {
      FloatVal lb = FloatVal::infinity();
      FloatVal ub = -FloatVal::infinity();
      for (auto& i : r_bounds_float[j]) {
        lb = std::min(lb, i.l);
        ub = std::max(ub, i.u);
      }
      if (nr->ti()->domain() != nullptr) {
        FloatSetVal* isv = eval_floatset(env, nr->ti()->domain());
        Ranges::Const<FloatVal> ite_r(lb, ub);
        FloatSetRanges isv_r(isv);
        Ranges::Inter<FloatVal, Ranges::Const<FloatVal>, FloatSetRanges> inter(ite_r, isv_r);
        FloatSetVal* fsv_new = FloatSetVal::ai(inter);
        auto* r_dom = new SetLit(Location().introduce(), fsv_new);
        nr->ti()->domain(r_dom);
      } else {
        auto* r_dom = new SetLit(Location().introduce(), FloatSetVal::a(lb, ub));
        nr->ti()->domain(r_dom);
        nr->ti()->setComputedDomain(true);
      }
    }
  }

  // Create ite predicate calls
  KeepAlive al_cond;
  {
    GCLock lock;
    al_cond = new ArrayLit(Location().introduce(), conditions);
    Expression::type(al_cond(), Type::varbool(1));
  }
  for (unsigned int j = 0; j < results.size(); j++) {
    KeepAlive ite_pred;
    {
      GCLock lock;
      auto* al_branches = new ArrayLit(Location().introduce(), branches[j]);
      Type branches_t = Type::arrType(env, Type::bot(1), Expression::type(results[j]()));
      if (!allBranchesPar[j]) {
        branches_t.mkVar(env);
      }
      branches_t.ti(allBranchesPar[j] ? Type::TI_PAR : Type::TI_VAR);
      al_branches->type(branches_t);
      ite_pred = Call::a(Expression::loc(ite).introduce(), ASTString("if_then_else"),
                         {al_cond(), al_branches, Expression::cast<VarDecl>(results[j]())->id()});
      Expression::cast<Call>(ite_pred())
          ->decl(env.model->matchFn(env, Expression::cast<Call>(ite_pred()), false));
      Expression::type(Expression::cast<Call>(ite_pred()), Type::varbool());
      make_defined_var(env, Expression::cast<VarDecl>(results[j]()),
                       Expression::cast<Call>(ite_pred()));
    }
    (void)flat_exp(env, Ctx(), ite_pred(), env.constants.varTrue, env.constants.varTrue);
  }
  EE ret;
  if (noOtherBranches) {
    ret.r = env.constants.varTrue->id();
  } else if (ite->type().istuple() || ite->type().isrecord()) {
    ret.r = bind(env, ctx, r, tupleResult());
  } else {
    ret.r = Expression::cast<VarDecl>(results.back()())->id();
  }
  if (allDefined) {
    bind(env, Ctx(), b, env.constants.literalTrue);
    ret.b = env.constants.literalTrue;
  } else {
    // Otherwise, constraint linking conditions, b and the definedness variables
    KeepAlive ite_defined_pred;
    {
      GCLock lock;

      if (b == nullptr) {
        CallStackItem _csi(env, new StringLit(Location().introduce(), "b"));
        b = new_vardecl(env, Ctx(), new TypeInst(Location().introduce(), Type::varbool()), nullptr,
                        nullptr, nullptr);
      }
      ret.b = b->id();

      std::vector<Expression*> defined_conjunctions(conditions.size());
      for (unsigned int i = 0; i < conditions.size(); i++) {
        std::vector<Expression*> def_i;
        for (auto& j : defined) {
          assert(j.size() > i);
          if (j[i]() != env.constants.literalTrue) {
            def_i.push_back(j[i]());
          }
        }
        if (def_i.empty()) {
          defined_conjunctions[i] = env.constants.literalTrue;
        } else if (def_i.size() == 1) {
          defined_conjunctions[i] = def_i[0];
        } else {
          auto* al = new ArrayLit(Location().introduce(), def_i);
          al->type(Type::varbool(1));
          Call* forall = Call::a(Location().introduce(), env.constants.ids.forall, {al});
          forall->decl(env.model->matchFn(env, forall, false));
          forall->type(forall->decl()->rtype(env, {al}, nullptr, false));
          defined_conjunctions[i] = forall;
        }
      }
      auto* al_defined = new ArrayLit(Location().introduce(), defined_conjunctions);
      al_defined->type(Type::varbool(1));
      ite_defined_pred =
          Call::a(Expression::loc(ite).introduce(), ASTString("if_then_else_partiality"),
                  {al_cond(), al_defined, b->id()});
      Expression::cast<Call>(ite_defined_pred())
          ->decl(env.model->matchFn(env, Expression::cast<Call>(ite_defined_pred()), false));
      Expression::type(ite_defined_pred(), Type::varbool());
    }
    (void)flat_exp(env, Ctx(), ite_defined_pred(), env.constants.varTrue, env.constants.varTrue);
  }

  return ret;
}

}  // namespace MiniZinc
