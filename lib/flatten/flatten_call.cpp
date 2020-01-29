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

  std::vector<Expression*> toExpVec(std::vector<KeepAlive>& v) {
    std::vector<Expression*> r(v.size());
    for (unsigned int i=static_cast<unsigned int>(v.size()); i--;)
      r[i] = v[i]();
    return r;
  }

  bool isTotal(FunctionI* fi) {
    return fi->ann().contains(constants().ann.promise_total);
  }
  
  Call* same_call(EnvI& env, Expression* e, const ASTString& id) {
    assert(GC::locked());
    Expression* ce = follow_id(e);
    Call* c = Expression::dyn_cast<Call>(ce);
    if (c) {
      if (c->id() == id) {
        return ce->cast<Call>();
      } else if (c->id() == constants().ids.int2float) {
        Expression* i2f = follow_id(c->arg(0));
        Call* i2fc = Expression::dyn_cast<Call>(i2f);
        if (i2fc && i2fc->id() == id && id==constants().ids.lin_exp) {
          ArrayLit* coeffs = eval_array_lit(env, i2fc->arg(0));
          std::vector<Expression*> ncoeff_v(coeffs->size());
          for (unsigned int i=0; i<coeffs->size(); i++) {
            ncoeff_v[i] = FloatLit::a(eval_int(env, (*coeffs)[i]));
          }
          ArrayLit* ncoeff = new ArrayLit(coeffs->loc().introduce(), ncoeff_v);
          ncoeff->type(Type::parfloat(1));
          ArrayLit* vars = eval_array_lit(env, i2fc->arg(1));
          std::vector<Expression*> n_vars_v(vars->size());
          for (unsigned int i=0; i<vars->size(); i++) {
            Call* f2i = new Call((*vars)[i]->loc().introduce(), constants().ids.int2float, {(*vars)[i]});
            f2i->decl(env.model->matchFn(env, f2i, false));
            assert(f2i->decl());
            f2i->type(Type::varfloat());
            EE ee = flat_exp(env, Ctx(), f2i, NULL, constants().var_true);
            n_vars_v[i] = ee.r();
          }
          ArrayLit* nvars = new ArrayLit(vars->loc().introduce(), n_vars_v);
          nvars->type(Type::varfloat(1));
          FloatVal c = eval_int(env, i2fc->arg(2));
          Call* nlinexp = new Call(i2fc->loc().introduce(), constants().ids.lin_exp, {ncoeff, nvars, FloatLit::a(c)});
          nlinexp->decl(env.model->matchFn(env, nlinexp, false));
          assert(nlinexp->decl());
          nlinexp->type(Type::varfloat());
          return nlinexp;
        }
      }
    }
    return NULL;
  }

  class CmpExp {
  public:
    bool operator ()(const KeepAlive& i, const KeepAlive& j) const {
      if (Expression::equal(i(),j()))
        return false;
      return i()<j();
    }
  };

  bool remove_dups(std::vector<KeepAlive>& x, bool identity) {
    for (unsigned int i=0; i<x.size(); i++) {
      x[i] = follow_id_to_value(x[i]());
    }
    std::sort(x.begin(),x.end(),CmpExp());
    int ci = 0;
    Expression* prev = NULL;
    for (unsigned int i=0; i<x.size(); i++) {
      if (!Expression::equal(x[i](),prev)) {
        prev = x[i]();
        if (x[i]()->isa<BoolLit>()) {
          if (x[i]()->cast<BoolLit>()->v()==identity) {
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
    if (x.size()==0 || y.size()==0)
      return false;
    unsigned int ix=0;
    unsigned int iy=0;
    for (;;) {
      if (x[ix]()==y[iy]())
        return true;
      if (x[ix]() < y[iy]()) {
        ix++;
      } else {
        iy++;
      }
      if (ix==x.size() || iy==y.size())
        return false;
    }
  }
  
  template<class Lit>
  void flatten_linexp_call(EnvI& env, Ctx ctx, Ctx nctx, ASTString& cid, Call* c,
                           EE& ret, VarDecl* b, VarDecl* r,
                           std::vector<EE>& args_ee, std::vector<KeepAlive>& args) {
    typedef typename LinearTraits<Lit>::Val Val;
    Expression* al_arg = (cid==constants().ids.sum ? args_ee[0].r() : args_ee[1].r());
    EE flat_al = flat_exp(env,nctx,al_arg,NULL,NULL);
    ArrayLit* al = follow_id(flat_al.r())->template cast<ArrayLit>();
    KeepAlive al_ka = al;
    if (al->dims()>1) {
      Type alt = al->type();
      alt.dim(1);
      GCLock lock;
      al = new ArrayLit(al->loc(),*al);
      al->type(alt);
      al_ka = al;
    }
    Val d = (cid == constants().ids.sum ? Val(0) : LinearTraits<Lit>::eval(env,args_ee[2].r()));
    
    std::vector<Val> c_coeff(al->size());
    if (cid==constants().ids.sum) {
      for (unsigned int i=al->size(); i--;)
        c_coeff[i] = 1;
    } else {
      EE flat_coeff = flat_exp(env,nctx,args_ee[0].r(),NULL,NULL);
      ArrayLit* coeff = follow_id(flat_coeff.r())->template cast<ArrayLit>();
      for (unsigned int i=coeff->size(); i--;)
        c_coeff[i] = LinearTraits<Lit>::eval(env,(*coeff)[i]);
    }
    cid = constants().ids.lin_exp;
    std::vector<Val> coeffv;
    std::vector<KeepAlive> alv;
    for (unsigned int i=0; i<al->size(); i++) {
      GCLock lock;
      if (Call* sc = Expression::dyn_cast<Call>(same_call(env,(*al)[i],cid))) {
        if (VarDecl* alvi_decl = follow_id_to_decl((*al)[i])->dyn_cast<VarDecl>()) {
          if (alvi_decl->ti()->domain()) {
            // Test if the variable has tighter declared bounds than what can be inferred
            // from its RHS. If yes, keep the variable (don't aggregate), because the tighter
            // bounds are actually a constraint
            typename LinearTraits<Lit>::Domain sc_dom = LinearTraits<Lit>::eval_domain(env,alvi_decl->ti()->domain());
            typename LinearTraits<Lit>::Bounds sc_bounds = LinearTraits<Lit>::compute_bounds(env,sc);
            if (LinearTraits<Lit>::domain_tighter(sc_dom, sc_bounds)) {
              coeffv.push_back(c_coeff[i]);
              alv.push_back((*al)[i]);
              continue;
            }
          }
        }
        
        Val cd = c_coeff[i];
        ArrayLit* sc_coeff = eval_array_lit(env,sc->arg(0));
        ArrayLit* sc_al = eval_array_lit(env,sc->arg(1));
        Val sc_d = LinearTraits<Lit>::eval(env,sc->arg(2));
        assert(sc_coeff->size() == sc_al->size());
        for (unsigned int j=0; j<sc_coeff->size(); j++) {
          coeffv.push_back(cd*LinearTraits<Lit>::eval(env,(*sc_coeff)[j]));
          alv.push_back((*sc_al)[j]);
        }
        d += cd*sc_d;
      } else {
        coeffv.push_back(c_coeff[i]);
        alv.push_back((*al)[i]);
      }
    }
    simplify_lin<Lit>(coeffv,alv,d);
    if (coeffv.size()==0) {
      GCLock lock;
      ret.b = conj(env,b,Ctx(),args_ee);
      ret.r = bind(env,ctx,r,LinearTraits<Lit>::newLit(d));
      return;
    } else if (coeffv.size()==1 && coeffv[0]==1 && d==0) {
      ret.b = conj(env,b,Ctx(),args_ee);
      ret.r = bind(env,ctx,r,alv[0]());
      return;
    }
    GCLock lock;
    std::vector<Expression*> coeff_ev(coeffv.size());
    for (unsigned int i=static_cast<unsigned int>(coeff_ev.size()); i--;)
      coeff_ev[i] = LinearTraits<Lit>::newLit(coeffv[i]);
    ArrayLit* ncoeff = new ArrayLit(Location().introduce(),coeff_ev);
    Type t = coeff_ev[0]->type();
    t.dim(1);
    ncoeff->type(t);
    args.push_back(ncoeff);
    std::vector<Expression*> alv_e(alv.size());
    bool al_same_as_before = alv.size()==al->size();
    for (unsigned int i=static_cast<unsigned int>(alv.size()); i--;) {
      alv_e[i] = alv[i]();
      al_same_as_before = al_same_as_before && Expression::equal(alv_e[i],(*al)[i]);
    }
    if (al_same_as_before) {
      Expression* rd = follow_id_to_decl(flat_al.r());
      if (rd->isa<VarDecl>())
        rd = rd->cast<VarDecl>()->id();
      if (rd->type().dim()>1) {
        ArrayLit* al = eval_array_lit(env,rd);
        std::vector<std::pair<int,int> > dims(1);
        dims[0].first = 1;
        dims[0].second = al->size();
        rd = new ArrayLit(al->loc(),*al,dims);
        Type t = al->type();
        t.dim(1);
        rd->type(t);
      }
      args.push_back(rd);
    } else {
      ArrayLit* nal = new ArrayLit(al->loc(),alv_e);
      nal->type(al->type());
      args.push_back(nal);
    }
    Lit* il = LinearTraits<Lit>::newLit(d);
    args.push_back(il);
  }
  
  /// Special form of disjunction for SCIP
  bool addBoundsDisj(EnvI& env, Expression* arg, Call* c_orig) {
    auto pArrayLit = arg->dyn_cast<ArrayLit>();
    if (nullptr == pArrayLit)
      return false;
    std::vector<Expression*>
        isUBI, bndI, varI,         // integer bounds and vars
        isUBF, bndF, varF;         // float bounds and vars
    for (int i=pArrayLit->size(); i--; ) {
      auto pId=pArrayLit->operator [](i)->dyn_cast<Id>();
      if (nullptr==pId)
        return false;
      auto pDecl=follow_id_to_decl(pId)->dyn_cast<VarDecl>();
      /// Checking the rhs
      auto pRhs=pDecl->e();
      if (nullptr==pRhs)
        return false;              // not checking this boolean
      auto pCall=pRhs->dyn_cast<Call>();
      if (nullptr==pCall)
        return false;
      if (constants().ids.int_.le!=pCall->id() && constants().ids.float_.le!=pCall->id())
        return false;
      /// See if one is a constant and one a variable
      Expression *pConst=nullptr, *pVar=nullptr;
      bool fFloat=false;
      bool isUB=false;
      for (int j=pCall->n_args(); j--; ) {
        if (auto pF=pCall->arg(j)->dyn_cast<FloatLit>()) {
          pConst=pF;
          fFloat=true;
          isUB = (1==j);
        } else
          if (auto pF=pCall->arg(j)->dyn_cast<IntLit>()) {
            pConst=pF;
            fFloat=false;
            isUB = (1==j);
          } else
            if (auto pId=pCall->arg(j)->dyn_cast<Id>()) {
              if (nullptr!=pVar)
                return false;                   // 2 variables, exit
              pVar = pId;
            }
      }
      /// All good, add them
      if (fFloat) {
        isUBF.push_back( constants().boollit(isUB) );
        bndF.push_back(pConst);
        varF.push_back(pVar);
      } else {
        isUBI.push_back( constants().boollit(isUB) );
        bndI.push_back(pConst);
        varI.push_back(pVar);
      }
    }
    /// Create new call
    GCLock lock;
    auto loc = c_orig->loc().introduce();
    std::vector<Expression*> args =
    {
      new ArrayLit(loc, isUBI),
      new ArrayLit(loc, bndI),
      new ArrayLit(loc, varI),
      new ArrayLit(loc, isUBF),
      new ArrayLit(loc, bndF),
      new ArrayLit(loc, varF)
    };

    Call* c = new Call(c_orig->loc().introduce(),
                       env.model->getFnDecls().bounds_disj.second->id(),
                       args);
    c->type(Type::varbool());
    c->decl(env.model->getFnDecls().bounds_disj.second);
    env.flat_addItem(new ConstraintI(c_orig->loc().introduce(), c));
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
    ~IgnorePartial(void) {
      env.ignorePartial = ignorePartial;
    }
  };
  
  EE flatten_call(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    EE ret;
    Call* c = e->cast<Call>();
    IgnorePartial ignorePartial(env,c);
    if (c->id().endsWith("_reif")) {
      env.n_reif_ct++;
    } else if (c->id().endsWith("_imp")) {
      env.n_imp_ct++;
    }
    FunctionI* decl = env.model->matchFn(env,c,false);
    if (decl == NULL) {
      throw InternalError("undeclared function or predicate "
                          +c->id().str());
    }
    
    Ctx nctx = ctx;
    nctx.neg = false;
    ASTString cid = c->id();
    CallStackItem _csi(env,e);
    
    if (cid == constants().ids.bool2int && c->type().dim()==0) {
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
    } else if (decl->e()==NULL && (cid == constants().ids.assert || cid == constants().ids.trace || cid == constants().ids.mzn_deprecate)) {
      if (cid == constants().ids.assert && c->n_args()==2) {
        (void) decl->_builtins.b(env,c);
        ret = flat_exp(env,ctx,constants().lit_true,r,b);
      } else {
        KeepAlive callres = decl->_builtins.e(env,c);
        ret = flat_exp(env,ctx,callres(),r,b);
        // This is all we need to do for assert, so break out of the E_CALL
      }
      return ret;
    } else if (decl->e() && ctx.b==C_ROOT && decl->e()->isa<BoolLit>() && eval_bool(env,decl->e())) {
      bool allBool = true;
      for (unsigned int i=0; i<c->n_args(); i++) {
        if (c->arg(i)->type().bt()!=Type::BT_BOOL) {
          allBool = false;
          break;
        }
      }
      if (allBool) {
        ret.r = bind(env,ctx,r,constants().lit_true);
        ret.b = bind(env,ctx,b,constants().lit_true);
        return ret;
      }
    }
    
    if (ctx.b==C_ROOT && decl->e()==NULL &&
        cid == constants().ids.forall && r==constants().var_true) {
      ret.b = bind(env,ctx,b,constants().lit_true);
      ArrayLit* al;
      if (c->arg(0)->isa<ArrayLit>()) {
        al = c->arg(0)->cast<ArrayLit>();
      } else {
        EE flat_al = flat_exp(env,Ctx(),c->arg(0),constants().var_ignore,constants().var_true);
        al = follow_id(flat_al.r())->cast<ArrayLit>();
      }
      nctx.b = C_ROOT;
      for (unsigned int i=0; i<al->size(); i++)
        (void) flat_exp(env,nctx,(*al)[i],r,b);
      ret.r = bind(env,ctx,r,constants().lit_true);
    } else {
      if (decl->e() && decl->params().size()==1 && decl->e()->isa<Id>() &&
          decl->params()[0]->ti()->domain()==NULL &&
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
      
      std::vector<EE> args_ee(c->n_args());
      bool isPartial = false;
      
      if (cid == constants().ids.lin_exp && c->type().isint()) {
        // Linear expressions need special context handling:
        // the context of a variable expression depends on the corresponding coefficient
        
        // flatten the coefficient array
        Expression* tmp = follow_id_to_decl(c->arg(0));
        ArrayLit* coeffs;
        if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
          tmp = vd->id();
        {
          CallArgItem cai(env);
          args_ee[0] = flat_exp(env,nctx,tmp,NULL,NULL);
          isPartial |= isfalse(env, args_ee[0].b());
          coeffs = eval_array_lit(env, args_ee[0].r());
        }
        
        ArrayLit* vars = eval_array_lit(env, c->arg(1));
        if (vars->flat()) {
          args_ee[1].r = vars;
          args_ee[1].b = constants().var_true;
        } else {
          CallArgItem cai(env);
          CallStackItem _csi(env,c->arg(1));
          std::vector<EE> elems_ee(vars->size());
          for (unsigned int i=vars->size(); i--;) {
            Ctx argctx = nctx;
            argctx.i = eval_int(env,(*coeffs)[i])<0 ? -nctx.i : +nctx.i;
            elems_ee[i] = flat_exp(env,argctx,(*vars)[i],NULL,NULL);
          }
          std::vector<Expression*> elems(elems_ee.size());
          for (unsigned int i=static_cast<unsigned int>(elems.size()); i--;)
            elems[i] = elems_ee[i].r();
          KeepAlive ka;
          {
            GCLock lock;
            ArrayLit* alr = new ArrayLit(Location().introduce(),elems);
            alr->type(vars->type());
            alr->flat(true);
            ka = alr;
          }
          args_ee[1].r = ka();
          args_ee[1].b = conj(env,b,Ctx(),elems_ee);
        }
        
        {
          Expression* constant = follow_id_to_decl(c->arg(2));
          if (VarDecl* vd = constant->dyn_cast<VarDecl>())
            constant = vd->id();
          CallArgItem cai(env);
          args_ee[2] = flat_exp(env,nctx,constant,NULL,NULL);
          isPartial |= isfalse(env, args_ee[2].b());
        }
        
      } else {
        bool mixContext =
        (cid != constants().ids.forall && cid != constants().ids.exists &&
         (cid != constants().ids.bool2int || c->type().dim()>0) &&
         cid != constants().ids.sum && cid != "assert" &&
         cid != constants().var_redef->id() &&
         cid != "mzn_reverse_map_var");
        if (cid == "mzn_reverse_map_var") {
          env.in_reverse_map_var = true;
        }
        if (cid == constants().ids.clause && c->arg(0)->isa<ArrayLit>() && c->arg(1)->isa<ArrayLit>()) {
          Ctx argctx = nctx;
          
          // handle negated args first, try to make them positive
          
          if (mixContext) {
            argctx.b = -nctx.b;
          }
          std::vector<KeepAlive> neg_args;
          std::vector<KeepAlive> pos_args;
          std::vector<KeepAlive> newPositives;
          bool is_subsumed = false;
          ArrayLit* al_neg = c->arg(1)->cast<ArrayLit>();
          {
            CallArgItem cai(env);
            for (unsigned int i=0; i<al_neg->size(); i++) {
              BinOp* bo = (*al_neg)[i]->dyn_cast<BinOp>();
              Call* co = (*al_neg)[i]->dyn_cast<Call>();
              if (bo || (co && (co->id()==constants().ids.forall || co->id()==constants().ids.exists || co->id()==constants().ids.clause))) {
                GCLock lock;
                UnOp* notBoe0 = new UnOp(Location().introduce(), UOT_NOT, (*al_neg)[i]);
                notBoe0->type(Type::varbool());
                newPositives.push_back(notBoe0);
              } else {
                EE res = flat_exp(env,argctx,(*al_neg)[i],NULL,constants().var_true);
                if (res.r()->type().ispar()) {
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
                  neg_args.push_back(res.r());
                }
              }
            }
          }

          // Now process new and previous positive arguments
          if (mixContext) {
            argctx.b = +nctx.b;
          }
          ArrayLit* al_pos = c->arg(0)->cast<ArrayLit>();
          for (unsigned int i=0; i<al_pos->size(); i++) {
            newPositives.push_back((*al_pos)[i]);
          }
          {
            CallArgItem cai(env);
            for (unsigned int i=0; i<newPositives.size(); i++) {
              EE res = flat_exp(env,argctx,newPositives[i](),NULL,constants().var_true);
              if (res.r()->type().ispar()) {
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
                pos_args.push_back(res.r());
              }
            }
          }
          
          GCLock lock;
          ArrayLit* al_new_pos = new ArrayLit(al_pos->loc(), toExpVec(pos_args));
          al_new_pos->type(Type::varbool(1));
          al_new_pos->flat(true);
          args_ee[0] = EE(al_new_pos, constants().lit_true);
          ArrayLit* al_new_neg = new ArrayLit(al_neg->loc(), toExpVec(neg_args));
          al_new_neg->flat(true);
          al_new_neg->type(Type::varbool(1));
          args_ee[1] = EE(al_new_neg, constants().lit_true);
        } else if ( (cid == constants().ids.forall || cid == constants().ids.exists)
              && c->arg(0)->isa<ArrayLit>() ) {
          bool is_conj = (cid == constants().ids.forall);
          Ctx argctx = nctx;
          if (mixContext) {
            argctx.b = C_MIX;
          }
          ArrayLit* al = c->arg(0)->cast<ArrayLit>();
          ArrayLit* al_new;
          if (al->flat()) {
            al_new = al;
          } else {
            std::vector<KeepAlive> flat_args;
            CallArgItem cai(env);
            for (unsigned int i=0; i<al->size(); i++) {
              EE res = flat_exp(env,argctx,(*al)[i],NULL,constants().var_true);
              if (res.r()->type().ispar()) {
                if (eval_bool(env, res.r())==is_conj) {
                  // this element is irrelevant
                } else {
                  // this element subsumes all other elements
                  flat_args = {res.r()};
                  break;
                }
              } else {
                flat_args.push_back(res.r());
              }
            }
            GCLock lock;
            al_new = new ArrayLit(al->loc(), toExpVec(flat_args));
            al_new->type(Type::varbool(1));
            al_new->flat(true);
          }
          args_ee[0] = EE(al_new, constants().lit_true);
        } else {
          for (unsigned int i=c->n_args(); i--;) {
            Ctx argctx = nctx;
            if (mixContext) {
              if (cid==constants().ids.clause) {
                argctx.b = (i==0 ? +nctx.b : -nctx.b);
              } else if (c->arg(i)->type().bt()==Type::BT_BOOL) {
                argctx.b = C_MIX;
              } else if (c->arg(i)->type().bt()==Type::BT_INT) {
                argctx.i = C_MIX;
              }
            } else if (cid == constants().ids.sum && c->arg(i)->type().bt()==Type::BT_BOOL) {
              argctx.b = argctx.i;
            }
            Expression* tmp = follow_id_to_decl(c->arg(i));
            if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
              tmp = vd->id();
            CallArgItem cai(env);
            args_ee[i] = flat_exp(env,argctx,tmp,NULL,NULL);
            isPartial |= isfalse(env, args_ee[i].b());
          }
        }
      }
      if (isPartial && c->type().isbool() && !c->type().isopt()) {
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        args_ee.resize(1);
        args_ee[0] = EE(NULL, constants().lit_false);
        ret.r = conj(env, r, ctx, args_ee);
        return ret;
      }
      
      std::vector<KeepAlive> args;
      if (decl->e()==NULL && (cid == constants().ids.exists || cid == constants().ids.clause)) {
        std::vector<KeepAlive> pos_alv;
        std::vector<KeepAlive> neg_alv;
        
        std::vector<Expression*> pos_stack;
        std::vector<Expression*> neg_stack;
        
        ArrayLit* al_pos = follow_id(args_ee[0].r())->cast<ArrayLit>();
        for (unsigned int i=0; i<al_pos->size(); i++) {
          pos_stack.push_back((*al_pos)[i]);
        }
        if (cid == constants().ids.clause) {
          ArrayLit* al_neg = follow_id(args_ee[1].r())->cast<ArrayLit>();
          for (unsigned int i=0; i<al_neg->size(); i++) {
            neg_stack.push_back((*al_neg)[i]);
          }
        }
        
        std::unordered_set<Expression*> seen;
        
        while (!pos_stack.empty() || !neg_stack.empty()) {
          
          while (!pos_stack.empty()) {
            Expression* cur = pos_stack.back();
            pos_stack.pop_back();
            if (cur->isa<Id>() && seen.find(cur) != seen.end()) {
              pos_alv.push_back(cur);
            } else {
              seen.insert(cur);
              GCLock lock;
              if (Call* sc = Expression::dyn_cast<Call>(same_call(env,cur,constants().ids.exists))) {
                GCLock lock;
                ArrayLit* sc_c = eval_array_lit(env,sc->arg(0));
                for (unsigned int j=0; j<sc_c->size(); j++) {
                  pos_stack.push_back((*sc_c)[j]);
                }
              } else if (Call* sc = Expression::dyn_cast<Call>(same_call(env,cur,constants().ids.clause))) {
                GCLock lock;
                ArrayLit* sc_c = eval_array_lit(env,sc->arg(0));
                for (unsigned int j=0; j<sc_c->size(); j++) {
                  pos_stack.push_back((*sc_c)[j]);
                }
                sc_c = eval_array_lit(env,sc->arg(1));
                for (unsigned int j=0; j<sc_c->size(); j++) {
                  neg_stack.push_back((*sc_c)[j]);
                }
              } else {
                Call* eq_call = Expression::dyn_cast<Call>(same_call(env,cur,constants().ids.bool_eq));
                if (eq_call && Expression::equal(eq_call->arg(1),constants().lit_false)) {
                  neg_stack.push_back(eq_call->arg(0));
                } else if (eq_call && Expression::equal(eq_call->arg(0),constants().lit_false)) {
                  neg_stack.push_back(eq_call->arg(1));
                } else if (eq_call && Expression::equal(eq_call->arg(1),constants().lit_true)) {
                  pos_stack.push_back(eq_call->arg(0));
                } else if (eq_call && Expression::equal(eq_call->arg(0),constants().lit_true)) {
                  pos_stack.push_back(eq_call->arg(1));
                } else if (Id* ident = cur->dyn_cast<Id>()) {
                  if (ident->decl()->ti()->domain()!=constants().lit_false) {
                    pos_alv.push_back(ident);
                  }
                } else {
                  pos_alv.push_back(cur);
                }
              }
            }
          }
          
          while (!neg_stack.empty()) {
            GCLock lock;
            Expression* cur = neg_stack.back();
            neg_stack.pop_back();
            if (cur->isa<Id>() && seen.find(cur) != seen.end()) {
              neg_alv.push_back(cur);
            } else {
              seen.insert(cur);
              if (Call* sc = Expression::dyn_cast<Call>(same_call(env,cur,constants().ids.forall))) {
                GCLock lock;
                ArrayLit* sc_c = eval_array_lit(env,sc->arg(0));
                for (unsigned int j=0; j<sc_c->size(); j++) {
                  neg_stack.push_back((*sc_c)[j]);
                }
              } else {
                Call* eq_call = Expression::dyn_cast<Call>(same_call(env,cur,constants().ids.bool_eq));
                if (eq_call && Expression::equal(eq_call->arg(1),constants().lit_false)) {
                  pos_stack.push_back(eq_call->arg(0));
                } else if (eq_call && Expression::equal(eq_call->arg(0),constants().lit_false)) {
                  pos_stack.push_back(eq_call->arg(1));
                } else if (eq_call && Expression::equal(eq_call->arg(1),constants().lit_true)) {
                  neg_stack.push_back(eq_call->arg(0));
                } else if (eq_call && Expression::equal(eq_call->arg(0),constants().lit_true)) {
                  neg_stack.push_back(eq_call->arg(1));
                } else if (Id* ident = cur->dyn_cast<Id>()) {
                  if (ident->decl()->ti()->domain()!=constants().lit_true) {
                    neg_alv.push_back(ident);
                  }
                } else {
                  neg_alv.push_back(cur);
                }
              }
            }
          }
          
        }
        
        bool subsumed = remove_dups(pos_alv,false);
        subsumed = subsumed || remove_dups(neg_alv,true);
        subsumed = subsumed || contains_dups(pos_alv, neg_alv);
        if (subsumed) {
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          ret.r = bind(env,ctx,r,constants().lit_true);
          return ret;
        }
        if (neg_alv.empty()) {
          if (pos_alv.size()==0) {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            ret.r = bind(env,ctx,r,constants().lit_false);
            return ret;
          } else if (pos_alv.size()==1) {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            ret.r = bind(env,ctx,r,pos_alv[0]());
            return ret;
          }
          GCLock lock;
          ArrayLit* nal = new ArrayLit(Location().introduce(),toExpVec(pos_alv));
          nal->type(Type::varbool(1));
          args.push_back(nal);
          cid = constants().ids.exists;
        } else {
          GCLock lock;
          ArrayLit* pos_al = new ArrayLit(Location().introduce(),toExpVec(pos_alv));
          pos_al->type(Type::varbool(1));
          ArrayLit* neg_al = new ArrayLit(Location().introduce(),toExpVec(neg_alv));
          neg_al->type(Type::varbool(1));
          cid = constants().ids.clause;
          args.push_back(pos_al);
          args.push_back(neg_al);
        }
        if (C_ROOT==ctx.b && cid == constants().ids.exists) {
          /// Check the special bounds disjunction for SCIP
          /// Only in root context
          if (!env.model->getFnDecls().bounds_disj.first) {
            env.model->getFnDecls().bounds_disj.first = true;
            std::vector<Type> bj_t =
            { Type::parbool(1), Type::parint(1), Type::varint(1),
              Type::parbool(1), Type::parfloat(1), Type::varfloat(1) };
            GCLock lock;
            env.model->getFnDecls().bounds_disj.second =
                env.model->matchFn(env, ASTString("bounds_disj"), bj_t, false);
          }
          /// When the SCIP predicate is declared only
          bool fBoundsDisj_Maybe =
              ( nullptr != env.model->getFnDecls().bounds_disj.second );
          if (fBoundsDisj_Maybe) {
            if (addBoundsDisj(env, args[0](), c)) {
              ret.b = bind(env,Ctx(),b,constants().lit_true);
              ret.r = bind(env,ctx,r,constants().lit_true);
              return ret;
            }
          }
        }

      } else if (decl->e()==NULL && cid == constants().ids.forall) {
        ArrayLit* al = follow_id(args_ee[0].r())->cast<ArrayLit>();
        std::vector<KeepAlive> alv;
        for (unsigned int i=0; i<al->size(); i++) {
          GCLock lock;
          if (Call* sc = Expression::dyn_cast<Call>(same_call(env,(*al)[i],cid))) {
            GCLock lock;
            ArrayLit* sc_c = eval_array_lit(env,sc->arg(0));
            for (unsigned int j=0; j<sc_c->size(); j++) {
              alv.push_back((*sc_c)[j]);
            }
          } else {
            alv.push_back((*al)[i]);
          }
        }
        bool subsumed = remove_dups(alv,true);
        if (subsumed) {
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          ret.r = bind(env,ctx,r,constants().lit_false);
          return ret;
        }
        if (alv.size()==0) {
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          ret.r = bind(env,ctx,r,constants().lit_true);
          return ret;
        } else if (alv.size()==1) {
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          ret.r = bind(env,ctx,r,alv[0]());
          return ret;
        }
        GCLock lock;
        ArrayLit* nal = new ArrayLit(al->loc(),toExpVec(alv));
        nal->type(al->type());
        args.push_back(nal);
      } else if (decl->e()==NULL && (cid == constants().ids.lin_exp || cid==constants().ids.sum)) {
        if (e->type().isint()) {
          flatten_linexp_call<IntLit>(env,ctx,nctx,cid,c,ret,b,r,args_ee,args);
        } else {
          flatten_linexp_call<FloatLit>(env,ctx,nctx,cid,c,ret,b,r,args_ee,args);
        }
        if (args.size()==0)
          return ret;
      } else {
        for (unsigned int i=0; i<args_ee.size(); i++)
          args.push_back(args_ee[i].r());
      }
      bool hadImplementation = (decl->e() != nullptr);
      KeepAlive cr;
      {
        GCLock lock;
        std::vector<Expression*> e_args = toExpVec(args);
        Call* cr_c = new Call(c->loc().introduce(),cid,e_args);
        decl = env.model->matchFn(env,cr_c,false);
        if (decl==NULL)
          throw FlatteningError(env,cr_c->loc(), "cannot find matching declaration");
        cr_c->type(decl->rtype(env,e_args,false));
        assert(decl);
        cr_c->decl(decl);
        cr = cr_c;
      }
      if (hadImplementation && decl->e()==NULL && (cid == constants().ids.lin_exp || cid==constants().ids.sum)) {
        args.clear();
        if (e->type().isint()) {
          flatten_linexp_call<IntLit>(env,ctx,nctx,cid,cr()->cast<Call>(),ret,b,r,args_ee,args);
        } else {
          flatten_linexp_call<FloatLit>(env,ctx,nctx,cid,cr()->cast<Call>(),ret,b,r,args_ee,args);
        }
        if (args.size()==0)
          return ret;
        GCLock lock;
        std::vector<Expression*> e_args = toExpVec(args);
        Call* cr_c = new Call(c->loc().introduce(),cid,e_args);
        decl = env.model->matchFn(env,cr_c,false);
        if (decl==NULL)
          throw FlatteningError(env,cr_c->loc(), "cannot find matching declaration");
        cr_c->type(decl->rtype(env,e_args,false));
        assert(decl);
        cr_c->decl(decl);
        cr = cr_c;
      }
      EnvI::CSEMap::iterator cit = env.cse_map_find(cr());
      if (cit != env.cse_map_end()) {
        ret.b = bind(env,Ctx(),b,env.ignorePartial ? constants().lit_true : cit->second.b());
        ret.r = bind(env,ctx,r,cit->second.r());
      } else {
        for (unsigned int i=0; i<decl->params().size(); i++) {
          if (decl->params()[i]->type().dim() > 0) {
            // Check array index sets
            ArrayLit* al = follow_id(args[i]())->cast<ArrayLit>();
            VarDecl* pi = decl->params()[i];
            for (unsigned int j=0; j<pi->ti()->ranges().size(); j++) {
              TypeInst* range_ti = pi->ti()->ranges()[j];
              if (range_ti->domain() && !range_ti->domain()->isa<TIId>()) {
                GCLock lock;
                IntSetVal* isv = eval_intset(env, range_ti->domain());
                if (isv->min() != al->min(j) || isv->max() != al->max(j)) {
                  std::ostringstream oss;
                  oss << "array index set " << (j+1) << " of argument " << (i+1) << " does not match declared index set";
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
                IntSetVal* isv = eval_intset(env,dom);
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
                    IntBounds ib = compute_int_bounds(env,args[i]());
                    needToConstrain = !ib.valid || isv->size()==0 || ib.l < isv->min(0) || ib.u > isv->max(isv->size()-1);
                  }
                }
                if (needToConstrain) {
                  GCLock lock;
                  Expression* domconstraint;
                  if (args[i]()->type().dim() > 0) {
                    std::vector<Expression*> domargs(2);
                    domargs[0] = args[i]();
                    domargs[1] = dom;
                    Call* c = new Call(Location().introduce(),"var_dom",domargs);
                    c->type(Type::varbool());
                    c->decl(env.model->matchFn(env,c,false));
                    if (c->decl()==NULL)
                      throw InternalError("no matching declaration found for var_dom");
                    domconstraint = c;
                  } else {
                    domconstraint = new BinOp(Location().introduce(),args[i](),bot,dom);
                  }
                  domconstraint->type(args[i]()->type().ispar() ? Type::parbool() : Type::varbool());
                  if (ctx.b == C_ROOT) {
                    (void) flat_exp(env, Ctx(), domconstraint, constants().var_true, constants().var_true);
                  } else {
                    EE ee = flat_exp(env, Ctx(), domconstraint, NULL, constants().var_true);
                    ee.b = ee.r;
                    args_ee.push_back(ee);
                  }
                }
              } else if (args[i]()->type().bt() == Type::BT_FLOAT) {
                GCLock lock;
                
                FloatSetVal* fsv = eval_floatset(env,dom);
                bool needToConstrain;
                if (args[i]()->type().dim() > 0) {
                  needToConstrain = true;
                } else {
                  FloatBounds fb = compute_float_bounds(env,args[i]());
                  needToConstrain = !fb.valid || fsv->size()==0 || fb.l < fsv->min(0) || fb.u > fsv->max(fsv->size()-1);
                }
                
                if (needToConstrain) {
                  GCLock lock;
                  Expression* domconstraint;
                  if (args[i]()->type().dim() > 0) {
                    std::vector<Expression*> domargs(2);
                    domargs[0] = args[i]();
                    domargs[1] = dom;
                    Call* c = new Call(Location().introduce(),"var_dom",domargs);
                    c->type(Type::varbool());
                    c->decl(env.model->matchFn(env,c,false));
                    if (c->decl()==NULL)
                      throw InternalError("no matching declaration found for var_dom");
                    domconstraint = c;
                  } else {
                    domconstraint = new BinOp(Location().introduce(),args[i](),BOT_IN,dom);
                  }
                  domconstraint->type(args[i]()->type().ispar() ? Type::parbool() : Type::varbool());
                  if (ctx.b == C_ROOT) {
                    (void) flat_exp(env, Ctx(), domconstraint, constants().var_true, constants().var_true);
                  } else {
                    EE ee = flat_exp(env, Ctx(), domconstraint, NULL, constants().var_true);
                    ee.b = ee.r;
                    args_ee.push_back(ee);
                  }
                }
              } else if (args[i]()->type().bt() == Type::BT_BOT) {
                // Nothing to be done for empty arrays/sets
              } else {
                throw EvalError(env,decl->params()[i]->loc(),"domain restrictions other than int and float not supported yet");
              }
            }
          }
        }
        if (cr()->type().isbool() &&  !cr()->type().ispar() && !cr()->type().isopt() && (ctx.b != C_ROOT || r != constants().var_true)) {
          std::vector<Type> argtypes(args.size());
          for (unsigned int i=0; i<args.size(); i++)
            argtypes[i] = args[i]()->type();
          argtypes.push_back(Type::varbool());
          GCLock lock;
          ASTString r_cid = env.reifyId(cid);
          FunctionI* reif_decl = env.model->matchFn(env, r_cid, argtypes, false);
          if (reif_decl && reif_decl->e()) {
            addPathAnnotation(env, reif_decl->e());
            VarDecl* reif_b;
            if (r==NULL || (r != NULL && r->e() != NULL)) {
              reif_b = newVarDecl(env, Ctx(), new TypeInst(Location().introduce(),Type::varbool()), NULL, NULL, NULL);
              addCtxAnn(reif_b, ctx.b);
              if (reif_b->ti()->domain()) {
                if (reif_b->ti()->domain() == constants().lit_true) {
                  bind(env,ctx,r,constants().lit_true);
                  r = constants().var_true;
                  ctx.b = C_ROOT;
                  goto call_nonreif;
                } else {
                  std::vector<Expression*> args_e(args.size()+1);
                  for (unsigned int i=0; i<args.size(); i++)
                    args_e[i] = args[i]();
                  args_e[args.size()] = constants().lit_false;
                  Call* reif_call = new Call(Location().introduce(), r_cid, args_e);
                  reif_call->type(Type::varbool());
                  reif_call->decl(reif_decl);
                  flat_exp(env, Ctx(), reif_call, constants().var_true, constants().var_true);
                  args_ee.push_back(EE(NULL,constants().lit_false));
                  ret.r = conj(env,r,ctx,args_ee);
                  ret.b = bind(env,ctx,b,constants().lit_true);
                  return ret;
                }
              }
            } else {
              reif_b = r;
            }
            // Annotate cr() with getPath()
            addPathAnnotation(env, cr());
            reif_b->e(cr());
            if (r != NULL && r->e() != NULL) {
              Ctx reif_ctx;
              reif_ctx.neg = ctx.neg;
              bind(env,reif_ctx,r,reif_b->id());
            }
            env.vo_add_exp(reif_b);
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            args_ee.push_back(EE(NULL,reif_b->id()));
            ret.r = conj(env,NULL,ctx,args_ee);
            if (!ctx.neg && !cr()->type().isann()) {
              env.cse_map_insert(cr(),ret);
            }
            return ret;
          }
        }
      call_nonreif:
        if ( (cr()->type().ispar() && !cr()->type().isann()) || decl->e()==NULL) {
          Call* cr_c = cr()->cast<Call>();
          /// All builtins are total
          std::vector<Type> argt(cr_c->n_args());
          for (unsigned int i=static_cast<unsigned int>(argt.size()); i--;)
            argt[i] = cr_c->arg(i)->type();
          Type callt = decl->rtype(env,argt,false);
          if (callt.ispar() && callt.bt()!=Type::BT_ANN) {
            GCLock lock;
            try {
              ret.r = bind(env,ctx,r,eval_par(env,cr_c));
              ret.b = conj(env,b,Ctx(),args_ee);
            } catch (ResultUndefinedError&) {
              ret.r = createDummyValue(env, cr_c->type());
              ret.b = bind(env,Ctx(),b,constants().lit_false);
              return ret;
            }
            // Do not insert into map, since par results will quickly become
            // garbage anyway and then disappear from the map
          } else if (decl->_builtins.e) {
            KeepAlive callres;
            {
              GCLock lock;
              callres = decl->_builtins.e(env,cr_c);
            }
            EE res = flat_exp(env,ctx,callres(),r,b);
            args_ee.push_back(res);
            ret.b = conj(env,b,Ctx(),args_ee);
            addPathAnnotation(env, res.r());
            ret.r = bind(env,ctx,r,res.r());
            if (!ctx.neg && !cr_c->type().isann())
              env.cse_map_insert(cr_c,ret);
          } else {
            ret.b = conj(env,b,Ctx(),args_ee);
            addPathAnnotation(env, cr_c);
            ret.r = bind(env,ctx,r,cr_c);
            if (!ctx.neg && !cr_c->type().isann())
              env.cse_map_insert(cr_c,ret);
          }
        } else {
          std::vector<KeepAlive> previousParameters(decl->params().size());
          for (unsigned int i=decl->params().size(); i--;) {
            VarDecl* vd = decl->params()[i];
            previousParameters[i] = vd->e();
            vd->flat(vd);
            vd->e(args[i]());
          }
          
          if (decl->e()->type().isbool() && !decl->e()->type().isopt()) {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            if (ctx.b==C_ROOT && r==constants().var_true) {
              (void) flat_exp(env,Ctx(),decl->e(),r,constants().var_true);
            } else {
              Ctx nctx;
              if (!isTotal(decl)) {
                nctx = ctx;
                nctx.neg = false;
              }
              EE ee = flat_exp(env,nctx,decl->e(),NULL,constants().var_true);
              ee.b = ee.r;
              args_ee.push_back(ee);
            }
            ret.r = conj(env,r,ctx,args_ee);
          } else {
            if (isTotal(decl)) {
              EE ee = flat_exp(env,Ctx(),decl->e(),r,constants().var_true);
              ret.r = bind(env,ctx,r,ee.r());
            } else {
              ret = flat_exp(env,ctx,decl->e(),r,NULL);
              args_ee.push_back(ret);
              if (decl->e()->type().dim() > 0) {
                ArrayLit* al = follow_id(ret.r())->cast<ArrayLit>();
                assert(al->dims() == decl->e()->type().dim());
                for (unsigned int i=0; i<decl->ti()->ranges().size(); i++) {
                  if (decl->ti()->ranges()[i]->domain() &&
                      !decl->ti()->ranges()[i]->domain()->isa<TIId>()) {
                    GCLock lock;
                    IntSetVal* isv = eval_intset(env, decl->ti()->ranges()[i]->domain());
                    if (al->min(i) != isv->min() || al->max(i) != isv->max()) {
                      EE ee;
                      ee.b = constants().lit_false;
                      args_ee.push_back(ee);
                    }
                  }
                }
              }
              if (decl->ti()->domain() && !decl->ti()->domain()->isa<TIId>()) {
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
                  Call* c = new Call(Location().introduce(),"var_dom",domargs);
                  c->type(Type::varbool());
                  c->decl(env.model->matchFn(env,c,false));
                  if (c->decl()==NULL)
                    throw InternalError("no matching declaration found for var_dom");
                  domconstraint = c;
                } else {
                  GCLock lock;
                  domconstraint = new BinOp(Location().introduce(),ret.r(),bot,decl->ti()->domain());
                }
                domconstraint()->type(ret.r()->type().ispar() ? Type::parbool() : Type::varbool());
                if (ctx.b == C_ROOT) {
                  (void) flat_exp(env, Ctx(), domconstraint(), constants().var_true, constants().var_true);
                } else {
                  EE ee = flat_exp(env, Ctx(), domconstraint(), NULL, constants().var_true);
                  ee.b = ee.r;
                  args_ee.push_back(ee);
                }
              }
            }
            ret.b = conj(env,b,Ctx(),args_ee);
          }
          if (!ctx.neg && !cr()->type().isann())
            env.cse_map_insert(cr(),ret);
          
          // Restore previous mapping
          for (unsigned int i=decl->params().size(); i--;) {
            VarDecl* vd = decl->params()[i];
            vd->e(previousParameters[i]());
            vd->flat(vd->e() ? vd : NULL);
          }
        }
      }
    }
    if (cid == "mzn_reverse_map_var") {
      env.in_reverse_map_var = false;
    }
    return ret;
  }
}
