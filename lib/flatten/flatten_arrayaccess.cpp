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
  auto* aa = e->cast<ArrayAccess>();
  KeepAlive aa_ka = aa;

  Ctx nctx = ctx;
  nctx.b = +nctx.b;
  nctx.neg = false;
  EE eev = flat_exp(env, nctx, aa->v(), nullptr, nctx.partialityVar(env));
  std::vector<EE> ees;

start_flatten_arrayaccess:
  for (unsigned int i = 0; i < aa->idx().size(); i++) {
    Expression* tmp = follow_id_to_decl(aa->idx()[i]);
    if (auto* vd = tmp->dynamicCast<VarDecl>()) {
      tmp = vd->id();
    }
    if (tmp->type().isPar()) {
      ArrayLit* al;
      if (eev.r()->isa<ArrayLit>()) {
        al = eev.r()->cast<ArrayLit>();
      } else {
        Id* id = eev.r()->cast<Id>();
        if (id->decl() == nullptr) {
          throw InternalError("undefined identifier");
        }
        if (id->decl()->e() == nullptr) {
          throw InternalError("array without initialiser not supported");
        }
        Expression* id_e = follow_id(id);
        if (id_e->isa<ArrayLit>()) {
          al = id_e->cast<ArrayLit>();
        } else {
          throw InternalError("builtin function returning array not supported");
        }
      }

      std::vector<KeepAlive> elems;
      std::vector<IntVal> idx(aa->idx().size());
      std::vector<std::pair<int, int>> dims;
      std::vector<Expression*> newaccess;
      std::vector<int> nonpar;
      std::vector<int> stack;
      for (int j = 0; j < aa->idx().size(); j++) {
        Expression* tmp = follow_id_to_decl(aa->idx()[j]);
        if (auto* vd = tmp->dynamicCast<VarDecl>()) {
          tmp = vd->id();
        }
        if (tmp->type().isPar()) {
          GCLock lock;
          idx[j] = eval_int(env, tmp).toInt();
        } else {
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
        {
          GCLock lock;
          ka = eval_arrayaccess(env, al, idx, success);
          if (!success() && env.inMaybePartial == 0) {
            ResultUndefinedError warning(env, al->loc(), success.errorMessage(env, al));
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
                ResultUndefinedError warning(env, al->loc(), success.errorMessage(env, al));
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
        Expression* newal = new ArrayLit(al->loc(), elems_e, dims);
        Type t = al->type();
        t.dim(static_cast<int>(dims.size()));
        newal->type(t);
        eev.r = newal;
        auto* n_aa = new ArrayAccess(aa->loc(), newal, newaccess);
        n_aa->type(aa->type());
        aa = n_aa;
        aa_ka = aa;
      }
    }
  }

  if (aa->idx().size() == 1 && aa->idx()[0]->isa<ArrayAccess>()) {
    auto* aa_inner = aa->idx()[0]->cast<ArrayAccess>();
    ArrayLit* al;
    if (eev.r()->isa<ArrayLit>()) {
      al = eev.r()->cast<ArrayLit>();
    } else {
      Id* id = eev.r()->cast<Id>();
      if (id->decl() == nullptr) {
        throw InternalError("undefined identifier");
      }
      if (id->decl()->e() == nullptr) {
        throw InternalError("array without initialiser not supported");
      }
      al = follow_id(id)->cast<ArrayLit>();
    }
    if (aa_inner->v()->type().isPar()) {
      KeepAlive ka_al_inner = flat_cv_exp(env, ctx, aa_inner->v());
      auto* al_inner = ka_al_inner()->cast<ArrayLit>();
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
        Expression* newal = new ArrayLit(al->loc(), composed_e, dims);
        Type t = al->type();
        t.dim(static_cast<int>(dims.size()));
        newal->type(t);
        eev.r = newal;
        auto* n_aa = new ArrayAccess(aa->loc(), newal, aa_inner->idx());
        n_aa->type(aa->type());
        aa = n_aa;
        aa_ka = aa;
        goto start_flatten_arrayaccess;
      }
    }
  }
flatten_arrayaccess:
  Ctx dimctx = ctx;
  dimctx.neg = false;
  for (unsigned int i = 0; i < aa->idx().size(); i++) {
    Expression* tmp = follow_id_to_decl(aa->idx()[i]);
    if (auto* vd = tmp->dynamicCast<VarDecl>()) {
      tmp = vd->id();
    }
    ees.push_back(flat_exp(env, dimctx, tmp, nullptr, dimctx.partialityVar(env)));
  }
  ees.emplace_back(nullptr, eev.b());

  bool parAccess = true;
  for (unsigned int i = 0; i < aa->idx().size(); i++) {
    if (!ees[i].r()->type().isPar()) {
      parAccess = false;
      break;
    }
  }

  if (parAccess) {
    ArrayLit* al;
    if (eev.r()->isa<ArrayLit>()) {
      al = eev.r()->cast<ArrayLit>();
    } else {
      Id* id = eev.r()->cast<Id>();
      if (id->decl() == nullptr) {
        throw InternalError("undefined identifier");
      }
      if (id->decl()->e() == nullptr) {
        throw InternalError("array without initialiser not supported");
      }
      al = follow_id(id)->cast<ArrayLit>();
    }
    KeepAlive ka;
    ArrayAccessSucess success;
    {
      GCLock lock;
      std::vector<IntVal> dims(aa->idx().size());
      for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
        dims[i] = eval_int(env, ees[i].r());
      }
      ka = eval_arrayaccess(env, al, dims, success);
    }
    if (!success() && env.inMaybePartial == 0) {
      ResultUndefinedError warning(env, al->loc(), "array access out of bounds");
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
    assert(eev.r()->type().bt() == aa->type().bt());

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
        field_aa[i] = new ArrayAccess(aa->loc().introduce(), field_al[i], idx);
        field_aa[i]()->type((*res_st)[i]);
      }
    }
    // Flatten field based array access expressions
    // WARNING: Expressions stored in field_res, rely on being also in ees to be kept alive
    std::vector<Expression*> field_res(res_st->size());
    for (int i = 0; i < res_st->size(); ++i) {
      // TODO: Does the context need to be changed? Are 'r' and 'b' correct?
      EE ee = flat_exp(env, ctx, field_aa[i](), nullptr, b);
      field_res[i] = ee.r();
      ees.push_back(ee);
    }
    KeepAlive tuple_lit = ArrayLit::constructTuple(Location().introduce(), field_res);
    tuple_lit()->type(aa->type());
    ret.r = bind(env, ctx, r, tuple_lit());
    ret.b = conj(env, b, ctx, ees);
  } else {
    std::vector<Expression*> args(aa->idx().size() + 1);
    for (unsigned int i = aa->idx().size(); (i--) != 0U;) {
      args[i] = ees[i].r();
    }
    args[aa->idx().size()] = eev.r();
    KeepAlive ka;
    {
      GCLock lock;
      Call* cc = Call::a(e->loc().introduce(), env.constants.ids.element, args);
      cc->type(aa->type());
      FunctionI* fi = env.model->matchFn(env, cc->id(), args, false);
      if (fi == nullptr) {
        throw FlatteningError(env, cc->loc(), "cannot find matching declaration");
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
