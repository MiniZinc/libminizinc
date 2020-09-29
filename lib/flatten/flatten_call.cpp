/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flat_exp.hh>

namespace MiniZinc {

std::vector<Expression*> to_exp_vec(std::vector<KeepAlive>& v) {
  std::vector<Expression*> r(v.size());
  for (auto i = static_cast<unsigned int>(v.size()); (i--) != 0U;) {
    r[i] = v[i]();
  }
  return r;
}

bool is_total(FunctionI* fi) { return fi->ann().contains(constants().ann.promise_total); }

Call* same_call(EnvI& env, Expression* e, const ASTString& id) {
  assert(GC::locked());
  Expression* ce = follow_id(e);
  Call* c = Expression::dynamicCast<Call>(ce);
  if (c != nullptr) {
    if (c->id() == id) {
      return ce->cast<Call>();
    }
    if (c->id() == constants().ids.int2float) {
      Expression* i2f = follow_id(c->arg(0));
      Call* i2fc = Expression::dynamicCast<Call>(i2f);
      if ((i2fc != nullptr) && i2fc->id() == id && id == constants().ids.lin_exp) {
        ArrayLit* coeffs = eval_array_lit(env, i2fc->arg(0));
        std::vector<Expression*> ncoeff_v(coeffs->size());
        for (unsigned int i = 0; i < coeffs->size(); i++) {
          ncoeff_v[i] = FloatLit::a(eval_int(env, (*coeffs)[i]));
        }
        auto* ncoeff = new ArrayLit(coeffs->loc().introduce(), ncoeff_v);
        ncoeff->type(Type::parfloat(1));
        ArrayLit* vars = eval_array_lit(env, i2fc->arg(1));
        std::vector<Expression*> n_vars_v(vars->size());
        for (unsigned int i = 0; i < vars->size(); i++) {
          Call* f2i =
              new Call((*vars)[i]->loc().introduce(), constants().ids.int2float, {(*vars)[i]});
          f2i->decl(env.model->matchFn(env, f2i, false));
          assert(f2i->decl());
          f2i->type(Type::varfloat());
          EE ee = flat_exp(env, Ctx(), f2i, nullptr, constants().varTrue);
          n_vars_v[i] = ee.r();
        }
        auto* nvars = new ArrayLit(vars->loc().introduce(), n_vars_v);
        nvars->type(Type::varfloat(1));
        FloatVal c = eval_int(env, i2fc->arg(2));
        Call* nlinexp = new Call(i2fc->loc().introduce(), constants().ids.lin_exp,
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
      if (x[i]()->isa<BoolLit>()) {
        if (x[i]()->cast<BoolLit>()->v() == identity) {
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
  Expression* al_arg = (cid == constants().ids.sum ? args_ee[0].r() : args_ee[1].r());
  EE flat_al = flat_exp(env, nctx, al_arg, nullptr, nullptr);
  auto* al = follow_id(flat_al.r())->template cast<ArrayLit>();
  KeepAlive al_ka = al;
  if (al->dims() > 1) {
    Type alt = al->type();
    alt.dim(1);
    GCLock lock;
    al = new ArrayLit(al->loc(), *al);
    al->type(alt);
    al_ka = al;
  }
  Val d = (cid == constants().ids.sum ? Val(0) : LinearTraits<Lit>::eval(env, args_ee[2].r()));

  std::vector<Val> c_coeff(al->size());
  if (cid == constants().ids.sum) {
    for (unsigned int i = al->size(); i--;) {
      c_coeff[i] = 1;
    }
  } else {
    EE flat_coeff = flat_exp(env, nctx, args_ee[0].r(), nullptr, nullptr);
    auto* coeff = follow_id(flat_coeff.r())->template cast<ArrayLit>();
    for (unsigned int i = coeff->size(); i--;) {
      c_coeff[i] = LinearTraits<Lit>::eval(env, (*coeff)[i]);
    }
  }
  cid = constants().ids.lin_exp;
  std::vector<Val> coeffv;
  std::vector<KeepAlive> alv;
  for (unsigned int i = 0; i < al->size(); i++) {
    GCLock lock;
    if (Call* sc = Expression::dynamicCast<Call>(same_call(env, (*al)[i], cid))) {
      if (auto* alvi_decl = follow_id_to_decl((*al)[i])->template dynamicCast<VarDecl>()) {
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
  Type t = coeff_ev[0]->type();
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
    if (rd->isa<VarDecl>()) {
      rd = rd->cast<VarDecl>()->id();
    }
    if (rd->type().dim() > 1) {
      ArrayLit* al = eval_array_lit(env, rd);
      std::vector<std::pair<int, int> > dims(1);
      dims[0].first = 1;
      dims[0].second = al->size();
      rd = new ArrayLit(al->loc(), *al, dims);
      Type t = al->type();
      t.dim(1);
      rd->type(t);
    }
    args.emplace_back(rd);
  } else {
    auto* nal = new ArrayLit(al->loc(), alv_e);
    nal->type(al->type());
    args.emplace_back(nal);
  }
  Lit* il = LinearTraits<Lit>::newLit(d);
  args.push_back(il);
}

/// Special form of disjunction for SCIP
bool is_totaladd_bounds_disj(EnvI& env, Expression* arg, Call* c_orig) {
  auto* pArrayLit = arg->dynamicCast<ArrayLit>();
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
    auto* pId = pArrayLit->operator[](i)->dynamicCast<Id>();
    if (nullptr == pId) {
      return false;
    }
    auto* pDecl = follow_id_to_decl(pId)->dynamicCast<VarDecl>();
    /// Checking the rhs
    auto* pRhs = pDecl->e();
    if (nullptr == pRhs) {
      return false;  // not checking this boolean
    }
    auto* pCall = pRhs->dynamicCast<Call>();
    if (nullptr == pCall) {
      return false;
    }
    if (constants().ids.int_.le != pCall->id() && constants().ids.float_.le != pCall->id()) {
      return false;
    }
    /// See if one is a constant and one a variable
    Expression* pConst = nullptr;
    Expression* pVar = nullptr;
    bool fFloat = false;
    bool isUB = false;
    for (unsigned int j = pCall->argCount(); (j--) != 0U;) {
      if (auto* pF = pCall->arg(j)->dynamicCast<FloatLit>()) {
        pConst = pF;
        fFloat = true;
        isUB = (1 == j);
      } else if (auto* pF = pCall->arg(j)->dynamicCast<IntLit>()) {
        pConst = pF;
        fFloat = false;
        isUB = (1 == j);
      } else if (auto* pId = pCall->arg(j)->dynamicCast<Id>()) {
        if (nullptr != pVar) {
          return false;  // 2 variables, exit
        }
        pVar = pId;
      }
    }
    /// All good, add them
    if (fFloat) {
      isUBF.push_back(constants().boollit(isUB));
      bndF.push_back(pConst);
      varF.push_back(pVar);
    } else {
      isUBI.push_back(constants().boollit(isUB));
      bndI.push_back(pConst);
      varI.push_back(pVar);
    }
  }
  /// Create new call
  GCLock lock;
  auto loc = c_orig->loc().introduce();
  std::vector<Expression*> args = {new ArrayLit(loc, isUBI), new ArrayLit(loc, bndI),
                                   new ArrayLit(loc, varI),  new ArrayLit(loc, isUBF),
                                   new ArrayLit(loc, bndF),  new ArrayLit(loc, varF)};

  Call* c =
      new Call(c_orig->loc().introduce(), env.model->getFnDecls().boundsDisj.second->id(), args);
  c->type(Type::varbool());
  c->decl(env.model->getFnDecls().boundsDisj.second);
  env.flatAddItem(new ConstraintI(c_orig->loc().introduce(), c));
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
  Call* c = e->cast<Call>();
  IgnorePartial ignorePartial(env, c);
  if (c->id().endsWith("_reif")) {
    env.counters.reifConstraints++;
  } else if (c->id().endsWith("_imp")) {
    env.counters.impConstraints++;
  }
  FunctionI* decl = env.model->matchFn(env, c, false);
  if (decl == nullptr) {
    std::ostringstream ss;
    ss << "undeclared function or predicate " << c->id();
    throw InternalError(ss.str());
  }

  Ctx ctx = input_ctx;
  Ctx nctx = ctx;
  nctx.neg = false;
  ASTString cid = c->id();
  CallStackItem _csi(env, e);

  if (cid == constants().ids.bool2int && c->type().dim() == 0) {
    if (ctx.neg) {
      ctx.neg = false;
      nctx.neg = true;
      nctx.b = -ctx.i;
    } else {
      nctx.b = ctx.i;
    }
  } else if (cid == constants().ids.forall) {
    nctx.b = +nctx.b;
    if (ctx.neg) {
      ctx.neg = false;
      nctx.neg = true;
      cid = constants().ids.exists;
    }
  } else if (cid == constants().ids.exists) {
    nctx.b = +nctx.b;
    if (ctx.neg) {
      ctx.neg = false;
      nctx.neg = true;
      cid = constants().ids.forall;
    }
  } else if (decl->e() == nullptr &&
             (cid == constants().ids.assert || cid == constants().ids.trace ||
              cid == constants().ids.mzn_deprecate)) {
    if (cid == constants().ids.assert && c->argCount() == 2) {
      (void)decl->builtins.b(env, c);
      ret = flat_exp(env, ctx, constants().literalTrue, r, b);
    } else {
      KeepAlive callres = decl->builtins.e(env, c);
      ret = flat_exp(env, ctx, callres(), r, b);
      // This is all we need to do for assert, so break out of the E_CALL
    }
    return ret;
  } else if ((decl->e() != nullptr) && ctx.b == C_ROOT && decl->e()->isa<BoolLit>() &&
             eval_bool(env, decl->e())) {
    bool allBool = true;
    for (unsigned int i = 0; i < c->argCount(); i++) {
      if (c->arg(i)->type().bt() != Type::BT_BOOL) {
        allBool = false;
        break;
      }
    }
    if (allBool) {
      ret.r = bind(env, ctx, r, constants().literalTrue);
      ret.b = bind(env, ctx, b, constants().literalTrue);
      return ret;
    }
  }

  if (ctx.b == C_ROOT && decl->e() == nullptr && cid == constants().ids.forall &&
      r == constants().varTrue) {
    ret.b = bind(env, ctx, b, constants().literalTrue);
    ArrayLit* al;
    if (c->arg(0)->isa<ArrayLit>()) {
      al = c->arg(0)->cast<ArrayLit>();
    } else {
      EE flat_al = flat_exp(env, Ctx(), c->arg(0), constants().varIgnore, constants().varTrue);
      al = follow_id(flat_al.r())->cast<ArrayLit>();
    }
    nctx.b = C_ROOT;
    for (unsigned int i = 0; i < al->size(); i++) {
      (void)flat_exp(env, nctx, (*al)[i], r, b);
    }
    ret.r = bind(env, ctx, r, constants().literalTrue);
  } else {
    if ((decl->e() != nullptr) && decl->params().size() == 1 && decl->e()->isa<Id>() &&
        decl->params()[0]->ti()->domain() == nullptr &&
        decl->e()->cast<Id>()->decl() == decl->params()[0]) {
      Expression* arg = c->arg(0);
      for (ExpressionSetIter esi = decl->e()->ann().begin(); esi != decl->e()->ann().end(); ++esi) {
        arg->addAnnotation(*esi);
      }
      for (ExpressionSetIter esi = c->ann().begin(); esi != c->ann().end(); ++esi) {
        arg->addAnnotation(*esi);
      }
      ret = flat_exp(env, ctx, c->arg(0), r, b);
      return ret;
    }

    std::vector<EE> args_ee(c->argCount());
    bool isPartial = false;

    if (cid == constants().ids.lin_exp && c->type().isint()) {
      // Linear expressions need special context handling:
      // the context of a variable expression depends on the corresponding coefficient

      // flatten the coefficient array
      Expression* tmp = follow_id_to_decl(c->arg(0));
      ArrayLit* coeffs;
      if (auto* vd = tmp->dynamicCast<VarDecl>()) {
        tmp = vd->id();
      }
      {
        CallArgItem cai(env);
        args_ee[0] = flat_exp(env, nctx, tmp, nullptr, nullptr);
        isPartial |= isfalse(env, args_ee[0].b());
        coeffs = eval_array_lit(env, args_ee[0].r());
      }

      ArrayLit* vars = eval_array_lit(env, c->arg(1));
      if (vars->flat()) {
        args_ee[1].r = vars;
        args_ee[1].b = constants().varTrue;
      } else {
        CallArgItem cai(env);
        CallStackItem _csi(env, c->arg(1));
        std::vector<EE> elems_ee(vars->size());
        for (unsigned int i = vars->size(); (i--) != 0U;) {
          Ctx argctx = nctx;
          argctx.i = eval_int(env, (*coeffs)[i]) < 0 ? -nctx.i : +nctx.i;
          elems_ee[i] = flat_exp(env, argctx, (*vars)[i], nullptr, nullptr);
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
        if (auto* vd = constant->dynamicCast<VarDecl>()) {
          constant = vd->id();
        }
        CallArgItem cai(env);
        args_ee[2] = flat_exp(env, nctx, constant, nullptr, nullptr);
        isPartial |= isfalse(env, args_ee[2].b());
      }

    } else {
      bool mixContext =
          (cid != constants().ids.forall && cid != constants().ids.exists &&
           (cid != constants().ids.bool2int || c->type().dim() > 0) && cid != constants().ids.sum &&
           cid != "assert" && cid != constants().varRedef->id() && cid != "mzn_reverse_map_var");
      if (cid == "mzn_reverse_map_var") {
        env.inReverseMapVar = true;
      }
      if (cid == constants().ids.clause && c->arg(0)->isa<ArrayLit>() &&
          c->arg(1)->isa<ArrayLit>()) {
        Ctx argctx = nctx;

        // handle negated args first, try to make them positive

        if (mixContext) {
          argctx.b = -nctx.b;
        }
        std::vector<KeepAlive> neg_args;
        std::vector<KeepAlive> pos_args;
        std::vector<KeepAlive> newPositives;
        bool is_subsumed = false;
        auto* al_neg = c->arg(1)->cast<ArrayLit>();
        {
          CallArgItem cai(env);
          for (unsigned int i = 0; i < al_neg->size(); i++) {
            auto* bo = (*al_neg)[i]->dynamicCast<BinOp>();
            Call* co = (*al_neg)[i]->dynamicCast<Call>();
            if ((bo != nullptr) || ((co != nullptr) && (co->id() == constants().ids.forall ||
                                                        co->id() == constants().ids.exists ||
                                                        co->id() == constants().ids.clause))) {
              GCLock lock;
              UnOp* notBoe0 = new UnOp(Location().introduce(), UOT_NOT, (*al_neg)[i]);
              notBoe0->type(Type::varbool());
              newPositives.emplace_back(notBoe0);
            } else {
              EE res = flat_exp(env, argctx, (*al_neg)[i], nullptr, constants().varTrue);
              if (res.r()->type().isPar()) {
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
        auto* al_pos = c->arg(0)->cast<ArrayLit>();
        for (unsigned int i = 0; i < al_pos->size(); i++) {
          newPositives.emplace_back((*al_pos)[i]);
        }
        {
          CallArgItem cai(env);
          for (auto& newPositive : newPositives) {
            EE res = flat_exp(env, argctx, newPositive(), nullptr, constants().varTrue);
            if (res.r()->type().isPar()) {
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
        auto* al_new_pos = new ArrayLit(al_pos->loc(), to_exp_vec(pos_args));
        al_new_pos->type(Type::varbool(1));
        al_new_pos->flat(true);
        args_ee[0] = EE(al_new_pos, constants().literalTrue);
        auto* al_new_neg = new ArrayLit(al_neg->loc(), to_exp_vec(neg_args));
        al_new_neg->flat(true);
        al_new_neg->type(Type::varbool(1));
        args_ee[1] = EE(al_new_neg, constants().literalTrue);
      } else if ((cid == constants().ids.forall || cid == constants().ids.exists) &&
                 c->arg(0)->isa<ArrayLit>()) {
        bool is_conj = (cid == constants().ids.forall);
        Ctx argctx = nctx;
        if (mixContext) {
          argctx.b = C_MIX;
        }
        auto* al = c->arg(0)->cast<ArrayLit>();
        ArrayLit* al_new;
        if (al->flat()) {
          al_new = al;
        } else {
          std::vector<KeepAlive> flat_args;
          CallArgItem cai(env);
          for (unsigned int i = 0; i < al->size(); i++) {
            EE res = flat_exp(env, argctx, (*al)[i], nullptr, constants().varTrue);
            if (res.r()->type().isPar()) {
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
          GCLock lock;
          al_new = new ArrayLit(al->loc(), to_exp_vec(flat_args));
          al_new->type(Type::varbool(1));
          al_new->flat(true);
        }
        args_ee[0] = EE(al_new, constants().literalTrue);
      } else {
        for (unsigned int i = c->argCount(); (i--) != 0U;) {
          Ctx argctx = nctx;
          if (mixContext) {
            if (cid == constants().ids.clause) {
              argctx.b = (i == 0 ? +nctx.b : -nctx.b);
            } else if (c->arg(i)->type().bt() == Type::BT_BOOL) {
              argctx.b = C_MIX;
            } else if (c->arg(i)->type().bt() == Type::BT_INT) {
              argctx.i = C_MIX;
            }
          } else if (cid == constants().ids.sum && c->arg(i)->type().bt() == Type::BT_BOOL) {
            argctx.b = argctx.i;
          }
          Expression* tmp = follow_id_to_decl(c->arg(i));
          if (auto* vd = tmp->dynamicCast<VarDecl>()) {
            tmp = vd->id();
          }
          CallArgItem cai(env);
          args_ee[i] = flat_exp(env, argctx, tmp, nullptr, nullptr);
          isPartial |= isfalse(env, args_ee[i].b());
        }
      }
    }
    if (isPartial && c->type().isbool() && !c->type().isOpt()) {
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      args_ee.resize(1);
      args_ee[0] = EE(nullptr, constants().literalFalse);
      ret.r = conj(env, r, ctx, args_ee);
      return ret;
    }

    std::vector<KeepAlive> args;
    if (decl->e() == nullptr && (cid == constants().ids.exists || cid == constants().ids.clause)) {
      std::vector<KeepAlive> pos_alv;
      std::vector<KeepAlive> neg_alv;

      std::vector<Expression*> pos_stack;
      std::vector<Expression*> neg_stack;

      auto* al_pos = follow_id(args_ee[0].r())->cast<ArrayLit>();
      for (unsigned int i = 0; i < al_pos->size(); i++) {
        pos_stack.push_back((*al_pos)[i]);
      }
      if (cid == constants().ids.clause) {
        auto* al_neg = follow_id(args_ee[1].r())->cast<ArrayLit>();
        for (unsigned int i = 0; i < al_neg->size(); i++) {
          neg_stack.push_back((*al_neg)[i]);
        }
      }

      std::unordered_set<Expression*> seen;

      while (!pos_stack.empty() || !neg_stack.empty()) {
        while (!pos_stack.empty()) {
          Expression* cur = pos_stack.back();
          pos_stack.pop_back();
          if (cur->isa<Id>() && seen.find(cur) != seen.end()) {
            pos_alv.emplace_back(cur);
          } else {
            seen.insert(cur);
            GCLock lock;
            if (Call* sc =
                    Expression::dynamicCast<Call>(same_call(env, cur, constants().ids.exists))) {
              GCLock lock;
              ArrayLit* sc_c = eval_array_lit(env, sc->arg(0));
              for (unsigned int j = 0; j < sc_c->size(); j++) {
                pos_stack.push_back((*sc_c)[j]);
              }
            } else if (Call* sc = Expression::dynamicCast<Call>(
                           same_call(env, cur, constants().ids.clause))) {
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
                  Expression::dynamicCast<Call>(same_call(env, cur, constants().ids.bool_eq));
              Call* not_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, constants().ids.bool_not));
              if ((eq_call != nullptr) &&
                  Expression::equal(eq_call->arg(1), constants().literalFalse)) {
                neg_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), constants().literalFalse)) {
                neg_stack.push_back(eq_call->arg(1));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(1), constants().literalTrue)) {
                pos_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), constants().literalTrue)) {
                pos_stack.push_back(eq_call->arg(1));
              } else if ((not_call != nullptr) && not_call->argCount() == 1) {
                neg_stack.push_back(not_call->arg(0));
              } else if (Id* ident = cur->dynamicCast<Id>()) {
                if (ident->decl()->ti()->domain() != constants().literalFalse) {
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
          if (cur->isa<Id>() && seen.find(cur) != seen.end()) {
            neg_alv.emplace_back(cur);
          } else {
            seen.insert(cur);
            if (Call* sc =
                    Expression::dynamicCast<Call>(same_call(env, cur, constants().ids.forall))) {
              GCLock lock;
              ArrayLit* sc_c = eval_array_lit(env, sc->arg(0));
              for (unsigned int j = 0; j < sc_c->size(); j++) {
                neg_stack.push_back((*sc_c)[j]);
              }
            } else {
              Call* eq_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, constants().ids.bool_eq));
              Call* not_call =
                  Expression::dynamicCast<Call>(same_call(env, cur, constants().ids.bool_not));
              if ((eq_call != nullptr) &&
                  Expression::equal(eq_call->arg(1), constants().literalFalse)) {
                pos_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), constants().literalFalse)) {
                pos_stack.push_back(eq_call->arg(1));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(1), constants().literalTrue)) {
                neg_stack.push_back(eq_call->arg(0));
              } else if ((eq_call != nullptr) &&
                         Expression::equal(eq_call->arg(0), constants().literalTrue)) {
                neg_stack.push_back(eq_call->arg(1));
              } else if ((not_call != nullptr) && not_call->argCount() == 1) {
                pos_stack.push_back(not_call->arg(0));
              } else if (Id* ident = cur->dynamicCast<Id>()) {
                if (ident->decl()->ti()->domain() != constants().literalTrue) {
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
        ret.b = bind(env, Ctx(), b, constants().literalTrue);
        ret.r = bind(env, ctx, r, constants().literalTrue);
        return ret;
      }
      if (neg_alv.empty()) {
        if (pos_alv.empty()) {
          ret.b = bind(env, Ctx(), b, constants().literalTrue);
          ret.r = bind(env, ctx, r, constants().literalFalse);
          return ret;
        }
        if (pos_alv.size() == 1) {
          ret.b = bind(env, Ctx(), b, constants().literalTrue);
          ret.r = bind(env, ctx, r, pos_alv[0]());
          return ret;
        }
        GCLock lock;
        auto* nal = new ArrayLit(Location().introduce(), to_exp_vec(pos_alv));
        nal->type(Type::varbool(1));
        args.emplace_back(nal);
        cid = constants().ids.exists;
      } else {
        if (pos_alv.empty() && neg_alv.size() == 1) {
          ret.b = bind(env, Ctx(), b, constants().literalTrue);
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
        cid = constants().ids.clause;
        args.emplace_back(pos_al);
        args.emplace_back(neg_al);
      }
      if (C_ROOT == ctx.b && cid == constants().ids.exists) {
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
            ret.b = bind(env, Ctx(), b, constants().literalTrue);
            ret.r = bind(env, ctx, r, constants().literalTrue);
            return ret;
          }
        }
      }

    } else if (decl->e() == nullptr && cid == constants().ids.forall) {
      auto* al = follow_id(args_ee[0].r())->cast<ArrayLit>();
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
        ret.b = bind(env, Ctx(), b, constants().literalTrue);
        ret.r = bind(env, ctx, r, constants().literalFalse);
        return ret;
      }
      if (alv.empty()) {
        ret.b = bind(env, Ctx(), b, constants().literalTrue);
        ret.r = bind(env, ctx, r, constants().literalTrue);
        return ret;
      }
      if (alv.size() == 1) {
        ret.b = bind(env, Ctx(), b, constants().literalTrue);
        ret.r = bind(env, ctx, r, alv[0]());
        return ret;
      }
      GCLock lock;
      auto* nal = new ArrayLit(al->loc(), to_exp_vec(alv));
      nal->type(al->type());
      args.emplace_back(nal);
    } else if (decl->e() == nullptr &&
               (cid == constants().ids.lin_exp || cid == constants().ids.sum)) {
      if (e->type().isint()) {
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
      Call* cr_c = new Call(c->loc().introduce(), cid, e_args);
      decl = env.model->matchFn(env, cr_c, false);
      if (decl == nullptr) {
        throw FlatteningError(env, cr_c->loc(), "cannot find matching declaration");
      }
      cr_c->type(decl->rtype(env, e_args, false));
      assert(decl);
      cr_c->decl(decl);
      cr = cr_c;
    }
    if (hadImplementation && decl->e() == nullptr &&
        (cid == constants().ids.lin_exp || cid == constants().ids.sum)) {
      args.clear();
      if (e->type().isint()) {
        flatten_linexp_call<IntLit>(env, ctx, nctx, cid, cr()->cast<Call>(), ret, b, r, args_ee,
                                    args);
      } else {
        flatten_linexp_call<FloatLit>(env, ctx, nctx, cid, cr()->cast<Call>(), ret, b, r, args_ee,
                                      args);
      }
      if (args.empty()) {
        return ret;
      }
      GCLock lock;
      std::vector<Expression*> e_args = to_exp_vec(args);
      Call* cr_c = new Call(c->loc().introduce(), cid, e_args);
      decl = env.model->matchFn(env, cr_c, false);
      if (decl == nullptr) {
        throw FlatteningError(env, cr_c->loc(), "cannot find matching declaration");
      }
      cr_c->type(decl->rtype(env, e_args, false));
      assert(decl);
      cr_c->decl(decl);
      cr = cr_c;
    }
    auto cit = env.cseMapFind(cr());
    if (cit != env.cseMapEnd()) {
      if (env.ignorePartial) {
        ret.b = bind(env, Ctx(), b, constants().literalTrue);
      } else {
        args_ee.emplace_back(nullptr, cit->second.b());
        ret.b = conj(env, b, Ctx(), args_ee);
      }
      ret.r = bind(env, ctx, r, cit->second.r());
    } else {
      for (unsigned int i = 0; i < decl->params().size(); i++) {
        if (decl->params()[i]->type().dim() > 0) {
          // Check array index sets
          auto* al = follow_id(args[i]())->cast<ArrayLit>();
          VarDecl* pi = decl->params()[i];
          for (unsigned int j = 0; j < pi->ti()->ranges().size(); j++) {
            TypeInst* range_ti = pi->ti()->ranges()[j];
            if ((range_ti->domain() != nullptr) && !range_ti->domain()->isa<TIId>()) {
              GCLock lock;
              IntSetVal* isv = eval_intset(env, range_ti->domain());
              if (isv->min() != al->min(j) || isv->max() != al->max(j)) {
                std::ostringstream oss;
                oss << "array index set " << (j + 1) << " of argument " << (i + 1)
                    << " does not match declared index set";
                throw FlatteningError(env, e->loc(), oss.str());
              }
            }
          }
        }
        if (Expression* dom = decl->params()[i]->ti()->domain()) {
          if (!dom->isa<TIId>()) {
            // May have to constrain actual argument
            if (args[i]()->type().bt() == Type::BT_INT) {
              GCLock lock;
              IntSetVal* isv = eval_intset(env, dom);
              BinOpType bot;
              bool needToConstrain;
              if (args[i]()->type().st() == Type::ST_SET) {
                bot = BOT_SUBSET;
                needToConstrain = true;
              } else {
                bot = BOT_IN;
                if (args[i]()->type().dim() > 0) {
                  needToConstrain = true;
                } else {
                  IntBounds ib = compute_int_bounds(env, args[i]());
                  needToConstrain = !ib.valid || isv->size() == 0 || ib.l < isv->min(0) ||
                                    ib.u > isv->max(isv->size() - 1);
                }
              }
              if (needToConstrain) {
                GCLock lock;
                Expression* domconstraint;
                if (args[i]()->type().dim() > 0) {
                  std::vector<Expression*> domargs(2);
                  domargs[0] = args[i]();
                  domargs[1] = dom;
                  Call* c = new Call(Location().introduce(), "var_dom", domargs);
                  c->type(Type::varbool());
                  c->decl(env.model->matchFn(env, c, false));
                  if (c->decl() == nullptr) {
                    throw InternalError("no matching declaration found for var_dom");
                  }
                  domconstraint = c;
                } else {
                  domconstraint = new BinOp(Location().introduce(), args[i](), bot, dom);
                }
                domconstraint->type(args[i]()->type().isPar() ? Type::parbool() : Type::varbool());
                if (ctx.b == C_ROOT) {
                  (void)flat_exp(env, Ctx(), domconstraint, constants().varTrue,
                                 constants().varTrue);
                } else {
                  EE ee = flat_exp(env, Ctx(), domconstraint, nullptr, constants().varTrue);
                  ee.b = ee.r;
                  args_ee.push_back(ee);
                }
              }
            } else if (args[i]()->type().bt() == Type::BT_FLOAT) {
              GCLock lock;

              FloatSetVal* fsv = eval_floatset(env, dom);
              bool needToConstrain;
              if (args[i]()->type().dim() > 0) {
                needToConstrain = true;
              } else {
                FloatBounds fb = compute_float_bounds(env, args[i]());
                needToConstrain = !fb.valid || fsv->size() == 0 || fb.l < fsv->min(0) ||
                                  fb.u > fsv->max(fsv->size() - 1);
              }

              if (needToConstrain) {
                GCLock lock;
                Expression* domconstraint;
                if (args[i]()->type().dim() > 0) {
                  std::vector<Expression*> domargs(2);
                  domargs[0] = args[i]();
                  domargs[1] = dom;
                  Call* c = new Call(Location().introduce(), "var_dom", domargs);
                  c->type(Type::varbool());
                  c->decl(env.model->matchFn(env, c, false));
                  if (c->decl() == nullptr) {
                    throw InternalError("no matching declaration found for var_dom");
                  }
                  domconstraint = c;
                } else {
                  domconstraint = new BinOp(Location().introduce(), args[i](), BOT_IN, dom);
                }
                domconstraint->type(args[i]()->type().isPar() ? Type::parbool() : Type::varbool());
                if (ctx.b == C_ROOT) {
                  (void)flat_exp(env, Ctx(), domconstraint, constants().varTrue,
                                 constants().varTrue);
                } else {
                  EE ee = flat_exp(env, Ctx(), domconstraint, nullptr, constants().varTrue);
                  ee.b = ee.r;
                  args_ee.push_back(ee);
                }
              }
            } else if (args[i]()->type().bt() == Type::BT_BOT) {
              // Nothing to be done for empty arrays/sets
            } else {
              throw EvalError(env, decl->params()[i]->loc(),
                              "domain restrictions other than int and float not supported yet");
            }
          }
        }
      }
      if (cr()->type().isbool() && !cr()->type().isPar() && !cr()->type().isOpt() &&
          (ctx.b != C_ROOT || r != constants().varTrue)) {
        std::vector<Type> argtypes(args.size());
        for (unsigned int i = 0; i < args.size(); i++) {
          argtypes[i] = args[i]()->type();
        }
        argtypes.push_back(Type::varbool());
        GCLock lock;
        ASTString r_cid = env.reifyId(cid);
        FunctionI* reif_decl = env.model->matchFn(env, r_cid, argtypes, false);
        if ((reif_decl != nullptr) && (reif_decl->e() != nullptr)) {
          add_path_annotation(env, reif_decl->e());
          VarDecl* reif_b;
          if (r == nullptr || (r != nullptr && r->e() != nullptr)) {
            reif_b = new_vardecl(env, Ctx(), new TypeInst(Location().introduce(), Type::varbool()),
                                 nullptr, nullptr, nullptr);
            add_ctx_ann(reif_b, ctx.b);
            if (reif_b->ti()->domain() != nullptr) {
              if (reif_b->ti()->domain() == constants().literalTrue) {
                bind(env, ctx, r, constants().literalTrue);
                r = constants().varTrue;
                ctx.b = C_ROOT;
                goto call_nonreif;
              } else {
                std::vector<Expression*> args_e(args.size() + 1);
                for (unsigned int i = 0; i < args.size(); i++) {
                  args_e[i] = args[i]();
                }
                args_e[args.size()] = constants().literalFalse;
                Call* reif_call = new Call(Location().introduce(), r_cid, args_e);
                reif_call->type(Type::varbool());
                reif_call->decl(reif_decl);
                flat_exp(env, Ctx(), reif_call, constants().varTrue, constants().varTrue);
                args_ee.emplace_back(nullptr, constants().literalFalse);
                ret.r = conj(env, r, ctx, args_ee);
                ret.b = bind(env, ctx, b, constants().literalTrue);
                return ret;
              }
            }
          } else {
            reif_b = r;
          }
          // Annotate cr() with get_path()
          add_path_annotation(env, cr());
          reif_b->e(cr());
          if (r != nullptr && r->e() != nullptr) {
            Ctx reif_ctx;
            reif_ctx.neg = ctx.neg;
            bind(env, reif_ctx, r, reif_b->id());
          }
          env.voAddExp(reif_b);
          ret.b = bind(env, Ctx(), b, constants().literalTrue);
          args_ee.emplace_back(nullptr, reif_b->id());
          ret.r = conj(env, nullptr, ctx, args_ee);
          if (!ctx.neg && !cr()->type().isAnn()) {
            env.cseMapInsert(cr(), ret);
          }
          return ret;
        }
      }
    call_nonreif:
      if (decl->e() == nullptr ||
          (cr()->type().isPar() && !cr()->type().isAnn() && !decl->e()->type().cv())) {
        Call* cr_c = cr()->cast<Call>();
        /// All builtins are total
        std::vector<Type> argt(cr_c->argCount());
        for (auto i = static_cast<unsigned int>(argt.size()); (i--) != 0U;) {
          argt[i] = cr_c->arg(i)->type();
        }
        Type callt = decl->rtype(env, argt, false);
        if (callt.isPar() && callt.bt() != Type::BT_ANN) {
          GCLock lock;
          try {
            ret.r = bind(env, ctx, r, eval_par(env, cr_c));
            ret.b = conj(env, b, Ctx(), args_ee);
          } catch (ResultUndefinedError&) {
            ret.r = create_dummy_value(env, cr_c->type());
            ret.b = bind(env, Ctx(), b, constants().literalFalse);
            return ret;
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
          ret.r = bind(env, ctx, r, res.r());
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
        std::vector<KeepAlive> previousParameters(decl->params().size());
        for (unsigned int i = decl->params().size(); (i--) != 0U;) {
          VarDecl* vd = decl->params()[i];
          previousParameters[i] = vd->e();
          vd->flat(vd);
          vd->e(args[i]());
        }

        if (decl->e()->type().isbool() && !decl->e()->type().isOpt()) {
          ret.b = bind(env, Ctx(), b, constants().literalTrue);
          if (ctx.b == C_ROOT && r == constants().varTrue) {
            (void)flat_exp(env, Ctx(), decl->e(), r, constants().varTrue);
          } else {
            Ctx nctx;
            if (!is_total(decl)) {
              nctx = ctx;
              nctx.neg = false;
            }
            EE ee = flat_exp(env, nctx, decl->e(), nullptr, constants().varTrue);
            ee.b = ee.r;
            args_ee.push_back(ee);
          }
          ret.r = conj(env, r, ctx, args_ee);
        } else {
          if (is_total(decl)) {
            EE ee = flat_exp(env, Ctx(), decl->e(), r, constants().varTrue);
            ret.r = bind(env, ctx, r, ee.r());
          } else {
            ret = flat_exp(env, ctx, decl->e(), r, nullptr);
            args_ee.push_back(ret);
            if (decl->e()->type().dim() > 0) {
              auto* al = follow_id(ret.r())->cast<ArrayLit>();
              assert(al->dims() == decl->e()->type().dim());
              for (unsigned int i = 0; i < decl->ti()->ranges().size(); i++) {
                if ((decl->ti()->ranges()[i]->domain() != nullptr) &&
                    !decl->ti()->ranges()[i]->domain()->isa<TIId>()) {
                  GCLock lock;
                  IntSetVal* isv = eval_intset(env, decl->ti()->ranges()[i]->domain());
                  if (al->min(i) != isv->min() || al->max(i) != isv->max()) {
                    EE ee;
                    ee.b = constants().literalFalse;
                    args_ee.push_back(ee);
                  }
                }
              }
            }
            if ((decl->ti()->domain() != nullptr) && !decl->ti()->domain()->isa<TIId>()) {
              BinOpType bot;
              if (ret.r()->type().st() == Type::ST_SET) {
                bot = BOT_SUBSET;
              } else {
                bot = BOT_IN;
              }

              KeepAlive domconstraint;
              if (decl->e()->type().dim() > 0) {
                GCLock lock;
                std::vector<Expression*> domargs(2);
                domargs[0] = ret.r();
                domargs[1] = decl->ti()->domain();
                Call* c = new Call(Location().introduce(), "var_dom", domargs);
                c->type(Type::varbool());
                c->decl(env.model->matchFn(env, c, false));
                if (c->decl() == nullptr) {
                  throw InternalError("no matching declaration found for var_dom");
                }
                domconstraint = c;
              } else {
                GCLock lock;
                domconstraint =
                    new BinOp(Location().introduce(), ret.r(), bot, decl->ti()->domain());
              }
              domconstraint()->type(ret.r()->type().isPar() ? Type::parbool() : Type::varbool());
              if (ctx.b == C_ROOT) {
                (void)flat_exp(env, Ctx(), domconstraint(), constants().varTrue,
                               constants().varTrue);
              } else {
                EE ee = flat_exp(env, Ctx(), domconstraint(), nullptr, constants().varTrue);
                ee.b = ee.r;
                args_ee.push_back(ee);
              }
            }
          }
          ret.b = conj(env, b, Ctx(), args_ee);
        }
        if (!ctx.neg && !cr()->type().isAnn()) {
          env.cseMapInsert(cr(), ret);
        }

        // Restore previous mapping
        for (unsigned int i = decl->params().size(); (i--) != 0U;) {
          VarDecl* vd = decl->params()[i];
          vd->e(previousParameters[i]());
          vd->flat(vd->e() != nullptr ? vd : nullptr);
        }
      }
    }
  }
  if (cid == "mzn_reverse_map_var") {
    env.inReverseMapVar = false;
  }
  return ret;
}
}  // namespace MiniZinc
