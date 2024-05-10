/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/gc.hh>

#include <cassert>
#include <vector>

namespace MiniZinc {

EE flatten_arrayaccess(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  auto* aa = Expression::cast<ArrayAccess>(e);
  KeepAlive aa_ka = aa;

  Ctx nctx = ctx;
  nctx.b = +nctx.b;
  nctx.neg = false;
  EE eev = flat_exp(env, nctx, aa->v(), nullptr, nctx.partialityVar(env));
  std::vector<EE> ees;

start_flatten_arrayaccess:
  for (unsigned int i = 0; i < aa->idx().size(); i++) {
    Expression* tmp = follow_id_to_decl(aa->idx()[i]);
    if (auto* vd = Expression::dynamicCast<VarDecl>(tmp)) {
      tmp = vd->id();
    }
    if (Expression::type(tmp).isPar()) {
      ArrayLit* al;
      if (Expression::isa<ArrayLit>(eev.r())) {
        al = Expression::cast<ArrayLit>(eev.r());
      } else {
        Id* id = Expression::cast<Id>(eev.r());
        if (id->decl() == nullptr) {
          throw InternalError("undefined identifier");
        }
        if (id->decl()->e() == nullptr) {
          throw InternalError("array without initialiser not supported");
        }
        Expression* id_e = follow_id(id);
        if (Expression::isa<ArrayLit>(id_e)) {
          al = Expression::cast<ArrayLit>(id_e);
        } else {
          throw InternalError("builtin function returning array not supported");
        }
      }

      bool allAbsent = true;
      bool anyAbsent = false;
      std::vector<KeepAlive> elems;
      std::vector<IntVal> idx(aa->idx().size());
      std::vector<std::pair<int, int>> dims;
      std::vector<Expression*> newaccess;
      std::vector<int> nonpar;
      std::vector<int> stack;
      for (int j = 0; j < aa->idx().size(); j++) {
        Expression* tmp = follow_id_to_decl(aa->idx()[j]);
        if (auto* vd = Expression::dynamicCast<VarDecl>(tmp)) {
          tmp = vd->id();
        }
        if (Expression::type(tmp).isPar()) {
          GCLock lock;
          auto* v = eval_par(env, tmp);
          if (v == env.constants.absent) {
            anyAbsent = true;
            idx[j] = al->min(j);
          } else {
            allAbsent = false;
            idx[j] = IntLit::v(Expression::cast<IntLit>(v));
          }
        } else {
          allAbsent = false;
          idx[j] = al->min(j);
          stack.push_back(static_cast<int>(nonpar.size()));
          nonpar.push_back(j);
          dims.emplace_back(al->min(j), al->max(j));
          newaccess.push_back(aa->idx()[j]);
        }
      }
      if (stack.empty()) {
        ArrayAccessSucess success;
        KeepAlive ka;
        if (allAbsent) {
          ka = env.constants.absent;
        } else {
          GCLock lock;
          ka = eval_arrayaccess(env, al, idx, success);
          if (anyAbsent) {
            ka = env.constants.absent;
          }
          if (!success() && env.inMaybePartial == 0) {
            ResultUndefinedError warning(env, Expression::loc(al), success.errorMessage(env, al));
          }
        }
        ees.emplace_back(nullptr, env.constants.boollit(success()));
        ees.emplace_back(nullptr, eev.b());
        if (aa->type().isbool() && !aa->type().isOpt()) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          ees.emplace_back(nullptr, ka());
          ret.r = conj(env, r, ctx, ees);
        } else {
          ret.b = conj(env, b, ctx, ees);
          ret.r = bind(env, ctx, r, ka());
        }
        return ret;
      }
      while (!stack.empty()) {
        int cur = stack.back();
        if (cur == nonpar.size() - 1) {
          stack.pop_back();
          for (int i = al->min(nonpar[cur]); i <= al->max(nonpar[cur]); i++) {
            idx[nonpar[cur]] = i;
            ArrayAccessSucess success;
            GCLock lock;
            Expression* al_idx = eval_arrayaccess(env, al, idx, success);
            if (!success()) {
              if (env.inMaybePartial == 0) {
                ResultUndefinedError warning(env, Expression::loc(al),
                                             success.errorMessage(env, al));
              }
              ees.emplace_back(nullptr, env.constants.literalFalse);
              ees.emplace_back(nullptr, eev.b());
              if (aa->type().isbool() && !aa->type().isOpt()) {
                ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
                ret.r = conj(env, r, ctx, ees);
              } else {
                ret.b = conj(env, b, ctx, ees);
                ret.r = bind(env, ctx, r, al_idx);
              }
              return ret;
            }
            elems.emplace_back(al_idx);
          }
        } else {
          if (idx[nonpar[cur]].toInt() == al->max(nonpar[cur])) {
            idx[nonpar[cur]] = al->min(nonpar[cur]);
            stack.pop_back();
          } else {
            idx[nonpar[cur]]++;
            for (int j = cur + 1; j < nonpar.size(); j++) {
              stack.push_back(j);
            }
          }
        }
      }
      std::vector<Expression*> elems_e(elems.size());
      for (unsigned int i = 0; i < elems.size(); i++) {
        elems_e[i] = elems[i]();
      }
      {
        GCLock lock;
        Expression* newal = new ArrayLit(Expression::loc(al), elems_e, dims);
        Expression::type(
            newal, Type::arrType(env, Type::partop(static_cast<int>(dims.size())), al->type()));
        eev.r = newal;
        auto* n_aa = new ArrayAccess(Expression::loc(aa), newal, newaccess);
        n_aa->type(aa->type());
        aa = n_aa;
        aa_ka = aa;
      }
    }
  }

  if (aa->idx().size() == 1 && Expression::isa<ArrayAccess>(aa->idx()[0])) {
    auto* aa_inner = Expression::cast<ArrayAccess>(aa->idx()[0]);
    ArrayLit* al;
    if (Expression::isa<ArrayLit>(eev.r())) {
      al = Expression::cast<ArrayLit>(eev.r());
    } else {
      Id* id = Expression::cast<Id>(eev.r());
      if (id->decl() == nullptr) {
        throw InternalError("undefined identifier");
      }
      if (id->decl()->e() == nullptr) {
        throw InternalError("array without initialiser not supported");
      }
      al = Expression::cast<ArrayLit>(follow_id(id));
    }
    if (Expression::type(aa_inner->v()).isPar()) {
      KeepAlive ka_al_inner = flat_cv_exp(env, ctx, aa_inner->v());
      auto* al_inner = Expression::cast<ArrayLit>(ka_al_inner());
      std::vector<Expression*> composed_e(al_inner->size());
      for (unsigned int i = 0; i < al_inner->size(); i++) {
        GCLock lock;
        IntVal inner_idx = eval_int(env, (*al_inner)[i]);
        if (inner_idx < al->min(0) || inner_idx > al->max(0)) {
          goto flatten_arrayaccess;
        }
        composed_e[i] = (*al)[static_cast<int>(inner_idx.toInt()) - al->min(0)];
      }
      std::vector<std::pair<int, int>> dims(al_inner->dims());
      for (int i = 0; i < al_inner->dims(); i++) {
        dims[i] = std::make_pair(al_inner->min(i), al_inner->max(i));
      }
      {
        GCLock lock;
        Expression* newal = new ArrayLit(Expression::loc(al), composed_e, dims);
        Type t = al->type();
        t.dim(static_cast<int>(dims.size()));
        Expression::type(newal, t);
        eev.r = newal;
        auto* n_aa = new ArrayAccess(Expression::loc(aa), newal, aa_inner->idx());
        n_aa->type(aa->type());
        aa = n_aa;
        aa_ka = aa;
        goto start_flatten_arrayaccess;
      }
    }
  }
flatten_arrayaccess:
  Ctx dimctx = ctx;
  dimctx.i = C_MIX;
  dimctx.neg = false;
  for (unsigned int i = 0; i < aa->idx().size(); i++) {
    Expression* tmp = follow_id_to_decl(aa->idx()[i]);
    if (auto* vd = Expression::dynamicCast<VarDecl>(tmp)) {
      tmp = vd->id();
    }
    ees.push_back(flat_exp(env, dimctx, tmp, nullptr, dimctx.partialityVar(env)));
  }
  ees.emplace_back(nullptr, eev.b());

  bool parAccess = true;
  for (unsigned int i = 0; i < aa->idx().size(); i++) {
    if (!Expression::type(ees[i].r()).isPar()) {
      parAccess = false;
      break;
    }
  }

  if (parAccess) {
    ArrayLit* al;
    if (Expression::isa<ArrayLit>(eev.r())) {
      al = Expression::cast<ArrayLit>(eev.r());
    } else {
      Id* id = Expression::cast<Id>(eev.r());
      if (id->decl() == nullptr) {
        throw InternalError("undefined identifier");
      }
      if (id->decl()->e() == nullptr) {
        throw InternalError("array without initialiser not supported");
      }
      al = Expression::cast<ArrayLit>(follow_id(id));
    }
    KeepAlive ka;
    ArrayAccessSucess success;
    bool allAbsent = true;
    bool anyAbsent = false;
    {
      GCLock lock;
      std::vector<IntVal> dims(aa->idx().size());
      for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
        auto* v = eval_par(env, ees[i].r());
        if (v == env.constants.absent) {
          anyAbsent = true;
          dims[i] = al->min(i);
        } else {
          allAbsent = false;
          dims[i] = IntLit::v(Expression::cast<IntLit>(v));
        }
      }
      if (allAbsent) {
        ka = env.constants.absent;
      } else {
        ka = eval_arrayaccess(env, al, dims, success);
        if (anyAbsent) {
          ka = env.constants.absent;
        }
      }
    }
    if (!success() && env.inMaybePartial == 0) {
      ResultUndefinedError warning(env, Expression::loc(al), success.errorMessage(env, al));
    }
    ees.emplace_back(nullptr, env.constants.boollit(success()));
    if (aa->type().isbool() && !aa->type().isOpt()) {
      ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
      ees.emplace_back(nullptr, ka());
      ret.r = conj(env, r, ctx, ees);
    } else {
      ret.b = conj(env, b, ctx, ees);
      ret.r = bind(env, ctx, r, ka());
    }
  } else if (aa->type().structBT()) {
    // x[i], where x is an array of tuples, and i is an index variable
    // Strategy: create/flatten a seperate array access for each field, combine to new tuple literal
    assert(Expression::type(eev.r()).bt() == aa->type().bt());

    std::vector<Expression*> idx(aa->idx().size());
    for (size_t i = 0; i < aa->idx().size(); ++i) {
      idx[i] = ees[i].r();
    }

    StructType* res_st = env.getStructType(aa->type());

    // Construct field based array access expressions
    std::vector<KeepAlive> field_aa(res_st->size());
    {
      GCLock lock;
      std::vector<Expression*> field_al = field_slices(env, eev.r());
      assert(res_st->size() == field_al.size());
      for (int i = 0; i < res_st->size(); ++i) {
        field_aa[i] = new ArrayAccess(Expression::loc(aa).introduce(), field_al[i], idx);
        Expression::type(field_aa[i](), (*res_st)[i]);
      }
    }
    // Flatten field based array access expressions
    // WARNING: Expressions stored in field_res, rely on being also in ees to be kept alive
    std::vector<Expression*> field_res(res_st->size());
    for (int i = 0; i < res_st->size(); ++i) {
      // TODO: Does the context need to be changed? Are 'r' and 'b' correct?
      CallStackItem _csi(env, IntLit::a(i));
      EE ee = flat_exp(env, ctx, field_aa[i](), nullptr, b);
      field_res[i] = ee.r();
      ees.push_back(ee);
    }
    {
      GCLock lock;
      ArrayLit* tuple_lit = ArrayLit::constructTuple(Location().introduce(), field_res);
      tuple_lit->type(aa->type());
      ret.r = bind(env, ctx, r, tuple_lit);
      ret.b = conj(env, b, ctx, ees);
    }
  } else {
    std::vector<Expression*> args(aa->idx().size() + 1);
    for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
      args[i] = ees[i].r();
    }
    args[aa->idx().size()] = eev.r();
    KeepAlive ka;
    {
      GCLock lock;
      Call* cc = Call::a(Expression::loc(e).introduce(), env.constants.ids.element, args);
      cc->type(aa->type());
      FunctionI* fi = nullptr;
      try {
        fi = env.model->matchFn(env, cc->id(), args, false);
      } catch (TypeError&) {
        // Actual array is bottom, but previously had a type so use that version
        args[aa->idx().size()] = aa->v();
        fi = env.model->matchFn(env, cc->id(), args, false);
        args[aa->idx().size()] = eev.r();
      }
      if (fi == nullptr) {
        throw FlatteningError(env, Expression::loc(cc), "cannot find matching declaration");
      }
      assert(fi);
      assert(env.isSubtype(fi->rtype(env, args, nullptr, false), cc->type(), false));
      cc->decl(fi);
      ka = cc;
    }
    Ctx elemctx = ctx;
    elemctx.neg = false;
    EE ee = flat_exp(env, elemctx, ka(), nullptr, elemctx.partialityVar(env));
    ees.push_back(ee);
    if (aa->type().isbool() && !aa->type().isOpt()) {
      ee.b = ee.r;
      ees.push_back(ee);
      ret.r = conj(env, r, ctx, ees);
      ret.b = bind(env, ctx, b, env.constants.boollit(!ctx.neg));
    } else {
      ret.r = bind(env, ctx, r, ee.r());
      ret.b = conj(env, b, ctx, ees);
    }
  }
  return ret;
}

}  // namespace MiniZinc
