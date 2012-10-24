/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/builtins.hh>
#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {
  
  void rb(ASTContext& ctx, const CtxStringH& id, const std::vector<Type>& t, 
          FunctionI::builtin_e b) {
    FunctionI* fi = ctx.matchFn(id,t);
    if (fi) {
      fi->_builtins.e = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(ASTContext& ctx, const CtxStringH& id, const std::vector<Type>& t, 
          FunctionI::builtin_f b) {
    FunctionI* fi = ctx.matchFn(id,t);
    if (fi) {
      fi->_builtins.f = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(ASTContext& ctx, const CtxStringH& id, const std::vector<Type>& t, 
          FunctionI::builtin_i b) {
    FunctionI* fi = ctx.matchFn(id,t);
    if (fi) {
      fi->_builtins.i = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }
  void rb(ASTContext& ctx, const CtxStringH& id, const std::vector<Type>& t, 
          FunctionI::builtin_b b) {
    FunctionI* fi = ctx.matchFn(id,t);
    if (fi) {
      fi->_builtins.b = b;
    } else {
      assert(false); // TODO: is this an error?
    }
  }

  IntVal b_min(ASTContext& ctx, CtxVec<Expression*>* args) {
    switch (args->size()) {
    case 1:
      if ((*args)[0]->_type.isset()) {
        throw EvalError((*args)[0]->_loc, "sets not supported");
      } else {
        ArrayLit* al = eval_array_lit(ctx, (*args)[0]);
        if (al->_v->size()==0)
          throw EvalError(al->_loc, "min on empty array undefined");
        IntVal m = eval_int(ctx,(*al->_v)[0]);
        for (unsigned int i=1; i<al->_v->size(); i++)
          m = std::min(m, eval_int(ctx,(*al->_v)[i]));
        return m;
      }
    case 2:
      {
        return std::min(eval_int(ctx, (*args)[0]),eval_int(ctx, (*args)[1]));
      }
    default:
      throw EvalError(Location(), "dynamic type error");
    }
  }

  void registerBuiltins(ASTContext& ctx) {
    
    std::vector<Type> t_intint(2);
    t_intint[0] = Type::parint();
    t_intint[1] = Type::parint();

    std::vector<Type> t_intarray(1);
    t_intarray[0] = Type::parint(-1);
    
    rb(ctx, CtxStringH(ctx,"min"), t_intint, b_min);
    rb(ctx, CtxStringH(ctx,"min"), t_intarray, b_min);
  }
  
}


