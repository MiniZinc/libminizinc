/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/// Linear-expression construction, normalisation, and deferred aggregation.
/// Multi-term expressions keep temporary names while the flat model is built so
/// CSE can detect sharing. The redefinition agenda later inlines single-use
/// expressions and emits definitions for shared ones.

#pragma once

#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

class CmpExpIdx {
public:
  std::vector<Ref<Expression>>& x;
  CmpExpIdx(std::vector<Ref<Expression>>& x0) : x(x0) {}
  bool operator()(int i, int j) const {
    const long long int i_idn = Expression::isa<Id>(x[i]) ? Expression::cast<Id>(x[i])->idn() : -1;
    const long long int j_idn = Expression::isa<Id>(x[j]) ? Expression::cast<Id>(x[j])->idn() : -1;
    const bool i_is_valid_id = i_idn != -1;
    const bool j_is_valid_id = j_idn != -1;
    if (i_is_valid_id != j_is_valid_id) {
      return i_is_valid_id;
    }
    if (i_is_valid_id && j_is_valid_id) {
      return i_idn < j_idn;
    }
    return Expression::compare(x[i], x[j]) < 0;
  }
};

template <class Lit>
class LinearTraits {};
template <>
class LinearTraits<IntLit> {
public:
  using Val = IntVal;
  static Val eval(EnvI& env, Expression* e) { return eval_int(env, e); }
  static void constructLinBuiltin(EnvI& env, BinOpType bot, ASTString& callid, int& coeff_sign,
                                  Val& d) {
    switch (bot) {
      case BOT_LE:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = 1;
        d += 1;
        break;
      case BOT_LQ:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = 1;
        break;
      case BOT_GR:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = -1;
        d = -d + 1;
        break;
      case BOT_GQ:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = -1;
        d = -d;
        break;
      case BOT_EQ:
        callid = env.constants.ids.int_.lin_eq;
        coeff_sign = 1;
        break;
      case BOT_NQ:
        callid = env.constants.ids.int_.lin_ne;
        coeff_sign = 1;
        break;
      default:
        assert(false);
        break;
    }
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_eq() { return Constants::constants().ids.int_.eq; }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_lin_eq() { return Constants::constants().ids.int_.lin_eq; }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_lin_ne() { return Constants::constants().ids.int_.lin_ne; }
  using Bounds = IntBounds;
  static bool finite(const IntBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
  static bool finite(const IntVal& v) { return v.isFinite(); }
  static Bounds computeBounds(EnvI& env, Expression* e) { return compute_int_bounds(env, e); }
  using Domain = Ref<IntSetVal>;
  static Domain evalDomain(EnvI& env, Expression* e) { return eval_intset(env, e); }
  static Ref<Expression> fixedLit(VarDecl* vd) {
    auto* sl = Expression::dynamicCast<SetLit>(vd->ti()->domain());
    if (sl == nullptr || sl->isv() == nullptr) {
      return nullptr;
    }
    IntSetVal* isv = sl->isv();
    if (isv->size() != 1 || isv->min() != isv->max() || !isv->min().isFinite()) {
      return nullptr;
    }
    return IntLit::a(isv->min());
  }
  static Ref<Expression> newDomain(Val v) {
    return make<SetLit>(Location().introduce(), IntSetVal::a(v, v));
  }
  static Ref<Expression> newDomain(Val v0, Val v1) {
    return make<SetLit>(Location().introduce(), IntSetVal::a(v0, v1));
  }
  static Ref<Expression> newDomain(Domain d) { return make<SetLit>(Location().introduce(), d); }
  static bool domainContains(const Domain& dom, Val v) { return dom->contains(v); }
  static bool domainEquals(const Domain& dom, Val v) {
    return dom->size() == 1 && dom->min(0) == v && dom->max(0) == v;
  }
  static bool domainEquals(const Domain& dom1, const Domain& dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::equal(d1, d2);
  }
  static bool domainSubset(const Domain& dom1, const Domain& dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::subset(d1, d2);
  }
  static bool domainDisjoint(const Domain& dom1, const Domain& dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::disjoint(d1, d2);
  }
  static bool domainTighter(const Domain& dom, Bounds b) {
    return !b.valid || dom->min() > b.l || dom->max() < b.u;
  }
  static bool domainIntersects(const Domain& dom, Val v0, Val v1) {
    return (v0 > v1) || (!dom->empty() && dom->min(0) <= v1 && v0 <= dom->max(dom->size() - 1));
  }
  static bool domainEmpty(const Domain& dom) { return dom->empty(); }
  static Domain limitDomain(BinOpType bot, const Domain& dom, Val v) {
    IntSetRanges dr(dom);
    Domain ndomain;
    switch (bot) {
      case BOT_LE:
        v -= 1;
        // fall through
      case BOT_LQ: {
        Ranges::Bounded<IntVal, IntSetRanges> b =
            Ranges::Bounded<IntVal, IntSetRanges>::maxiter(dr, v);
        ndomain = IntSetVal::ai(b);
      } break;
      case BOT_GR:
        v += 1;
        // fall through
      case BOT_GQ: {
        Ranges::Bounded<IntVal, IntSetRanges> b =
            Ranges::Bounded<IntVal, IntSetRanges>::miniter(dr, v);
        ndomain = IntSetVal::ai(b);
      } break;
      case BOT_NQ: {
        Ranges::Const<IntVal> c(v, v);
        Ranges::Diff<IntVal, IntSetRanges, Ranges::Const<IntVal>> d(dr, c);
        ndomain = IntSetVal::ai(d);
      } break;
      default:
        assert(false);
        return nullptr;
    }
    return ndomain;
  }
  static Domain intersectDomain(const Domain& dom, Val v0, Val v1) {
    IntSetRanges dr(dom);
    Ranges::Const<IntVal> c(v0, v1);
    Ranges::Inter<IntVal, IntSetRanges, Ranges::Const<IntVal>> inter(dr, c);
    return IntSetVal::ai(inter);
  }
  static Domain intersectDomain(const Domain& dom0, const Domain& dom1) {
    IntSetRanges dr0(dom0);
    IntSetRanges dr1(dom1);
    Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(dr0, dr1);
    return IntSetVal::ai(inter);
  }
  /// Greatest common divisor of the coefficients, or 1 when there is none
  static IntVal commonDivisor(const std::vector<IntVal>& c) {
    IntVal g = 0;
    for (auto v : c) {
      IntVal a = g < 0 ? -g : g;
      IntVal b = v < 0 ? -v : v;
      while (b != 0) {
        IntVal t = a % b;
        a = b;
        b = t;
      }
      g = a;
    }
    return g > 1 ? g : IntVal(1);
  }
  static Val floorDiv(Val v0, Val v1) {
    Val q = v0 / v1;
    return v0 % v1 != 0 && (v0 < 0) != (v1 < 0) ? q - 1 : q;
  }
  static Val ceilDiv(Val v0, Val v1) {
    Val q = v0 / v1;
    return v0 % v1 != 0 && (v0 < 0) == (v1 < 0) ? q + 1 : q;
  }
  static Ref<IntLit> newLit(Val v) { return IntLit::a(v); }
  static IntVal v(const IntLit* il) { return IntLit::v(il); }
};
template <>
class LinearTraits<FloatLit> {
public:
  using Val = FloatVal;
  static Val eval(EnvI& env, Expression* e) { return eval_float(env, e); }
  static void constructLinBuiltin(EnvI& env, BinOpType bot, ASTString& callid, int& coeff_sign,
                                  Val& d) {
    switch (bot) {
      case BOT_LE:
        callid = env.constants.ids.float_.lin_lt;
        coeff_sign = 1;
        break;
      case BOT_LQ:
        callid = env.constants.ids.float_.lin_le;
        coeff_sign = 1;
        break;
      case BOT_GR:
        callid = env.constants.ids.float_.lin_lt;
        coeff_sign = -1;
        d = -d;
        break;
      case BOT_GQ:
        callid = env.constants.ids.float_.lin_le;
        coeff_sign = -1;
        d = -d;
        break;
      case BOT_EQ:
        callid = env.constants.ids.float_.lin_eq;
        coeff_sign = 1;
        break;
      case BOT_NQ:
        callid = env.constants.ids.float_.lin_ne;
        coeff_sign = 1;
        break;
      default:
        assert(false);
        break;
    }
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_eq() { return Constants::constants().ids.float_.eq; }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_lin_eq() { return Constants::constants().ids.float_.lin_eq; }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_lin_ne() { return Constants::constants().ids.float_.lin_ne; }
  using Bounds = FloatBounds;
  static bool finite(const FloatBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
  static bool finite(const FloatVal& v) { return v.isFinite(); }
  static Bounds computeBounds(EnvI& env, Expression* e) { return compute_float_bounds(env, e); }
  using Domain = Ref<FloatSetVal>;
  static Domain evalDomain(EnvI& env, Expression* e) { return eval_floatset(env, e); }
  static Ref<Expression> fixedLit(VarDecl* vd) {
    auto* sl = Expression::dynamicCast<SetLit>(vd->ti()->domain());
    if (sl == nullptr || sl->fsv() == nullptr) {
      return nullptr;
    }
    FloatSetVal* fsv = sl->fsv();
    if (fsv->size() != 1 || fsv->min() != fsv->max() || !fsv->min().isFinite()) {
      return nullptr;
    }
    return FloatLit::a(fsv->min());
  }

  static Ref<Expression> newDomain(Val v) {
    return make<SetLit>(Location().introduce(), FloatSetVal::a(v, v));
  }
  static Ref<Expression> newDomain(Val v0, Val v1) {
    return make<SetLit>(Location().introduce(), FloatSetVal::a(v0, v1));
  }
  static Ref<Expression> newDomain(Domain d) { return make<SetLit>(Location().introduce(), d); }
  static bool domainContains(const Domain& dom, Val v) { return dom->contains(v); }
  static bool domainEquals(const Domain& dom, Val v) {
    return dom->size() == 1 && dom->min(0) == v && dom->max(0) == v;
  }

  static bool domainTighter(const Domain& dom, Bounds b) {
    return !b.valid || dom->min() > b.l || dom->max() < b.u;
  }
  static bool domainIntersects(const Domain& dom, Val v0, Val v1) {
    return (v0 > v1) || (!dom->empty() && dom->min(0) <= v1 && v0 <= dom->max(dom->size() - 1));
  }
  static bool domainEmpty(const Domain& dom) { return dom->empty(); }

  static bool domainEquals(const Domain& dom1, const Domain& dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::equal(d1, d2);
  }
  static bool domainSubset(const Domain& dom1, const Domain& dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::subset(d1, d2);
  }
  static bool domainDisjoint(const Domain& dom1, const Domain& dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::disjoint(d1, d2);
  }
  static Domain intersectDomain(const Domain& dom, Val v0, Val v1) {
    if (dom != nullptr) {
      FloatSetRanges dr(dom);
      Ranges::Const<FloatVal> c(v0, v1);
      Ranges::Inter<FloatVal, FloatSetRanges, Ranges::Const<FloatVal>> inter(dr, c);
      return FloatSetVal::ai(inter);
    }
    Domain d = FloatSetVal::a(v0, v1);
    return d;
  }
  static Domain intersectDomain(Domain dom0, Domain dom1) {
    if (dom0 == nullptr) {
      return dom1;
    }
    if (dom1 == nullptr) {
      return dom0;
    }
    FloatSetRanges dr0(dom0);
    FloatSetRanges dr1(dom1);
    Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> inter(dr0, dr1);
    return FloatSetVal::ai(inter);
  }

  static Domain limitDomain(BinOpType bot, const Domain& dom, Val v) {
    FloatSetRanges dr(dom);
    Domain ndomain;
    switch (bot) {
      case BOT_LE:
        return nullptr;
      case BOT_LQ: {
        Ranges::Bounded<FloatVal, FloatSetRanges> b =
            Ranges::Bounded<FloatVal, FloatSetRanges>::maxiter(dr, v);
        ndomain = FloatSetVal::ai(b);
      } break;
      case BOT_GR:
        return nullptr;
      case BOT_GQ: {
        Ranges::Bounded<FloatVal, FloatSetRanges> b =
            Ranges::Bounded<FloatVal, FloatSetRanges>::miniter(dr, v);
        ndomain = FloatSetVal::ai(b);
      } break;
      case BOT_NQ: {
        Ranges::Const<FloatVal> c(v, v);
        Ranges::Diff<FloatVal, FloatSetRanges, Ranges::Const<FloatVal>> d(dr, c);
        ndomain = FloatSetVal::ai(d);
      } break;
      default:
        assert(false);
        return nullptr;
    }
    return ndomain;
  }
  /// Float coefficients have no common divisor to remove
  static FloatVal commonDivisor(const std::vector<FloatVal>& /*c*/) { return FloatVal(1.0); }
  static Val floorDiv(Val v0, Val v1) { return v0 / v1; }
  static Val ceilDiv(Val v0, Val v1) { return v0 / v1; }
  static Ref<FloatLit> newLit(Val v) { return FloatLit::a(v); }
  static FloatVal v(const FloatLit* fl) { return FloatLit::v(fl); }
};

template <class Lit>
void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c, std::vector<Ref<Expression>>& x,
                  typename LinearTraits<Lit>::Val& d, bool fixDomains) {
  std::vector<int> idx(c.size());
  for (auto i = static_cast<int>(idx.size()); i--;) {
    idx[i] = i;
    Expression* e = follow_id_to_decl(x[i]);
    if (auto* vd = Expression::dynamicCast<VarDecl>(e)) {
      // Domains may fix variables before their definitions are rewritten.
      Ref<Expression> fixed;
      if (vd->e() != nullptr && Expression::isa<Lit>(vd->e())) {
        fixed = vd->e();
      } else if (fixDomains && vd->ti()->domain() != nullptr) {
        fixed = LinearTraits<Lit>::fixedLit(vd);
      }
      if (fixed != nullptr) {
        x[i] = fixed;
      } else {
        x[i] = vd->id();
      }
    } else {
      x[i] = e;
    }
  }
  // Canonical term order gives equivalent expressions the same CSE key.
  std::sort(idx.begin(), idx.end(), CmpExpIdx(x));
  unsigned int ci = 0;
  for (; ci < x.size(); ci++) {
    if (Lit* il = Expression::dynamicCast<Lit>(x[idx[ci]])) {
      d += c[idx[ci]] * LinearTraits<Lit>::v(il);
      c[idx[ci]] = 0;
    } else {
      break;
    }
  }
  for (unsigned int i = ci + 1; i < x.size(); i++) {
    if (Expression::equal(x[idx[i]], x[idx[ci]])) {
      c[idx[ci]] += c[idx[i]];
      c[idx[i]] = 0;
    } else if (Lit* il = Expression::dynamicCast<Lit>(x[idx[i]])) {
      d += c[idx[i]] * LinearTraits<Lit>::v(il);
      c[idx[i]] = 0;
    } else {
      ci = i;
    }
  }
  // Emit the terms in the order the merge used, so that equal expressions get
  // the same form and the common subexpression map can match them.
  std::vector<typename LinearTraits<Lit>::Val> nc;
  std::vector<Ref<Expression>> nx;
  nc.reserve(c.size());
  nx.reserve(x.size());
  for (int i : idx) {
    if (c[i] != 0) {
      nc.push_back(c[i]);
      nx.push_back(x[i]);
    }
  }
  c.swap(nc);
  x.swap(nx);
}

/// Return a `lin_exp` or id if \a e is a `lin_exp` or id.
/// \a isDefinition says the expression is being equated to a variable in a root
/// context. That equality *is* the name, so the expression is aggregated into it
/// rather than kept behind a second one.
template <class Lit>
Expression* get_linexp(EnvI& env, Expression* e, bool isDefinition = false);

/// Return the `lin_exp` for `c0 * e0 + c1 * e1`.
template <class Lit>
Ref<Expression> mklinexp(EnvI& env, typename LinearTraits<Lit>::Val c0,
                         typename LinearTraits<Lit>::Val c1, Expression* e0, Expression* e1);

/// Build the comparison \a bot between the linear expressions \a le0 and \a le1,
/// leaving the call to make in \a callid and \a args, or settling it in \a ret.
template <class Lit>
void flatten_linexp_binop(EnvI& env, const Ctx& ctx, VarDecl* r, VarDecl* b, EE& ret,
                          Expression* le0, Expression* le1, BinOpType& bot, bool doubleNeg,
                          std::vector<EE>& ees, std::vector<Ref<Expression>>& args,
                          ASTString& callid);

/// Build the `lin_exp`/`sum` call \a c, aggregating the expressions it reads.
template <class Lit>
void flatten_linexp_call(EnvI& env, const Ctx& ctx, const Ctx& nctx, ASTString& cid, Call* c,
                         EE& ret, VarDecl* b, VarDecl* r, std::vector<EE>& args_ee,
                         std::vector<Ref<Expression>>& args);

/// Like follow_id, but stops at a variable that keeps its linear expression
/// (see linear_barrier).
Expression* follow_id_linear(EnvI& env, Expression* e);

/// Return \a e as a call to \a id, following identifiers, or nullptr. An
/// `int2float` around an integer linear expression is rewritten to the float
/// expression it stands for.
Ref<Call> same_call(EnvI& env, Expression* e, const ASTString& id);

/// Whether \a e is a multi-term `lin_exp`.
bool is_linear_sum(EnvI& env, Expression* e);

/// Whether aggregation stops at \a vd. During deferral every multi-term
/// expression is a barrier; afterwards only expressions that keep a name are.
bool linear_barrier(EnvI& env, VarDecl* vd);

/// Whether \a def should keep \a vd because it is shared or externally visible.
/// Only used when FlatteningOptions::eagerLinearAggregation is off.
bool linear_keeps_name(EnvI& env, VarDecl* vd, Call* def);

/// Drop an unread linear definition, preserving its range domain as
/// constraints. Definitions with non-contiguous domains keep their variable.
bool release_linear_definition(EnvI& env, VarDeclI* vdi);

/// Return the `int_lin_eq`/`float_lin_eq` that defines \a vd from \a def,
/// without a declaration.
Ref<Call> linear_definition_call(VarDecl* vd, const Call* def);

struct LinearRelation {
  Ref<BinOp> comparison;
  VarDecl* reifiedInto = nullptr;
  bool halfReified = false;
};

/// Decode a supported scalar or linear comparison. With \a deferredOnly,
/// reject relations that no longer need deferred rebuilding.
LinearRelation linear_relation(EnvI& env, Call* c, bool deferredOnly);

/// Rebuild the linear expression or relation \a c held by \a i as \a e, now
/// that the names in it are decided. A relation not \a reifiedInto a variable of
/// its own is bound to the variable \a i declares, if any.
void rebuild_linear(EnvI& env, Item* i, Call* c, Expression* e, VarDecl* reifiedInto,
                    bool halfReified);

/// Inline \a vdi into its constraint and variable-declaration readers.
void inline_linear_definition(EnvI& env, VarDeclI* vdi);

/// Turn \a c into domain constraints if possible.
/// Return whether \a c is still required in the model.
bool check_domain_constraints(EnvI& env, Call* c);

}  // namespace MiniZinc
