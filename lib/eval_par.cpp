/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {

  Expression* eval_id(ASTContext& ctx, Id* e) { return NULL; }
  
  Expression* eval_arrayaccess(ASTContext& ctx, ArrayAccess* e) {
    return NULL;
  }
  
  Expression* eval_call(ASTContext& ctx, Call* e) { return NULL; }
  
  Expression* eval_let(ASTContext& ctx, Let* e) { return NULL; }

  FloatVal eval_float(ASTContext& ctx, Expression* e) { return 0.0; }
  
  class IntSetVal {
  public:
    bool contains(const IntVal& v) const { return false; }
  };
  IntSetVal eval_intset(ASTContext& ctx, Expression* e) {
    return IntSetVal();
  }

  bool eval_bool(ASTContext& ctx, Expression* e) {
    switch (e->_eid) {
    case Expression::E_BOOLLIT: return e->template cast<BoolLit>()->_v;
    case Expression::E_INTLIT: 
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_SETLIT:
    case Expression::E_ARRAYLIT:
    case Expression::E_COMP:
    case Expression::E_VARDECL:
    case Expression::E_ANN:
    case Expression::E_TI:
      throw EvalError(e->_loc,"not a bool expression");
      break;
    case Expression::E_ID:
      return eval_bool(ctx,eval_id(ctx,e->template cast<Id>()));
      break;
    case Expression::E_ARRAYACCESS:
      return eval_bool(ctx,eval_arrayaccess(ctx,
        e->template cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->template cast<ITE>();
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(ctx,it.first))
            return eval_bool(ctx,it.second);
        }
        return eval_bool(ctx,ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->template cast<BinOp>();
        if (bo->_e0->_type.isbool() && bo->_e1->_type.isbool()) {
          bool v0 = eval_bool(ctx,bo->_e0);
          bool v1 = eval_bool(ctx,bo->_e1);
          switch (bo->_op) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          case BOT_IN: assert(false && "not implemented"); /// TODO
          case BOT_SUBSET: assert(false && "not implemented"); /// TODO
          case BOT_SUPERSET: assert(false && "not implemented"); /// TODO
          case BOT_EQUIV: return v0==v1;
          case BOT_IMPL: return (!v0)||v1;
          case BOT_RIMPL: return (!v1)||v0;
          case BOT_OR: return v0||v1;
          case BOT_AND: return v0&&v1;
          case BOT_XOR: return v0^v1;
          default: throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isint()) {
          IntVal v0 = eval_int(ctx,bo->_e0);
          IntVal v1 = eval_int(ctx,bo->_e1);
          switch (bo->_op) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          default: throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isfloat() && bo->_e1->_type.isfloat()) {
          FloatVal v0 = eval_float(ctx,bo->_e0);
          FloatVal v1 = eval_float(ctx,bo->_e1);
          switch (bo->_op) {
          case BOT_LE: return v0<v1;
          case BOT_LQ: return v0<=v1;
          case BOT_GR: return v0>v1;
          case BOT_GQ: return v0>=v1;
          case BOT_EQ: return v0==v1;
          case BOT_NQ: return v0!=v1;
          default: throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isint() && bo->_e1->_type.isintset()) {
          IntVal v0 = eval_int(ctx,bo->_e0);
          IntSetVal v1 = eval_intset(ctx,bo->_e1);
          switch (bo->_op) {
          case BOT_IN: return v1.contains(v0);
          default: throw EvalError(e->_loc,"not a bool expression");
          }
        } else if (bo->_e0->_type.isset() && bo->_e1->_type.isset()) {
          assert(false && "not implemented"); /// TODO
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* uo = e->template cast<UnOp>();
        bool v0 = eval_bool(ctx,uo->_e0);
        switch (uo->_op) {
        case UOT_NOT: return !v0;
        default: throw EvalError(e->_loc,"not a bool expression");
        }
      }
      break;
    case Expression::E_CALL:
      return eval_bool(ctx,eval_call(ctx,e->template cast<Call>()));
      break;
    case Expression::E_LET:
      return eval_bool(ctx,eval_let(ctx,e->template cast<Let>()));
      break;
    default:
      assert(false);
      break;
    }
  }

  IntVal eval_int(ASTContext& ctx, Expression* e) {
    switch (e->_eid) {
    case Expression::E_INTLIT: return e->template cast<IntLit>()->_v;
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_SETLIT:
    case Expression::E_ARRAYLIT:
    case Expression::E_COMP:
    case Expression::E_VARDECL:
    case Expression::E_ANN:
    case Expression::E_TI:
      throw EvalError(e->_loc,"not an integer expression");
      break;
    case Expression::E_ID:
      return eval_int(ctx,eval_id(ctx,e->template cast<Id>()));
      break;
    case Expression::E_ARRAYACCESS:
      return eval_int(ctx,eval_arrayaccess(ctx,
        e->template cast<ArrayAccess>()));
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->template cast<ITE>();
        for (ITE::IfThen& it : *ite->_e_if) {
          if (eval_bool(ctx,it.first))
            return eval_int(ctx,it.second);
        }
        return eval_int(ctx,ite->_e_else);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->template cast<BinOp>();
        IntVal v0 = eval_int(ctx,bo->_e0);
        IntVal v1 = eval_int(ctx,bo->_e1);
        switch (bo->_op) {
        case BOT_PLUS: return v0+v1;
        case BOT_MINUS: return v0-v1;
        case BOT_MULT: return v0*v1;
        case BOT_IDIV: return v0 / v1;
        case BOT_MOD: return v0 % v1;
        default: throw EvalError(e->_loc,"not an integer expression");
        }
      }
      break;
    case Expression::E_UNOP:
      {
        UnOp* uo = e->template cast<UnOp>();
        IntVal v0 = eval_int(ctx,uo->_e0);
        switch (uo->_op) {
        case UOT_PLUS: return v0;
        case UOT_MINUS: return -v0;
        default: throw EvalError(e->_loc,"not an integer expression");
        }
      }
      break;
    case Expression::E_CALL:
      return eval_int(ctx,eval_call(ctx,e->template cast<Call>()));
      break;
    case Expression::E_LET:
      return eval_int(ctx,eval_let(ctx,e->template cast<Let>()));
      break;
    default:
      assert(false);
      break;
    }
  }
}
