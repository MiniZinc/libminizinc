/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/typecheck.hh>

namespace MiniZinc {

std::vector<Expression*> to_exp_vec(std::vector<KeepAlive>& v) {
  std::vector<Expression*> r(v.size());
  for (auto i = static_cast<unsigned int>(v.size()); (i--) != 0U;) {
    r[i] = v[i]();
  }
  return r;
}

bool is_total(EnvI& env, FunctionI* fi) {
  return fi->ann().contains(env.constants.ann.promise_total);
}

Call* same_call(EnvI& env, Expression* e, const ASTString& id) {
  assert(GC::locked());
  Expression* ce = follow_id(e);
  Call* c = Expression::dynamicCast<Call>(ce);
  if (c != nullptr) {
    if (c->id() == id) {
      return Expression::cast<Call>(ce);
    }
    if (c->id() == env.constants.ids.int2float) {
      Expression* i2f = follow_id(c->arg(0));
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

class CmpExp {
public:
  bool operator()(const KeepAlive& i, const KeepAlive& j) const {
    if (Expression::equal(i(), j())) {
      return false;
    }
    return i() < j();
  }
};

bool remove_dups(std::vector<KeepAlive>& x, bool identity) {
  for (auto& i : x) {
    i = follow_id_to_value(i());
  }
  std::sort(x.begin(), x.end(), CmpExp());
  int ci = 0;
  Expression* prev = nullptr;
  for (unsigned int i = 0; i < x.size(); i++) {
    if (!Expression::equal(x[i](), prev)) {
      prev = x[i]();
      if (Expression::isa<BoolLit>(x[i]())) {
        if (Expression::cast<BoolLit>(x[i]())->v() == identity) {
          // skip
        } else {
          return true;
        }
      } else {
        x[ci++] = x[i];
      }
    }
  }
  x.resize(ci);
  return false;
}
bool contains_dups(std::vector<KeepAlive>& x, std::vector<KeepAlive>& y) {
  if (x.empty() || y.empty()) {
    return false;
  }
  unsigned int ix = 0;
  unsigned int iy = 0;
  for (;;) {
    if (x[ix]() == y[iy]()) {
      return true;
    }
    if (x[ix]() < y[iy]()) {
      ix++;
    } else {
      iy++;
    }
    if (ix == x.size() || iy == y.size()) {
      return false;
    }
  }
}

template <class Lit>
void flatten_linexp_call(EnvI& env, Ctx ctx, const Ctx& nctx, ASTString& cid, Call* c, EE& ret,
                         VarDecl* b, VarDecl* r, std::vector<EE>& args_ee,
                         std::vector<KeepAlive>& args) {
  typedef typename LinearTraits<Lit>::Val Val;
  Expression* al_arg = (cid == env.constants.ids.sum ? args_ee[0].r() : args_ee[1].r());
  EE flat_al = flat_exp(env, nctx, al_arg, nullptr, nctx.partialityVar(env));
  auto* al = Expression::cast<ArrayLit>(follow_id(flat_al.r()));
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
          // Test if the variable has tighter declared bounds than what can be inferred
          // from its RHS. If yes, keep the variable (don't aggregate), because the tighter
          // bounds are actually a constraint
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

/// Special form of disjunction for SCIP
bool is_totaladd_bounds_disj(EnvI& env, Expression* arg, Call* c_orig) {
  auto* pArrayLit = Expression::dynamicCast<ArrayLit>(arg);
  if (nullptr == pArrayLit) {
    return false;
  }
  // integer bounds and vars
  std::vector<Expression*> isUBI;
  std::vector<Expression*> bndI;
  std::vector<Expression*> varI;
  // float bounds and vars
  std::vector<Expression*> isUBF;
  std::vector<Expression*> bndF;
  std::vector<Expression*> varF;
  for (unsigned int i = pArrayLit->size(); (i--) != 0U;) {
    auto* pId = Expression::dynamicCast<Id>(pArrayLit->operator[](i));
    if (nullptr == pId) {
      return false;
    }
    auto* pDecl = Expression::dynamicCast<VarDecl>(follow_id_to_decl(pId));
    /// Checking the rhs
    auto* pRhs = pDecl->e();
    if (nullptr == pRhs) {
      return false;  // not checking this boolean
    }
    auto* pCall = Expression::dynamicCast<Call>(pRhs);
    if (nullptr == pCall) {
      return false;
    }
    if (env.constants.ids.int_.le != pCall->id() && env.constants.ids.float_.le != pCall->id()) {
      return false;
    }
    /// See if one is a constant and one a variable
    Expression* pConst = nullptr;
    Expression* pVar = nullptr;
    bool fFloat = false;
    bool isUB = false;
    for (unsigned int j = pCall->argCount(); (j--) != 0U;) {
      if (auto* pF = Expression::dynamicCast<FloatLit>(pCall->arg(j))) {
        pConst = pF;
        fFloat = true;
        isUB = (1 == j);
      } else if (auto* pF = Expression::dynamicCast<IntLit>(pCall->arg(j))) {
        pConst = pF;
        fFloat = false;
        isUB = (1 == j);
      } else if (auto* pId = Expression::dynamicCast<Id>(pCall->arg(j))) {
        if (nullptr != pVar) {
          return false;  // 2 variables, exit
        }
        pVar = pId;
      }
    }
    /// All good, add them
    if (fFloat) {
      isUBF.push_back(env.constants.boollit(isUB));
      bndF.push_back(pConst);
      varF.push_back(pVar);
    } else {
      isUBI.push_back(env.constants.boollit(isUB));
      bndI.push_back(pConst);
      varI.push_back(pVar);
    }
  }
  /// Create new call
  GCLock lock;
  auto loc = Expression::loc(c_orig).introduce();
  std::vector<Expression*> args = {new ArrayLit(loc, isUBI), new ArrayLit(loc, bndI),
                                   new ArrayLit(loc, varI),  new ArrayLit(loc, isUBF),
                                   new ArrayLit(loc, bndF),  new ArrayLit(loc, varF)};

  Call* c = Call::a(Expression::loc(c_orig).introduce(),
                    env.model->getFnDecls().boundsDisj.second->id(), args);
  c->type(Type::varbool());
  c->decl(env.model->getFnDecls().boundsDisj.second);
  env.flatAddItem(new ConstraintI(Expression::loc(c_orig).introduce(), c));
  return true;
}

class IgnorePartial {
public:
  EnvI& env;
  bool ignorePartial;
  IgnorePartial(EnvI& env0, Call* c) : env(env0), ignorePartial(env.ignorePartial) {
    if (c->id().endsWith("_reif") || c->id().endsWith("_imp")) {
      env.ignorePartial = true;
    }
  }
  ~IgnorePartial() { env.ignorePartial = ignorePartial; }
};

// NOLINTNEXTLINE(readability-function-size): TODO??
EE flatten_call(EnvI& env, const Ctx& input_ctx, Expression* e, VarDecl* r, VarDecl* b) {
  EE ret;
  Call* c = Expression::cast<Call>(e);
  IgnorePartial ignorePartial(env, c);
  if (c->id().endsWith("_reif")) {
    env.counters.reifConstraints++;
  } else if (c->id().endsWith("_imp")) {
    env.counters.impConstraints++;
  }
  FunctionI* decl = c->decl();
  try {
    decl = env.model->matchFn(env, c, false);
  } catch (TypeError&) { /* NOLINT(bugprone-empty-catch) */
    // Can't actually match call, maybe it's being called with args which are now known to be
    // bottom, so just use the decl that was already in the call if there was one
  }
  if (decl == nullptr) {
    std::ostringstream ss;
    ss << "undeclared function or predicate " << demonomorphise_identifier(c->id());
    throw InternalError(ss.str());
  }

  Ctx ctx = input_ctx;
  Ctx nctx = ctx;
  nctx.neg = false;
  ASTString cid = c->id();
  CallStackItem _csi(env, e);

  if (cid == env.constants.ids.bool2int && c->type().dim() == 0) {
    if (ctx.neg) {
      ctx.neg = false;
      nctx.neg = true;
      nctx.b = -ctx.i;
    } else {
      nctx.b = ctx.i;
    }
  } else if (cid == env.constants.ids.forall) {
    nctx.b = +nctx.b;
    if (ctx.neg) {
      ctx.neg = false;
      nctx.neg = true;
      cid = env.constants.ids.exists;
    }
  } else if (cid == env.constants.ids.exists) {
    nctx.b = +nctx.b;
    if (ctx.neg) {
      ctx.neg = false;
      nctx.neg = true;
      cid = env.constants.ids.forall;
    }
  } else if (!env.fopts.debug &&
             (cid == env.constants.ids.assert_dbg || cid == env.constants.ids.trace_dbg ||
              cid == env.constants.ann.expression_name_dbg)) {
    if (c->type().isAnn()) {
      return flat_exp(env, ctx, env.constants.ann.empty_annotation, r, b);
    }
    assert(c->type().isbool());
    return flat_exp(env, ctx, env.constants.literalTrue, r, b);
  } else if (decl->e() == nullptr && env.constants.isCallByReferenceId(cid)) {
    if ((cid == env.constants.ids.assert || cid == env.constants.ids.assert_dbg ||
         cid == env.constants.ids.output_to_section ||
         cid == env.constants.ids.output_to_json_section) &&
        c->argCount() == 2) {
      (void)decl->builtins.b(env, c);
      ret = flat_exp(env, ctx, env.constants.literalTrue, r, b);
    } else {
      KeepAlive callres = decl->builtins.e(env, c);
      ret = flat_exp(env, ctx, callres(), r, b);
      // This is all we need to do for assert, so break out of the E_CALL
    }
    return ret;
  } else if ((decl->e() != nullptr) && ctx.b == C_ROOT && Expression::isa<BoolLit>(decl->e()) &&
             eval_bool(env, decl->e())) {
    bool allBool = true;
    for (unsigned int i = 0; i < c->argCount(); i++) {
      if (Expression::type(c->arg(i)).bt() != Type::BT_BOOL) {
        allBool = false;
        break;
      }
    }
    if (allBool) {
      ret.r = bind(env, ctx, r, env.constants.literalTrue);
      ret.b = bind(env, ctx, b, env.constants.literalTrue);
      return ret;
    }
  }

  if (ctx.b == C_ROOT && decl->e() == nullptr && cid == env.constants.ids.forall &&
      r == env.constants.varTrue) {
    ret.b = bind(env, ctx, b, env.constants.literalTrue);
    KeepAlive ka;
    ArrayLit* al;
    if (Expression::isa<ArrayLit>(c->arg(0))) {
      al = Expression::cast<ArrayLit>(c->arg(0));
    } else {
      EE flat_al = flat_exp(env, Ctx(), c->arg(0), env.constants.varIgnore, env.constants.varTrue);
      al = Expression::cast<ArrayLit>(follow_id(flat_al.r()));
      ka = al;  // Ensure al is kept alive while we flatten its elements
    }
    nctx.b = C_ROOT;
    for (unsigned int i = 0; i < al->size(); i++) {
      (void)flat_exp(env, nctx, (*al)[i], r, b);
    }
    ret.r = bind(env, ctx, r, env.constants.literalTrue);
  } else {
    if ((decl->e() != nullptr) && decl->paramCount() == 1 && Expression::isa<Id>(decl->e()) &&
        decl->param(0)->ti()->domain() == nullptr &&
        Expression::cast<Id>(decl->e())->decl() == decl->param(0)) {
      Expression* arg = c->arg(0);
      for (ExpressionSetIter esi = Expression::ann(decl->e()).begin();
           esi != Expression::ann(decl->e()).end(); ++esi) {
        Expression::addAnnotation(arg, *esi);
      }
      for (ExpressionSetIter esi = Expression::ann(c).begin(); esi != Expression::ann(c).end();
           ++esi) {
        Expression::addAnnotation(arg, *esi);
      }
      ret = flat_exp(env, ctx, c->arg(0), r, b);
      return ret;
    }

    std::vector<EE> args_ee(c->argCount());
    bool isPartial = false;

    if (cid == env.constants.ids.lin_exp && c->type().isint()) {
      // Linear expressions need special context handling:
      // the context of a variable expression depends on the corresponding coefficient

      // flatten the coefficient array
      Expression* tmp = follow_id_to_decl(c->arg(0));
      ArrayLit* coeffs;
      if (auto* vd = Expression::dynamicCast<VarDecl>(tmp)) {
        tmp = vd->id();
      }
      {
        CallArgItem cai(env);
        args_ee[0] = flat_exp(env, nctx, tmp, nullptr, nctx.partialityVar(env));
        isPartial |= isfalse(env, args_ee[0].b());
        coeffs = eval_array_lit(env, args_ee[0].r());
      }

      ArrayLit* vars = eval_array_lit(env, c->arg(1));
      if (vars->flat()) {
        args_ee[1].r = vars;
        args_ee[1].b = env.constants.literalTrue;
      } else {
        CallArgItem cai(env);
        CallStackItem _csi(env, c->arg(1));
        std::vector<EE> elems_ee(vars->size());
        for (unsigned int i = vars->size(); (i--) != 0U;) {
          Ctx argctx = nctx;
          argctx.i = eval_int(env, (*coeffs)[i]) < 0 ? -nctx.i : +nctx.i;
          elems_ee[i] = flat_exp(env, argctx, (*vars)[i], nullptr, argctx.partialityVar(env));
        }
        std::vector<Expression*> elems(elems_ee.size());
        for (auto i = static_cast<unsigned int>(elems.size()); (i--) != 0U;) {
          elems[i] = elems_ee[i].r();
        }
        KeepAlive ka;
        {
          GCLock lock;
          auto* alr = new ArrayLit(Location().introduce(), elems);
          alr->type(vars->type());
          alr->flat(true);
          ka = alr;
        }
        args_ee[1].r = ka();
        args_ee[1].b = conj(env, b, Ctx(), elems_ee);
      }

      {
        Expression* constant = follow_id_to_decl(c->arg(2));
        if (auto* vd = Expression::dynamicCast<VarDecl>(constant)) {
          constant = vd->id();
        }
        CallArgItem cai(env);
        args_ee[2] = flat_exp(env, nctx, constant, nullptr, nctx.partialityVar(env));
        isPartial |= isfalse(env, args_ee[2].b());
      }

    } else {
      bool mixContext = (cid != env.constants.ids.forall && cid != env.constants.ids.exists &&
                         (cid != env.constants.ids.bool2int || c->type().dim() > 0) &&
                         cid != env.constants.ids.sum &&
                         cid != env.constants.ids.assert&& cid != env.constants.varRedef->id() &&
                         cid != env.constants.ids.mzn_reverse_map_var &&
                         cid != env.constants.ids.arrayXd && cid != env.constants.ids.array2d &&
                         cid != env.constants.ids.array3d && cid != env.constants.ids.array4d &&
                         cid != env.constants.ids.array5d && cid != env.constants.ids.array6d);
      if (cid == env.constants.ids.mzn_reverse_map_var) {
        env.inReverseMapVar = true;
      }
      if (decl->e() == nullptr && cid == env.constants.ids.clause &&
          Expression::isa<ArrayLit>(c->arg(0)) && Expression::isa<ArrayLit>(c->arg(1))) {
        Ctx argctx = nctx;

        // handle negated args first, try to make them positive

        if (mixContext) {
          argctx.b = -nctx.b;
        }
        auto* al_pos = Expression::cast<ArrayLit>(c->arg(0));
        auto* al_neg = Expression::cast<ArrayLit>(c->arg(1));

        std::vector<KeepAlive> positives;
        std::vector<KeepAlive> negatives;
        for (unsigned int i = 0; i < al_pos->size(); i++) {
          if (auto* uo = Expression::dynamicCast<UnOp>((*al_pos)[i])) {
            if (uo->op() == UOT_NOT) {
              negatives.emplace_back(uo->e());
              continue;
            }
          }
          positives.emplace_back((*al_pos)[i]);
        }
        for (unsigned int i = 0; i < al_neg->size(); i++) {
          if (auto* uo = Expression::dynamicCast<UnOp>((*al_neg)[i])) {
            if (uo->op() == UOT_NOT) {
              positives.emplace_back(uo->e());
              continue;
            }
          }
          negatives.emplace_back((*al_neg)[i]);
        }

        bool is_subsumed = false;
        std::vector<KeepAlive> neg_args;
        std::vector<KeepAlive> pos_args;
        {
          CallArgItem cai(env);
          for (auto& negative : negatives) {
            auto* bo = Expression::dynamicCast<BinOp>(negative());
            Call* co = Expression::dynamicCast<Call>(negative());
            if ((bo != nullptr) || ((co != nullptr) && (co->id() == env.constants.ids.forall ||
                                                        co->id() == env.constants.ids.exists ||
                                                        co->id() == env.constants.ids.clause))) {
              GCLock lock;
              UnOp* notBoe0 = new UnOp(Location().introduce(), UOT_NOT, negative());
              notBoe0->type(Type::varbool());
              positives.emplace_back(notBoe0);
            } else {
              EE res = flat_exp(env, argctx, negative(), nullptr, env.constants.varTrue);
              if (Expression::type(res.r()).isPar() && !Expression::type(res.r()).isOpt()) {
                if (eval_bool(env, res.r())) {
                  // this element is irrelevant
                } else {
                  // this element subsumes all other elements
                  neg_args = {res.r()};
                  pos_args = {};
                  is_subsumed = true;
                  break;
                }
              } else {
                neg_args.emplace_back(res.r());
              }
            }
          }
        }

        // Now process new and previous positive arguments
        if (mixContext) {
          argctx.b = +nctx.b;
        }
        {
          CallArgItem cai(env);
          for (auto& positive : positives) {
            EE res = flat_exp(env, argctx, positive(), nullptr, env.constants.varTrue);
            if (Expression::type(res.r()).isPar()) {
              if (!eval_bool(env, res.r())) {
                // this element is irrelevant
              } else {
                // this element subsumes all other elements
                pos_args = {res.r()};
                neg_args = {};
                is_subsumed = true;
                break;
              }
            } else {
              pos_args.emplace_back(res.r());
            }
          }
        }

        GCLock lock;
        auto* al_new_pos = new ArrayLit(Expression::loc(al_pos), to_exp_vec(pos_args));
        al_new_pos->type(Type::varbool(1));
        al_new_pos->flat(true);
        args_ee[0] = EE(al_new_pos, env.constants.literalTrue);
        auto* al_new_neg = new ArrayLit(Expression::loc(al_neg), to_exp_vec(neg_args));
        al_new_neg->flat(true);
        al_new_neg->type(Type::varbool(1));
        args_ee[1] = EE(al_new_neg, env.constants.literalTrue);
      } else if ((cid == env.constants.ids.forall || cid == env.constants.ids.exists) &&
                 Expression::isa<ArrayLit>(c->arg(0))) {
        bool is_conj = (cid == env.constants.ids.forall);
        Ctx argctx = nctx;
        if (mixContext) {
          argctx.b = C_MIX;
        }
        auto* al = Expression::cast<ArrayLit>(c->arg(0));
        KeepAlive al_new;
        if (al->flat()) {
          al_new = al;
        } else {
          std::vector<KeepAlive> flat_args;
          CallArgItem cai(env);
          for (unsigned int i = 0; i < al->size(); i++) {
            EE res = flat_exp(env, argctx, (*al)[i], nullptr, env.constants.varTrue);
            if (Expression::type(res.r()).isPar()) {
              if (eval_bool(env, res.r()) == is_conj) {
                // this element is irrelevant
              } else {
                // this element subsumes all other elements
                flat_args = {res.r()};
                break;
              }
            } else {
              flat_args.emplace_back(res.r());
            }
          }
          {
            GCLock lock;
            al_new = new ArrayLit(Expression::loc(al), to_exp_vec(flat_args));
            Expression::type(al_new(), Type::varbool(1));
            Expression::cast<ArrayLit>(al_new())->flat(true);
          }
        }
        args_ee[0] = EE(al_new(), env.constants.literalTrue);
      } else {
        BCtx transfer_ctx = c->type().bt() == Type::BT_INT ? nctx.i : nctx.b;
        for (unsigned int i = c->argCount(); (i--) != 0U;) {
          Ctx argctx = nctx;
          if (mixContext) {
            if (cid == env.constants.ids.clause) {
              argctx.b = (i == 0 ? +nctx.b : -nctx.b);
            } else if (Expression::type(c->arg(i)).bt() == Type::BT_BOOL) {
              if (c->decl() != nullptr && Expression::ann(c->decl()->param(i))
                                              .contains(env.constants.ctx.promise_monotone)) {
                argctx.b = +transfer_ctx;
              } else if (c->decl() != nullptr &&
                         Expression::ann(c->decl()->param(i))
                             .contains(env.constants.ctx.promise_antitone)) {
                argctx.b = -transfer_ctx;
              } else {
                argctx.b = C_MIX;
              }
            } else if (Expression::type(c->arg(i)).bt() == Type::BT_INT) {
              if (c->decl() != nullptr && Expression::ann(c->decl()->param(i))
                                              .contains(env.constants.ctx.promise_monotone)) {
                argctx.i = +transfer_ctx;
              } else if (c->decl() != nullptr &&
                         Expression::ann(c->decl()->param(i))
                             .contains(env.constants.ctx.promise_antitone)) {
                argctx.i = -transfer_ctx;
              } else {
                argctx.i = C_MIX;
              }
            }
          } else if (cid == env.constants.ids.sum &&
                     Expression::type(c->arg(i)).bt() == Type::BT_BOOL) {
            argctx.b = argctx.i;
          }
          Expression* tmp = follow_id_to_decl(c->arg(i));
          if (auto* vd = Expression::dynamicCast<VarDecl>(tmp)) {
            tmp = vd->id();
          }
          CallArgItem cai(env);
          args_ee[i] = flat_exp(env, argctx, tmp, nullptr, argctx.partialityVar(env));
          isPartial |= isfalse(env, args_ee[i].b());
        }
      }
    }
    if (isPartial && c->type().isbool() && !c->type().isOpt()) {
      ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
      args_ee.resize(1);
      args_ee[0] = EE(nullptr, env.constants.literalFalse);
      ret.r = conj(env, r, ctx, args_ee);
      return ret;
    }

    std::vector<KeepAlive> args;
    if (decl->e() == nullptr &&
        (cid == env.constants.ids.exists || cid == env.constants.ids.clause)) {
      std::vector<KeepAlive> pos_alv;
      std::vector<KeepAlive> neg_alv;

      std::vector<Expression*> pos_stack;
      std::vector<Expression*> neg_stack;

      auto* al_pos = Expression::cast<ArrayLit>(follow_id(args_ee[0].r()));
      for (unsigned int i = 0; i < al_pos->size(); i++) {
        pos_stack.push_back((*al_pos)[i]);
      }
      if (cid == env.constants.ids.clause) {
        auto* al_neg = Expression::cast<ArrayLit>(follow_id(args_ee[1].r()));
        for (unsigned int i = 0; i < al_neg->size(); i++) {
          neg_stack.push_back((*al_neg)[i]);
        }
      }

      std::unordered_set<Expression*> seen;

      while (!pos_stack.empty() || !neg_stack.empty()) {
        while (!pos_stack.empty()) {
          Expression* cur = pos_stack.back();
          pos_stack.pop_back();
          if (Expression::isa<Id>(cur) && seen.find(cur) != seen.end()) {
            pos_alv.emplace_back(cur);
          } else {
            seen.insert(cur);
            GCLock lock;
            if (Call* sc =
                    Expression::dynamicCast<Call>(same_call(env, cur, env.constants.ids.exists))) {
              GCLock lock;
              ArrayLit* sc_c = eval_array_lit(env, sc->arg(0));
              for (unsigned int j = 0; j < sc_c->size(); j++) {
                pos_stack.push_back((*sc_c)[j]);
              }
            } else if (Call* sc = Expression::dynamicCast<Call>(
                           same_call(env, cur, env.constants.ids.clause))) {
              GCLock lock;
              ArrayLit* sc_c = eval_array_lit(env, sc->arg(0));
              for (unsigned int j = 0; j < sc_c->size(); j++) {
                pos_stack.push_back((*sc_c)[j]);
              }
              sc_c = eval_array_lit(env, sc->arg(1));
              for (unsigned int j = 0; j < sc_c->size(); j++) {
                neg_stack.push_back((*sc_c)[j]);
              }
            } else {
              Call* eq_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, env.constants.ids.bool_.eq));
              Call* not_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, env.constants.ids.bool_.not_));
              if ((eq_call != nullptr) &&
                  Expression::equal(eq_call->arg(1), env.constants.literalFalse)) {
                neg_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), env.constants.literalFalse)) {
                neg_stack.push_back(eq_call->arg(1));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(1), env.constants.literalTrue)) {
                pos_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), env.constants.literalTrue)) {
                pos_stack.push_back(eq_call->arg(1));
              } else if ((not_call != nullptr) && not_call->argCount() == 1) {
                neg_stack.push_back(not_call->arg(0));
              } else if (Id* ident = Expression::dynamicCast<Id>(cur)) {
                if (ident->decl()->ti()->domain() != env.constants.literalFalse) {
                  pos_alv.emplace_back(ident);
                }
              } else {
                pos_alv.emplace_back(cur);
              }
            }
          }
        }

        while (!neg_stack.empty()) {
          GCLock lock;
          Expression* cur = neg_stack.back();
          neg_stack.pop_back();
          if (Expression::isa<Id>(cur) && seen.find(cur) != seen.end()) {
            neg_alv.emplace_back(cur);
          } else {
            seen.insert(cur);
            if (Call* sc =
                    Expression::dynamicCast<Call>(same_call(env, cur, env.constants.ids.forall))) {
              GCLock lock;
              ArrayLit* sc_c = eval_array_lit(env, sc->arg(0));
              for (unsigned int j = 0; j < sc_c->size(); j++) {
                neg_stack.push_back((*sc_c)[j]);
              }
            } else {
              Call* eq_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, env.constants.ids.bool_.eq));
              Call* not_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, env.constants.ids.bool_.not_));
              if ((eq_call != nullptr) &&
                  Expression::equal(eq_call->arg(1), env.constants.literalFalse)) {
                pos_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), env.constants.literalFalse)) {
                pos_stack.push_back(eq_call->arg(1));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(1), env.constants.literalTrue)) {
                neg_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), env.constants.literalTrue)) {
                neg_stack.push_back(eq_call->arg(1));
              } else if ((not_call != nullptr) && not_call->argCount() == 1) {
                pos_stack.push_back(not_call->arg(0));
              } else if (Id* ident = Expression::dynamicCast<Id>(cur)) {
                if (ident->decl()->ti()->domain() != env.constants.literalTrue) {
                  neg_alv.emplace_back(ident);
                }
              } else {
                neg_alv.emplace_back(cur);
              }
            }
          }
        }
      }

      bool subsumed = remove_dups(pos_alv, false);
      subsumed = subsumed || remove_dups(neg_alv, true);
      subsumed = subsumed || contains_dups(pos_alv, neg_alv);
      if (subsumed) {
        ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
        ret.r = bind(env, ctx, r, env.constants.literalTrue);
        return ret;
      }
      if (neg_alv.empty()) {
        if (pos_alv.empty()) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          ret.r = bind(env, ctx, r, env.constants.literalFalse);
          return ret;
        }
        if (pos_alv.size() == 1) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          ret.r = bind(env, ctx, r, pos_alv[0]());
          return ret;
        }
        GCLock lock;
        auto* nal = new ArrayLit(Location().introduce(), to_exp_vec(pos_alv));
        nal->type(Type::varbool(1));
        args.emplace_back(nal);
        cid = env.constants.ids.exists;
      } else {
        if (pos_alv.empty() && neg_alv.size() == 1) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          Ctx nctx = ctx;
          nctx.neg = !nctx.neg;
          nctx.b = -nctx.b;
          ret.r = bind(env, nctx, r, neg_alv[0]());
          return ret;
        }
        GCLock lock;
        auto* pos_al = new ArrayLit(Location().introduce(), to_exp_vec(pos_alv));
        pos_al->type(Type::varbool(1));
        auto* neg_al = new ArrayLit(Location().introduce(), to_exp_vec(neg_alv));
        neg_al->type(Type::varbool(1));
        cid = env.constants.ids.clause;
        args.emplace_back(pos_al);
        args.emplace_back(neg_al);
      }
      if (C_ROOT == ctx.b && cid == env.constants.ids.exists) {
        /// Check the special bounds disjunction for SCIP
        /// Only in root context
        if (!env.model->getFnDecls().boundsDisj.first) {
          env.model->getFnDecls().boundsDisj.first = true;
          std::vector<Type> bj_t = {Type::parbool(1), Type::parint(1),   Type::varint(1),
                                    Type::parbool(1), Type::parfloat(1), Type::varfloat(1)};
          GCLock lock;
          env.model->getFnDecls().boundsDisj.second =
              env.model->matchFn(env, ASTString("bounds_disj"), bj_t, false);
        }
        /// When the SCIP predicate is declared only
        bool fBoundsDisj_Maybe = (nullptr != env.model->getFnDecls().boundsDisj.second);
        if (fBoundsDisj_Maybe) {
          if (is_totaladd_bounds_disj(env, args[0](), c)) {
            ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
            ret.r = bind(env, ctx, r, env.constants.literalTrue);
            return ret;
          }
        }
      }

    } else if (decl->e() == nullptr && cid == env.constants.ids.forall) {
      auto* al = Expression::cast<ArrayLit>(follow_id(args_ee[0].r()));
      std::vector<KeepAlive> alv;
      for (unsigned int i = 0; i < al->size(); i++) {
        GCLock lock;
        if (Call* sc = Expression::dynamicCast<Call>(same_call(env, (*al)[i], cid))) {
          GCLock lock;
          ArrayLit* sc_c = eval_array_lit(env, sc->arg(0));
          for (unsigned int j = 0; j < sc_c->size(); j++) {
            alv.emplace_back((*sc_c)[j]);
          }
        } else {
          alv.emplace_back((*al)[i]);
        }
      }
      bool subsumed = remove_dups(alv, true);
      if (subsumed) {
        ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
        ret.r = bind(env, ctx, r, env.constants.literalFalse);
        return ret;
      }
      if (alv.empty()) {
        ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
        ret.r = bind(env, ctx, r, env.constants.literalTrue);
        return ret;
      }
      if (alv.size() == 1) {
        ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
        ret.r = bind(env, ctx, r, alv[0]());
        return ret;
      }
      GCLock lock;
      auto* nal = new ArrayLit(Expression::loc(al), to_exp_vec(alv));
      nal->type(al->type());
      args.emplace_back(nal);
    } else if (decl->e() == nullptr &&
               (cid == env.constants.ids.lin_exp || cid == env.constants.ids.sum)) {
      if (Expression::type(e).isint()) {
        flatten_linexp_call<IntLit>(env, ctx, nctx, cid, c, ret, b, r, args_ee, args);
      } else {
        flatten_linexp_call<FloatLit>(env, ctx, nctx, cid, c, ret, b, r, args_ee, args);
      }
      if (args.empty()) {
        return ret;
      }
    } else {
      for (auto& i : args_ee) {
        args.emplace_back(i.r());
      }
    }
    bool hadImplementation = (decl->e() != nullptr);
    KeepAlive cr;
    {
      GCLock lock;
      std::vector<Expression*> e_args = to_exp_vec(args);
      Call* cr_c = Call::a(Expression::loc(c).introduce(), cid, e_args);
      if (cid == c->id()) {
        try {
          auto* cr_d = env.model->matchFn(env, cr_c, false);
          if (cr_d != nullptr) {
            decl = cr_d;
          }
        } catch (TypeError&) { /* NOLINT(bugprone-empty-catch) */
          // Matches multiple incompatible overloads (e.g. because arg is actually bottom)
          // So just use original decl from type checker
        }
      } else {
        decl = env.model->matchFn(env, cr_c, false);
        if (decl == nullptr) {
          throw FlatteningError(env, Expression::loc(cr_c), "cannot find matching declaration");
        }
      }
      cr_c->type(decl->rtype(env, e_args, nullptr, false));
      assert(decl);
      cr_c->decl(decl);
      cr = cr_c;
      _csi.replace();
    }
    if (hadImplementation && decl->e() == nullptr &&
        (cid == env.constants.ids.lin_exp || cid == env.constants.ids.sum)) {
      args.clear();
      if (Expression::type(e).isint()) {
        flatten_linexp_call<IntLit>(env, ctx, nctx, cid, Expression::cast<Call>(cr()), ret, b, r,
                                    args_ee, args);
      } else {
        flatten_linexp_call<FloatLit>(env, ctx, nctx, cid, Expression::cast<Call>(cr()), ret, b, r,
                                      args_ee, args);
      }
      if (args.empty()) {
        return ret;
      }
      GCLock lock;
      std::vector<Expression*> e_args = to_exp_vec(args);
      Call* cr_c = Call::a(Expression::loc(c).introduce(), cid, e_args);
      decl = env.model->matchFn(env, cr_c, false);
      if (decl == nullptr) {
        throw FlatteningError(env, Expression::loc(cr_c), "cannot find matching declaration");
      }
      cr_c->type(decl->rtype(env, e_args, nullptr, false));
      assert(decl);
      cr_c->decl(decl);
      cr = cr_c;
    }

    auto cit = env.cseMapEnd();
    if (!ctx.neg && !Expression::type(cr()).isAnn()) {
      cit = env.cseMapFind(cr());
    }
    if (cit != env.cseMapEnd()) {
      if (Expression::type(e).isbool() && !Expression::type(e).isOpt()) {
        cse_result_change_ctx(env, cit->second.r, ctx.b);
        ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
        args_ee.emplace_back(nullptr, cit->second.r);
        ret.r = conj(env, r, ctx, args_ee);
      } else {
        if (env.ignorePartial) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
        } else {
          args_ee.emplace_back(nullptr, cit->second.b);
          ret.b = conj(env, b, Ctx(), args_ee);
        }
        ret.r = bind(env, ctx, r, cit->second.r);
      }
    } else {
      for (unsigned int i = 0; i < decl->paramCount(); i++) {
        if (decl->param(i)->type().dim() > 0) {
          check_index_sets(env, decl->param(i), args[i](), true);
        }
        if (Expression* dom = decl->param(i)->ti()->domain()) {
          if (!Expression::isa<TIId>(dom)) {
            KeepAlive domconstraint;
            {
              GCLock lock;
              domconstraint = mk_domain_constraint(env, args[i](), dom);
            }
            if (domconstraint() != nullptr) {
              if (ctx.b == C_ROOT) {
                (void)flat_exp(env, Ctx(), domconstraint(), env.constants.varTrue,
                               env.constants.varTrue);
              } else {
                Ctx domctx = ctx;
                domctx.neg = false;
                EE ee = flat_exp(env, domctx, domconstraint(), nullptr, env.constants.varTrue);
                ee.b = ee.r;
                args_ee.push_back(ee);
              }
            }
          }
        }
      }
      if (Expression::type(cr()).isbool() && !Expression::type(cr()).isPar() &&
          !Expression::type(cr()).isOpt() && (ctx.b != C_ROOT || r != env.constants.varTrue)) {
        std::vector<Type> argtypes(args.size());
        for (unsigned int i = 0; i < args.size(); i++) {
          argtypes[i] = Expression::type(args[i]());
        }
        argtypes.push_back(Type::varbool());
        GCLock lock;
        ASTString r_cid;
        FunctionI* reif_decl(nullptr);
        if (env.fopts.enableHalfReification && ctx.b == C_POS) {
          r_cid = EnvI::halfReifyId(cid);
          reif_decl = env.model->matchFn(env, r_cid, argtypes, false);
        }
        if (reif_decl == nullptr) {
          r_cid = env.reifyId(cid);
          reif_decl = env.model->matchFn(env, r_cid, argtypes, false);
        }
        if ((reif_decl != nullptr) && (reif_decl->e() != nullptr)) {
          add_path_annotation(env, reif_decl->e());
          VarDecl* reif_b;
          if (r == nullptr || (r != nullptr && r->e() != nullptr) || ctx.neg) {
            reif_b = new_vardecl(env, Ctx(), new TypeInst(Location().introduce(), Type::varbool()),
                                 nullptr, nullptr, nullptr);
            env.addCtxAnn(reif_b, ctx.b);
            if (reif_b->ti()->domain() != nullptr) {
              if (reif_b->ti()->domain() == env.constants.literalTrue) {
                bind(env, ctx, r, env.constants.literalTrue);
                r = env.constants.varTrue;
                ctx.b = C_ROOT;
                goto call_nonreif;
              } else {
                std::vector<Expression*> args_e(args.size() + 1);
                for (unsigned int i = 0; i < args.size(); i++) {
                  args_e[i] = args[i]();
                }
                args_e[args.size()] = env.constants.literalFalse;
                Call* reif_call = Call::a(Location().introduce(), r_cid, args_e);
                reif_call->type(Type::varbool());
                reif_call->decl(reif_decl);
                flat_exp(env, Ctx(), reif_call, env.constants.varTrue, env.constants.varTrue);
                args_ee.emplace_back(nullptr, env.constants.literalFalse);
                ret.r = conj(env, r, ctx, args_ee);
                ret.b = bind(env, ctx, b, env.constants.literalTrue);
                return ret;
              }
            }
          } else {
            reif_b = r;
          }
          // Annotate cr() with get_path()
          add_path_annotation(env, cr());
          reif_b->e(cr());
          if (r != nullptr && r != reif_b) {
            Ctx reif_ctx;
            reif_ctx.neg = ctx.neg;
            bind(env, reif_ctx, r, reif_b->id());
          }
          env.voAddExp(reif_b);
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          args_ee.emplace_back(nullptr, reif_b->id());
          ret.r = conj(env, nullptr, ctx, args_ee);
          if (!ctx.neg && !Expression::type(cr()).isAnn()) {
            env.cseMapInsert(cr(), ret);
          }
          return ret;
        }
      }
    call_nonreif:
      if (decl->e() == nullptr ||
          (Expression::type(cr()).isPar() && Expression::type(cr()).bt() != Type::BT_ANN &&
           !Expression::type(decl->e()).cv())) {
        Call* cr_c = Expression::cast<Call>(cr());
        /// All builtins are total
        std::vector<Type> argt(cr_c->argCount());
        for (auto i = static_cast<unsigned int>(argt.size()); (i--) != 0U;) {
          argt[i] = Expression::type(cr_c->arg(i));
        }
        Type callt = decl->rtype(env, argt, nullptr, false);
        if (callt.isPar()) {
          GCLock lock;
          if (callt.isbool() && !callt.isOpt()) {
            ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
            bool b = eval_bool(env, cr_c);
            EE ee;
            ee.b = env.constants.boollit(b);
            args_ee.push_back(ee);
            ret.r = conj(env, r, ctx, args_ee);
          } else {
            try {
              ret.r = bind(env, ctx, r, eval_par(env, cr_c));
              ret.b = conj(env, b, Ctx(), args_ee);
            } catch (ResultUndefinedError&) {
              ret.r = create_dummy_value(env, cr_c->type());
              ret.b = bind(env, Ctx(), b, env.constants.literalFalse);
              return ret;
            }
          }
          // Do not insert into map, since par results will quickly become
          // garbage anyway and then disappear from the map
        } else if (decl->builtins.e != nullptr) {
          KeepAlive callres;
          {
            GCLock lock;
            callres = decl->builtins.e(env, cr_c);
          }
          EE res = flat_exp(env, ctx, callres(), r, b);
          args_ee.push_back(res);
          ret.b = conj(env, b, Ctx(), args_ee);
          add_path_annotation(env, res.r());
          ret.r = bind(env, Ctx(), r, res.r());
          if (!ctx.neg && !cr_c->type().isAnn()) {
            env.cseMapInsert(cr_c, ret);
          }
        } else {
          GCLock lock;
          ret.b = conj(env, b, Ctx(), args_ee);
          add_path_annotation(env, cr_c);
          ret.r = bind(env, ctx, r, cr_c);
          if (!ctx.neg && !cr_c->type().isAnn()) {
            env.cseMapInsert(cr_c, ret);
          }
        }
      } else {
        std::vector<KeepAlive> previousParameters(decl->paramCount());
        KeepAlive previousCapture;
        if (decl->capturedAnnotationsVar() != nullptr) {
          previousCapture = decl->capturedAnnotationsVar()->e();
          GCLock lock;
          decl->capturedAnnotationsVar()->flat(decl->capturedAnnotationsVar());
          ArrayLit* al = env.createAnnotationArray(c->type().isbool() ? ctx.b : ctx.i);
          decl->capturedAnnotationsVar()->e(al);
        }
        for (unsigned int i = decl->paramCount(); (i--) != 0U;) {
          VarDecl* vd = decl->param(i);
          previousParameters[i] = vd->e();
          vd->flat(vd);
          vd->e(args[i]());
        }

        if (Expression::type(decl->e()).isbool() && !Expression::type(decl->e()).isOpt()) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          if (ctx.b == C_ROOT && r == env.constants.varTrue) {
            (void)flat_exp(env, Ctx(), decl->e(), r, env.constants.varTrue);
          } else {
            Ctx nctx;
            if (!is_total(env, decl)) {
              nctx = ctx;
              nctx.neg = false;
            }
            EE ee = flat_exp(env, nctx, decl->e(), nullptr, env.constants.varTrue);
            ee.b = ee.r;
            args_ee.push_back(ee);
          }
          ret.r = conj(env, r, ctx, args_ee);
        } else {
          if (is_total(env, decl)) {
            Ctx nctx;
            nctx.i = ctx.i;
            EE ee = flat_exp(env, nctx, decl->e(), r, env.constants.varTrue);
            ret.r = bind(env, ctx, r, ee.r());
          } else {
            ret = flat_exp(env, ctx, decl->e(), r, ctx.partialityVar(env));
            args_ee.push_back(ret);
            if (Expression::type(decl->e()).dim() > 0) {
              auto* al = Expression::cast<ArrayLit>(follow_id(ret.r()));
              assert(al->dims() == Expression::type(decl->e()).dim());
              for (unsigned int i = 0; i < decl->ti()->ranges().size(); i++) {
                if ((decl->ti()->ranges()[i]->domain() != nullptr) &&
                    !Expression::isa<TIId>(decl->ti()->ranges()[i]->domain())) {
                  GCLock lock;
                  IntSetVal* isv = eval_intset(env, decl->ti()->ranges()[i]->domain());
                  if (al->min(i) != isv->min() || al->max(i) != isv->max()) {
                    EE ee;
                    ee.b = env.constants.literalFalse;
                    args_ee.push_back(ee);
                  }
                }
              }
            }
            if ((decl->ti()->domain() != nullptr) && !Expression::isa<TIId>(decl->ti()->domain())) {
              KeepAlive domconstraint;
              {
                GCLock lock;
                domconstraint = mk_domain_constraint(env, ret.r(), decl->ti()->domain());
              }
              if (domconstraint() != nullptr) {
                if (ctx.b == C_ROOT) {
                  (void)flat_exp(env, Ctx(), domconstraint(), env.constants.varTrue,
                                 env.constants.varTrue);
                } else {
                  EE ee = flat_exp(env, Ctx(), domconstraint(), nullptr, env.constants.varTrue);
                  ee.b = ee.r;
                  args_ee.push_back(ee);
                }
              }
            }
          }
          ret.b = conj(env, b, Ctx(), args_ee);
        }
        if (!ctx.neg && !Expression::type(cr()).isAnn()) {
          env.cseMapInsert(cr(), ret);
        }

        // Restore previous mapping
        for (unsigned int i = decl->paramCount(); (i--) != 0U;) {
          VarDecl* vd = decl->param(i);
          vd->e(previousParameters[i]());
          vd->flat(vd->e() != nullptr ? vd : nullptr);
        }
        if (decl->capturedAnnotationsVar() != nullptr) {
          decl->capturedAnnotationsVar()->e(previousCapture());
          decl->capturedAnnotationsVar()->flat(decl->capturedAnnotationsVar()->e() != nullptr
                                                   ? decl->capturedAnnotationsVar()
                                                   : nullptr);
        }
      }
    }
  }
  if (cid == env.constants.ids.mzn_reverse_map_var) {
    env.inReverseMapVar = false;
  }
  return ret;
}
}  // namespace MiniZinc
