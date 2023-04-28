/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten_internal.hh>

#include <vector>

namespace MiniZinc {

CallArgItem::CallArgItem(EnvI& env0) : env(env0) {
  env.idStack.push_back(static_cast<int>(env.callStack.size()));
}
CallArgItem::~CallArgItem() { env.idStack.pop_back(); }

Expression* create_dummy_value(EnvI& env, const Type& t) {
  Type ret_t = t;
  ret_t.mkPar(env);
  if (t.dim() > 0) {
    Expression* ret = new ArrayLit(Location().introduce(), std::vector<Expression*>());
    Expression::type(ret, ret_t);
    return ret;
  }
  if (t.istuple() || t.isrecord()) {
    StructType* st = env.getStructType(t);
    std::vector<Expression*> fields(st->size());
    for (size_t i = 0; i < st->size(); ++i) {
      fields[i] = create_dummy_value(env, (*st)[i]);
    }
    Expression* ret = ArrayLit::constructTuple(Location().introduce(), fields);
    Expression::type(ret, ret_t);
    return ret;
  }
  if (t.st() == Type::ST_SET) {
    Expression* ret = new SetLit(Location().introduce(), std::vector<Expression*>());
    Expression::type(ret, ret_t);
    return ret;
  }
  if (t.ot() == Type::OT_OPTIONAL) {
    return env.constants.absent;
  }
  switch (t.bt()) {
    case Type::BT_INT:
      return IntLit::a(0);
    case Type::BT_BOOL:
      return env.constants.literalFalse;
    case Type::BT_FLOAT:
      return FloatLit::a(0);
    case Type::BT_STRING:
      return new StringLit(Location().introduce(), "");
    case Type::BT_ANN:
      return env.constants.ann.promise_total;
    default:
      return nullptr;
  }
}

EE flatten_error(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  throw InternalError("invalid expression encountered during compilation");
}

#ifndef NDEBUG
void mzn_break_here(Expression* e) { std::cerr << "% mzn_break_here: " << *e << "\n"; }
#endif

typedef EE (*ExprFlattener)(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);

EE flatten_setlit(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_anon(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_arraylit(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_arrayaccess(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_fieldaccess(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_comp(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_ite(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_binop(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_unop(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_call(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_vardecl(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_let(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_par(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);

EE flat_exp(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  assert(ctx.b != C_ROOT || b == env.constants.varTrue);

  if (e == nullptr) {
    return EE();
  }

#ifndef NDEBUG
  Annotation& e_ann = Expression::ann(e);
  if (e_ann.contains(env.constants.ann.mzn_break_here)) {
    mzn_break_here(e);
  }
#endif

  assert(!Expression::type(e).isunknown());

  static const ExprFlattener flattener_dispatch[] = {
      &flatten_par,          //      par expressions
      &flatten_error,        //      E_INTLIT
      &flatten_error,        //      E_FLOATLIT
      &flatten_setlit,       //      E_SETLIT
      &flatten_error,        //      E_BOOLLIT
      &flatten_error,        //      E_STRINGLIT
      &flatten_id,           //      E_ID
      &flatten_anon,         //      E_ANON
      &flatten_arraylit,     //      E_ARRAYLIT
      &flatten_arrayaccess,  //      E_ARRAYACCESS
      &flatten_fieldaccess,  //      E_ARRAYACCESS
      &flatten_comp,         //      E_COMP
      &flatten_ite,          //      E_ITE
      &flatten_binop,        //      E_BINOP
      &flatten_unop,         //      E_UNOP
      &flatten_call,         //      E_CALL
      &flatten_vardecl,      //      E_VARDECL
      &flatten_let,          //      E_LET
      &flatten_error,        //      E_TI
      &flatten_error         //      E_TIID
  };

  bool is_par = Expression::type(e).isPar() &&
                (!Expression::type(e).cv() || !Expression::type(e).isbool() || ctx.b != C_ROOT ||
                 Expression::isa<BoolLit>(e)) &&
                !Expression::isa<Let>(e) && !Expression::isa<VarDecl>(e) &&
                Expression::type(e).bt() != Type::BT_ANN;

#ifdef OUTPUT_CALLTREE
  if (auto* call = e->dynamicCast<Call>()) {
    for (int i = 0; i < env.callDepth; ++i) {
      std::cerr << "──";
    }
    std::cerr << *call << std::endl;
    env.callDepth++;

    EE ee = flatten_call(env, ctx, e, r, b);

    env.callDepth--;
    return ee;
  }
#endif

  int dispatch = is_par ? 0 : Expression::eid(e) - Expression::E_INTLIT + 1;

  return flattener_dispatch[dispatch](env, ctx, e, r, b);
}

}  // namespace MiniZinc
