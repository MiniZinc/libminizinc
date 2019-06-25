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

  CallArgItem::CallArgItem(EnvI& env0) : env(env0) {
    env.idStack.push_back(static_cast<int>(env.callStack.size()));
  }
  CallArgItem::~CallArgItem(void) {
    env.idStack.pop_back();
  }

  Expression* createDummyValue(EnvI& env, const Type& t) {
    if (t.dim()>0) {
      Expression* ret = new ArrayLit(Location().introduce(), std::vector<Expression*>());
      Type ret_t = t;
      ret_t.ti(Type::TI_PAR);
      ret->type(ret_t);
      return ret;
    }
    if (t.st()==Type::ST_SET) {
      Expression* ret = new SetLit(Location().introduce(), std::vector<Expression*>());
      Type ret_t = t;
      ret_t.ti(Type::TI_PAR);
      ret->type(ret_t);
      return ret;
    }
    switch (t.bt()) {
      case Type::BT_INT:
        return IntLit::a(0);
      case Type::BT_BOOL:
        return constants().boollit(false);
      case Type::BT_FLOAT:
        return FloatLit::a(0);
      case Type::BT_STRING:
        return new StringLit(Location().introduce(), "");
      case Type::BT_ANN:
        return constants().ann.promise_total;
      default:
        return NULL;
    }
  }

  EE flatten_error(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    throw InternalError("invalid expression encountered during compilation");
  }

#ifndef NDEBUG
  void mzn_break_here(Expression* e) {
    std::cerr << "% mzn_break_here: " << *e << "\n";
  }
#endif

  typedef EE (*ExprFlattener) (EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  
  EE flatten_setlit(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_id(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_anon(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_arraylit(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_arrayaccess(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_comp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_ite(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_binop(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_unop(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_call(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_vardecl(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_let(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_par(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  EE flatten_error(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  
  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
    
#ifndef NDEBUG
    Annotation& e_ann = e->ann();
    if(e_ann.contains(constants().ann.mzn_break_here))
      mzn_break_here(e);
#endif
    
    assert(!e->type().isunknown());
    
    static const ExprFlattener flattener_dispatch[] = {
      &flatten_par,         //      par expressions
      &flatten_error,       //      E_INTLIT
      &flatten_error,       //      E_FLOATLIT
      &flatten_setlit,      //      E_SETLIT
      &flatten_error,       //      E_BOOLLIT
      &flatten_error,       //      E_STRINGLIT
      &flatten_id,          //      E_ID
      &flatten_anon,        //      E_ANON
      &flatten_arraylit,    //      E_ARRAYLIT
      &flatten_arrayaccess, //      E_ARRAYACCESS
      &flatten_comp,        //      E_COMP
      &flatten_ite,         //      E_ITE
      &flatten_binop,       //      E_BINOP
      &flatten_unop,        //      E_UNOP
      &flatten_call,        //      E_CALL
      &flatten_vardecl,     //      E_VARDECL
      &flatten_let,         //      E_LET
      &flatten_error,       //      E_TI
      &flatten_error        //      E_TIID
    };
    
    int dispatch = (e->type().ispar() && !e->isa<Let>() && !e->isa<VarDecl>() && e->type().bt()!=Type::BT_ANN) ? 0 : e->eid()-Expression::E_INTLIT+1;
    
    return flattener_dispatch[dispatch](env, ctx, e, r, b);
    
  }

}
