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

  Expression* bind(Env& env, VarDecl* vd, Expression* e) {
    if (vd==constants.t) {
      if (!istrue(env.ctx,e)) {
        env.m->addItem(ConstraintI::a(env.ctx,Location(),e));
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
          return Id::a(env.ctx,Location(),vd->_id,vd);
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
      BinOp* ret = BinOp::a(env.ctx,Location(),
                            nontrue[0],BOT_AND,nontrue[1]);
      for (unsigned int i=2; i<nontrue.size(); i++) {
        ret = BinOp::a(env.ctx,Location(),ret,BOT_AND,nontrue[i]);
      }
      return bind(env,b,ret);
    }
  }

  TypeInst* eval_typeinst(Env& env, TypeInst* ti) {
    /// TODO: evaluate all par components in the domain. This probably
    ///       needs the VarDecl to compute the actual dimensions of
    ///       array[int] expressions
    return eval_par(env.ctx,ti)->cast<TypeInst>();
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
        if (vd->_e)
          ret.r = bind(env,r,vd->_e);
        else
          ret.r = bind(env,r,Id::a(env.ctx,Location(),vd->_id,vd));
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
        ArrayLit* alr = ArrayLit::a(env.ctx,Location(),elems);
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
      ret.b = bind(env,b,constants.lt);
      ret.r = bind(env,r,copy(env.ctx,e));
      break;
    case Expression::E_UNOP:
      assert(false);
      throw InternalError("not supported yet");
      break;
    case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        if (c->_decl == NULL)
          throw FlatteningError(e->_loc,"undefined function or predicate");
        if (c->_decl->_e==NULL) {
          /// For now assume that all builtins are total
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
            if (c->_decl->_builtins.e) {
              EE res = flat_exp(env,bctx,
                                c->_decl->_builtins.e(env.ctx,cr->_args),r,b);
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
          throw InternalError("function with body not supported yet");
        }
      }
      break;
    case Expression::E_VARDECL:
      {
        if (bctx != C_ROOT)
          throw FlatteningError(e->_loc, "not in root context");
        VarDecl* v = e->cast<VarDecl>();
        Id* id = Id::a(env.ctx,Location(),v->_id,NULL); /// TODO: avoid allocation
        Env::Map::iterator it = env.map.find(id);
        if (it==env.map.end()) {
          VarDecl* vd = VarDecl::a(env.ctx,Location(),
                                   eval_typeinst(env,v->_ti),
                                   v->_id.str());
          VarDeclI* nv = VarDeclI::a(env.ctx,Location(),vd);
          if (v->_e)
            (void) flat_exp(env,C_ROOT,v->_e,vd,constants.t);
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
      assert(false);
      throw InternalError("not supported yet");
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
