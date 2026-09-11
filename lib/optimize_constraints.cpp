/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/optimize_constraints.hh>

namespace MiniZinc {

void OptimizeRegistry::reg(const MiniZinc::ASTString& call, optimizer opt) {
  _m.insert(std::make_pair(call, opt));
}

OptimizeRegistry::ConstraintStatus OptimizeRegistry::process(EnvI& env, MiniZinc::Item* i,
                                                             MiniZinc::Call* c,
                                                             Expression*& rewrite) {
  auto it = _m.find(c->id());
  if (it != _m.end()) {
    return it->second(env, i, c, rewrite);
  }
  return CS_NONE;
}

OptimizeRegistry& OptimizeRegistry::registry() {
  static OptimizeRegistry reg;
  return reg;
}

namespace Optimizers {


OptimizeRegistry::ConstraintStatus o_linear(EnvI& env, Item* ii, Call* c, Expression*& rewrite) {
  ArrayLit* al_c = eval_array_lit(env, c->arg(0));
  std::vector<IntVal> coeffs(al_c->size());
  for (unsigned int i = 0; i < al_c->size(); i++) {
    coeffs[i] = eval_int(env, (*al_c)[i]);
  }
  ArrayLit* al_x = eval_array_lit(env, c->arg(1));
  std::vector<KeepAlive> x(al_x->size());
  for (unsigned int i = 0; i < al_x->size(); i++) {
    x[i] = (*al_x)[i];
  }
  IntVal d = 0;
  simplify_lin<IntLit>(coeffs, x, d);
  if (coeffs.empty()) {
    bool failed;
    if (c->id() == env.constants.ids.int_.lin_le ||
        c->id() == env.constants.ids.fznso.int_lin_le) {
      failed = (d > eval_int(env, c->arg(2)));
    } else if (c->id() == env.constants.ids.int_.lin_eq ||
               c->id() == env.constants.ids.fznso.int_lin_eq) {
      failed = (d != eval_int(env, c->arg(2)));
    } else {
      failed = (d == eval_int(env, c->arg(2)));
    }
    if (failed) {
      return OptimizeRegistry::CS_FAILED;
    }
    return OptimizeRegistry::CS_ENTAILED;
  }
  if (coeffs.size() == 1 && (ii->isa<ConstraintI>() || ii->cast<VarDeclI>()->e()->ti()->domain() ==
                                                           env.constants.literalTrue)) {
    VarDecl* vd = Expression::cast<Id>(x[0]())->decl();
    IntSetVal* domain =
        vd->ti()->domain() != nullptr ? eval_intset(env, vd->ti()->domain()) : nullptr;
    assert(!domain->empty());
    if (c->id() == env.constants.ids.int_.lin_eq ||
        c->id() == env.constants.ids.fznso.int_lin_eq) {
      IntVal rd = eval_int(env, c->arg(2)) - d;
      if (rd % coeffs[0] == 0) {
        IntVal nd = rd / coeffs[0];
        if ((domain != nullptr) && !domain->contains(nd)) {
          return OptimizeRegistry::CS_FAILED;
        }
        std::vector<Expression*> args(2);
        args[0] = x[0]();
        args[1] = IntLit::a(nd);
        Call* nc = Call::a(Location(), env.constants.ids.int_.eq, args);
        nc->type(Type::varbool());
        rewrite = nc;
        return OptimizeRegistry::CS_REWRITE;
      }
      return OptimizeRegistry::CS_FAILED;
    }
    if (c->id() == env.constants.ids.int_.lin_le ||
        c->id() == env.constants.ids.fznso.int_lin_le) {
      IntVal ac = abs(coeffs[0]);
      IntVal rd = eval_int(env, c->arg(2)) - d;
      IntVal ad = abs(rd);
      IntVal nd;
      if (ad % ac == 0) {
        nd = rd / coeffs[0];
      } else {
        double nd_d = static_cast<double>(ad.toInt()) / static_cast<double>(ac.toInt());
        if (coeffs[0] >= 0 && rd >= 0) {
          nd = static_cast<long long int>(std::floor(nd_d));
        } else if (rd >= 0) {
          nd = -static_cast<long long int>(std::floor(nd_d));
        } else if (coeffs[0] >= 0) {
          nd = -static_cast<long long int>(std::ceil(nd_d));
        } else {
          nd = static_cast<long long int>(std::ceil(nd_d));
        }
      }
      bool swapSign = coeffs[0] < 0;
      if (domain != nullptr) {
        if (swapSign) {
          if (domain->max() < nd) {
            return OptimizeRegistry::CS_FAILED;
          }
          if (domain->min() >= nd) {
            return OptimizeRegistry::CS_ENTAILED;
          }
        } else {
          if (domain->min() > nd) {
            return OptimizeRegistry::CS_FAILED;
          }
          if (domain->max() <= nd) {
            return OptimizeRegistry::CS_ENTAILED;
          }
        }
        std::vector<Expression*> args(2);
        args[0] = x[0]();
        args[1] = IntLit::a(nd);
        if (swapSign) {
          std::swap(args[0], args[1]);
        }
        Call* nc = Call::a(Location(), env.constants.ids.int_.le, args);
        nc->type(Type::varbool());
        rewrite = nc;
        return OptimizeRegistry::CS_REWRITE;
      }
    }
  } else if ((c->id() == env.constants.ids.int_.lin_eq ||
              c->id() == env.constants.ids.fznso.int_lin_eq) &&
             coeffs.size() == 2 &&
             ((coeffs[0] == 1 && coeffs[1] == -1) || (coeffs[1] == 1 && coeffs[0] == -1)) &&
             eval_int(env, c->arg(2)) - d == 0) {
    std::vector<Expression*> args(2);
    args[0] = x[0]();
    args[1] = x[1]();
    Call* nc = Call::a(Location(), env.constants.ids.int_.eq, args);
    rewrite = nc;
    return OptimizeRegistry::CS_REWRITE;
  }
  if (coeffs.size() < al_c->size()) {
    std::vector<Expression*> coeffs_e(coeffs.size());
    std::vector<Expression*> x_e(coeffs.size());
    for (unsigned int i = 0; i < coeffs.size(); i++) {
      coeffs_e[i] = IntLit::a(coeffs[i]);
      x_e[i] = x[i]();
    }
    auto* al_c_new = new ArrayLit(Expression::loc(al_c), coeffs_e);
    al_c_new->type(Type::parint(1));
    auto* al_x_new = new ArrayLit(Expression::loc(al_x), x_e);
    al_x_new->type(al_x->type());

    std::vector<Expression*> args(3);
    args[0] = al_c_new;
    args[1] = al_x_new;
    args[2] = IntLit::a(eval_int(env, c->arg(2)) - d);
    Call* nc = Call::a(Location(), c->id(), args);
    nc->type(Type::varbool());
    for (ExpressionSetIter it = Expression::ann(c).begin(); it != Expression::ann(c).end(); ++it) {
      Expression::addAnnotation(nc, *it);
    }

    rewrite = nc;
    return OptimizeRegistry::CS_REWRITE;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_lin_exp(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->type().isint()) {
    ArrayLit* al_c = eval_array_lit(env, c->arg(0));
    std::vector<IntVal> coeffs(al_c->size());
    for (unsigned int j = 0; j < al_c->size(); j++) {
      coeffs[j] = eval_int(env, (*al_c)[j]);
    }
    ArrayLit* al_x = eval_array_lit(env, c->arg(1));
    std::vector<KeepAlive> x(al_x->size());
    for (unsigned int j = 0; j < al_x->size(); j++) {
      x[j] = (*al_x)[j];
    }
    IntVal d = eval_int(env, c->arg(2));
    simplify_lin<IntLit>(coeffs, x, d);
    if (coeffs.empty()) {
      rewrite = IntLit::a(d);
      return OptimizeRegistry::CS_REWRITE;
    }
    if (coeffs.size() < al_c->size()) {
      if (coeffs.size() == 1 && coeffs[0] == 1 && d == 0) {
        rewrite = x[0]();
        return OptimizeRegistry::CS_REWRITE;
      }

      std::vector<Expression*> coeffs_e(coeffs.size());
      std::vector<Expression*> x_e(coeffs.size());
      for (unsigned int j = 0; j < coeffs.size(); j++) {
        coeffs_e[j] = IntLit::a(coeffs[j]);
        x_e[j] = x[j]();
      }
      auto* al_c_new = new ArrayLit(Expression::loc(al_c), coeffs_e);
      al_c_new->type(Type::parint(1));
      auto* al_x_new = new ArrayLit(Expression::loc(al_x), x_e);
      al_x_new->type(al_x->type());

      std::vector<Expression*> args(3);
      args[0] = al_c_new;
      args[1] = al_x_new;
      args[2] = IntLit::a(d);
      Call* nc = Call::a(Location(), c->id(), args);
      nc->type(c->type());
      for (ExpressionSetIter it = Expression::ann(c).begin(); it != Expression::ann(c).end();
           ++it) {
        Expression::addAnnotation(nc, *it);
      }
      rewrite = nc;
      return OptimizeRegistry::CS_REWRITE;
    }
  }
  return OptimizeRegistry::CS_OK;
}

/// `o_element` for the shape the FZnSO registry gives element.
///
/// The registry opens with the array and carries the index offset the caller
/// numbers it from, where FlatZinc puts the index first and is always
/// one-based. Same simplification — a fixed index picks its element out — but
/// it cannot be the same function, because an optimiser reads its arguments by
/// position.
OptimizeRegistry::ConstraintStatus o_element_offset(EnvI& env, Item* i, Call* c,
                                                    Expression*& rewrite) {
  if (!Expression::isa<IntLit>(c->arg(2))) {
    return OptimizeRegistry::CS_OK;
  }
  ArrayLit* al = eval_array_lit(env, c->arg(0));
  IntVal idx = eval_int(env, c->arg(2)) - eval_int(env, c->arg(1)) + 1;
  if (idx < 1 || idx > al->size()) {
    return OptimizeRegistry::CS_FAILED;
  }
  std::vector<Expression*> args(2);
  args[0] = (*al)[static_cast<int>(idx.toInt()) - 1];
  args[1] = c->arg(3);
  rewrite = Call::a(Location(), env.constants.ids.int_.eq, args);
  return OptimizeRegistry::CS_REWRITE;
}

OptimizeRegistry::ConstraintStatus o_element(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (Expression::isa<IntLit>(c->arg(0))) {
    IntVal idx = eval_int(env, c->arg(0));
    ArrayLit* al = eval_array_lit(env, c->arg(1));
    if (idx < 1 || idx > al->size()) {
      return OptimizeRegistry::CS_FAILED;
    }
    Expression* result = (*al)[static_cast<int>(idx.toInt()) - 1];
    std::vector<Expression*> args(2);
    args[0] = result;
    args[1] = c->arg(2);
    Call* eq = Call::a(Location(), env.constants.ids.int_.eq, args);
    rewrite = eq;
    return OptimizeRegistry::CS_REWRITE;
  }
  return OptimizeRegistry::CS_OK;
}

/// Whether a clause literal is already decided: 1 for true, 0 for false, -1 if
/// it is still a decision.
///
/// A fixed variable is one whose domain has closed to a single value. It is
/// still an `Id` of type `var bool`, so it does not answer to `isPar` and has
/// to be read through its declaration.
int fixed_bool_lit(EnvI& env, Expression* e) {
  if (Expression::type(e).isPar()) {
    return eval_bool(env, e) ? 1 : 0;
  }
  if (Id* ident = Expression::dynamicCast<Id>(e)) {
    Expression* dom = ident->decl()->ti()->domain();
    if (dom == env.constants.literalTrue) {
      return 1;
    }
    if (dom == env.constants.literalFalse) {
      return 0;
    }
  }
  return -1;
}

/// Simplify `bool_clause(pos, neg)`, which holds when some `pos` is true or
/// some `neg` is false.
///
/// Three things are decided here. A literal that is already true satisfies the
/// whole clause; a literal that is already false cannot contribute and is
/// dropped; and a variable appearing on both sides makes the clause a
/// tautology. Dropping every literal leaves the empty clause, which is a
/// contradiction.
///
/// Without this, a clause whose literals have all been fixed elsewhere stays in
/// the model saying nothing. That is not hypothetical: rewriting `bool_eq`,
/// `bool_le` and `bool_lt` into clauses produces pairs that unification then
/// fixes, and they were the bulk of the constraints the FZnSO path emitted over
/// the FlatZinc one.
///
/// The rewrite is guarded on a literal actually being dropped, so it cannot
/// re-enter itself.
OptimizeRegistry::ConstraintStatus o_clause(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  ArrayLit* al_pos = eval_array_lit(env, c->arg(0));
  ArrayLit* al_neg = eval_array_lit(env, c->arg(1));
  std::vector<Expression*> pos_e;
  std::vector<Expression*> neg_e;
  std::vector<VarDecl*> pos;
  std::vector<VarDecl*> neg;

  for (unsigned int j = 0; j < al_pos->size(); j++) {
    Expression* e = (*al_pos)[j];
    int fixed = fixed_bool_lit(env, e);
    if (fixed == 1) {
      return OptimizeRegistry::CS_ENTAILED;
    }
    if (fixed == 0) {
      continue;
    }
    pos_e.push_back(e);
    if (Id* ident = Expression::dynamicCast<Id>(e)) {
      pos.push_back(ident->decl());
    }
  }
  for (unsigned int j = 0; j < al_neg->size(); j++) {
    Expression* e = (*al_neg)[j];
    int fixed = fixed_bool_lit(env, e);
    if (fixed == 0) {
      return OptimizeRegistry::CS_ENTAILED;
    }
    if (fixed == 1) {
      continue;
    }
    neg_e.push_back(e);
    if (Id* ident = Expression::dynamicCast<Id>(e)) {
      neg.push_back(ident->decl());
    }
  }

  if (!pos.empty() && !neg.empty()) {
    std::vector<VarDecl*> both(pos);
    std::sort(both.begin(), both.end());
    std::vector<VarDecl*> other(neg);
    std::sort(other.begin(), other.end());
    unsigned int ix = 0;
    unsigned int iy = 0;
    for (;;) {
      if (both[ix] == other[iy]) {
        return OptimizeRegistry::CS_ENTAILED;
      }
      if (both[ix] < other[iy]) {
        ix++;
      } else {
        iy++;
      }
      if (ix == both.size() || iy == other.size()) {
        break;
      }
    }
  }

  if (pos_e.empty() && neg_e.empty()) {
    return OptimizeRegistry::CS_FAILED;
  }
  if (pos_e.size() == al_pos->size() && neg_e.size() == al_neg->size()) {
    return OptimizeRegistry::CS_OK;
  }

  GCLock lock;
  auto* nal_pos = new ArrayLit(Expression::loc(al_pos), pos_e);
  nal_pos->type(al_pos->type());
  auto* nal_neg = new ArrayLit(Expression::loc(al_neg), neg_e);
  nal_neg->type(al_neg->type());
  Call* nc = Call::a(Location(), c->id(), {nal_pos, nal_neg});
  nc->type(Type::varbool());
  nc->decl(c->decl());
  for (ExpressionSetIter it = Expression::ann(c).begin(); it != Expression::ann(c).end(); ++it) {
    Expression::addAnnotation(nc, *it);
  }
  rewrite = nc;
  return OptimizeRegistry::CS_REWRITE;
}

OptimizeRegistry::ConstraintStatus o_forall(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  ArrayLit* al = eval_array_lit(env, c->arg(0));
  bool subsumed = true;
  for (unsigned int j = 0; j < al->size(); j++) {
    if (Expression::type((*al)[j]).isPar()) {
      if (!eval_bool(env, (*al)[j])) {
        return OptimizeRegistry::CS_FAILED;
      }
    } else if (Id* ident = Expression::dynamicCast<Id>((*al)[j])) {
      if (Expression* dom = ident->decl()->ti()->domain()) {
        if (dom == env.constants.literalFalse) {
          return OptimizeRegistry::CS_FAILED;
        }
      } else {
        subsumed = false;
      }
    } else {
      subsumed = false;
    }
  }
  if (subsumed) {
    return OptimizeRegistry::CS_ENTAILED;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_exists(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  ArrayLit* al = eval_array_lit(env, c->arg(0));
  bool failed = true;
  for (unsigned int j = 0; j < al->size(); j++) {
    if (Expression::type((*al)[j]).isPar()) {
      if (eval_bool(env, (*al)[j])) {
        return OptimizeRegistry::CS_ENTAILED;
      }
    } else if (Id* ident = Expression::dynamicCast<Id>((*al)[j])) {
      if (Expression* dom = ident->decl()->ti()->domain()) {
        if (dom == env.constants.literalTrue) {
          return OptimizeRegistry::CS_ENTAILED;
        }
      } else {
        failed = false;
      }
    } else {
      failed = false;
    }
  }
  if (failed) {
    return OptimizeRegistry::CS_FAILED;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_not(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->argCount() == 2) {
    Expression* e0 = c->arg(0);
    Expression* e1 = c->arg(1);
    if (Expression::type(e0).isPar() && Expression::type(e1).isPar()) {
      return eval_bool(env, e0) == eval_bool(env, e1) ? OptimizeRegistry::CS_FAILED
                                                      : OptimizeRegistry::CS_ENTAILED;
    }
    if (Expression::type(e1).isPar()) {
      std::swap(e0, e1);
    }
    if (Expression::type(e0).isPar()) {
      Call* eq = Call::a(Location(), env.constants.ids.bool_.eq,
                         {e1, env.constants.boollit(!eval_bool(env, e0))});
      rewrite = eq;
      return OptimizeRegistry::CS_REWRITE;
    }
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_div(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (Expression::type(c->arg(1)).isPar()) {
    IntVal c1v = eval_int(env, c->arg(1));
    if (Expression::type(c->arg(0)).isPar() && c->argCount() == 3 &&
        Expression::type(c->arg(2)).isPar()) {
      IntVal c0v = eval_int(env, c->arg(0));
      IntVal c2v = eval_int(env, c->arg(2));
      return (c0v / c1v == c2v) ? OptimizeRegistry::CS_ENTAILED : OptimizeRegistry::CS_FAILED;
    }
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_times(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  Expression* result = nullptr;
  Expression* arg0 = c->arg(0);
  Expression* arg1 = c->arg(1);
  if (Expression::type(arg0).isPar() && Expression::type(arg1).isPar()) {
    IntVal c0v = eval_int(env, arg0);
    IntVal c1v = eval_int(env, arg1);
    result = IntLit::a(c0v * c1v);
  } else if (Expression::type(arg0).isPar()) {
    IntVal c0v = eval_int(env, arg0);
    if (c0v == 0) {
      result = IntLit::a(0);
    } else if (c0v == 1) {
      result = arg1;
    }
  } else if (Expression::type(arg1).isPar()) {
    IntVal c1v = eval_int(env, arg1);
    if (c1v == 0) {
      result = IntLit::a(0);
    }
    if (c1v == 1) {
      result = arg0;
    }
  }

  if (result != nullptr) {
    if (c->argCount() == 2) {
      // this is the functional version of times
      rewrite = result;
      return OptimizeRegistry::CS_REWRITE;
    }  // this is the relational version of times
    assert(c->argCount() == 3);
    rewrite = Call::a(Location().introduce(), env.constants.ids.int_.eq, {c->arg(2), result});
    return OptimizeRegistry::CS_REWRITE;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_set_in(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (Expression::type(c->arg(1)).isPar()) {
    if (Expression::type(c->arg(0)).isPar()) {
      IntSetVal* isv = eval_intset(env, c->arg(1));
      return isv->contains(eval_int(env, c->arg(0))) ? OptimizeRegistry::CS_ENTAILED
                                                     : OptimizeRegistry::CS_FAILED;
    }
    if (Id* ident = Expression::dynamicCast<Id>(c->arg(0))) {
      VarDecl* vd = ident->decl();
      IntSetVal* isv = eval_intset(env, c->arg(1));
      if (vd->ti()->domain() != nullptr) {
        IntSetVal* dom = eval_intset(env, vd->ti()->domain());
        {
          IntSetRanges isv_r(isv);
          IntSetRanges dom_r(dom);
          if (Ranges::subset(dom_r, isv_r)) {
            return OptimizeRegistry::CS_ENTAILED;
          }
        }
        {
          IntSetRanges isv_r(isv);
          IntSetRanges dom_r(dom);
          if (Ranges::disjoint(dom_r, isv_r)) {
            return OptimizeRegistry::CS_FAILED;
          }
        }
      } else if (isv->min() == isv->max()) {
        std::vector<Expression*> args(2);
        args[0] = vd->id();
        args[1] = IntLit::a(isv->min());
        Call* eq = Call::a(Location(), env.constants.ids.int_.eq, args);
        rewrite = eq;
        return OptimizeRegistry::CS_REWRITE;
      }
    }
  }
  return OptimizeRegistry::CS_OK;
}

/// Fold the constants out of a Boolean parity constraint.
///
/// `bool_array_xor(xs)` holds when an odd number of `xs` do. A constant
/// contributes a fixed amount to that count, so it can be dropped and what it
/// contributed carried in the parity left to prove — which is what makes
/// `bool_array_xor([true, x])` the statement that `x` is false.
///
/// When an odd number of constants were true the rest have to hold an *even*
/// number of times, and `bool_array_xor` cannot say that on its own: one `true`
/// stays behind to carry the parity. That is why the rewrite is guarded on the
/// array actually getting shorter. Without the guard `[true, x, y]` rewrites to
/// itself and the optimiser does not terminate.
///
/// Only the unreified form is folded; `array_bool_xor(xs, b)` is a different
/// relation and takes a different number of arguments.
OptimizeRegistry::ConstraintStatus o_bool_array_xor(EnvI& env, Item* i, Call* c,
                                                    Expression*& rewrite) {
  if (c->argCount() != 1) {
    return OptimizeRegistry::CS_OK;
  }
  ArrayLit* al = eval_array_lit(env, c->arg(0));
  std::vector<Expression*> vars;
  bool odd = false;
  for (unsigned int j = 0; j < al->size(); j++) {
    Expression* e = (*al)[j];
    if (Expression::type(e).isPar()) {
      odd = odd != eval_bool(env, e);
    } else {
      vars.push_back(e);
    }
  }
  if (vars.size() == al->size()) {
    return OptimizeRegistry::CS_OK;
  }
  if (vars.empty()) {
    return odd ? OptimizeRegistry::CS_ENTAILED : OptimizeRegistry::CS_FAILED;
  }
  GCLock lock;
  if (vars.size() == 1) {
    // One left: the parity fixes it outright. Stated as an equality against a
    // constant, which the domain passes fold into the variable rather than
    // leaving a constraint behind.
    rewrite = Call::a(Location(), env.constants.ids.bool_.eq,
                      {vars[0], env.constants.boollit(!odd)});
    return OptimizeRegistry::CS_REWRITE;
  }
  if (odd) {
    vars.push_back(env.constants.literalTrue);
  }
  if (vars.size() >= al->size()) {
    return OptimizeRegistry::CS_OK;
  }
  auto* nal = new ArrayLit(Expression::loc(al), vars);
  nal->type(al->type());
  Call* nc = Call::a(Location(), c->id(), {nal});
  nc->type(Type::varbool());
  for (ExpressionSetIter it = Expression::ann(c).begin(); it != Expression::ann(c).end(); ++it) {
    Expression::addAnnotation(nc, *it);
  }
  rewrite = nc;
  return OptimizeRegistry::CS_REWRITE;
}

OptimizeRegistry::ConstraintStatus o_int_ne(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  Expression* e0 = c->arg(0);
  Expression* e1 = c->arg(1);
  if (Expression::type(e0).isPar() && Expression::type(e1).isPar()) {
    return eval_int(env, e0) != eval_int(env, e1) ? OptimizeRegistry::CS_ENTAILED
                                                  : OptimizeRegistry::CS_FAILED;
  }
  if (Expression::isa<Id>(e1)) {
    std::swap(e0, e1);
  }
  if (Id* ident = Expression::dynamicCast<Id>(e0)) {
    if (Expression::type(e1).isPar()) {
      if (ident->decl()->ti()->domain() != nullptr) {
        IntVal e1v = eval_int(env, e1);
        IntSetVal* isv = eval_intset(env, ident->decl()->ti()->domain());
        if (!isv->contains(e1v)) {
          return OptimizeRegistry::CS_ENTAILED;
        }
        if (e1v == isv->min() && e1v == isv->max()) {
          return OptimizeRegistry::CS_FAILED;
        }
      }
    }
  }

  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_int_le(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  Expression* e0 = c->arg(0);
  Expression* e1 = c->arg(1);
  if (Expression::type(e0).isPar() && Expression::type(e1).isPar()) {
    return eval_int(env, e0) <= eval_int(env, e1) ? OptimizeRegistry::CS_ENTAILED
                                                  : OptimizeRegistry::CS_FAILED;
  }
  bool swapped = false;
  if (Expression::isa<Id>(e1)) {
    std::swap(e0, e1);
    swapped = true;
  }
  if (Id* ident = Expression::dynamicCast<Id>(e0)) {
    if (Expression::type(e1).isPar()) {
      if (ident->decl()->ti()->domain() != nullptr) {
        IntVal e1v = eval_int(env, e1);
        IntSetVal* isv = eval_intset(env, ident->decl()->ti()->domain());
        if (!swapped) {
          if (isv->max() <= e1v) {
            return OptimizeRegistry::CS_ENTAILED;
          }
          if (isv->min() > e1v) {
            return OptimizeRegistry::CS_FAILED;
          }
        } else {
          if (e1v <= isv->min()) {
            return OptimizeRegistry::CS_ENTAILED;
          }
          if (e1v > isv->max()) {
            return OptimizeRegistry::CS_FAILED;
          }
        }
      }
    }
  }

  return OptimizeRegistry::CS_OK;
}

class Register {
private:
  Model* _keepAliveModel;

public:
  Register() {
    GCLock lock;
    _keepAliveModel = new Model;
    ASTString id_element("array_int_element");
    ASTString id_var_element("array_var_int_element");
    ASTString id_array_bool_xor("array_bool_xor");
    std::vector<Expression*> e;
    e.push_back(new StringLit(Location(), id_element));
    e.push_back(new StringLit(Location(), id_var_element));
    e.push_back(new StringLit(Location(), id_array_bool_xor));
    _keepAliveModel->addItem(new ConstraintI(Location(), new ArrayLit(Location(), e)));
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.lin_eq, o_linear);
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.lin_le, o_linear);
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.lin_ne, o_linear);
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.div, o_div);
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.times, o_times);
    OptimizeRegistry::registry().reg(id_element, o_element);
    OptimizeRegistry::registry().reg(Constants::constants().ids.lin_exp, o_lin_exp);
    OptimizeRegistry::registry().reg(id_var_element, o_element);
    OptimizeRegistry::registry().reg(Constants::constants().ids.clause, o_clause);
    OptimizeRegistry::registry().reg(Constants::constants().ids.bool_.clause, o_clause);
    OptimizeRegistry::registry().reg(Constants::constants().ids.forall, o_forall);
    OptimizeRegistry::registry().reg(Constants::constants().ids.exists, o_exists);
    OptimizeRegistry::registry().reg(Constants::constants().ids.bool_.not_, o_not);
    OptimizeRegistry::registry().reg(Constants::constants().ids.set_.in, o_set_in);
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.ne, o_int_ne);
    OptimizeRegistry::registry().reg(Constants::constants().ids.int_.le, o_int_le);
    OptimizeRegistry::registry().reg(id_array_bool_xor, o_bool_array_xor);

    // The same simplifications, under the names the FZnSO constraint registry
    // gives the same builtins. Behind that interface every builtin is rewritten
    // onto its registry name before reaching here, so a registration under the
    // FlatZinc spelling alone would stop firing — `int_div` on constant
    // arguments would be posted rather than folded away.
    //
    // Registered one by one rather than derived by prefixing, because the
    // registry renames as well as prefixes, and an optimiser reads its
    // arguments by position. `int_array_element` is deliberately absent: the
    // registry puts the array first and carries an index offset, so `o_element`
    // — which reads argument 0 as the index — would misread it. That one needs
    // an optimiser written for its shape, not a second name on this one.
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.int_lin_eq, o_linear);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.int_lin_le, o_linear);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.int_lin_ne, o_linear);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.int_div, o_div);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.int_times, o_times);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.bool_clause, o_clause);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.set_in, o_set_in);
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.array_bool_xor,
                                     o_bool_array_xor);
    // Element is the one that cannot share its optimiser: the registry reshapes
    // it rather than only renaming it.
    OptimizeRegistry::registry().reg(Constants::constants().ids.fznso.array_int_element,
                                     o_element_offset);
  }
  ~Register() { delete _keepAliveModel; }
} _r;  // NOLINT(bugprone-throwing-static-initialization)

}  // namespace Optimizers

}  // namespace MiniZinc
