/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astiterator.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten_linear.hh>

namespace MiniZinc {

namespace {

class DisableLinearRelationDeferral {
public:
  explicit DisableLinearRelationDeferral(EnvI& env)
      : _env(env), _previous(env.deferLinearRelations) {
    _env.deferLinearRelations = false;
  }
  ~DisableLinearRelationDeferral() { _env.deferLinearRelations = _previous; }
  DisableLinearRelationDeferral(const DisableLinearRelationDeferral&) = delete;
  DisableLinearRelationDeferral& operator=(const DisableLinearRelationDeferral&) = delete;

private:
  EnvI& _env;
  bool _previous;
};

/// Scale coefficients to a positive leading term; also remove integer GCDs.
template <class Lit>
typename LinearTraits<Lit>::Val linear_scale(
    const std::vector<typename LinearTraits<Lit>::Val>& c) {
  typedef typename LinearTraits<Lit>::Val Val;
  if (c.empty()) {
    return 1;
  }
  Val scale = LinearTraits<Lit>::commonDivisor(c);
  return c[0] < 0 ? -scale : scale;
}

template <class Lit>
void collect_linexps(EnvI& env, typename LinearTraits<Lit>::Val in_c, Expression* exp,
                     std::vector<typename LinearTraits<Lit>::Val>& coeffs,
                     std::vector<KeepAlive>& vars, typename LinearTraits<Lit>::Val& constval) {
  typedef typename LinearTraits<Lit>::Val Val;
  struct StackItem {
    Expression* e;
    Val c;
    StackItem(Expression* e0, Val c0) : e(e0), c(c0) {}
  };
  std::vector<StackItem> stack;
  stack.push_back(StackItem(exp, in_c));
  while (!stack.empty()) {
    Expression* e = stack.back().e;
    Val c = stack.back().c;
    stack.pop_back();
    if (e == nullptr) {
      continue;
    }
    if (Expression::type(e).isPar()) {
      constval += c * LinearTraits<Lit>::eval(env, e);
    } else if (Lit* l = Expression::dynamicCast<Lit>(e)) {
      constval += c * LinearTraits<Lit>::v(l);
    } else if (auto* bo = Expression::dynamicCast<BinOp>(e)) {
      if (bo->decl() != nullptr && bo->decl()->e() != nullptr) {
        // This is an overloaded operator (e.g. on option types), so do not aggregate
        coeffs.push_back(c);
        vars.emplace_back(e);
      } else {
        switch (bo->op()) {
          case BOT_PLUS:
            stack.push_back(StackItem(bo->lhs(), c));
            stack.push_back(StackItem(bo->rhs(), c));
            break;
          case BOT_MINUS:
            stack.push_back(StackItem(bo->lhs(), c));
            stack.push_back(StackItem(bo->rhs(), -c));
            break;
          case BOT_MULT:
            if (Expression::type(bo->lhs()).isPar()) {
              stack.push_back(StackItem(bo->rhs(), c * LinearTraits<Lit>::eval(env, bo->lhs())));
            } else if (Expression::type(bo->rhs()).isPar()) {
              stack.push_back(StackItem(bo->lhs(), c * LinearTraits<Lit>::eval(env, bo->rhs())));
            } else {
              coeffs.push_back(c);
              vars.emplace_back(e);
            }
            break;
          case BOT_DIV:
            if (Expression::isa<FloatLit>(bo->rhs()) &&
                FloatLit::v(Expression::cast<FloatLit>(bo->rhs())) == 1.0) {
              stack.push_back(StackItem(bo->lhs(), c));
            } else {
              coeffs.push_back(c);
              vars.emplace_back(e);
            }
            break;
          case BOT_IDIV:
            if (Expression::isa<IntLit>(bo->rhs()) &&
                IntLit::v(Expression::cast<IntLit>(bo->rhs())) == 1) {
              stack.push_back(StackItem(bo->lhs(), c));
            } else {
              coeffs.push_back(c);
              vars.emplace_back(e);
            }
            break;
          default:
            coeffs.push_back(c);
            vars.emplace_back(e);
            break;
        }
      }
    } else {
      coeffs.push_back(c);
      vars.emplace_back(e);
    }
  }
}

}  // namespace

bool is_linear_sum(EnvI& env, Expression* e) {
  Call* c = Expression::dynamicCast<Call>(e);
  if (c == nullptr || c->id() != env.constants.ids.lin_exp) {
    return false;
  }
  auto* al = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(1)));
  return al != nullptr && al->size() > 1;
}

bool linear_keeps_name(EnvI& env, VarDecl* vd, Call* def) {
  if (!is_linear_sum(env, def)) {
    return false;
  }
  if (is_output(vd) || env.hasReverseMapper(vd->id())) {
    return true;
  }
  // Share expressions used by multiple items; inline single-use expressions.
  return env.varOccurrences.occurrences(vd) > 1;
}

bool linear_barrier(EnvI& env, VarDecl* vd) {
  Call* def = Expression::dynamicCast<Call>(vd->e());
  if (def == nullptr) {
    return false;
  }
  return env.deferLinearRelations ? is_linear_sum(env, def) : linear_keeps_name(env, vd, def);
}

/// Drop an unread definition, preserving its range domain as constraints.
/// Definitions with non-contiguous domains must keep their variable.
template <class Lit>
bool release_linear_definition(EnvI& env, VarDeclI* vdi, Call* def) {
  typedef typename LinearTraits<Lit>::Val Val;
  VarDecl* vd = vdi->e();
  typename LinearTraits<Lit>::Domain dom = LinearTraits<Lit>::evalDomain(env, vd->ti()->domain());
  if (dom == nullptr || dom->empty()) {
    return false;
  }
  // Only what fits a single linear constraint is posted: the bounds, and a `!=`
  // for each hole of one value. A wider gap does not, so the variable stays.
  std::vector<Val> holes;
  for (unsigned int j = 0; j + 1 < dom->size(); j++) {
    if (!std::is_same<Lit, IntLit>::value || dom->min(j + 1) != dom->max(j) + Val(2)) {
      return false;
    }
    holes.push_back(dom->max(j) + Val(1));
  }
  typename LinearTraits<Lit>::Bounds bounds = LinearTraits<Lit>::computeBounds(env, def);
  std::vector<VarDecl*> removed;
  CollectDecls cd(env, env.varOccurrences, removed, vdi);
  top_down(cd, def);
  vd->e(nullptr);
  vd->ti()->domain(nullptr);
  vd->ti()->setComputedDomain(true);
  env.cseMapRemove(def);
  env.flatRemoveItem(vdi);
  // The expression is not getting a name, so the bound goes straight into it;
  // deferring it again would capture the expression in a new variable, whose
  // domain would absorb the bound, and so on.
  DisableLinearRelationDeferral disableDeferral(env);
  auto post = [&](BinOpType bot, Val bound) {
    auto* bo = new BinOp(Expression::loc(vd), def, bot, LinearTraits<Lit>::newLit(bound));
    bo->type(Type::varbool());
    (void)flat_exp(env, Ctx(), bo, env.constants.varTrue, env.constants.varTrue);
  };
  if (dom->min() == dom->max()) {
    post(BOT_EQ, dom->min());
  } else {
    // Skip bounds already implied by the expression.
    if (!bounds.valid || dom->max() < bounds.u) {
      post(BOT_LQ, dom->max());
    }
    if (!bounds.valid || dom->min() > bounds.l) {
      post(BOT_GQ, dom->min());
    }
    for (const Val& h : holes) {
      post(BOT_NQ, h);
    }
  }
  return true;
}

bool release_linear_definition(EnvI& env, VarDeclI* vdi) {
  VarDecl* vd = vdi->e();
  Call* def = Expression::dynamicCast<Call>(vd->e());
  if (def == nullptr || def->id() != env.constants.ids.lin_exp || vd->ti()->domain() == nullptr ||
      vd->ti()->computedDomain()) {
    return false;
  }
  GCLock lock;
  if (vd->type().isint()) {
    return release_linear_definition<IntLit>(env, vdi, def);
  }
  if (vd->type().isfloat()) {
    return release_linear_definition<FloatLit>(env, vdi, def);
  }
  return false;
}

Call* linear_definition_call(EnvI& env, VarDecl* vd, const Call* def) {
  const bool isInt = Expression::type(def).bt() == Type::BT_INT;
  auto* defC = Expression::cast<ArrayLit>(follow_id(def->arg(0)));
  std::vector<Expression*> coeffs(defC->size());
  for (unsigned int i = 0; i < defC->size(); i++) {
    coeffs[i] = (*defC)[i];
  }
  coeffs.push_back(isInt ? static_cast<Expression*>(IntLit::a(-1))
                         : static_cast<Expression*>(FloatLit::a(-1.0)));
  auto* defX = Expression::cast<ArrayLit>(follow_id(def->arg(1)));
  std::vector<Expression*> vars(defX->size());
  for (unsigned int i = 0; i < defX->size(); i++) {
    vars[i] = (*defX)[i];
  }
  vars.push_back(vd->id());
  std::vector<Expression*> args(3);
  args[0] = new ArrayLit(Location().introduce(), coeffs);
  Expression::type(args[0], isInt ? Type::parint(1) : Type::parfloat(1));
  args[1] = new ArrayLit(Location().introduce(), vars);
  Expression::type(args[1], defX->type());
  args[2] =
      isInt ? static_cast<Expression*>(IntLit::a(-IntLit::v(Expression::cast<IntLit>(def->arg(2)))))
            : static_cast<Expression*>(
                  FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>(def->arg(2)))));
  Call* nc = Call::a(Expression::loc(def).introduce(),
                     isInt ? env.constants.ids.int_.lin_eq : env.constants.ids.float_.lin_eq, args);
  nc->type(Type::varbool());
  nc->decl(env.model->matchFn(env, nc, false));
  return nc;
}

LinearRelation linear_relation(EnvI& env, Call* c, bool deferredOnly) {
  const auto& ids = env.constants.ids;
  const ASTString& id = c->id();
  BinOpType bot = BOT_EQ;
  VarDecl* reifiedInto = nullptr;
  bool halfReified = false;
  // Number of relation arguments before an optional reification result.
  unsigned int arity = 0;
  auto match = [&](const ASTString& base, BinOpType b, unsigned int a) {
    const ASTString reif = env.reifyId(base);
    if (id != base && id != reif && id != EnvI::halfReifyId(base)) {
      return false;
    }
    bot = b;
    arity = a;
    halfReified = id != base && id != reif;
    return true;
  };
  if (!(match(ids.int_.lt, BOT_LE, 2) || match(ids.int_.le, BOT_LQ, 2) ||
        match(ids.int_.eq, BOT_EQ, 2) || match(ids.int_.ne, BOT_NQ, 2) ||
        match(ids.float_.lt, BOT_LE, 2) || match(ids.float_.le, BOT_LQ, 2) ||
        match(ids.float_.eq, BOT_EQ, 2) || match(ids.float_.ne, BOT_NQ, 2) ||
        match(ids.int_.lin_le, BOT_LQ, 3) || match(ids.int_.lin_eq, BOT_EQ, 3) ||
        match(ids.int_.lin_ne, BOT_NQ, 3) || match(ids.float_.lin_lt, BOT_LE, 3) ||
        match(ids.float_.lin_le, BOT_LQ, 3) || match(ids.float_.lin_eq, BOT_EQ, 3) ||
        match(ids.float_.lin_ne, BOT_NQ, 3))) {
    return {};
  }
  // Rebuild scalar relations if an operand may be inlined. Strict integer
  // relations also need rebuilding because FlatZinc represents them as <=.
  const bool strictInt = bot == BOT_LE && c->argCount() >= 2 && Expression::type(c->arg(0)).isint();
  auto holdsLinearSum = [&](Expression* e) {
    Id* ident = Expression::dynamicCast<Id>(e);
    if (ident == nullptr || ident->decl() == nullptr) {
      return false;
    }
    auto* def = Expression::dynamicCast<Call>(ident->decl()->e());
    return def != nullptr && is_linear_sum(env, def) &&
           (strictInt || !linear_keeps_name(env, ident->decl(), def));
  };
  if (deferredOnly && (arity != 2 || c->argCount() < 2 ||
                       !(holdsLinearSum(c->arg(0)) || holdsLinearSum(c->arg(1))))) {
    return {};
  }
  if (c->argCount() != arity && c->argCount() != arity + 1) {
    return {};
  }
  if (c->argCount() == arity + 1) {
    Id* ident = Expression::dynamicCast<Id>(c->arg(arity));
    if (ident == nullptr) {
      return {};
    }
    reifiedInto = ident->decl();
  }
  Expression* lhs = c->arg(0);
  if (arity == 3) {
    std::vector<Expression*> args({c->arg(0), c->arg(1),
                                   Expression::type(c->arg(0)).bt() == Type::BT_INT
                                       ? static_cast<Expression*>(IntLit::a(0))
                                       : static_cast<Expression*>(FloatLit::a(0.0))});
    auto* le = Call::a(Expression::loc(c), env.constants.ids.lin_exp, args);
    le->decl(env.model->matchFn(env, le, false));
    if (le->decl() == nullptr) {
      return {};
    }
    le->type(le->decl()->rtype(env, args, nullptr, false));
    lhs = le;
  }
  auto* bo = new BinOp(Expression::loc(c), lhs, bot, c->arg(arity - 1));
  bo->type(Type::varbool());
  return {bo, reifiedInto, halfReified};
}

void rebuild_linear(EnvI& env, Item* i, Call* c, Expression* e, VarDecl* reifiedInto,
                    bool halfReified) {
  auto* vdi = i->dynamicCast<VarDeclI>();
  // Prefer an explicit reification variable, then the item's result, then true.
  VarDecl* r = reifiedInto;
  if (r == nullptr) {
    r = vdi != nullptr ? vdi->e() : env.constants.varTrue;
  }
  // Reusing the root context here would incorrectly turn reifications into facts.
  Ctx ctx;
  if (r != env.constants.varTrue &&
      !Expression::equal(r->ti()->domain(), env.constants.literalTrue)) {
    if (halfReified) {
      // Half reification is positive-only.
      ctx.b = C_POS;
    } else {
      ctx.b = std::get<0>(env.annToCtx(r));
      if (ctx.b == C_ROOT) {
        ctx.b = C_MIX;
      }
    }
  }
  // Do not rediscover the item being replaced through CSE.
  env.cseMapRemove(c);
  if (vdi != nullptr) {
    vdi->e()->e(nullptr);
  }
  // Keep this item counted as a reader until naming decisions are made.
  {
    // The names in this expression are decided, so build it rather than
    // deferring it again.
    DisableLinearRelationDeferral disableDeferral(env);
    (void)flat_exp(env, ctx, e, r, env.constants.varTrue);
  }
  std::vector<VarDecl*> removed;
  CollectDecls cd(env, env.varOccurrences, removed, i);
  top_down(cd, c);
  if (vdi != nullptr) {
    // CollectDecls also removed the replacement RHS; restore its occurrences.
    if (vdi->e()->e() != nullptr) {
      CollectOccurrencesE ce(env, env.varOccurrences, i);
      top_down(ce, vdi->e()->e());
    }
  } else {
    i->cast<ConstraintI>()->e(env.constants.literalTrue);
    i->remove();
  }
  if (Call* def = Expression::dynamicCast<Call>(r->e())) {
    Expression::ann(def).merge(Expression::ann(c));
    if (def->id() != env.constants.ids.lin_exp) {
      make_defined_var(env, r, def);
    }
  }
  // Revisit definitions whose reader count changed.
  for (VarDecl* cur : removed) {
    int idx = env.varOccurrences.find(cur);
    if (idx >= 0) {
      env.modifiedVarDecls.push_back(idx);
    }
  }
}

void inline_linear_definition(EnvI& env, VarDeclI* vdi) {
  auto items = env.varOccurrences.itemMap.find(vdi->e()->id());
  if (!items.first) {
    return;
  }
  const std::vector<Item*> readers(items.second->begin(), items.second->end());
  for (Item* reader : readers) {
    if (reader->removed()) {
      continue;
    }
    // A reader that is neither of these (a solve item's annotation, say) cannot
    // take the terms, and keeps counting so that the definition is built.
    Expression* read = nullptr;
    if (auto* readerVdi = reader->dynamicCast<VarDeclI>()) {
      read = readerVdi->e()->e();
    } else if (auto* readerCi = reader->dynamicCast<ConstraintI>()) {
      read = readerCi->e();
    }
    Call* rc = Expression::dynamicCast<Call>(read);
    if (rc == nullptr) {
      // An array only passes the terms on: inline them into the linear
      // expressions reading the array, and drop the array once nothing does.
      auto* arrayVdi = reader->dynamicCast<VarDeclI>();
      if (arrayVdi != nullptr && Expression::isa<ArrayLit>(read)) {
        inline_linear_definition(env, arrayVdi);
        if (!is_output(arrayVdi->e()) && env.varOccurrences.occurrences(arrayVdi->e()) == 0) {
          std::vector<VarDecl*> removed;
          CollectDecls cd(env, env.varOccurrences, removed, arrayVdi);
          top_down(cd, read);
          env.flatRemoveItem(arrayVdi);
        }
      }
      continue;
    }
    if (rc->id() == env.constants.ids.lin_exp) {
      rebuild_linear(env, reader, rc, rc, nullptr, false);
      continue;
    }
    LinearRelation relation = linear_relation(env, rc, false);
    if (relation.comparison != nullptr) {
      rebuild_linear(env, reader, rc, relation.comparison, relation.reifiedInto,
                     relation.halfReified);
    }
  }
}

template <class Lit>
KeepAlive mklinexp(EnvI& env, typename LinearTraits<Lit>::Val c0,
                   typename LinearTraits<Lit>::Val c1, Expression* e0, Expression* e1) {
  typedef typename LinearTraits<Lit>::Val Val;
  GCLock lock;

  std::vector<Val> coeffs;
  std::vector<KeepAlive> vars;
  Val constval = 0;
  collect_linexps<Lit>(env, c0, e0, coeffs, vars, constval);
  collect_linexps<Lit>(env, c1, e1, coeffs, vars, constval);
  simplify_lin<Lit>(coeffs, vars, constval);
  KeepAlive ka;
  if (coeffs.empty()) {
    ka = LinearTraits<Lit>::newLit(constval);
  } else if (coeffs.size() == 1 && coeffs[0] == 1 && constval == 0) {
    ka = vars[0];
  } else {
    std::vector<Expression*> coeffs_e(coeffs.size());
    for (auto i = static_cast<unsigned int>(coeffs.size()); i--;) {
      if (!LinearTraits<Lit>::finite(coeffs[i])) {
        throw FlatteningError(
            env, Expression::loc(e0),
            "unbounded coefficient in linear expression."
            " Make sure variables involved in non-linear/logical expressions have finite bounds"
            " in their definition or via constraints");
      }
      coeffs_e[i] = LinearTraits<Lit>::newLit(coeffs[i]);
    }
    std::vector<Expression*> vars_e(vars.size());
    for (auto i = static_cast<unsigned int>(vars.size()); i--;) {
      vars_e[i] = vars[i]();
    }

    std::vector<Expression*> args(3);
    args[0] = new ArrayLit(Expression::loc(e0), coeffs_e);
    Type t = Type::arrType(env, Type::partop(1), Expression::type(coeffs_e[0]));
    Expression::type(args[0], t);
    args[1] = new ArrayLit(Expression::loc(e0), vars_e);
    Type tt = Type::arrType(env, Type::partop(1), Expression::type(vars_e[0]));
    Expression::type(args[1], tt);
    args[2] = LinearTraits<Lit>::newLit(constval);
    Call* c = Call::a(Expression::loc(e0).introduce(), env.constants.ids.lin_exp, args);
    add_path_annotation(env, c);
    c->decl(env.model->matchFn(env, c, false));
    if (c->decl() == nullptr) {
      throw FlatteningError(env, Expression::loc(c), "cannot find matching declaration");
    }
    c->type(c->decl()->rtype(env, args, nullptr, false));
    ka = c;
  }
  assert(ka());
  return ka;
}

template <class Lit>
Expression* get_linexp(EnvI& env, Expression* e, bool isDefinition) {
  Expression* prev_e = nullptr;
  for (;;) {
    if (e && Expression::eid(e) == Expression::E_ID && e != env.constants.absent) {
      VarDecl* decl = Expression::cast<Id>(e)->decl();
      if ((decl->e() != nullptr) && (isDefinition || !linear_barrier(env, decl))) {
        prev_e = e;
        e = decl->e();
      } else {
        break;
      }
    } else {
      break;
    }
  }
  if (e && (Expression::isa<Id>(e) || Expression::isa<Lit>(e) ||
            (Expression::isa<Call>(e) &&
             Expression::cast<Call>(e)->id() == env.constants.ids.lin_exp))) {
    return e;
  }
  if (prev_e != nullptr) {
    return prev_e;
  }
  return nullptr;
}

template <class Lit>
void flatten_linexp_binop(EnvI& env, const Ctx& ctx, VarDecl* r, VarDecl* b, EE& ret,
                          Expression* le0, Expression* le1, BinOpType& bot, bool doubleNeg,
                          std::vector<EE>& ees, std::vector<KeepAlive>& args, ASTString& callid) {
  typedef typename LinearTraits<Lit>::Val Val;
  std::vector<Val> coeffv;
  std::vector<KeepAlive> alv;
  Val d = 0;
  Expression* le[2] = {le0, le1};

  // Assign linear expression directly if one side is an Id.
  Id* assignTo = nullptr;
  if (bot == BOT_EQ && ctx.b == C_ROOT) {
    if (Expression::isa<Id>(le0)) {
      assignTo = Expression::cast<Id>(le0);
    } else if (Expression::isa<Id>(le1)) {
      assignTo = Expression::cast<Id>(le1);
    }
  } else {
    if (Expression::type(le0).isPar()) {
      Val v0 = LinearTraits<Lit>::eval(env, le0);
      if (!v0.isFinite()) {
        bool result;
        switch (bot) {
          case BOT_NQ:
            result = true;
            break;
          case BOT_LE:
          case BOT_LQ:
            result = v0.isMinusInfinity();
            break;
          case BOT_GR:
          case BOT_GQ:
            result = v0.isPlusInfinity();
            break;
          case BOT_EQ:
            result = false;
            break;
          default:
            assert(false);
        }
        if (doubleNeg) {
          result = !result;
        }
        ees[2].b = env.constants.boollit(result);
        ret.r = conj(env, r, ctx, ees);
        return;
      }
    } else if (Expression::type(le1).isPar()) {
      Val v1 = LinearTraits<Lit>::eval(env, le1);
      if (!v1.isFinite()) {
        bool result;
        switch (bot) {
          case BOT_NQ:
            result = true;
            break;
          case BOT_LE:
          case BOT_LQ:
            result = v1.isPlusInfinity();
            break;
          case BOT_GR:
          case BOT_GQ:
            result = v1.isMinusInfinity();
            break;
          case BOT_EQ:
            result = false;
            break;
          default:
            assert(false);
        }
        if (doubleNeg) {
          result = !result;
        }
        ees[2].b = env.constants.boollit(result);
        ret.r = conj(env, r, ctx, ees);
        return;
      }
    }
  }

  for (unsigned int i = 0; i < 2; i++) {
    Val sign = (i == 0 ? 1 : -1);
    if (Lit* l = Expression::dynamicCast<Lit>(le[i])) {
      try {
        d += sign * LinearTraits<Lit>::v(l);
      } catch (ArithmeticError& e) {
        throw EvalError(env, Expression::loc(l), e.msg());
      }
    } else if (Expression::isa<Id>(le[i])) {
      coeffv.push_back(sign);
      alv.emplace_back(le[i]);
    } else if (Call* sc = Expression::dynamicCast<Call>(le[i])) {
      GCLock lock;
      ArrayLit* sc_coeff = eval_array_lit(env, sc->arg(0));
      ArrayLit* sc_al = eval_array_lit(env, sc->arg(1));
      try {
        d += sign * LinearTraits<Lit>::eval(env, sc->arg(2));
        for (unsigned int j = 0; j < sc_coeff->size(); j++) {
          coeffv.push_back(sign * LinearTraits<Lit>::eval(env, (*sc_coeff)[j]));
          alv.emplace_back((*sc_al)[j]);
        }
      } catch (ArithmeticError& e) {
        throw EvalError(env, Expression::loc(sc), e.msg());
      }

    } else {
      throw EvalError(env, Expression::loc(le[i]),
                      "Internal error, unexpected expression inside linear expression");
    }
  }
  simplify_lin<Lit>(coeffv, alv, d);
  if ((bot == BOT_EQ || bot == BOT_NQ) && coeffv.size() > 1 && coeffv[0] < 0) {
    // Normalise the sign of symmetric relations for CSE.
    for (auto& cv : coeffv) {
      cv = -cv;
    }
    d = -d;
  }
  if (coeffv.empty()) {
    bool result;
    switch (bot) {
      case BOT_LE:
        result = (0 < -d);
        break;
      case BOT_LQ:
        result = (0 <= -d);
        break;
      case BOT_GR:
        result = (0 > -d);
        break;
      case BOT_GQ:
        result = (0 >= -d);
        break;
      case BOT_EQ:
        result = (0 == -d);
        break;
      case BOT_NQ:
        result = (0 != -d);
        break;
      default:
        assert(false);
        break;
    }
    if (doubleNeg) {
      result = !result;
    }
    ees[2].b = env.constants.boollit(result);
    ret.r = conj(env, r, ctx, ees);
    return;
  }
  if (coeffv.size() == 1 && abs(coeffv[0]) == 1) {
    if (coeffv[0] == -1) {
      switch (bot) {
        case BOT_LE:
          bot = BOT_GR;
          break;
        case BOT_LQ:
          bot = BOT_GQ;
          break;
        case BOT_GR:
          bot = BOT_LE;
          break;
        case BOT_GQ:
          bot = BOT_LQ;
          break;
        default:
          break;
      }
    } else {
      d = -d;
    }
    typename LinearTraits<Lit>::Bounds ib = LinearTraits<Lit>::computeBounds(env, alv[0]());
    if (ib.valid) {
      bool failed = false;
      bool subsumed = false;
      switch (bot) {
        case BOT_LE:
          subsumed = ib.u < d;
          failed = ib.l >= d;
          break;
        case BOT_LQ:
          subsumed = ib.u <= d;
          failed = ib.l > d;
          break;
        case BOT_GR:
          subsumed = ib.l > d;
          failed = ib.u <= d;
          break;
        case BOT_GQ:
          subsumed = ib.l >= d;
          failed = ib.u < d;
          break;
        case BOT_EQ:
          subsumed = ib.l == d && ib.u == d;
          failed = ib.u < d || ib.l > d;
          break;
        case BOT_NQ:
          subsumed = ib.u < d || ib.l > d;
          failed = ib.l == d && ib.u == d;
          break;
        default:
          break;
      }
      if (doubleNeg) {
        std::swap(subsumed, failed);
      }
      if (subsumed) {
        ees[2].b = env.constants.literalTrue;
        ret.r = conj(env, r, ctx, ees);
        return;
      }
      if (failed) {
        ees[2].b = env.constants.literalFalse;
        ret.r = conj(env, r, ctx, ees);
        return;
      }
    }

    if (ctx.b == C_ROOT && Expression::isa<Id>(alv[0]()) && bot == BOT_EQ) {
      GCLock lock;
      VarDecl* vd = Expression::cast<Id>(alv[0]())->decl();
      if (vd->ti()->domain()) {
        typename LinearTraits<Lit>::Domain domain =
            LinearTraits<Lit>::evalDomain(env, vd->ti()->domain());
        if (LinearTraits<Lit>::domainContains(domain, d)) {
          if (!LinearTraits<Lit>::domainEquals(domain, d)) {
            set_computed_domain(env, vd, LinearTraits<Lit>::newDomain(d), false);
          }
          ret.r = bind(env, ctx, r, env.constants.literalTrue);
        } else {
          ret.r = bind(env, ctx, r, env.constants.literalFalse);
        }
      } else {
        set_computed_domain(env, vd, LinearTraits<Lit>::newDomain(d), false);
        ret.r = bind(env, ctx, r, env.constants.literalTrue);
      }
    } else {
      GCLock lock;
      Expression* e0;
      Expression* e1;
      BinOpType old_bot = bot;
      Val old_d = d;
      switch (bot) {
        case BOT_LE:
          e0 = alv[0]();
          if (Expression::type(e0).isint()) {
            d--;
            bot = BOT_LQ;
          }
          e1 = LinearTraits<Lit>::newLit(d);
          break;
        case BOT_GR:
          e1 = alv[0]();
          if (Expression::type(e1).isint()) {
            d++;
            bot = BOT_LQ;
          } else {
            bot = BOT_LE;
          }
          e0 = LinearTraits<Lit>::newLit(d);
          break;
        case BOT_GQ:
          e0 = LinearTraits<Lit>::newLit(d);
          e1 = alv[0]();
          bot = BOT_LQ;
          break;
        default:
          e0 = alv[0]();
          e1 = LinearTraits<Lit>::newLit(d);
      }
      if (ctx.b == C_ROOT && Expression::isa<Id>(alv[0]()) &&
          !env.hasReverseMapper(Expression::cast<Id>(alv[0]())) &&
          Expression::cast<Id>(alv[0]())->decl()->ti()->domain()) {
        VarDecl* vd = Expression::cast<Id>(alv[0]())->decl();
        typename LinearTraits<Lit>::Domain domain =
            LinearTraits<Lit>::evalDomain(env, vd->ti()->domain());
        typename LinearTraits<Lit>::Domain ndomain =
            LinearTraits<Lit>::limitDomain(old_bot, domain, old_d);
        if (domain && ndomain) {
          if (LinearTraits<Lit>::domainEmpty(ndomain)) {
            ret.r = bind(env, ctx, r, env.constants.literalFalse);
            return;
          }
          if (!LinearTraits<Lit>::domainEquals(domain, ndomain)) {
            ret.r = bind(env, ctx, r, env.constants.literalTrue);
            set_computed_domain(env, vd, LinearTraits<Lit>::newDomain(ndomain), false);

            if (r == env.constants.varTrue) {
              auto* bo = new BinOp(Location().introduce(), e0, bot, e1);
              bo->type(Type::varbool());
              std::vector<Expression*> boargs(2);
              boargs[0] = e0;
              boargs[1] = e1;
              Call* c = Call::a(Location(), op_to_builtin(env, e0, e1, bot), boargs);
              c->type(Type::varbool());
              c->decl(env.model->matchFn(env, c, false));
              auto it = env.cseMapFind(c);
              if (it != env.cseMapEnd()) {
                if (Id* ident = Expression::dynamicCast<Id>(it->second.r)) {
                  bind(env, Ctx(), ident->decl(), env.constants.literalTrue);
                  it->second.r = env.constants.literalTrue;
                }
                if (Id* ident = Expression::dynamicCast<Id>(it->second.b)) {
                  bind(env, Ctx(), ident->decl(), env.constants.literalTrue);
                  it->second.b = env.constants.literalTrue;
                }
              }
            }
          }
          return;
        }
      }
      args.emplace_back(e0);
      args.emplace_back(e1);
    }
  } else if (bot == BOT_EQ && coeffv.size() == 2 && coeffv[0] == -coeffv[1] && d == 0) {
    Id* id0 = Expression::cast<Id>(alv[0]());
    Id* id1 = Expression::cast<Id>(alv[1]());
    if (ctx.b == C_ROOT && r == env.constants.varTrue &&
        (id0->decl()->e() == nullptr || id1->decl()->e() == nullptr)) {
      if (id0->decl()->e() == nullptr &&
          (id1->decl()->e() != nullptr ||
           (id0->decl()->introduced() && !id1->decl()->introduced()))) {
        (void)bind(env, ctx, id0->decl(), id1);
      } else {
        (void)bind(env, ctx, id1->decl(), id0);
      }
    } else {
      callid = LinearTraits<Lit>::id_eq();
      args.emplace_back(alv[0]());
      args.emplace_back(alv[1]());
    }
  } else {
    GCLock lock;
    if (assignTo != nullptr) {
      Val resultCoeff = 0;
      typename LinearTraits<Lit>::Bounds bounds(d, d, true);
      for (auto i = static_cast<unsigned int>(coeffv.size()); i--;) {
        if (alv[i]() == assignTo) {
          resultCoeff = coeffv[i];
          continue;
        }
        typename LinearTraits<Lit>::Bounds bound = LinearTraits<Lit>::computeBounds(env, alv[i]());

        if (bound.valid && LinearTraits<Lit>::finite(bound)) {
          if (coeffv[i] > 0) {
            bounds.l += coeffv[i] * bound.l;
            bounds.u += coeffv[i] * bound.u;
          } else {
            bounds.l += coeffv[i] * bound.u;
            bounds.u += coeffv[i] * bound.l;
          }
        } else {
          bounds.valid = false;
          break;
        }
      }
      if (bounds.valid && resultCoeff != 0) {
        if (resultCoeff < 0) {
          bounds.l = LinearTraits<Lit>::floorDiv(bounds.l, -resultCoeff);
          bounds.u = LinearTraits<Lit>::ceilDiv(bounds.u, -resultCoeff);
        } else {
          Val bl = bounds.l;
          bounds.l = LinearTraits<Lit>::ceilDiv(bounds.u, -resultCoeff);
          bounds.u = LinearTraits<Lit>::floorDiv(bl, -resultCoeff);
        }
        VarDecl* vd = assignTo->decl();
        if (vd->ti()->domain()) {
          typename LinearTraits<Lit>::Domain domain =
              LinearTraits<Lit>::evalDomain(env, vd->ti()->domain());
          if (LinearTraits<Lit>::domainIntersects(domain, bounds.l, bounds.u)) {
            typename LinearTraits<Lit>::Domain new_domain =
                LinearTraits<Lit>::intersectDomain(domain, bounds.l, bounds.u);
            if (!LinearTraits<Lit>::domainEquals(domain, new_domain)) {
              set_computed_domain(env, vd, LinearTraits<Lit>::newDomain(new_domain), false);
            }
          } else {
            ret.r = bind(env, ctx, r, env.constants.literalFalse);
          }
        } else {
          // Can only set as computed if there was no other RHS
          bool is_computed = vd->e() == nullptr;
          set_computed_domain(env, vd, LinearTraits<Lit>::newDomain(bounds.l, bounds.u),
                              is_computed);
        }
      }
    }

    int coeff_sign;
    LinearTraits<Lit>::constructLinBuiltin(env, bot, callid, coeff_sign, d);

    // Remove a common integer factor. At this point the relation is
    // `sum <= -d`, `sum = -d`, or `sum != -d`.
    if (std::is_same<Lit, IntLit>::value) {
      Val g = LinearTraits<Lit>::commonDivisor(coeffv);
      if (g > 1) {
        Val rhs = -d;
        const bool isEq = callid == LinearTraits<Lit>::id_lin_eq();
        const bool isNe = callid == LinearTraits<Lit>::id_lin_ne();
        if ((isEq || isNe) && rhs % g != 0) {
          // The equality is impossible when g does not divide its bound.
          bool result = isNe;
          if (doubleNeg) {
            result = !result;
          }
          ees[2].b = env.constants.boollit(result);
          ret.r = conj(env, r, ctx, ees);
          return;
        }
        rhs = (isEq || isNe) ? rhs / g : LinearTraits<Lit>::floorDiv(rhs, g);
        for (auto& cv : coeffv) {
          cv = cv / g;
        }
        d = -rhs;
      }
    }
    std::vector<Expression*> coeff_ev(coeffv.size());
    for (auto i = static_cast<unsigned int>(coeff_ev.size()); i--;) {
      coeff_ev[i] = LinearTraits<Lit>::newLit(coeff_sign * coeffv[i]);
    }
    auto* ncoeff = new ArrayLit(Location().introduce(), coeff_ev);
    Type t = Type::arrType(env, Type::partop(1), Expression::type(coeff_ev[0]));
    ncoeff->type(t);
    args.emplace_back(ncoeff);
    std::vector<Expression*> alv_e(alv.size());
    Type tt = Type::arrType(env, Type::partop(1), Expression::type(alv[0]()));
    for (auto i = static_cast<unsigned int>(alv.size()); i--;) {
      if (Expression::type(alv[i]()).isvar()) {
        tt.mkVar(env);
      }
      alv_e[i] = alv[i]();
    }
    auto* nal = new ArrayLit(Location().introduce(), alv_e);
    nal->type(tt);
    args.emplace_back(nal);
    Lit* il = LinearTraits<Lit>::newLit(-d);
    args.push_back(il);
  }
}
Expression* follow_id_linear(EnvI& env, Expression* e) {
  for (;;) {
    if (e == nullptr) {
      return nullptr;
    }
    if (Expression::eid(e) != Expression::E_ID) {
      return e;
    }
    Id* ident = Expression::cast<Id>(e);
    if (ident == env.constants.absent || (ident->type().isAnn() && ident->decl() == nullptr)) {
      return ident;
    }
    if (linear_barrier(env, ident->decl())) {
      return e;
    }
    e = ident->decl()->e();
  }
}

Call* same_call(EnvI& env, Expression* e, const ASTString& id) {
  assert(GC::locked());
  Expression* ce = follow_id_linear(env, e);
  Call* c = Expression::dynamicCast<Call>(ce);
  if (c != nullptr) {
    if (c->id() == id) {
      return Expression::cast<Call>(ce);
    }
    if (c->id() == env.constants.ids.int2float) {
      Expression* i2f = follow_id_linear(env, c->arg(0));
      Call* i2fc = Expression::dynamicCast<Call>(i2f);
      if ((i2fc != nullptr) && i2fc->id() == id && id == env.constants.ids.lin_exp) {
        ArrayLit* coeffs = eval_array_lit(env, i2fc->arg(0));
        std::vector<Expression*> ncoeff_v(coeffs->size());
        for (unsigned int i = 0; i < coeffs->size(); i++) {
          ncoeff_v[i] = FloatLit::a(eval_int(env, (*coeffs)[i]));
        }
        auto* ncoeff = new ArrayLit(Expression::loc(coeffs).introduce(), ncoeff_v);
        ncoeff->type(Type::parfloat(1));
        ArrayLit* vars = eval_array_lit(env, i2fc->arg(1));
        std::vector<Expression*> n_vars_v(vars->size());
        for (unsigned int i = 0; i < vars->size(); i++) {
          Call* f2i = Call::a(Expression::loc((*vars)[i]).introduce(), env.constants.ids.int2float,
                              {(*vars)[i]});
          f2i->decl(env.model->matchFn(env, f2i, false));
          assert(f2i->decl());
          f2i->type(Type::varfloat());
          EE ee = flat_exp(env, Ctx(), f2i, nullptr, env.constants.varTrue);
          n_vars_v[i] = ee.r();
        }
        auto* nvars = new ArrayLit(Expression::loc(vars).introduce(), n_vars_v);
        nvars->type(Type::varfloat(1));
        FloatVal c = eval_int(env, i2fc->arg(2));
        Call* nlinexp = Call::a(Expression::loc(i2fc).introduce(), env.constants.ids.lin_exp,
                                {ncoeff, nvars, FloatLit::a(c)});
        nlinexp->decl(env.model->matchFn(env, nlinexp, false));
        assert(nlinexp->decl());
        nlinexp->type(Type::varfloat());
        return nlinexp;
      }
    }
  }
  return nullptr;
}

template <class Lit>
void flatten_linexp_call(EnvI& env, Ctx ctx, const Ctx& nctx, ASTString& cid, Call* c, EE& ret,
                         VarDecl* b, VarDecl* r, std::vector<EE>& args_ee,
                         std::vector<KeepAlive>& args) {
  typedef typename LinearTraits<Lit>::Val Val;
  Expression* al_arg = (cid == env.constants.ids.sum ? args_ee[0].r() : args_ee[1].r());
  EE flat_al = flat_exp(env, nctx, al_arg, nullptr, nctx.partialityVar(env));
  auto* al = Expression::cast<ArrayLit>(follow_id(flat_al.r()));
  // Keep a reshaped copy rooted after the GC lock is released.
  KeepAlive al_ka = al;
  if (al->dims() > 1) {
    Type alt = Type::arrType(env, Type::partop(1), al->type());
    GCLock lock;
    al = new ArrayLit(Expression::loc(al), al);
    al->type(alt);
    al_ka = al;
  }
  Val d = (cid == env.constants.ids.sum ? Val(0) : LinearTraits<Lit>::eval(env, args_ee[2].r()));

  std::vector<Val> c_coeff(al->size());
  if (cid == env.constants.ids.sum) {
    for (unsigned int i = al->size(); i--;) {
      c_coeff[i] = 1;
    }
  } else {
    EE flat_coeff = flat_exp(env, nctx, args_ee[0].r(), nullptr, nctx.partialityVar(env));
    auto* coeff = Expression::cast<ArrayLit>(follow_id(flat_coeff.r()));
    for (unsigned int i = coeff->size(); i--;) {
      c_coeff[i] = LinearTraits<Lit>::eval(env, (*coeff)[i]);
    }
  }
  cid = env.constants.ids.lin_exp;
  std::vector<Val> coeffv;
  std::vector<KeepAlive> alv;
  for (unsigned int i = 0; i < al->size(); i++) {
    GCLock lock;
    if (Call* sc = Expression::dynamicCast<Call>(same_call(env, (*al)[i], cid))) {
      if (auto* alvi_decl = Expression::dynamicCast<VarDecl>(follow_id_to_decl((*al)[i]))) {
        if (alvi_decl->ti()->domain()) {
          // A tighter declared domain is a constraint, so do not aggregate past it.
          typename LinearTraits<Lit>::Domain sc_dom =
              LinearTraits<Lit>::evalDomain(env, alvi_decl->ti()->domain());
          typename LinearTraits<Lit>::Bounds sc_bounds = LinearTraits<Lit>::computeBounds(env, sc);
          if (LinearTraits<Lit>::domainTighter(sc_dom, sc_bounds)) {
            coeffv.push_back(c_coeff[i]);
            alv.emplace_back((*al)[i]);
            continue;
          }
        }
      }

      Val cd = c_coeff[i];
      ArrayLit* sc_coeff = eval_array_lit(env, sc->arg(0));
      ArrayLit* sc_al = eval_array_lit(env, sc->arg(1));
      Val sc_d = LinearTraits<Lit>::eval(env, sc->arg(2));
      assert(sc_coeff->size() == sc_al->size());
      for (unsigned int j = 0; j < sc_coeff->size(); j++) {
        coeffv.push_back(cd * LinearTraits<Lit>::eval(env, (*sc_coeff)[j]));
        alv.emplace_back((*sc_al)[j]);
      }
      d += cd * sc_d;
    } else {
      coeffv.push_back(c_coeff[i]);
      alv.emplace_back((*al)[i]);
    }
  }
  simplify_lin<Lit>(coeffv, alv, d);
  if (coeffv.empty()) {
    GCLock lock;
    ret.b = conj(env, b, Ctx(), args_ee);
    ret.r = bind(env, ctx, r, LinearTraits<Lit>::newLit(d));
    return;
  }
  if (coeffv.size() == 1 && coeffv[0] == 1 && d == 0) {
    ret.b = conj(env, b, Ctx(), args_ee);
    ret.r = bind(env, ctx, r, alv[0]());
    return;
  }
  // Share the normalised, constant-free core through CSE.
  if (env.deferLinearRelations && coeffv.size() > 1) {
    typename LinearTraits<Lit>::Val scale = linear_scale<Lit>(coeffv);
    if (scale != 1 || d != 0) {
      GCLock lock;
      std::vector<Expression*> core_c(coeffv.size());
      std::vector<Expression*> core_x(coeffv.size());
      for (unsigned int i = 0; i < coeffv.size(); i++) {
        core_c[i] = LinearTraits<Lit>::newLit(coeffv[i] / scale);
        core_x[i] = alv[i]();
      }
      auto* cc = new ArrayLit(Location().introduce(), core_c);
      Type ct = Expression::type(core_c[0]);
      ct.dim(1);
      cc->type(ct);
      auto* cx = new ArrayLit(Location().introduce(), core_x);
      Type xt = Expression::type(core_x[0]);
      xt.dim(1);
      xt.mkVar(env);
      cx->type(xt);
      Call* core = Call::a(Expression::loc(c).introduce(), env.constants.ids.lin_exp,
                           {cc, cx, LinearTraits<Lit>::newLit(0)});
      core->decl(env.model->matchFn(env, core, false));
      if (core->decl() != nullptr) {
        core->type(c->type());
        EE coreEE = flat_exp(env, nctx, core, nullptr, env.constants.varTrue);
        args_ee.push_back(coreEE);
        coeffv.clear();
        coeffv.push_back(scale);
        alv.clear();
        alv.emplace_back(coreEE.r());
      }
    }
  }
  GCLock lock;
  std::vector<Expression*> coeff_ev(coeffv.size());
  for (auto i = static_cast<unsigned int>(coeff_ev.size()); i--;) {
    coeff_ev[i] = LinearTraits<Lit>::newLit(coeffv[i]);
  }
  auto* ncoeff = new ArrayLit(Location().introduce(), coeff_ev);
  Type t = Expression::type(coeff_ev[0]);
  t.dim(1);
  ncoeff->type(t);
  args.emplace_back(ncoeff);
  std::vector<Expression*> alv_e(alv.size());
  bool al_same_as_before = alv.size() == al->size();
  for (auto i = static_cast<unsigned int>(alv.size()); i--;) {
    alv_e[i] = alv[i]();
    al_same_as_before = al_same_as_before && Expression::equal(alv_e[i], (*al)[i]);
  }
  if (al_same_as_before) {
    Expression* rd = follow_id_to_decl(flat_al.r());
    if (Expression::isa<VarDecl>(rd)) {
      rd = Expression::cast<VarDecl>(rd)->id();
    }
    if (Expression::type(rd).dim() > 1) {
      ArrayLit* al = eval_array_lit(env, rd);
      std::vector<std::pair<int, int>> dims(1);
      dims[0].first = 1;
      dims[0].second = static_cast<int>(al->size());
      rd = new ArrayLit(Expression::loc(al), al, dims);
      Expression::type(rd, Type::arrType(env, Type::top(1), al->type()));
    }
    args.emplace_back(rd);
  } else {
    auto* nal = new ArrayLit(Expression::loc(al), alv_e);
    nal->type(al->type());
    args.emplace_back(nal);
  }
  Lit* il = LinearTraits<Lit>::newLit(d);
  args.push_back(il);
}

// Only integers and floats have linear expressions.
template Expression* get_linexp<IntLit>(EnvI&, Expression*, bool);
template Expression* get_linexp<FloatLit>(EnvI&, Expression*, bool);
template KeepAlive mklinexp<IntLit>(EnvI&, IntVal, IntVal, Expression*, Expression*);
template KeepAlive mklinexp<FloatLit>(EnvI&, FloatVal, FloatVal, Expression*, Expression*);
template void flatten_linexp_binop<IntLit>(EnvI&, const Ctx&, VarDecl*, VarDecl*, EE&, Expression*,
                                           Expression*, BinOpType&, bool, std::vector<EE>&,
                                           std::vector<KeepAlive>&, ASTString&);
template void flatten_linexp_binop<FloatLit>(EnvI&, const Ctx&, VarDecl*, VarDecl*, EE&,
                                             Expression*, Expression*, BinOpType&, bool,
                                             std::vector<EE>&, std::vector<KeepAlive>&, ASTString&);
template void flatten_linexp_call<IntLit>(EnvI&, Ctx, const Ctx&, ASTString&, Call*, EE&, VarDecl*,
                                          VarDecl*, std::vector<EE>&, std::vector<KeepAlive>&);
template void flatten_linexp_call<FloatLit>(EnvI&, Ctx, const Ctx&, ASTString&, Call*, EE&,
                                            VarDecl*, VarDecl*, std::vector<EE>&,
                                            std::vector<KeepAlive>&);

}  // namespace MiniZinc
