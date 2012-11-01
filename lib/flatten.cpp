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

  /// Check if \a e is NULL or true
  bool istrue(Expression* e) {
    return e==NULL || (e->isa<BoolLit>() && e->cast<BoolLit>()->_v);
  }  
  /// Check if \a e is non-NULL and false
  bool isfalse(Expression* e) {
    return e!=NULL && e->isa<BoolLit>() && !e->cast<BoolLit>()->_v;
  }  

  /// Result of evaluation
  class EE {
  public:
    /// The result value
    Expression* r;
    /// Boolean expression representing whether result is defined
    Expression* b;
    /// Constructor
    EE(Expression* r0, Expression* b0) : r(r0), b(b0) {}
  };

  class Constants {
  private:
    ASTContext ctx;
  public:
    VarDecl* t;
    VarDecl* f;
    Constants(void) {
      TypeInst* ti = TypeInst::a(ctx, Location(), Type::parbool());
      t = VarDecl::a(ctx, Location(), ti, "_bool_true",
                     BoolLit::a(ctx, Location(), true));
      f = VarDecl::a(ctx, Location(), ti, "_bool_false",
                     BoolLit::a(ctx, Location(), false));
    }
  } constants;

  class Env {
  public:
    ASTContext& ctx;
    Model* m;
    typedef ExpressionMap<Expression*> Map;
    Map map;
    unsigned int ids;
    Env(ASTContext& ctx0, Model* m0) : ctx(ctx0), m(m0), ids(0) {}
    CtxStringH genId(const std::string& s) {
      std::ostringstream oss; oss << "_" << s << "_" << ids++;
      return CtxStringH(ctx,oss.str());
    }
  };

  void bind(Env& env, VarDecl*& vd, Expression* e) {}

  TypeInst* eval_typeinst(Env& env, TypeInst* ti) {
    /// TODO: evaluate all par components in the domain. This probably
    ///       needs the VarDecl to compute the actual dimensions of
    ///       array[int] expressions
    return static_cast<TypeInst*>(eval_par(env.ctx,ti));
  }

  EE flat_exp(Env& env, BCtx bctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e->_type.ispar() && !e->_type.isann()) {
      bind(env,b,constants.t);
      bind(env,r,eval_par(env.ctx,e));
      return EE(r->_e,b->_e);
    }
    return EE(NULL,NULL);
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
          VarDecl* vd = VarDecl::a(env.ctx,Location(),
                                   eval_typeinst(env,v->_e->_ti),
                                   v->_e->_id.str());
          VarDeclI* nv = VarDeclI::a(env.ctx,Location(),vd);
          if (v->_e->_e)
            (void) flat_exp(env,C_ROOT,v->_e->_e,vd,constants.t);
          env.m->addItem(nv);
        }
      }
      void vConstraintI(ConstraintI* ci) {
        (void) flat_exp(env,C_ROOT,ci->_e,constants.t,constants.t);
      }
      void vSolveI(SolveI* si) {
        Annotation* ann = 
          static_cast<Annotation*>(flat_exp(env,C_ROOT,si->_ann,
                                            NULL,constants.t).r);
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
