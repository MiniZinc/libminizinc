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

  /// Placeholder for Boolean return value
  class NB {
  protected:
    Model* _flat;
    VarDecl* _nb;
    bool _universal;
    NB(Model* flat, VarDecl* nb, bool universal)
      : _flat(flat), _nb(nb), _universal(universal) {}
  public:
    static NB u(Model* flat) { return NB(flat,NULL,true); }
    static NB b(Model* flat, VarDecl* b=NULL) { return NB(flat,b,false); }

    Expression* eq(ASTContext& ctx, Expression* e) const {
      if (_universal) {
        if (!istrue(e))
          _flat->addItem(ConstraintI::a(ctx,Location(),e));
        return NULL;
      } else {
        if (_nb) {
          Id* id = Id::a(ctx,Location(),_nb->_id.str(),_nb);
          _flat->addItem(ConstraintI::a(ctx, Location(),
            BinOp::a(ctx,Location(),
              id,
              BOT_EQUIV,
              e
            )));
          return id;
        } else if (e->isa<Id>()) {
          return e;
        } else if (e->isa<BoolLit>()) {
          return e;
        } else {
          // string id = fenv._flat->newid("BOOL");
          // VardeclExpr* ve = new VardeclExpr(Location(),
          //     TiExpr::var(Location(),new BoolTiExpr()),
          //     id);
          // const ExprType* lt = typecheck(fenv._tm,ve);
          // fenv._tm.add(ve->_id,lt,ve);
          // fenv._flat->addItem(new VardeclItem(Location(),ve));
          // fenv._flat->addItem(new ConstraintItem(Location(),
          //   new BinOpExpr(Location(),
          //     new Identifier(Location(),id,ve),
          //     BinOpExpr::BOT_EQUIV,
          //     e
          //   )));
          // return new Identifier(Location(),id,ve);
          return NULL;
        }
      }
    }
    Expression* nq(ASTContext& ctx, Expression* e) const {
      if (_universal) {
        if (!isfalse(e)) {
          _flat->addItem(ConstraintI::a(ctx,Location(),
            BinOp::a(ctx,Location(),
              BoolLit::a(ctx,Location(),false),
              BOT_EQUIV,
              e
            )));
        }
        return NULL;
      } else {
        return NULL;
        // Identifier* ie;
        // if (e->eid()==Expression::E_ID) {
        //   ie = static_cast<Identifier*>(e);
        // } else {
        //   string id = fenv._flat->newid("BOOL");
        //   VardeclExpr* ve = new VardeclExpr(Location(),
        //       TiExpr::var(Location(),new BoolTiExpr()),
        //       id);
        //   const ExprType* lt = typecheck(fenv._tm,ve);
        //   fenv._tm.add(ve->_id,lt,ve);
        //   fenv._flat->addItem(new VardeclItem(Location(),ve));
        //   fenv._flat->addItem(new ConstraintItem(Location(),
        //     new BinOpExpr(Location(),
        //       new Identifier(Location(),id,ve),
        //       BinOpExpr::BOT_EQUIV,
        //       e
        //     )));
        //   ie = new Identifier(Location(),id,ve);
        // }
        // VardeclExpr* nb = _nb;
        // if (nb==NULL) {
        //   string id = fenv._flat->newid("BOOL");
        //   nb = new VardeclExpr(Location(),
        //     TiExpr::var(Location(),new BoolTiExpr()),id);
        //   const ExprType* lt = typecheck(fenv._tm,nb);
        //   fenv._tm.add(nb->_id,lt,nb);
        // }
        // vector<Expression*> args(2);
        // args[0] = ie;
        // args[1] = new Identifier(Location(),nb->_id,nb);
        // fenv._flat->addItem(new ConstraintItem(Location(),
        //   new CallExpr(Location(),"bool_not",args)
        // ));
        // return new Identifier(Location(),nb->_id,nb);
      }
    }
  };  

  Expression* flatc(ASTContext& ctx, NB nb, BCtx bctx, Expression* e) {
    if (e==NULL)
      return NULL;
    if (e->_type.ispar()) {
      return nb.eq(ctx,BoolLit::a(ctx,Location(),eval_bool(ctx,e)));
    }
  }

  Model* flatten(ASTContext& ctx, Model* m) {
    Model* flat = new Model;
    
    class FV : public ItemVisitor {
    public:
      ASTContext& ctx;
      Model* flat;
      FV(ASTContext& ctx0, Model* flat0) : ctx(ctx0), flat(flat0) {}
      void vVarDeclI(VarDeclI*) {}
      void vConstraintI(ConstraintI* ci) {
        if (Expression* b = flatc(ctx,NB::u(flat),C_ROOT,ci->_e))
          flat->addItem(ConstraintI::a(ctx,ci->_loc,b));
      }
      void vSolveI(SolveI*) {}
    } _fv(ctx,flat);
    iterItems<FV>(_fv,m);
    
    return flat;
  }
  
}
