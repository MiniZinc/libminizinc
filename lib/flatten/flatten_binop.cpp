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

  ASTString opToBuiltin(BinOp* op, BinOpType bot) {
    std::string builtin;
    if (op->rhs()->type().isint()) {
      switch (bot) {
        case BOT_PLUS: return constants().ids.int_.plus;
        case BOT_MINUS: return constants().ids.int_.minus;
        case BOT_MULT: return constants().ids.int_.times;
        case BOT_POW: return constants().ids.pow;
        case BOT_IDIV: return constants().ids.int_.div;
        case BOT_MOD: return constants().ids.int_.mod;
        case BOT_LE: return constants().ids.int_.lt;
        case BOT_LQ: return constants().ids.int_.le;
        case BOT_GR: return constants().ids.int_.gt;
        case BOT_GQ: return constants().ids.int_.ge;
        case BOT_EQ: return constants().ids.int_.eq;
        case BOT_NQ: return constants().ids.int_.ne;
        default:
          throw InternalError("not yet implemented");
      }
    } else if (op->rhs()->type().isbool()) {
      if (bot==BOT_EQ || bot==BOT_EQUIV)
        return constants().ids.bool_eq;
      builtin = "bool_";
    } else if (op->rhs()->type().is_set()) {
      builtin = "set_";
    } else if (op->rhs()->type().isfloat()) {
      switch (bot) {
        case BOT_PLUS: return constants().ids.float_.plus;
        case BOT_MINUS: return constants().ids.float_.minus;
        case BOT_MULT: return constants().ids.float_.times;
        case BOT_POW: return constants().ids.pow;
        case BOT_DIV: return constants().ids.float_.div;
        case BOT_MOD: return constants().ids.float_.mod;
        case BOT_LE: return constants().ids.float_.lt;
        case BOT_LQ: return constants().ids.float_.le;
        case BOT_GR: return constants().ids.float_.gt;
        case BOT_GQ: return constants().ids.float_.ge;
        case BOT_EQ: return constants().ids.float_.eq;
        case BOT_NQ: return constants().ids.float_.ne;
        default:
          throw InternalError("not yet implemented");
      }
    } else if (op->rhs()->type().isopt() &&
               (bot==BOT_EQUIV || bot==BOT_EQ)) {
      /// TODO: extend to all option type operators
      switch (op->lhs()->type().bt()) {
        case Type::BT_BOOL: return constants().ids.bool_eq;
        case Type::BT_FLOAT: return constants().ids.float_.eq;
        case Type::BT_INT:
          if (op->lhs()->type().st()==Type::ST_PLAIN)
            return constants().ids.int_.eq;
          else
            return constants().ids.set_eq;
        default:
          throw InternalError("not yet implemented");
      }
      
    } else {
      throw InternalError(op->opToString().str()+" not yet implemented");
    }
    switch (bot) {
      case BOT_PLUS:
        return builtin+"plus";
      case BOT_MINUS:
        return builtin+"minus";
      case BOT_MULT:
        return builtin+"times";
      case BOT_DIV:
        return builtin+"div";
      case BOT_IDIV:
        return builtin+"div";
      case BOT_MOD:
        return builtin+"mod";
      case BOT_LE:
        return builtin+"lt";
      case BOT_LQ:
        return builtin+"le";
      case BOT_GR:
        return builtin+"gt";
      case BOT_GQ:
        return builtin+"ge";
      case BOT_EQ:
        return builtin+"eq";
      case BOT_NQ:
        return builtin+"ne";
      case BOT_IN:
        return constants().ids.set_in;
      case BOT_SUBSET:
        return builtin+"subset";
      case BOT_SUPERSET:
        return builtin+"superset";
      case BOT_UNION:
        return builtin+"union";
      case BOT_DIFF:
        return builtin+"diff";
      case BOT_SYMDIFF:
        return builtin+"symdiff";
      case BOT_INTERSECT:
        return builtin+"intersect";
      case BOT_PLUSPLUS:
      case BOT_DOTDOT:
        throw InternalError("not yet implemented");
      case BOT_EQUIV:
        return builtin+"eq";
      case BOT_IMPL:
        return builtin+"le";
      case BOT_RIMPL:
        return builtin+"ge";
      case BOT_OR:
        return builtin+"or";
      case BOT_AND:
        return builtin+"and";
      case BOT_XOR:
        return constants().ids.bool_xor;
      default:
        assert(false); return ASTString("");
    }
  }
  
  bool isReverseMap(BinOp* e) {
    return e->ann().contains(constants().ann.is_reverse_map);
  }

  template<class Lit>
  void collectLinExps(EnvI& env,
                      typename LinearTraits<Lit>::Val c, Expression* exp,
                      std::vector<typename LinearTraits<Lit>::Val>& coeffs,
                      std::vector<KeepAlive>& vars,
                      typename LinearTraits<Lit>::Val& constval) {
    typedef typename LinearTraits<Lit>::Val Val;
    struct StackItem {
      Expression* e;
      Val c;
      StackItem(Expression* e0, Val c0) : e(e0), c(c0) {}
    };
    std::vector<StackItem> stack;
    stack.push_back(StackItem(exp,c));
    while (!stack.empty()) {
      Expression* e = stack.back().e;
      Val c = stack.back().c;
      stack.pop_back();
      if (e==NULL)
        continue;
      if (e->type().ispar()) {
        constval += c * LinearTraits<Lit>::eval(env,e);
      } else if (Lit* l = e->dyn_cast<Lit>()) {
        constval += c * l->v();
      } else if (BinOp* bo = e->dyn_cast<BinOp>()) {
        switch (bo->op()) {
          case BOT_PLUS:
            stack.push_back(StackItem(bo->lhs(),c));
            stack.push_back(StackItem(bo->rhs(),c));
            break;
          case BOT_MINUS:
            stack.push_back(StackItem(bo->lhs(),c));
            stack.push_back(StackItem(bo->rhs(),-c));
            break;
          case BOT_MULT:
            if (bo->lhs()->type().ispar()) {
              stack.push_back(StackItem(bo->rhs(),c*LinearTraits<Lit>::eval(env,bo->lhs())));
            } else if (bo->rhs()->type().ispar()) {
              stack.push_back(StackItem(bo->lhs(),c*LinearTraits<Lit>::eval(env,bo->rhs())));
            } else {
              coeffs.push_back(c);
              vars.push_back(e);
            }
            break;
          case BOT_DIV:
            if (bo->rhs()->isa<FloatLit>() && bo->rhs()->cast<FloatLit>()->v()==1.0) {
              stack.push_back(StackItem(bo->lhs(),c));
            } else {
              coeffs.push_back(c);
              vars.push_back(e);
            }
            break;
          case BOT_IDIV:
            if (bo->rhs()->isa<IntLit>() && bo->rhs()->cast<IntLit>()->v()==1) {
              stack.push_back(StackItem(bo->lhs(),c));
            } else {
              coeffs.push_back(c);
              vars.push_back(e);
            }
            break;
          default:
            coeffs.push_back(c);
            vars.push_back(e);
            break;
        }
        //      } else if (Call* call = e->dyn_cast<Call>()) {
        //        /// TODO! Handle sum, lin_exp (maybe not that important?)
      } else {
        coeffs.push_back(c);
        vars.push_back(e);
      }
    }
  }
  
  template<class Lit>
  KeepAlive mklinexp(EnvI& env, typename LinearTraits<Lit>::Val c0, typename LinearTraits<Lit>::Val c1,
                     Expression* e0, Expression* e1) {
    typedef typename LinearTraits<Lit>::Val Val;
    GCLock lock;
    
    std::vector<Val> coeffs;
    std::vector<KeepAlive> vars;
    Val constval = 0;
    collectLinExps<Lit>(env, c0, e0, coeffs, vars, constval);
    collectLinExps<Lit>(env, c1, e1, coeffs, vars, constval);
    simplify_lin<Lit>(coeffs, vars, constval);
    KeepAlive ka;
    if (coeffs.size()==0) {
      ka = LinearTraits<Lit>::newLit(constval);
    } else if (coeffs.size()==1 && coeffs[0]==1 && constval==0) {
      ka = vars[0];
    } else {
      std::vector<Expression*> coeffs_e(coeffs.size());
      for (unsigned int i=static_cast<unsigned int>(coeffs.size()); i--;) {
        if (!LinearTraits<Lit>::finite(coeffs[i])) {
          throw FlatteningError(env,e0->loc(),
              "unbounded coefficient in linear expression."
              " Make sure variables involved in non-linear/logical expressions have finite bounds"
              " in their definition or via constraints" );
        }
        coeffs_e[i] = LinearTraits<Lit>::newLit(coeffs[i]);
      }
      std::vector<Expression*> vars_e(vars.size());
      for (unsigned int i=static_cast<unsigned int>(vars.size()); i--;)
        vars_e[i] = vars[i]();
      
      std::vector<Expression*> args(3);
      args[0]=new ArrayLit(e0->loc(),coeffs_e);
      Type t = coeffs_e[0]->type();
      t.dim(1);
      args[0]->type(t);
      args[1]=new ArrayLit(e0->loc(),vars_e);
      Type tt = vars_e[0]->type();
      tt.dim(1);
      args[1]->type(tt);
      args[2] = LinearTraits<Lit>::newLit(constval);
      Call* c = new Call(e0->loc().introduce(),constants().ids.lin_exp,args);
      addPathAnnotation(env, c);
      tt = args[1]->type();
      tt.dim(0);
      c->decl(env.model->matchFn(env, c, false));
      if (c->decl()==NULL) {
        throw FlatteningError(env,c->loc(), "cannot find matching declaration");
      }
      c->type(c->decl()->rtype(env, args, false));
      ka = c;
    }
    assert(ka());
    return ka;
  }
  
  Call* aggregateAndOrOps(EnvI& env, BinOp* bo, bool negateArgs, BinOpType bot) {
    assert(bot == BOT_AND || bot == BOT_OR);
    BinOpType negbot = (bot == BOT_AND ? BOT_OR : BOT_AND);
    typedef std::pair<Expression*,bool> arg_literal;
    std::vector<arg_literal> bo_args(2);
    bo_args[0] = arg_literal(bo->lhs(), !negateArgs);
    bo_args[1] = arg_literal(bo->rhs(), !negateArgs);
    std::vector<Expression*> output_pos;
    std::vector<Expression*> output_neg;
    unsigned int processed = 0;
    while (processed < bo_args.size()) {
      BinOp* bo_arg = bo_args[processed].first->dyn_cast<BinOp>();
      UnOp* uo_arg = bo_args[processed].first->dyn_cast<UnOp>();
      bool positive = bo_args[processed].second;
      if (bo_arg && positive && bo_arg->op() == bot) {
        bo_args[processed].first = bo_arg->lhs();
        bo_args.push_back(arg_literal(bo_arg->rhs(),true));
      } else if (bo_arg && !positive && bo_arg->op() == negbot) {
        bo_args[processed].first = bo_arg->lhs();
        bo_args.push_back(arg_literal(bo_arg->rhs(),false));
      } else if (uo_arg && !positive && uo_arg->op() == UOT_NOT) {
        bo_args[processed].first = uo_arg->e();
        bo_args[processed].second = true;
      } else if (bot==BOT_OR && uo_arg && positive && uo_arg->op() == UOT_NOT) {
        output_neg.push_back(uo_arg->e());
        processed++;
      } else {
        if (positive) {
          output_pos.push_back(bo_args[processed].first);
        } else {
          output_neg.push_back(bo_args[processed].first);
        }
        processed++;
      }
    }
    Call* c;
    std::vector<Expression*> c_args(1);
    if (bot == BOT_AND) {
      for (unsigned int i=0; i<output_neg.size(); i++) {
        UnOp* neg_arg = new UnOp(output_neg[i]->loc(),UOT_NOT,output_neg[i]);
        neg_arg->type(output_neg[i]->type());
        output_pos.push_back(neg_arg);
      }
      ArrayLit* al = new ArrayLit(bo->loc().introduce(), output_pos);
      Type al_t = bo->type();
      al_t.dim(1);
      al->type(al_t);
      env.annotateFromCallStack(al);
      c_args[0] = al;
      c = new Call(bo->loc().introduce(), bot==BOT_AND ? constants().ids.forall : constants().ids.exists, c_args);
    } else {
      ArrayLit* al_pos = new ArrayLit(bo->loc().introduce(), output_pos);
      Type al_t = bo->type();
      al_t.dim(1);
      al_pos->type(al_t);
      env.annotateFromCallStack(al_pos);
      c_args[0] = al_pos;
      if (output_neg.size() > 0) {
        ArrayLit* al_neg = new ArrayLit(bo->loc().introduce(), output_neg);
        al_neg->type(al_t);
        env.annotateFromCallStack(al_neg);
        c_args.push_back(al_neg);
      }
      c = new Call(bo->loc().introduce(), output_neg.empty() ? constants().ids.exists : constants().ids.clause, c_args);
    }
    c->decl(env.model->matchFn(env, c, false));
    assert(c->decl());
    Type t = c->decl()->rtype(env, c_args, false);
    t.cv(bo->type().cv());
    c->type(t);
    return c;
  }
  
  /// Return a lin_exp or id if \a e is a lin_exp or id
  template<class Lit>
  Expression* get_linexp(Expression* e) {
    for (;;) {
      if (e && e->eid()==Expression::E_ID && e != constants().absent) {
        if (e->cast<Id>()->decl()->e()) {
          e = e->cast<Id>()->decl()->e();
        } else {
          break;
        }
      } else {
        break;
      }
    }
    if (e && (e->isa<Id>() || e->isa<Lit>() ||
              (e->isa<Call>() && e->cast<Call>()->id() == constants().ids.lin_exp)))
      return e;
    return NULL;
  }

  template<class Lit>
  void flatten_linexp_binop(EnvI& env, Ctx ctx, VarDecl* r, VarDecl* b, EE& ret,
                            Expression* le0, Expression* le1, BinOpType& bot, bool doubleNeg,
                            std::vector<EE>& ees, std::vector<KeepAlive>& args, ASTString& callid) {
    typedef typename LinearTraits<Lit>::Val Val;
    std::vector<Val> coeffv;
    std::vector<KeepAlive> alv;
    Val d = 0;
    Expression* le[2] = {le0,le1};
    
    Id* assignTo = NULL;
    if (bot==BOT_EQ && ctx.b == C_ROOT) {
      if (le0->isa<Id>()) {
        assignTo = le0->cast<Id>();
      } else if (le1->isa<Id>()) {
        assignTo = le1->cast<Id>();
      }
    }
    
    for (unsigned int i=0; i<2; i++) {
      Val sign = (i==0 ? 1 : -1);
      if (Lit* l = le[i]->dyn_cast<Lit>()) {
        try {
          d += sign*l->v();
        } catch (ArithmeticError& e) {
          throw EvalError(env,l->loc(),e.msg());
        }
      } else if (le[i]->isa<Id>()) {
        coeffv.push_back(sign);
        alv.push_back(le[i]);
      } else if (Call* sc = le[i]->dyn_cast<Call>()) {
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(env,sc->arg(0));
        ArrayLit* sc_al = eval_array_lit(env,sc->arg(1));
        try {
          d += sign*LinearTraits<Lit>::eval(env,sc->arg(2));
          for (unsigned int j=0; j<sc_coeff->size(); j++) {
            coeffv.push_back(sign*LinearTraits<Lit>::eval(env,(*sc_coeff)[j]));
            alv.push_back((*sc_al)[j]);
          }
        } catch (ArithmeticError& e) {
          throw EvalError(env,sc->loc(),e.msg());
        }
        
      } else {
        throw EvalError(env, le[i]->loc(), "Internal error, unexpected expression inside linear expression");
      }
    }
    simplify_lin<Lit>(coeffv,alv,d);
    if (coeffv.size()==0) {
      bool result;
      switch (bot) {
        case BOT_LE: result = (0<-d); break;
        case BOT_LQ: result = (0<=-d); break;
        case BOT_GR: result = (0>-d); break;
        case BOT_GQ: result = (0>=-d); break;
        case BOT_EQ: result = (0==-d); break;
        case BOT_NQ: result = (0!=-d); break;
        default: assert(false); break;
      }
      if (doubleNeg)
        result = !result;
      ees[2].b = constants().boollit(result);
      ret.r = conj(env,r,ctx,ees);
      return;
    } else if (coeffv.size()==1 &&
               std::abs(coeffv[0])==1) {
      if (coeffv[0]==-1) {
        switch (bot) {
          case BOT_LE: bot = BOT_GR; break;
          case BOT_LQ: bot = BOT_GQ; break;
          case BOT_GR: bot = BOT_LE; break;
          case BOT_GQ: bot = BOT_LQ; break;
          default: break;
        }
      } else {
        d = -d;
      }
      typename LinearTraits<Lit>::Bounds ib = LinearTraits<Lit>::compute_bounds(env,alv[0]());
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
            subsumed = ib.l==d && ib.u==d;
            failed = ib.u < d || ib.l > d;
            break;
          case BOT_NQ:
            subsumed = ib.u < d || ib.l > d;
            failed = ib.l==d && ib.u==d;
            break;
          default: break;
        }
        if (doubleNeg) {
          std::swap(subsumed, failed);
        }
        if (subsumed) {
          ees[2].b = constants().lit_true;
          ret.r = conj(env,r,ctx,ees);
          return;
        } else if (failed) {
          ees[2].b = constants().lit_false;
          ret.r = conj(env,r,ctx,ees);
          return;
        }
      }
      
      if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && bot==BOT_EQ) {
        GCLock lock;
        VarDecl* vd = alv[0]()->cast<Id>()->decl();
        if (vd->ti()->domain()) {
          typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(env,vd->ti()->domain());
          if (LinearTraits<Lit>::domain_contains(domain,d)) {
            if (!LinearTraits<Lit>::domain_equals(domain,d)) {
              //vd->ti()->setComputedDomain(false);
              //vd->ti()->domain(LinearTraits<Lit>::new_domain(d));
              setComputedDomain(env, vd, LinearTraits<Lit>::new_domain(d), false);
            }
            ret.r = bind(env,ctx,r,constants().lit_true);
          } else {
            ret.r = bind(env,ctx,r,constants().lit_false);
          }
        } else {
          //vd->ti()->setComputedDomain(false);
          //vd->ti()->domain(LinearTraits<Lit>::new_domain(d));
          setComputedDomain(env, vd, LinearTraits<Lit>::new_domain(d), false);
          ret.r = bind(env,ctx,r,constants().lit_true);
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
            if (e0->type().isint()) {
              d--;
              bot = BOT_LQ;
            }
            e1 = LinearTraits<Lit>::newLit(d);
            break;
          case BOT_GR:
            e1 = alv[0]();
            if (e1->type().isint()) {
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
        if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && alv[0]()->cast<Id>()->decl()->ti()->domain()) {
          VarDecl* vd = alv[0]()->cast<Id>()->decl();
          typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(env,vd->ti()->domain());
          typename LinearTraits<Lit>::Domain ndomain = LinearTraits<Lit>::limit_domain(old_bot,domain,old_d);
          if (domain && ndomain) {
            if (LinearTraits<Lit>::domain_empty(ndomain)) {
              ret.r = bind(env,ctx,r,constants().lit_false);
              return;
            } else if (!LinearTraits<Lit>::domain_equals(domain,ndomain)) {
              ret.r = bind(env,ctx,r,constants().lit_true);
              //vd->ti()->setComputedDomain(false);
              //vd->ti()->domain(LinearTraits<Lit>::new_domain(ndomain));
              setComputedDomain(env, vd, LinearTraits<Lit>::new_domain(ndomain), false);
              
              if (r==constants().var_true) {
                BinOp* bo = new BinOp(Location().introduce(), e0, bot, e1);
                bo->type(Type::varbool());
                std::vector<Expression*> boargs(2);
                boargs[0] = e0;
                boargs[1] = e1;
                Call* c = new Call(Location(), opToBuiltin(bo, bot), boargs);
                c->type(Type::varbool());
                c->decl(env.model->matchFn(env, c, false));
                EnvI::CSEMap::iterator it = env.cse_map_find(c);
                if (it != env.cse_map_end()) {
                  if (Id* ident = it->second.r()->template dyn_cast<Id>()) {
                    bind(env, Ctx(), ident->decl(), constants().lit_true);
                    it->second.r = constants().lit_true;
                  }
                  if (Id* ident = it->second.b()->template dyn_cast<Id>()) {
                    bind(env, Ctx(), ident->decl(), constants().lit_true);
                    it->second.b = constants().lit_true;
                  }
                }
              }
            }
            return;
          }
        }
        args.push_back(e0);
        args.push_back(e1);
      }
    } else if (bot==BOT_EQ && coeffv.size()==2 && coeffv[0]==-coeffv[1] && d==0) {
      Id* id0 = alv[0]()->cast<Id>();
      Id* id1 = alv[1]()->cast<Id>();
      if (ctx.b == C_ROOT && r==constants().var_true &&
          (id0->decl()->e()==NULL || id1->decl()->e()==NULL)) {
        if (id0->decl()->e())
          (void) bind(env,ctx,id1->decl(),id0);
        else
          (void) bind(env,ctx,id0->decl(),id1);
      } else {
        callid = LinearTraits<Lit>::id_eq();
        args.push_back(alv[0]());
        args.push_back(alv[1]());
      }
    } else {
      GCLock lock;
      if (assignTo != NULL) {
        Val resultCoeff = 0;
        typename LinearTraits<Lit>::Bounds bounds(d,d,true);
        for (unsigned int i=static_cast<unsigned int>(coeffv.size()); i--;) {
          if (alv[i]()==assignTo) {
            resultCoeff = coeffv[i];
            continue;
          }
          typename LinearTraits<Lit>::Bounds b = LinearTraits<Lit>::compute_bounds(env,alv[i]());
          
          if (b.valid && LinearTraits<Lit>::finite(b)) {
            if (coeffv[i] > 0) {
              bounds.l += coeffv[i]*b.l;
              bounds.u += coeffv[i]*b.u;
            } else {
              bounds.l += coeffv[i]*b.u;
              bounds.u += coeffv[i]*b.l;
            }
          } else {
            bounds.valid = false;
            break;
          }
        }
        if (bounds.valid && resultCoeff!=0) {
          if (resultCoeff < 0) {
            bounds.l = LinearTraits<Lit>::floor_div(bounds.l,-resultCoeff);
            bounds.u = LinearTraits<Lit>::ceil_div(bounds.u,-resultCoeff);
          } else {
            Val bl = bounds.l;
            bounds.l = LinearTraits<Lit>::ceil_div(bounds.u,-resultCoeff);
            bounds.u = LinearTraits<Lit>::floor_div(bl,-resultCoeff);
          }
          VarDecl* vd = assignTo->decl();
          if (vd->ti()->domain()) {
            typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(env,vd->ti()->domain());
            if (LinearTraits<Lit>::domain_intersects(domain,bounds.l,bounds.u)) {
              typename LinearTraits<Lit>::Domain new_domain = LinearTraits<Lit>::intersect_domain(domain,bounds.l,bounds.u);
              if (!LinearTraits<Lit>::domain_equals(domain,new_domain)) {
                //vd->ti()->setComputedDomain(false);
                //vd->ti()->domain(LinearTraits<Lit>::new_domain(new_domain));
                setComputedDomain(env, vd, LinearTraits<Lit>::new_domain(new_domain), false);
              }
            } else {
              ret.r = bind(env,ctx,r,constants().lit_false);
            }
          } else {
            //vd->ti()->setComputedDomain(true);
            //vd->ti()->domain(LinearTraits<Lit>::new_domain(bounds.l,bounds.u));
            setComputedDomain(env, vd, LinearTraits<Lit>::new_domain(bounds.l, bounds.u), true);
          }
        }
      }
      
      int coeff_sign;
      LinearTraits<Lit>::constructLinBuiltin(bot,callid,coeff_sign,d);
      std::vector<Expression*> coeff_ev(coeffv.size());
      for (unsigned int i=static_cast<unsigned int>(coeff_ev.size()); i--;)
        coeff_ev[i] = LinearTraits<Lit>::newLit(coeff_sign*coeffv[i]);
      ArrayLit* ncoeff = new ArrayLit(Location().introduce(),coeff_ev);
      Type t = coeff_ev[0]->type();
      t.dim(1);
      ncoeff->type(t);
      args.push_back(ncoeff);
      std::vector<Expression*> alv_e(alv.size());
      Type tt = alv[0]()->type();
      tt.dim(1);
      for (unsigned int i=static_cast<unsigned int>(alv.size()); i--;) {
        if (alv[i]()->type().isvar())
          tt.ti(Type::TI_VAR);
        alv_e[i] = alv[i]();
      }
      ArrayLit* nal = new ArrayLit(Location().introduce(),alv_e);
      nal->type(tt);
      args.push_back(nal);
      Lit* il = LinearTraits<Lit>::newLit(-d);
      args.push_back(il);
    }
  }
  
  EE flatten_binop(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    BinOp* bo = e->cast<BinOp>();
    if (isReverseMap(bo)) {
      CallArgItem cai(env);
      Id* id = bo->lhs()->dyn_cast<Id>();
      if (id==NULL)
        throw EvalError(env, bo->lhs()->loc(), "Reverse mappers are only defined for identifiers");
      if (bo->op() != BOT_EQ && bo->op() != BOT_EQUIV)
        throw EvalError(env, bo->loc(), "Reverse mappers have to use `=` as the operator");
      Call* c = bo->rhs()->dyn_cast<Call>();
      if (c==NULL)
        throw EvalError(env, bo->rhs()->loc(), "Reverse mappers require call on right hand side");
      
      std::vector<Expression*> args(c->n_args());
      for (unsigned int i=0; i<c->n_args(); i++) {
        Id* idi = c->arg(i)->dyn_cast<Id>();
        if (idi==NULL)
          throw EvalError(env, c->arg(i)->loc(), "Reverse mapper calls require identifiers as arguments");
        EE ee = flat_exp(env, Ctx(), idi, NULL, constants().var_true);
        args[i] = ee.r();
      }
      
      EE ee = flat_exp(env, Ctx(), id, NULL, constants().var_true);
      
      GCLock lock;
      Call* revMap = new Call(Location().introduce(),c->id(),args);
      
      args.push_back(ee.r());
      Call* keepAlive = new Call(Location().introduce(),constants().var_redef->id(),args);
      keepAlive->type(Type::varbool());
      keepAlive->decl(constants().var_redef);
      ret = flat_exp(env, Ctx(), keepAlive, constants().var_true, constants().var_true);
      
      if (ee.r()->isa<Id>()) {
        env.reverseMappers.insert(ee.r()->cast<Id>(),revMap);
      }
      return ret;
    }
    if ( (bo->op()==BOT_EQ ||  bo->op()==BOT_EQUIV) && (bo->lhs()==constants().absent || bo->rhs()==constants().absent) ) {
      GCLock lock;
      std::vector<Expression*> args(1);
      args[0] = bo->lhs()==constants().absent ? bo->rhs() : bo->lhs();
      if (args[0] != constants().absent) {
        Call* cr = new Call(bo->loc().introduce(),"absent",args);
        cr->decl(env.model->matchFn(env, cr, false));
        cr->type(cr->decl()->rtype(env, args, false));
        ret = flat_exp(env, ctx, cr, r, b);
      } else {
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,ctx,r,constants().lit_true);
      }
      return ret;
    }
    Ctx ctx0 = ctx;
    ctx0.neg = false;
    Ctx ctx1 = ctx;
    ctx1.neg = false;
    BinOpType bot = bo->op();
    if (bo->lhs()->type().isbool()) {
      switch (bot) {
        case BOT_EQ: bot = BOT_EQUIV; break;
        case BOT_NQ: bot = BOT_XOR; break;
        case BOT_LQ: bot = BOT_IMPL; break;
        case BOT_GQ: bot = BOT_RIMPL; break;
        default: break;
      }
    }
    bool negArgs = false;
    bool doubleNeg = false;
    if (ctx.neg) {
      switch (bot) {
        case BOT_AND:
          ctx.b = -ctx.b;
          ctx.neg = false;
          negArgs = true;
          bot = BOT_OR;
          break;
        case BOT_OR:
          ctx.b = -ctx.b;
          ctx.neg = false;
          negArgs = true;
          bot = BOT_AND;
          break;
        default: break;
      }
    }
    Expression* boe0 = bo->lhs();
    Expression* boe1 = bo->rhs();
    switch (bot) {
      case BOT_PLUS:
      {
        KeepAlive ka;
        if (boe0->type().isint()) {
          ka = mklinexp<IntLit>(env,1,1,boe0,boe1);
        } else {
          ka = mklinexp<FloatLit>(env,1.0,1.0,boe0,boe1);
        }
        ret = flat_exp(env,ctx,ka(),r,b);
      }
        break;
      case BOT_MINUS:
      {
        KeepAlive ka;
        if (boe0->type().isint()) {
          ka = mklinexp<IntLit>(env,1,-1,boe0,boe1);
        } else {
          ka = mklinexp<FloatLit>(env,1.0,-1.0,boe0,boe1);
        }
        ret = flat_exp(env,ctx,ka(),r,b);
      }
        break;
      case BOT_MULT:
      case BOT_IDIV:
      case BOT_MOD:
      case BOT_POW:
      case BOT_DIV:
      case BOT_UNION:
      case BOT_DIFF:
      case BOT_SYMDIFF:
      case BOT_INTERSECT:
      case BOT_DOTDOT:
      {
        assert(!ctx0.neg);
        assert(!ctx1.neg);
        EE e0 = flat_exp(env,ctx0,boe0,NULL,b);
        EE e1 = flat_exp(env,ctx1,boe1,NULL,b);
        
        if (e0.r()->type().ispar() && e1.r()->type().ispar()) {
          GCLock lock;
          BinOp* parbo = new BinOp(bo->loc(),e0.r(),bo->op(),e1.r());
          Type tt = bo->type();
          tt.ti(Type::TI_PAR);
          parbo->type(tt);
          try {
            Expression* res = eval_par(env,parbo);
            assert(!res->type().isunknown());
            ret.r = bind(env,ctx,r,res);
            std::vector<EE> ees(2);
            ees[0].b = e0.b; ees[1].b = e1.b;
            ret.b = conj(env,b,Ctx(),ees);
          } catch (ResultUndefinedError&) {
            ret.r = createDummyValue(env, e->type());
            ret.b = bind(env,Ctx(),b,constants().lit_false);
          }
          break;
        }
        
        if (bot==BOT_MULT) {
          Expression* e0r = e0.r();
          Expression* e1r = e1.r();
          if (e0r->type().ispar())
            std::swap(e0r,e1r);
          if (e1r->type().ispar() && e1r->type().isint()) {
            IntVal coeff = eval_int(env,e1r);
            KeepAlive ka = mklinexp<IntLit>(env,coeff,0,e0r,NULL);
            ret = flat_exp(env,ctx,ka(),r,b);
            break;
          } else if (e1r->type().ispar() && e1r->type().isfloat()) {
            FloatVal coeff = eval_float(env,e1r);
            KeepAlive ka = mklinexp<FloatLit>(env,coeff,0.0,e0r,NULL);
            ret = flat_exp(env,ctx,ka(),r,b);
            break;
          }
        } else if (bot==BOT_DIV || bot==BOT_IDIV) {
          Expression* e0r = e0.r();
          Expression* e1r = e1.r();
          if (e1r->type().ispar() && e1r->type().isint()) {
            IntVal coeff = eval_int(env,e1r);
            if (coeff==1) {
              ret = flat_exp(env,ctx,e0r,r,b);
              break;
            }
          } else if (e1r->type().ispar() && e1r->type().isfloat()) {
            FloatVal coeff = eval_float(env,e1r);
            if (coeff==1.0) {
              ret = flat_exp(env,ctx,e0r,r,b);
              break;
            } else {
              KeepAlive ka = mklinexp<FloatLit>(env,1.0/coeff,0.0,e0r,NULL);
              ret = flat_exp(env,ctx,ka(),r,b);
              break;
            }
          }
        }
        
        
        GC::lock();
        std::vector<Expression*> args(2);
        args[0] = e0.r(); args[1] = e1.r();
        Call* cc;
        if (bo->decl()) {
          cc = new Call(bo->loc().introduce(),bo->opToString(),args);
        } else {
          cc = new Call(bo->loc().introduce(),opToBuiltin(bo,bot),args);
        }
        cc->type(bo->type());
        
        EnvI::CSEMap::iterator cit;
        if ( (cit = env.cse_map_find(cc)) != env.cse_map_end()) {
          ret.b = bind(env,Ctx(),b,env.ignorePartial ? constants().lit_true : cit->second.b());
          ret.r = bind(env,ctx,r,cit->second.r());
        } else {
          if (FunctionI* fi = env.model->matchFn(env,cc->id(),args,false)) {
            assert(cc->type() == fi->rtype(env,args,false));
            cc->decl(fi);
            cc->type(cc->decl()->rtype(env,args,false));
            KeepAlive ka(cc);
            GC::unlock();
            EE ee = flat_exp(env,ctx,cc,r,NULL);
            GC::lock();
            ret.r = ee.r;
            std::vector<EE> ees(3);
            ees[0].b = e0.b; ees[1].b = e1.b; ees[2].b = ee.b;
            ret.b = conj(env,b,Ctx(),ees);
          } else {
            addPathAnnotation(env, cc);
            ret.r = bind(env,ctx,r,cc);
            std::vector<EE> ees(2);
            ees[0].b = e0.b; ees[1].b = e1.b;
            ret.b = conj(env,b,Ctx(),ees);
            if (!ctx.neg)
              env.cse_map_insert(cc,ret);
          }
        }
      }
        GC::unlock();
        break;
        
      case BOT_AND:
      {
        if (r==constants().var_true) {
          Ctx nctx;
          nctx.neg = negArgs;
          nctx.b = negArgs ? C_NEG : C_ROOT;
          std::vector<Expression*> todo;
          todo.push_back(boe1);
          todo.push_back(boe0);
          while (!todo.empty()) {
            Expression* e_todo = todo.back();
            todo.pop_back();
            BinOp* e_bo = e_todo->dyn_cast<BinOp>();
            if (e_bo && e_bo->op()==BOT_AND) {
              todo.push_back(e_bo->rhs());
              todo.push_back(e_bo->lhs());
            } else {
              (void) flat_exp(env,nctx,e_todo,constants().var_true,constants().var_true);
            }
          }
          ret.r = bind(env,ctx,r,constants().lit_true);
          break;
        } else {
          GC::lock();
          Call* c = aggregateAndOrOps(env, bo, negArgs, bot);
          KeepAlive ka(c);
          GC::unlock();
          ret = flat_exp(env,ctx,c,r,b);
          if (Id* id = ret.r()->dyn_cast<Id>()) {
            addCtxAnn(id->decl(), ctx.b);
          }
        }
        break;
      }
      case BOT_OR:
      {
        GC::lock();
        Call* c = aggregateAndOrOps(env, bo, negArgs, bot);
        KeepAlive ka(c);
        GC::unlock();
        ret = flat_exp(env,ctx,c,r,b);
        if (Id* id = ret.r()->dyn_cast<Id>()) {
          addCtxAnn(id->decl(), ctx.b);
        }
      }
        break;
      case BOT_RIMPL:
      {
        std::swap(boe0,boe1);
      }
        // fall through
      case BOT_IMPL:
      {
        if (ctx.b==C_ROOT && r==constants().var_true && boe0->type().ispar()) {
          bool b;
          {
            GCLock lock;
            b = eval_bool(env,boe0);
          }
          if (b) {
            Ctx nctx = ctx;
            nctx.neg = negArgs;
            nctx.b = negArgs ? C_NEG : C_ROOT;
            ret = flat_exp(env,nctx,boe1,constants().var_true,constants().var_true);
          } else {
            Ctx nctx = ctx;
            nctx.neg = negArgs;
            nctx.b = negArgs ? C_NEG : C_ROOT;
            ret = flat_exp(env,nctx,constants().lit_true,constants().var_true,constants().var_true);
          }
          break;
        }
        if (ctx.b==C_ROOT && r==constants().var_true && boe1->type().ispar()) {
          bool b;
          {
            GCLock lock;
            b = eval_bool(env,boe1);
          }
          if (b) {
            Ctx nctx = ctx;
            nctx.neg = negArgs;
            nctx.b = negArgs ? C_NEG : C_ROOT;
            ret = flat_exp(env,nctx,constants().lit_true,constants().var_true,constants().var_true);
            break;
          } else {
            Ctx nctx = ctx;
            nctx.neg = !negArgs;
            nctx.b = !negArgs ? C_NEG : C_ROOT;
            ret = flat_exp(env,nctx,boe0,constants().var_true,constants().var_true);
            break;
          }
        }
        GC::lock();
        std::vector<Expression*> args;
        ASTString id;
        if (ctx.neg) {
          std::vector<Expression*> bo_args(2);
          bo_args[0] = boe0;
          bo_args[1] = new UnOp(bo->loc(),UOT_NOT,boe1);
          bo_args[1]->type(boe1->type());
          id = constants().ids.forall;
          args.push_back(new ArrayLit(bo->loc(),bo_args));
          args[0]->type(Type::varbool(1));
        } else {
          std::vector<Expression*> clause_pos(1);
          clause_pos[0] = boe1;
          std::vector<Expression*> clause_neg(1);
          clause_neg[0] = boe0;
          args.push_back(new ArrayLit(boe1->loc().introduce(), clause_pos));
          Type t0 = boe1->type();
          t0.dim(1);
          args[0]->type(t0);
          args.push_back(new ArrayLit(boe0->loc().introduce(), clause_neg));
          Type t1 = boe0->type();
          t1.dim(1);
          args[1]->type(t1);
          id = constants().ids.clause;
        }
        ctx.neg = false;
        Call* c = new Call(bo->loc().introduce(),id,args);
        c->decl(env.model->matchFn(env,c,false));
        if (c->decl()==NULL) {
          throw FlatteningError(env,c->loc(), "cannot find matching declaration");
        }
        c->type(c->decl()->rtype(env,args,false));
        KeepAlive ka(c);
        GC::unlock();
        ret = flat_exp(env,ctx,c,r,b);
        if (Id* id = ret.r()->dyn_cast<Id>()) {
          addCtxAnn(id->decl(),ctx.b);
        }
      }
        break;
      case BOT_EQUIV:
        if (ctx.neg) {
          ctx.neg = false;
          ctx.b = -ctx.b;
          bot = BOT_XOR;
          ctx0.b = ctx1.b = C_MIX;
          goto flatten_bool_op;
        } else {
          if (!boe1->type().isopt() && istrue(env, boe0)) {
            return flat_exp(env, ctx, boe1, r, b);
          }
          if (!boe0->type().isopt() && istrue(env, boe1)) {
            return flat_exp(env, ctx, boe0, r, b);
          }
          if (r && r==constants().var_true) {
            if (boe1->type().ispar() || boe1->isa<Id>())
              std::swap(boe0,boe1);
            if (istrue(env,boe0)) {
              return flat_exp(env,ctx1,boe1,r,b);
            } else if (isfalse(env,boe0)) {
              ctx1.neg = true;
              ctx1.b = -ctx1.b;
              return flat_exp(env,ctx1,boe1,r,b);
            } else {
              ctx0.b = C_MIX;
              EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
              if (istrue(env,e0.r())) {
                return flat_exp(env,ctx1,boe1,r,b);
              } else if (isfalse(env,e0.r())) {
                ctx1.neg = true;
                ctx1.b = -ctx1.b;
                return flat_exp(env,ctx1,boe1,r,b);
              } else {
                Id* id = e0.r()->cast<Id>();
                ctx1.b = C_MIX;
                (void) flat_exp(env,ctx1,boe1,id->decl(),constants().var_true);
                addCtxAnn(id->decl(), ctx1.b);
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                ret.r = bind(env,Ctx(),r,constants().lit_true);
              }
            }
            break;
          } else {
            ctx0.b = ctx1.b = C_MIX;
            goto flatten_bool_op;
          }
        }
      case BOT_XOR:
        if (ctx.neg) {
          ctx.neg = false;
          ctx.b = -ctx.b;
          bot = BOT_EQUIV;
        }
        ctx0.b = ctx1.b = C_MIX;
        goto flatten_bool_op;
      case BOT_LE:
        if (ctx.neg) {
          doubleNeg = true;
          bot = BOT_GQ;
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = +ctx0.b;
            ctx1.b = -ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = +ctx0.b;
            ctx1.i = -ctx1.b;
          }
        } else {
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = -ctx0.b;
            ctx1.b = +ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = -ctx0.b;
            ctx1.i = +ctx1.b;
          }
        }
        goto flatten_bool_op;
      case BOT_LQ:
        if (ctx.neg) {
          doubleNeg = true;
          bot = BOT_GR;
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = +ctx0.b;
            ctx1.b = -ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = +ctx0.b;
            ctx1.i = -ctx1.b;
          }
        } else {
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = -ctx0.b;
            ctx1.b = +ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = -ctx0.b;
            ctx1.i = +ctx1.b;
          }
        }
        goto flatten_bool_op;
      case BOT_GR:
        if (ctx.neg) {
          doubleNeg = true;
          bot = BOT_LQ;
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = -ctx0.b;
            ctx1.b = +ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = -ctx0.b;
            ctx1.i = +ctx1.b;
          }
        } else {
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = +ctx0.b;
            ctx1.b = -ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = +ctx0.b;
            ctx1.i = -ctx1.b;
          }
        }
        goto flatten_bool_op;
      case BOT_GQ:
        if (ctx.neg) {
          doubleNeg = true;
          bot = BOT_LE;
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = -ctx0.b;
            ctx1.b = +ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = -ctx0.b;
            ctx1.i = +ctx1.b;
          }
        } else {
          if (boe0->type().bt()==Type::BT_BOOL) {
            ctx0.b = +ctx0.b;
            ctx1.b = -ctx1.b;
          } else if (boe0->type().bt()==Type::BT_INT) {
            ctx0.i = +ctx0.b;
            ctx1.i = -ctx1.b;
          }
        }
        goto flatten_bool_op;
      case BOT_EQ:
        if (ctx.neg) {
          doubleNeg = true;
          bot = BOT_NQ;
        }
        if (boe0->type().bt()==Type::BT_BOOL) {
          ctx0.b = ctx1.b = C_MIX;
        } else if (boe0->type().bt()==Type::BT_INT) {
          ctx0.i = ctx1.i = C_MIX;
        }
        goto flatten_bool_op;
      case BOT_NQ:
        if (ctx.neg) {
          doubleNeg = true;
          bot = BOT_EQ;
        }
        if (boe0->type().bt()==Type::BT_BOOL) {
          ctx0.b = ctx1.b = C_MIX;
        } else if (boe0->type().bt()==Type::BT_INT) {
          ctx0.i = ctx1.i = C_MIX;
        }
        goto flatten_bool_op;
      case BOT_IN:
      case BOT_SUBSET:
      case BOT_SUPERSET:
        ctx0.i = ctx1.i = C_MIX;
      flatten_bool_op:
      {
        bool inRootCtx = (ctx0.b==ctx1.b && ctx0.b==C_ROOT && b==constants().var_true);
        EE e0 = flat_exp(env,ctx0,boe0,NULL,inRootCtx ? b : NULL);
        EE e1 = flat_exp(env,ctx1,boe1,NULL,inRootCtx ? b : NULL);
        
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        
        std::vector<EE> ees(3);
        ees[0].b = e0.b; ees[1].b = e1.b;
        
        if (isfalse(env, e0.b()) || isfalse(env, e1.b())) {
          ees.resize(2);
          ret.r = conj(env,r,ctx,ees);
          break;
        }
        
        if (e0.r()->type().ispar() && e1.r()->type().ispar()) {
          GCLock lock;
          BinOp* bo_par = new BinOp(e->loc(),e0.r(),bot,e1.r());
          std::vector<Expression*> args({e0.r(),e1.r()});
          bo_par->decl(env.model->matchFn(env,bo_par->opToString(),args,false));
          if (bo_par->decl()==NULL) {
            throw FlatteningError(env,bo_par->loc(), "cannot find matching declaration");
          }
          bo_par->type(Type::parbool());
          bool bo_val = eval_bool(env,bo_par);
          if (doubleNeg)
            bo_val = !bo_val;
          ees[2].b = constants().boollit(bo_val);
          ret.r = conj(env,r,ctx,ees);
          break;
        }
        
        if (e0.r()->type().bt()==Type::BT_INT && e1.r()->type().ispar() && e0.r()->isa<Id>() && (bot==BOT_IN || bot==BOT_SUBSET)) {
          VarDecl* vd = e0.r()->cast<Id>()->decl();
          Id* ident = vd->id();
          if (ctx.b==C_ROOT && r==constants().var_true) {
            if (vd->ti()->domain()==NULL) {
              vd->ti()->domain(e1.r());
            } else {
              GCLock lock;
              IntSetVal* newdom = eval_intset(env,e1.r());
              while (ident != NULL) {
                bool changeDom = false;
                if (ident->decl()->ti()->domain()) {
                  IntSetVal* domain = eval_intset(env,ident->decl()->ti()->domain());
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(newdom);
                  Ranges::Inter<IntVal,IntSetRanges,IntSetRanges> i(dr,ibr);
                  IntSetVal* newibv = IntSetVal::ai(i);
                  if (domain->card() != newibv->card()) {
                    newdom = newibv;
                    changeDom = true;
                  }
                } else {
                  changeDom = true;
                }
                if (ident->type().st()==Type::ST_PLAIN && newdom->size()==0) {
                  env.fail();
                } else if (changeDom) {
                  //ident->decl()->ti()->setComputedDomain(false);
                  //ident->decl()->ti()->domain(new SetLit(Location().introduce(),newdom));
                  setComputedDomain(env, ident->decl(), new SetLit(Location().introduce(), newdom), false);
                  if (ident->decl()->e()==NULL && newdom->min()==newdom->max()) {
                    ident->decl()->e(IntLit::a(newdom->min()));
                  }
                }
                ident = ident->decl()->e() ? ident->decl()->e()->dyn_cast<Id>() : NULL;
              }
            }
            ret.r = bind(env,ctx,r,constants().lit_true);
            break;
          } else if (vd->ti()->domain()!=NULL) {
            // check if current domain is already subsumed or falsified by this constraint
            GCLock lock;
            IntSetVal* check_dom = eval_intset(env,e1.r());
            IntSetVal* domain = eval_intset(env,ident->decl()->ti()->domain());
            {
              IntSetRanges cdr(check_dom);
              IntSetRanges dr(domain);
              if (Ranges::subset(dr,cdr)) {
                // the constraint is subsumed
                ret.r = bind(env,ctx,r,constants().lit_true);
                break;
              }
            }
            if (vd->type().st()==Type::ST_PLAIN) {
              // only for var int (for var set of int, subset can never fail because of the domain)
              IntSetRanges cdr(check_dom);
              IntSetRanges dr(domain);
              if (Ranges::disjoint(cdr, dr)) {
                // the constraint is false
                ret.r = bind(env,ctx,r,constants().lit_false);
                break;
              }
            }
          }
        }
        
        std::vector<KeepAlive> args;
        ASTString callid;
        
        Expression* le0 = NULL;
        Expression* le1 = NULL;
        
        if (boe0->type().isint() && !boe0->type().isopt() && bot != BOT_IN) {
          le0 = get_linexp<IntLit>(e0.r());
        } else if (boe0->type().isfloat() && !boe0->type().isopt() && bot != BOT_IN) {
          le0 = get_linexp<FloatLit>(e0.r());
        }
        if (le0) {
          if (boe0->type().isint() && boe1->type().isint() && !boe1->type().isopt()) {
            le1 = get_linexp<IntLit>(e1.r());
          } else if (boe0->type().isfloat() && boe1->type().isfloat() && !boe1->type().isopt()) {
            le1 = get_linexp<FloatLit>(e1.r());
          }
        }
        if (le1) {
          if (boe0->type().isint()) {
            flatten_linexp_binop<IntLit>(env,ctx,r,b,ret,le0,le1,bot,doubleNeg,ees,args,callid);
          } else {
            flatten_linexp_binop<FloatLit>(env,ctx,r,b,ret,le0,le1,bot,doubleNeg,ees,args,callid);
          }
        } else {
          if (bo->decl()==NULL) {
            switch (bot) {
              case BOT_GR:
                std::swap(e0,e1);
                bot = BOT_LE;
                break;
              case BOT_GQ:
                std::swap(e0,e1);
                bot = BOT_LQ;
                break;
              default:
                break;
            }
          }
          args.push_back(e0.r);
          args.push_back(e1.r);
        }
        
        if (args.size() > 0) {
          GC::lock();
          
          if (callid=="") {
            if (bo->decl()) {
              callid = bo->decl()->id();
            } else {
              callid = opToBuiltin(bo,bot);
            }
          }
          
          std::vector<Expression*> args_e(args.size());
          for (unsigned int i=static_cast<unsigned int>(args.size()); i--;)
            args_e[i] = args[i]();
          Call* cc = new Call(e->loc().introduce(),callid,args_e);
          cc->decl(env.model->matchFn(env,cc->id(),args_e,false));
          if (cc->decl()==NULL) {
            throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
          }
          cc->type(cc->decl()->rtype(env,args_e,false));
          
          // add defines_var annotation if applicable
          Id* assignTo = NULL;
          if (bot==BOT_EQ && ctx.b == C_ROOT) {
            if (le0 && le0->isa<Id>()) {
              assignTo = le0->cast<Id>();
            } else if (le1 && le1->isa<Id>()) {
              assignTo = le1->cast<Id>();
            }
            if (assignTo) {
              makeDefinedVar(assignTo->decl()->flat(), cc);
            }
          }
          
          EnvI::CSEMap::iterator cit = env.cse_map_find(cc);
          if (cit != env.cse_map_end()) {
            ees[2].b = cit->second.r();
            if (doubleNeg) {
              Type t = ees[2].b()->type();
              ees[2].b = new UnOp(Location().introduce(),UOT_NOT,ees[2].b());
              ees[2].b()->type(t);
            }
            if (Id* id = ees[2].b()->dyn_cast<Id>()) {
              addCtxAnn(id->decl(),ctx.b);
            }
            ret.r = conj(env,r,ctx,ees);
            GC::unlock();
          } else {
            bool singleExp = true;
            for (unsigned int i=0; i<ees.size(); i++) {
              if (!istrue(env,ees[i].b())) {
                singleExp = false;
                break;
              }
            }
            KeepAlive ka(cc);
            GC::unlock();
            if (singleExp) {
              if (doubleNeg) {
                ctx.b = -ctx.b;
                ctx.neg = !ctx.neg;
              }
              ret.r = flat_exp(env,ctx,cc,r,NULL).r;
            } else {
              ees[2].b = flat_exp(env,Ctx(),cc,NULL,NULL).r;
              if (doubleNeg) {
                GCLock lock;
                Type t = ees[2].b()->type();
                ees[2].b = new UnOp(Location().introduce(),UOT_NOT,ees[2].b());
                ees[2].b()->type(t);
              }
              if (Id* id = ees[2].b()->dyn_cast<Id>()) {
                addCtxAnn(id->decl(),ctx.b);
              }
              ret.r = conj(env,r,ctx,ees);
            }
          }
        } else {
          ret.r = conj(env,r,ctx,ees);
        }
      }
        break;
        
      case BOT_PLUSPLUS:
      {
        std::vector<EE> ee(2);
        EE eev = flat_exp(env,ctx,boe0,NULL,NULL);
        ee[0] = eev;
        ArrayLit* al;
        if (eev.r()->isa<ArrayLit>()) {
          al = eev.r()->cast<ArrayLit>();
        } else {
          Id* id = eev.r()->cast<Id>();
          if (id->decl()==NULL) {
            throw InternalError("undefined identifier");
          }
          if (id->decl()->e()==NULL) {
            throw InternalError("array without initialiser not supported");
          }
          al = follow_id(id)->cast<ArrayLit>();
        }
        ArrayLit* al0 = al;
        eev = flat_exp(env,ctx,boe1,NULL,NULL);
        ee[1] = eev;
        if (eev.r()->isa<ArrayLit>()) {
          al = eev.r()->cast<ArrayLit>();
        } else {
          Id* id = eev.r()->cast<Id>();
          if (id->decl()==NULL) {
            throw InternalError("undefined identifier");
          }
          if (id->decl()->e()==NULL) {
            throw InternalError("array without initialiser not supported");
          }
          al = follow_id(id)->cast<ArrayLit>();
        }
        ArrayLit* al1 = al;
        std::vector<Expression*> v(al0->size()+al1->size());
        for (unsigned int i=al0->size(); i--;)
          v[i] = (*al0)[i];
        for (unsigned int i=al1->size(); i--;)
          v[al0->size()+i] = (*al1)[i];
        GCLock lock;
        ArrayLit* alret = new ArrayLit(e->loc(),v);
        alret->type(e->type());
        ret.b = conj(env,b,Ctx(),ee);
        ret.r = bind(env,ctx,r,alret);
      }
        break;
    }
    return ret;
  }
  
}
