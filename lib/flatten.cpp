/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/copy.hh>
#include <minizinc/hash.hh>
#include <minizinc/exception.hh>

// temporary
#include <minizinc/prettyprinter.hh>

namespace MiniZinc {

  /// Evaluation context
  enum BCtx { C_ROOT, C_POS, C_NEG, C_MIX };

  /// Turn \a c into positive context
  BCtx operator +(const BCtx& c) {
    switch (c) {
    case C_ROOT: return C_POS;
    case C_POS: return C_POS;
    case C_NEG: return C_NEG;
    case C_MIX: return C_MIX;
    default: assert(false); return C_ROOT;
    }
  }

  /// Turn \a c into negative context
  BCtx operator -(const BCtx& c) {
    switch (c) {
    case C_ROOT: return C_NEG;
    case C_POS: return C_NEG;
    case C_NEG: return C_POS;
    case C_MIX: return C_MIX;
    default: assert(false); return C_ROOT;
    }
  }
  /// Check if \a c is non-positive
  bool nonpos(const BCtx& c) {
    return c==C_NEG || c==C_MIX;
  }
  /// Check if \a c is non-negative
  bool nonneg(const BCtx& c) {
    return c==C_ROOT || c==C_POS;
  }

  /// Result of evaluation
  class EE {
  public:
    /// The result value
    Expression* r;
    /// Boolean expression representing whether result is defined
    Expression* b;
    /// Constructor
    explicit EE(Expression* r0=NULL, Expression* b0=NULL) : r(r0), b(b0) {}
  };

  class Constants {
  private:
    ASTContext ctx;
  public:
    BoolLit* lt;
    VarDecl* t;
    BoolLit* lf;
    VarDecl* f;
    Constants(void) {
      TypeInst* ti = TypeInst::a(ctx, Location(), Type::parbool());
      lt = BoolLit::a(ctx, Location(), true);
      t = VarDecl::a(ctx, Location(), ti, "_bool_true", lt);
      lf = BoolLit::a(ctx, Location(), false);
      f = VarDecl::a(ctx, Location(), ti, "_bool_false", lf);
    }
  } constants;

  /// Check if \a e is NULL or true
  bool istrue(ASTContext& ctx, Expression* e) {
    return e==NULL || (e->_type.ispar() && e->_type.isbool()
                       && eval_bool(ctx,e));
  }  
  /// Check if \a e is non-NULL and false
  bool isfalse(ASTContext& ctx, Expression* e) {
    return e!=NULL && e->_type.ispar() && e->_type.isbool()
           && !eval_bool(ctx,e);
  }  

  class Env {
  public:
    ASTContext& ctx;
    Model* m;
    typedef ExpressionMap<EE> Map;
    Map map;
    unsigned int ids;
    Env(ASTContext& ctx0, Model* m0) : ctx(ctx0), m(m0), ids(0) {}
    CtxStringH genId(const std::string& s) {
      std::ostringstream oss; oss << "_" << s << "_" << ids++;
      return CtxStringH(ctx,oss.str());
    }
  };

  bool isTotal(FunctionI* fi) {
    Annotation* a = fi->_ann;
    Printer p;
    for (; a!=NULL; a=a->_a) {
      VarDecl* vd = NULL;
      Expression * ae = a->_e;
      while (ae && ae->_eid==Expression::E_ID &&
             ae->cast<Id>()->_decl!=NULL) {
        vd = ae->cast<Id>()->_decl;
        ae = vd->_e;
      }
      
      if (vd && vd->_type.isann() && vd->_id == "total")
        return true;
    }
    return false;
  }

  Expression* bind(Env& env, VarDecl* vd, Expression* e) {
    if (vd==constants.t) {
      if (!istrue(env.ctx,e)) {
        if (Id* id = e->dyn_cast<Id>()) {
          assert(id->_decl != NULL);
          id->_decl->_ti->_domain = constants.lt;
        } else {
          env.m->addItem(ConstraintI::a(env.ctx,Location(),e));
        }
      }
      return NULL;
    } else if (vd==constants.f) {
      if (!isfalse(env.ctx,e)) {
        assert(false);
        throw InternalError("not supported yet");
      }
      return NULL;
    } else if (vd==NULL) {
      if (e==NULL) return NULL;
      switch (e->_eid) {
      case Expression::E_INTLIT:
      case Expression::E_FLOATLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
      case Expression::E_ID:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_VARDECL:
      case Expression::E_ANN:
        return e;
      case Expression::E_BINOP:
      case Expression::E_UNOP:
        return e; /// TODO: should not happen once operators are evaluated
      case Expression::E_ARRAYACCESS:
      case Expression::E_COMP:
      case Expression::E_ITE:
      case Expression::E_LET:
      case Expression::E_TI:
        assert(false && "unevaluated expression");
        throw InternalError("unevaluated expression");
      case Expression::E_ARRAYLIT:
        return e;
      case Expression::E_CALL:
        {
          /// TODO: handle array types
          TypeInst* ti = TypeInst::a(env.ctx,Location(),e->_type);
          VarDecl* vd = VarDecl::a(env.ctx,Location(),ti,env.genId("X"),e);
          VarDeclI* nv = VarDeclI::a(env.ctx,Location(),vd);
          env.m->addItem(nv);
          Id* id = Id::a(env.ctx,Location(),vd->_id,vd);
          id->_type = e->_type;

          EE ee(vd,NULL);
          env.map.insert(id,ee);

          return id;
        }
      }
    } else {
      if (vd->_e==NULL) {
        if (e==NULL) {
          vd->_e = constants.lt;
        } else if (e->_type.ispar() && e->_type.isbool()) {
          if (eval_bool(env.ctx,e)) {
            vd->_e = constants.lt;
          } else {
            vd->_e = constants.lf;
          }
        } else {
          vd->_e = e;
        }
        return e;
      } else if (vd->_e != e) {
        assert(false);
        throw InternalError("not supported yet");
      } else {
        return e;
      }
    }
  }

  EE flat_exp(Env& env, BCtx bctx, Expression* e, VarDecl* r, VarDecl* b);

  Expression* conj(Env& env,VarDecl* b, const std::vector<EE>& e) {
    std::vector<Expression*> nontrue;
    for (const EE& ee : e) {
      if (istrue(env.ctx,ee.b))
        continue;
      if (isfalse(env.ctx,ee.b)) {
        return bind(env,b,constants.lf);
      }
      nontrue.push_back(ee.b);
    }
    if (nontrue.empty()) {
      return bind(env,b,constants.lt);
    } else if (nontrue.size()==1) {
      return bind(env,b,nontrue[0]);
    } else {
      if (b==constants.t) {
        for (Expression* ne : nontrue)
          bind(env,b,ne);
        return NULL;
      } else {
        BinOp* ret = BinOp::a(env.ctx,Location(),
                              nontrue[0],BOT_AND,nontrue[1]);
        ret->_type = Type::varbool();
        for (unsigned int i=2; i<nontrue.size(); i++) {
          ret = BinOp::a(env.ctx,Location(),ret,BOT_AND,nontrue[i]);
          ret->_type = Type::varbool();
        }
        EE rete = flat_exp(env,C_ROOT,ret,NULL,constants.t);
        return bind(env,b,rete.r);
      }
    }
  }

  TypeInst* eval_typeinst(Env& env, TypeInst* ti) {
    /// TODO: evaluate all par components in the domain. This probably
    ///       needs the VarDecl to compute the actual dimensions of
    ///       array[int] expressions
    return eval_par(env.ctx,ti)->cast<TypeInst>();
  }

  std::string opToBuiltin(BinOp* op) {
    std::string builtin;
    if (op->_e1->_type.isint()) {
      builtin = "int_";
    } else if (op->_e1->_type.isbool()) {
      builtin = "bool_";
    } else if (op->_e1->_type.isset()) {
      builtin = "set_";
    } else {
      throw InternalError("not yet implemented");
    }
    switch (op->_op) {
    case BOT_PLUS:
      return builtin+"plus";
    case BOT_MINUS:
      return builtin+"minus";
    case BOT_MULT:
      return builtin+"mult";
    case BOT_DIV:
      return builtin+"div";
    case BOT_IDIV:
      return builtin+"safediv";
    case BOT_MOD:
      return builtin+"mod";
    case BOT_LE:
      return builtin+"le";
    case BOT_LQ:
      return builtin+"lq";
    case BOT_GR:
      return builtin+"gr";
    case BOT_GQ:
      return builtin+"gq";
    case BOT_EQ:
      return builtin+"eq";
    case BOT_NQ:
      return builtin+"nq";
    case BOT_IN:
      return "set_in";
    case BOT_SUBSET:
      return "subset";
    case BOT_SUPERSET:
      return "superset";
    case BOT_UNION:
    case BOT_DIFF:
    case BOT_SYMDIFF:
    case BOT_INTERSECT:
    case BOT_PLUSPLUS:
    case BOT_DOTDOT:
      throw InternalError("not yet implemented");
    case BOT_EQUIV:
      return builtin+"eq";
    case BOT_IMPL:
      return builtin+"lq";
    case BOT_RIMPL:
      return builtin+"gq";
    case BOT_OR:
      return builtin+"or";
    case BOT_AND:
      return builtin+"and";
    case BOT_XOR:
      return builtin+"xor";
    }
  }

  EE flat_exp(Env& env, BCtx bctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
    EE ret;
    if (e->_type.ispar() && !e->_type.isann()) {
      ret.b = bind(env,b,constants.lt);
      ret.r = bind(env,r,eval_par(env.ctx,e));
      return ret;
    }
    switch (e->_eid) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_SETLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
      throw InternalError("expression should be par");
      break;
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->_decl==NULL)
          throw FlatteningError(e->_loc, "undefined identifier");
        Env::Map::iterator it = env.map.find(id);
        VarDecl* vd;
        if (it==env.map.end()) {
          // New top-level id, need to copy into env.m
          vd = flat_exp(env,C_ROOT,id->_decl,NULL,constants.t).r
               ->cast<VarDecl>();
        } else {
          vd = it->second.r->cast<VarDecl>();
        }
        ret.b = bind(env,b,constants.lt);
        Expression* rete = NULL;
        if (vd->_e!=NULL) {
          switch (vd->_e->_eid) {
          case Expression::E_INTLIT:
          case Expression::E_FLOATLIT:
          case Expression::E_ID:
            rete = vd->_e;
            break;
          default: break;
          }
        }
        if (rete==NULL) {
          rete = Id::a(env.ctx,Location(),vd->_id,vd);
          rete->_type = id->_type;
        }
        ret.r = bind(env,r,rete);
      }
      break;
    case Expression::E_ANON:
      throw InternalError("anonymous variables not supported yet");
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        std::vector<EE> elems_ee(al->_v->size());
        for (unsigned int i=al->_v->size(); i--;)
          elems_ee[i] = flat_exp(env,bctx,(*al->_v)[i],NULL,NULL);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r;
        std::vector<std::pair<int,int> > dims(al->_dims->size());
        for (unsigned int i=al->_dims->size(); i--;)
          dims[i] = (*al->_dims)[i];
        ArrayLit* alr = ArrayLit::a(env.ctx,Location(),elems,dims);
        alr->_type = al->_type;
        ret.b = conj(env,b,elems_ee);
        ret.r = bind(env,r,alr);
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        bool parAccess=true;
        for (Expression* i : *aa->_idx) {
          if (!i->_type.ispar()) {
            parAccess = false;
            break;
          }
        }
        if (parAccess) {
          EE eev = flat_exp(env,+bctx,aa->_v,NULL,NULL);
          ArrayLit* al;
          if (eev.r->isa<ArrayLit>()) {
            al = eev.r->cast<ArrayLit>();
          } else {
            Id* id = eev.r->cast<Id>();
            if (id->_decl==NULL) {
              assert(false);
              throw InternalError("undefined identifier");
            }
            if (id->_decl->_e==NULL) {
              assert(false);
              throw InternalError("array without initialiser not supported");
            }
            al = id->_decl->_e->cast<ArrayLit>();
          }
          std::vector<IntVal> dims(aa->_idx->size());
          for (unsigned int i=aa->_idx->size(); i--;)
            dims[i] = eval_int(env.ctx,(*aa->_idx)[i]);
          Expression* val = eval_arrayaccess(env.ctx,al,dims);
          ret.b = bind(env,b,constants.lt);
          ret.r = bind(env,r,val);
        } else {
          assert(false);
          throw InternalError("not supported yet");
        }
      }
      break;
    case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        if (c->_set) {
          assert(false);
          throw InternalError("not supported yet");
        }
        class EvalF {
        public:
          Env& env;
          BCtx bctx;
          EvalF(Env& env0, BCtx bctx0) : env(env0), bctx(bctx0) {}
          typedef EE ArrayVal;
          EE e(ASTContext& ctx, Expression* e) {
            return flat_exp(env,bctx,e,NULL,NULL);
          }
        } _evalf(env,bctx);
        std::vector<EE> elems_ee = eval_comp<EvalF>(env.ctx,_evalf,c);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r;
        ArrayLit* alr = ArrayLit::a(env.ctx,Location(),elems);
        alr->_type = c->_type;
        ret.b = conj(env,b,elems_ee);
        ret.r = bind(env,r,alr);
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        bool done = false;
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(env.ctx,it.first)) {
            ret = flat_exp(env,bctx,it.second,r,b);
            done = true;
            break;
          }
        }
        if (!done) {
          ret = flat_exp(env,bctx,ite->_e_else,r,b);
        }
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        BCtx bctx0 = bctx;
        BCtx bctx1 = bctx;
        switch (bo->_op) {
        case BOT_PLUS:
        case BOT_MINUS:
        case BOT_MULT:
        case BOT_IDIV:
        case BOT_MOD:
        case BOT_DIV:
          {
            EE e0 = flat_exp(env,bctx0,bo->_e0,NULL,NULL);
            EE e1 = flat_exp(env,bctx1,bo->_e1,NULL,NULL);

            std::vector<Expression*> args(2);
            args[0] = e0.r; args[1] = e1.r;
            Call* cc = Call::a(env.ctx,Location(),opToBuiltin(bo),args);
            cc->_type = bo->_type;
            ret.r = bind(env,r,cc);

            std::vector<EE> ees(2);
            ees[0].b = e0.b; ees[1].b = e1.b;
            ret.b = conj(env,b,ees);
          }
          break;

        case BOT_AND:
          {
            if (bctx==C_ROOT && r==constants.t) {
              (void) flat_exp(env,C_ROOT,bo->_e0,constants.t,constants.t);
              (void) flat_exp(env,C_ROOT,bo->_e1,constants.t,constants.t);
              break;
            }
            // else fall through
          }
        case BOT_EQUIV:
        case BOT_IMPL:
        case BOT_RIMPL:
        case BOT_OR:
        case BOT_XOR:
        case BOT_LE:
        case BOT_LQ:
        case BOT_GR:
        case BOT_GQ:
        case BOT_EQ:
        case BOT_NQ:
        case BOT_IN:
        case BOT_SUBSET:
        case BOT_SUPERSET:
          {
            switch (bo->_op) {
            case BOT_XOR:
            case BOT_EQUIV:
              bctx0 = bctx1 = C_MIX;
              break;
            case BOT_IMPL:
              bctx0 = -bctx0;
              bctx1 = +bctx1;
              break;
            case BOT_RIMPL:
              bctx0 = +bctx0;
              bctx1 = -bctx1;
              break;
            case BOT_OR:
              bctx0 = +bctx0;
              bctx1 = +bctx1;
              break;
            default:
              break;
            }
            EE e0 = flat_exp(env,bctx0,bo->_e0,NULL,NULL);
            EE e1 = flat_exp(env,bctx1,bo->_e1,NULL,NULL);
            ret.b = bind(env,b,constants.lt);

            std::vector<Expression*> args(2);
            args[0] = e0.r; args[1] = e1.r;
            Call* cc = Call::a(env.ctx,Location(),opToBuiltin(bo),args);
            cc->_type = bo->_type;
            std::vector<EE> ees(3);
            ees[0].b = e0.b; ees[1].b = e1.b; ees[2].b = cc;
            ret.r = conj(env,r,ees);
          }
          break;

        case BOT_UNION:
        case BOT_DIFF:
        case BOT_SYMDIFF:
        case BOT_INTERSECT:

        case BOT_PLUSPLUS:
          assert(false);
          throw InternalError("not yet implemented");


        case BOT_DOTDOT:
          assert(false);
          throw InternalError("not yet implemented");
        }
      }
      break;
    case Expression::E_UNOP:
      assert(false);
      throw InternalError("not supported yet");
      break;
    case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        if (c->_decl == NULL) {
          ret.r = bind(env,r,e);
          ret.b = bind(env,b,constants.lt);
          break;
        }
          // throw FlatteningError(e->_loc,"undefined function or predicate");

        std::vector<EE> args_ee(c->_args->size());
        for (unsigned int i=c->_args->size(); i--;)
          args_ee[i] = flat_exp(env,bctx,(*c->_args)[i],NULL,NULL);
        std::vector<Expression*> args(args_ee.size());
        for (unsigned int i=args_ee.size(); i--;)
          args[i] = args_ee[i].r;
        Call* cr = Call::a(env.ctx,Location(),c->_id.str(),args);
        cr->_type = c->_type;
        Env::Map::iterator cit = env.map.find(cr);
        if (cit != env.map.end()) {
          ret.b = bind(env,b,cit->second.b);
          ret.r = bind(env,r,cit->second.r);
        } else {
          if (c->_decl->_e==NULL) {
            /// For now assume that all builtins are total
            if (cit != env.map.end()) {
              ret.b = bind(env,b,cit->second.b);
              ret.r = bind(env,r,cit->second.r);
            } else {
              if (c->_decl->_builtins.e) {
                Expression* callres = 
                  c->_decl->_builtins.e(env.ctx,cr->_args);
                EE res = flat_exp(env,bctx,callres,r,b);
                args_ee.push_back(res);
                ret.b = conj(env,b,args_ee);
                ret.r = bind(env,r,res.r);
                env.map.insert(cr,ret);
              } else {
                ret.b = conj(env,b,args_ee);
                ret.r = bind(env,r,cr);
                env.map.insert(cr,ret);
              }
            }
          } else {
            std::vector<std::pair<Id*,Expression*> > idmap;
            // Save mapping from Ids to VarDecls and set to parameters
            for (unsigned int i=c->_decl->_params->size(); i--;) {
              VarDecl* vd = (*c->_decl->_params)[i];
              Id* id = Id::a(env.ctx,Location(),vd->_id,NULL);
              id->_type = vd->_type;
              Env::Map::iterator idit = env.map.find(id);
              if (idit==env.map.end()) {
                EE ee(vd,NULL);
                idmap.push_back(std::pair<Id*,Expression*>(id,NULL));
                env.map.insert(id,ee);
              } else {
                idmap.push_back(
                  std::pair<Id*,Expression*>(id,idit->second.r));
                idit->second.r = vd;
              }
              vd->_e = args[i];
            }
            if (isTotal(c->_decl)) {
              EE ee = flat_exp(env,C_ROOT,c->_decl->_e,r,constants.t);
              ret.r = bind(env,r,ee.r);
              ret.b = conj(env,b,args_ee);
              env.map.insert(cr,ret);
            } else {
              ret = flat_exp(env,bctx,c->_decl->_e,r,NULL);
              args_ee.push_back(ret);
              ret.b = conj(env,b,args_ee);
              env.map.insert(cr,ret);
            }
            // Restore previous mapping
            for (std::pair<Id*,Expression*>& idvd : idmap) {
              Env::Map::iterator idit = env.map.find(idvd.first);
              assert(idit != env.map.end());
              if (idvd.second==NULL) {
                env.map.remove(idvd.first);
              } else {
                idit->second.r = idvd.second;
              }
            }
          }
        }
      }
      break;
    case Expression::E_VARDECL:
      {
        if (bctx != C_ROOT)
          throw FlatteningError(e->_loc, "not in root context");
        VarDecl* v = e->cast<VarDecl>();
        Id* id = Id::a(env.ctx,Location(),v->_id,NULL); /// TODO: avoid allocation
        id->_type = v->_type;
        Env::Map::iterator it = env.map.find(id);
        if (it==env.map.end()) {
          VarDecl* vd = VarDecl::a(env.ctx,Location(),
                                   eval_typeinst(env,v->_ti),
                                   v->_id.str());
          VarDeclI* nv = VarDeclI::a(env.ctx,Location(),vd);
          if (v->_e) {
            (void) flat_exp(env,
              v->_e->_type.isbool() ? C_MIX : C_ROOT,v->_e,vd,constants.t);
          }
          env.m->addItem(nv);
          
          EE ee(vd,NULL);
          env.map.insert(id,ee);
          ret.r = bind(env,r,vd);
        } else {
          ret.r = bind(env,r,it->second.r);
        }
        ret.b = bind(env,b,constants.lt);
      }
      break;
    case Expression::E_LET:
      {
        Let* let = e->cast<Let>();
        env.ctx.mark();
        std::vector<EE> cs;
        std::vector<std::pair<Id*,Expression*> > idmap;
        for (Expression* le : *let->_let) {
          if (VarDecl* vd = le->dyn_cast<VarDecl>()) {
            env.ctx.trail(vd);
            if (!vd->_e) {
              if (bctx==C_NEG || bctx==C_MIX)
                throw FlatteningError(vd->_loc,
                  "free variable in non-positive context");
              TypeInst* ti = copy(env.ctx,vd->_ti)->cast<TypeInst>();
              VarDecl* nvd = 
                VarDecl::a(env.ctx,Location(),ti,env.genId("FromLet"));
              VarDeclI* nv = VarDeclI::a(env.ctx,Location(),nvd);
              env.m->addItem(nv);
              Id* id = Id::a(env.ctx,Location(),nvd->_id,nvd);
              id->_type = e->_type;
              EE ee(nvd,NULL);
              env.map.insert(id,ee);
              vd->_e = id;

              id = Id::a(env.ctx,Location(),vd->_id,NULL);
              id->_type = e->_type;
              Env::Map::iterator it = env.map.find(id);
              if (it==env.map.end()) {
                idmap.push_back(std::pair<Id*,Expression*>(id,NULL));
                env.map.insert(id,ee);
              } else {
                idmap.push_back(std::pair<Id*,Expression*>(id,it->second.r));
                it->second.r = vd;
              }
            } else {
              EE ee = flat_exp(env,bctx,vd->_e,NULL,NULL);
              cs.push_back(ee);
              vd->_e = ee.r;
            }
          } else {
            EE ee = flat_exp(env,bctx,le,NULL,constants.t);
            ee.b = ee.r;
            cs.push_back(ee);
          }
        }
        EE ee = flat_exp(env,bctx,let->_in,NULL,NULL);
        if (let->_type.isbool()) {
          ee.b = ee.r;
          cs.push_back(ee);
          ret.r = conj(env,r,cs);
          ret.b = bind(env,b,constants.lt);
        } else {
          cs.push_back(ee);
          ret.r = bind(env,r,ee.r);
          ret.b = conj(env,b,cs);
        }
        env.ctx.untrail();
        // Restore previous mapping
        for (std::pair<Id*,Expression*>& idvd : idmap) {
          Env::Map::iterator idit = env.map.find(idvd.first);
          assert(idit != env.map.end());
          if (idvd.second==NULL) {
            env.map.remove(idvd.first);
          } else {
            idit->second.r = idvd.second;
          }
        }
      }
      break;
    case Expression::E_ANN:
      assert(false);
      throw InternalError("not supported yet");
      break;
    case Expression::E_TI:
      assert(false);
      throw InternalError("not supported yet");
      break;
    case Expression::E_TIID:
      assert(false);
      throw InternalError("not supported yet");
      break;
    }
    return ret;
  }


  Model* flatten(ASTContext& ctx, Model* m) {
    Model* flat = new Model;
    Env env(ctx,flat);
    
    class FV : public ItemVisitor {
    public:
      Env& env;
      FV(Env& env0) : env(env0) {}
      void vVarDeclI(VarDeclI* v) {
        if (v->_e->_type.isvar()) {
          (void) flat_exp(env,C_ROOT,v->_e,NULL,constants.t);
        }
      }
      void vConstraintI(ConstraintI* ci) {
        (void) flat_exp(env,C_ROOT,ci->_e,constants.t,constants.t);
      }
      void vSolveI(SolveI* si) {
        Annotation* ann = static_cast<Annotation*>(
          flat_exp(env,C_ROOT,si->_ann,NULL,constants.t).r);
        switch (si->_st) {
        case SolveI::ST_SAT:
          env.m->addItem(SolveI::sat(env.ctx,Location(),ann));
          break;
        case SolveI::ST_MIN:
          env.m->addItem(SolveI::min(env.ctx,Location(),
            flat_exp(env,C_ROOT,si->_e,NULL,constants.t).r,
            ann));
          break;
        case SolveI::ST_MAX:
          env.m->addItem(SolveI::max(env.ctx,Location(),
            flat_exp(env,C_ROOT,si->_e,NULL,constants.t).r,
            ann));
          break;
        }
      }
    } _fv(env);
    iterItems<FV>(_fv,m);
    
    return flat;
  }
  
}
