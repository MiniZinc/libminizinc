/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/gc.hh>
#include <minizinc/hash.hh>
#include <minizinc/iter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/values.hh>

#include <cassert>
#include <cmath>

namespace MiniZinc {

void check_par_domain(EnvI& env, VarDecl* vd, Expression* rhs, bool isArg) {
  struct ToCheck {
    Expression* accessor;
    Expression* e;
    TypeInst* ti;
    ToCheck(Expression* _accessor, Expression* _e, TypeInst* _ti)
        : accessor(_accessor), e(_e), ti(_ti) {}
  };
  bool hasName = !vd->id()->str().empty();
  std::vector<ToCheck> todo({{hasName ? vd->id() : nullptr, rhs, vd->ti()}});
  GCLock lock;

  // Firstly just walks through checking domains, then if we encounter an error,
  // starts again recording what accessors are needed to generate the error message.
  bool hadError = false;
  while (!todo.empty()) {
    auto it = todo.back();
    todo.pop_back();
    if (it.ti->domain() == nullptr || Expression::isa<TIId>(it.ti->domain())) {
      continue;
    }
    if (Expression::type(it.e).dim() > 0) {
      ArrayLit* al = eval_array_lit(env, it.e);
      unsigned int enumId = it.ti->type().typeId();
      for (unsigned int i = 0; i < al->size(); i++) {
        Expression* access = nullptr;
        if (hadError) {
          std::vector<int> indexes(al->dims());
          int remDim = static_cast<int>(i);
          for (unsigned int j = al->dims(); (j--) != 0U;) {
            indexes[j] = (remDim % (al->max(j) - al->min(j) + 1)) + al->min(j);
            remDim = remDim / (al->max(j) - al->min(j) + 1);
          }
          std::vector<Expression*> indexes_s(indexes.size());
          if (enumId != 0) {
            const auto& enumIds = env.getArrayEnum(enumId);
            for (unsigned int j = 0; j < indexes.size(); j++) {
              std::ostringstream index_oss;
              if (enumIds[j] != 0) {
                auto name = env.enumToString(enumIds[j], indexes[j]);
                indexes_s[j] = new Id(Location().introduce(), name, nullptr);
              } else {
                indexes_s[j] = IntLit::a(indexes[j]);
              }
            }
          } else {
            for (unsigned int j = 0; j < indexes.size(); j++) {
              indexes_s[j] = IntLit::a(indexes[j]);
            }
          }
          access = new ArrayAccess(Location().introduce(), it.accessor, indexes_s);
        }
        todo.emplace_back(access, (*al)[i], it.ti);
      }
    } else if (Expression::type(it.e).istuple()) {
      auto* domains = Expression::cast<ArrayLit>(it.ti->domain());
      auto* al = eval_array_lit(env, it.e);
      for (unsigned int i = 0; i < al->size(); i++) {
        auto* access = hadError
                           ? new FieldAccess(Location().introduce(), it.accessor, IntLit::a(i + 1))
                           : nullptr;
        todo.emplace_back(access, (*al)[i], Expression::cast<TypeInst>((*domains)[i]));
      }
    } else if (Expression::type(it.e).isrecord()) {
      RecordType* rt = env.getRecordType(Expression::type(it.e));
      auto* domains = Expression::cast<ArrayLit>(it.ti->domain());
      auto* al = eval_array_lit(env, it.e);
      for (unsigned int i = 0; i < al->size(); i++) {
        Expression* access = nullptr;
        if (hadError) {
          auto* field = new Id(Location().introduce(), rt->fieldName(i), nullptr);
          access = new FieldAccess(Location().introduce(), it.accessor, field);
        }
        todo.emplace_back(access, (*al)[i], Expression::cast<TypeInst>((*domains)[i]));
      }
    } else if (Expression::type(it.e) == Type::parint()) {
      IntSetVal* isv = eval_intset(env, it.ti->domain());
      IntVal v = eval_int(env, it.e);
      if (!isv->contains(v)) {
        if (it.accessor != nullptr) {
          auto enumId = Expression::type(it.ti->domain()).typeId();
          std::ostringstream oss;
          oss << (isArg ? "argument" : "parameter") << " value out of range: ";
          oss << "declared domain of `" << *it.accessor << "' is " << env.show(isv, enumId) << ", ";
          oss << "but assigned value is " << env.show(v, enumId);
          throw ResultUndefinedError(env, Expression::loc(rhs), oss.str());
        }
        hadError = true;
        todo.clear();
        Id* ident =
            hasName ? vd->id()
                    : new Id(Location().introduce(), env.constants.ids.unnamedArgument, nullptr);
        todo.emplace_back(ident, rhs, vd->ti());
      }
    } else if (Expression::type(it.e) == Type::parfloat()) {
      FloatSetVal* fsv = eval_floatset(env, it.ti->domain());
      FloatVal v = eval_float(env, it.e);
      if (!fsv->contains(v)) {
        if (it.accessor != nullptr) {
          std::ostringstream oss;
          oss << (isArg ? "argument" : "parameter") << " value out of range: ";
          oss << "declared domain of `" << *it.accessor << "' is " << *fsv << ", ";
          oss << "but assigned value is " << v;
          throw ResultUndefinedError(env, Expression::loc(rhs), oss.str());
        }
        hadError = true;
        todo.clear();
        Id* ident =
            hasName ? vd->id()
                    : new Id(Location().introduce(), env.constants.ids.unnamedArgument, nullptr);
        todo.emplace_back(ident, rhs, vd->ti());
      }
    } else if (Expression::type(it.e) == Type::parsetint()) {
      IntSetVal* isv = eval_intset(env, it.ti->domain());
      IntSetRanges ir(isv);
      IntSetVal* rsv = eval_intset(env, it.e);
      IntSetRanges rr(rsv);
      if (!Ranges::subset(rr, ir)) {
        if (it.accessor != nullptr) {
          auto enumId = Expression::type(it.ti->domain()).typeId();
          std::ostringstream oss;
          oss << (isArg ? "argument" : "parameter") << " value out of range: ";
          oss << "declared domain of `" << *it.accessor << "' is " << env.show(isv, enumId) << ", ";
          oss << "but assigned value is " << env.show(rsv, enumId);
          throw ResultUndefinedError(env, Expression::loc(rhs), oss.str());
        }
        hadError = true;
        todo.clear();
        Id* ident =
            hasName ? vd->id()
                    : new Id(Location().introduce(), env.constants.ids.unnamedArgument, nullptr);
        todo.emplace_back(ident, rhs, vd->ti());
      }
    } else if (Expression::type(it.e) == Type::parsetfloat()) {
      FloatSetVal* fsv = eval_floatset(env, it.ti->domain());
      FloatSetRanges fr(fsv);
      FloatSetVal* rsv = eval_floatset(env, it.e);
      FloatSetRanges rr(rsv);
      if (!Ranges::subset(rr, fr)) {
        if (it.accessor != nullptr) {
          std::ostringstream oss;
          oss << (isArg ? "argument" : "parameter") << " value out of range: ";
          oss << "declared domain of `" << *it.accessor << "' is " << *fsv << ", ";
          oss << "but assigned value is " << *rsv;
          throw ResultUndefinedError(env, Expression::loc(rhs), oss.str());
        }
        hadError = true;
        todo.clear();
        Id* ident =
            hasName ? vd->id()
                    : new Id(Location().introduce(), env.constants.ids.unnamedArgument, nullptr);
        todo.emplace_back(ident, rhs, vd->ti());
      }
    }
  }
}

void check_par_declaration(EnvI& env, VarDecl* vd) {
  check_index_sets(env, vd, vd->e());
  check_par_domain(env, vd, vd->e());
}

void check_struct_retval(EnvI& env, Expression* v, FunctionI* fi) {
  // TODO: more specific error messages
  std::vector<std::pair<Expression*, TypeInst*>> todo({{v, fi->ti()}});
  while (!todo.empty()) {
    auto entry = todo.back();
    todo.pop_back();

    if (Expression::type(entry.first).dim() > 0) {
      auto* al = eval_array_lit(env, entry.first);
      for (unsigned int i = 0; i < entry.second->ranges().size(); i++) {
        if ((entry.second->ranges()[i]->domain() != nullptr) &&
            !Expression::isa<TIId>(entry.second->ranges()[i]->domain())) {
          IntSetVal* isv = eval_intset(env, entry.second->ranges()[i]->domain());
          bool bothEmpty = isv->empty() && al->min(i) > al->max(i);
          if (!bothEmpty && !isv->empty() &&
              (al->min(i) != isv->min() || al->max(i) != isv->max())) {
            throw ResultUndefinedError(env, Expression::loc(fi->e()),
                                       "function result violates function type-inst");
          }
        }
      }
      for (unsigned int i = 0; i < al->size(); i++) {
        todo.emplace_back((*al)[i], entry.second);
      }
      continue;
    }

    if (entry.second->domain() == nullptr || Expression::isa<TIId>(entry.second->domain())) {
      continue;
    }

    if (Expression::type(entry.first).structBT()) {
      auto* domains = eval_array_lit(env, entry.second->domain());
      auto* al = eval_array_lit(env, entry.first);
      for (unsigned int i = 0; i < al->size(); i++) {
        todo.emplace_back((*al)[i], Expression::cast<TypeInst>((*domains)[i]));
      }
      continue;
    }

    if (Expression::type(entry.first) == Type::parint()) {
      IntSetVal* isv = eval_intset(env, entry.second->domain());
      IntVal v = eval_int(env, entry.first);
      if (!isv->contains(v)) {
        throw ResultUndefinedError(env, Expression::loc(fi->e()),
                                   "function result violates function type-inst");
      }
    } else if (Expression::type(entry.first) == Type::parfloat()) {
      FloatSetVal* fsv = eval_floatset(env, entry.second->domain());
      FloatVal v = eval_float(env, entry.first);
      if (!fsv->contains(v)) {
        throw ResultUndefinedError(env, Expression::loc(fi->e()),
                                   "function result violates function type-inst");
      }
    } else if (Expression::type(entry.first) == Type::parsetint()) {
      IntSetVal* isv = eval_intset(env, entry.second->domain());
      IntSetRanges ir(isv);
      IntSetVal* rsv = eval_intset(env, entry.first);
      IntSetRanges rr(rsv);
      if (!Ranges::subset(rr, ir)) {
        throw ResultUndefinedError(env, Expression::loc(fi->e()),
                                   "function result violates function type-inst");
      }
    } else if (Expression::type(entry.first) == Type::parsetfloat()) {
      FloatSetVal* fsv = eval_floatset(env, entry.second->domain());
      FloatSetRanges fr(fsv);
      FloatSetVal* rsv = eval_floatset(env, entry.first);
      FloatSetRanges rr(rsv);
      if (!Ranges::subset(rr, fr)) {
        throw ResultUndefinedError(env, Expression::loc(fi->e()),
                                   "function result violates function type-inst");
      }
    }
  }
}

ArrayLit* eval_record_merge(EnvI& env, ArrayLit* lhs, ArrayLit* rhs) {
  RecordType* fields1 = env.getRecordType(lhs->type());
  RecordType* fields2 = env.getRecordType(rhs->type());
  RecordFieldSort cmp;

  std::vector<Expression*> all_fields;
  const unsigned int total_size = fields1->size() + fields2->size();
  all_fields.reserve(total_size);
  unsigned int l = 0;
  unsigned int r = 0;
  for (unsigned int i = 0; i < total_size; i++) {
    if (l >= fields1->size()) {
      // must choose rhs
      all_fields.emplace_back((*rhs)[r]);
      ++r;
    } else if (r >= fields2->size()) {
      // must choose lhs
      all_fields.emplace_back((*lhs)[l]);
      ++l;
    } else {
      ASTString lhsN(fields1->fieldName(l));
      ASTString rhsN(fields2->fieldName(r));
      if (cmp(lhsN, rhsN)) {
        // lhsN < rhsN
        all_fields.emplace_back((*lhs)[l]);
        ++l;
      } else {
        all_fields.emplace_back((*rhs)[r]);
        ++r;
      }
    }
  }
  assert(r + l == all_fields.size());

  ArrayLit* ret = ArrayLit::constructTuple(Location().introduce(), all_fields);
  return ret;
}

template <class E>
typename E::Val eval_id(EnvI& env, Expression* e) {
  Id* id = Expression::cast<Id>(e);
  if (id == env.constants.absent) {
    throw InternalError("unexpected absent literal");
  }
  if (!id->decl()) {
    GCLock lock;
    throw EvalError(env, Expression::loc(e), "undeclared identifier", id->str());
  }
  VarDecl* vd = id->decl();
  while (vd->flat() && vd->flat() != vd) {
    vd = vd->flat();
  }
  if (!vd->e()) {
    GCLock lock;
    throw EvalError(env, Expression::loc(vd), "cannot evaluate expression", id->str());
  }
  typename E::Val r = E::e(env, vd->e());
  if (!vd->evaluated() &&
      (vd->toplevel() || (!Expression::isa<Id>(vd->e()) && vd->type().dim() > 0))) {
    Expression* ne = E::exp(r);
    vd->e(ne);
    vd->evaluated(true);
  }
  return r;
}

bool EvalBase::evalBoolCV(EnvI& env, Expression* e) {
  GCLock lock;
  if (Expression::type(e).cv()) {
    return eval_bool(env, flat_cv_exp(env, Ctx(), e)());
  }
  return eval_bool(env, e);
};

KeepAlive EvalBase::flattenCV(EnvI& env, Expression* e) {
  GCLock lock;
  Ctx ctx;
  ctx.i = C_MIX;
  ctx.b = (Expression::type(e).bt() == Type::BT_BOOL) ? C_MIX : C_ROOT;
  EE ee = flat_exp(env, ctx, e, nullptr, env.constants.varTrue);
  return ee.r;
}

class EvalIntLit : public EvalBase {
public:
  typedef IntLit* Val;
  typedef Expression* ArrayVal;
  static IntLit* e(EnvI& env, Expression* e) { return IntLit::a(eval_int(env, e)); }
  static Expression* exp(IntLit* e) { return e; }
};
class EvalIntVal : public EvalBase {
public:
  typedef IntVal Val;
  typedef IntVal ArrayVal;
  static IntVal e(EnvI& env, Expression* e) { return eval_int(env, e); }
  static Expression* exp(IntVal e) { return IntLit::a(e); }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {
    if ((fi->ti()->domain() != nullptr) && !Expression::isa<TIId>(fi->ti()->domain())) {
      IntSetVal* isv = eval_intset(env, fi->ti()->domain());
      if (!isv->contains(v)) {
        std::ostringstream oss;
        oss << "result of function `" << demonomorphise_identifier(fi->id()) << "' is "
            << env.show(v, fi->ti()->type().typeId()) << ", which violates function type-inst "
            << env.show(isv, fi->ti()->type().typeId());
        throw ResultUndefinedError(env, Location().introduce(), oss.str());
      }
    }
  }
};
class EvalFloatVal : public EvalBase {
public:
  typedef FloatVal Val;
  typedef FloatVal ArrayVal;
  static FloatVal e(EnvI& env, Expression* e) { return eval_float(env, e); }
  static Expression* exp(FloatVal e) { return FloatLit::a(e); }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {
    if ((fi->ti()->domain() != nullptr) && !Expression::isa<TIId>(fi->ti()->domain())) {
      FloatSetVal* fsv = eval_floatset(env, fi->ti()->domain());
      if (!fsv->contains(v)) {
        std::ostringstream oss;
        oss << "result of function `" << demonomorphise_identifier(fi->id()) << "' is " << v
            << ", which violates function type-inst " << *fsv;
        throw ResultUndefinedError(env, Location().introduce(), oss.str());
      }
    }
  }
};
class EvalFloatLit : public EvalBase {
public:
  typedef FloatLit* Val;
  typedef Expression* ArrayVal;
  static FloatLit* e(EnvI& env, Expression* e) { return FloatLit::a(eval_float(env, e)); }
  static Expression* exp(Expression* e) { return e; }
};
class EvalString : public EvalBase {
public:
  typedef std::string Val;
  typedef std::string ArrayVal;
  static std::string e(EnvI& env, Expression* e) { return eval_string(env, e); }
  static Expression* exp(const std::string& e) { return new StringLit(Location(), e); }
  static void checkRetVal(EnvI& env, const Val& v, FunctionI* fi) {}
};
class EvalStringLit : public EvalBase {
public:
  typedef StringLit* Val;
  typedef Expression* ArrayVal;
  static StringLit* e(EnvI& env, Expression* e) {
    return new StringLit(Location(), eval_string(env, e));
  }
  static Expression* exp(Expression* e) { return e; }
};
class EvalBoolLit : public EvalBase {
public:
  typedef BoolLit* Val;
  typedef Expression* ArrayVal;
  static BoolLit* e(EnvI& env, Expression* e) { return env.constants.boollit(eval_bool(env, e)); }
  static Expression* exp(Expression* e) { return e; }
};
class EvalBoolVal : public EvalBase {
public:
  typedef bool Val;
  static bool e(EnvI& env, Expression* e) { return eval_bool(env, e); }
  static Expression* exp(bool e) { return Constants::constants().boollit(e); }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {}
};
class EvalArrayLit : public EvalBase {
public:
  typedef ArrayLit* Val;
  typedef Expression* ArrayVal;
  static ArrayLit* e(EnvI& env, Expression* e) { return eval_array_lit(env, e); }
  static Expression* exp(Expression* e) { return e; }
};
class EvalArrayLitCopy : public EvalBase {
public:
  typedef ArrayLit* Val;
  typedef Expression* ArrayVal;
  static ArrayLit* e(EnvI& env, Expression* e) {
    return Expression::cast<ArrayLit>(copy(env, eval_array_lit(env, e), true));
  }
  static Expression* exp(Expression* e) { return e; }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {
    for (unsigned int i = 0; i < fi->ti()->ranges().size(); i++) {
      if ((fi->ti()->ranges()[i]->domain() != nullptr) &&
          !Expression::isa<TIId>(fi->ti()->ranges()[i]->domain())) {
        IntSetVal* isv = eval_intset(env, fi->ti()->ranges()[i]->domain());
        bool bothEmpty = isv->empty() && v->min(i) > v->max(i);
        if (!bothEmpty && !isv->empty() && (v->min(i) != isv->min() || v->max(i) != isv->max())) {
          std::ostringstream oss;
          oss << "array index set " << (i + 1) << " of function result violates function type-inst";
          throw ResultUndefinedError(env, Expression::loc(fi->e()), oss.str());
        }
      }
    }
    if ((fi->ti()->domain() != nullptr) && !Expression::isa<TIId>(fi->ti()->domain()) &&
        fi->ti()->type().ti() == Type::TI_PAR) {
      Type base_t = fi->ti()->type();
      if (base_t.bt() == Type::BT_INT) {
        IntSetVal* isv = eval_intset(env, fi->ti()->domain());
        auto enumId = Expression::type(fi->ti()->domain()).typeId();
        if (base_t.st() == Type::ST_PLAIN) {
          for (unsigned int i = 0; i < v->size(); i++) {
            auto* v_i = (*v)[i];
            if (Expression::type(v_i).isOpt()) {
              v_i = eval_par(env, v_i);
              if (v_i == env.constants.absent) {
                continue;
              }
            }
            IntVal iv = eval_int(env, v_i);
            if (!isv->contains(iv)) {
              std::ostringstream oss;
              oss << "array contains value " << env.show(iv, enumId)
                  << " which is not contained in " << env.show(isv, enumId);
              throw ResultUndefinedError(
                  env, Expression::loc(fi->e()),
                  "function result violates function type-inst, " + oss.str());
            }
          }
        } else {
          for (unsigned int i = 0; i < v->size(); i++) {
            IntSetVal* iv = eval_intset(env, (*v)[i]);
            IntSetRanges isv_r(isv);
            IntSetRanges v_r(iv);
            if (!Ranges::subset(v_r, isv_r)) {
              std::ostringstream oss;
              oss << "array contains value " << env.show(iv, enumId) << " which is not a subset of "
                  << env.show(isv, enumId);
              throw ResultUndefinedError(
                  env, Expression::loc(fi->e()),
                  "function result violates function type-inst, " + oss.str());
            }
          }
        }
      } else if (base_t.bt() == Type::BT_FLOAT) {
        FloatSetVal* fsv = eval_floatset(env, fi->ti()->domain());
        if (base_t.st() == Type::ST_PLAIN) {
          for (unsigned int i = 0; i < v->size(); i++) {
            auto* v_i = (*v)[i];
            if (Expression::type(v_i).isOpt()) {
              v_i = eval_par(env, v_i);
              if (v_i == env.constants.absent) {
                continue;
              }
            }
            FloatVal fv = eval_float(env, v_i);
            if (!fsv->contains(fv)) {
              std::ostringstream oss;
              oss << "array contains value " << fv << " which is not contained in " << *fsv;
              throw ResultUndefinedError(
                  env, Expression::loc(fi->e()),
                  "function result violates function type-inst, " + oss.str());
            }
          }
        } else {
          for (unsigned int i = 0; i < v->size(); i++) {
            FloatSetVal* fv = eval_floatset(env, (*v)[i]);
            FloatSetRanges fsv_r(fsv);
            FloatSetRanges v_r(fv);
            if (!Ranges::subset(v_r, fsv_r)) {
              std::ostringstream oss;
              oss << "array contains value " << *fv << " which is not a subset of " << *fsv;
              throw ResultUndefinedError(
                  env, Expression::loc(fi->e()),
                  "function result violates function type-inst, " + oss.str());
            }
          }
        }
      } else if (base_t.structBT()) {
        check_struct_retval(env, v, fi);
      }
    }
  }
};
class EvalIntSet : public EvalBase {
public:
  typedef IntSetVal* Val;
  static IntSetVal* e(EnvI& env, Expression* e) { return eval_intset(env, e); }
  static Expression* exp(IntSetVal* e) { return new SetLit(Location(), e); }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {
    if ((fi->ti()->domain() != nullptr) && !Expression::isa<TIId>(fi->ti()->domain())) {
      auto enumId = Expression::type(fi->ti()->domain()).typeId();
      IntSetVal* isv = eval_intset(env, fi->ti()->domain());
      IntSetRanges isv_r(isv);
      IntSetRanges v_r(v);
      if (!Ranges::subset(v_r, isv_r)) {
        std::ostringstream oss;
        oss << "result of function `" << demonomorphise_identifier(fi->id()) << "' is "
            << env.show(v, enumId) << ", which violates function type-inst "
            << env.show(isv, enumId);
        throw ResultUndefinedError(env, Location().introduce(), oss.str());
      }
    }
  }
};
class EvalFloatSet : public EvalBase {
public:
  typedef FloatSetVal* Val;
  static FloatSetVal* e(EnvI& env, Expression* e) { return eval_floatset(env, e); }
  static Expression* exp(FloatSetVal* e) { return new SetLit(Location(), e); }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {
    if ((fi->ti()->domain() != nullptr) && !Expression::isa<TIId>(fi->ti()->domain())) {
      FloatSetVal* fsv = eval_floatset(env, fi->ti()->domain());
      FloatSetRanges fsv_r(fsv);
      FloatSetRanges v_r(v);
      if (!Ranges::subset(v_r, fsv_r)) {
        std::ostringstream oss;
        oss << "result of function `" << demonomorphise_identifier(fi->id()) << "' is " << *v
            << ", which violates function type-inst " << *fsv;
        throw ResultUndefinedError(env, Location().introduce(), oss.str());
      }
    }
  }
};
class EvalBoolSet : public EvalBase {
public:
  typedef IntSetVal* Val;
  static IntSetVal* e(EnvI& env, Expression* e) { return eval_boolset(env, e); }
  static Expression* exp(IntSetVal* e) {
    auto* sl = new SetLit(Location(), e);
    sl->type(Type::parsetbool());
    return sl;
  }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {}
};
class EvalSetLit : public EvalBase {
public:
  typedef SetLit* Val;
  typedef Expression* ArrayVal;
  static SetLit* e(EnvI& env, Expression* e) { return eval_set_lit(env, e); }
  static Expression* exp(Expression* e) { return e; }
};
class EvalFloatSetLit : public EvalBase {
public:
  typedef SetLit* Val;
  typedef Expression* ArrayVal;
  static SetLit* e(EnvI& env, Expression* e) {
    return new SetLit(Expression::loc(e), eval_floatset(env, e));
  }
  static Expression* exp(Expression* e) { return e; }
};
class EvalBoolSetLit : public EvalBase {
public:
  typedef SetLit* Val;
  typedef Expression* ArrayVal;
  static SetLit* e(EnvI& env, Expression* e) {
    auto* sl = new SetLit(Expression::loc(e), eval_boolset(env, e));
    sl->type(Type::parsetbool());
    return sl;
  }
  static Expression* exp(Expression* e) { return e; }
};
class EvalCopy : public EvalBase {
public:
  typedef Expression* Val;
  typedef Expression* ArrayVal;
  static Expression* e(EnvI& env, Expression* e) { return copy(env, e, true); }
  static Expression* exp(Expression* e) { return e; }
};
class EvalPar : public EvalBase {
public:
  typedef Expression* Val;
  typedef Expression* ArrayVal;
  static Expression* e(EnvI& env, Expression* e) { return eval_par(env, e); }
  static Expression* exp(Expression* e) { return e; }
  static void checkRetVal(EnvI& env, Val v, FunctionI* fi) {
    if (fi->ti()->type().structBT()) {
      check_struct_retval(env, v, fi);
    }
  }
};

template <class CallClass>
class EvalCallCleanup {
private:
  CallClass* _call;
  std::vector<Expression*> _previousParameters;
  KeepAlive _previousCapture;

public:
  EvalCallCleanup(EnvI& env, CallClass* call)
      : _call(call), _previousParameters(call->decl()->paramCount()) {
    for (unsigned int i = 0; i < call->decl()->paramCount(); i++) {
      _previousParameters[i] = call->decl()->param(i)->e();
    }
    if (call->decl()->capturedAnnotationsVar() != nullptr) {
      _previousCapture = call->decl()->capturedAnnotationsVar()->e();
      GCLock lock;
      call->decl()->capturedAnnotationsVar()->flat(call->decl()->capturedAnnotationsVar());
      call->decl()->capturedAnnotationsVar()->e(env.createAnnotationArray(C_MIX));
    }
  }
  // NOLINTNEXTLINE(bugprone-exception-escape)
  ~EvalCallCleanup() {
    for (unsigned int i = 0; i < _call->decl()->paramCount(); i++) {
      VarDecl* vd = _call->decl()->param(i);
      vd->e(_previousParameters[i]);
      vd->flat(vd->e() == nullptr ? nullptr : vd);
    }
    if (_call->decl()->capturedAnnotationsVar() != nullptr) {
      _call->decl()->capturedAnnotationsVar()->e(_previousCapture());
      _call->decl()->capturedAnnotationsVar()->flat(_call->decl()->capturedAnnotationsVar()->e() !=
                                                            nullptr
                                                        ? _call->decl()->capturedAnnotationsVar()
                                                        : nullptr);
    }
  }
};

template <class Eval, class CallClass = Call>
typename Eval::Val eval_call(EnvI& env, CallClass* ce) {
  std::vector<Expression*> params(ce->decl()->paramCount());
  for (unsigned int i = 0; i < ce->decl()->paramCount(); i++) {
    params[i] = eval_par(env, ce->arg(i));
  }
  EvalCallCleanup<CallClass> ecc(env, ce);
  for (unsigned int i = ce->decl()->paramCount(); i--;) {
    VarDecl* vd = ce->decl()->param(i);
    auto arg_idx = i;
    check_index_sets(env, vd, params[i], true);
    vd->flat(vd);
    vd->e(params[i]);
    if (Expression::type(vd->e()).isPar()) {
      check_par_domain(env, vd, vd->e(), true);
    }
  }
  typename Eval::Val ret = Eval::e(env, ce->decl()->e());
  Eval::checkRetVal(env, ret, ce->decl());
  return ret;
}

Expression* eval_fieldaccess(EnvI& env, FieldAccess* fa) {
  assert(Expression::type(fa->v()).istuple() ||
         Expression::type(fa->v()).isrecord());  // TODO: Support for Records
  auto* al = Expression::dynamicCast<ArrayLit>(eval_array_lit(env, fa->v()));
  if (al == nullptr) {
    throw EvalError(env, Expression::loc(fa), "Internal error: could not evaluate structural type");
  }
  IntVal i = eval_int(env, fa->field());
  if (i < 1 || i > al->size()) {
    // This should not happen, type checking should ensure all fields are valid.
    throw EvalError(env, Expression::loc(fa), "Internal error: accessing invalid field");
  }
  return (*al)[static_cast<unsigned int>(i.toInt()) - 1];
}

ArrayLit* eval_array_comp(EnvI& env, Comprehension* e) {
  ArrayLit* ret;
  bool plainParNonAbsent = e->type().ti() == Type::TI_PAR && e->type().st() == Type::ST_PLAIN &&
                           e->type().ot() == Type::OT_PRESENT;
  if (plainParNonAbsent && e->type().bt() == Type::BT_INT) {
    auto a = eval_comp<EvalIntLit>(env, e);
    ret = new ArrayLit(Expression::loc(e), a.a, a.dims);
  } else if (plainParNonAbsent && e->type().bt() == Type::BT_BOOL) {
    auto a = eval_comp<EvalBoolLit>(env, e);
    ret = new ArrayLit(Expression::loc(e), a.a, a.dims);
  } else if (plainParNonAbsent && e->type().bt() == Type::BT_FLOAT) {
    auto a = eval_comp<EvalFloatLit>(env, e);
    ret = new ArrayLit(Expression::loc(e), a.a, a.dims);
  } else if (e->type().st() == Type::ST_SET) {
    auto a = eval_comp<EvalSetLit>(env, e);
    ret = new ArrayLit(Expression::loc(e), a.a, a.dims);
  } else if (plainParNonAbsent && e->type().bt() == Type::BT_STRING) {
    auto a = eval_comp<EvalStringLit>(env, e);
    ret = new ArrayLit(Expression::loc(e), a.a, a.dims);
  } else {
    auto a = eval_comp<EvalCopy>(env, e);
    ret = new ArrayLit(Expression::loc(e), a.a, a.dims);
  }
  ret->type(e->type());
  return ret;
}

Expression* eval_arrayaccess(EnvI& env, ArrayAccess* e);

ArrayLit* eval_array_lit(EnvI& env, Expression* e) {
  CallStackItem csi(env, e);
  switch (Expression::eid(e)) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_SETLIT:
    case Expression::E_ANON:
    case Expression::E_TI:
    case Expression::E_TIID:
    case Expression::E_VARDECL:
      throw EvalError(env, Expression::loc(e), "not an array expression");
    case Expression::E_ID:
      return eval_id<EvalArrayLit>(env, e);
    case Expression::E_ARRAYLIT:
      return Expression::cast<ArrayLit>(e);
    case Expression::E_ARRAYACCESS: {
      if (!Expression::type(e).structBT()) {
        throw EvalError(env, Expression::loc(e), "arrays of arrays not supported");
      }
      GCLock lock;
      return eval_array_lit(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
    }
    case Expression::E_FIELDACCESS: {
      auto* fa = Expression::cast<FieldAccess>(e);
      return eval_array_lit(env, eval_fieldaccess(env, fa));
    }
    case Expression::E_COMP:
      return eval_array_comp(env, Expression::cast<Comprehension>(e));
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      for (unsigned int i = 0; i < ite->size(); i++) {
        if (eval_bool(env, ite->ifExpr(i))) {
          return eval_array_lit(env, ite->thenExpr(i));
        }
      }
      return eval_array_lit(env, ite->elseExpr());
    }
    case Expression::E_BINOP: {
      auto* bo = Expression::cast<BinOp>(e);
      if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
        return eval_call<EvalArrayLitCopy, BinOp>(env, bo);
      }
      if (bo->op() == BOT_PLUSPLUS) {
        ArrayLit* al0 = eval_array_lit(env, bo->lhs());
        ArrayLit* al1 = eval_array_lit(env, bo->rhs());
        if (bo->type().isrecord()) {
          ArrayLit* rec = eval_record_merge(env, al0, al1);
          rec->type(bo->type());
          return rec;
        }
        std::vector<Expression*> v(al0->size() + al1->size());
        for (unsigned int i = al0->size(); (i--) != 0U;) {
          v[i] = (*al0)[i];
        }
        for (unsigned int i = al1->size(); (i--) != 0U;) {
          v[al0->size() + i] = (*al1)[i];
        }
        auto* ret = bo->type().istuple() ? ArrayLit::constructTuple(Expression::loc(e), v)
                                         : new ArrayLit(Expression::loc(e), v);
        ret->flat(al0->flat() && al1->flat());
        ret->type(Expression::type(e));
        return ret;
      }
      throw EvalError(env, Expression::loc(e), "not an array expression", bo->opToString());

    } break;
    case Expression::E_UNOP: {
      UnOp* uo = Expression::cast<UnOp>(e);
      if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
        return eval_call<EvalArrayLitCopy, UnOp>(env, uo);
      }
      throw EvalError(env, Expression::loc(e), "not an array expression");
    }
    case Expression::E_CALL: {
      Call* ce = Expression::cast<Call>(e);
      if (ce->decl() == nullptr) {
        throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
      }

      if (ce->decl()->builtins.e != nullptr) {
        return eval_array_lit(env, ce->decl()->builtins.e(env, ce));
      }

      if (ce->decl()->e() == nullptr) {
        std::ostringstream ss;
        ss << "internal error: missing builtin '" << ce->id() << "'";
        throw EvalError(env, Expression::loc(ce), ss.str());
      }

      return eval_call<EvalArrayLitCopy>(env, ce);
    }
    case Expression::E_LET: {
      Let* l = Expression::cast<Let>(e);
      LetPushBindings lpb(l);
      for (unsigned int i = 0; i < l->let().size(); i++) {
        // Evaluate all variable declarations
        if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
          vdi->e(eval_par(env, vdi->e()));
          if (vdi->e() != nullptr) {
            // Note: might be par because of singular domain, and might not have a right hand side.
            check_par_declaration(env, vdi);
          }
        } else {
          // This is a constraint item. Since the let is par,
          // it can only be a par bool expression. If it evaluates
          // to false, it means that the value of this let is undefined.
          if (!eval_bool(env, l->let()[i])) {
            throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                       "constraint in let failed");
          }
        }
      }
      ArrayLit* l_in = eval_array_lit(env, l->in());
      auto* ret = Expression::cast<ArrayLit>(copy(env, l_in, true));
      ret->flat(l_in->flat());
      return ret;
    }
  }
  assert(false);
  return nullptr;
}

std::string ArrayAccessSucess::errorMessage(EnvI& env, Expression* e) const {
  std::ostringstream oss;
  oss << "array access out of bounds, ";

  if (dimMin.toInt() > dimMax.toInt()) {
    oss << "array";
    if (Expression::isa<Id>(e)) {
      oss << " `" << *e << "'";
    }
    oss << " is empty";
    return oss.str();
  }

  if (Expression::type(e).dim() > 1) {
    oss << "dimension " << (dim + 1) << " of ";
  }
  oss << "array";
  if (Expression::isa<Id>(e)) {
    oss << " `" << *e << "'";
  }

  unsigned int enumId = Expression::type(e).typeId();
  if (enumId != 0) {
    const auto& enumIds = env.getArrayEnum(enumId);
    enumId = enumIds[dim];
  }
  if (enumId != 0) {
    oss << " has index set ";
    oss << env.enumToString(enumId, static_cast<int>(dimMin.toInt()));
    oss << "..";
    oss << env.enumToString(enumId, static_cast<int>(dimMax.toInt()));
    oss << ", but given index is ";
    oss << env.enumToString(enumId, static_cast<int>(idx.toInt()));
  } else {
    oss << " has index set " << dimMin << ".." << dimMax;
    oss << ", but given index is " << idx;
  }
  return oss.str();
}

Expression* ArrayAccessSucess::dummyLiteral(EnvI& env, Type t) const {
  if (t.isint()) {
    return IntLit::a(0);
  }
  if (t.isbool()) {
    return Constants::constants().literalFalse;
  }
  if (t.isfloat()) {
    return FloatLit::a(0.0);
  }
  if (t.st() == Type::ST_SET || t.isbot()) {
    auto* ret = new SetLit(Location(), std::vector<Expression*>());
    ret->type(t);
    return ret;
  }
  if (t.isstring()) {
    return new StringLit(Location(), "");
  }
  if (t.structBT()) {
    auto* tt = env.getStructType(t);
    std::vector<Expression*> fields;
    fields.reserve(tt->size());
    for (unsigned int i = 0; i < tt->size(); i++) {
      fields.push_back(dummyLiteral(env, (*tt)[i]));
    }
    auto* al = ArrayLit::constructTuple(Location(), fields);
    al->type(t);
    return al;
  }
  throw InternalError("unexpected type in array access expression");
}

Expression* eval_arrayaccess(EnvI& env, ArrayAccess* e, ArrayAccessSucess& success) {
  ArrayLit* al = eval_array_lit(env, e->v());
  std::vector<IntVal> indices(e->idx().size());
  bool allAbsent = true;
  bool anyAbsent = false;
  for (unsigned int i = 0; i < e->idx().size(); i++) {
    auto* idx = eval_par(env, e->idx()[i]);
    if (idx == env.constants.absent) {
      anyAbsent = true;
      indices[i] = al->min(i);
    } else {
      allAbsent = false;
      indices[i] = IntLit::v(Expression::cast<IntLit>(idx));
    }
  }
  if (allAbsent) {
    return env.constants.absent;
  }
  auto* result = eval_arrayaccess(env, al, indices, success);
  return anyAbsent ? env.constants.absent : result;
}
Expression* eval_arrayaccess(EnvI& env, ArrayAccess* e) {
  ArrayAccessSucess success;
  Expression* ret = eval_arrayaccess(env, e, success);
  if (success()) {
    return ret;
  }
  throw ResultUndefinedError(env, Expression::loc(e), success.errorMessage(env, e->v()));
}

SetLit* eval_set_lit(EnvI& env, Expression* e) {
  switch (Expression::type(e).bt()) {
    case Type::BT_INT:
    case Type::BT_BOT: {
      auto* sl = new SetLit(Expression::loc(e), eval_intset(env, e));
      if (Expression::type(e).typeId() != 0) {
        Type t = sl->type();
        t.typeId(Expression::type(e).typeId());
        sl->type(t);
      }
      return sl;
    }
    case Type::BT_BOOL: {
      auto* sl = new SetLit(Expression::loc(e), eval_boolset(env, e));
      sl->type(Type::parsetbool());
      return sl;
    }
    case Type::BT_FLOAT:
      return new SetLit(Expression::loc(e), eval_floatset(env, e));
    default:
      throw InternalError("invalid set literal type");
  }
}

IntSetVal* eval_intset(EnvI& env, Expression* e) {
  if (auto* sl = Expression::dynamicCast<SetLit>(e)) {
    if (sl->isv() != nullptr) {
      return sl->isv();
    }
  }
  CallStackItem csi(env, e);
  switch (Expression::eid(e)) {
    case Expression::E_SETLIT: {
      auto* sl = Expression::cast<SetLit>(e);
      std::vector<IntVal> vals;
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        Expression* vi = eval_par(env, sl->v()[i]);
        if (vi != env.constants.absent) {
          vals.push_back(eval_int(env, vi));
        }
      }
      return IntSetVal::a(vals);
    }
    case Expression::E_BOOLLIT:
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_VARDECL:
    case Expression::E_TI:
      throw EvalError(env, Expression::loc(e), "not a set of int expression");
      break;
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
      std::vector<IntVal> vals(al->size());
      for (unsigned int i = 0; i < al->size(); i++) {
        vals[i] = eval_int(env, (*al)[i]);
      }
      return IntSetVal::a(vals);
    } break;
    case Expression::E_COMP: {
      auto* c = Expression::cast<Comprehension>(e);
      auto a = eval_comp<EvalIntVal>(env, c);
      return IntSetVal::a(a.a);
    }
    case Expression::E_ID: {
      GCLock lock;
      return eval_id<EvalSetLit>(env, e)->isv();
    } break;
    case Expression::E_ARRAYACCESS: {
      GCLock lock;
      return eval_intset(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
    } break;
    case Expression::E_FIELDACCESS: {
      return eval_intset(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
    } break;
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      for (unsigned int i = 0; i < ite->size(); i++) {
        if (eval_bool(env, ite->ifExpr(i))) {
          return eval_intset(env, ite->thenExpr(i));
        }
      }
      return eval_intset(env, ite->elseExpr());
    } break;
    case Expression::E_BINOP: {
      auto* bo = Expression::cast<BinOp>(e);
      if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
        return eval_call<EvalIntSet, BinOp>(env, bo);
      }
      Expression* lhs = eval_par(env, bo->lhs());
      Expression* rhs = eval_par(env, bo->rhs());
      if (Expression::type(lhs).isIntSet() && Expression::type(rhs).isIntSet()) {
        IntSetVal* v0 = eval_intset(env, lhs);
        IntSetVal* v1 = eval_intset(env, rhs);
        IntSetRanges ir0(v0);
        IntSetRanges ir1(v1);
        switch (bo->op()) {
          case BOT_UNION: {
            Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            return IntSetVal::ai(u);
          }
          case BOT_DIFF: {
            Ranges::Diff<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            return IntSetVal::ai(u);
          }
          case BOT_SYMDIFF: {
            Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> i(ir0, ir1);
            Ranges::Diff<IntVal, Ranges::Union<IntVal, IntSetRanges, IntSetRanges>,
                         Ranges::Inter<IntVal, IntSetRanges, IntSetRanges>>
                sd(u, i);
            return IntSetVal::ai(sd);
          }
          case BOT_INTERSECT: {
            Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            return IntSetVal::ai(u);
          }
          default:
            throw EvalError(env, Expression::loc(e), "not a set of int expression",
                            bo->opToString());
        }
      } else if (Expression::type(lhs).isint() && Expression::type(rhs).isint()) {
        if (bo->op() != BOT_DOTDOT) {
          throw EvalError(env, Expression::loc(e), "not a set of int expression", bo->opToString());
        }
        return IntSetVal::a(eval_int(env, lhs), eval_int(env, rhs));
      } else {
        throw EvalError(env, Expression::loc(e), "not a set of int expression", bo->opToString());
      }
    } break;
    case Expression::E_UNOP: {
      UnOp* uo = Expression::cast<UnOp>(e);
      if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
        return eval_call<EvalIntSet, UnOp>(env, uo);
      }
      throw EvalError(env, Expression::loc(e), "not a set of int expression");
    }
    case Expression::E_CALL: {
      Call* ce = Expression::cast<Call>(e);
      if (ce->decl() == nullptr) {
        throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
      }

      if (ce->decl()->builtins.s != nullptr) {
        return ce->decl()->builtins.s(env, ce);
      }

      if (ce->decl()->builtins.e != nullptr) {
        return eval_intset(env, ce->decl()->builtins.e(env, ce));
      }

      if (ce->decl()->e() == nullptr) {
        std::ostringstream ss;
        ss << "internal error: missing builtin '" << ce->id() << "'";
        throw EvalError(env, Expression::loc(ce), ss.str());
      }

      return eval_call<EvalIntSet>(env, ce);
    } break;
    case Expression::E_LET: {
      Let* l = Expression::cast<Let>(e);
      LetPushBindings lpb(l);
      for (unsigned int i = 0; i < l->let().size(); i++) {
        // Evaluate all variable declarations
        if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
          vdi->e(eval_par(env, vdi->e()));
          check_par_declaration(env, vdi);
        } else {
          // This is a constraint item. Since the let is par,
          // it can only be a par bool expression. If it evaluates
          // to false, it means that the value of this let is undefined.
          if (!eval_bool(env, l->let()[i])) {
            throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                       "constraint in let failed");
          }
        }
      }
      IntSetVal* ret = eval_intset(env, l->in());
      return ret;
    } break;
    default:
      assert(false);
  }
  return nullptr;
}

FloatSetVal* eval_floatset(EnvI& env, Expression* e) {
  if (auto* sl = Expression::dynamicCast<SetLit>(e)) {
    if (sl->fsv() != nullptr) {
      return sl->fsv();
    }
    if (sl->isv() != nullptr) {
      IntSetRanges isr(sl->isv());
      return FloatSetVal::ai(isr);
    }
  }
  CallStackItem csi(env, e);
  switch (Expression::eid(e)) {
    case Expression::E_SETLIT: {
      auto* sl = Expression::cast<SetLit>(e);
      std::vector<FloatVal> vals;
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        Expression* vi = eval_par(env, sl->v()[i]);
        if (vi != env.constants.absent) {
          vals.push_back(eval_float(env, vi));
        }
      }
      return FloatSetVal::a(vals);
    }
    case Expression::E_BOOLLIT:
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_VARDECL:
    case Expression::E_TI:
      throw EvalError(env, Expression::loc(e), "not a set of float expression");
      break;
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
      std::vector<FloatVal> vals(al->size());
      for (unsigned int i = 0; i < al->size(); i++) {
        vals[i] = eval_float(env, (*al)[i]);
      }
      return FloatSetVal::a(vals);
    } break;
    case Expression::E_COMP: {
      auto* c = Expression::cast<Comprehension>(e);
      auto a = eval_comp<EvalFloatVal>(env, c);
      return FloatSetVal::a(a.a);
    }
    case Expression::E_ID: {
      GCLock lock;
      return eval_floatset(env, eval_id<EvalFloatSetLit>(env, e));
    } break;
    case Expression::E_ARRAYACCESS: {
      GCLock lock;
      return eval_floatset(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
    } break;
    case Expression::E_FIELDACCESS: {
      return eval_floatset(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
    } break;
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      for (unsigned int i = 0; i < ite->size(); i++) {
        if (eval_bool(env, ite->ifExpr(i))) {
          return eval_floatset(env, ite->thenExpr(i));
        }
      }
      return eval_floatset(env, ite->elseExpr());
    } break;
    case Expression::E_BINOP: {
      auto* bo = Expression::cast<BinOp>(e);
      if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
        return eval_call<EvalFloatSet, BinOp>(env, bo);
      }
      Expression* lhs = eval_par(env, bo->lhs());
      Expression* rhs = eval_par(env, bo->rhs());
      if (Expression::type(lhs).isFloatSet() && Expression::type(rhs).isFloatSet()) {
        FloatSetVal* v0 = eval_floatset(env, lhs);
        FloatSetVal* v1 = eval_floatset(env, rhs);
        FloatSetRanges fr0(v0);
        FloatSetRanges fr1(v1);
        switch (bo->op()) {
          case BOT_UNION: {
            Ranges::Union<FloatVal, FloatSetRanges, FloatSetRanges> u(fr0, fr1);
            return FloatSetVal::ai(u);
          }
          case BOT_DIFF: {
            Ranges::Diff<FloatVal, FloatSetRanges, FloatSetRanges> u(fr0, fr1);
            return FloatSetVal::ai(u);
          }
          case BOT_SYMDIFF: {
            Ranges::Union<FloatVal, FloatSetRanges, FloatSetRanges> u(fr0, fr1);
            Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> i(fr0, fr1);
            Ranges::Diff<FloatVal, Ranges::Union<FloatVal, FloatSetRanges, FloatSetRanges>,
                         Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges>>
                sd(u, i);
            return FloatSetVal::ai(sd);
          }
          case BOT_INTERSECT: {
            Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> u(fr0, fr1);
            return FloatSetVal::ai(u);
          }
          default:
            throw EvalError(env, Expression::loc(e), "not a set of int expression",
                            bo->opToString());
        }
      } else if (Expression::type(lhs).isfloat() && Expression::type(rhs).isfloat()) {
        if (bo->op() != BOT_DOTDOT) {
          throw EvalError(env, Expression::loc(e), "not a set of float expression",
                          bo->opToString());
        }
        return FloatSetVal::a(eval_float(env, lhs), eval_float(env, rhs));
      } else {
        throw EvalError(env, Expression::loc(e), "not a set of float expression", bo->opToString());
      }
    } break;
    case Expression::E_UNOP: {
      UnOp* uo = Expression::cast<UnOp>(e);
      if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
        return eval_call<EvalFloatSet, UnOp>(env, uo);
      }
      throw EvalError(env, Expression::loc(e), "not a set of float expression");
    }
    case Expression::E_CALL: {
      Call* ce = Expression::cast<Call>(e);
      if (ce->decl() == nullptr) {
        throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
      }

      if (ce->decl()->builtins.e != nullptr) {
        return eval_floatset(env, ce->decl()->builtins.e(env, ce));
      }

      if (ce->decl()->builtins.fs != nullptr) {
        return ce->decl()->builtins.fs(env, ce);
      }

      if (ce->decl()->e() == nullptr) {
        std::ostringstream ss;
        ss << "internal error: missing builtin '" << ce->id() << "'";
        throw EvalError(env, Expression::loc(ce), ss.str());
      }

      return eval_call<EvalFloatSet>(env, ce);
    } break;
    case Expression::E_LET: {
      Let* l = Expression::cast<Let>(e);
      LetPushBindings lpb(l);
      for (unsigned int i = 0; i < l->let().size(); i++) {
        // Evaluate all variable declarations
        if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
          vdi->e(eval_par(env, vdi->e()));
          check_par_declaration(env, vdi);
        } else {
          // This is a constraint item. Since the let is par,
          // it can only be a par bool expression. If it evaluates
          // to false, it means that the value of this let is undefined.
          if (!eval_bool(env, l->let()[i])) {
            throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                       "constraint in let failed");
          }
        }
      }
      FloatSetVal* ret = eval_floatset(env, l->in());
      return ret;
    } break;
    default:
      assert(false);
  }
  return nullptr;
}

bool eval_bool(EnvI& env, Expression* e) {
  CallStackItem csi(env, e);
  try {
    if (auto* bl = Expression::dynamicCast<BoolLit>(e)) {
      return bl->v();
    }
    switch (Expression::eid(e)) {
      case Expression::E_INTLIT:
      case Expression::E_FLOATLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_ARRAYLIT:
      case Expression::E_COMP:
      case Expression::E_VARDECL:
      case Expression::E_TI:
        assert(false);
        throw EvalError(env, Expression::loc(e), "not a bool expression");
        break;
      case Expression::E_ID: {
        GCLock lock;
        return eval_id<EvalBoolLit>(env, e)->v();
      } break;
      case Expression::E_ARRAYACCESS: {
        GCLock lock;
        return eval_bool(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
      } break;
      case Expression::E_FIELDACCESS: {
        return eval_bool(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
      } break;
      case Expression::E_ITE: {
        ITE* ite = Expression::cast<ITE>(e);
        for (unsigned int i = 0; i < ite->size(); i++) {
          if (eval_bool(env, ite->ifExpr(i))) {
            return eval_bool(env, ite->thenExpr(i));
          }
        }
        return eval_bool(env, ite->elseExpr());
      } break;
      case Expression::E_BINOP: {
        auto* bo = Expression::cast<BinOp>(e);
        Expression* lhs = bo->lhs();
        if (Expression::type(lhs).bt() == Type::BT_TOP || Expression::type(lhs).isbot()) {
          lhs = eval_par(env, lhs);
        }
        Expression* rhs = bo->rhs();
        if (Expression::type(rhs).bt() == Type::BT_TOP || Expression::type(rhs).isbot()) {
          rhs = eval_par(env, rhs);
        }
        if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
          return eval_call<EvalBoolVal, BinOp>(env, bo);
        }

        if (Expression::type(lhs).isbool() && Expression::type(rhs).isbool()) {
          try {
            switch (bo->op()) {
              case BOT_LE:
                return static_cast<int>(eval_bool(env, lhs)) <
                       static_cast<int>(eval_bool(env, rhs));
              case BOT_LQ:
                return static_cast<int>(eval_bool(env, lhs)) <=
                       static_cast<int>(eval_bool(env, rhs));
              case BOT_GR:
                return static_cast<int>(eval_bool(env, lhs)) >
                       static_cast<int>(eval_bool(env, rhs));
              case BOT_GQ:
                return static_cast<int>(eval_bool(env, lhs)) >=
                       static_cast<int>(eval_bool(env, rhs));
              case BOT_EQ:
                return eval_bool(env, lhs) == eval_bool(env, rhs);
              case BOT_NQ:
                return eval_bool(env, lhs) != eval_bool(env, rhs);
              case BOT_EQUIV:
                return eval_bool(env, lhs) == eval_bool(env, rhs);
              case BOT_IMPL:
                return (!eval_bool(env, lhs)) || eval_bool(env, rhs);
              case BOT_RIMPL:
                return (!eval_bool(env, rhs)) || eval_bool(env, lhs);
              case BOT_OR:
                return eval_bool(env, lhs) || eval_bool(env, rhs);
              case BOT_AND:
                return eval_bool(env, lhs) && eval_bool(env, rhs);
              case BOT_XOR:
                return eval_bool(env, lhs) ^ eval_bool(env, rhs);
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (Expression::type(lhs).isint() && Expression::type(rhs).isint()) {
          try {
            IntVal v0 = eval_int(env, lhs);
            IntVal v1 = eval_int(env, rhs);
            switch (bo->op()) {
              case BOT_LE:
                return v0 < v1;
              case BOT_LQ:
                return v0 <= v1;
              case BOT_GR:
                return v0 > v1;
              case BOT_GQ:
                return v0 >= v1;
              case BOT_EQ:
                return v0 == v1;
              case BOT_NQ:
                return v0 != v1;
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (Expression::type(lhs).isfloat() && Expression::type(rhs).isfloat()) {
          try {
            FloatVal v0 = eval_float(env, lhs);
            FloatVal v1 = eval_float(env, rhs);
            switch (bo->op()) {
              case BOT_LE:
                return v0 < v1;
              case BOT_LQ:
                return v0 <= v1;
              case BOT_GR:
                return v0 > v1;
              case BOT_GQ:
                return v0 >= v1;
              case BOT_EQ:
                return v0 == v1;
              case BOT_NQ:
                return v0 != v1;
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (Expression::type(lhs).isint() && Expression::type(rhs).isIntSet()) {
          try {
            IntVal v0 = eval_int(env, lhs);
            GCLock lock;
            IntSetVal* v1 = eval_intset(env, rhs);
            switch (bo->op()) {
              case BOT_IN:
                return v1->contains(v0);
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (Expression::type(lhs).isfloat() && Expression::type(rhs).isFloatSet()) {
          try {
            FloatVal v0 = eval_float(env, lhs);
            GCLock lock;
            FloatSetVal* v1 = eval_floatset(env, rhs);
            switch (bo->op()) {
              case BOT_IN:
                return v1->contains(v0);
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (Expression::type(lhs).isSet() && Expression::type(rhs).isSet()) {
          try {
            GCLock lock;
            IntSetVal* v0 = eval_intset(env, lhs);
            IntSetVal* v1 = eval_intset(env, rhs);
            IntSetRanges ir0(v0);
            IntSetRanges ir1(v1);
            switch (bo->op()) {
              case BOT_LE:
                return Ranges::less(ir0, ir1);
              case BOT_LQ:
                return Ranges::less_eq(ir0, ir1);
              case BOT_GR:
                return Ranges::less(ir1, ir0);
              case BOT_GQ:
                return Ranges::less_eq(ir1, ir0);
              case BOT_EQ:
                return Ranges::equal(ir0, ir1);
              case BOT_NQ:
                return !Ranges::equal(ir0, ir1);
              case BOT_SUBSET:
                return Ranges::subset(ir0, ir1);
              case BOT_SUPERSET:
                return Ranges::subset(ir1, ir0);
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (Expression::type(lhs).isstring() && Expression::type(rhs).isstring()) {
          try {
            GCLock lock;
            std::string s0 = eval_string(env, lhs);
            std::string s1 = eval_string(env, rhs);
            switch (bo->op()) {
              case BOT_EQ:
                return s0 == s1;
              case BOT_NQ:
                return s0 != s1;
              case BOT_LE:
                return s0 < s1;
              case BOT_LQ:
                return s0 <= s1;
              case BOT_GR:
                return s0 > s1;
              case BOT_GQ:
                return s0 >= s1;
              default:
                assert(false);
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (bo->op() == BOT_EQ && Expression::type(lhs).isAnn()) {
          // follow ann id to value, since there might be indirection (e.g. func argument, see
          // test_equality_of_indirect_annotations.mzn)
          return Expression::equal(follow_id_to_value(lhs), follow_id_to_value(rhs));
        } else if (Expression::type(lhs).structBT()) {
          assert(Expression::type(rhs).bt() == Expression::type(lhs).bt());
          try {
            auto struct_equal = [&](ArrayLit* structA, ArrayLit* structB) {
              for (unsigned int i = 0; i < structA->size(); ++i) {
                if (!Expression::equal(eval_par(env, (*structA)[i]),
                                       eval_par(env, (*structB)[i]))) {
                  return false;
                }
              }
              return true;
            };
            auto struct_less = [&](ArrayLit* structA, ArrayLit* structB, bool allow_equal) {
              for (unsigned int i = 0; i < structA->size(); ++i) {
                Expression* parA = eval_par(env, (*structA)[i]);
                Expression* parB = eval_par(env, (*structB)[i]);
                if (!Expression::equal(parA, parB)) {
                  KeepAlive binop;
                  {
                    GCLock lock;
                    binop = new BinOp(Location().introduce(), parA, BOT_LE, parB);
                    Expression::type(binop(), Type::parbool());
                  }
                  return eval_bool(env, binop());
                }
              }
              return allow_equal;
            };
            ArrayLit* struct0 = eval_array_lit(env, lhs);
            ArrayLit* struct1 = eval_array_lit(env, rhs);
            switch (bo->op()) {
              case BOT_EQ:
                return struct_equal(struct0, struct1);
              case BOT_NQ:
                return !struct_equal(struct0, struct1);
              case BOT_LE:
                return struct_less(struct0, struct1, false);
              case BOT_LQ:
                return struct_less(struct0, struct1, true);
              case BOT_GR:
                return struct_less(struct1, struct0, false);
              case BOT_GQ:
                return struct_less(struct1, struct0, true);
              case BOT_IN: {
                // Note: tup1 is an array of tuples
                for (unsigned int i = 0; i < struct1->size(); ++i) {
                  if (struct_equal(struct0, eval_array_lit(env, (*struct1)[0]))) {
                    return true;
                  }
                }
                return false;
              }
              default:
                throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
            }
            return true;
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else if (bo->op() == BOT_EQ && Expression::type(lhs).dim() > 0 &&
                   Expression::type(rhs).dim() > 0) {
          try {
            ArrayLit* al0 = eval_array_lit(env, lhs);
            ArrayLit* al1 = eval_array_lit(env, rhs);
            if (al0->size() != al1->size()) {
              return false;
            }
            for (unsigned int i = 0; i < al0->size(); i++) {
              if (!Expression::equal(eval_par(env, (*al0)[i]), eval_par(env, (*al1)[i]))) {
                return false;
              }
            }
            return true;
          } catch (ResultUndefinedError&) {
            return false;
          }
        } else {
          assert(false);
          throw EvalError(env, Expression::loc(e), "not a bool expression", bo->opToString());
        }
      } break;
      case Expression::E_UNOP: {
        UnOp* uo = Expression::cast<UnOp>(e);
        if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
          return eval_call<EvalBoolVal, UnOp>(env, uo);
        }
        bool v0 = eval_bool(env, uo->e());
        switch (uo->op()) {
          case UOT_NOT:
            return !v0;
          default:
            assert(false);
            throw EvalError(env, Expression::loc(e), "not a bool expression", uo->opToString());
        }
      } break;
      case Expression::E_CALL: {
        try {
          Call* ce = Expression::cast<Call>(e);
          if (ce->decl() == nullptr) {
            throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
          }

          if (ce->decl()->builtins.b != nullptr) {
            if (!env.fopts.debug && (ce->id() == env.constants.ids.assert_dbg ||
                                     ce->id() == env.constants.ids.trace_dbg)) {
              return true;
            }
            return ce->decl()->builtins.b(env, ce);
          }

          if (ce->decl()->builtins.e != nullptr) {
            return eval_bool(env, ce->decl()->builtins.e(env, ce));
          }

          if (ce->decl()->e() == nullptr) {
            std::ostringstream ss;
            ss << "internal error: missing builtin '" << ce->id() << "'";
            throw EvalError(env, Expression::loc(ce), ss.str());
          }

          return eval_call<EvalBoolVal>(env, ce);
        } catch (ResultUndefinedError&) {
          return false;
        }
      } break;
      case Expression::E_LET: {
        Let* l = Expression::cast<Let>(e);
        LetPushBindings lpb(l);
        bool ret = true;
        for (unsigned int i = 0; i < l->let().size(); i++) {
          // Evaluate all variable declarations
          if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
            vdi->e(eval_par(env, vdi->e()));
            bool maybe_partial = Expression::ann(vdi).contains(env.constants.ann.maybe_partial);
            if (maybe_partial) {
              env.inMaybePartial++;
            }
            try {
              check_par_declaration(env, vdi);
            } catch (ResultUndefinedError&) {
              ret = false;
            }
            if (maybe_partial) {
              env.inMaybePartial--;
            }
          } else {
            // This is a constraint item. Since the let is par,
            // it can only be a par bool expression. If it evaluates
            // to false, it means that the value of this let is false.
            if (!eval_bool(env, l->let()[i])) {
              if (Expression::ann(l->let()[i]).contains(env.constants.ann.maybe_partial)) {
                ret = false;
              } else {
                throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                           "constraint in let failed");
              }
            }
          }
        }
        ret = ret && eval_bool(env, l->in());
        return ret;
      } break;
      default:
        assert(false);
        return false;
    }
  } catch (ResultUndefinedError&) {
    // undefined means false
    return false;
  }
  return false;
}

IntSetVal* eval_boolset(EnvI& env, Expression* e) {
  CallStackItem csi(env, e);
  switch (Expression::eid(e)) {
    case Expression::E_SETLIT: {
      auto* sl = Expression::cast<SetLit>(e);
      if (sl->isv() != nullptr) {
        return sl->isv();
      }
      std::vector<IntVal> vals;
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        Expression* vi = eval_par(env, sl->v()[i]);
        if (vi != env.constants.absent) {
          vals.push_back(eval_int(env, vi));
        }
      }
      return IntSetVal::a(vals);
    }
    case Expression::E_BOOLLIT:
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_VARDECL:
    case Expression::E_TI:
      throw EvalError(env, Expression::loc(e), "not a set of bool expression");
      break;
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
      std::vector<IntVal> vals(al->size());
      for (unsigned int i = 0; i < al->size(); i++) {
        vals[i] = static_cast<long long>(eval_bool(env, (*al)[i]));
      }
      return IntSetVal::a(vals);
    } break;
    case Expression::E_COMP: {
      auto* c = Expression::cast<Comprehension>(e);
      auto a = eval_comp<EvalIntVal>(env, c);
      return IntSetVal::a(a.a);
    }
    case Expression::E_ID: {
      GCLock lock;
      return eval_id<EvalBoolSetLit>(env, e)->isv();
    } break;
    case Expression::E_ARRAYACCESS: {
      GCLock lock;
      return eval_boolset(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
    } break;
    case Expression::E_FIELDACCESS: {
      return eval_boolset(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
    } break;
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      for (unsigned int i = 0; i < ite->size(); i++) {
        if (eval_bool(env, ite->ifExpr(i))) {
          return eval_boolset(env, ite->thenExpr(i));
        }
      }
      return eval_boolset(env, ite->elseExpr());
    } break;
    case Expression::E_BINOP: {
      auto* bo = Expression::cast<BinOp>(e);
      if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
        return eval_call<EvalBoolSet, BinOp>(env, bo);
      }
      Expression* lhs = eval_par(env, bo->lhs());
      Expression* rhs = eval_par(env, bo->rhs());
      if (Expression::type(lhs).isIntSet() && Expression::type(rhs).isIntSet()) {
        IntSetVal* v0 = eval_boolset(env, lhs);
        IntSetVal* v1 = eval_boolset(env, rhs);
        IntSetRanges ir0(v0);
        IntSetRanges ir1(v1);
        switch (bo->op()) {
          case BOT_UNION: {
            Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            return IntSetVal::ai(u);
          }
          case BOT_DIFF: {
            Ranges::Diff<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            return IntSetVal::ai(u);
          }
          case BOT_SYMDIFF: {
            Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> i(ir0, ir1);
            Ranges::Diff<IntVal, Ranges::Union<IntVal, IntSetRanges, IntSetRanges>,
                         Ranges::Inter<IntVal, IntSetRanges, IntSetRanges>>
                sd(u, i);
            return IntSetVal::ai(sd);
          }
          case BOT_INTERSECT: {
            Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> u(ir0, ir1);
            return IntSetVal::ai(u);
          }
          default:
            throw EvalError(env, Expression::loc(e), "not a set of bool expression",
                            bo->opToString());
        }
      } else if (Expression::type(lhs).isbool() && Expression::type(rhs).isbool()) {
        if (bo->op() != BOT_DOTDOT) {
          throw EvalError(env, Expression::loc(e), "not a set of bool expression",
                          bo->opToString());
        }
        return IntSetVal::a(static_cast<long long>(eval_bool(env, lhs)),
                            static_cast<long long>(eval_bool(env, rhs)));
      } else {
        throw EvalError(env, Expression::loc(e), "not a set of bool expression", bo->opToString());
      }
    } break;
    case Expression::E_UNOP: {
      UnOp* uo = Expression::cast<UnOp>(e);
      if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
        return eval_call<EvalBoolSet, UnOp>(env, uo);
      }
      throw EvalError(env, Expression::loc(e), "not a set of bool expression");
    }
    case Expression::E_CALL: {
      Call* ce = Expression::cast<Call>(e);
      if (ce->decl() == nullptr) {
        throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
      }

      if (ce->decl()->builtins.s != nullptr) {
        return ce->decl()->builtins.s(env, ce);
      }

      if (ce->decl()->builtins.e != nullptr) {
        return eval_boolset(env, ce->decl()->builtins.e(env, ce));
      }

      if (ce->decl()->e() == nullptr) {
        std::ostringstream ss;
        ss << "internal error: missing builtin '" << ce->id() << "'";
        throw EvalError(env, Expression::loc(ce), ss.str());
      }

      return eval_call<EvalBoolSet>(env, ce);
    } break;
    case Expression::E_LET: {
      Let* l = Expression::cast<Let>(e);
      LetPushBindings lpb(l);
      for (unsigned int i = 0; i < l->let().size(); i++) {
        // Evaluate all variable declarations
        if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
          vdi->e(eval_par(env, vdi->e()));
          check_par_declaration(env, vdi);
        } else {
          // This is a constraint item. Since the let is par,
          // it can only be a par bool expression. If it evaluates
          // to false, it means that the value of this let is undefined.
          if (!eval_bool(env, l->let()[i])) {
            throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                       "constraint in let failed");
          }
        }
      }
      IntSetVal* ret = eval_boolset(env, l->in());
      return ret;
    } break;
    default:
      assert(false);
  }
  return nullptr;
}

IntVal eval_int_internal(EnvI& env, Expression* e) {
  if (Expression::type(e).isbool()) {
    return static_cast<long long>(eval_bool(env, e));
  }
  if (auto* il = Expression::dynamicCast<IntLit>(e)) {
    return IntLit::v(il);
  }
  CallStackItem csi(env, e);
  try {
    switch (Expression::eid(e)) {
      case Expression::E_FLOATLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_ARRAYLIT:
      case Expression::E_COMP:
      case Expression::E_VARDECL:
      case Expression::E_TI:
        throw EvalError(env, Expression::loc(e), "not an integer expression");
        break;
      case Expression::E_ID: {
        GCLock lock;
        return IntLit::v(eval_id<EvalIntLit>(env, e));
      } break;
      case Expression::E_ARRAYACCESS: {
        GCLock lock;
        return eval_int(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
      } break;
      case Expression::E_FIELDACCESS: {
        return eval_int(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
      } break;
      case Expression::E_ITE: {
        ITE* ite = Expression::cast<ITE>(e);
        for (unsigned int i = 0; i < ite->size(); i++) {
          if (eval_bool(env, ite->ifExpr(i))) {
            return eval_int(env, ite->thenExpr(i));
          }
        }
        return eval_int(env, ite->elseExpr());
      } break;
      case Expression::E_BINOP: {
        auto* bo = Expression::cast<BinOp>(e);
        if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
          return eval_call<EvalIntVal, BinOp>(env, bo);
        }
        IntVal v0 = eval_int(env, bo->lhs());
        IntVal v1 = eval_int(env, bo->rhs());
        switch (bo->op()) {
          case BOT_PLUS:
            return v0 + v1;
          case BOT_MINUS:
            return v0 - v1;
          case BOT_MULT:
            return v0 * v1;
          case BOT_POW:
            if (v0 == 0 && v1 < 0) {
              throw ResultUndefinedError(env, Expression::loc(e),
                                         "negative power of zero is undefined");
            }
            return v0.pow(v1);
          case BOT_IDIV:
            if (v1 == 0) {
              throw ResultUndefinedError(env, Expression::loc(e), "division by zero");
            }
            return v0 / v1;
          case BOT_MOD:
            if (v1 == 0) {
              throw ResultUndefinedError(env, Expression::loc(e), "division by zero");
            }
            return v0 % v1;
          default:
            throw EvalError(env, Expression::loc(e), "not an integer expression", bo->opToString());
        }
      } break;
      case Expression::E_UNOP: {
        UnOp* uo = Expression::cast<UnOp>(e);
        if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
          return eval_call<EvalIntVal, UnOp>(env, uo);
        }
        IntVal v0 = eval_int(env, uo->e());
        switch (uo->op()) {
          case UOT_PLUS:
            return v0;
          case UOT_MINUS:
            return -v0;
          default:
            throw EvalError(env, Expression::loc(e), "not an integer expression", uo->opToString());
        }
      } break;
      case Expression::E_CALL: {
        Call* ce = Expression::cast<Call>(e);
        if (ce->decl() == nullptr) {
          throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
        }
        if (ce->decl()->builtins.i != nullptr) {
          return ce->decl()->builtins.i(env, ce);
        }

        if (ce->decl()->builtins.e != nullptr) {
          return eval_int(env, ce->decl()->builtins.e(env, ce));
        }

        if (ce->decl()->e() == nullptr) {
          std::ostringstream ss;
          ss << "internal error: missing builtin '" << ce->id() << "'";
          throw EvalError(env, Expression::loc(ce), ss.str());
        }

        return eval_call<EvalIntVal>(env, ce);
      } break;
      case Expression::E_LET: {
        Let* l = Expression::cast<Let>(e);
        LetPushBindings lpb(l);
        for (unsigned int i = 0; i < l->let().size(); i++) {
          // Evaluate all variable declarations
          if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
            vdi->e(eval_par(env, vdi->e()));
            check_par_declaration(env, vdi);
          } else {
            // This is a constraint item. Since the let is par,
            // it can only be a par bool expression. If it evaluates
            // to false, it means that the value of this let is undefined.
            if (!eval_bool(env, l->let()[i])) {
              throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                         "constraint in let failed");
            }
          }
        }
        IntVal ret = eval_int(env, l->in());
        return ret;
      } break;
      default:
        assert(false);
        return 0;
    }
  } catch (ArithmeticError& err) {
    throw EvalError(env, Expression::loc(e), err.msg());
  }
  return 0;
}

FloatVal eval_float(EnvI& env, Expression* e) {
  if (Expression::type(e).isint()) {
    return static_cast<double>(eval_int(env, e).toInt());
  }
  if (Expression::type(e).isbool()) {
    return static_cast<double>(eval_bool(env, e));
  }
  CallStackItem csi(env, e);
  try {
    if (auto* fl = Expression::dynamicCast<FloatLit>(e)) {
      return FloatLit::v(fl);
    }
    switch (Expression::eid(e)) {
      case Expression::E_INTLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_ARRAYLIT:
      case Expression::E_COMP:
      case Expression::E_VARDECL:
      case Expression::E_TI:
        throw EvalError(env, Expression::loc(e), "not a float expression");
        break;
      case Expression::E_ID: {
        GCLock lock;
        return FloatLit::v(eval_id<EvalFloatLit>(env, e));
      } break;
      case Expression::E_ARRAYACCESS: {
        GCLock lock;
        return eval_float(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
      } break;
      case Expression::E_FIELDACCESS: {
        return eval_float(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
      } break;
      case Expression::E_ITE: {
        ITE* ite = Expression::cast<ITE>(e);
        for (unsigned int i = 0; i < ite->size(); i++) {
          if (eval_bool(env, ite->ifExpr(i))) {
            return eval_float(env, ite->thenExpr(i));
          }
        }
        return eval_float(env, ite->elseExpr());
      } break;
      case Expression::E_BINOP: {
        auto* bo = Expression::cast<BinOp>(e);
        if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
          return eval_call<EvalFloatVal, BinOp>(env, bo);
        }
        FloatVal v0 = eval_float(env, bo->lhs());
        FloatVal v1 = eval_float(env, bo->rhs());
        switch (bo->op()) {
          case BOT_PLUS:
            return v0 + v1;
          case BOT_MINUS:
            return v0 - v1;
          case BOT_MULT:
            return v0 * v1;
          case BOT_POW:
            return std::pow(v0.toDouble(), v1.toDouble());
          case BOT_DIV:
            if (v1 == 0.0) {
              throw ResultUndefinedError(env, Expression::loc(e), "division by zero");
            }
            return v0 / v1;
          default:
            throw EvalError(env, Expression::loc(e), "not a float expression", bo->opToString());
        }
      } break;
      case Expression::E_UNOP: {
        UnOp* uo = Expression::cast<UnOp>(e);
        if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
          return eval_call<EvalFloatVal, UnOp>(env, uo);
        }
        FloatVal v0 = eval_float(env, uo->e());
        switch (uo->op()) {
          case UOT_PLUS:
            return v0;
          case UOT_MINUS:
            return -v0;
          default:
            throw EvalError(env, Expression::loc(e), "not a float expression", uo->opToString());
        }
      } break;
      case Expression::E_CALL: {
        Call* ce = Expression::cast<Call>(e);
        if (ce->decl() == nullptr) {
          throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
        }
        if (ce->decl()->builtins.f != nullptr) {
          return ce->decl()->builtins.f(env, ce);
        }

        if (ce->decl()->builtins.e != nullptr) {
          return eval_float(env, ce->decl()->builtins.e(env, ce));
        }

        if (ce->decl()->e() == nullptr) {
          std::ostringstream ss;
          ss << "internal error: missing builtin '" << ce->id() << "'";
          throw EvalError(env, Expression::loc(ce), ss.str());
        }

        return eval_call<EvalFloatVal>(env, ce);
      } break;
      case Expression::E_LET: {
        Let* l = Expression::cast<Let>(e);
        LetPushBindings lpb(l);
        for (unsigned int i = 0; i < l->let().size(); i++) {
          // Evaluate all variable declarations
          if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
            vdi->e(eval_par(env, vdi->e()));
            check_par_declaration(env, vdi);
          } else {
            // This is a constraint item. Since the let is par,
            // it can only be a par bool expression. If it evaluates
            // to false, it means that the value of this let is undefined.
            if (!eval_bool(env, l->let()[i])) {
              throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                         "constraint in let failed");
            }
          }
        }
        FloatVal ret = eval_float(env, l->in());
        return ret;
      } break;
      default:
        assert(false);
        return 0.0;
    }
  } catch (ArithmeticError& err) {
    throw EvalError(env, Expression::loc(e), err.msg());
  }
  return 0.0;
}

std::string eval_string(EnvI& env, Expression* e) {
  CallStackItem csi(env, e);
  switch (Expression::eid(e)) {
    case Expression::E_STRINGLIT: {
      ASTString str = Expression::cast<StringLit>(e)->v();
      return std::string(str.c_str(), str.size());
    }
    case Expression::E_FLOATLIT:
    case Expression::E_INTLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_ANON:
    case Expression::E_TIID:
    case Expression::E_SETLIT:
    case Expression::E_ARRAYLIT:
    case Expression::E_COMP:
    case Expression::E_VARDECL:
    case Expression::E_TI:
      throw EvalError(env, Expression::loc(e), "not a string expression");
      break;
    case Expression::E_ID: {
      GCLock lock;
      ASTString str = eval_id<EvalStringLit>(env, e)->v();
      return std::string(str.c_str(), str.size());
    } break;
    case Expression::E_ARRAYACCESS: {
      GCLock lock;
      return eval_string(env, eval_arrayaccess(env, Expression::cast<ArrayAccess>(e)));
    } break;
    case Expression::E_FIELDACCESS: {
      return eval_string(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
    } break;
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      for (unsigned int i = 0; i < ite->size(); i++) {
        if (eval_bool(env, ite->ifExpr(i))) {
          return eval_string(env, ite->thenExpr(i));
        }
      }
      return eval_string(env, ite->elseExpr());
    } break;
    case Expression::E_BINOP: {
      auto* bo = Expression::cast<BinOp>(e);
      if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
        return eval_call<EvalString, BinOp>(env, bo);
      }
      std::string v0 = eval_string(env, bo->lhs());
      std::string v1 = eval_string(env, bo->rhs());
      switch (bo->op()) {
        case BOT_PLUSPLUS:
          return v0 + v1;
        default:
          throw EvalError(env, Expression::loc(e), "not a string expression", bo->opToString());
      }
    } break;
    case Expression::E_UNOP: {
      UnOp* uo = Expression::cast<UnOp>(e);
      if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
        return eval_call<EvalString, UnOp>(env, uo);
      }
      throw EvalError(env, Expression::loc(e), "not a string expression");
    } break;
    case Expression::E_CALL: {
      Call* ce = Expression::cast<Call>(e);
      if (ce->decl() == nullptr) {
        throw EvalError(env, Expression::loc(e), "undeclared function", ce->id());
      }

      if (ce->decl()->builtins.str != nullptr) {
        return ce->decl()->builtins.str(env, ce);
      }
      if (ce->decl()->builtins.e != nullptr) {
        return eval_string(env, ce->decl()->builtins.e(env, ce));
      }

      if (ce->decl()->e() == nullptr) {
        std::ostringstream ss;
        ss << "internal error: missing builtin '" << ce->id() << "'";
        throw EvalError(env, Expression::loc(ce), ss.str());
      }

      return eval_call<EvalString>(env, ce);
    } break;
    case Expression::E_LET: {
      Let* l = Expression::cast<Let>(e);
      LetPushBindings lpb(l);
      for (unsigned int i = 0; i < l->let().size(); i++) {
        // Evaluate all variable declarations
        if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
          vdi->e(eval_par(env, vdi->e()));
          check_par_declaration(env, vdi);
        } else {
          // This is a constraint item. Since the let is par,
          // it can only be a par bool expression. If it evaluates
          // to false, it means that the value of this let is undefined.
          if (!eval_bool(env, l->let()[i])) {
            throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                       "constraint in let failed");
          }
        }
      }
      std::string ret = eval_string(env, l->in());
      return ret;
    } break;
    default:
      assert(false);
  }
  return "";
}

Expression* eval_par(EnvI& env, Expression* e) {
  if (e == nullptr) {
    return nullptr;
  }
  switch (Expression::eid(e)) {
    case Expression::E_ANON:
    case Expression::E_TIID: {
      return e;
    }
    case Expression::E_COMP:
      if (Expression::cast<Comprehension>(e)->set()) {
        return EvalSetLit::e(env, e);
      }
      // fall through
    case Expression::E_ARRAYLIT: {
      ArrayLit* al = eval_array_lit(env, e);
      std::vector<Expression*> args(al->size());
      bool allFlat = true;
      for (unsigned int i = 0; i < al->size(); i++) {
        Expression* ali = (*al)[i];
        if (!Expression::isa<IntLit>(ali) && !Expression::isa<FloatLit>(ali) &&
            !Expression::isa<BoolLit>(ali) &&
            !(Expression::isa<SetLit>(ali) && Expression::cast<SetLit>(ali)->evaluated())) {
          allFlat = false;
          args[i] = eval_par(env, ali);
        } else {
          args[i] = ali;
        }
      }
      if (allFlat) {
        return al;
      }
      ArrayLit* ret = nullptr;
      if (al->isTuple()) {
        ret = ArrayLit::constructTuple(Expression::loc(al), args);
      } else {
        std::vector<std::pair<int, int>> dims(al->dims());
        for (unsigned int i = al->dims(); (i--) != 0U;) {
          dims[i].first = al->min(i);
          dims[i].second = al->max(i);
        }
        ret = new ArrayLit(Expression::loc(al), args, dims);
      }
      Type t = al->type();
      if (t.isbot() && !ret->empty()) {
        t.bt(Expression::type((*ret)[0]).bt());
      }
      ret->type(t);
      return ret;
    }
    case Expression::E_VARDECL: {
      auto* vd = Expression::cast<VarDecl>(e);
      throw EvalError(env, Expression::loc(vd), "cannot evaluate variable declaration",
                      vd->id()->v());
    }
    case Expression::E_TI: {
      auto* t = Expression::cast<TypeInst>(e);
      ASTExprVec<TypeInst> r;
      if (!t->ranges().empty()) {
        std::vector<TypeInst*> rv(t->ranges().size());
        for (unsigned int i = t->ranges().size(); (i--) != 0U;) {
          rv[i] = static_cast<TypeInst*>(eval_par(env, t->ranges()[i]));
        }
        r = ASTExprVec<TypeInst>(rv);
      }
      return new TypeInst(Location(), t->type(), r, eval_par(env, t->domain()));
    }
    case Expression::E_ID: {
      if (e == env.constants.absent) {
        return e;
      }
      Id* id = Expression::cast<Id>(e);
      if (id->decl() == nullptr) {
        if (id->type().isAnn()) {
          return id;
        }
        throw EvalError(env, Expression::loc(e), "undefined identifier", id->v());
      }
      if (id->decl()->ti()->domain() != nullptr && id->decl()->type().isPresent()) {
        if (auto* bl = Expression::dynamicCast<BoolLit>(id->decl()->ti()->domain())) {
          return bl;
        }
        if (id->decl()->ti()->type().isint()) {
          if (auto* sl = Expression::dynamicCast<SetLit>(id->decl()->ti()->domain())) {
            if ((sl->isv() != nullptr) && !sl->isv()->empty() &&
                sl->isv()->min() == sl->isv()->max()) {
              return IntLit::a(sl->isv()->min());
            }
          }
        } else if (id->decl()->ti()->type().isfloat()) {
          if (id->decl()->ti()->domain() != nullptr) {
            FloatSetVal* fsv = eval_floatset(env, id->decl()->ti()->domain());
            if (!fsv->empty() && fsv->min() == fsv->max()) {
              return FloatLit::a(fsv->min());
            }
          }
        }
      }
      if (id->decl()->e() == nullptr) {
        return id;
      }
      return eval_par(env, id->decl()->e());
    }
    case Expression::E_STRINGLIT:
      return e;
    default: {
      if (Expression::type(e).dim() != 0) {
        ArrayLit* al = eval_array_lit(env, e);
        std::vector<Expression*> args(al->size());
        for (unsigned int i = al->size(); (i--) != 0U;) {
          args[i] = eval_par(env, (*al)[i]);
        }
        std::vector<std::pair<int, int>> dims(al->dims());
        for (unsigned int i = al->dims(); (i--) != 0U;) {
          dims[i].first = al->min(i);
          dims[i].second = al->max(i);
        }
        auto* ret = new ArrayLit(Expression::loc(al), args, dims);
        Type t = al->type();
        if ((t.bt() == Type::BT_BOT || t.bt() == Type::BT_TOP) && !ret->empty()) {
          t.bt(Expression::type((*ret)[0]).bt());
        }
        ret->type(t);
        return ret;
      }
      if (Expression::type(e).isPar()) {
        if (Expression::type(e).isSet() && !Expression::type(e).isOpt()) {
          return EvalSetLit::e(env, e);
        }
        if (Expression::type(e) == Type::parint()) {
          return EvalIntLit::e(env, e);
        }
        if (Expression::type(e) == Type::parbool()) {
          return EvalBoolLit::e(env, e);
        }
        if (Expression::type(e) == Type::parfloat()) {
          return EvalFloatLit::e(env, e);
        }
        if (Expression::type(e) == Type::parstring()) {
          return EvalStringLit::e(env, e);
        }
      }
      switch (Expression::eid(e)) {
        case Expression::E_ITE: {
          ITE* ite = Expression::cast<ITE>(e);
          if (Expression::type(ite).isOptBot()) {
            // Must be absent
            return env.constants.absent;
          }
          for (unsigned int i = 0; i < ite->size(); i++) {
            if (Expression::type(ite->ifExpr(i)) == Type::parbool()) {
              if (eval_bool(env, ite->ifExpr(i))) {
                return eval_par(env, ite->thenExpr(i));
              }
            } else {
              std::vector<Expression*> e_ifthen(static_cast<size_t>(ite->size()) * 2);
              for (unsigned int i = 0; i < ite->size(); i++) {
                e_ifthen[2 * static_cast<size_t>(i)] = eval_par(env, ite->ifExpr(i));
                e_ifthen[2 * static_cast<size_t>(i) + 1] = eval_par(env, ite->thenExpr(i));
              }
              ITE* n_ite = new ITE(Expression::loc(ite), e_ifthen, eval_par(env, ite->elseExpr()));
              n_ite->type(ite->type());
              return n_ite;
            }
          }
          return eval_par(env, ite->elseExpr());
        }
        case Expression::E_CALL: {
          Call* c = Expression::cast<Call>(e);
          if (c->decl() != nullptr) {
            if (c->decl()->builtins.e != nullptr) {
              return eval_par(env, c->decl()->builtins.e(env, c));
            }
            if (c->decl()->e() == nullptr) {
              if (c->id() == env.constants.ids.deopt &&
                  Expression::equal(c->arg(0), env.constants.absent)) {
                throw ResultUndefinedError(env, Expression::loc(e), "deopt(<>) is undefined");
              }
            } else {
              return eval_call<EvalPar>(env, c);
            }
          }
          std::vector<Expression*> args(c->argCount());
          for (unsigned int i = 0; i < args.size(); i++) {
            if (Expression::type(c->arg(i)).isPar()) {
              args[i] = eval_par(env, c->arg(i));
            } else {
              args[i] = c->arg(i);
            }
          }
          Call* nc = Call::a(Expression::loc(c), c->id(), args);
          nc->type(c->type());
          return nc;
        }
        case Expression::E_BINOP: {
          auto* bo = Expression::cast<BinOp>(e);
          if ((bo->decl() != nullptr) && (bo->decl()->e() != nullptr)) {
            return eval_call<EvalPar, BinOp>(env, bo);
          }
          auto* nbo = new BinOp(Expression::loc(e), eval_par(env, bo->lhs()), bo->op(),
                                eval_par(env, bo->rhs()));
          nbo->type(bo->type());
          if (nbo->op() == BOT_PLUSPLUS && nbo->type().structBT()) {
            assert(Expression::type(nbo->lhs()).structBT() &&
                   Expression::type(nbo->lhs()).bt() == Expression::type(nbo->rhs()).bt() &&
                   Expression::type(nbo->lhs()).dim() == 0 &&
                   Expression::type(nbo->rhs()).dim() == 0);
            return eval_array_lit(env, nbo);
          }
          return nbo;
        }
        case Expression::E_UNOP: {
          UnOp* uo = Expression::cast<UnOp>(e);
          if ((uo->decl() != nullptr) && (uo->decl()->e() != nullptr)) {
            return eval_call<EvalPar, UnOp>(env, uo);
          }
          UnOp* nuo = new UnOp(Expression::loc(e), uo->op(), eval_par(env, uo->e()));
          nuo->type(uo->type());
          return nuo;
        }
        case Expression::E_ARRAYACCESS: {
          auto* aa = Expression::cast<ArrayAccess>(e);
          for (unsigned int i = 0; i < aa->idx().size(); i++) {
            if (!Expression::type(aa->idx()[i]).isPar()) {
              std::vector<Expression*> idx(aa->idx().size());
              for (unsigned int j = 0; j < aa->idx().size(); j++) {
                idx[j] = eval_par(env, aa->idx()[j]);
              }
              auto* aa_new = new ArrayAccess(Expression::loc(e), eval_par(env, aa->v()), idx);
              aa_new->type(aa->type());
              return aa_new;
            }
          }
          return eval_par(env, eval_arrayaccess(env, aa));
        }
        case Expression::E_FIELDACCESS: {
          return eval_par(env, eval_fieldaccess(env, Expression::cast<FieldAccess>(e)));
        } break;
        case Expression::E_LET: {
          Let* l = Expression::cast<Let>(e);
          assert(l->type().isPar());
          LetPushBindings lpb(l);
          for (unsigned int i = 0; i < l->let().size(); i++) {
            // Evaluate all variable declarations
            if (auto* vdi = Expression::dynamicCast<VarDecl>(l->let()[i])) {
              vdi->e(eval_par(env, vdi->e()));
              check_par_declaration(env, vdi);
            } else {
              // This is a constraint item. Since the let is par,
              // it can only be a par bool expression. If it evaluates
              // to false, it means that the value of this let is undefined.
              if (!eval_bool(env, l->let()[i])) {
                throw ResultUndefinedError(env, Expression::loc(l->let()[i]),
                                           "constraint in let failed");
              }
            }
          }
          Expression* ret = eval_par(env, l->in());
          return ret;
        }
        default:
          return e;
      }
    }
  }
}

class ComputeIntBounds : public EVisitor {
public:
  typedef std::pair<IntVal, IntVal> Bounds;
  std::vector<Bounds> bounds;
  bool valid;
  EnvI& env;
  ComputeIntBounds(EnvI& env0) : valid(true), env(env0) {}
  bool enter(Expression* e) {
    if (Expression::type(e).isAnn()) {
      return false;
    }
    if (Expression::isa<VarDecl>(e)) {
      return false;
    }
    if (Expression::type(e).dim() > 0) {
      return false;
    }
    if (Expression::type(e).isPar() && !Expression::type(e).cv()) {
      Expression* exp = eval_par(env, e);
      if (Expression::type(e).isbool() && exp != env.constants.absent) {
        auto* b = Expression::cast<BoolLit>(exp);
        IntVal i = b->v() ? IntVal(1) : IntVal(0);
        bounds.emplace_back(i, i);
      } else if (Expression::type(e).isint() && exp != env.constants.absent) {
        IntVal v = IntLit::v(Expression::cast<IntLit>(exp));
        bounds.emplace_back(v, v);
      } else {
        valid = false;
      }
      return false;
    }
    if (Expression::type(e).isint()) {
      if (ITE* ite = Expression::dynamicCast<ITE>(e)) {
        Bounds itebounds(IntVal::infinity(), -IntVal::infinity());
        for (unsigned int i = 0; i < ite->size(); i++) {
          if (Expression::type(ite->ifExpr(i)).isPar() &&
              static_cast<int>(Expression::type(ite->ifExpr(i)).cv()) == Type::CV_NO) {
            if (eval_bool(env, ite->ifExpr(i))) {
              BottomUpIterator<ComputeIntBounds> cbi(*this);
              cbi.run(ite->thenExpr(i));
              Bounds& back = bounds.back();
              back.first = std::min(itebounds.first, back.first);
              back.second = std::max(itebounds.second, back.second);
              return false;
            }
          } else {
            BottomUpIterator<ComputeIntBounds> cbi(*this);
            cbi.run(ite->thenExpr(i));
            Bounds back = bounds.back();
            bounds.pop_back();
            itebounds.first = std::min(itebounds.first, back.first);
            itebounds.second = std::max(itebounds.second, back.second);
          }
        }
        BottomUpIterator<ComputeIntBounds> cbi(*this);
        cbi.run(ite->elseExpr());
        Bounds& back = bounds.back();
        back.first = std::min(itebounds.first, back.first);
        back.second = std::max(itebounds.second, back.second);
        return false;
      }
      return true;
    }
    if (Expression::type(e).isbool()) {
      if (Expression::isa<BoolLit>(e) || Expression::isa<Id>(e)) {
        return true;
      }
      bounds.emplace_back(0, 1);
      return false;
    }
    valid = false;
    return false;
  }
  /// Visit integer literal
  void vIntLit(const IntLit* i) { bounds.emplace_back(IntLit::v(i), IntLit::v(i)); }
  /// Visit floating point literal
  void vFloatLit(const FloatLit* /*f*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit Boolean literal
  void vBoolLit(const BoolLit* b) {
    IntVal i = b->v() ? IntVal(1) : IntVal(0);
    bounds.emplace_back(i, i);
  }
  /// Visit set literal
  void vSetLit(const SetLit* /*sl*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit string literal
  void vStringLit(const StringLit* /*sl*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit identifier
  void vId(const Id* id) {
    VarDecl* vd = id->decl();
    while ((vd->flat() != nullptr) && vd->flat() != vd) {
      vd = vd->flat();
    }
    if (vd->ti()->domain() != nullptr) {
      GCLock lock;
      if (vd->type().isbool()) {
        bool b = eval_bool(env, vd->ti()->domain());
        IntVal i = b ? IntVal(1) : IntVal(0);
        bounds.emplace_back(i, i);
      } else {
        IntSetVal* isv = eval_intset(env, vd->ti()->domain());
        if (isv->empty()) {
          valid = false;
          bounds.emplace_back(0, 0);
        } else {
          bounds.emplace_back(isv->min(0), isv->max(isv->size() - 1));
        }
      }
    } else {
      if (vd->e() != nullptr) {
        BottomUpIterator<ComputeIntBounds> cbi(*this);
        cbi.run(vd->e());
      } else if (vd->type().isbool()) {
        bounds.emplace_back(0, 1);
      } else {
        bounds.emplace_back(-IntVal::infinity(), IntVal::infinity());
      }
    }
  }
  /// Visit anonymous variable
  void vAnonVar(const AnonVar* /*v*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit array literal
  void vArrayLit(const ArrayLit* /*al*/) {}
  /// Visit array access
  void vArrayAccess(ArrayAccess* aa) {
    bool parAccess = true;
    for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
      bounds.pop_back();
      if (!Expression::type(aa->idx()[i]).isPar()) {
        parAccess = false;
      }
    }
    if (Id* id = Expression::dynamicCast<Id>(aa->v())) {
      while ((id->decl()->e() != nullptr) && Expression::isa<Id>(id->decl()->e())) {
        id = Expression::cast<Id>(id->decl()->e());
      }
      if (parAccess && (id->decl()->e() != nullptr)) {
        ArrayAccessSucess success;
        Expression* e = eval_arrayaccess(env, aa, success);
        if (success()) {
          BottomUpIterator<ComputeIntBounds> cbi(*this);
          cbi.run(e);
          return;
        }
      }
      if (id->decl()->ti()->domain() != nullptr) {
        GCLock lock;
        IntSetVal* isv = eval_intset(env, id->decl()->ti()->domain());
        if (!isv->empty()) {
          bounds.emplace_back(isv->min(0), isv->max(isv->size() - 1));
          return;
        }
      }
    }
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit array comprehension
  void vComprehension(const Comprehension* /*c*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit if-then-else
  void vITE(const ITE* /*ite*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit binary operator
  void vBinOp(const BinOp* bo) {
    Bounds b1 = bounds.back();
    bounds.pop_back();
    Bounds b0 = bounds.back();
    bounds.pop_back();
    if (!b1.first.isFinite() || !b1.second.isFinite() || !b0.first.isFinite() ||
        !b0.second.isFinite()) {
      valid = false;
      bounds.emplace_back(0, 0);
    } else {
      switch (bo->op()) {
        case BOT_PLUS:
          bounds.emplace_back(b0.first + b1.first, b0.second + b1.second);
          break;
        case BOT_MINUS:
          bounds.emplace_back(b0.first - b1.second, b0.second - b1.first);
          break;
        case BOT_MULT: {
          IntVal x0 = b0.first * b1.first;
          IntVal x1 = b0.first * b1.second;
          IntVal x2 = b0.second * b1.first;
          IntVal x3 = b0.second * b1.second;
          IntVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
          IntVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
          bounds.emplace_back(m, n);
        } break;
        case BOT_IDIV: {
          IntVal b0f = b0.first == 0 ? 1 : b0.first;
          IntVal b0s = b0.second == 0 ? -1 : b0.second;
          IntVal b1f = b1.first == 0 ? 1 : b1.first;
          IntVal b1s = b1.second == 0 ? -1 : b1.second;
          IntVal x0 = b0f / b1f;
          IntVal x1 = b0f / b1s;
          IntVal x2 = b0s / b1f;
          IntVal x3 = b0s / b1s;
          IntVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
          IntVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
          bounds.emplace_back(m, n);
        } break;
        case BOT_MOD: {
          IntVal b0f = b0.first == 0 ? 1 : b0.first;
          IntVal b0s = b0.second == 0 ? -1 : b0.second;
          IntVal b1f = b1.first == 0 ? 1 : b1.first;
          IntVal b1s = b1.second == 0 ? -1 : b1.second;
          IntVal x0 = b0f % b1f;
          IntVal x1 = b0f % b1s;
          IntVal x2 = b0s % b1f;
          IntVal x3 = b0s % b1s;
          IntVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
          IntVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
          bounds.emplace_back(m, n);
        } break;
        case BOT_POW:
        case BOT_DIV:
        case BOT_LE:
        case BOT_LQ:
        case BOT_GR:
        case BOT_GQ:
        case BOT_EQ:
        case BOT_NQ:
        case BOT_IN:
        case BOT_SUBSET:
        case BOT_SUPERSET:
        case BOT_UNION:
        case BOT_DIFF:
        case BOT_SYMDIFF:
        case BOT_INTERSECT:
        case BOT_PLUSPLUS:
        case BOT_EQUIV:
        case BOT_IMPL:
        case BOT_RIMPL:
        case BOT_OR:
        case BOT_AND:
        case BOT_XOR:
        case BOT_DOTDOT:
          valid = false;
          bounds.emplace_back(0, 0);
      }
    }
  }
  /// Visit unary operator
  void vUnOp(const UnOp* uo) {
    switch (uo->op()) {
      case UOT_PLUS:
        break;
      case UOT_MINUS:
        bounds.back().first = -bounds.back().first;
        bounds.back().second = -bounds.back().second;
        std::swap(bounds.back().first, bounds.back().second);
        break;
      case UOT_NOT:
        valid = false;
    }
  }
  /// Visit call
  void vCall(Call* c) {
    if (c->id() == env.constants.ids.lin_exp || c->id() == env.constants.ids.sum) {
      bool le = c->id() == env.constants.ids.lin_exp;
      ArrayLit* coeff = le ? eval_array_lit(env, c->arg(0)) : nullptr;
      if (Expression::type(c->arg(le ? 1 : 0)).isOpt()) {
        valid = false;
        bounds.emplace_back(0, 0);
        return;
      }
      auto* al = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(le ? 1 : 0)));
      if (al == nullptr) {
        // can't use the array directly
        valid = false;
        bounds.emplace_back(0, 0);
        return;
      }
      if (le) {
        bounds.pop_back();  // remove constant (third arg) from stack
      }

      IntVal d = le ? IntLit::v(Expression::cast<IntLit>(c->arg(2))) : 0;
      int stacktop = static_cast<int>(bounds.size());
      for (unsigned int i = al->size(); (i--) != 0U;) {
        BottomUpIterator<ComputeIntBounds> cbi(*this);
        cbi.run((*al)[i]);
        if (!valid) {
          for (unsigned int j = al->size() - 1; j > i; j--) {
            bounds.pop_back();
          }
          return;
        }
      }
      assert(stacktop + al->size() == bounds.size());
      IntVal lb = d;
      IntVal ub = d;
      for (unsigned int i = 0; i < al->size(); i++) {
        Bounds b = bounds.back();
        bounds.pop_back();
        IntVal cv = le ? eval_int(env, (*coeff)[i]) : 1;
        if (cv > 0) {
          if (b.first.isFinite()) {
            if (lb.isFinite()) {
              lb += cv * b.first;
            }
          } else {
            lb = b.first;
          }
          if (b.second.isFinite()) {
            if (ub.isFinite()) {
              ub += cv * b.second;
            }
          } else {
            ub = b.second;
          }
        } else {
          if (b.second.isFinite()) {
            if (lb.isFinite()) {
              lb += cv * b.second;
            }
          } else {
            lb = -b.second;
          }
          if (b.first.isFinite()) {
            if (ub.isFinite()) {
              ub += cv * b.first;
            }
          } else {
            ub = -b.first;
          }
        }
      }
      bounds.emplace_back(lb, ub);
    } else if (c->id() == env.constants.ids.card) {
      if (IntSetVal* isv = compute_intset_bounds(env, c->arg(0))) {
        IntSetRanges isr(isv);
        bounds.emplace_back(0, Ranges::cardinality(isr));
      } else {
        valid = false;
        bounds.emplace_back(0, 0);
      }
    } else if (c->id() == env.constants.ids.int_.times) {
      Bounds b1 = bounds.back();
      bounds.pop_back();
      Bounds b0 = bounds.back();
      bounds.pop_back();
      if (!b1.first.isFinite() || !b1.second.isFinite() || !b0.first.isFinite() ||
          !b0.second.isFinite()) {
        valid = false;
        bounds.emplace_back(0, 0);
      } else {
        IntVal x0 = b0.first * b1.first;
        IntVal x1 = b0.first * b1.second;
        IntVal x2 = b0.second * b1.first;
        IntVal x3 = b0.second * b1.second;
        IntVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
        IntVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
        bounds.emplace_back(m, n);
      }
    } else if (c->id() == env.constants.ids.bool2int) {
      bounds.back().first = std::max(0, bounds.back().first);
      bounds.back().second = std::min(1, bounds.back().second);
    } else if (c->id() == env.constants.ids.abs) {
      Bounds b0 = bounds.back();
      if (b0.first < 0) {
        bounds.pop_back();
        if (b0.second < 0) {
          bounds.emplace_back(-b0.second, -b0.first);
        } else {
          bounds.emplace_back(0, std::max(-b0.first, b0.second));
        }
      }
    } else if (c->id() == env.constants.ids.on_restart.uniform_on_restart) {
      if (c->argCount() == 2) {
        Bounds b0 = bounds.back();
        bounds.pop_back();
        Bounds b1 = bounds.back();
        bounds.pop_back();
        assert(b0.first == b0.second && b1.first == b1.second);
        bounds.emplace_back(b0.first, b1.first);
      } else {
        // Take bounds of the argument
      }
    } else if (c->id() == env.constants.ids.on_restart.sol ||
               c->id() == env.constants.ids.on_restart.last_val) {
      // Take bounds of the argument
    } else if ((c->decl() != nullptr) && (c->decl()->ti()->domain() != nullptr) &&
               !Expression::isa<TIId>(c->decl()->ti()->domain())) {
      for (unsigned int i = 0; i < c->argCount(); i++) {
        if (Expression::type(c->arg(i)).isint()) {
          assert(!bounds.empty());
          bounds.pop_back();
        }
      }
      IntSetVal* isv = eval_intset(env, c->decl()->ti()->domain());
      if (isv->empty()) {
        bounds.emplace_back(1, 0);
      } else {
        bounds.emplace_back(isv->min(), isv->max());
      }
    } else {
      valid = false;
      bounds.emplace_back(0, 0);
    }
  }
  /// Visit let
  void vLet(const Let* /*l*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit variable declaration
  void vVarDecl(const VarDecl* /*vd*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit annotation
  void vAnnotation(const Annotation* /*e*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit type inst
  void vTypeInst(const TypeInst* /*e*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  /// Visit TIId
  void vTIId(const TIId* /*e*/) {
    valid = false;
    bounds.emplace_back(0, 0);
  }
  // Visit field access
  void vFieldAccess(const FieldAccess* fa) {
    // TODO: actually allow bounds computation
    valid = false;
    bounds.emplace_back(0, 0);
  }
};

IntBounds compute_int_bounds(EnvI& env, Expression* e) {
  try {
    ComputeIntBounds cb(env);
    BottomUpIterator<ComputeIntBounds> cbi(cb);
    cbi.run(e);
    if (cb.valid) {
      assert(cb.bounds.size() == 1);
      return IntBounds(cb.bounds.back().first, cb.bounds.back().second, true);
    }
    if (Expression::type(e).isbool()) {
      return IntBounds(0, 1, true);
    }
    return IntBounds(0, 0, false);

  } catch (ResultUndefinedError&) {
    return IntBounds(0, 0, false);
  }
}

class ComputeFloatBounds : public EVisitor {
protected:
  typedef std::pair<FloatVal, FloatVal> FBounds;

public:
  std::vector<FBounds> bounds;
  bool valid;
  EnvI& env;
  ComputeFloatBounds(EnvI& env0) : valid(true), env(env0) {}
  bool enter(Expression* e) {
    if (Expression::type(e).isAnn()) {
      return false;
    }
    if (Expression::isa<VarDecl>(e)) {
      return false;
    }
    if (Expression::type(e).dim() > 0) {
      return false;
    }
    if (Expression::type(e).isPar()) {
      Expression* exp = eval_par(env, e);
      if (exp == env.constants.absent) {
        valid = false;
      } else if (Expression::type(e).isfloat()) {
        FloatVal v = FloatLit::v(Expression::cast<FloatLit>(exp));
        bounds.emplace_back(v, v);
        return false;
      }
      valid = false;
      return false;
    }
    if (Expression::type(e).isfloat()) {
      if (ITE* ite = Expression::dynamicCast<ITE>(e)) {
        FBounds itebounds(FloatVal::infinity(), -FloatVal::infinity());
        for (unsigned int i = 0; i < ite->size(); i++) {
          if (Expression::type(ite->ifExpr(i)).isPar() &&
              static_cast<int>(Expression::type(ite->ifExpr(i)).cv()) == Type::CV_NO) {
            if (eval_bool(env, ite->ifExpr(i))) {
              BottomUpIterator<ComputeFloatBounds> cbi(*this);
              cbi.run(ite->thenExpr(i));
              FBounds& back = bounds.back();
              back.first = std::min(itebounds.first, back.first);
              back.second = std::max(itebounds.second, back.second);
              return false;
            }
          } else {
            BottomUpIterator<ComputeFloatBounds> cbi(*this);
            cbi.run(ite->thenExpr(i));
            FBounds back = bounds.back();
            bounds.pop_back();
            itebounds.first = std::min(itebounds.first, back.first);
            itebounds.second = std::max(itebounds.second, back.second);
          }
        }
        BottomUpIterator<ComputeFloatBounds> cbi(*this);
        cbi.run(ite->elseExpr());
        FBounds& back = bounds.back();
        back.first = std::min(itebounds.first, back.first);
        back.second = std::max(itebounds.second, back.second);
        return false;
      }
      return true;
    }
    return false;
  }
  /// Visit integer literal
  void vIntLit(const IntLit* /*i*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit floating point literal
  void vFloatLit(const FloatLit* f) { bounds.emplace_back(FloatLit::v(f), FloatLit::v(f)); }
  /// Visit Boolean literal
  void vBoolLit(const BoolLit* /*b*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit set literal
  void vSetLit(const SetLit* /*sl*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit string literal
  void vStringLit(const StringLit* /*sl*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit identifier
  void vId(const Id* id) {
    VarDecl* vd = id->decl();
    while ((vd->flat() != nullptr) && vd->flat() != vd) {
      vd = vd->flat();
    }
    if (vd->ti()->domain() != nullptr) {
      GCLock lock;
      FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
      if (fsv->empty()) {
        valid = false;
        bounds.emplace_back(0, 0);
      } else {
        bounds.emplace_back(fsv->min(0), fsv->max(fsv->size() - 1));
      }
    } else {
      if (vd->e() != nullptr) {
        BottomUpIterator<ComputeFloatBounds> cbi(*this);
        cbi.run(vd->e());
      } else {
        bounds.emplace_back(-FloatVal::infinity(), FloatVal::infinity());
      }
    }
  }
  /// Visit anonymous variable
  void vAnonVar(const AnonVar* /*v*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit array literal
  void vArrayLit(const ArrayLit* /*al*/) {}
  /// Visit array access
  void vArrayAccess(ArrayAccess* aa) {
    bool parAccess = true;
    for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
      if (!Expression::type(aa->idx()[i]).isPar()) {
        parAccess = false;
      }
    }
    if (Id* id = Expression::dynamicCast<Id>(aa->v())) {
      while ((id->decl()->e() != nullptr) && Expression::isa<Id>(id->decl()->e())) {
        id = Expression::cast<Id>(id->decl()->e());
      }
      if (parAccess && (id->decl()->e() != nullptr)) {
        ArrayAccessSucess success;
        Expression* e = eval_arrayaccess(env, aa, success);
        if (success()) {
          BottomUpIterator<ComputeFloatBounds> cbi(*this);
          cbi.run(e);
          return;
        }
      }
      if (id->decl()->ti()->domain() != nullptr) {
        FloatSetVal* fsv = eval_floatset(env, id->decl()->ti()->domain());
        if (fsv->empty()) {
          bounds.emplace_back(1, 0);
        } else {
          bounds.emplace_back(fsv->min(), fsv->max());
        }
        return;
      }
    }
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit array comprehension
  void vComprehension(const Comprehension* /*c*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit if-then-else
  void vITE(const ITE* /*ite*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit binary operator
  void vBinOp(const BinOp* bo) {
    FBounds b1 = bounds.back();
    bounds.pop_back();
    FBounds b0 = bounds.back();
    bounds.pop_back();
    if (!b1.first.isFinite() || !b1.second.isFinite() || !b0.first.isFinite() ||
        !b0.second.isFinite()) {
      valid = false;
      bounds.emplace_back(0.0, 0.0);
    } else {
      switch (bo->op()) {
        case BOT_PLUS:
          bounds.emplace_back(b0.first + b1.first, b0.second + b1.second);
          break;
        case BOT_MINUS:
          bounds.emplace_back(b0.first - b1.second, b0.second - b1.first);
          break;
        case BOT_MULT: {
          FloatVal x0 = b0.first * b1.first;
          FloatVal x1 = b0.first * b1.second;
          FloatVal x2 = b0.second * b1.first;
          FloatVal x3 = b0.second * b1.second;
          FloatVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
          FloatVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
          bounds.emplace_back(m, n);
        } break;
        case BOT_POW: {
          FloatVal x0 = std::pow(b0.first.toDouble(), b1.first.toDouble());
          FloatVal x1 = std::pow(b0.first.toDouble(), b1.second.toDouble());
          FloatVal x2 = std::pow(b0.second.toDouble(), b1.first.toDouble());
          FloatVal x3 = std::pow(b0.second.toDouble(), b1.second.toDouble());
          FloatVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
          FloatVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
          bounds.emplace_back(m, n);
        } break;
        case BOT_DIV: {
          if (0.0 == std::fabs(b0.first.toDouble()) && 0.0 == std::fabs(b0.second.toDouble())) {
            bounds.emplace_back(0.0, 0.0);
            break;
          }
          if (0.0 >= b1.first.toDouble() * b1.second.toDouble()) {
            valid = false;
            bounds.emplace_back(0.0, 0.0);
            break;
          }
          FloatVal b0f = b0.first;
          FloatVal b0s = b0.second;
          FloatVal b1f = b1.first;
          FloatVal b1s = b1.second;
          FloatVal x0 = b0f / b1f;
          FloatVal x1 = b0f / b1s;
          FloatVal x2 = b0s / b1f;
          FloatVal x3 = b0s / b1s;
          FloatVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
          FloatVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
          bounds.emplace_back(m, n);
        } break;
        case BOT_IDIV:
        case BOT_MOD:
        case BOT_LE:
        case BOT_LQ:
        case BOT_GR:
        case BOT_GQ:
        case BOT_EQ:
        case BOT_NQ:
        case BOT_IN:
        case BOT_SUBSET:
        case BOT_SUPERSET:
        case BOT_UNION:
        case BOT_DIFF:
        case BOT_SYMDIFF:
        case BOT_INTERSECT:
        case BOT_PLUSPLUS:
        case BOT_EQUIV:
        case BOT_IMPL:
        case BOT_RIMPL:
        case BOT_OR:
        case BOT_AND:
        case BOT_XOR:
        case BOT_DOTDOT:
          valid = false;
          bounds.emplace_back(0.0, 0.0);
      }
    }
  }
  /// Visit unary operator
  void vUnOp(const UnOp* uo) {
    switch (uo->op()) {
      case UOT_PLUS:
        break;
      case UOT_MINUS:
        bounds.back().first = -bounds.back().first;
        bounds.back().second = -bounds.back().second;
        break;
      case UOT_NOT:
        valid = false;
        bounds.emplace_back(0.0, 0.0);
    }
  }
  /// Visit call
  void vCall(Call* c) {
    if (c->id() == env.constants.ids.lin_exp || c->id() == env.constants.ids.sum) {
      bool le = c->id() == env.constants.ids.lin_exp;
      ArrayLit* coeff = le ? eval_array_lit(env, c->arg(0)) : nullptr;
      if (le) {
        bounds.pop_back();  // remove constant (third arg) from stack
      }
      if (Expression::type(c->arg(le ? 1 : 0)).isOpt()) {
        valid = false;
        bounds.emplace_back(0.0, 0.0);
        return;
      }
      auto* al = Expression::dynamicCast<ArrayLit>(follow_id(c->arg(le ? 1 : 0)));
      if (al == nullptr) {
        // can't use the array directly
        valid = false;
        bounds.emplace_back(0, 0);
        return;
      }
      FloatVal d = le ? FloatLit::v(Expression::cast<FloatLit>(c->arg(2))) : 0.0;
      int stacktop = static_cast<int>(bounds.size());
      for (unsigned int i = al->size(); (i--) != 0U;) {
        BottomUpIterator<ComputeFloatBounds> cbi(*this);
        cbi.run((*al)[i]);
        if (!valid) {
          return;
        }
      }
      assert(stacktop + al->size() == bounds.size());
      FloatVal lb = d;
      FloatVal ub = d;
      for (unsigned int i = 0; i < (*al).size(); i++) {
        FBounds b = bounds.back();
        bounds.pop_back();
        FloatVal cv = le ? eval_float(env, (*coeff)[i]) : 1.0;

        if (cv > 0) {
          if (b.first.isFinite()) {
            if (lb.isFinite()) {
              lb += cv * b.first;
            }
          } else {
            lb = b.first;
          }
          if (b.second.isFinite()) {
            if (ub.isFinite()) {
              ub += cv * b.second;
            }
          } else {
            ub = b.second;
          }
        } else {
          if (b.second.isFinite()) {
            if (lb.isFinite()) {
              lb += cv * b.second;
            }
          } else {
            lb = -b.second;
          }
          if (b.first.isFinite()) {
            if (ub.isFinite()) {
              ub += cv * b.first;
            }
          } else {
            ub = -b.first;
          }
        }
      }
      bounds.emplace_back(lb, ub);
    } else if (c->id() == env.constants.ids.float_.times) {
      BottomUpIterator<ComputeFloatBounds> cbi(*this);
      cbi.run(c->arg(0));
      cbi.run(c->arg(1));
      FBounds b1 = bounds.back();
      bounds.pop_back();
      FBounds b0 = bounds.back();
      bounds.pop_back();
      if (!b1.first.isFinite() || !b1.second.isFinite() || !b0.first.isFinite() ||
          !b0.second.isFinite()) {
        valid = false;
        bounds.emplace_back(0, 0);
      } else {
        FloatVal x0 = b0.first * b1.first;
        FloatVal x1 = b0.first * b1.second;
        FloatVal x2 = b0.second * b1.first;
        FloatVal x3 = b0.second * b1.second;
        FloatVal m = std::min(x0, std::min(x1, std::min(x2, x3)));
        FloatVal n = std::max(x0, std::max(x1, std::max(x2, x3)));
        bounds.emplace_back(m, n);
      }
    } else if (c->id() == env.constants.ids.int2float) {
      ComputeIntBounds ib(env);
      BottomUpIterator<ComputeIntBounds> cbi(ib);
      cbi.run(c->arg(0));
      if (!ib.valid) {
        valid = false;
      }
      ComputeIntBounds::Bounds result = ib.bounds.back();
      if (!result.first.isFinite() || !result.second.isFinite()) {
        valid = false;
        bounds.emplace_back(0.0, 0.0);
      } else {
        bounds.emplace_back(static_cast<double>(result.first.toInt()),
                            static_cast<double>(result.second.toInt()));
      }
    } else if (c->id() == env.constants.ids.abs) {
      BottomUpIterator<ComputeFloatBounds> cbi(*this);
      cbi.run(c->arg(0));
      FBounds b0 = bounds.back();
      if (b0.first < 0) {
        bounds.pop_back();
        if (b0.second < 0) {
          bounds.emplace_back(-b0.second, -b0.first);
        } else {
          bounds.emplace_back(0.0, std::max(-b0.first, b0.second));
        }
      }
    } else if (c->id() == env.constants.ids.on_restart.uniform_on_restart) {
      FBounds b0 = bounds.back();
      bounds.pop_back();
      FBounds b1 = bounds.back();
      bounds.pop_back();
      assert(b0.first == b0.second && b1.first == b1.second);
      bounds.emplace_back(b0.first, b1.first);
    } else if (c->id() == env.constants.ids.on_restart.sol ||
               c->id() == env.constants.ids.on_restart.last_val) {
      // Take bounds of the argument
    } else if ((c->decl() != nullptr) && (c->decl()->ti()->domain() != nullptr) &&
               !Expression::isa<TIId>(c->decl()->ti()->domain())) {
      for (unsigned int i = 0; i < c->argCount(); i++) {
        if (Expression::type(c->arg(i)).isfloat()) {
          assert(!bounds.empty());
          bounds.pop_back();
        }
      }
      FloatSetVal* fsv = eval_floatset(env, c->decl()->ti()->domain());
      if (fsv->empty()) {
        bounds.emplace_back(1, 0);
      } else {
        bounds.emplace_back(fsv->min(), fsv->max());
      }
    } else {
      valid = false;
      bounds.emplace_back(0.0, 0.0);
    }
  }
  /// Visit let
  void vLet(const Let* /*l*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit variable declaration
  void vVarDecl(const VarDecl* /*vd*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit annotation
  void vAnnotation(const Annotation* /*e*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit type inst
  void vTypeInst(const TypeInst* /*e*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
  /// Visit TIId
  void vTIId(const TIId* /*e*/) {
    valid = false;
    bounds.emplace_back(0.0, 0.0);
  }
};

FloatBounds compute_float_bounds(EnvI& env, Expression* e) {
  try {
    ComputeFloatBounds cb(env);
    BottomUpIterator<ComputeFloatBounds> cbi(cb);
    cbi.run(e);
    if (cb.valid) {
      assert(!cb.bounds.empty());
      return FloatBounds(cb.bounds.back().first, cb.bounds.back().second, true);
    }
    return FloatBounds(0.0, 0.0, false);

  } catch (ResultUndefinedError&) {
    return FloatBounds(0.0, 0.0, false);
  }
}

class ComputeIntSetBounds : public EVisitor {
public:
  std::vector<IntSetVal*> bounds;
  bool valid;
  EnvI& env;
  ComputeIntSetBounds(EnvI& env0) : valid(true), env(env0) {}
  bool enter(Expression* e) {
    if (Expression::type(e).isAnn()) {
      return false;
    }
    if (Expression::isa<VarDecl>(e)) {
      return false;
    }
    if (Expression::type(e).dim() > 0) {
      return false;
    }
    if (!Expression::type(e).isIntSet()) {
      return false;
    }
    if (Expression::type(e).isPar()) {
      bounds.push_back(eval_intset(env, e));
      return false;
    }
    return true;
  }
  /// Visit set literal
  void vSetLit(const SetLit* sl) {
    assert(sl->type().isvar());
    assert(sl->isv() == nullptr);

    IntSetVal* isv = IntSetVal::a();
    for (unsigned int i = 0; i < sl->v().size(); i++) {
      IntSetRanges i0(isv);
      IntBounds ib = compute_int_bounds(env, sl->v()[i]);
      if (!ib.valid || !ib.l.isFinite() || !ib.u.isFinite()) {
        valid = false;
        bounds.push_back(nullptr);
        return;
      }
      Ranges::Const<IntVal> cr(ib.l, ib.u);
      Ranges::Union<IntVal, IntSetRanges, Ranges::Const<IntVal>> u(i0, cr);
      isv = IntSetVal::ai(u);
    }
    bounds.push_back(isv);
  }
  /// Visit identifier
  void vId(const Id* id) {
    if ((id->decl()->ti()->domain() != nullptr) &&
        !Expression::isa<TIId>(id->decl()->ti()->domain())) {
      bounds.push_back(eval_intset(env, id->decl()->ti()->domain()));
    } else {
      if (id->decl()->e() != nullptr) {
        BottomUpIterator<ComputeIntSetBounds> cbi(*this);
        cbi.run(id->decl()->e());
      } else {
        valid = false;
        bounds.push_back(nullptr);
      }
    }
  }
  /// Visit anonymous variable
  void vAnonVar(const AnonVar* /*v*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit array access
  void vArrayAccess(ArrayAccess* aa) {
    bool parAccess = true;
    for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
      if (!Expression::type(aa->idx()[i]).isPar()) {
        parAccess = false;
        break;
      }
    }
    if (Id* id = Expression::dynamicCast<Id>(aa->v())) {
      while ((id->decl()->e() != nullptr) && Expression::isa<Id>(id->decl()->e())) {
        id = Expression::cast<Id>(id->decl()->e());
      }
      if (parAccess && (id->decl()->e() != nullptr)) {
        ArrayAccessSucess success;
        Expression* e = eval_arrayaccess(env, aa, success);
        if (success()) {
          BottomUpIterator<ComputeIntSetBounds> cbi(*this);
          cbi.run(e);
          return;
        }
      }
      if (id->decl()->ti()->domain() != nullptr) {
        bounds.push_back(eval_intset(env, id->decl()->ti()->domain()));
        return;
      }
    }
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit array comprehension
  void vComprehension(const Comprehension* /*c*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit if-then-else
  void vITE(const ITE* /*ite*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit binary operator
  void vBinOp(const BinOp* bo) {
    if (bo->op() == BOT_DOTDOT) {
      IntBounds lb = compute_int_bounds(env, bo->lhs());
      IntBounds ub = compute_int_bounds(env, bo->rhs());
      valid = valid && lb.valid && ub.valid;
      bounds.push_back(IntSetVal::a(lb.l, ub.u));
    } else {
      IntSetVal* b1 = bounds.back();
      bounds.pop_back();
      IntSetVal* b0 = bounds.back();
      bounds.pop_back();
      switch (bo->op()) {
        case BOT_SYMDIFF:
        case BOT_INTERSECT:
        case BOT_UNION: {
          IntSetRanges b0r(b0);
          IntSetRanges b1r(b1);
          Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(b0r, b1r);
          bounds.push_back(IntSetVal::ai(u));
        } break;
        case BOT_DIFF: {
          bounds.push_back(b0);
        } break;
        case BOT_PLUS:
        case BOT_MINUS:
        case BOT_MULT:
        case BOT_POW:
        case BOT_DIV:
        case BOT_IDIV:
        case BOT_MOD:
        case BOT_LE:
        case BOT_LQ:
        case BOT_GR:
        case BOT_GQ:
        case BOT_EQ:
        case BOT_NQ:
        case BOT_IN:
        case BOT_SUBSET:
        case BOT_SUPERSET:
        case BOT_PLUSPLUS:
        case BOT_EQUIV:
        case BOT_IMPL:
        case BOT_RIMPL:
        case BOT_OR:
        case BOT_AND:
        case BOT_XOR:
        case BOT_DOTDOT:
          valid = false;
          bounds.push_back(nullptr);
      }
    }
  }
  /// Visit unary operator
  void vUnOp(const UnOp* /*uo*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit call
  void vCall(Call* c) {
    if (valid &&
        (c->id() == env.constants.ids.set_.intersect || c->id() == env.constants.ids.set_.union_ ||
         c->id() == env.constants.ids.set_.symdiff)) {
      IntSetVal* b0 = bounds.back();
      bounds.pop_back();
      IntSetVal* b1 = bounds.back();
      bounds.pop_back();
      IntSetRanges b0r(b0);
      IntSetRanges b1r(b1);
      Ranges::Union<IntVal, IntSetRanges, IntSetRanges> u(b0r, b1r);
      bounds.push_back(IntSetVal::ai(u));
    } else if (valid && c->id() == env.constants.ids.set_.diff) {
      IntSetVal* b0 = bounds.back();
      bounds.pop_back();
      bounds.pop_back();  // don't need bounds of right hand side
      bounds.push_back(b0);
    } else if ((c->decl() != nullptr) && (c->decl()->ti()->domain() != nullptr) &&
               !Expression::isa<TIId>(c->decl()->ti()->domain())) {
      for (unsigned int i = 0; i < c->argCount(); i++) {
        if (Expression::type(c->arg(i)).isIntSet()) {
          assert(!bounds.empty());
          bounds.pop_back();
        }
      }
      IntSetVal* fsv = eval_intset(env, c->decl()->ti()->domain());
      bounds.push_back(fsv);
    } else {
      valid = false;
      bounds.push_back(nullptr);
    }
  }
  /// Visit let
  void vLet(const Let* /*l*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit variable declaration
  void vVarDecl(const VarDecl* /*vd*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit annotation
  void vAnnotation(const Annotation* /*e*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit type inst
  void vTypeInst(const TypeInst* /*e*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
  /// Visit TIId
  void vTIId(const TIId* /*e*/) {
    valid = false;
    bounds.push_back(nullptr);
  }
};

IntSetVal* compute_intset_bounds(EnvI& env, Expression* e) {
  try {
    ComputeIntSetBounds cb(env);
    BottomUpIterator<ComputeIntSetBounds> cbi(cb);
    cbi.run(e);
    if (cb.valid) {
      return cb.bounds.back();
    }
    return nullptr;

  } catch (ResultUndefinedError&) {
    return nullptr;
  }
}

Expression* follow_id(Expression* e) {
  for (;;) {
    if (e == nullptr) {
      return nullptr;
    }
    if (Expression::eid(e) == Expression::E_ID) {
      Id* ident = Expression::cast<Id>(e);
      if (ident == Constants::constants().absent ||
          (ident->type().isAnn() && ident->decl() == nullptr)) {
        return ident;
      }
      e = ident->decl()->e();
    } else {
      return e;
    }
  }
}

Expression* follow_id_to_decl(Expression* e) {
  for (;;) {
    if (e == nullptr) {
      return nullptr;
    }
    if (e == Constants::constants().absent) {
      return e;
    }
    switch (Expression::eid(e)) {
      case Expression::E_ID: {
        Id* ident = Expression::cast<Id>(e);
        if (ident->type().isAnn() && ident->decl() == nullptr) {
          return ident;
        }
        e = ident->decl();
        break;
      }
      case Expression::E_VARDECL: {
        Expression* vd_e = Expression::cast<VarDecl>(e)->e();
        if (vd_e != nullptr && Expression::isa<Id>(vd_e) && vd_e != Constants::constants().absent &&
            !(Expression::type(vd_e).isAnn() && Expression::cast<Id>(vd_e)->decl() == nullptr)) {
          e = vd_e;
        } else {
          return e;
        }
        break;
      }
      default:
        return e;
    }
  }
}

Expression* follow_id_to_value(Expression* e) {
  Expression* decl = follow_id_to_decl(e);
  if (auto* vd = Expression::dynamicCast<VarDecl>(decl)) {
    if ((vd->e() != nullptr) && Expression::type(vd->e()).isPar()) {
      return vd->e();
    }
    return vd->id();
  }
  return decl;
}

void eval_static_function_body(EnvI& env, FunctionI* decl, Model& toAdd) {
  Expression* e = decl->e();
  if (e == nullptr) {
    return;
  }
  while (Expression::ann(e).contains(env.constants.ann.mzn_evaluate_once)) {
    if (Expression::isa<ITE>(e)) {
      ITE* ite = Expression::cast<ITE>(e);
      if (ite->size() != 1) {
        env.addWarning(Expression::loc(ite),
                       "::mzn_evaluate_once ignored, elseif expressions are not supported");
        return;
      }
      if (!Expression::type(ite->ifExpr(0)).isPar()) {
        env.addWarning(Expression::loc(ite->ifExpr(0)),
                       "::mzn_evaluate_once ignored, var conditions are not supported");
        return;
      }
      if (Expression::type(ite->ifExpr(0)).cv()) {
        env.addWarning(
            Expression::loc(ite->ifExpr(0)),
            "::mzn_evaluate_once ignored, par conditions that contain variables are not supported");
        return;
      }
      GCLock lock;
      bool cond = eval_bool(env, ite->ifExpr(0));
      if (cond) {
        decl->e(ite->thenExpr(0));
        e = decl->e();
      } else {
        decl->e(ite->elseExpr());
        e = decl->e();
      }
    } else if (Expression::isa<Let>(e)) {
      Let* let = Expression::cast<Let>(e);
      if (let->let().size() != 1) {
        env.addWarning(
            Expression::loc(let),
            "::mzn_evaluate_once ignored, lets with more than one declaration are not supported");
        return;
      }
      if (!Expression::type(let->let()[0]).isPar()) {
        env.addWarning(Expression::loc(let),
                       "::mzn_evaluate_once ignored, lets with var declarations are not supported");
        return;
      }
      if (!Expression::isa<VarDecl>(let->let()[0])) {
        env.addWarning(Expression::loc(let),
                       "::mzn_evaluate_once ignored, lets with constraints are not supported");
        return;
      }
      GCLock lock;
      auto* vd = Expression::cast<VarDecl>(let->let()[0]);
      vd->e(eval_par(env, vd->e()));
      check_par_declaration(env, vd);
      vd->toplevel(true);
      vd->id()->idn(env.genId());
      toAdd.addItem(VarDeclI::a(Expression::loc(vd), vd));
      decl->e(let->in());
      e = decl->e();
    } else {
      env.addWarning(Expression::loc(e), "::mzn_evaluate_once ignored, invalid expression");
      return;
    }
  }
}

}  // namespace MiniZinc
