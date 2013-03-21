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
  void rb(ASTContext& ctx, const CtxStringH& id, const std::vector<Type>& t, 
          FunctionI::builtin_s b) {
    FunctionI* fi = ctx.matchFn(id,t);
    if (fi) {
      fi->_builtins.s = b;
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

  IntVal b_max(ASTContext& ctx, CtxVec<Expression*>* args) {
    switch (args->size()) {
    case 1:
      if ((*args)[0]->_type.isset()) {
        throw EvalError((*args)[0]->_loc, "sets not supported");
      } else {
        ArrayLit* al = eval_array_lit(ctx, (*args)[0]);
        if (al->_v->size()==0)
          throw EvalError(al->_loc, "max on empty array undefined");
        IntVal m = eval_int(ctx,(*al->_v)[0]);
        for (unsigned int i=1; i<al->_v->size(); i++)
          m = std::max(m, eval_int(ctx,(*al->_v)[i]));
        return m;
      }
    case 2:
      {
        return std::max(eval_int(ctx, (*args)[0]),eval_int(ctx, (*args)[1]));
      }
    default:
      throw EvalError(Location(), "dynamic type error");
    }
  }

  IntVal b_sum(ASTContext& ctx, CtxVec<Expression*>* args) {
    assert(args->size()==1);
    ArrayLit* al = eval_array_lit(ctx, (*args)[0]);
    if (al->_v->size()==0)
      return 0;
    IntVal m = 0;
    for (unsigned int i=0; i<al->_v->size(); i++)
      m += eval_int(ctx,(*al->_v)[i]);
    return m;
  }

  IntSetVal* b_index_set(ASTContext& ctx, CtxVec<Expression*>* args, int i) {
    if (args->size() != 1)
      throw EvalError(Location(), "index_set needs exactly one argument");
    if ((*args)[0]->_eid != Expression::E_ID)
      throw EvalError(Location(), "index_set only supported for identifiers");
    Id* id = (*args)[0]->template cast<Id>();
    if (id->_decl == NULL)
      throw EvalError(id->_loc, "undefined identifier");
    if (id->_decl->_ti->_ranges->size() < i)
      throw EvalError(id->_loc, "index_set: wrong dimension");
    if ((*id->_decl->_ti->_ranges)[i-1]->_domain == NULL) {
      ArrayLit* al = eval_array_lit(ctx, id);
      if (al->_dims->size() < i)
        throw EvalError(id->_loc, "index_set: wrong dimension");
      return IntSetVal::a(ctx,(*al->_dims)[i-1].first,
                              (*al->_dims)[i-1].second);
    }
    return eval_intset(ctx, (*id->_decl->_ti->_ranges)[i-1]->_domain);
  }
  IntSetVal* b_index_set1(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_index_set(ctx,args,1);
  }
  IntSetVal* b_index_set2(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_index_set(ctx,args,2);
  }
  IntSetVal* b_index_set3(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_index_set(ctx,args,3);
  }
  IntSetVal* b_index_set4(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_index_set(ctx,args,4);
  }
  IntSetVal* b_index_set5(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_index_set(ctx,args,5);
  }
  IntSetVal* b_index_set6(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_index_set(ctx,args,6);
  }

  IntVal b_min_parsetint(ASTContext& ctx, CtxVec<Expression*>* args) {
    assert(args->size() == 1);
    IntSetVal* isv = eval_intset(ctx,(*args)[0]);
    return isv->min(0);
  }
  IntVal b_max_parsetint(ASTContext& ctx, CtxVec<Expression*>* args) {
    assert(args->size() == 1);
    IntSetVal* isv = eval_intset(ctx,(*args)[0]);
    return isv->max(isv->size()-1);
  }

  IntSetVal* b_ub_set(ASTContext& ctx, CtxVec<Expression*>* args) {
    assert(args->size() == 1);
    Expression* e = (*args)[0];
    for (;;) {
      switch (e->_eid) {
      case Expression::E_SETLIT: return eval_intset(ctx,e);
      case Expression::E_ID:
        {
          Id* id = e->cast<Id>();
          if (id->_decl==NULL)
            throw EvalError(id->_loc,"undefined identifier");
          if (id->_decl->_e==NULL)
            return eval_intset(ctx,id->_decl->_ti->_domain);
          else
            e = id->_decl->_e;
        }
        break;
      default:
        throw EvalError(e->_loc,"invalid argument to ub");
      }
    }
  }

  IntSetVal* b_dom_varint(ASTContext& ctx, Expression* e) {
    for (;;) {
      switch (e->_eid) {
      case Expression::E_INTLIT:
        {
          IntVal v = e->cast<IntLit>()->_v;
          return IntSetVal::a(ctx,v,v);
        }
      case Expression::E_ID:
        {
          Id* id = e->cast<Id>();
          if (id->_decl==NULL)
            throw EvalError(id->_loc,"undefined identifier");
          if (id->_decl->_e==NULL)
            return eval_intset(ctx,id->_decl->_ti->_domain);
          else
            e = id->_decl->_e;
        }
        break;
      default:
        throw EvalError(e->_loc,"invalid argument to dom");
      }
    }
  }
  IntSetVal* b_dom_varint(ASTContext& ctx, CtxVec<Expression*>* args) {
    assert(args->size() == 1);
    return b_dom_varint(ctx,(*args)[0]);
  }

  IntSetVal* b_dom_array(ASTContext& ctx, CtxVec<Expression*>* args) {
    assert(args->size() == 1);
    Expression* ae = (*args)[0];
    ArrayLit* al = NULL;
    while (al==NULL) {
      switch (ae->_eid) {
      case Expression::E_ARRAYLIT:
        al = ae->cast<ArrayLit>();
        break;
      case Expression::E_ID:
        {
          Id* id = ae->cast<Id>();
          if (id->_decl==NULL)
            throw EvalError(id->_loc,"undefined identifier");
          if (id->_decl->_e==NULL)
            throw EvalError(id->_loc,"array without initialiser");
          else
            ae = id->_decl->_e;
        }
        break;
      default:
        throw EvalError(ae->_loc,"invalid argument to ub");
      }
    }
    if (al->_v->size()==0)
      return IntSetVal::a(ctx);
    IntSetVal* isv = b_dom_varint(ctx,(*al->_v)[0]);
    for (unsigned int i=1; i<al->_v->size(); i++) {
      IntSetRanges isr(isv);
      IntSetRanges r(b_dom_varint(ctx,(*al->_v)[i]));
      Ranges::Union<IntSetRanges,IntSetRanges> u(isr,r);
      isv = IntSetVal::ai(ctx,u);
    }
    return isv;
  }

  ArrayLit* b_arrayXd(ASTContext& ctx, CtxVec<Expression*>* args, int d) {
    ArrayLit* al = eval_array_lit(ctx, (*args)[d]);
    std::vector<std::pair<int,int> > dims(d);
    unsigned int dim1d = 1;
    for (int i=0; i<d; i++) {
      IntSetVal* di = eval_intset(ctx, (*args)[i]);
      if (di->size() != 1)
        throw EvalError((*args)[i]->_loc, "arrayXd only defined for ranges");
      dims[i] = std::pair<int,int>(di->min(0),di->max(0));
      dim1d *= dims[i].second-dims[i].first+1;
    }
    if (dim1d != al->_v->size())
      throw EvalError(al->_loc, "mismatch in array dimensions");
    ArrayLit* ret = ArrayLit::a(ctx, al->_loc, al->_v, dims);
    ret->_type = al->_type;
    return ret;
  }
  Expression* b_array1d(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_arrayXd(ctx,args,1);
  }
  Expression* b_array2d(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_arrayXd(ctx,args,2);
  }
  Expression* b_array3d(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_arrayXd(ctx,args,3);
  }
  Expression* b_array4d(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_arrayXd(ctx,args,4);
  }
  Expression* b_array5d(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_arrayXd(ctx,args,5);
  }
  Expression* b_array6d(ASTContext& ctx, CtxVec<Expression*>* args) {
    return b_arrayXd(ctx,args,6);
  }

  IntVal b_length(ASTContext& ctx, CtxVec<Expression*>* args) {
    ArrayLit* al = eval_array_lit(ctx, (*args)[0]);
    return al->_v->size();
  }
  
  IntVal b_bool2int(ASTContext& ctx, CtxVec<Expression*>* args) {
    return eval_bool(ctx, (*args)[0]) ? 1 : 0;
  }

  bool b_forall_par(ASTContext& ctx, CtxVec<Expression*>* args) {
    if (args->size()!=1)
      throw EvalError(Location(), "forall needs exactly one argument");
    ArrayLit* al = eval_array_lit(ctx,(*args)[0]);
    for (unsigned int i=al->_v->size(); i--;)
      if (!eval_bool(ctx,(*al->_v)[i]))
        return false;
    return true;
  }
  Expression* b_forall_var(ASTContext& ctx, CtxVec<Expression*>* args) {
    if (args->size()!=1)
      throw EvalError(Location(), "forall needs exactly one argument");
    ArrayLit* al = eval_array_lit(ctx,(*args)[0]);
    if (al->_v->size() == 0) {
      return BoolLit::a(ctx,Location(),true);
    } else {
      Expression* r = (*al->_v)[0];
      for (unsigned int i=1; i<al->_v->size(); i++) {
        r = BinOp::a(ctx,Location(),r,BOT_AND,(*al->_v)[i]);
        r->_type = Type::varbool();
      }
      return r;
    }
  }
  bool b_exists_par(ASTContext& ctx, CtxVec<Expression*>* args) {
    if (args->size()!=1)
      throw EvalError(Location(), "exists needs exactly one argument");
    ArrayLit* al = eval_array_lit(ctx,(*args)[0]);
    for (unsigned int i=al->_v->size(); i--;)
      if (eval_bool(ctx,(*al->_v)[i]))
        return true;
    return false;
  }
  Expression* b_exists_var(ASTContext& ctx, CtxVec<Expression*>* args) {
    if (args->size()!=1)
      throw EvalError(Location(), "exists needs exactly one argument");
    ArrayLit* al = eval_array_lit(ctx,(*args)[0]);
    if (al->_v->size() == 0) {
      return BoolLit::a(ctx,Location(),false);
    } else {
      Expression* r = (*al->_v)[0];
      for (unsigned int i=1; i<al->_v->size(); i++) {
        r = BinOp::a(ctx,Location(),r,BOT_OR,(*al->_v)[i]);
        r->_type = Type::varbool();
      }
      return r;
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
    rb(ctx, CtxStringH(ctx,"max"), t_intarray, b_max);
    rb(ctx, CtxStringH(ctx,"sum"), t_intarray, b_sum);

    {
      std::vector<Type> t_anyarray1(1);
      t_anyarray1[0] = Type::any(1);
      rb(ctx, CtxStringH(ctx,"index_set"), t_anyarray1, b_index_set1);
    }
    {
      std::vector<Type> t_anyarray2(1);
      t_anyarray2[0] = Type::any(2);
      rb(ctx, CtxStringH(ctx,"index_set_1of2"), t_anyarray2, b_index_set1);
      rb(ctx, CtxStringH(ctx,"index_set_2of2"), t_anyarray2, b_index_set2);
    }
    {
      std::vector<Type> t_anyarray3(1);
      t_anyarray3[0] = Type::any(3);
      rb(ctx, CtxStringH(ctx,"index_set_1of3"), t_anyarray3, b_index_set1);
      rb(ctx, CtxStringH(ctx,"index_set_2of3"), t_anyarray3, b_index_set2);
      rb(ctx, CtxStringH(ctx,"index_set_3of3"), t_anyarray3, b_index_set3);
    }
    {
      std::vector<Type> t_anyarray4(1);
      t_anyarray4[0] = Type::any(4);
      rb(ctx, CtxStringH(ctx,"index_set_1of4"), t_anyarray4, b_index_set1);
      rb(ctx, CtxStringH(ctx,"index_set_2of4"), t_anyarray4, b_index_set2);
      rb(ctx, CtxStringH(ctx,"index_set_3of4"), t_anyarray4, b_index_set3);
      rb(ctx, CtxStringH(ctx,"index_set_4of4"), t_anyarray4, b_index_set4);
    }
    {
      std::vector<Type> t_anyarray5(1);
      t_anyarray5[0] = Type::any(5);
      rb(ctx, CtxStringH(ctx,"index_set_1of5"), t_anyarray5, b_index_set1);
      rb(ctx, CtxStringH(ctx,"index_set_2of5"), t_anyarray5, b_index_set2);
      rb(ctx, CtxStringH(ctx,"index_set_3of5"), t_anyarray5, b_index_set3);
      rb(ctx, CtxStringH(ctx,"index_set_4of5"), t_anyarray5, b_index_set4);
      rb(ctx, CtxStringH(ctx,"index_set_5of5"), t_anyarray5, b_index_set5);
    }
    {
      std::vector<Type> t_anyarray6(1);
      t_anyarray6[0] = Type::any(6);
      rb(ctx, CtxStringH(ctx,"index_set_1of6"), t_anyarray6, b_index_set1);
      rb(ctx, CtxStringH(ctx,"index_set_2of6"), t_anyarray6, b_index_set2);
      rb(ctx, CtxStringH(ctx,"index_set_3of6"), t_anyarray6, b_index_set3);
      rb(ctx, CtxStringH(ctx,"index_set_4of6"), t_anyarray6, b_index_set4);
      rb(ctx, CtxStringH(ctx,"index_set_5of6"), t_anyarray6, b_index_set5);
      rb(ctx, CtxStringH(ctx,"index_set_6of6"), t_anyarray6, b_index_set6);
    }
    {
      std::vector<Type> t_arrayXd(2);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"array1d"), t_arrayXd, b_array1d);
    }
    {
      std::vector<Type> t_arrayXd(3);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"array2d"), t_arrayXd, b_array2d);
    }
    {
      std::vector<Type> t_arrayXd(4);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"array3d"), t_arrayXd, b_array3d);
    }
    {
      std::vector<Type> t_arrayXd(5);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"array4d"), t_arrayXd, b_array4d);
    }
    {
      std::vector<Type> t_arrayXd(6);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::parsetint();
      t_arrayXd[5] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"array5d"), t_arrayXd, b_array5d);
    }
    {
      std::vector<Type> t_arrayXd(7);
      t_arrayXd[0] = Type::parsetint();
      t_arrayXd[1] = Type::parsetint();
      t_arrayXd[2] = Type::parsetint();
      t_arrayXd[3] = Type::parsetint();
      t_arrayXd[4] = Type::parsetint();
      t_arrayXd[5] = Type::parsetint();
      t_arrayXd[6] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"array6d"), t_arrayXd, b_array6d);
    }
    {
      std::vector<Type> t_length(1);
      t_length[0] = Type::any(-1);
      rb(ctx, CtxStringH(ctx,"length"), t_length, b_length);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool();
      rb(ctx, CtxStringH(ctx,"bool2int"), t, b_bool2int);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varbool(-1);
      rb(ctx, CtxStringH(ctx,"forall"), t, b_forall_var);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool(-1);
      rb(ctx, CtxStringH(ctx,"forall"), t, b_forall_par);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varbool(-1);
      rb(ctx, CtxStringH(ctx,"exists"), t, b_exists_var);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parbool(-1);
      rb(ctx, CtxStringH(ctx,"exists"), t, b_exists_par);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varsetint();
      rb(ctx, CtxStringH(ctx,"ub"), t, b_ub_set);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint();
      rb(ctx, CtxStringH(ctx,"dom"), t, b_dom_varint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::varint(-1);
      rb(ctx, CtxStringH(ctx,"dom_array"), t, b_dom_array);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(ctx, CtxStringH(ctx,"min"), t, b_min_parsetint);
    }
    {
      std::vector<Type> t(1);
      t[0] = Type::parsetint();
      rb(ctx, CtxStringH(ctx,"max"), t, b_max_parsetint);
    }
  }
  
}


