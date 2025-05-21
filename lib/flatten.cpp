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
#include <minizinc/aststring.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flat_exp.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/gc.hh>
#include <minizinc/hash.hh>
#include <minizinc/iter.hh>
#include <minizinc/optimize.hh>
#include <minizinc/output.hh>
#include <minizinc/statistics.hh>
#include <minizinc/type.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/utils.hh>
#include <minizinc/values.hh>

#include <algorithm>
#include <cassert>
#include <map>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MiniZinc {

// Create domain constraints. Return true if successful.
bool create_explicit_domain_constraints(EnvI& envi, VarDecl* vd, Expression* domain) {
  std::vector<KeepAlive> calls;
  {
    GCLock lock;
    Location iloc = Location().introduce();

    if (vd->type().isOpt()) {
      calls.emplace_back(Call::a(iloc, "var_dom", {vd->id(), domain}));
    } else if (vd->type().isIntSet()) {
      assert(Expression::type(domain).isint() || Expression::type(domain).isIntSet());
      IntSetVal* isv = eval_intset(envi, domain);
      calls.emplace_back(
          Call::a(iloc, envi.constants.ids.set_.subset, {vd->id(), new SetLit(iloc, isv)}));
    } else if (Expression::type(domain).isbool()) {
      calls.emplace_back(Call::a(iloc, envi.constants.ids.bool_.eq, {vd->id(), domain}));
    } else if (Expression::type(domain).isBoolSet()) {
      IntSetVal* bsv = eval_boolset(envi, domain);
      assert(bsv->size() == 1);
      if (bsv->min() != bsv->max()) {
        return true;  // Both values are still possible, no change
      }
      calls.emplace_back(Call::a(iloc, envi.constants.ids.bool_.eq,
                                 {vd->id(), envi.constants.boollit(bsv->min() > 0)}));
    } else if (Expression::type(domain).isfloat() || Expression::type(domain).isFloatSet()) {
      FloatSetVal* fsv = eval_floatset(envi, domain);
      if (fsv->size() == 1) {  // Range based
        if (fsv->min() == fsv->max()) {
          calls.emplace_back(
              Call::a(iloc, envi.constants.ids.float_.eq, {vd->id(), FloatLit::a(fsv->min())}));
        } else {
          FloatSetVal* cfsv;
          if (vd->ti()->domain() != nullptr) {
            cfsv = eval_floatset(envi, vd->ti()->domain());
          } else {
            cfsv = FloatSetVal::a(-FloatVal::infinity(), FloatVal::infinity());
          }
          if (cfsv->min() < fsv->min()) {
            calls.emplace_back(
                Call::a(iloc, envi.constants.ids.float_.le, {FloatLit::a(fsv->min()), vd->id()}));
          }
          if (cfsv->max() > fsv->max()) {
            calls.emplace_back(
                Call::a(iloc, envi.constants.ids.float_.le, {vd->id(), FloatLit::a(fsv->max())}));
          }
        }
      } else {
        calls.emplace_back(
            Call::a(iloc, envi.constants.ids.set_.in, {vd->id(), new SetLit(iloc, fsv)}));
      }
    } else if (Expression::type(domain).isint() || Expression::type(domain).isIntSet()) {
      IntSetVal* isv = eval_intset(envi, domain);
      if (isv->size() == 1) {  // Range based
        if (isv->min() == isv->max()) {
          calls.emplace_back(
              Call::a(iloc, envi.constants.ids.int_.eq, {vd->id(), IntLit::a(isv->min())}));
        } else {
          IntSetVal* cisv;
          if (vd->ti()->domain() != nullptr) {
            cisv = eval_intset(envi, vd->ti()->domain());
          } else {
            cisv = IntSetVal::a(-IntVal::infinity(), IntVal::infinity());
          }
          if (cisv->min() < isv->min()) {
            calls.emplace_back(
                Call::a(iloc, envi.constants.ids.int_.le, {IntLit::a(isv->min()), vd->id()}));
          }
          if (cisv->max() > isv->max()) {
            calls.emplace_back(
                Call::a(iloc, envi.constants.ids.int_.le, {vd->id(), IntLit::a(isv->max())}));
          }
        }
      } else {
        calls.emplace_back(
            Call::a(iloc, envi.constants.ids.set_.in, {vd->id(), new SetLit(iloc, isv)}));
      }
    } else {
      return false;
    }
  }

  int counter = 0;
  for (const auto& ka : calls) {
    auto* c = Expression::cast<Call>(ka());
    CallStackItem csi(envi, IntLit::a(counter++));
    Expression::ann(c).add(envi.constants.ann.domain_change_constraint);
    Expression::type(c, Type::varbool());
    c->decl(envi.model->matchFn(envi, c, true));
    flat_exp(envi, Ctx(), c, envi.constants.varTrue, envi.constants.varTrue);
  }
  return true;
}

void set_computed_domain(EnvI& envi, VarDecl* vd, Expression* domain, bool is_computed) {
  if (envi.hasReverseMapper(vd->id())) {
    if (!create_explicit_domain_constraints(envi, vd, domain)) {
      std::ostringstream ss;
      ss << "Unable to create domain constraint for reverse mapped variable: " << *vd->id() << " = "
         << *domain << std::endl;
      throw EvalError(envi, Expression::loc(domain), ss.str());
    }
    vd->ti()->domain(domain);
    return;
  }
  if (envi.fopts.recordDomainChanges &&
      !Expression::ann(vd).contains(envi.constants.ann.is_defined_var) && !vd->introduced() &&
      !(vd->type().dim() > 0)) {
    if (create_explicit_domain_constraints(envi, vd, domain)) {
      return;
    }
    std::cerr << "Warning: domain change not handled by -g mode: " << *vd->id() << " = " << *domain
              << std::endl;
  }
  vd->ti()->domain(domain);
  vd->ti()->setComputedDomain(is_computed);
}

// Create assignment constraints. Return true if successful.
bool create_explicit_assignment_constraints(EnvI& envi, VarDecl* vd, Expression* val) {
  assert(vd->type().dim() == 0);
  KeepAlive ka;
  Location iloc = Location().introduce();

  {
    GCLock lock;
    Call* call;
    if (vd->type().isIntSet()) {
      assert(Expression::type(val).isIntSet());
      call = Call::a(iloc, envi.constants.ids.set_.eq, {vd->id(), val});
    } else {
      assert(vd->type().st() == Type::ST_PLAIN);
      if (vd->type().bt() == Type::BT_BOOL) {
        call = Call::a(iloc, envi.constants.ids.bool_.eq, {vd->id(), val});
      } else if (vd->type().bt() == Type::BT_INT) {
        call = Call::a(iloc, envi.constants.ids.int_.eq, {vd->id(), val});
      } else if (vd->type().bt() == Type::BT_FLOAT) {
        call = Call::a(iloc, envi.constants.ids.float_.eq, {vd->id(), val});
      } else {
        return false;
      }
    }
    CallStackItem csi(envi, IntLit::a(0));
    Expression::ann(call).add(envi.constants.ann.domain_change_constraint);
    Expression::type(call, Type::varbool());
    call->decl(envi.model->matchFn(envi, call, true));
    ka = call;
  }

  flat_exp(envi, Ctx(), ka(), envi.constants.varTrue, envi.constants.varTrue);
  return true;
}

/// Output operator for contexts
template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, Ctx& ctx) {
  switch (ctx.b) {
    case C_ROOT:
      os << "R";
      break;
    case C_POS:
      os << "+";
      break;
    case C_NEG:
      os << "-";
      break;
    case C_MIX:
      os << "M";
      break;
    default:
      assert(false);
      break;
  }
  switch (ctx.i) {
    case C_ROOT:
      os << "R";
      break;
    case C_POS:
      os << "+";
      break;
    case C_NEG:
      os << "-";
      break;
    case C_MIX:
      os << "M";
      break;
    default:
      assert(false);
      break;
  }
  if (ctx.neg) {
    os << "!";
  }
  return os;
}

BCtx operator+(const BCtx& c) {
  switch (c) {
    case C_ROOT:
    case C_POS:
      return C_POS;
    case C_NEG:
      return C_NEG;
    case C_MIX:
      return C_MIX;
    default:
      assert(false);
      return C_ROOT;
  }
}

BCtx operator-(const BCtx& c) {
  switch (c) {
    case C_ROOT:
    case C_POS:
      return C_NEG;
    case C_NEG:
      return C_POS;
    case C_MIX:
      return C_MIX;
    default:
      assert(false);
      return C_ROOT;
  }
}

/// Check if \a c is non-positive
bool nonpos(const BCtx& c) { return c == C_NEG || c == C_MIX; }
/// Check if \a c is non-negative
bool nonneg(const BCtx& c) { return c == C_ROOT || c == C_POS; }

void dump_ee_b(const std::vector<EE>& ee) {
  for (const auto& i : ee) {
    std::cerr << *i.b() << "\n";
  }
}
void dump_ee_r(const std::vector<EE>& ee) {
  for (const auto& i : ee) {
    std::cerr << *i.r() << "\n";
  }
}

std::tuple<BCtx, bool> EnvI::annToCtx(VarDecl* vd) const {
  if (Expression::ann(vd).contains(constants.ctx.root)) {
    return std::make_tuple(C_ROOT, true);
  }
  if (Expression::ann(vd).contains(constants.ctx.mix)) {
    return std::make_tuple(C_MIX, true);
  }
  if (Expression::ann(vd).contains(constants.ctx.pos)) {
    return std::make_tuple(C_POS, true);
  }
  if (Expression::ann(vd).contains(constants.ctx.neg)) {
    return std::make_tuple(C_NEG, true);
  }
  return std::make_tuple(C_MIX, false);
}

Id* EnvI::ctxToAnn(BCtx c) const {
  switch (c) {
    case C_ROOT:
      return constants.ctx.root;
    case C_POS:
      return constants.ctx.pos;
    case C_NEG:
      return constants.ctx.neg;
    case C_MIX:
      return constants.ctx.mix;
    default:
      assert(false);
      return nullptr;
  }
}

void EnvI::addCtxAnn(VarDecl* vd, const BCtx& c) const {
  if (vd != nullptr) {
    BCtx nc;
    bool annotated;
    std::tie(nc, annotated) = annToCtx(vd);
    // If previously annotated
    if (annotated) {
      // Early exit
      if (nc == c || nc == C_ROOT || (nc == C_MIX && c != C_ROOT)) {
        return;
      }
      // Remove old annotation
      Id* old_ann = ctxToAnn(nc);
      Expression::ann(vd).remove(old_ann);
      // Determine new context
      if (c == C_ROOT) {
        nc = C_ROOT;
      } else {
        nc = C_MIX;
      }
    } else {
      nc = c;
    }

    Id* ctx_id = ctxToAnn(nc);
    Expression::addAnnotation(vd, ctx_id);
  }
}

void make_defined_var(EnvI& env, VarDecl* vd, Call* c) {
  if (!Expression::ann(vd).contains(env.constants.ann.is_defined_var)) {
    std::vector<Expression*> args(1);
    args[0] = vd->id();
    Call* dv = Call::a(Location().introduce(), env.constants.ann.defines_var, args);
    Expression::type(dv, Type::ann());
    Expression::addAnnotation(vd, env.constants.ann.is_defined_var);
    Expression::addAnnotation(c, dv);
  }
}

bool is_defines_var_ann(EnvI& env, Expression* e) {
  return Expression::isa<Call>(e) &&
         Expression::cast<Call>(e)->id() == env.constants.ann.defines_var;
}

/// Check if \a e is NULL or true
bool istrue(EnvI& env, Expression* e) {
  if (e == nullptr) {
    return true;
  }
  if (Expression::type(e) == Type::parbool()) {
    if (Expression::type(e).cv()) {
      Ctx ctx;
      ctx.b = C_MIX;
      KeepAlive r = flat_cv_exp(env, ctx, e);
      return eval_bool(env, r());
    }
    GCLock lock;
    return eval_bool(env, e);
  }
  return false;
}
/// Check if \a e is non-NULL and false
bool isfalse(EnvI& env, Expression* e) {
  if (e == nullptr) {
    return false;
  }
  if (Expression::type(e) == Type::parbool()) {
    if (Expression::type(e).cv()) {
      Ctx ctx;
      ctx.b = C_MIX;
      KeepAlive r = flat_cv_exp(env, ctx, e);
      return !eval_bool(env, r());
    }
    GCLock lock;
    return !eval_bool(env, e);
  }
  return false;
}

/// Use bounds from ovd for vd if they are better.
/// Returns true if ovd's bounds are better.
bool update_bounds(EnvI& envi, VarDecl* ovd, VarDecl* vd) {
  bool tighter = false;
  bool fixed = false;
  if ((ovd->ti()->domain() != nullptr) || (ovd->e() != nullptr)) {
    IntVal intval;
    FloatVal doubleval;
    bool boolval;

    if (vd->type().isint()) {
      IntBounds oldbounds = compute_int_bounds(envi, ovd->id());
      IntBounds bounds(0, 0, false);
      if ((vd->ti()->domain() != nullptr) || (vd->e() != nullptr)) {
        bounds = compute_int_bounds(envi, vd->id());
      }

      if (((vd->ti()->domain() != nullptr) || (vd->e() != nullptr)) && bounds.valid &&
          bounds.l.isFinite() && bounds.u.isFinite()) {
        if (oldbounds.valid && oldbounds.l.isFinite() && oldbounds.u.isFinite()) {
          fixed = oldbounds.u == oldbounds.l || bounds.u == bounds.l;
          if (fixed) {
            tighter = true;
            intval = oldbounds.u == oldbounds.l ? oldbounds.u : bounds.l;
            ovd->ti()->domain(new SetLit(Expression::loc(ovd), IntSetVal::a(intval, intval)));
          } else {
            IntSetVal* olddom =
                ovd->ti()->domain() != nullptr ? eval_intset(envi, ovd->ti()->domain()) : nullptr;
            IntSetVal* newdom =
                vd->ti()->domain() != nullptr ? eval_intset(envi, vd->ti()->domain()) : nullptr;

            if (olddom != nullptr) {
              if (newdom == nullptr) {
                tighter = true;
              } else {
                IntSetRanges oisr(olddom);
                IntSetRanges nisr(newdom);
                IntSetRanges nisr_card(newdom);

                Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(oisr, nisr);

                if (Ranges::cardinality(inter) < Ranges::cardinality(nisr_card)) {
                  IntSetRanges oisr_inter(olddom);
                  IntSetRanges nisr_inter(newdom);
                  Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter_card(oisr_inter,
                                                                               nisr_inter);
                  tighter = true;
                  ovd->ti()->domain(new SetLit(Expression::loc(ovd), IntSetVal::ai(inter_card)));
                }
              }
            }
          }
        }
      } else {
        if (oldbounds.valid && oldbounds.l.isFinite() && oldbounds.u.isFinite()) {
          tighter = true;
          fixed = oldbounds.u == oldbounds.l;
          if (fixed) {
            intval = oldbounds.u;
            ovd->ti()->domain(new SetLit(Expression::loc(ovd), IntSetVal::a(intval, intval)));
          }
        }
      }
    } else if (vd->type().isfloat()) {
      FloatBounds oldbounds = compute_float_bounds(envi, ovd->id());
      FloatBounds bounds(0.0, 0.0, false);
      if ((vd->ti()->domain() != nullptr) || (vd->e() != nullptr)) {
        bounds = compute_float_bounds(envi, vd->id());
      }
      if (((vd->ti()->domain() != nullptr) || (vd->e() != nullptr)) && bounds.valid) {
        if (oldbounds.valid) {
          fixed = oldbounds.u == oldbounds.l || bounds.u == bounds.l;
          if (fixed) {
            doubleval = oldbounds.u == oldbounds.l ? oldbounds.u : bounds.l;
          }
          tighter = fixed || (oldbounds.u - oldbounds.l < bounds.u - bounds.l);
        }
      } else {
        if (oldbounds.valid) {
          tighter = true;
          fixed = oldbounds.u == oldbounds.l;
          if (fixed) {
            doubleval = oldbounds.u;
          }
        }
      }
    } else if (vd->type().isbool()) {
      if (ovd->ti()->domain() != nullptr) {
        fixed = tighter = true;
        boolval = eval_bool(envi, ovd->ti()->domain());
      } else {
        fixed = tighter = ((ovd->e() != nullptr) && Expression::isa<BoolLit>(ovd->e()));
        if (fixed) {
          boolval = Expression::cast<BoolLit>(ovd->e())->v();
        }
      }
    }

    if (tighter) {
      vd->ti()->domain(ovd->ti()->domain());
      if (vd->e() == nullptr && fixed) {
        if (vd->ti()->type().isvarint()) {
          Expression::type(vd, Type::parint());
          vd->ti(new TypeInst(Expression::loc(vd), Type::parint()));
          vd->e(IntLit::a(intval));
        } else if (vd->ti()->type().isvarfloat()) {
          Expression::type(vd, Type::parfloat());
          vd->ti(new TypeInst(Expression::loc(vd), Type::parfloat()));
          vd->e(FloatLit::a(doubleval));
        } else if (vd->ti()->type().isvarbool()) {
          Expression::type(vd, Type::parbool());
          vd->ti(new TypeInst(Expression::loc(vd), Type::parbool()));
          vd->ti()->domain(envi.constants.boollit(boolval));
          vd->e(new BoolLit(Expression::loc(vd), boolval));
        }
      }
    }
  }

  return tighter;
}

std::string get_path(EnvI& env) {
  std::string path;
  std::stringstream ss;
  if (env.dumpPath(ss)) {
    path = ss.str();
  }

  return path;
}

inline Location get_loc(EnvI& env, Expression* e1, Expression* e2) {
  if (e1 != nullptr) {
    return Expression::loc(e1).introduce();
  }
  if (e2 != nullptr) {
    return Expression::loc(e2).introduce();
  }
  return Location().introduce();
}
inline Id* get_id(EnvI& env, Id* origId) {
  return origId != nullptr ? origId : new Id(Location().introduce(), env.genId(), nullptr);
}

StringLit* get_longest_mzn_path_annotation(EnvI& env, const Expression* e) {
  StringLit* sl = nullptr;

  if (const auto* vd = Expression::dynamicCast<const VarDecl>(e)) {
    VarPathStore::ReversePathMap& reversePathMap = env.varPathStore.getReversePathMap();
    auto it = reversePathMap.find(vd->id()->decl());
    if (it != reversePathMap.end()) {
      sl = new StringLit(Location(), it->second);
    }
  } else {
    for (ExpressionSetIter it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
      if (Call* ca = Expression::dynamicCast<Call>(*it)) {
        if (ca->id() == env.constants.ann.mzn_path) {
          auto* sl1 = Expression::cast<StringLit>(ca->arg(0));
          if (sl != nullptr) {
            if (sl1->v().size() > sl->v().size()) {
              sl = sl1;
            }
          } else {
            sl = sl1;
          }
        }
      }
    }
  }
  return sl;
}

void add_path_annotation(EnvI& env, Expression* e) {
  if (!(Expression::type(e).isAnn() || Expression::isa<Id>(e)) && Expression::type(e).dim() == 0) {
    GCLock lock;
    Annotation& ann = Expression::ann(e);
    if (ann.containsCall(env.constants.ann.mzn_path)) {
      return;
    }

    VarPathStore::ReversePathMap& reversePathMap = env.varPathStore.getReversePathMap();

    std::vector<Expression*> path_args(1);
    std::string p;
    auto it = reversePathMap.find(e);
    if (it == reversePathMap.end()) {
      p = get_path(env);
    } else {
      p = it->second;
    }

    if (!p.empty()) {
      path_args[0] = new StringLit(Location(), p);
      Call* path_call = Call::a(Expression::loc(e), env.constants.ann.mzn_path, path_args);
      Expression::type(path_call, Type::ann());
      Expression::addAnnotation(e, path_call);
    }
  }
}

void flatten_vardecl_annotations(EnvI& env, VarDecl* origVd, VarDeclI* vdi, VarDecl* toAnnotate) {
  for (ExpressionSetIter it = Expression::ann(origVd).begin(); it != Expression::ann(origVd).end();
       ++it) {
    // Check if we need to add the annotated expression as an argument
    Call* addAnnotatedExpression = nullptr;
    if (Id* ident = Expression::dynamicCast<Id>(*it)) {
      if (ident->decl() != nullptr) {
        addAnnotatedExpression =
            Expression::ann(ident->decl()).getCall(env.constants.ann.mzn_add_annotated_expression);
      }
    } else if (Call* call = Expression::dynamicCast<Call>(*it)) {
      if (call->decl() != nullptr) {
        addAnnotatedExpression =
            call->decl()->ann().getCall(env.constants.ann.mzn_add_annotated_expression);
      }
    }
    KeepAlive ann;
    if (addAnnotatedExpression != nullptr) {
      GCLock lock;
      Call* c;
      if (Expression::isa<Id>(*it)) {
        c = Call::a(Location().introduce(), Expression::cast<Id>(*it)->v(), {toAnnotate->id()});
      } else {
        unsigned int annotatedExpressionIdx =
            static_cast<unsigned int>(eval_int(env, addAnnotatedExpression->arg(0)).toInt());
        Call* orig_call = Expression::cast<Call>(*it);
        std::vector<Expression*> args(orig_call->argCount() + 1);
        for (unsigned int i = 0, j = 0; i < orig_call->argCount(); i++) {
          if (j == annotatedExpressionIdx) {
            args[j++] = toAnnotate->id();
          }
          args[j++] = orig_call->arg(i);
        }
        if (annotatedExpressionIdx == orig_call->argCount()) {
          args[orig_call->argCount()] = toAnnotate->id();
        }
        c = Call::a(Location().introduce(), Expression::cast<Call>(*it)->id(), args);
      }
      c->decl(env.model->matchFn(env, c, false));
      Expression::type(c, Type::ann());
      ann = c;
    } else {
      ann = *it;
    }
    EE ee_ann = flat_exp(env, Ctx(), ann(), nullptr, env.constants.varTrue);
    if (vdi != nullptr) {
      Expression::addAnnotation(toAnnotate, ee_ann.r());
      CollectOccurrencesE ce(env, env.varOccurrences, vdi);
      top_down(ce, ee_ann.r());
    }
  }
}

VarDecl* new_vardecl(EnvI& env, const Ctx& ctx, TypeInst* ti, Id* origId, VarDecl* origVd,
                     Expression* rhs) {
  VarDecl* vd = nullptr;

  // Is this vardecl already in the FlatZinc (for unification)
  bool hasBeenAdded = false;

  // Don't use paths for arrays or annotations
  if (ti->type().dim() == 0 && !ti->type().isAnn()) {
    std::string path = get_path(env);
    if (!path.empty()) {
      VarPathStore::ReversePathMap& reversePathMap = env.varPathStore.getReversePathMap();
      VarPathStore::PathMap& pathMap = env.varPathStore.getPathMap();
      auto it = pathMap.find(path);

      if (it != pathMap.end()) {
        auto* ovd = Expression::cast<VarDecl>((it->second.decl)());
        unsigned int ovd_pass = it->second.passNumber;

        if (ovd != nullptr) {
          // If ovd was introduced during the same pass, we can unify
          if (env.multiPassInfo.currentPassNumber == ovd_pass) {
            vd = ovd;
            if (origId != nullptr) {
              origId->decl(vd);
            }
            hasBeenAdded = true;
          } else {
            vd = new VarDecl(get_loc(env, origVd, rhs), ti, get_id(env, origId));
            hasBeenAdded = false;
            update_bounds(env, ovd, vd);
          }

          // Check whether ovd was unified in a previous pass
          if (ovd->id() != ovd->id()->decl()->id()) {
            // We may not have seen the pointed to decl just yet
            auto path2It = reversePathMap.find(ovd->id()->decl());
            if (path2It != reversePathMap.end()) {
              std::string path2 = path2It->second;
              VarPathStore::PathVar vd_tup{vd, env.multiPassInfo.currentPassNumber};

              pathMap[path] = vd_tup;
              pathMap[path2] = vd_tup;
              reversePathMap.insert(vd, path);
            }
          }
        }
      } else {
        // Create new VarDecl and add it to the maps
        vd = new VarDecl(get_loc(env, origVd, rhs), ti, get_id(env, origId));
        hasBeenAdded = false;
        VarPathStore::PathVar vd_tup{vd, env.multiPassInfo.currentPassNumber};
        pathMap[path] = vd_tup;
        reversePathMap.insert(vd, path);
      }
    }
  }
  if (vd == nullptr) {
    vd = new VarDecl(get_loc(env, origVd, rhs), ti, get_id(env, origId));
    hasBeenAdded = false;
  }

  // If vd has an e() use bind to turn rhs into a constraint
  if (vd->e() != nullptr) {
    if (rhs != nullptr) {
      bind(env, ctx, vd, rhs);
    }
  } else {
    vd->e(rhs);
    if ((rhs != nullptr) && hasBeenAdded) {
      // This variable is being reused, so it won't be added to the model below.
      // Therefore, we need to register that we changed the RHS, in order
      // for the reference counts to be accurate.
      env.voAddExp(vd);
    }
  }
  if ((origVd != nullptr) && (origVd->id()->idn() != -1 || origVd->toplevel())) {
    vd->introduced(origVd->introduced());
  } else {
    vd->introduced(true);
  }

  vd->flat(vd);

  VarDeclI* vdi;
  if (hasBeenAdded) {
    vdi = (*env.flat())[env.varOccurrences.find(vd)]->cast<VarDeclI>();
  } else {
    vdi = VarDeclI::a(Location().introduce(), vd);
    env.flatAddItem(vdi);
  }

  // Copy annotations from origVd
  if (origVd != nullptr) {
    flatten_vardecl_annotations(env, origVd, vdi, vd);
  }

  return vd;
}

#define MZN_FILL_REIFY_MAP(T, ID) \
  _reifyMap.insert(std::pair<ASTString, ASTString>(constants.ids.T.ID, constants.ids.T##reif.ID));

MultiPassInfo::MultiPassInfo() : currentPassNumber(0), finalPassNumber(1) {}

VarPathStore::VarPathStore() : maxPathDepth(0) {}

void OutputSectionStore::add(EnvI& env, ASTString section, Expression* e, bool json) {
  if (json) {
    auto ret = _idx.emplace(section, _sections.size());
    if (ret.second) {
      _sections.emplace_back(section, e, true);
    } else {
      std::stringstream ss;
      ss << "JSON output section '" << section << "' already used. Ignoring.";
      env.addWarning(Expression::loc(e), ss.str());
    }
    return;
  }

  _blank = false;
  auto ret = _idx.emplace(section, _sections.size());
  if (ret.second) {
    _sections.emplace_back(section, e, false);
  } else {
    GCLock lock;
    auto idx = ret.first->second;
    auto* orig = _sections[idx].e;
    auto* bo = new BinOp(Location().introduce(), orig, BOT_PLUSPLUS, e);
    Expression::type(bo, Type::parstring(1));
    _sections[idx].e = bo;
  }
}

TupleType::TupleType(const std::vector<Type>& fields) {
  _size = static_cast<unsigned int>(fields.size());
  for (size_t i = 0; i < _size; ++i) {
    _fields[i] = fields[i];
  }
}
TupleType* TupleType::a(const std::vector<Type>& fields) {
  TupleType* tt = static_cast<TupleType*>(::malloc(
      sizeof(TupleType) + sizeof(Type) * (std::max(0, static_cast<int>(fields.size()) - 1))));
  tt = new (tt) TupleType(fields);
  return tt;
}
bool TupleType::matchesBT(const EnvI& env, const TupleType& other) const {
  if (other.size() != size()) {
    return false;
  }
  for (unsigned int i = 0; i < other.size(); ++i) {
    const Type& ty = operator[](i);
    if (ty.bt() != Type::BT_BOT || other[i].bt() != Type::BT_BOT) {
      continue;
    }
    if (other[i].bt() != Type::BT_TOP && ty.bt() != other[i].bt()) {
      return false;
    }
    if (ty.bt() == Type::BT_TUPLE &&
        !env.getTupleType(ty)->matchesBT(env, *env.getTupleType(other[i]))) {
      return false;
    }
    if (ty.bt() == Type::BT_RECORD &&
        !env.getRecordType(ty)->matchesBT(env, *env.getRecordType(other[i]))) {
      return false;
    }
  }
  return true;
}
bool StructType::containsArray(const EnvI& env) const {
  for (unsigned int i = 0; i < size(); ++i) {
    const Type& ti = operator[](i);
    if (ti.dim() != 0) {
      return true;
    }
    if (ti.structBT() && env.getStructType(ti)->containsArray(env)) {
      return true;
    }
  }
  return false;
}

RecordType::RecordType(const std::vector<std::pair<ASTString, Type>>& fields) {
  _size = static_cast<unsigned int>(fields.size());
  size_t str_size = 0;
  for (size_t i = 0; i < _size; ++i) {
    _fields[i] = {str_size, fields[i].second};
    str_size += fields[i].first.size();
  }
  _fieldNames.reserve(str_size);
  for (size_t i = 0; i < _size; ++i) {
    _fieldNames.append(std::string(fields[i].first.c_str()));
  }
}
RecordType::RecordType(const RecordType& orig) {
  _size = orig._size;
  _fieldNames = orig._fieldNames;
  for (size_t i = 0; i < _size; ++i) {
    _fields[i] = orig._fields[i];
  }
}
RecordType* RecordType::a(const std::vector<std::pair<ASTString, Type>>& fields) {
  RecordType* tt = static_cast<RecordType*>(::malloc(
      sizeof(RecordType) + sizeof(FieldTup) * (std::max(0, static_cast<int>(fields.size()) - 1))));
  tt = new (tt) RecordType(fields);
  return tt;
}
RecordType* RecordType::a(const RecordType* orig, const std::vector<Type>& types) {
  RecordType* tt = static_cast<RecordType*>(::malloc(
      sizeof(RecordType) + sizeof(FieldTup) * (std::max(0, static_cast<int>(orig->size()) - 1))));
  tt = new (tt) RecordType(*orig);
  assert(orig->size() == types.size());
  for (size_t i = 0; i < types.size(); i++) {
    tt->_fields[i].second = types[i];
  }
  return tt;
}
bool RecordType::matchesBT(const EnvI& env, const RecordType& other) const {
  if (other.size() != size()) {
    return false;
  }
  for (unsigned int i = 0; i < other.size(); ++i) {
    if (fieldName(i) != other.fieldName(i)) {
      return false;
    }
    const Type& ty = operator[](i);
    if (ty.bt() != Type::BT_BOT || other[i].bt() != Type::BT_BOT) {
      continue;
    }
    if (other[i].bt() != Type::BT_TOP && ty.bt() != other[i].bt()) {
      return false;
    }
    if (ty.bt() == Type::BT_TUPLE &&
        !env.getTupleType(ty)->matchesBT(env, *env.getTupleType(other[i]))) {
      return false;
    }
    if (ty.bt() == Type::BT_RECORD &&
        !env.getRecordType(ty)->matchesBT(env, *env.getRecordType(other[i]))) {
      return false;
    }
  }
  return true;
}

EnvI::EnvI(Model* model0, std::ostream& outstream0, std::ostream& errstream0)
    : model(model0),
      originalModel(nullptr),
      output(new Model),
      constants(Constants::constants()),
      outstream(outstream0),
      errstream(errstream0),
      ignorePartial(false),
      ignoreUnknownIds(false),
      maxCallStack(0),
      inRedundantConstraint(0),
      inSymmetryBreakingConstraint(0),
      inMaybePartial(0),
      inTraceExp(false),
      inReverseMapVar(false),
      warnImplicitEnum2Int(true),
      counters({0, 0, 0, 0}),
      _flat(new Model),
      _failed(false),
      _ids(0),
      _collectVardecls(false) {
  MZN_FILL_REIFY_MAP(int_, lin_eq);
  MZN_FILL_REIFY_MAP(int_, lin_le);
  MZN_FILL_REIFY_MAP(int_, lin_ne);
  MZN_FILL_REIFY_MAP(int_, plus);
  MZN_FILL_REIFY_MAP(int_, minus);
  MZN_FILL_REIFY_MAP(int_, times);
  MZN_FILL_REIFY_MAP(int_, div);
  MZN_FILL_REIFY_MAP(int_, mod);
  MZN_FILL_REIFY_MAP(int_, lt);
  MZN_FILL_REIFY_MAP(int_, le);
  MZN_FILL_REIFY_MAP(int_, gt);
  MZN_FILL_REIFY_MAP(int_, ge);
  MZN_FILL_REIFY_MAP(int_, eq);
  MZN_FILL_REIFY_MAP(int_, ne);
  MZN_FILL_REIFY_MAP(float_, lin_eq);
  MZN_FILL_REIFY_MAP(float_, lin_le);
  MZN_FILL_REIFY_MAP(float_, lin_lt);
  MZN_FILL_REIFY_MAP(float_, lin_ne);
  MZN_FILL_REIFY_MAP(float_, plus);
  MZN_FILL_REIFY_MAP(float_, minus);
  MZN_FILL_REIFY_MAP(float_, times);
  MZN_FILL_REIFY_MAP(float_, div);
  MZN_FILL_REIFY_MAP(float_, mod);
  MZN_FILL_REIFY_MAP(float_, lt);
  MZN_FILL_REIFY_MAP(float_, le);
  MZN_FILL_REIFY_MAP(float_, gt);
  MZN_FILL_REIFY_MAP(float_, ge);
  MZN_FILL_REIFY_MAP(float_, eq);
  MZN_FILL_REIFY_MAP(float_, ne);
  _reifyMap.insert(std::pair<ASTString, ASTString>(constants.ids.forall, constants.ids.forallReif));
  _reifyMap.insert(
      std::pair<ASTString, ASTString>(constants.ids.bool_.eq, constants.ids.bool_reif.eq));
  _reifyMap.insert(
      std::pair<ASTString, ASTString>(constants.ids.bool_.clause, constants.ids.bool_reif.clause));
  _reifyMap.insert(
      std::pair<ASTString, ASTString>(constants.ids.clause, constants.ids.bool_reif.clause));
  _reifyMap.insert({constants.ids.bool_.not_, constants.ids.bool_.not_});
}
EnvI::~EnvI() {
  for (auto* _tupleType : _tupleTypes) {
    TupleType::free(_tupleType);
  }
  delete _flat;
  delete output;
  delete model;
  delete originalModel;
}
long long int EnvI::genId() { return _ids++; }
void EnvI::cseMapInsert(Expression* e, const EE& ee) {
  GCLock lock;
  if (Expression::type(e).isPar() && !Expression::isa<ArrayLit>(e)) {
    return;
  }
  Call* c = Expression::dynamicCast<Call>(e);
  if ((c != nullptr) && c->decl() != nullptr) {
    if (c->decl()->ann().contains(constants.ann.no_cse)) {
      return;
    }
    if (c->decl()->ann().contains(constants.ann.promise_commutative)) {
      e = Call::commutativeNormalized(*this, c);
    }
  }

  _cseMap.insert(e, WW(ee.r(), ee.b()));
  if ((c != nullptr) && c->id() == constants.ids.bool_.not_ && Expression::isa<Id>(c->arg(0)) &&
      Expression::isa<Id>(ee.r()) && ee.b() == constants.literalTrue) {
    Call* neg_c = Call::a(Location().introduce(), c->id(), {ee.r()});
    Expression::type(neg_c, Expression::type(e));
    neg_c->decl(c->decl());
    _cseMap.insert(neg_c, WW(c->arg(0), ee.b()));
  }
}
EnvI::CSEMap::iterator EnvI::cseMapFind(Expression* e) {
  GCLock lock;
  Call* c = Expression::dynamicCast<Call>(e);
  if ((c != nullptr) && c->decl() != nullptr) {
    if (c->decl()->ann().contains(constants.ann.promise_commutative)) {
      e = Call::commutativeNormalized(*this, c);
    }
  }

  auto it = _cseMap.find(e);
  if (it != _cseMap.end()) {
    if (it->second.r != nullptr) {
      VarDecl* it_vd = Expression::isa<Id>(it->second.r)
                           ? Expression::cast<Id>(it->second.r)->decl()
                           : Expression::dynamicCast<VarDecl>(it->second.r);
      if (it_vd != nullptr) {
        int idx = varOccurrences.find(it_vd);
        if (idx == -1 || (*_flat)[idx]->removed()) {
          _cseMap.remove(e);
          return _cseMap.end();
        }
      }
    } else {
      _cseMap.remove(e);
      return _cseMap.end();
    }
  }
  return it;
}
void EnvI::cseMapRemove(Expression* e) { _cseMap.remove(e); }
EnvI::CSEMap::iterator EnvI::cseMapEnd() { return _cseMap.end(); }
void EnvI::dump() {
  struct EED {
    static std::string k(Expression* e) {
      std::ostringstream oss;
      oss << *e;
      return oss.str();
    }
    static std::string d(const WW& ee) {
      std::ostringstream oss;
      oss << *ee.r << " " << ee.b;
      return oss.str();
    }
  };
  _cseMap.dump<EED>();
}

void EnvI::flatAddItem(Item* i) {
  assert(_flat);
  if (_failed) {
    return;
  }
  _flat->addItem(i);

  Expression* toAnnotate = nullptr;
  Expression* toAdd = nullptr;
  switch (i->iid()) {
    case Item::II_VD: {
      auto* vd = i->cast<VarDeclI>();
      add_path_annotation(*this, vd->e());
      toAnnotate = vd->e()->e();
      varOccurrences.addIndex(vd, _flat->size() - 1);
      toAdd = vd->e();
      break;
    }
    case Item::II_CON: {
      auto* ci = i->cast<ConstraintI>();

      if (Expression::isa<BoolLit>(ci->e()) && !Expression::cast<BoolLit>(ci->e())->v()) {
        fail();
      } else {
        toAnnotate = ci->e();
        add_path_annotation(*this, ci->e());
        toAdd = ci->e();
      }
      break;
    }
    case Item::II_SOL: {
      auto* si = i->cast<SolveI>();
      CollectOccurrencesE ce(*this, varOccurrences, si);
      top_down(ce, si->e());
      for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
        top_down(ce, *it);
      }
      break;
    }
    case Item::II_OUT: {
      auto* si = i->cast<OutputI>();
      toAdd = si->e();
      break;
    }
    default:
      break;
  }
  if ((toAnnotate != nullptr) && Expression::isa<Call>(toAnnotate)) {
    annotateFromCallStack(toAnnotate);
  }
  if (toAdd != nullptr) {
    CollectOccurrencesE ce(*this, varOccurrences, i);
    top_down(ce, toAdd);
  }
}

void EnvI::annotateFromCallStack(Expression* e) {
  int prev = !idStack.empty() ? idStack.back() : 0;
  bool allCalls = true;
  for (int i = static_cast<int>(callStack.size()) - 1; i >= prev; i--) {
    Expression* ee = callStack[i].e;
    if (Expression::type(ee).isAnn()) {
      // If we are inside an annotation call, don't annotate it again with
      // anything from outside the call
      break;
    }
    if (Expression::isa<Call>(ee) && Expression::cast<Call>(ee)->decl() != nullptr &&
        Expression::cast<Call>(ee)->decl()->capturedAnnotationsVar() != nullptr) {
      // capturing annotations means they are not propagated
      break;
    }
    allCalls = allCalls && (i == callStack.size() - 1 || Expression::isa<Call>(ee) ||
                            Expression::isa<BinOp>(ee));
    for (ExpressionSetIter it = Expression::ann(ee).begin(); it != Expression::ann(ee).end();
         ++it) {
      EE ee_ann = flat_exp(*this, Ctx(), *it, nullptr, constants.varTrue);
      if (allCalls || !is_defines_var_ann(*this, ee_ann.r())) {
        Expression::addAnnotation(e, ee_ann.r());
      }
    }
  }
}

ArrayLit* EnvI::createAnnotationArray(const BCtx& ctx) {
  std::vector<Expression*> annotations;
  int prev = !idStack.empty() ? idStack.back() : 0;
  bool allCalls = true;
  for (int i = static_cast<int>(callStack.size()) - 1; i >= prev; i--) {
    Expression* ee = callStack[i].e;
    if (Expression::type(ee).isAnn()) {
      // If we are inside an annotation call, don't annotate it again with
      // anything from outside the call
      break;
    }
    if (i != static_cast<int>(callStack.size()) - 1 && Expression::isa<Call>(ee) &&
        Expression::cast<Call>(ee)->decl() != nullptr &&
        Expression::cast<Call>(ee)->decl()->capturedAnnotationsVar() != nullptr &&
        !callStack[i].replaced) {
      // capturing annotations means they are not propagated
      break;
    }
    allCalls = allCalls && (i == callStack.size() - 1 || Expression::isa<Call>(ee) ||
                            Expression::isa<BinOp>(ee));
    for (ExpressionSetIter it = Expression::ann(ee).begin(); it != Expression::ann(ee).end();
         ++it) {
      EE ee_ann = flat_exp(*this, Ctx(), *it, nullptr, constants.varTrue);
      if (allCalls || !is_defines_var_ann(*this, ee_ann.r())) {
        annotations.push_back(ee_ann.r());
      }
    }
  }
  if (ctx != C_MIX) {
    Expression* ctx_ann;
    if (ctx == C_ROOT) {
      ctx_ann = constants.ctx.root;
    } else if (ctx == C_POS) {
      ctx_ann = constants.ctx.pos;
    } else {
      ctx_ann = constants.ctx.neg;
    }
    annotations.push_back(ctx_ann);
  }
  auto* al = new ArrayLit(Location(), annotations);
  Expression::type(al, Type::ann(1));
  return al;
}

void EnvI::copyPathMapsAndState(EnvI& env) {
  multiPassInfo.finalPassNumber = env.multiPassInfo.finalPassNumber;
  multiPassInfo.currentPassNumber = env.multiPassInfo.currentPassNumber;

  varPathStore.pathMap = env.varPathStore.getPathMap();
  varPathStore.reversePathMap = env.varPathStore.getReversePathMap();

  varPathStore.filenameSet = env.varPathStore.filenameSet;
  varPathStore.maxPathDepth = env.varPathStore.maxPathDepth;
}

void EnvI::flatRemoveExpr(Expression* e, Item* i) {
  std::vector<VarDecl*> toRemove;
  CollectDecls cd(*this, varOccurrences, toRemove, i);
  top_down(cd, e);

  Model& flat = (*_flat);
  while (!toRemove.empty()) {
    VarDecl* cur = toRemove.back();
    toRemove.pop_back();
    assert(varOccurrences.occurrences(cur) == 0 && CollectDecls::varIsFree(cur));

    auto cur_idx = varOccurrences.idx.find(cur->id());
    if (cur_idx.first) {
      auto* vdi = flat[*cur_idx.second]->cast<VarDeclI>();

      if (!is_output(vdi->e()) && !vdi->removed()) {
        CollectDecls cd(*this, varOccurrences, toRemove, vdi);
        top_down(cd, vdi->e()->e());
        vdi->remove();
      }
    }
  }
}

void EnvI::flatRemoveItem(ConstraintI* ci) {
  flatRemoveExpr(ci->e(), ci);
  ci->e(constants.literalTrue);
  ci->remove();
}
void EnvI::flatRemoveItem(VarDeclI* vdi) {
  flatRemoveExpr(vdi->e(), vdi);
  vdi->remove();
}

void EnvI::fail(const std::string& msg, const Location& loc) {
  if (!_failed) {
    addWarning(loc, std::string("model inconsistency detected") +
                        (msg.empty() ? std::string() : (": " + msg)));
    _failed = true;
    for (auto& i : *_flat) {
      i->remove();
    }
    auto* failedConstraint = new ConstraintI(Location().introduce(), constants.literalFalse);
    _flat->addItem(failedConstraint);
    _flat->addItem(SolveI::sat(Location().introduce()));
    for (auto& i : *output) {
      i->remove();
    }
    output->addItem(
        new OutputI(Location().introduce(), new ArrayLit(Location(), std::vector<Expression*>())));
    throw ModelInconsistent(*this, Location().introduce(), msg);
  }
}

bool EnvI::failed() const { return _failed; }

unsigned int EnvI::registerEnum(VarDeclI* vdi) {
  auto it = _enumMap.find(vdi);
  unsigned int ret;
  if (it == _enumMap.end()) {
    ret = static_cast<unsigned int>(_enumVarDecls.size());
    _enumVarDecls.push_back(vdi);
    _enumMap.insert(std::make_pair(vdi, ret));
  } else {
    ret = it->second;
  }
  return ret + 1;
}
VarDeclI* EnvI::getEnum(unsigned int i) const {
  assert(i > 0 && i <= _enumVarDecls.size());
  return _enumVarDecls[i - 1];
}
unsigned int EnvI::registerArrayEnum(const std::vector<unsigned int>& arrayEnum) {
  std::ostringstream oss;
  bool allZero = true;
  for (unsigned int i : arrayEnum) {
    assert(i <= _enumVarDecls.size() || i <= _tupleTypes.size() || i <= _recordTypes.size());
    oss << i << ".";
    allZero = allZero && (i == 0);
  }
  if (allZero) {
    return 0;
  }
  auto it = _arrayEnumMap.find(oss.str());
  unsigned int ret;
  if (it == _arrayEnumMap.end()) {
    ret = static_cast<unsigned int>(_arrayEnumDecls.size());
    _arrayEnumDecls.push_back(arrayEnum);
    _arrayEnumMap.insert(std::make_pair(oss.str(), ret));
  } else {
    ret = it->second;
  }
  return ret + 1;
}
const std::vector<unsigned int>& EnvI::getArrayEnum(unsigned int i) const {
  assert(i > 0 && i <= _arrayEnumDecls.size());
  return _arrayEnumDecls[i - 1];
}

unsigned int EnvI::registerTupleType(const std::vector<Type>& fields) {
  assert(!fields.empty());
  auto* tt = TupleType::a(fields);

  auto it = _tupleTypeMap.find(tt);
  unsigned int ret;
  if (it == _tupleTypeMap.end()) {
    ret = static_cast<unsigned int>(_tupleTypes.size());
    _tupleTypes.push_back(tt);
    _tupleTypeMap.emplace(std::make_pair(tt, ret));
  } else {
    TupleType::free(tt);
    ret = it->second;
  }
  return ret + 1;
}

unsigned int EnvI::registerTupleType(TypeInst* ti) {
  auto* dom = Expression::cast<ArrayLit>(ti->domain());

  bool isArrayOfArray = getTransparentType(dom) != Expression::type(dom);

  std::vector<Type> fields(isArrayOfArray ? 2 : dom->size());
  bool cv = false;
  bool var = true;
  for (unsigned int i = 0; i < dom->size(); i++) {
    auto* tii = Expression::cast<TypeInst>((*dom)[i]);

    // Register tuple field type when required
    if (tii->type().bt() == Type::BT_TUPLE && tii->type().typeId() == 0) {
      registerTupleType(tii);
    } else if (tii->type().bt() == Type::BT_RECORD && tii->type().typeId() == 0) {
      registerRecordType(tii);
    }

    fields[i] = tii->type();
    cv = cv || fields[i].isvar() || fields[i].cv();
    var = var && fields[i].isvar();
  }
  if (isArrayOfArray) {
    // Add marker for array of array
    fields[1] = Type();
  }

  // the TI_VAR ti is not processed by this function. This cononicalisation should have been done
  // during typechecking.
  assert(ti->type().ti() == Type::TI_PAR || var);

  unsigned int ret = registerTupleType(fields);
  Type t = ti->type();
  t.typeId(0);  // Reset any current TypeId
  t.cv(cv);
  t.ti(var ? Type::TI_VAR : Type::TI_PAR);
  // Register an array type when required
  if (!ti->ranges().empty()) {
    assert(t.dim() != 0);
    std::vector<unsigned int> typeIds(ti->ranges().size() + 1);
    for (unsigned int i = 0; i < ti->ranges().size(); i++) {
      typeIds[i] = ti->ranges()[i]->type().typeId();
    }
    typeIds[ti->ranges().size()] = ret;
    ret = registerArrayEnum(typeIds);
  } else {
    assert(t.dim() == 0);
  }
  t.typeId(ret);
  Expression::type(ti, t);
  Expression::type(ti->domain(), t.elemType(*this));
  return ret;
}

unsigned int EnvI::registerTupleType(ArrayLit* tup) {
  assert(tup->isTuple() && tup->dims() == 1);
  Type ty = tup->type();
  bool isArrayOfArray = ty.typeId() != 0 && getTupleType(ty.typeId())->size() != tup->size();
  ty.bt(Type::BT_TUPLE);
  ty.typeId(0);  // Reset any current TypeId
  std::vector<Type> fields(isArrayOfArray ? 2 : tup->size());
  bool cv = false;
  bool var = true;
  for (unsigned int i = 0; i < tup->size(); i++) {
    fields[i] = Expression::type((*tup)[i]);
    cv = cv || fields[i].isvar() || fields[i].cv();
    var = var && fields[i].isvar();
    assert(!fields[i].structBT() || fields[i].typeId() != 0);
  }
  if (isArrayOfArray) {
    fields[1] = Type();
  }
  unsigned int typeId = registerTupleType(fields);
  assert(ty.dim() == 0);  // Tuple literals do not have array dimensions (otherwise, we should
                          // register arrayEnumTypes)
  ty.cv(cv);
  ty.ti(var ? Type::TI_VAR : Type::TI_PAR);
  ty.typeId(typeId);
  Expression::type(tup, ty);
  return typeId;
}

Type EnvI::commonTuple(Type tuple1, Type tuple2, bool ignoreTuple1Dim) {
  if (tuple1 == tuple2) {
    return tuple1;
  }
  if (tuple1.isunknown() || tuple2.isunknown()) {
    return Type();
  }

  // Allow to ignore the dimensions of (in progress) LHS when arrayEnumIds not yet in use
  int oldDim = tuple1.dim();
  if (ignoreTuple1Dim) {
    unsigned int typeId = tuple1.typeId();
    tuple1.typeId(0);
    tuple1.dim(0);
    tuple1.typeId(typeId);
  }

  if (tuple1.dim() != tuple2.dim()) {
    return Type();
  }
  TupleType* tt1 = getTupleType(tuple1);
  TupleType* tt2 = getTupleType(tuple2);
  if (tt1->size() != tt2->size()) {
    return Type();
  }

  std::vector<Type> common(tt1->size());
  for (unsigned int i = 0; i < tt1->size(); i++) {
    common[i] = Type::commonType(*this, (*tt1)[i], (*tt2)[i]);
    if (common[i].isunknown()) {
      return Type();
    }
  }
  unsigned int typeId = registerTupleType(common);
  if (ignoreTuple1Dim) {
    tuple1.typeId(0);
    tuple1.dim(oldDim);
  } else if (tuple1.dim() != 0) {
    const std::vector<unsigned int>& arrayEnumIds1 = getArrayEnum(tuple1.typeId());
    const std::vector<unsigned int>& arrayEnumIds2 = getArrayEnum(tuple2.typeId());
    std::vector<unsigned int> typeIds(tuple1.dim() + 1);
    for (int i = 0; i < tuple1.dim(); i++) {
      if (arrayEnumIds1[i] == arrayEnumIds2[i]) {
        typeIds[i] = arrayEnumIds1[i];
      } else {
        typeIds[i] = 0;
      }
    }
    typeIds[tuple1.dim()] = typeId;
    typeId = registerArrayEnum(typeIds);
  }
  tuple1.typeId(typeId);
  tuple1.cv(tuple1.cv() || tuple2.cv());
  assert(tuple1.bt() != Type::BT_TUPLE || tuple1.typeId() != 0);
  return tuple1;
}

Type EnvI::commonRecord(Type record1, Type record2, bool ignoreRecord1Dim) {
  if (record1 == record2) {
    return record1;
  }
  if (record1.isunknown() || record2.isunknown()) {
    return Type();
  }

  // Allow to ignore the dimensions of (in progress) LHS when arrayEnumIds not yet in use
  int oldDim = record1.dim();
  if (ignoreRecord1Dim) {
    unsigned int typeId = record1.typeId();
    record1.typeId(0);
    record1.dim(0);
    record1.typeId(typeId);
  }

  if (record1.dim() != record2.dim()) {
    return Type();
  }
  RecordType* rt1 = getRecordType(record1);
  RecordType* rt2 = getRecordType(record2);
  if (rt1->size() != rt2->size()) {
    return Type();
  }

  std::vector<std::pair<ASTString, Type>> common(rt1->size());
  for (unsigned int i = 0; i < rt1->size(); i++) {
    ASTString name(rt1->fieldName(i));
    if (name != rt2->fieldName(i)) {
      return Type();
    }
    Type ct = Type::commonType(*this, (*rt1)[i], (*rt2)[i]);
    if (ct.isunknown()) {
      return Type();
    }
    common[i] = {name, ct};
  }
  unsigned int typeId = registerRecordType(common);

  if (ignoreRecord1Dim) {
    record1.typeId(0);
    record1.dim(oldDim);
  } else if (record1.dim() != 0) {
    const std::vector<unsigned int>& arrayEnumIds1 = getArrayEnum(record1.typeId());
    const std::vector<unsigned int>& arrayEnumIds2 = getArrayEnum(record2.typeId());
    std::vector<unsigned int> typeIds(record1.dim() + 1);
    for (int i = 0; i < record1.dim(); i++) {
      if (arrayEnumIds1[i] == arrayEnumIds2[i]) {
        typeIds[i] = arrayEnumIds1[i];
      } else {
        typeIds[i] = 0;
      }
    }
    typeIds[record1.dim()] = typeId;
    typeId = registerArrayEnum(typeIds);
  }
  record1.typeId(typeId);
  record1.cv(record1.cv() || record2.cv());
  return record1;
}

Type EnvI::mergeRecord(Type record1, Type record2, Location loc) {
  RecordType* fields1 = getRecordType(record1);
  RecordType* fields2 = getRecordType(record2);
  RecordFieldSort cmp;

  std::vector<std::pair<ASTString, Type>> all_fields;
  const unsigned int total_size = fields1->size() + fields2->size();
  all_fields.reserve(total_size);
  unsigned int l = 0;
  unsigned int r = 0;
  for (unsigned int i = 0; i < total_size; i++) {
    if (l >= fields1->size()) {
      // must choose rhs
      all_fields.emplace_back(fields2->fieldName(r), (*fields2)[r]);
      ++r;
    } else if (r >= fields2->size()) {
      // must choose lhs
      all_fields.emplace_back(fields1->fieldName(l), (*fields1)[l]);
      ++l;
    } else {
      ASTString lhsN(fields1->fieldName(l));
      ASTString rhsN(fields2->fieldName(r));
      if (cmp(lhsN, rhsN)) {
        // lhsN < rhsN
        all_fields.emplace_back(lhsN, (*fields1)[l]);
        ++l;
      } else {
        all_fields.emplace_back(rhsN, (*fields2)[r]);
        ++r;
      }
    }
    if (i > 0 && all_fields[i].first == all_fields[i - 1].first) {
      std::ostringstream ss;
      ss << "cannot merge of record types `" << record1.toString(*this) << "' and `"
         << record2.toString(*this) << "'. Both record types contain a field with the name `"
         << all_fields[i].first << "'";
      throw TypeError(*this, loc, ss.str());
    }
  }
  assert(r + l == all_fields.size());
  unsigned int typeId = registerRecordType(all_fields);
  bool is_var = record1.ti() == record2.ti() && record1.ti() == Type::TI_VAR;
  bool is_cv = record1.cv() || record2.cv();

  Type ret = Type::record(0, 0);  // Delay TypeId until TI and CV is set
  ret.ti(is_var ? Type::TI_VAR : Type::TI_PAR);
  ret.cv(is_cv);
  ret.typeId(typeId);
  return ret;
}

Type EnvI::concatTuple(Type tuple1, Type tuple2) {
  TupleType* fields1 = getTupleType(tuple1);
  TupleType* fields2 = getTupleType(tuple2);

  std::vector<Type> all_fields(fields1->size() + fields2->size());
  for (unsigned int i = 0; i < fields1->size(); i++) {
    all_fields[i] = (*fields1)[i];
  }
  for (unsigned int i = 0; i < fields2->size(); i++) {
    all_fields[fields1->size() + i] = (*fields2)[i];
  }
  unsigned int typeId = registerTupleType(all_fields);

  bool is_var = tuple1.ti() == tuple2.ti() && tuple1.ti() == Type::TI_VAR;
  bool is_cv = tuple1.cv() || tuple2.cv();

  Type ret = Type::tuple(0, 0);  // Delay TypeId until TI and CV is set
  ret.ti(is_var ? Type::TI_VAR : Type::TI_PAR);
  ret.cv(is_cv);
  ret.typeId(typeId);
  return ret;
}

Type EnvI::getTransparentType(Type t) const {
  if (t.istuple()) {
    auto* tt = getTupleType(t);
    if (tt->size() == 2 && (*tt)[1].isunknown()) {
      return (*tt)[0];
    }
  }
  return t;
}
Type EnvI::getTransparentType(const Expression* e) const {
  return getTransparentType(Expression::type(e));
}

std::string EnvI::enumToString(unsigned int enumId, int i) {
  Id* ti_id = getEnum(enumId)->e()->id();
  ASTString enumName(create_enum_to_string_name(ti_id, "_toString_"));
  auto* call = Call::a(Location().introduce(), enumName,
                       {IntLit::a(i), constants.boollit(true), constants.boollit(false)});
  auto* fi = model->matchFn(*this, call, false, true);
  assert(fi);
  call->decl(fi);
  Expression::type(call, Type::parstring());
  return eval_string(*this, call);
}
bool EnvI::isSubtype(const Type& t10, const Type& t2, bool strictEnums) const {
  Type t1 = t10;
  if (t1.istuple() && t2.dim() != 0) {
    auto* tupleType = getTupleType(t1);
    if (tupleType->size() == 2 && (*tupleType)[1].isunknown()) {
      t1 = (*tupleType)[0];
    }
  }
  if (!t1.isSubtypeOf(*this, t2, strictEnums)) {
    return false;
  }
  if (strictEnums && t1.bt() == Type::BT_INT && t1.dim() == 0 && t2.dim() != 0 &&
      t2.typeId() != 0) {
    // set assigned to an array
    const std::vector<unsigned int>& t2enumIds = getArrayEnum(t2.typeId());
    if (t2enumIds[t2enumIds.size() - 1] != 0 && t1.typeId() != t2enumIds[t2enumIds.size() - 1]) {
      return false;
    }
  }
  if (strictEnums && t1.bt() == Type::BT_INT && t1.dim() != 0 && t1.typeId() != t2.typeId()) {
    if (t1.typeId() == 0) {
      return t1.isbot();
    }
    if (t2.typeId() != 0) {
      const std::vector<unsigned int>& t1enumIds = getArrayEnum(t1.typeId());
      const std::vector<unsigned int>& t2enumIds = getArrayEnum(t2.typeId());
      assert(t1enumIds.size() == t2enumIds.size());
      for (unsigned int i = 0; i < t1enumIds.size() - 1; i++) {
        if (t2enumIds[i] != 0 && t1enumIds[i] != t2enumIds[i]) {
          return false;
        }
      }
      if (!t1.isbot() && t2enumIds[t1enumIds.size() - 1] != 0 &&
          t1enumIds[t1enumIds.size() - 1] != t2enumIds[t2enumIds.size() - 1]) {
        return false;
      }
    }
  }
  return true;
}

#define register_rt(rt)                                     \
  do {                                                      \
    auto it = _recordTypeMap.find(rt);                      \
    unsigned int ret;                                       \
    if (it == _recordTypeMap.end()) {                       \
      ret = static_cast<unsigned int>(_recordTypes.size()); \
      _recordTypes.push_back(rt);                           \
      _recordTypeMap.emplace(std::make_pair(rt, ret));      \
    } else {                                                \
      RecordType::free(rt);                                 \
      ret = it->second;                                     \
    }                                                       \
    return ret + 1;                                         \
  } while (0);

unsigned int EnvI::registerRecordType(const std::vector<std::pair<ASTString, Type>>& fields) {
  assert(!fields.empty());
  assert(std::is_sorted(fields.begin(), fields.end(), RecordFieldSort{}));
  auto* rt = RecordType::a(fields);
  register_rt(rt);
}

unsigned int EnvI::registerRecordType(const RecordType* orig, const std::vector<Type>& field_type) {
  RecordType* rt = RecordType::a(orig, field_type);
  register_rt(rt);
}

unsigned int EnvI::registerRecordType(ArrayLit* rec) {
  assert(rec->isTuple() && rec->dims() == 1 && rec->type().isrecord());
  Type ty = rec->type();
  ty.bt(Type::BT_RECORD);
  ty.typeId(0);  // Reset any current TypeId
  std::vector<VarDecl*> fields(rec->size());
  bool cv = false;
  bool var = true;
  for (unsigned int i = 0; i < rec->size(); i++) {
    fields[i] = Expression::cast<VarDecl>((*rec)[i]);
    cv = cv || fields[i]->type().isvar() || fields[i]->type().cv();
    var = var && fields[i]->type().isvar();
    assert(!fields[i]->type().structBT() || fields[i]->type().typeId() != 0);
  }

  // Sort fields (and literal content)
  std::sort(fields.begin(), fields.end(), RecordFieldSort{});
  for (unsigned int i = 0; i < rec->size(); i++) {
    rec->set(i, fields[i]->e());
  }

  std::vector<std::pair<ASTString, Type>> field_pairs(fields.size());
  for (unsigned int i = 0; i < fields.size(); i++) {
    field_pairs[i] = {fields[i]->id()->str(), fields[i]->type()};
    if (i > 0 && field_pairs[i - 1].first == field_pairs[i].first) {
      std::ostringstream oss;
      oss << "record contains multiple fields with the name `" << field_pairs[i].first.c_str()
          << "'.";
      throw TypeError(*this, Expression::loc(rec), oss.str());
    }
  }

  unsigned int typeId = registerRecordType(field_pairs);
  assert(ty.dim() == 0);  // Tuple literals do not have array dimensions (otherwise, we should
                          // register arrayEnumTypes)
  ty.cv(cv);
  ty.ti(var ? Type::TI_VAR : Type::TI_PAR);
  ty.typeId(typeId);
  Expression::type(rec, ty);
  return typeId;
}

unsigned int EnvI::registerRecordType(TypeInst* ti) {
  auto* dom = Expression::cast<ArrayLit>(ti->domain());

  // Register new Record
  bool cv = false;
  bool var = true;
  unsigned int ret;
  if (ti->type().typeId() == 0) {
    std::vector<std::pair<ASTString, Type>> field_pairs(dom->size());
    std::vector<VarDecl*> fields(dom->size());
    for (unsigned int i = 0; i < dom->size(); i++) {
      fields[i] = Expression::cast<VarDecl>((*dom)[i]);

      // Register tuple field type when required
      if (fields[i]->type().bt() == Type::BT_TUPLE && fields[i]->type().typeId() == 0) {
        registerTupleType(fields[i]->ti());
        Expression::type(fields[i], fields[i]->ti()->type());
      } else if (fields[i]->type().bt() == Type::BT_RECORD && fields[i]->type().typeId() == 0) {
        registerRecordType(fields[i]->ti());
        Expression::type(fields[i], fields[i]->ti()->type());
      }

      cv = cv || fields[i]->type().isvar() || fields[i]->type().cv();
      var = var && fields[i]->type().isvar();
    }

    // Sort fields (and literal content)
    std::sort(fields.begin(), fields.end(), RecordFieldSort{});
    for (unsigned int i = 0; i < dom->size(); i++) {
      dom->set(i, fields[i]->ti());
    }

    for (unsigned int i = 0; i < fields.size(); i++) {
      field_pairs[i] = {fields[i]->id()->str(), fields[i]->type()};
      if (i > 0 && field_pairs[i - 1].first == field_pairs[i].first) {
        std::ostringstream oss;
        oss << "record type contains multiple fields with the name `"
            << field_pairs[i].first.c_str() << "'.";
        throw TypeError(*this, Expression::loc(ti), oss.str());
      }
    }
    ret = registerRecordType(field_pairs);
  } else {  // Update already registered Record (dom contains TypeInst, no need to sort)
    std::vector<Type> types(dom->size());
    RecordType* orig = getRecordType(ti->type());
    for (unsigned int i = 0; i < types.size(); i++) {
      types[i] = Expression::type((*dom)[i]);

      cv = cv || Expression::type((*dom)[i]).isvar() || Expression::type((*dom)[i]).cv();
      var = var && Expression::type((*dom)[i]).isvar();
    }
    ret = registerRecordType(orig, types);
  }
  // the TI_VAR ti is not processed by this function. This cononicalisation should have been done
  // during typechecking.
  assert(ti->type().ti() == Type::TI_PAR || var);

  Type t = ti->type();
  t.typeId(0);  // Reset any current TypeId
  t.cv(cv);
  t.ti(var ? Type::TI_VAR : Type::TI_PAR);
  // Register an array type when required
  if (!ti->ranges().empty()) {
    assert(t.dim() != 0);
    std::vector<unsigned int> typeIds(ti->ranges().size() + 1);
    for (unsigned int i = 0; i < ti->ranges().size(); i++) {
      typeIds[i] = ti->ranges()[i]->type().typeId();
    }
    typeIds[ti->ranges().size()] = ret;
    ret = registerArrayEnum(typeIds);
  } else {
    assert(t.dim() == 0);
  }
  t.typeId(ret);
  Expression::type(ti, t);
  Expression::type(ti->domain(), t.elemType(*this));
  return ret;
}

void EnvI::collectVarDecls(bool b) { _collectVardecls = b; }
void EnvI::voAddExp(VarDecl* vd) {
  if ((vd->e() != nullptr) && Expression::isa<Call>(vd->e()) &&
      !Expression::type(vd->e()).isAnn()) {
    int prev = !idStack.empty() ? idStack.back() : 0;
    for (int i = static_cast<int>(callStack.size()) - 1; i >= prev; i--) {
      Expression* ee = callStack[i].e;
      for (ExpressionSetIter it = Expression::ann(ee).begin(); it != Expression::ann(ee).end();
           ++it) {
        Expression* ann = *it;
        if (ann != constants.ann.add_to_output && ann != constants.ann.output &&
            ann != constants.ann.rhs_from_assignment) {
          bool needAnnotation = true;
          if (Call* ann_c = Expression::dynamicCast<Call>(ann)) {
            if (ann_c->id() == constants.ann.defines_var) {
              // only add defines_var annotation if vd is the defined variable
              if (Id* defined_var = Expression::dynamicCast<Id>(ann_c->arg(0))) {
                if (defined_var->decl() != vd->id()->decl()) {
                  needAnnotation = false;
                }
              }
            }
          }
          if (needAnnotation) {
            EE ee_ann = flat_exp(*this, Ctx(), *it, nullptr, constants.varTrue);
            Expression::addAnnotation(vd->e(), ee_ann.r());
          }
        }
      }
    }
  }
  int idx = varOccurrences.find(vd);
  CollectOccurrencesE ce(*this, varOccurrences, (*_flat)[idx]);
  top_down(ce, vd->e());
  if (_collectVardecls) {
    modifiedVarDecls.push_back(idx);
  }
}
Model* EnvI::flat() { return _flat; }
void EnvI::swap() {
  Model* tmp = model;
  model = _flat;
  _flat = tmp;
}
ASTString EnvI::reifyId(const ASTString& id) {
  auto it = _reifyMap.find(id);
  if (it == _reifyMap.end()) {
    std::ostringstream ss;
    ss << id << "_reif";
    return ASTString(ss.str());
  }
  return it->second;
}
#undef MZN_FILL_REIFY_MAP
ASTString EnvI::halfReifyId(const ASTString& id) {
  std::ostringstream ss;
  ss << id << "_imp";
  return ASTString(ss.str());
}

int EnvI::addWarning(const std::string& msg) { return addWarning(Location(), msg, false); }

int EnvI::addWarning(const Location& loc, const std::string& msg, bool dumpStack) {
  if (fopts.supressWarnings) {
    return -1;
  }
  if (warnings.size() >= 20) {
    if (warnings.size() == 20) {
      warnings.emplace_back(new Warning("Further warnings have been suppressed."));
    }
    return -1;
  }
  if (dumpStack) {
    warnings.emplace_back(new Warning(*this, loc, msg));
  } else {
    warnings.emplace_back(new Warning(loc, msg));
  }
  return static_cast<int>(warnings.size()) - 1;
}

Call* EnvI::surroundingCall() const {
  if (callStack.size() >= 2) {
    return Expression::dynamicCast<Call>(callStack[callStack.size() - 2].e);
  }
  return nullptr;
}

void EnvI::cleanupExceptOutput() {
  cmap.clear();
  _cseMap.clear();
  delete _flat;
  delete model;
  delete originalModel;
  _flat = nullptr;
  model = nullptr;
}

bool EnvI::outputSectionEnabled(ASTString section) const {
  return fopts.notSections.count(section.c_str()) == 0 &&
         (fopts.onlySections.empty() || fopts.onlySections.count(section.c_str()) > 0);
}

std::string EnvI::show(Expression* e) {
  auto* call = Call::a(Location().introduce(), constants.ids.show, {e});
  call->decl(model->matchFn(*this, call, false));
  Expression::type(call, Type::parstring());
  return eval_string(*this, call);
}

std::string EnvI::show(const IntVal& iv, unsigned int enumId) {
  if (enumId == 0 || !iv.isFinite()) {
    std::stringstream ss;
    ss << iv;
    return ss.str();
  }
  return enumToString(enumId, static_cast<int>(iv.toInt()));
}

std::string EnvI::show(IntSetVal* isv, unsigned int enumId) {
  auto* sl = new SetLit(Location().introduce(), isv);
  Type t = sl->type();
  t.typeId(enumId);
  Expression::type(sl, t);
  return show(sl);
}

CallStackItem::CallStackItem(EnvI& env0, Expression* e, const Ctx& ctx)
    : _env(env0), _csiType(CSI_NONE) {
  assert(Expression::type(e).bt() != Type::BT_UNKNOWN);

  env0.checkCancel();

  if (Expression::isa<VarDecl>(e)) {
    _env.idStack.push_back(static_cast<int>(_env.callStack.size()));
    _csiType = CSI_VD;
  } else if (Expression::isa<Call>(e)) {
    if (Expression::cast<Call>(e)->id() == _env.constants.ids.redundant_constraint) {
      _env.inRedundantConstraint++;
      _csiType = CSI_REDUNDANT;
    } else if (Expression::cast<Call>(e)->id() == _env.constants.ids.symmetry_breaking_constraint) {
      _env.inSymmetryBreakingConstraint++;
      _csiType = CSI_SYMMETRY;
    }
  }
  if (Expression::ann(e).contains(_env.constants.ann.maybe_partial)) {
    _env.inMaybePartial++;
    _maybePartial = true;
  } else {
    _maybePartial = false;
  }
  _env.callStack.emplace_back(e, false, ctx);
  _env.maxCallStack = std::max(_env.maxCallStack, static_cast<unsigned int>(_env.callStack.size()));
}
CallStackItem::CallStackItem(EnvI& env0, Id* ident, IntVal i)
    : _env(env0), _csiType(CSI_NONE), _maybePartial(false) {
  _env.callStack.emplace_back(ident, true, Ctx());
  _env.maxCallStack = std::max(_env.maxCallStack, static_cast<unsigned int>(_env.callStack.size()));
}
void CallStackItem::replace() { _env.callStack.back().replaced = true; }
CallStackItem::~CallStackItem() {
  switch (_csiType) {
    case CSI_NONE:
      break;
    case CSI_VD:
      _env.idStack.pop_back();
      break;
    case CSI_REDUNDANT:
      _env.inRedundantConstraint--;
      break;
    case CSI_SYMMETRY:
      _env.inSymmetryBreakingConstraint--;
      break;
  }
  if (_maybePartial) {
    _env.inMaybePartial--;
  }
  assert(!_env.callStack.empty());
  _env.callStack.pop_back();
}

FlatteningError::FlatteningError(EnvI& env, const Location& loc, const std::string& msg)
    : LocationException(env, loc, msg) {}

Env::Env(Model* m, std::ostream& outstream, std::ostream& errstream)
    : _e(new EnvI(m, outstream, errstream)) {}
Env::~Env() { delete _e; }

Model* Env::model() { return _e->model; }
void Env::model(Model* m) { _e->model = m; }
Model* Env::flat() { return _e->flat(); }
void Env::swap() { _e->swap(); }
Model* Env::output() { return _e->output; }

std::ostream& Env::evalOutput(std::ostream& os, std::ostream& log) {
  return _e->evalOutput(os, log);
}
EnvI& Env::envi() { return *_e; }
const EnvI& Env::envi() const { return *_e; }

bool EnvI::dumpPath(std::ostream& os, bool force) {
  force = force ? force : fopts.collectMznPaths;
  if (callStack.size() > varPathStore.maxPathDepth) {
    if (!force && multiPassInfo.currentPassNumber >= multiPassInfo.finalPassNumber - 1) {
      return false;
    }
    varPathStore.maxPathDepth = static_cast<int>(callStack.size());
  }

  auto lastError = static_cast<unsigned int>(callStack.size());

  std::string major_sep = ";";
  std::string minor_sep = "|";
  for (unsigned int i = 0; i < lastError; i++) {
    Expression* e = callStack[i].e;
    bool isCompIter = callStack[i].tag;
    Location loc = Expression::loc(e);
    auto findFilename = varPathStore.filenameSet.find(loc.filename());
    if (findFilename == varPathStore.filenameSet.end()) {
      if (!force && multiPassInfo.currentPassNumber >= multiPassInfo.finalPassNumber - 1) {
        return false;
      }
      varPathStore.filenameSet.insert(loc.filename());
    }

    // If this call is not a dummy StringLit with empty Location (so that deferred compilation
    // doesn't drop the paths)
    if (Expression::eid(e) != Expression::E_STRINGLIT || (loc.firstLine() != 0U) ||
        (loc.firstColumn() != 0U) || (loc.lastLine() != 0U) || (loc.lastColumn() != 0U)) {
      os << loc.filename() << minor_sep << loc.firstLine() << minor_sep << loc.firstColumn()
         << minor_sep << loc.lastLine() << minor_sep << loc.lastColumn() << minor_sep;
      switch (Expression::eid(e)) {
        case Expression::E_INTLIT:
          os << "il" << minor_sep << *e;
          break;
        case Expression::E_FLOATLIT:
          os << "fl" << minor_sep << *e;
          break;
        case Expression::E_SETLIT:
          os << "sl" << minor_sep << *e;
          break;
        case Expression::E_BOOLLIT:
          os << "bl" << minor_sep << *e;
          break;
        case Expression::E_STRINGLIT:
          os << "stl" << minor_sep << *e;
          break;
        case Expression::E_ID:
          if (isCompIter) {
            // if (e->cast<Id>()->decl()->e()->type().isPar())
            os << *e << "=" << *Expression::cast<Id>(e)->decl()->e();
            // else
            //  os << *e << "=?";
          } else {
            os << "id" << minor_sep << *e;
          }
          break;
        case Expression::E_ANON:
          os << "anon";
          break;
        case Expression::E_ARRAYLIT:
          os << "al";
          break;
        case Expression::E_ARRAYACCESS:
          os << "aa";
          break;
        case Expression::E_COMP: {
          const Comprehension* cmp = Expression::cast<Comprehension>(e);
          if (cmp->set()) {
            os << "sc";
          } else {
            os << "ac";
          }
        } break;
        case Expression::E_ITE:
          os << "ite";
          break;
        case Expression::E_BINOP:
          os << "bin" << minor_sep << Expression::cast<BinOp>(e)->opToString();
          break;
        case Expression::E_UNOP:
          os << "un" << minor_sep << Expression::cast<UnOp>(e)->opToString();
          break;
        case Expression::E_CALL:
          if (fopts.onlyToplevelPaths) {
            return false;
          }
          os << "ca" << minor_sep << Expression::cast<Call>(e)->id();
          break;
        case Expression::E_VARDECL:
          os << "vd";
          break;
        case Expression::E_LET:
          os << "l";
          break;
        case Expression::E_TI:
          os << "ti";
          break;
        case Expression::E_TIID:
          os << "ty";
          break;
        default:
          assert(false);
          os << "unknown expression (internal error)";
          break;
      }
      os << major_sep;
    } else {
      os << Expression::cast<StringLit>(e)->v() << major_sep;
    }
  }
  return true;
}

void populate_output(Env& env, bool encapsulateJSON) {
  EnvI& envi = env.envi();
  Model* _flat = envi.flat();
  Model* _output = envi.output;
  std::vector<Expression*> outputVars;
  for (VarDeclIterator it = _flat->vardecls().begin(); it != _flat->vardecls().end(); ++it) {
    VarDecl* vd = it->e();
    Annotation& ann = Expression::ann(vd);
    ArrayLit* dims = nullptr;
    bool has_output_ann = false;
    if (!ann.isEmpty()) {
      for (ExpressionSetIter ait = ann.begin(); ait != ann.end(); ++ait) {
        if (Call* c = Expression::dynamicCast<Call>(*ait)) {
          if (c->id() == envi.constants.ann.output_array) {
            dims = Expression::cast<ArrayLit>(c->arg(0));
            has_output_ann = true;
            break;
          }
        } else if (Expression::isa<Id>(*ait) &&
                   Expression::cast<Id>(*ait)->str() == envi.constants.ann.output_var->str()) {
          has_output_ann = true;
        }
      }
      if (has_output_ann) {
        std::ostringstream s;
        s << vd->id()->str() << " = ";

        auto* vd_output = Expression::cast<VarDecl>(copy(env.envi(), vd));
        Type vd_t = vd_output->type();
        vd_t.mkPar(env.envi());
        Expression::type(vd_output, vd_t);
        Expression::type(vd_output->ti(), vd_t);

        if (dims != nullptr) {
          std::vector<TypeInst*> ranges(dims->size());
          s << "array" << dims->size() << "d(";
          for (unsigned int i = 0; i < dims->size(); i++) {
            IntSetVal* idxset = eval_intset(envi, (*dims)[i]);
            ranges[i] = new TypeInst(Location().introduce(), Type(),
                                     new SetLit(Location().introduce(), idxset));
            s << *idxset << ",";
          }
          Type t = vd_t;
          vd_t.dim(static_cast<int>(dims->size()));
          Expression::type(vd_output, t);
          vd_output->ti(new TypeInst(Location().introduce(), vd_t, ranges));
        }
        _output->addItem(VarDeclI::a(Location().introduce(), vd_output));

        auto* sl = new StringLit(Location().introduce(), s.str());
        outputVars.push_back(sl);

        std::vector<Expression*> showArgs(1);
        showArgs[0] = vd_output->id();
        Call* show = Call::a(Location().introduce(), envi.constants.ids.show, showArgs);
        Expression::type(show, Type::parstring());
        FunctionI* fi = _flat->matchFn(envi, show, false);
        assert(fi);
        show->decl(fi);
        outputVars.push_back(show);
        std::string ends = dims != nullptr ? ")" : "";
        ends += ";\n";
        auto* eol = new StringLit(Location().introduce(), ends);
        outputVars.push_back(eol);
      }
    }
  }
  auto* al = new ArrayLit(Location().introduce(), outputVars);
  Expression::type(al, Type::parstring(1));
  if (encapsulateJSON) {
    auto* concat = Call::a(Location().introduce(), envi.constants.ids.concat, {al});
    concat->type(Type::parstring());
    concat->decl(_flat->matchFn(envi, concat, false));
    auto* showJSON = Call::a(Location().introduce(), envi.constants.ids.showJSON, {concat});
    showJSON->type(Type::parstring());
    showJSON->decl(_flat->matchFn(envi, showJSON, false));
    std::vector<Expression*> al_v(
        {new StringLit(Location().introduce(), "\"output\": {\"dzn\": "), showJSON,
         new StringLit(Location().introduce(), "}, \"sections\": [\"dzn\"]")});
    al = new ArrayLit(Location().introduce(), al_v);
    Expression::type(al, Type::parstring(1));
  }
  auto* newOutputItem = new OutputI(Location().introduce(), al);
  _output->addItem(newOutputItem);
  envi.flat()->mergeStdLib(envi, _output);
}

std::ostream& EnvI::evalOutput(std::ostream& os, std::ostream& log) {
  GCLock lock;
  warnings.clear();
  ArrayLit* al = eval_array_lit(*this, output->outputItem()->e());
  bool fLastEOL = true;
  for (unsigned int i = 0; i < al->size(); i++) {
    std::string s = eval_string(*this, (*al)[i]);
    if (!s.empty()) {
      os << s;
      fLastEOL = ('\n' == s.back());
    }
  }
  if (!fLastEOL) {
    os << '\n';
  }
  for (auto& w : warnings) {
    w->print(log, false);
  }
  return os;
}

const std::vector<std::unique_ptr<Warning>>& Env::warnings() { return envi().warnings; }

std::ostream& Env::dumpWarnings(std::ostream& os, bool werror, bool json, int exceptWarning) {
  int curIdx = 0;
  bool didPrint = false;
  for (const auto& warning : warnings()) {
    if (curIdx == exceptWarning) {
      continue;
    }
    if (!json && (curIdx > 1 || (curIdx == 1 && exceptWarning != 0))) {
      os << "\n";
    }
    curIdx++;
    if (json) {
      warning->json(os, werror);
    } else {
      didPrint = true;
      warning->print(os, werror);
    }
  }
  if (didPrint) {
    os << "\n";
  }
  return os;
}

void Env::clearWarnings() { envi().warnings.clear(); }

unsigned int Env::maxCallStack() const { return envi().maxCallStack; }

void check_index_sets(EnvI& env, VarDecl* vd, Expression* e, bool isArg) {
  struct ToCheck {
    Expression* accessor;
    Expression* e;
    TypeInst* ti;
    ToCheck(Expression* _accessor, Expression* _e, TypeInst* _ti)
        : accessor(_accessor), e(_e), ti(_ti) {}
  };
  GCLock lock;
  bool hasName = !vd->id()->str().empty();
  std::vector<ToCheck> todo({{hasName ? vd->id() : nullptr, e, vd->ti()}});
  CopyMap cm;
  bool needNewTypeInst = false;
  // Firstly just walks through checking index sets, then if we encounter an error,
  // starts again recording what field accessors are needed to generate the error message.
  bool hadError = false;
  while (!todo.empty()) {
    auto item = todo.back();
    todo.pop_back();
    if (Expression::type(item.e).dim() > 0) {
      ASTExprVec<TypeInst> tis = item.ti->ranges();
      GCLock lock;
      switch (Expression::eid(item.e)) {
        case Expression::E_ID: {
          Id* id = Expression::cast<Id>(item.e);
          ASTExprVec<TypeInst> e_tis = id->decl()->ti()->ranges();
          if (tis.size() != e_tis.size()) {
            continue;
          }
          for (unsigned int i = 0; i < e_tis.size(); i++) {
            if (tis[i]->domain() == nullptr) {
              if (!isArg) {
                cm.insert(tis[i], e_tis[i]);
                needNewTypeInst = true;
              }
            } else if (!Expression::isa<TIId>(tis[i]->domain())) {
              IntSetVal* isv0 = eval_intset(env, tis[i]->domain());
              IntSetVal* isv1 = eval_intset(env, e_tis[i]->domain());
              if (!isv0->equal(isv1)) {
                if (item.accessor != nullptr) {
                  std::ostringstream oss;
                  oss << "Index set mismatch. Declared index "
                      << (tis.size() == 1 ? "set" : "sets");
                  oss << (isArg ? " of argument " : " of ") << "`" << *item.accessor << "' "
                      << (tis.size() == 1 ? "is [" : "are [");
                  for (unsigned int j = 0; j < tis.size(); j++) {
                    if (tis[j]->domain() != nullptr) {
                      oss << env.show(tis[j]->domain());
                    } else {
                      oss << "int";
                    }
                    if (j < tis.size() - 1) {
                      oss << ", ";
                    }
                  }
                  oss << "], but is assigned to array `" << *id << "' with index "
                      << (tis.size() == 1 ? "set [" : "sets [");
                  for (unsigned int j = 0; j < e_tis.size(); j++) {
                    oss << env.show(e_tis[j]->domain());
                    if (j < e_tis.size() - 1) {
                      oss << ", ";
                    }
                  }
                  oss << "]. You may need to coerce the index sets using the array" << tis.size()
                      << "d function.";
                  throw EvalError(env, Expression::loc(e), oss.str());
                }
                hadError = true;
                todo.clear();
                Id* ident = hasName ? vd->id()
                                    : new Id(Location().introduce(),
                                             env.constants.ids.unnamedArgument, nullptr);
                todo.emplace_back(ident, e, vd->ti());
              }
            }
          }
        } break;
        case Expression::E_ARRAYLIT: {
          auto* al = Expression::cast<ArrayLit>(item.e);
          if (tis.size() != al->dims()) {
            continue;
          }
          for (unsigned int i = 0; i < tis.size(); i++) {
            if (tis[i]->domain() == nullptr) {
              if (!isArg) {
                cm.insert(tis[i], new TypeInst(Location().introduce(), Type::parint(),
                                               new SetLit(Location().introduce(),
                                                          IntSetVal::a(al->min(i), al->max(i)))));
                needNewTypeInst = true;
              }
            } else if ((i == 0 || !al->empty()) && !Expression::isa<TIId>(tis[i]->domain())) {
              IntSetVal* isv = eval_intset(env, tis[i]->domain());
              assert(isv->size() <= 1);
              if ((isv->empty() && al->min(i) <= al->max(i)) ||
                  (!isv->empty() && (isv->min(0) != al->min(i) || isv->max(0) != al->max(i)))) {
                if (item.accessor != nullptr) {
                  std::ostringstream oss;
                  oss << "Index set mismatch. Declared index "
                      << (tis.size() == 1 ? "set" : "sets");
                  oss << (isArg ? " of argument " : " of ") << "`" << *item.accessor << "' "
                      << (tis.size() == 1 ? "is [" : "are [");
                  for (unsigned int j = 0; j < tis.size(); j++) {
                    if (tis[j]->domain() != nullptr) {
                      oss << env.show(tis[j]->domain());
                    } else {
                      oss << "int";
                    }
                    if (j < tis.size() - 1) {
                      oss << ",";
                    }
                  }
                  oss << "], but is assigned to array with index "
                      << (tis.size() == 1 ? "set [" : "sets [");
                  Type al_t = al->type();
                  if (al_t.typeId() == 0) {
                    for (unsigned int j = 0; j < al->dims(); j++) {
                      oss << al->min(j) << ".." << al->max(j);
                      if (j < al->dims() - 1) {
                        oss << ", ";
                      }
                    }
                  } else {
                    const auto& arrayIds = env.getArrayEnum(al_t.typeId());
                    for (unsigned int j = 0; j < al->dims(); j++) {
                      display_enum_range(oss, env, al->min(j), al->max(j), arrayIds[j]);
                      if (j < al->dims() - 1) {
                        oss << ", ";
                      }
                    }
                  }
                  oss << "]. You may need to coerce the index sets using the array" << tis.size()
                      << "d function.";
                  throw EvalError(env, Expression::loc(e), oss.str());
                }
                hadError = true;
                todo.clear();
                Id* ident = hasName ? vd->id()
                                    : new Id(Location().introduce(),
                                             env.constants.ids.unnamedArgument, nullptr);
                todo.emplace_back(ident, e, vd->ti());
              } else {
                unsigned int enumId = item.ti->type().typeId();
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
                    access = new ArrayAccess(Location().introduce(), item.accessor, indexes_s);
                  }
                  todo.emplace_back(access, (*al)[i], item.ti);
                }
              }
            }
          }
        } break;
        default:
          throw InternalError("not supported yet");
      }
    } else if (Expression::type(item.e).istuple()) {
      auto* domains = Expression::dynamicCast<ArrayLit>(item.ti->domain());
      if (domains != nullptr) {
        auto* al = eval_array_lit(env, item.e);
        for (unsigned int i = 0; i < al->size(); i++) {
          auto* access =
              hadError ? new FieldAccess(Location().introduce(), item.accessor, IntLit::a(i + 1))
                       : nullptr;
          todo.emplace_back(access, (*al)[i], Expression::cast<TypeInst>((*domains)[i]));
        }
      }
    } else if (Expression::type(item.e).isrecord()) {
      auto* domains = Expression::dynamicCast<ArrayLit>(item.ti->domain());
      if (domains != nullptr) {
        RecordType* rt = env.getRecordType(Expression::type(item.e));
        auto* al = eval_array_lit(env, item.e);
        for (unsigned int i = 0; i < al->size(); i++) {
          Expression* access = nullptr;
          if (hadError) {
            auto* field = new Id(Location().introduce(), rt->fieldName(i), nullptr);
            access = new FieldAccess(Location().introduce(), item.accessor, field);
          }
          todo.emplace_back(access, (*al)[i], Expression::cast<TypeInst>((*domains)[i]));
        }
      }
    }
  }
  if (needNewTypeInst) {
    vd->ti(Expression::cast<TypeInst>(copy(env, cm, vd->ti())));
  }
}

Expression* mk_domain_constraint(EnvI& env, Expression* expr, Expression* dom) {
  assert(GC::locked());
  Type ty = Expression::type(expr);
  Type retType = ty.isPar() ? Type::parbool() : Type::varbool();

  std::vector<Expression*> domargs(2, nullptr);
  domargs[0] = expr;

  switch (ty.bt()) {
    case Type::BT_INT: {
      IntSetVal* isv = eval_intset(env, dom);
      if (ty.st() != Type::ST_SET && ty.dim() == 0) {
        IntBounds ib = compute_int_bounds(env, expr);
        if (ib.valid && !isv->empty() && isv->isSubset(IntSetVal::Range(ib.l, ib.u))) {
          return nullptr;
        }
      }
      domargs[1] = new SetLit(Expression::loc(dom), isv);
      break;
    }
    case Type::BT_FLOAT: {
      FloatSetVal* fsv = eval_floatset(env, dom);
      if (ty.dim() == 0) {
        FloatBounds fb = compute_float_bounds(env, expr);
        if (fb.valid && !fsv->empty() && fsv->isSubset(FloatSetVal::Range(fb.l, fb.u))) {
          return nullptr;
        }
      }
      if (fsv->size() == 1) {
        domargs[1] = FloatLit::a(fsv->min());
        domargs.push_back(FloatLit::a(fsv->max()));
      } else {
        domargs[1] = new SetLit(Expression::loc(dom), fsv);
      }
      break;
    }
    case Type::BT_TUPLE:  // fall through
    case Type::BT_RECORD: {
      ArrayLit* al = eval_array_lit(env, expr);
      StructType* st = env.getStructType(ty);
      auto* dom_al = Expression::cast<ArrayLit>(dom);

      std::vector<std::pair<int, int>> dims;
      if (ty.dim() > 0) {
        dims.resize(al->dims());
        for (unsigned int i = 0; i < al->dims(); i++) {
          dims[i] = std::make_pair(al->min(i), al->max(i));
        }
      }
      std::vector<Expression*> fieldwise;
      for (unsigned int i = 0; i < dom_al->size(); ++i) {
        auto* field_ti = Expression::cast<TypeInst>((*dom_al)[i]);
        if (field_ti->domain() != nullptr) {
          if (ty.dim() > 0) {
            ArrayLit* slice = field_slice(env, st, al, dims, i + 1);
            for (unsigned int j = 0; j < slice->size(); j++) {
              auto* con = mk_domain_constraint(env, (*slice)[j], field_ti->domain());
              if (con != nullptr) {
                fieldwise.push_back(con);
              };
            }
          } else {
            auto* con = mk_domain_constraint(env, (*al)[i], field_ti->domain());
            if (con != nullptr) {
              fieldwise.push_back(con);
            };
          }
        }
      }
      if (fieldwise.size() <= 1) {
        return fieldwise.empty() ? nullptr : fieldwise[0];
      }
      auto* forall_al = new ArrayLit(Location().introduce(), fieldwise);
      Type al_t = retType;
      al_t.dim(1);
      forall_al->type(al_t);
      auto* c = Call::a(Location().introduce(), env.constants.ids.forall, {forall_al});
      c->type(retType);
      c->decl(env.model->matchFn(env, c, false));
      return c;
    }
    case Type::BT_BOT: {
      // Nothing to be done for empty arrays/sets
      return nullptr;
    }
    default: {
      throw EvalError(env, Expression::loc(dom),
                      "domain restrictions other than int and float not supported yet");
    }
  }
  assert(domargs[1] != nullptr);

  Call* c = Call::a(Location().introduce(), "var_dom", domargs);
  c->type(Type::varbool());
  c->decl(env.model->matchFn(env, c, false));
  if (c->decl() == nullptr) {
    throw InternalError("no matching declaration found for var_dom");
  }
  c->type(retType);
  return c;
}

/// Turn \a c into domain constraints if possible.
/// Return whether \a c is still required in the model.
bool check_domain_constraints(EnvI& env, Call* c) {
  if (env.fopts.recordDomainChanges) {
    return true;
  }
  if (c->id() == env.constants.ids.int_.le) {
    Expression* e0 = c->arg(0);
    Expression* e1 = c->arg(1);
    if (Expression::type(e0).isPar() && Expression::isa<Id>(e1)) {
      // greater than
      Id* id = Expression::cast<Id>(e1);
      IntVal lb = eval_int(env, e0);
      if (id->decl()->ti()->domain() != nullptr) {
        IntSetVal* domain = eval_intset(env, id->decl()->ti()->domain());
        assert(!domain->empty());
        if (domain->min() >= lb) {
          return false;
        }
        if (domain->max() < lb) {
          env.fail();
          return false;
        }
        IntSetRanges dr(domain);
        Ranges::Const<IntVal> cr(lb, IntVal::infinity());
        Ranges::Inter<IntVal, IntSetRanges, Ranges::Const<IntVal>> i(dr, cr);
        IntSetVal* newibv = IntSetVal::ai(i);
        id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
        id->decl()->ti()->setComputedDomain(false);
      } else {
        id->decl()->ti()->domain(
            new SetLit(Location().introduce(), IntSetVal::a(lb, IntVal::infinity())));
      }
      return false;
    }
    if (Expression::type(e1).isPar() && Expression::isa<Id>(e0)) {
      // less than
      Id* id = Expression::cast<Id>(e0);
      IntVal ub = eval_int(env, e1);
      if (id->decl()->ti()->domain() != nullptr) {
        IntSetVal* domain = eval_intset(env, id->decl()->ti()->domain());
        if (domain->max() <= ub) {
          return false;
        }
        if (domain->min() > ub) {
          env.fail();
          return false;
        }
        IntSetRanges dr(domain);
        Ranges::Const<IntVal> cr(-IntVal::infinity(), ub);
        Ranges::Inter<IntVal, IntSetRanges, Ranges::Const<IntVal>> i(dr, cr);
        IntSetVal* newibv = IntSetVal::ai(i);
        id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
        id->decl()->ti()->setComputedDomain(false);
      } else {
        id->decl()->ti()->domain(
            new SetLit(Location().introduce(), IntSetVal::a(-IntVal::infinity(), ub)));
      }
    }
  } else if (c->id() == env.constants.ids.int_.lin_le) {
    auto* al_c = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
    if (al_c->size() == 1) {
      auto* al_x = Expression::cast<ArrayLit>(follow_id(c->arg(1)));
      IntVal coeff = eval_int(env, (*al_c)[0]);
      IntVal y = eval_int(env, c->arg(2));
      IntVal lb = -IntVal::infinity();
      IntVal ub = IntVal::infinity();
      IntVal r = y % coeff;
      if (coeff >= 0) {
        ub = y / coeff;
        if (r < 0) {
          --ub;
        }
      } else {
        lb = y / coeff;
        if (r < 0) {
          ++lb;
        }
      }
      if (Id* id = Expression::dynamicCast<Id>((*al_x)[0])) {
        if (id->decl()->ti()->domain() != nullptr) {
          IntSetVal* domain = eval_intset(env, id->decl()->ti()->domain());
          assert(!domain->empty());
          if (domain->max() <= ub && domain->min() >= lb) {
            return false;
          }
          if (domain->min() > ub || domain->max() < lb) {
            env.fail();
            return false;
          }
          IntSetRanges dr(domain);
          Ranges::Const<IntVal> cr(lb, ub);
          Ranges::Inter<IntVal, IntSetRanges, Ranges::Const<IntVal>> i(dr, cr);
          IntSetVal* newibv = IntSetVal::ai(i);
          id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
          id->decl()->ti()->setComputedDomain(false);
        } else {
          id->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(lb, ub)));
        }
        return false;
      }
    }
  }
  return true;
}

// Computer a new domain for the given TypeInst given an expression being assigned to it.
// Note that this function enforces tighter domains on the expression when possible.
KeepAlive compute_combined_domain(EnvI& env, TypeInst* ti, Expression* cur) {
  if (Expression::type(cur).dim() > 0) {
    if (ti->domain() != nullptr) {
      auto* al = Expression::dynamicCast<ArrayLit>(
          Expression::isa<Id>(cur) ? Expression::cast<Id>(cur)->decl()->e() : cur);
      if (al != nullptr) {
        std::vector<Expression*> ndom(0);
        for (unsigned int i = 0; i < al->size(); i++) {
          // TODO: Currently ignores setting the union of the RHS, should we compute the union and
          // set?
          compute_combined_domain(env, ti, (*al)[i]);
        }
        ti->setComputedDomain(true);
      }
    }
    return nullptr;
  }
  if (Expression::type(cur).structBT()) {
    auto* tl = Expression::dynamicCast<ArrayLit>(
        Expression::isa<Id>(cur) ? Expression::cast<Id>(cur)->decl()->e() : cur);
    if (tl != nullptr) {
      auto* tis = Expression::cast<ArrayLit>(ti->domain());
      std::vector<KeepAlive> dom_expr(tis->size());
      bool any_changed = false;
      for (unsigned int i = 0; i < tis->size(); i++) {
        dom_expr[i] = compute_combined_domain(env, Expression::cast<TypeInst>((*tis)[i]), (*tl)[i]);
        any_changed = any_changed || dom_expr[i]() != nullptr;
      }
      if (any_changed) {
        GCLock lock;
        std::vector<Expression*> dom(tis->size());
        for (unsigned int i = 0; i < tis->size(); i++) {
          auto* ti = Expression::cast<TypeInst>((*tis)[i]);
          dom[i] = new TypeInst(Location().introduce(), ti->type(),
                                dom_expr[i]() == nullptr ? ti->domain() : dom_expr[i]());
        }
        return new ArrayLit(Location().introduce(), dom);
      }
    }
    return nullptr;
  }
  Id* ident = Expression::dynamicCast<Id>(cur);
  if (ident != nullptr &&
      (ident == env.constants.absent || (ident->type().isAnn() && ident->decl() == nullptr))) {
    // no need to do anything
    return nullptr;
  }
  if (ident != nullptr && ident->decl()->ti()->domain() == nullptr) {
    if (ti->domain() != nullptr) {
      // RHS has no domain, so take ours
      GCLock lock;
      Expression* vd_dom = eval_par(env, ti->domain());
      set_computed_domain(env, ident->decl(), vd_dom, ident->decl()->ti()->computedDomain());
    }
    return nullptr;
  }
  if (cur != nullptr && Expression::type(cur).bt() == Type::BT_INT) {
    GCLock lock;
    IntSetVal* rhsBounds = nullptr;
    if (Expression::type(cur).isSet()) {
      rhsBounds = compute_intset_bounds(env, cur);
    } else {
      IntBounds ib = compute_int_bounds(env, cur);
      if (ib.valid) {
        Call* call = Expression::dynamicCast<Call>(cur);
        if ((call != nullptr) && call->id() == env.constants.ids.lin_exp) {
          ArrayLit* al = eval_array_lit(env, call->arg(1));
          if (al->size() == 1) {
            IntBounds check_zeroone = compute_int_bounds(env, (*al)[0]);
            if (check_zeroone.l == 0 && check_zeroone.u == 1) {
              ArrayLit* coeffs = eval_array_lit(env, call->arg(0));
              auto c = eval_int(env, (*coeffs)[0]);
              auto d = eval_int(env, call->arg(2));
              std::vector<IntVal> newdom(2);
              newdom[0] = std::min(c + d, d);
              newdom[1] = std::max(c + d, d);
              rhsBounds = IntSetVal::a(newdom);
            }
          }
        }
        if (rhsBounds == nullptr) {
          rhsBounds = IntSetVal::a(ib.l, ib.u);
        }
      }
    }
    IntSetVal* combinedDomain = rhsBounds;
    IntSetVal* rhsDomain = nullptr;
    if (ident != nullptr && ident->decl()->ti()->domain() != nullptr &&
        !Expression::isa<TIId>(ident->decl()->ti()->domain())) {
      // RHS has a domain, so intersect with its bounds
      rhsDomain = eval_intset(env, ident->decl()->ti()->domain());
      if (rhsBounds == nullptr) {
        combinedDomain = rhsDomain;
      } else {
        IntSetRanges dr(rhsDomain);
        IntSetRanges ibr(rhsBounds);
        Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> i(dr, ibr);
        combinedDomain = IntSetVal::ai(i);
        if (combinedDomain->card() == 0 && !Expression::type(cur).isSet()) {
          env.fail();
        }
      }
    }
    if (combinedDomain != nullptr) {
      IntSetVal* domain = nullptr;
      if (ti->domain() != nullptr && !Expression::isa<TIId>(ti->domain())) {
        // LHS has a domain so intersect with RHS domain
        domain = eval_intset(env, ti->domain());
        IntSetRanges dr(domain);
        IntSetRanges ibr(combinedDomain);
        Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> i(dr, ibr);
        combinedDomain = IntSetVal::ai(i);
        if (combinedDomain->card() == 0 && !Expression::type(cur).isSet() &&
            !Expression::type(cur).isOpt()) {
          env.fail();
        }
      }
      if (ident != nullptr && (rhsDomain == nullptr || !combinedDomain->equal(rhsDomain))) {
        // If the RHS is an identifier, make its domain match
        set_computed_domain(env, ident->decl(), new SetLit(Location().introduce(), combinedDomain),
                            false);
      }
      if (domain == nullptr || !domain->equal(combinedDomain)) {
        return new SetLit(Location().introduce(), combinedDomain);
      }
    }
    return nullptr;
  }
  if (cur != nullptr && Expression::type(cur).bt() == Type::BT_FLOAT) {
    GCLock lock;
    FloatSetVal* rhsBounds = nullptr;
    FloatBounds fb = compute_float_bounds(env, cur);
    if (fb.valid) {
      rhsBounds = FloatSetVal::a(fb.l, fb.u);
    }
    FloatSetVal* combinedDomain = rhsBounds;
    FloatSetVal* rhsDomain = nullptr;
    if (ident != nullptr && ident->decl()->ti()->domain() != nullptr &&
        !Expression::isa<TIId>(ident->decl()->ti()->domain())) {
      // RHS has a domain, so intersect with its bounds
      rhsDomain = eval_floatset(env, ident->decl()->ti()->domain());
      if (rhsBounds == nullptr) {
        combinedDomain = rhsDomain;
      } else {
        FloatSetRanges dr(rhsDomain);
        FloatSetRanges ibr(rhsBounds);
        Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> i(dr, ibr);
        combinedDomain = FloatSetVal::ai(i);
        if (combinedDomain->empty()) {
          env.fail();
        }
      }
    }
    if (combinedDomain != nullptr) {
      FloatSetVal* domain = nullptr;
      if (ti->domain() != nullptr && !Expression::isa<TIId>(ti->domain())) {
        // LHS has a domain so intersect with RHS domain
        domain = eval_floatset(env, ti->domain());
        FloatSetRanges dr(domain);
        FloatSetRanges fbr(combinedDomain);
        Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> i(dr, fbr);
        combinedDomain = FloatSetVal::ai(i);
        if (combinedDomain->empty()) {
          env.fail();
        }
      }
      if (ident != nullptr && (rhsDomain == nullptr || !combinedDomain->equal(rhsDomain))) {
        // If the RHS is an identifier, make its domain match
        set_computed_domain(env, ident->decl(), new SetLit(Location().introduce(), combinedDomain),
                            false);
      }
      if (domain == nullptr || !domain->equal(combinedDomain)) {
        return new SetLit(Location().introduce(), combinedDomain);
      }
    }
    return nullptr;
  }
  return nullptr;
}

KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e) {
  assert(e == nullptr || !Expression::isa<VarDecl>(e));
  if (vd == env.constants.varIgnore) {
    return e;
  }
  if (Id* ident = Expression::dynamicCast<Id>(e)) {
    if (ident->decl() != nullptr) {
      e = follow_id_to_decl(ident);
      if (auto* e_vd = Expression::dynamicCast<VarDecl>(e)) {
        e = e_vd->id();
        if (!env.inReverseMapVar && ctx.b != C_ROOT && Expression::type(e) == Type::varbool()) {
          env.addCtxAnn(e_vd, ctx.b);
          if (e_vd != ident->decl()) {
            env.addCtxAnn(ident->decl(), ctx.b);
          }
        }
      }
    }
  }
  if (ctx.neg) {
    assert(Expression::type(e).isbool());
    auto wrap_bool_not = [&](Expression* e) {
      KeepAlive ka;
      {
        GCLock lock;
        Call* bn = Call::a(Expression::loc(e), env.constants.ids.bool_.not_, {e});
        bn->type(Expression::type(e));
        bn->decl(env.model->matchFn(env, bn, false));
        ka = bn;
      }
      return flat_exp(env, Ctx(), ka(), vd, env.constants.varTrue);
    };
    if (vd == env.constants.varTrue) {
      if (!isfalse(env, e)) {
        if (Id* id = Expression::dynamicCast<Id>(e)) {
          while (id != nullptr) {
            assert(id->decl() != nullptr);
            if ((id->decl()->ti()->domain() != nullptr) &&
                istrue(env, id->decl()->ti()->domain())) {
              GCLock lock;
              env.flatAddItem(new ConstraintI(Location().introduce(), env.constants.literalFalse));
            } else {
              GCLock lock;
              std::vector<Expression*> args(2);
              args[0] = id;
              args[1] = env.constants.literalFalse;
              Call* c = Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
              c->decl(env.model->matchFn(env, c, false));
              c->type(c->decl()->rtype(env, args, nullptr, false));
              if (c->decl()->e() != nullptr) {
                flat_exp(env, Ctx(), c, env.constants.varTrue, env.constants.varTrue);
              }
              set_computed_domain(env, id->decl(), env.constants.literalFalse,
                                  id->decl()->ti()->computedDomain());
            }
            id =
                id->decl()->e() != nullptr ? Expression::dynamicCast<Id>(id->decl()->e()) : nullptr;
          }
          return env.constants.literalTrue;
        }
        EE ee = wrap_bool_not(e);
        return ee.r;
      }
      return env.constants.literalTrue;
    }
    EE ee = wrap_bool_not(e);
    return ee.r;
  }
  if (vd == env.constants.varTrue) {
    if (!istrue(env, e)) {
      if (Id* id = Expression::dynamicCast<Id>(e)) {
        assert(id->decl() != nullptr);
        while (id != nullptr) {
          if ((id->decl()->ti()->domain() != nullptr) && isfalse(env, id->decl()->ti()->domain())) {
            GCLock lock;
            env.flatAddItem(new ConstraintI(Location().introduce(), env.constants.literalFalse));
          } else if (id->decl()->ti()->domain() == nullptr) {
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = id;
            args[1] = env.constants.literalTrue;
            Call* c = Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
            c->decl(env.model->matchFn(env, c, false));
            c->type(c->decl()->rtype(env, args, nullptr, false));
            if (c->decl()->e() != nullptr) {
              flat_exp(env, Ctx(), c, env.constants.varTrue, env.constants.varTrue);
            }
            set_computed_domain(env, id->decl(), env.constants.literalTrue,
                                id->decl()->ti()->computedDomain());
          }
          id = id->decl()->e() != nullptr ? Expression::dynamicCast<Id>(id->decl()->e()) : nullptr;
        }
      } else {
        GCLock lock;
        // extract domain information from added constraint if possible
        if (!Expression::isa<Call>(e) || check_domain_constraints(env, Expression::cast<Call>(e))) {
          env.flatAddItem(new ConstraintI(Location().introduce(), e));
        }
      }
    }
    return env.constants.literalTrue;
  }
  if (vd == env.constants.varFalse) {
    if (!isfalse(env, e)) {
      throw InternalError("not supported yet");
    }
    return env.constants.literalTrue;
  }
  if (vd == nullptr) {
    if (e == nullptr) {
      return nullptr;
    }
    switch (Expression::eid(e)) {
      case Expression::E_INTLIT:
      case Expression::E_FLOATLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_ANON:
      case Expression::E_ID:
      case Expression::E_TIID:
      case Expression::E_SETLIT:
      case Expression::E_VARDECL:
      case Expression::E_BINOP:  // TODO: should not happen once operators are evaluated
      case Expression::E_UNOP:   // TODO: should not happen once operators are evaluated
        return e;
      case Expression::E_ARRAYACCESS:
      case Expression::E_COMP:
      case Expression::E_ITE:
      case Expression::E_LET:
      case Expression::E_TI:
        throw InternalError("unevaluated expression");
      case Expression::E_ARRAYLIT: {
        GCLock lock;
        auto* al = Expression::cast<ArrayLit>(e);
        /// TODO: review if limit of 10 is a sensible choice
        if (al->type().isbot() || al->type().bt() == Type::BT_ANN || al->size() <= 10) {
          return e;
        }

        auto it = env.cseMapFind(al);
        if (it != env.cseMapEnd()) {
          return Expression::cast<VarDecl>(it->second.r)->id();
        }

        std::vector<TypeInst*> ranges(al->dims());
        for (unsigned int i = 0; i < ranges.size(); i++) {
          ranges[i] = new TypeInst(
              Expression::loc(e), Type(),
              new SetLit(Location().introduce(), IntSetVal::a(al->min(i), al->max(i))));
        }
        ASTExprVec<TypeInst> ranges_v(ranges);
        Expression* domain = nullptr;
        if (!al->empty() && Expression::type((*al)[0]).isint()) {
          IntVal min = IntVal::infinity();
          IntVal max = -IntVal::infinity();
          for (unsigned int i = 0; i < al->size(); i++) {
            IntBounds ib = compute_int_bounds(env, (*al)[i]);
            if (!ib.valid) {
              min = -IntVal::infinity();
              max = IntVal::infinity();
              break;
            }
            min = std::min(min, ib.l);
            max = std::max(max, ib.u);
          }
          if (min != -IntVal::infinity() && max != IntVal::infinity()) {
            domain = new SetLit(Location().introduce(), IntSetVal::a(min, max));
          }
        }
        auto* ti = new TypeInst(Expression::loc(e), al->type(), ranges_v, domain);
        if (domain != nullptr) {
          ti->setComputedDomain(true);
        }

        VarDecl* nvd = new_vardecl(env, ctx, ti, nullptr, nullptr, al);
        EE ee(nvd, nullptr);
        env.cseMapInsert(al, ee);
        if (al != nvd->e()) {
          env.cseMapInsert(nvd->e(), ee);
        }
        return nvd->id();
      }
      case Expression::E_CALL: {
        if (Expression::type(e).isAnn()) {
          return e;
        }
        GCLock lock;
        /// TODO: handle array types
        auto* ti = new TypeInst(Location().introduce(), Expression::type(e));
        VarDecl* nvd = new_vardecl(env, ctx, ti, nullptr, nullptr, e);
        if (Expression::type(nvd->e()).bt() == Type::BT_INT &&
            Expression::type(nvd->e()).dim() == 0) {
          IntSetVal* ibv = nullptr;
          if (Expression::type(nvd->e()).isSet()) {
            ibv = compute_intset_bounds(env, nvd->e());
          } else {
            IntBounds ib = compute_int_bounds(env, nvd->e());
            if (ib.valid) {
              ibv = IntSetVal::a(ib.l, ib.u);
            }
          }
          if (ibv != nullptr) {
            Id* id = nvd->id();
            while (id != nullptr) {
              bool is_computed = id->decl()->ti()->computedDomain();
              if (id->decl()->ti()->domain() != nullptr) {
                IntSetVal* domain = eval_intset(env, id->decl()->ti()->domain());
                IntSetRanges dr(domain);
                IntSetRanges ibr(ibv);
                Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> i(dr, ibr);
                IntSetVal* newibv = IntSetVal::ai(i);
                if (ibv->equal(newibv)) {
                  is_computed = true;
                } else {
                  ibv = newibv;
                }
              } else {
                is_computed = true;
              }
              if (id->type().st() == Type::ST_PLAIN && ibv->empty()) {
                env.fail();
              } else {
                set_computed_domain(env, id->decl(), new SetLit(Location().introduce(), ibv),
                                    is_computed);
              }
              id = Expression::dynamicCast<Id>(id->decl()->e());
            }
          }
        } else if (Expression::type(nvd->e()).isbool()) {
          env.addCtxAnn(nvd, ctx.b);
        } else if (Expression::type(nvd->e()).bt() == Type::BT_FLOAT &&
                   Expression::type(nvd->e()).dim() == 0) {
          FloatBounds fb = compute_float_bounds(env, nvd->e());
          FloatSetVal* ibv = LinearTraits<FloatLit>::intersectDomain(nullptr, fb.l, fb.u);
          if (fb.valid) {
            Id* id = nvd->id();
            while (id != nullptr) {
              bool is_computed = id->decl()->ti()->computedDomain();
              if (id->decl()->ti()->domain() != nullptr) {
                FloatSetVal* domain = eval_floatset(env, id->decl()->ti()->domain());
                FloatSetVal* ndomain = LinearTraits<FloatLit>::intersectDomain(domain, fb.l, fb.u);
                if ((ibv != nullptr) && ndomain == domain) {
                  is_computed = true;
                } else {
                  ibv = ndomain;
                }
              } else {
                is_computed = true;
              }
              if (LinearTraits<FloatLit>::domainEmpty(ibv)) {
                env.fail();
              } else {
                set_computed_domain(env, id->decl(), new SetLit(Location().introduce(), ibv),
                                    is_computed);
              }
              id = Expression::dynamicCast<Id>(id->decl()->e());
            }
          }
        }

        return nvd->id();
      }
      default:
        assert(false);
        return nullptr;
    }
  } else {
    if (vd->e() == nullptr) {
      Expression* ret = e;
      if (e == nullptr || (Expression::type(e).isPar() && Expression::type(e).isbool())) {
        GCLock lock;
        bool isTrue = (e == nullptr || eval_bool(env, e));

        // Check if redefinition of bool_eq exists, if yes call it
        std::vector<Expression*> args(2);
        args[0] = vd->id();
        args[1] = env.constants.boollit(isTrue);
        Call* c = Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
        c->decl(env.model->matchFn(env, c, false));
        c->type(c->decl()->rtype(env, args, nullptr, false));
        bool didRewrite = false;
        if (c->decl()->e() != nullptr) {
          flat_exp(env, Ctx(), c, env.constants.varTrue, env.constants.varTrue);
          didRewrite = true;
        }

        vd->e(env.constants.boollit(isTrue));
        if (vd->ti()->domain() != nullptr) {
          if (vd->ti()->domain() != vd->e()) {
            env.fail();
            return vd->id();
          }
        } else {
          set_computed_domain(env, vd, vd->e(), true);
        }
        if (didRewrite) {
          return vd->id();
        }
      } else {
        check_index_sets(env, vd, e);

        KeepAlive combinedDom = compute_combined_domain(env, vd->ti(), e);
        if (combinedDom() != nullptr) {
          set_computed_domain(env, vd, combinedDom(), vd->ti()->domain() == nullptr);
        }

        if (env.hasReverseMapper(vd->id()) && Expression::type(e).dim() == 0 &&
            Expression::isa<Id>(e) && Expression::cast<Id>(e) != vd->id()) {
          // Reverse mapped variable aliasing:
          // E.g for optional variables, we can constrain the deopt values and the occurs values
          // to match rather than only equating the deopts when they both occur.
          GCLock lock;
          auto* alias =
              Call::a(Location().introduce(), env.constants.ids.mzn_alias_eq, {vd->id(), e});
          auto* decl = env.model->matchFn(env, alias, false);
          if (decl != nullptr) {
            alias->decl(decl);
            alias->type(Type::varbool());
            if (ctx.b != C_ROOT) {
              env.addCtxAnn(vd, ctx.b);
              env.addCtxAnn(Expression::cast<Id>(e)->decl(), ctx.b);
            }
            flat_exp(env, Ctx(), alias, env.constants.varTrue, env.constants.varTrue);
            ret = vd->id();
            vd->e(e);
            env.voAddExp(vd);
          }
        }

        if (ret != vd->id()) {
          vd->e(ret);
          add_path_annotation(env, ret);
          env.voAddExp(vd);
          ret = vd->id();
        }
      }
      return ret;
    }
    if (vd == e) {
      return vd->id();
    }
    if (vd->e() != e) {
      e = follow_id_to_decl(e);
      if (vd == e) {
        return vd->id();
      }
      switch (Expression::eid(e)) {
        case Expression::E_BOOLLIT: {
          Id* id = vd->id();
          while (id != nullptr) {
            if ((id->decl()->ti()->domain() != nullptr) &&
                eval_bool(env, id->decl()->ti()->domain()) == Expression::cast<BoolLit>(e)->v()) {
              return env.constants.literalTrue;
            }
            if ((id->decl()->ti()->domain() != nullptr) &&
                eval_bool(env, id->decl()->ti()->domain()) != Expression::cast<BoolLit>(e)->v()) {
              env.fail("domain is empty");
            }
            {
              GCLock lock;
              std::vector<Expression*> args(2);
              args[0] = id;
              args[1] = e;
              Call* c = Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
              c->decl(env.model->matchFn(env, c, false));
              c->type(c->decl()->rtype(env, args, nullptr, false));
              if (c->decl()->e() != nullptr) {
                flat_exp(env, Ctx(), c, env.constants.varTrue, env.constants.varTrue);
              }
              set_computed_domain(env, id->decl(), e, id->decl()->ti()->computedDomain());
            }
            id =
                id->decl()->e() != nullptr ? Expression::dynamicCast<Id>(id->decl()->e()) : nullptr;
          }
          return env.constants.literalTrue;
        }
        case Expression::E_INTLIT: {
          Id* id = vd->id();
          while (id != nullptr) {
            if (id->decl()->ti()->domain() != nullptr) {
              GCLock lock;
              IntSetVal* dom = eval_set_lit(env, id->decl()->ti()->domain())->isv();
              IntVal v = IntLit::v(Expression::cast<IntLit>(e));
              if (!dom->contains(v)) {
                env.fail("domain is empty");
              }
              if (dom->min(0) == v && dom->max(dom->size() - 1) == v) {
                return e;
              }
            }
            {
              GCLock lock;
              std::vector<Expression*> args(2);
              args[0] = id;
              args[1] = e;
              Call* c = Call::a(Location().introduce(), env.constants.ids.int_.eq, args);
              c->decl(env.model->matchFn(env, c, false));
              c->type(c->decl()->rtype(env, args, nullptr, false));
              if (c->decl()->e() != nullptr) {
                flat_exp(env, Ctx(), c, env.constants.varTrue, env.constants.varTrue);
              }
              set_computed_domain(env, id->decl(), e, id->decl()->ti()->computedDomain());
            }
            id =
                id->decl()->e() != nullptr ? Expression::dynamicCast<Id>(id->decl()->e()) : nullptr;
          }
          return e;
        }
        case Expression::E_FLOATLIT: {
          Id* id = vd->id();
          while (id != nullptr) {
            if (id->decl()->ti()->domain() != nullptr) {
              GCLock lock;
              FloatSetVal* dom = eval_set_lit(env, id->decl()->ti()->domain())->fsv();
              FloatVal v = FloatLit::v(Expression::cast<FloatLit>(e));
              if (!dom->contains(v)) {
                env.fail("domain is empty");
              }
              if (dom->min(0) == v && dom->max(dom->size() - 1) == v) {
                return e;
              }
            }
            {
              GCLock lock;
              std::vector<Expression*> args(2);
              args[0] = id;
              args[1] = e;
              Call* c = Call::a(Location().introduce(), env.constants.ids.float_.eq, args);
              c->decl(env.model->matchFn(env, c, false));
              c->type(c->decl()->rtype(env, args, nullptr, false));
              if (c->decl()->e() != nullptr) {
                flat_exp(env, Ctx(), c, env.constants.varTrue, env.constants.varTrue);
              }
              set_computed_domain(env, id->decl(), e, id->decl()->ti()->computedDomain());
            }
            id =
                id->decl()->e() != nullptr ? Expression::dynamicCast<Id>(id->decl()->e()) : nullptr;
          }
          return e;
        }
        case Expression::E_VARDECL: {
          auto* e_vd = Expression::cast<VarDecl>(e);
          if (vd->e() == e_vd->id() || e_vd->e() == vd->id()) {
            return vd->id();
          }
          if (Expression::type(e).dim() != 0) {
            throw InternalError("not supported yet");
          }
          create_explicit_assignment_constraints(env, vd, e_vd->id());
          return vd->id();
        }
        case Expression::E_CALL: {
          Call* c = Expression::cast<Call>(e);
          GCLock lock;
          Call* nc;
          std::vector<Expression*> args;
          if (c->id() == env.constants.ids.lin_exp) {
            auto* le_c = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
            std::vector<Expression*> ncoeff(le_c->size());
            for (auto i = static_cast<unsigned int>(ncoeff.size()); (i--) != 0U;) {
              ncoeff[i] = (*le_c)[i];
            }
            ncoeff.push_back(IntLit::a(-1));
            args.push_back(new ArrayLit(Location().introduce(), ncoeff));
            Expression::type(args[0], le_c->type());
            auto* le_x = Expression::cast<ArrayLit>(follow_id(c->arg(1)));
            std::vector<Expression*> nx(le_x->size());
            for (auto i = static_cast<unsigned int>(nx.size()); (i--) != 0U;) {
              nx[i] = (*le_x)[i];
            }
            nx.push_back(vd->id());
            args.push_back(new ArrayLit(Location().introduce(), nx));
            Expression::type(args[1], le_x->type());
            args.push_back(c->arg(2));
            nc = Call::a(Expression::loc(c).introduce(), env.constants.ids.lin_exp, args);
            nc->decl(env.model->matchFn(env, nc, false));
            if (nc->decl() == nullptr) {
              std::ostringstream ss;
              ss << "undeclared function or predicate " << nc->id();
              throw InternalError(ss.str());
            }
            nc->type(nc->decl()->rtype(env, args, nullptr, false));
            auto* bop = new BinOp(Expression::loc(nc), nc, BOT_EQ, IntLit::a(0));
            bop->type(Type::varbool());
            flat_exp(env, Ctx(), bop, env.constants.varTrue, env.constants.varTrue);
            return vd->id();
          }
          args.resize(c->argCount());
          for (auto i = static_cast<unsigned int>(args.size()); (i--) != 0U;) {
            args[i] = c->arg(i);
          }
          if (c->id() == env.constants.ids.exists) {
            args.push_back(env.constants.emptyBoolArray);
          }
          args.push_back(vd->id());

          ASTString nid = c->id();
          FunctionI* nc_decl(nullptr);
          if (c->id() == env.constants.ids.exists) {
            nid = env.constants.ids.bool_reif.clause;
          } else if (c->id() == env.constants.ids.forall) {
            nid = env.constants.ids.bool_reif.array_and;
          } else if (vd->type().isbool()) {
            bool canHalfReify = env.fopts.enableHalfReification &&
                                Expression::ann(vd).contains(env.constants.ctx.pos);
            nc_decl = env.model->matchReification(env, c->id(), args, canHalfReify, false);
            nid = (nc_decl != nullptr) ? nc_decl->id() : env.reifyId(c->id());
          }
          nc = Call::a(Expression::loc(c).introduce(), nid, args);
          if (nc_decl == nullptr) {
            nc_decl = env.model->matchFn(env, nc, false);
          }
          if (nc_decl == nullptr) {
            std::ostringstream ss;
            ss << "undeclared function or predicate " << nc->id();
            throw InternalError(ss.str());
          }
          nc->decl(nc_decl);
          nc->type(nc->decl()->rtype(env, args, nullptr, false));
          // vd already had a RHS, so don't make it a defined var
          // make_defined_var(env, vd, nc);
          flat_exp(env, Ctx(), nc, env.constants.varTrue, env.constants.varTrue);
          return vd->id();
        } break;
        case Expression::E_SETLIT: {
          Id* id = vd->id();
          auto* sl = Expression::cast<SetLit>(e);
          while (id != nullptr) {
            if (auto* rhs = Expression::dynamicCast<SetLit>(id->decl()->e())) {
              if (id->type().isIntSet() ? sl->isv()->equal(rhs->isv())
                                        : sl->fsv()->equal(rhs->fsv())) {
                return sl;
              }
              env.fail("domain is empty");
            }
            if (id->decl()->ti()->domain() != nullptr) {
              GCLock lock;
              SetLit* dom = eval_set_lit(env, id->decl()->ti()->domain());
              if (id->type().isIntSet()) {
                IntSetRanges slr(sl->isv());
                IntSetRanges domr(dom->isv());
                if (!Ranges::subset(slr, domr)) {
                  env.fail("domain is empty");
                }
              } else {
                FloatSetRanges slr(sl->fsv());
                FloatSetRanges domr(dom->fsv());
                if (!Ranges::subset(slr, domr)) {
                  env.fail("domain is empty");
                }
              }
            }
            Id* next =
                id->decl()->e() != nullptr ? Expression::dynamicCast<Id>(id->decl()->e()) : nullptr;
            if (auto* _rhs = Expression::dynamicCast<Id>(id->decl()->e())) {
              id->decl()->e(nullptr);
            }
            // There is already a RHS. If it's a call, make it relational.
            // Otherwise, create explicit constraint
            bool made_relational = false;
            if (id->decl()->e() != nullptr) {
              if (auto* c = Expression::dynamicCast<Call>(id->decl()->e())) {
                KeepAlive ka;
                // Remove call from RHS and add it as new constraint with the literal
                std::vector<Expression*> args(c->argCount() + 1);
                for (unsigned int i = 0; i < c->argCount(); ++i) {
                  args[i] = c->arg(i);
                }
                args[c->argCount()] = id->decl()->id();
                FunctionI* decl = nullptr;
                {
                  GCLock lock;
                  auto* nc = Call::a(Expression::loc(c), c->id(), args);
                  nc->type(Type::varbool());
                  decl = env.model->matchFn(env, nc, false);
                  ka = nc;
                }
                if (decl != nullptr) {
                  made_relational = true;
                  Expression::cast<Call>(ka())->decl(decl);
                  flat_exp(env, Ctx(), ka(), env.constants.varTrue, env.constants.varTrue);
                }
              }
            }
            if (id->decl()->e() == nullptr || made_relational) {
              GCLock lock;
              Type nty = vd->type();
              nty.mkPar(env);
              id->decl()->ti(new TypeInst(Expression::loc(vd), nty));
              id->decl()->e(sl);
              id->decl()->type(nty);
            } else {
              create_explicit_assignment_constraints(env, id->decl(), sl);
            }
            id = next;
          }
          return sl;
        }
        default:
          throw InternalError("not supported yet");
      }
    } else {
      return e;
    }
  }
}

KeepAlive conj(EnvI& env, VarDecl* b, const Ctx& ctx, const std::vector<EE>& e) {
  if (!ctx.neg) {
    std::vector<Expression*> nontrue;
    for (const auto& i : e) {
      if (istrue(env, i.b())) {
        continue;
      }
      if (isfalse(env, i.b())) {
        return bind(env, Ctx(), b, env.constants.literalFalse);
      }
      nontrue.push_back(i.b());
    }
    if (nontrue.empty()) {
      return bind(env, Ctx(), b, env.constants.literalTrue);
    }
    if (nontrue.size() == 1) {
      return bind(env, ctx, b, nontrue[0]);
    }
    if (b == env.constants.varTrue) {
      for (auto& i : nontrue) {
        bind(env, ctx, b, i);
      }
      return env.constants.literalTrue;
    }
    GC::lock();
    std::vector<Expression*> args;
    auto* al = new ArrayLit(Location().introduce(), nontrue);
    al->type(Type::varbool(1));
    args.push_back(al);
    Call* ret = Call::a(Expression::loc(nontrue[0]).introduce(), env.constants.ids.forall, args);
    ret->decl(env.model->matchFn(env, ret, false));
    ret->type(ret->decl()->rtype(env, args, nullptr, false));
    KeepAlive ka(ret);
    GC::unlock();
    return flat_exp(env, ctx, ret, b, env.constants.varTrue).r;
  }
  Ctx nctx = ctx;
  nctx.neg = false;
  nctx.b = -nctx.b;
  // negated
  std::vector<Expression*> nonfalse;
  for (const auto& i : e) {
    if (istrue(env, i.b())) {
      continue;
    }
    if (isfalse(env, i.b())) {
      return bind(env, Ctx(), b, env.constants.literalTrue);
    }
    nonfalse.push_back(i.b());
  }
  if (nonfalse.empty()) {
    return bind(env, Ctx(), b, env.constants.literalFalse);
  }
  if (nonfalse.size() == 1) {
    GC::lock();
    UnOp* uo = new UnOp(Expression::loc(nonfalse[0]), UOT_NOT, nonfalse[0]);
    uo->type(Type::varbool());
    KeepAlive ka(uo);
    GC::unlock();
    return flat_exp(env, nctx, uo, b, env.constants.varTrue).r;
  }
  if (b == env.constants.varFalse) {
    for (auto& i : nonfalse) {
      bind(env, nctx, b, i);
    }
    return env.constants.literalFalse;
  }
  GC::lock();
  std::vector<Expression*> args;
  for (auto& i : nonfalse) {
    UnOp* uo = new UnOp(Expression::loc(i), UOT_NOT, i);
    uo->type(Type::varbool());
    i = uo;
  }
  auto* al = new ArrayLit(Location().introduce(), nonfalse);
  al->type(Type::varbool(1));
  args.push_back(al);
  Call* ret = Call::a(Location().introduce(), env.constants.ids.exists, args);
  ret->decl(env.model->matchFn(env, ret, false));
  ret->type(ret->decl()->rtype(env, args, nullptr, false));
  assert(ret->decl());
  KeepAlive ka(ret);
  GC::unlock();
  return flat_exp(env, nctx, ret, b, env.constants.varTrue).r;
}

Expression* eval_typeinst_domain(EnvI& env, const Ctx& ctx, Expression* dom) {
  if (dom == nullptr) {
    return nullptr;
  }
  if (auto* tup = Expression::dynamicCast<ArrayLit>(dom)) {
    std::vector<Expression*> evaluated(tup->size());
    for (unsigned int i = 0; i < tup->size(); ++i) {
      auto* tupi = Expression::cast<TypeInst>((*tup)[i]);
      if (tupi->domain() == nullptr) {
        evaluated[i] = new TypeInst(Expression::loc(tupi), tupi->type(), tupi->ranges());
      } else if (Expression::isa<ArrayLit>(tupi->domain())) {
        evaluated[i] = new TypeInst(Expression::loc(tupi), tupi->type(), tupi->ranges(),
                                    eval_typeinst_domain(env, ctx, tupi->domain()));
      } else {
        evaluated[i] = new TypeInst(Expression::loc(tupi), tupi->type(), tupi->ranges(),
                                    flat_cv_exp(env, ctx, tupi->domain())());
      }
    }
    return ArrayLit::constructTuple(Expression::loc(tup), evaluated);
  }
  return flat_cv_exp(env, ctx, dom)();
}

TypeInst* eval_typeinst(EnvI& env, const Ctx& ctx, VarDecl* vd) {
  bool domIsTiVar = (vd->ti()->domain() != nullptr) && Expression::isa<TIId>(vd->ti()->domain());
  bool hasTiVars = domIsTiVar;
  for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
    hasTiVars = hasTiVars || ((vd->ti()->ranges()[i]->domain() != nullptr) &&
                              Expression::isa<TIId>(vd->ti()->ranges()[i]->domain()));
  }
  if (hasTiVars) {
    assert(vd->e());
    if (Expression::type(vd->e()).dim() == 0) {
      return new TypeInst(Location().introduce(), Expression::type(vd->e()));
    }
    ArrayLit* al = eval_array_lit(env, vd->e());
    std::vector<TypeInst*> dims(al->dims());
    for (unsigned int i = 0; i < dims.size(); i++) {
      dims[i] =
          new TypeInst(Location().introduce(), Type(),
                       new SetLit(Location().introduce(), IntSetVal::a(al->min(i), al->max(i))));
    }
    return new TypeInst(Location().introduce(), Expression::type(vd->e()), dims,
                        domIsTiVar ? nullptr : flat_cv_exp(env, ctx, vd->ti()->domain())());
  }
  std::vector<TypeInst*> dims(vd->ti()->ranges().size());
  for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
    if (vd->ti()->ranges()[i]->domain() != nullptr) {
      KeepAlive range = flat_cv_exp(env, ctx, vd->ti()->ranges()[i]->domain());
      IntSetVal* isv = eval_intset(env, range());
      if (isv->size() > 1) {
        throw EvalError(env, Expression::loc(vd->ti()->ranges()[i]->domain()),
                        "array index set must be contiguous range");
      }
      auto* sl = new SetLit(Expression::loc(vd->ti()->ranges()[i]), isv);
      sl->type(Type::parsetint());
      dims[i] = new TypeInst(Expression::loc(vd->ti()->ranges()[i]), Type(), sl);
    } else {
      dims[i] = new TypeInst(Expression::loc(vd->ti()->ranges()[i]), Type(), nullptr);
    }
  }
  Type t = ((vd->e() != nullptr) && !Expression::type(vd->e()).isbot()) ? Expression::type(vd->e())
                                                                        : vd->ti()->type();
  return new TypeInst(Expression::loc(vd->ti()), t, dims,
                      eval_typeinst_domain(env, ctx, vd->ti()->domain()));
}

KeepAlive flat_cv_exp(EnvI& env, Ctx ctx, Expression* e) {
  if (e == nullptr) {
    return nullptr;
  }
  GCLock lock;
  if (Expression::type(e).isPar() && !Expression::type(e).cv()) {
    if (Expression::type(e) == Type::parbool()) {
      try {
        return eval_par(env, e);
      } catch (ResultUndefinedError&) {
        return env.constants.boollit(false);
      }
    }
    return eval_par(env, e);
  }
  if (Expression::type(e).isvar()) {
    EE ee = flat_exp(env, ctx, e, nullptr, ctx.partialityVar(env));
    if (isfalse(env, ee.b())) {
      throw ResultUndefinedError(env, Expression::loc(e), "");
    }
    return ee.r();
  }
  try {
    switch (Expression::eid(e)) {
      case Expression::E_INTLIT:
      case Expression::E_FLOATLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_TIID:
      case Expression::E_VARDECL:
      case Expression::E_TI:
      case Expression::E_ANON:
        assert(false);
        return nullptr;
      case Expression::E_ID: {
        Id* id = Expression::cast<Id>(e);
        return flat_cv_exp(env, ctx, id->decl()->e());
      }
      case Expression::E_SETLIT: {
        auto* sl = Expression::cast<SetLit>(e);
        if ((sl->isv() != nullptr) || (sl->fsv() != nullptr)) {
          return sl;
        }
        std::vector<Expression*> es(sl->v().size());
        GCLock lock;
        for (unsigned int i = 0; i < sl->v().size(); i++) {
          es[i] = flat_cv_exp(env, ctx, sl->v()[i])();
        }
        auto* sl_ret = new SetLit(Location().introduce(), es);
        Type t = sl->type();
        t.cv(false);
        sl_ret->type(t);
        return eval_par(env, sl_ret);
      }
      case Expression::E_ARRAYLIT: {
        auto* al = Expression::cast<ArrayLit>(e);
        std::vector<Expression*> es(al->size());
        GCLock lock;
        for (unsigned int i = 0; i < al->size(); i++) {
          es[i] = flat_cv_exp(env, ctx, (*al)[i])();
        }
        if (al->isTuple()) {
          CallStackItem csi(env, al);
          Expression* al_ret = ArrayLit::constructTuple(Location().introduce(), es);
          Expression::type(al_ret, al->type());  // still contains var, so still CV

          // Add reverse mapper for tuple literal containing variables
          VarDecl* vd = new_vardecl(env, Ctx(), new TypeInst(Location().introduce(), al->type()),
                                    nullptr, nullptr, al_ret);
          vd->ti()->setStructDomain(env, al->type());
          env.reverseMappers.insert(vd->id(), al_ret);

          return vd->id();
        }
        std::vector<std::pair<int, int>> dims(al->dims());
        for (unsigned int i = 0; i < al->dims(); i++) {
          dims[i] = std::make_pair(al->min(i), al->max(i));
        }
        Expression* al_ret = new ArrayLit(Location().introduce(), es, dims);
        Type t = al->type();
        t.cv(false);
        Expression::type(al_ret, t);
        return al_ret;
      }
      case Expression::E_ARRAYACCESS: {
        auto* aa = Expression::cast<ArrayAccess>(e);
        GCLock lock;
        Expression* av = flat_cv_exp(env, ctx, aa->v())();
        std::vector<Expression*> idx(aa->idx().size());
        for (unsigned int i = 0; i < aa->idx().size(); i++) {
          idx[i] = flat_cv_exp(env, ctx, aa->idx()[i])();
        }
        auto* aa_ret = new ArrayAccess(Location().introduce(), av, idx);
        Type t = aa->type();
        t.cv(false);
        aa_ret->type(t);
        return eval_par(env, aa_ret);
      }
      case Expression::E_FIELDACCESS: {
        auto* fa = Expression::cast<FieldAccess>(e);
        assert(Expression::type(fa->v()).structBT());

        assert(Expression::isa<IntLit>(fa->field()));
        IntVal i = IntLit::v(Expression::cast<IntLit>(fa->field()));

        auto* al = Expression::cast<ArrayLit>(eval_array_lit(env, fa->v()));

        return flat_cv_exp(env, ctx, (*al)[static_cast<unsigned int>(i.toInt()) - 1]);
      }
      case Expression::E_COMP: {
        auto* c = Expression::cast<Comprehension>(e);
        GCLock lock;
        class EvalFlatCvExp : public EvalBase {
        public:
          Ctx ctx;
          EvalFlatCvExp(Ctx& ctx0) : ctx(ctx0) {}
          typedef Expression* ArrayVal;
          Expression* e(EnvI& env, Expression* e) const { return flat_cv_exp(env, ctx, e)(); }
          static Expression* exp(Expression* e) { return e; }
        } eval(ctx);
        EvaluatedComp<Expression*> a = eval_comp<EvalFlatCvExp>(env, eval, c);

        Type t = Type::bot();
        bool allPar = true;
        bool allOcc = true;
        for (auto& i : a.a) {
          if (t == Type::bot()) {
            t = Expression::type(i);
          } else if (t.structBT()) {
            if (t.bt() == Type::BT_TUPLE) {
              t = env.commonTuple(t, Expression::type(i), true);
            } else {  // (tret.bt() == Type::BT_RECORD)
              t = env.commonRecord(t, Expression::type(i), true);
            }
          }
          if (!Expression::type(i).isPar()) {
            allPar = false;
          }
          if (Expression::type(i).isOpt()) {
            allOcc = false;
          }
        }
        if (!allPar) {
          t.mkVar(env);
        }
        if (!allOcc) {
          t.ot(Type::OT_OPTIONAL);
        }
        if (c->set()) {
          t.st(Type::ST_SET);
        } else {
          t = Type::arrType(env, c->type(), t);
        }
        t.cv(false);
        if (c->set()) {
          if (c->type().isPar() && allPar) {
            auto* sl = new SetLit(Expression::loc(c).introduce(), a.a);
            sl->type(t);
            Expression* slr = eval_par(env, sl);
            Expression::type(slr, t);
            return slr;
          }
          throw InternalError("var set comprehensions not supported yet");
        }
        auto* alr = new ArrayLit(Location().introduce(), a.a, a.dims);
        alr->type(t);
        alr->flat(true);
        return alr;
      }
      case Expression::E_ITE: {
        ITE* ite = Expression::cast<ITE>(e);
        Ctx nctx = ctx;
        nctx.b = C_MIX;
        for (unsigned int i = 0; i < ite->size(); i++) {
          bool condition;
          KeepAlive ka;
          try {
            ka = flat_cv_exp(env, nctx, ite->ifExpr(i));
            condition = eval_bool(env, ka());
          } catch (ResultUndefinedError&) {
            condition = false;
          }
          if (condition) {
            return flat_cv_exp(env, ctx, ite->thenExpr(i));
          }
        }
        return flat_cv_exp(env, ctx, ite->elseExpr());
      }
      case Expression::E_BINOP: {
        auto* bo = Expression::cast<BinOp>(e);
        BinOp* nbo = nullptr;
        Ctx nctx = ctx;
        {
          CallStackItem _csi(env, bo);
          if (bo->op() == BOT_AND) {
            GCLock lock;
            nctx.b = +nctx.b;
            Expression* lhs = flat_cv_exp(env, nctx, bo->lhs())();
            if (!eval_bool(env, lhs)) {
              return env.constants.literalFalse;
            }
            return eval_par(env, flat_cv_exp(env, ctx, bo->rhs())());
          }
          if (bo->op() == BOT_OR) {
            GCLock lock;
            nctx.b = +nctx.b;
            Expression* lhs = flat_cv_exp(env, nctx, bo->lhs())();
            if (eval_bool(env, lhs)) {
              return env.constants.literalTrue;
            }
            return eval_par(env, flat_cv_exp(env, ctx, bo->rhs())());
          }
          GCLock lock;
          if (Expression::type(bo).isbool()) {
            nctx.b = C_MIX;
          }
          nbo = new BinOp(Expression::loc(bo).introduce(), flat_cv_exp(env, nctx, bo->lhs())(),
                          bo->op(), flat_cv_exp(env, nctx, bo->rhs())());
          for (auto* ann : Expression::ann(bo)) {
            Expression::addAnnotation(nbo, ann);
          }
          nbo->type(bo->type());
          nbo->decl(bo->decl());
        }
        return eval_par(env, nbo);
      }
      case Expression::E_UNOP: {
        UnOp* uo = Expression::cast<UnOp>(e);
        UnOp* nuo = nullptr;
        Ctx nctx = ctx;
        {
          CallStackItem _csi(env, uo);
          if (Expression::type(uo).isbool()) {
            nctx.b = -nctx.b;
          }
          GCLock lock;
          nuo = new UnOp(Expression::loc(uo), uo->op(), flat_cv_exp(env, nctx, uo->e())());
          for (auto* ann : Expression::ann(uo)) {
            Expression::addAnnotation(nuo, ann);
          }
          nuo->type(uo->type());
        }
        return eval_par(env, nuo);
      }
      case Expression::E_CALL: {
        Call* c = Expression::cast<Call>(e);
        if (ctx.b == C_ROOT && (c->decl()->e() != nullptr) &&
            Expression::isa<BoolLit>(c->decl()->e())) {
          bool allBool = true;
          for (unsigned int i = 0; i < c->argCount(); i++) {
            if (Expression::type(c->arg(i)).bt() != Type::BT_BOOL) {
              allBool = false;
              break;
            }
          }
          if (allBool) {
            return c->decl()->e();
          }
        }
        std::vector<Expression*> args(c->argCount());
        GCLock lock;
        if (env.constants.isCallByReferenceId(c->id())) {
          return eval_par(env, c);
        }
        CallStackItem _csi(env, c);
        for (unsigned int i = 0; i < c->argCount(); i++) {
          Ctx c_mix;
          c_mix.b = C_MIX;
          c_mix.i = C_MIX;
          args[i] = flat_cv_exp(env, c_mix, c->arg(i))();
        }
        Call* nc = Call::a(Expression::loc(c), c->id(), args);
        nc->decl(c->decl());
        Type nct(c->type());
        for (auto* ann : Expression::ann(c)) {
          Expression::addAnnotation(nc, ann);
        }
        if ((nc->decl()->e() != nullptr) && Expression::type(nc->decl()->e()).cv()) {
          nct.cv(false);
          nct.mkVar(env);
          nc->type(nct);
          EE ee;
          if (c->decl()->ann().contains(env.constants.ann.cache_result)) {
            auto it = env.cseMapFind(nc);
            if (it == env.cseMapEnd()) {
              ee = flat_exp(env, ctx, nc, nullptr, ctx.partialityVar(env));
              env.cseMapInsert(nc, ee);
            } else {
              ee.r = it->second.r;
              ee.b = it->second.b;
            }
          } else {
            ee = flat_exp(env, ctx, nc, nullptr, ctx.partialityVar(env));
          }
          if (isfalse(env, ee.b())) {
            std::ostringstream ss;
            ss << "evaluation of `" << demonomorphise_identifier(nc->id()) << "` was undefined";
            throw ResultUndefinedError(env, Expression::loc(e), ss.str());
          }
          return ee.r();
        }
        nc->type(nct);
        return eval_par(env, nc);
      }
      case Expression::E_LET: {
        Let* l = Expression::cast<Let>(e);
        EE ee = flat_exp(env, ctx, l, nullptr, ctx.partialityVar(env));
        if (isfalse(env, ee.b())) {
          throw ResultUndefinedError(env, Expression::loc(e),
                                     "evaluation of let expression was undefined");
        }
        return ee.r();
      }
    }
    throw InternalError("internal error");
  } catch (ResultUndefinedError&) {
    if (Expression::type(e) == Type::parbool()) {
      return env.constants.boollit(false);
    }
    throw;
  }
}

class ItemTimer {
public:
  using TimingMap =
      std::map<std::pair<ASTString, unsigned int>, std::chrono::high_resolution_clock::duration>;
  ItemTimer(const Location& loc, TimingMap* tm)
      : _loc(loc), _tm(tm), _start(std::chrono::high_resolution_clock::now()) {}

  ~ItemTimer() {
    try {
      if (_tm != nullptr) {
        std::chrono::high_resolution_clock::time_point end =
            std::chrono::high_resolution_clock::now();
        unsigned int line = _loc.firstLine();
        auto it = _tm->find(std::make_pair(_loc.filename(), line));
        if (it != _tm->end()) {
          it->second += end - _start;
        } else {
          _tm->insert(std::make_pair(std::make_pair(_loc.filename(), line), end - _start));
        }
      }
    } catch (std::exception& /*e*/) {
      assert(false);  // Invariant: Operations on the TimingMap will not throw an exception
    }
  }

private:
  const Location& _loc;
  TimingMap* _tm;
  std::chrono::high_resolution_clock::time_point _start;
};

namespace {

class FlattenModelVisitor : public ItemVisitor {
public:
  EnvI& env;
  bool& hadSolveItem;
  ItemTimer::TimingMap* timingMap;
  FlattenModelVisitor(EnvI& env0, bool& hadSolveItem0, ItemTimer::TimingMap* timingMap0)
      : env(env0), hadSolveItem(hadSolveItem0), timingMap(timingMap0) {}
  bool enter(Item* i) const {
    env.checkCancel();
    return !(i->isa<ConstraintI>() && env.failed());
  }
  void vVarDeclI(VarDeclI* v) {
    ItemTimer item_timer(v->loc(), timingMap);
    Expression::ann(v->e()).remove(env.constants.ann.output_var);
    Expression::ann(v->e()).removeCall(env.constants.ann.output_array);
    if (Expression::ann(v->e()).contains(env.constants.ann.output_only)) {
      return;
    }
    if (v->e()->type().isPar() && !v->e()->type().isOpt() && v->e()->e() != nullptr &&
        !Expression::type(v->e()->e()).cv() && Expression::type(v->e()).dim() > 0 &&
        v->e()->ti()->domain() == nullptr &&
        (v->e()->type().bt() == Type::BT_INT || v->e()->type().bt() == Type::BT_FLOAT)) {
      // Compute bounds for array literals
      CallStackItem csi(env, v->e());
      GCLock lock;
      ArrayLit* al = eval_array_lit(env, v->e()->e());
      v->e()->e(al);

      for (unsigned int i = 0; i < v->e()->ti()->ranges().size(); i++) {
        if (v->e()->ti()->ranges()[i]->domain() != nullptr) {
          IntSetVal* isv = eval_intset(env, v->e()->ti()->ranges()[i]->domain());
          if (isv->size() > 1) {
            throw EvalError(env, Expression::loc(v->e()->ti()->ranges()[i]->domain()),
                            "array index set must be contiguous range");
          }
        }
      }
      check_index_sets(env, v->e(), v->e()->e());
      if (!al->empty()) {
        if (v->e()->type().bt() == Type::BT_INT && v->e()->type().st() == Type::ST_PLAIN) {
          IntVal lb = IntVal::infinity();
          IntVal ub = -IntVal::infinity();
          for (unsigned int i = 0; i < al->size(); i++) {
            IntVal vi = eval_int(env, (*al)[i]);
            lb = std::min(lb, vi);
            ub = std::max(ub, vi);
          }
          GCLock lock;
          set_computed_domain(env, v->e(), new SetLit(Location().introduce(), IntSetVal::a(lb, ub)),
                              true);
        } else if (v->e()->type().bt() == Type::BT_FLOAT && v->e()->type().st() == Type::ST_PLAIN) {
          FloatVal lb = FloatVal::infinity();
          FloatVal ub = -FloatVal::infinity();
          for (unsigned int i = 0; i < al->size(); i++) {
            FloatVal vi = eval_float(env, (*al)[i]);
            lb = std::min(lb, vi);
            ub = std::max(ub, vi);
          }
          GCLock lock;
          set_computed_domain(env, v->e(),
                              new SetLit(Location().introduce(), FloatSetVal::a(lb, ub)), true);
        }
      }
    } else if (v->e()->type().isvar() || v->e()->type().isAnn()) {
      Ctx ctx;
      if (Expression::ann(v->e()).contains(env.constants.ctx.pos) ||
          Expression::ann(v->e()).contains(env.constants.ctx.promise_monotone)) {
        if (v->e()->type().isint()) {
          ctx.i = C_POS;
        } else if (v->e()->type().isbool()) {
          ctx.b = C_POS;
        }
      } else if (Expression::ann(v->e()).contains(env.constants.ctx.neg) ||
                 Expression::ann(v->e()).contains(env.constants.ctx.promise_antitone)) {
        if (v->e()->type().isint()) {
          ctx.i = C_NEG;
        } else if (v->e()->type().isbool()) {
          ctx.b = C_NEG;
        }
      }
      (void)flatten_id(env, ctx, v->e()->id(), nullptr, env.constants.varTrue, true);
    } else {
      if (v->e()->e() == nullptr) {
        if (!v->e()->type().isAnn()) {
          throw EvalError(env, Expression::loc(v->e()), "Undefined parameter", v->e()->id()->v());
        }
      } else {
        CallStackItem csi(env, v->e());
        GCLock lock;
        Location v_loc = Expression::loc(v->e()->e());
        if (!Expression::type(v->e()->e()).cv()) {
          v->e()->e(eval_par(env, v->e()->e()));
        } else {
          EE ee = flat_exp(env, Ctx(), v->e()->e(), nullptr, env.constants.varTrue);
          v->e()->e(ee.r());
        }
        flatten_vardecl_annotations(env, v->e(), v, v->e());
        check_par_declaration(env, v->e());
      }
    }
  }
  void vConstraintI(ConstraintI* ci) {
    ItemTimer item_timer(ci->loc(), timingMap);
    (void)flat_exp(env, Ctx(), ci->e(), env.constants.varTrue, env.constants.varTrue);
  }
  void vSolveI(SolveI* si) {
    if (hadSolveItem) {
      throw FlatteningError(env, si->loc(), "Only one solve item allowed");
    }
    ItemTimer item_timer(si->loc(), timingMap);
    hadSolveItem = true;
    GCLock lock;
    SolveI* nsi = nullptr;
    switch (si->st()) {
      case SolveI::ST_SAT:
        nsi = SolveI::sat(Location());
        break;
      case SolveI::ST_MIN: {
        Ctx ctx;
        ctx.i = C_NEG;
        nsi = SolveI::min(Location().introduce(),
                          flat_exp(env, ctx, si->e(), nullptr, env.constants.varTrue).r());
      } break;
      case SolveI::ST_MAX: {
        Ctx ctx;
        ctx.i = C_POS;
        nsi = SolveI::max(Location().introduce(),
                          flat_exp(env, ctx, si->e(), nullptr, env.constants.varTrue).r());
      } break;
    }
    for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
      Expression* ret = flat_exp(env, Ctx(), *it, nullptr, env.constants.varTrue).r();
      if (auto* c = Expression::dynamicCast<Call>(ret)) {
        if (c->id() == env.constants.ids.on_restart.on_restart) {
          assert(c->argCount() == 1);
          auto* pred = Expression::cast<StringLit>(c->arg(0));
          Call* nc = Call::a(Expression::loc(c).introduce(), pred->v(), std::vector<Expression*>());
          nc->type(Type::varbool());
          FunctionI* decl = env.model->matchFn(env, nc, false);
          if (decl == nullptr) {
            std::ostringstream ss;
            ss << "Unknown predicate `" << pred->v()
               << "' found while evaluating on_restart annotation";
            throw FlatteningError(env, Expression::loc(pred), ss.str());
          }
          nc->decl(decl);
          env.flatAddItem(new ConstraintI(Expression::loc(c).introduce(), nc));
          continue;
        }
      }
      nsi->ann().add(ret);
    }
    env.flatAddItem(nsi);
  }
  void vOutputI(OutputI* oi) {
    assert(oi->ann().size() <= 1);
    auto section = env.constants.ids.mzn_default;
    bool json = false;
    auto* e = oi->e();
    GCLock lock;
    for (auto* ann : oi->ann()) {
      auto* s = Expression::dynamicCast<Call>(flat_cv_exp(env, Ctx(), ann)());
      if (s == nullptr || (s->id() != env.constants.ids.mzn_output_section)) {
        env.addWarning(Expression::loc(ann), "Unrecognised output item section annotation.");
      }
      json = eval_bool(env, s->arg(1));
      section = ASTString(eval_string(env, s->arg(0)));
    }
    if (json) {
      auto* c = Call::a(Expression::loc(e).introduce(), env.constants.ids.showJSON, {e});
      c->decl(env.model->matchFn(env, c, false));
      c->type(Type::parstring());
      auto* nl = new StringLit(Expression::loc(e).introduce(), "\n");
      std::vector<Expression*> v({c, nl});
      e = new ArrayLit(Expression::loc(e).introduce(), v);
      Expression::type(e, Type::parstring(1));
    } else if (Expression::type(e) == Type::parstring()) {
      std::vector<Expression*> v({e});
      e = new ArrayLit(Expression::loc(e).introduce(), v);
      Expression::type(e, Type::parstring(1));
    } else if (Expression::type(e) == Type::ann()) {
      auto* result = Expression::dynamicCast<Id>(flat_cv_exp(env, Ctx(), e)());
      if (!Expression::equal(result, env.constants.ann.output_only)) {
        throw FlatteningError(env, Expression::loc(e),
                              "An annotation which is the expression of an output item must "
                              "evaluate to `output_only'.");
      }
      return;
    } else if (Expression::type(e) != Type::parstring(1) && Expression::type(e) != Type::bot(1)) {
      throw TypeError(env, Expression::loc(e),
                      "invalid type in output item, expected `" + Type::parstring(1).toString(env) +
                          "', actual `" + Expression::type(e).toString(env) + "'");
    }
    env.outputSections.add(env, section, e, json);
  }
};

class SimplifyFunctionBodiesVisitor : public ItemVisitor {
public:
  Model& toAdd;
  EnvI& env;
  SimplifyFunctionBodiesVisitor(EnvI& env0, Model& toAdd0) : env(env0), toAdd(toAdd0) {}
  void vFunctionI(FunctionI* fi) { eval_static_function_body(env, fi, toAdd); }
};

}  // namespace

void flatten(Env& e, FlatteningOptions opt) {
  class OverflowWatch {
  public:
    OverflowWatch(Env& e) { OverflowHandler::setEnv(e); }
    ~OverflowWatch() { OverflowHandler::removeEnv(); }
  };
  OverflowWatch ow(e);
  e.envi().setRandomSeed(opt.randomSeed);
  ItemTimer::TimingMap timingMap_o;
  ItemTimer::TimingMap* timingMap = opt.detailedTiming ? &timingMap_o : nullptr;
  try {
    EnvI& env = e.envi();
    env.fopts = opt;

    // Statically evaluate some function bodies
    {
      std::unique_ptr<Model> toAdd(new Model);
      SimplifyFunctionBodiesVisitor _ffbv(env, *toAdd);
      iter_items<SimplifyFunctionBodiesVisitor>(_ffbv, e.model());
      for (auto* it : *toAdd) {
        e.model()->addItem(it);
      }
    }

    process_toplevel_output_vars(e.envi());

    bool onlyRangeDomains = false;
    if (opt.onlyRangeDomains) {
      onlyRangeDomains = true;  // compulsory
    } else {
      GCLock lock;
      Call* check_only_range = Call::a(Location(), "mzn_check_only_range_domains", {});
      check_only_range->type(Type::parbool());
      check_only_range->decl(env.model->matchFn(e.envi(), check_only_range, false));
      onlyRangeDomains = eval_bool(e.envi(), check_only_range);
    }

    bool halfReifyClause = true;
    if (!env.fopts.enableHalfReification) {
      halfReifyClause = false;
    } else {
      GCLock lock;
      Call* check_hr_clause = Call::a(Location(), "mzn_check_half_reify_clause", {});
      check_hr_clause->type(Type::parbool());
      check_hr_clause->decl(env.model->matchFn(e.envi(), check_hr_clause, false));
      halfReifyClause = eval_bool(e.envi(), check_hr_clause);
    }

    // Flatten main model
    bool hadSolveItem = false;
    FlattenModelVisitor _fv(env, hadSolveItem, timingMap);
    iter_items<FlattenModelVisitor>(_fv, e.model());

    if (!hadSolveItem) {
      GCLock lock;
      e.envi().flatAddItem(SolveI::sat(Location().introduce()));
    }

    // Create output model
    if (e.flat()->solveItem()->st() == SolveI::SolveType::ST_SAT) {
      // Never output _objective for SAT problems
      opt.outputObjective = false;
    }
    if (opt.keepOutputInFzn) {
      copy_output(env);
    } else {
      create_output(env, opt.outputMode, opt.outputObjective, opt.outputOutputItem, opt.hasChecker,
                    opt.encapsulateJSON);
    }

    // Flatten remaining redefinitions
    Model& m = *e.flat();
    int startItem = 0;
    int endItem = static_cast<int>(m.size()) - 1;

    FunctionI* int_lin_eq;
    {
      std::vector<Type> int_lin_eq_t(3);
      int_lin_eq_t[0] = Type::parint(1);
      int_lin_eq_t[1] = Type::varint(1);
      int_lin_eq_t[2] = Type::parint(0);
      GCLock lock;
      FunctionI* fi = env.model->matchFn(env, env.constants.ids.int_.lin_eq, int_lin_eq_t, false);
      int_lin_eq = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;
    }
    FunctionI* float_lin_eq;
    {
      std::vector<Type> float_lin_eq_t(3);
      float_lin_eq_t[0] = Type::parfloat(1);
      float_lin_eq_t[1] = Type::varfloat(1);
      float_lin_eq_t[2] = Type::parfloat(0);
      GCLock lock;
      FunctionI* fi =
          env.model->matchFn(env, env.constants.ids.float_.lin_eq, float_lin_eq_t, false);
      float_lin_eq = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;
    }
    FunctionI* array_bool_and;
    FunctionI* array_bool_and_imp;
    FunctionI* array_bool_clause;
    FunctionI* array_bool_clause_reif;
    FunctionI* bool_xor;
    {
      std::vector<Type> argtypes(2);
      argtypes[0] = Type::varbool(1);
      argtypes[1] = Type::varbool(0);
      GCLock lock;
      FunctionI* fi =
          env.model->matchFn(env, env.constants.ids.bool_reif.array_and, argtypes, false);
      array_bool_and = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;
      fi = env.model->matchFn(env, env.constants.ids.array_bool_and_imp, argtypes, false);
      array_bool_and_imp = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;

      argtypes[1] = Type::varbool(1);
      fi = env.model->matchFn(env, env.constants.ids.bool_.clause, argtypes, false);
      array_bool_clause = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;

      argtypes.push_back(Type::varbool());
      fi = env.model->matchFn(env, env.constants.ids.bool_reif.clause, argtypes, false);
      array_bool_clause_reif = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;

      argtypes[0] = Type::varbool();
      argtypes[1] = Type::varbool();
      argtypes[2] = Type::varbool();
      fi = env.model->matchFn(env, env.constants.ids.bool_.ne, argtypes, false);
      bool_xor = ((fi != nullptr) && (fi->e() != nullptr)) ? fi : nullptr;
    }

    std::vector<int> processLast;
    env.collectVarDecls(true);

    while (startItem <= endItem || !env.modifiedVarDecls.empty() || !processLast.empty()) {
      if (env.failed()) {
        return;
      }
      std::vector<int> agenda;
      for (int i = startItem; i <= endItem; i++) {
        agenda.push_back(i);
      }
      for (int modifiedVarDecl : env.modifiedVarDecls) {
        agenda.push_back(modifiedVarDecl);
      }
      env.modifiedVarDecls.clear();

      bool doLastProcessing = false;
      if (agenda.empty()) {
        for (auto i : processLast) {
          agenda.push_back(i);
        }
        processLast.clear();
        doLastProcessing = true;
      }

      for (int i : agenda) {
        env.checkCancel();

        if (auto* vdi = m[i]->dynamicCast<VarDeclI>()) {
          if (vdi->removed()) {
            continue;
          }
          /// Look at constraints
          if (!is_output(vdi->e())) {
            if (0 < env.varOccurrences.occurrences(vdi->e())) {
              const auto it = env.varOccurrences.itemMap.find(vdi->e()->id());
              if (it.first) {
                bool hasRedundantOccurrenciesOnly = true;
                for (const auto& c : *it.second) {
                  if (auto* constrI = c->dynamicCast<ConstraintI>()) {
                    if (auto* call = Expression::dynamicCast<Call>(constrI->e())) {
                      if (call->id() == env.constants.ids.mzn_reverse_map_var) {
                        continue;  // all good
                      }
                    }
                  }
                  hasRedundantOccurrenciesOnly = false;
                  break;
                }
                if (hasRedundantOccurrenciesOnly) {
                  env.flatRemoveItem(vdi);
                  env.varOccurrences.removeAllOccurrences(vdi->e());
                  for (const auto& c : *it.second) {
                    c->remove();
                  }
                  continue;
                }
              }
            } else {  // 0 occurrencies
              if ((vdi->e()->e() != nullptr) && (vdi->e()->ti()->domain() != nullptr)) {
                if (vdi->e()->type().isvar() && vdi->e()->type().isbool() &&
                    !vdi->e()->type().isOpt() &&
                    Expression::equal(vdi->e()->ti()->domain(), env.constants.literalTrue)) {
                  GCLock lock;
                  auto* ci = new ConstraintI(vdi->loc(), vdi->e()->e());
                  if (vdi->e()->introduced()) {
                    env.flatAddItem(ci);
                    env.flatRemoveItem(vdi);
                    continue;
                  }
                  vdi->e()->e(nullptr);
                  env.flatAddItem(ci);
                } else if (vdi->e()->type().isPar() || vdi->e()->ti()->computedDomain()) {
                  env.flatRemoveItem(vdi);
                  continue;
                }
              } else {
                env.flatRemoveItem(vdi);
                continue;
              }
            }
          }
          // If no reverse mapper has been put in place, introduce it now
          FunctionI* fi = env.model->matchRevMap(env, vdi->e()->type());
          if (fi != nullptr && !env.hasReverseMapper(vdi->e()->id())) {
            if (doLastProcessing) {
              GCLock lock;
              Call* revmap = Call::a(Location().introduce(), fi->id(), {vdi->e()->id()});
              revmap->decl(fi);
              Expression::type(revmap, Type::varbool());
              // Give distinct call stack
              Annotation& ann = Expression::ann(vdi->e());
              Expression* tmp = revmap;
              if (Expression* mznpath_ann = ann.getCall(env.constants.ann.mzn_path)) {
                tmp = Expression::cast<Call>(mznpath_ann)->arg(0);
              }
              CallStackItem csi(env, tmp);
              env.flatAddItem(new ConstraintI(Location().introduce(), revmap));
            } else {
              processLast.push_back(i);
            }
          }
          if (vdi->e()->type().dim() > 0 && vdi->e()->type().isvar()) {
            vdi->e()->ti()->eraseDomain();
          }
          if (vdi->e()->type().isint() && vdi->e()->type().isvar() &&
              vdi->e()->ti()->domain() != nullptr && !vdi->e()->type().isOpt()) {
            GCLock lock;
            IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());

            if (dom->empty()) {
              std::ostringstream oss;
              oss << "Variable has empty domain: " << (*vdi->e());
              env.fail(oss.str());
            }

            bool needRangeDomain = onlyRangeDomains;
            if (!needRangeDomain) {
              if (dom->min(0).isMinusInfinity() || dom->max(dom->size() - 1).isPlusInfinity()) {
                needRangeDomain = true;
              }
            }
            if (needRangeDomain) {
              if (doLastProcessing) {
                Expression* dom_expr = vdi->e()->ti()->domain();
                if (dom->min(0).isMinusInfinity() || dom->max(dom->size() - 1).isPlusInfinity()) {
                  // Erase the domain to remove infinity literal
                  auto* nti = Expression::cast<TypeInst>(copy(env, vdi->e()->ti()));
                  nti->domain(nullptr);
                  vdi->e()->ti(nti);
                  // Add the ub/lb back as a constraint
                  Call* call = nullptr;
                  if (dom->min(0).isFinite()) {
                    // Note: cannot be combined into `mzn_set_in_internal` as CSE interferes
                    call = Call::a(vdi->loc(), env.constants.ids.int_.le,
                                   {IntLit::a(dom->min(0)), vdi->e()->id()});
                  } else if (dom->max(dom->size() - 1).isFinite()) {
                    // Note: cannot be combined into `mzn_set_in_internal` as CSE interferes
                    call = Call::a(vdi->loc(), env.constants.ids.int_.le,
                                   {vdi->e()->id(), IntLit::a(dom->max(dom->size() - 1))});
                  }
                  if (call != nullptr) {
                    call->type(Type::varbool());
                    call->decl(env.model->matchFn(env, call, false));
                    env.flatAddItem(new ConstraintI(vdi->loc(), call));
                  }
                } else if (dom->size() > 1) {
                  // Simplify domains to be a single range
                  auto* newDom = new SetLit(Location().introduce(),
                                            IntSetVal::a(dom->min(0), dom->max(dom->size() - 1)));
                  auto* nti = Expression::cast<TypeInst>(copy(env, vdi->e()->ti()));
                  nti->domain(newDom);
                  vdi->e()->ti(nti);
                }

                // Re-add the more complex (range) domain constraint
                // note: might still be required when infinity in the domain
                if (dom->size() > 1) {
                  Call* call = Call::a(vdi->loc(), env.constants.ids.mzn_set_in_internal,
                                       {vdi->e()->id(), dom_expr});
                  call->type(Type::varbool());
                  call->decl(env.model->matchFn(env, call, false));
                  // Give distinct call stack
                  Annotation& ann = Expression::ann(vdi->e());
                  Expression* tmp = call;
                  if (Expression* mznpath_ann = ann.getCall(env.constants.ann.mzn_path)) {
                    tmp = Expression::cast<Call>(mznpath_ann)->arg(0);
                  }
                  CallStackItem csi(env, tmp);
                  env.flatAddItem(new ConstraintI(vdi->loc(), call));
                }
              } else {
                processLast.push_back(i);
              }
            }
          }
          if (vdi->e()->type().isfloat() && vdi->e()->type().isvar() &&
              vdi->e()->ti()->domain() != nullptr) {
            GCLock lock;
            Expression* dom_expr = vdi->e()->ti()->domain();
            FloatSetVal* vdi_dom = eval_floatset(env, dom_expr);
            if (vdi_dom->empty()) {
              std::ostringstream oss;
              oss << "Variable has empty domain: " << (*vdi->e());
              env.fail(oss.str());
            }
            FloatVal vmin = vdi_dom->min();
            FloatVal vmax = vdi_dom->max();
            if (vmin == -FloatVal::infinity() || vmax == FloatVal::infinity()) {
              // Erase the domain to remove infinity literal
              vdi->e()->ti()->domain(nullptr);
              // Add the ub/lb back as a constraint
              Call* call = nullptr;
              if (vmax != FloatVal::infinity()) {
                // Note: cannot be combined into `mzn_set_in_internal` as CSE interferes
                call = Call::a(vdi->loc(), env.constants.ids.float_.le,
                               {vdi->e()->id(), FloatLit::a(vmax)});
              } else if (vmin != -FloatVal::infinity()) {
                // Note: cannot be combined into `mzn_set_in_internal` as CSE interferes
                call = Call::a(Location().introduce(), env.constants.ids.float_.le,
                               {FloatLit::a(vmin), vdi->e()->id()});
              }
              if (call != nullptr) {
                call->type(Type::varbool());
                call->decl(env.model->matchFn(env, call, false));
                env.flatAddItem(new ConstraintI(vdi->loc(), call));
              }
            } else if (vdi_dom->size() > 1) {
              // Simplify domains to be a single range
              auto* range = new SetLit(Expression::loc(vdi->e()->ti()->domain()),
                                       FloatSetVal::a({FloatSetVal::Range(vmin, vmax)}));
              range->type(Type::parsetfloat());
              vdi->e()->ti()->domain(range);
            }

            // Re-add the more complex (range) domain constraint
            // note: might still be required when infinity in the domain
            if (vdi_dom->size() > 1) {
              Call* call = Call::a(vdi->loc(), env.constants.ids.mzn_set_in_internal,
                                   {vdi->e()->id(), dom_expr});
              call->type(Type::varbool());
              call->decl(env.model->matchFn(env, call, false));
              // Give distinct call stack
              Annotation& ann = Expression::ann(vdi->e());
              Expression* tmp = call;
              if (Expression* mznpath_ann = ann.getCall(env.constants.ann.mzn_path)) {
                tmp = Expression::cast<Call>(mznpath_ann)->arg(0);
              }
              CallStackItem csi(env, tmp);
              env.flatAddItem(new ConstraintI(vdi->loc(), call));
            }
          }
        }
      }

      // rewrite some constraints if there are redefinitions
      for (int i : agenda) {
        if (m[i]->removed()) {
          continue;
        }
        if (auto* vdi = m[i]->dynamicCast<VarDeclI>()) {
          VarDecl* vd = vdi->e();
          if (vd->e() != nullptr) {
            bool isTrueVar = vd->type().isbool() &&
                             Expression::equal(vd->ti()->domain(), env.constants.literalTrue);
            if (Call* c = Expression::dynamicCast<Call>(vd->e())) {
              GCLock lock;
              Call* nc = nullptr;
              if (c->id() == env.constants.ids.lin_exp) {
                if (c->type().isfloat() && (float_lin_eq != nullptr)) {
                  std::vector<Expression*> args(c->argCount());
                  auto* le_c = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
                  std::vector<Expression*> nc_c(le_c->size());
                  for (auto ii = static_cast<unsigned int>(nc_c.size()); (ii--) != 0U;) {
                    nc_c[ii] = (*le_c)[ii];
                  }
                  nc_c.push_back(FloatLit::a(-1));
                  args[0] = new ArrayLit(Location().introduce(), nc_c);
                  Expression::type(args[0], Type::parfloat(1));
                  auto* le_x = Expression::cast<ArrayLit>(follow_id(c->arg(1)));
                  std::vector<Expression*> nx(le_x->size());
                  for (auto ii = static_cast<unsigned int>(nx.size()); (ii--) != 0U;) {
                    nx[ii] = (*le_x)[ii];
                  }
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location().introduce(), nx);
                  Expression::type(args[1], Type::varfloat(1));
                  FloatVal d = FloatLit::v(Expression::cast<FloatLit>(c->arg(2)));
                  args[2] = FloatLit::a(-d);
                  Expression::type(args[2], Type::parfloat(0));
                  nc = Call::a(Expression::loc(c).introduce(), ASTString("float_lin_eq"), args);
                  nc->type(Type::varbool());
                  nc->decl(float_lin_eq);
                } else if (int_lin_eq != nullptr) {
                  assert(c->type().isint());
                  std::vector<Expression*> args(c->argCount());
                  auto* le_c = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
                  std::vector<Expression*> nc_c(le_c->size());
                  for (auto ii = static_cast<unsigned int>(nc_c.size()); (ii--) != 0U;) {
                    nc_c[ii] = (*le_c)[ii];
                  }
                  nc_c.push_back(IntLit::a(-1));
                  args[0] = new ArrayLit(Location().introduce(), nc_c);
                  Expression::type(args[0], Type::parint(1));
                  auto* le_x = Expression::cast<ArrayLit>(follow_id(c->arg(1)));
                  std::vector<Expression*> nx(le_x->size());
                  for (auto ii = static_cast<unsigned int>(nx.size()); (ii--) != 0U;) {
                    nx[ii] = (*le_x)[ii];
                  }
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location().introduce(), nx);
                  Expression::type(args[1], Type::varint(1));
                  IntVal d = IntLit::v(Expression::cast<IntLit>(c->arg(2)));
                  args[2] = IntLit::a(-d);
                  Expression::type(args[2], Type::parint(0));
                  nc = Call::a(Expression::loc(c).introduce(), ASTString("int_lin_eq"), args);
                  nc->type(Type::varbool());
                  nc->decl(int_lin_eq);
                }
              } else if (c->id() == env.constants.ids.exists) {
                if (isTrueVar && array_bool_clause != nullptr) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = env.constants.emptyBoolArray;
                  nc = Call::a(Expression::loc(c).introduce(), array_bool_clause->id(), args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause);
                } else if (halfReifyClause && Expression::ann(vd).contains(env.constants.ctx.pos)) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = new ArrayLit(Expression::loc(vd).introduce(),
                                         std::vector<Expression*>({vd->id()}));
                  Expression::type(args[1], Type::varbool(1));
                  nc = Call::a(Expression::loc(c).introduce(),
                               (array_bool_clause != nullptr) ? array_bool_clause->id()
                                                              : env.constants.ids.clause,
                               args);
                  nc->type(Type::varbool());
                  nc->decl((array_bool_clause != nullptr) ? array_bool_clause
                                                          : env.model->matchFn(env, nc, false));
                } else if (!isTrueVar && array_bool_clause_reif != nullptr) {
                  std::vector<Expression*> args(3);
                  args[0] = c->arg(0);
                  args[1] = env.constants.emptyBoolArray;
                  args[2] = vd->id();
                  nc = Call::a(Expression::loc(c).introduce(), array_bool_clause_reif->id(), args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause_reif);
                }
              } else if (!isTrueVar && c->id() == env.constants.ids.forall) {
                if (env.fopts.enableHalfReification &&
                    Expression::ann(vd).contains(env.constants.ctx.pos) &&
                    array_bool_and_imp != nullptr) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = vd->id();
                  nc = Call::a(Expression::loc(c).introduce(), array_bool_and_imp->id(), args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_and_imp);
                } else if (array_bool_and != nullptr) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = vd->id();
                  nc = Call::a(Expression::loc(c).introduce(), array_bool_and->id(), args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_and);
                }
              } else if (isTrueVar && c->id() == env.constants.ids.clause &&
                         (array_bool_clause != nullptr)) {
                std::vector<Expression*> args(2);
                args[0] = c->arg(0);
                args[1] = c->arg(1);
                nc = Call::a(Expression::loc(c).introduce(), array_bool_clause->id(), args);
                nc->type(Type::varbool());
                nc->decl(array_bool_clause);
              } else if (c->id() == env.constants.ids.clause && halfReifyClause &&
                         Expression::ann(vd).contains(env.constants.ctx.pos)) {
                std::vector<Expression*> args(2);
                args[0] = c->arg(0);
                ArrayLit* old = eval_array_lit(env, c->arg(1));
                std::vector<Expression*> neg(old->size() + 1);
                for (unsigned int i = 0; i < old->size(); ++i) {
                  neg[i] = (*old)[i];
                }
                neg[old->size()] = vd->id();
                args[1] = new ArrayLit(Expression::loc(old).introduce(), neg);
                Expression::type(args[1], Type::varbool(1));
                nc = Call::a(Expression::loc(c).introduce(),
                             (array_bool_clause != nullptr) ? array_bool_clause->id()
                                                            : env.constants.ids.clause,
                             args);
                nc->type(Type::varbool());
                nc->decl((array_bool_clause != nullptr) ? array_bool_clause : c->decl());
              } else if (!isTrueVar && c->id() == env.constants.ids.clause &&
                         (array_bool_clause_reif != nullptr)) {
                std::vector<Expression*> args(3);
                args[0] = c->arg(0);
                args[1] = c->arg(1);
                args[2] = vd->id();
                nc = Call::a(Expression::loc(c).introduce(), array_bool_clause_reif->id(), args);
                nc->type(Type::varbool());
                nc->decl(array_bool_clause_reif);
              } else if (c->id() == env.constants.ids.bool_.not_ && c->argCount() == 1 &&
                         c->decl()->e() == nullptr) {
                bool isFalseVar = Expression::equal(vd->ti()->domain(), env.constants.literalFalse);
                if (c->arg(0) == env.constants.literalTrue) {
                  if (isTrueVar) {
                    env.fail();
                  } else {
                    env.flatRemoveExpr(c, vdi);
                    env.cseMapRemove(c);
                    vd->e(env.constants.literalFalse);
                    vd->ti()->domain(env.constants.literalFalse);
                  }
                } else if (c->arg(0) == env.constants.literalFalse) {
                  if (isFalseVar) {
                    env.fail();
                  } else {
                    env.flatRemoveExpr(c, vdi);
                    env.cseMapRemove(c);
                    vd->e(env.constants.literalTrue);
                    vd->ti()->domain(env.constants.literalTrue);
                  }
                } else if (Expression::isa<Id>(c->arg(0)) && (isTrueVar || isFalseVar)) {
                  VarDecl* arg_vd = Expression::cast<Id>(c->arg(0))->decl();
                  if (arg_vd->ti()->domain() == nullptr) {
                    if (arg_vd->e() == nullptr) {
                      arg_vd->e(env.constants.boollit(!isTrueVar));
                    }
                    arg_vd->ti()->domain(env.constants.boollit(!isTrueVar));
                  } else if (arg_vd->ti()->domain() == env.constants.boollit(isTrueVar)) {
                    env.fail();
                  } else if (arg_vd->e() == nullptr) {
                    assert(arg_vd->ti()->domain() == env.constants.boollit(!isTrueVar));
                    arg_vd->e(arg_vd->ti()->domain());
                  }
                  env.flatRemoveExpr(c, vdi);
                  vd->e(nullptr);
                  // Need to remove right hand side from CSE map, otherwise
                  // flattening of nc could assume c has already been flattened
                  // to vd
                  env.cseMapRemove(c);
                } else {
                  // don't know how to handle, use bool_not/2
                  nc = Call::a(Expression::loc(c).introduce(), c->id(), {c->arg(0), vd->id()});
                  nc->type(Type::varbool());
                  nc->decl(env.model->matchFn(env, nc, false));
                }
              } else {
                if (isTrueVar) {
                  FunctionI* decl = env.model->matchFn(env, c, false);
                  env.cseMapRemove(c);
                  if ((decl->e() != nullptr) || c->id() == env.constants.ids.forall) {
                    if (decl->e() != nullptr) {
                      add_path_annotation(env, decl->e());
                    }
                    c->decl(decl);
                    nc = c;
                  }
                } else {
                  std::vector<Expression*> args(c->argCount());
                  for (auto i = static_cast<unsigned int>(args.size()); (i--) != 0U;) {
                    args[i] = c->arg(i);
                  }
                  args.push_back(vd->id());
                  FunctionI* decl = nullptr;
                  if (c->type().isbool() && vd->type().isbool()) {
                    bool canHalfReify = env.fopts.enableHalfReification &&
                                        Expression::ann(vd).contains(env.constants.ctx.pos);
                    decl = env.model->matchReification(env, c->id(), args, canHalfReify, false);

                    if (decl == nullptr) {
                      std::ostringstream ss;
                      ss << "'" << demonomorphise_identifier(c->id())
                         << "' is used in a reified context but no reified version is "
                            "available";
                      throw FlatteningError(env, Expression::loc(c), ss.str());
                    }
                  } else {
                    decl = env.model->matchFn(env, c->id(), args, false);
                  }
                  if ((decl != nullptr) && (decl->e() != nullptr)) {
                    add_path_annotation(env, decl->e());
                    nc = Call::a(Expression::loc(c).introduce(), decl->id(), args);
                    nc->type(Type::varbool());
                    nc->decl(decl);
                  }
                }
              }
              if (nc != nullptr) {
                // Note: Removal of VarDecl's referenced by c must be delayed
                // until nc is flattened
                std::vector<VarDecl*> toRemove;
                CollectDecls cd(env, env.varOccurrences, toRemove, vdi);
                top_down(cd, c);
                vd->e(nullptr);
                // Need to remove right hand side from CSE map, otherwise
                // flattening of nc could assume c has already been flattened
                // to vd
                env.cseMapRemove(c);
                /// TODO: check if removing variables here makes sense:
                //                  if (!is_output(vd) && env.varOccurrences.occurrences(vd)==0) {
                //                    removedItems.push_back(vdi);
                //                  }
                if (nc != c) {
                  make_defined_var(env, vd, nc);
                  for (ExpressionSetIter it = Expression::ann(c).begin();
                       it != Expression::ann(c).end(); ++it) {
                    EE ee_ann = flat_exp(env, Ctx(), *it, nullptr, env.constants.varTrue);
                    Expression::addAnnotation(nc, ee_ann.r());
                  }
                }
                StringLit* vsl = get_longest_mzn_path_annotation(env, vdi->e());
                StringLit* csl = get_longest_mzn_path_annotation(env, c);
                CallStackItem* vsi = nullptr;
                CallStackItem* csi = nullptr;
                if (vsl != nullptr) {
                  vsi = new CallStackItem(env, vsl);
                }
                if (csl != nullptr) {
                  csi = new CallStackItem(env, csl);
                }
                Location orig_loc = Expression::loc(nc);
                if (csl != nullptr) {
                  ASTString loc = csl->v();
                  size_t sep = loc.find('|');
                  std::string filename = loc.substr(0, sep);
                  std::string start_line_s = loc.substr(sep + 1, loc.find('|', sep + 1) - sep - 1);
                  int start_line = std::stoi(start_line_s);
                  Location new_loc(ASTString(filename), start_line, 0, start_line, 0);
                  orig_loc = new_loc;
                }
                ItemTimer item_timer(orig_loc, timingMap);
                (void)flat_exp(env, Ctx(), nc, env.constants.varTrue, env.constants.varTrue);

                delete csi;

                delete vsi;

                // Remove VarDecls becoming unused through the removal of c
                // because they are not used by nc
                while (!toRemove.empty()) {
                  VarDecl* cur = toRemove.back();
                  toRemove.pop_back();
                  if (env.varOccurrences.occurrences(cur) == 0 && CollectDecls::varIsFree(cur)) {
                    auto cur_idx = env.varOccurrences.idx.find(cur->id());
                    if (cur_idx.first) {
                      auto* vdi = m[*cur_idx.second]->cast<VarDeclI>();
                      if (!is_output(cur) && !m[*cur_idx.second]->removed()) {
                        CollectDecls cd(env, env.varOccurrences, toRemove, vdi);
                        top_down(cd, vdi->e()->e());
                        vdi->remove();
                      }
                    }
                  }
                }
              }
            }
          }
        } else if (auto* ci = m[i]->dynamicCast<ConstraintI>()) {
          if (Call* c = Expression::dynamicCast<Call>(ci->e())) {
            GCLock lock;
            Call* nc = nullptr;
            if (c->id() == env.constants.ids.exists) {
              if (array_bool_clause != nullptr) {
                std::vector<Expression*> args(2);
                args[0] = c->arg(0);
                args[1] = env.constants.emptyBoolArray;
                nc = Call::a(Expression::loc(c).introduce(), array_bool_clause->id(), args);
                nc->type(Type::varbool());
                nc->decl(array_bool_clause);
              }
            } else if (c->id() == env.constants.ids.forall) {
              if (array_bool_and != nullptr) {
                std::vector<Expression*> args(2);
                args[0] = c->arg(0);
                args[1] = env.constants.literalTrue;
                nc = Call::a(Expression::loc(c).introduce(), array_bool_and->id(), args);
                nc->type(Type::varbool());
                nc->decl(array_bool_and);
              }
            } else if (c->id() == env.constants.ids.clause) {
              if (array_bool_clause != nullptr) {
                std::vector<Expression*> args(2);
                args[0] = c->arg(0);
                args[1] = c->arg(1);
                nc = Call::a(Expression::loc(c).introduce(), array_bool_clause->id(), args);
                nc->type(Type::varbool());
                nc->decl(array_bool_clause);
              }
            } else if (c->id() == env.constants.ids.bool_.ne) {
              if (bool_xor != nullptr) {
                std::vector<Expression*> args(3);
                args[0] = c->arg(0);
                args[1] = c->arg(1);
                args[2] = c->argCount() == 2 ? env.constants.literalTrue : c->arg(2);
                nc = Call::a(Expression::loc(c).introduce(), bool_xor->id(), args);
                nc->type(Type::varbool());
                nc->decl(bool_xor);
              }
            } else if (c->id() == env.constants.ids.bool_.not_ && c->argCount() == 1 &&
                       c->decl()->e() == nullptr) {
              if (c->arg(0) == env.constants.literalTrue) {
                env.fail();
              } else if (c->arg(0) == env.constants.literalFalse) {
                // nothing to do, not false = true
              } else if (Expression::isa<Id>(c->arg(0))) {
                VarDecl* vd = Expression::cast<Id>(c->arg(0))->decl();
                if (vd->ti()->domain() == nullptr) {
                  vd->ti()->domain(env.constants.literalFalse);
                } else if (vd->ti()->domain() == env.constants.literalTrue) {
                  env.fail();
                }
              } else {
                // don't know how to handle, use bool_not/2
                nc = Call::a(Expression::loc(c).introduce(), c->id(),
                             {c->arg(0), env.constants.literalTrue});
                nc->type(Type::varbool());
                nc->decl(env.model->matchFn(env, nc, false));
              }
              if (nc == nullptr) {
                env.flatRemoveItem(ci);
              }
            } else {
              FunctionI* decl = env.model->matchFn(env, c, false);
              if ((decl != nullptr) && (decl->e() != nullptr)) {
                nc = c;
                nc->decl(decl);
              }
            }
            if (nc != nullptr) {
              if (nc != c) {
                for (ExpressionSetIter it = Expression::ann(c).begin();
                     it != Expression::ann(c).end(); ++it) {
                  EE ee_ann = flat_exp(env, Ctx(), *it, nullptr, env.constants.varTrue);
                  Expression::addAnnotation(nc, ee_ann.r());
                }
              }
              StringLit* sl = get_longest_mzn_path_annotation(env, c);
              CallStackItem* csi = nullptr;
              if (sl != nullptr) {
                csi = new CallStackItem(env, sl);
              }
              ItemTimer item_timer(Expression::loc(nc), timingMap);
              (void)flat_exp(env, Ctx(), nc, env.constants.varTrue, env.constants.varTrue);
              env.flatRemoveItem(ci);

              delete csi;
            }
          }
        }
      }

      startItem = endItem + 1;
      endItem = static_cast<int>(m.size()) - 1;
    }

    // Add redefinitions for output variables that may have been redefined since create_output
    for (unsigned int i = 0; i < env.output->size(); i++) {
      if (auto* vdi = (*env.output)[i]->dynamicCast<VarDeclI>()) {
        if (vdi->e()->e() == nullptr) {
          auto it = env.reverseMappers.find(vdi->e()->id());
          if (it != env.reverseMappers.end()) {
            GCLock lock;
            Call* rhs = Expression::cast<Call>(copy(env, env.cmap, it->second()));
            check_output_par_fn(env, rhs);
            output_vardecls(env, vdi, rhs);

            remove_is_output(vdi->e()->flat());
            vdi->e()->e(rhs);
          }
        }
      }
    }

    if (!opt.keepOutputInFzn) {
      finalise_output(env);
    }

    for (auto& i : m) {
      if (auto* ci = i->dynamicCast<ConstraintI>()) {
        if (Call* c = Expression::dynamicCast<Call>(ci->e())) {
          if (c->decl() == env.constants.varRedef) {
            env.flatRemoveItem(ci);
          }
        }
      }
    }

    cleanup_output(env);
  } catch (ModelInconsistent&) { /* NOLINT(bugprone-empty-catch) */
  }

  if (opt.detailedTiming) {
    if (opt.encapsulateJSON) {
      auto& os = e.envi().outstream;
      os << "{\"type\": \"profiling\", \"entries\": [";
      bool first = true;
      for (auto& entry : *timingMap) {
        std::chrono::milliseconds time_taken =
            std::chrono::duration_cast<std::chrono::milliseconds>(entry.second);
        if (time_taken > std::chrono::milliseconds(0)) {
          if (first) {
            first = false;
          } else {
            os << ", ";
          }
          os << "{\"filename\": \"" << Printer::escapeStringLit(entry.first.first)
             << "\", \"line\": " << entry.first.second << ", \"time\": " << time_taken.count()
             << "}";
        }
      }
      os << "]}" << std::endl;
    } else {
      StatisticsStream ss(e.envi().outstream, opt.encapsulateJSON);
      e.envi().outstream << "% Compilation profile (file,line,milliseconds)\n";
      if (opt.collectMznPaths) {
        e.envi().outstream << "% (time is allocated to toplevel item)\n";
      } else {
        e.envi().outstream << "% (locations are approximate, use --keep-paths to allocate times to "
                              "toplevel items)\n";
      }
      for (auto& entry : *timingMap) {
        std::chrono::milliseconds time_taken =
            std::chrono::duration_cast<std::chrono::milliseconds>(entry.second);
        if (time_taken > std::chrono::milliseconds(0)) {
          std::ostringstream oss;
          oss << "[\"" << entry.first.first << "\"," << entry.first.second << ","
              << time_taken.count() << "]";
          ss.addRaw("profiling", oss.str());
        }
      }
    }
  }
}

void clear_internal_annotations(EnvI& env, Expression* e, bool keepDefinesVar) {
  auto& ann = Expression::ann(e);
  ann.remove(env.constants.ann.promise_total);
  ann.remove(env.constants.ann.promise_commutative);
  ann.remove(env.constants.ann.maybe_partial);
  ann.remove(env.constants.ann.add_to_output);
  ann.remove(env.constants.ann.output);
  ann.remove(env.constants.ann.rhs_from_assignment);
  ann.remove(env.constants.ann.mzn_was_undefined);
  // Remove defines_var(x) annotation where x is par
  std::vector<Expression*> removeAnns;
  for (ExpressionSetIter anns = ann.begin(); anns != ann.end(); ++anns) {
    if (Call* c = Expression::dynamicCast<Call>(*anns)) {
      if (c->id() == env.constants.ann.defines_var &&
          (!keepDefinesVar || Expression::type(c->arg(0)).isPar())) {
        removeAnns.push_back(c);
      }
    }
  }
  for (auto& removeAnn : removeAnns) {
    ann.remove(removeAnn);
  }
}

std::vector<Expression*> cleanup_vardecl(EnvI& env, VarDeclI* vdi, VarDecl* vd,
                                         bool keepDefinesVar) {
  std::vector<Expression*> added_constraints;

  // In FlatZinc par variables have RHSs, not domains
  if (vd->type().isPar()) {
    Expression::ann(vd).clear();
    vd->introduced(false);
    vd->ti()->eraseDomain();
  }

  // In FlatZinc the RHS of a VarDecl must be a literal, Id or empty
  // Example:
  //   var 1..5: x = function(y)
  // becomes:
  //   var 1..5: x;
  //   relation(x, y);
  if (vd->type().isvar() && vd->type().isbool()) {
    bool is_fixed = (vd->ti()->domain() != nullptr);
    if (Expression::equal(vd->ti()->domain(), env.constants.literalTrue)) {
      // Ex: var true: b = e()

      // Store RHS
      Expression* ve = vd->e();
      vd->e(env.constants.literalTrue);
      vd->ti()->domain(nullptr);
      // Ex: var bool: b = true

      // If vd had a RHS
      if (ve != nullptr) {
        if (Call* vcc = Expression::dynamicCast<Call>(ve)) {
          // Convert functions to relations:
          //   exists([x]) => bool_clause([x],[])
          //   forall([x]) => array_bool_and([x],true)
          //   clause([x],[y]) => bool_clause([x],[y])
          ASTString cid;
          std::vector<Expression*> args;
          if (vcc->id() == env.constants.ids.exists) {
            cid = env.constants.ids.bool_.clause;
            args.push_back(vcc->arg(0));
            args.push_back(env.constants.emptyBoolArray);
          } else if (vcc->id() == env.constants.ids.forall) {
            cid = env.constants.ids.bool_reif.array_and;
            args.push_back(vcc->arg(0));
            args.push_back(env.constants.literalTrue);
          } else if (vcc->id() == env.constants.ids.clause) {
            cid = env.constants.ids.bool_.clause;
            args.push_back(vcc->arg(0));
            args.push_back(vcc->arg(1));
          }

          if (args.empty()) {
            // Post original RHS as stand alone constraint
            ve = vcc;
          } else {
            // Create new call, retain annotations from original RHS
            Call* nc = Call::a(Expression::loc(vcc).introduce(), cid, args);
            nc->type(vcc->type());
            Expression::ann(nc).merge(Expression::ann(vcc));
            ve = nc;
          }
        } else if (Id* id = Expression::dynamicCast<Id>(ve)) {
          if (id->decl()->ti()->domain() != env.constants.literalTrue) {
            // Inconsistent assignment: post bool_eq(y, true)
            std::vector<Expression*> args(2);
            args[0] = id;
            args[1] = env.constants.literalTrue;
            GCLock lock;
            ve = Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
          } else {
            // Don't post this
            ve = env.constants.literalTrue;
          }
        }
        // Post new constraint
        if (ve != env.constants.literalTrue) {
          clear_internal_annotations(env, ve, keepDefinesVar);
          added_constraints.push_back(ve);
        }
      }
    } else {
      // Ex: var false: b = e()
      if (vd->e() != nullptr) {
        if (Expression::eid(vd->e()) == Expression::E_CALL) {
          // Convert functions to relations:
          //  var false: b = exists([x]) => bool_clause_reif([x], [], b)
          //  var false: b = forall([x]) => array_bool_and([x], b)
          //  var false: b = clause([x]) => bool_clause_reif([x], b)
          const Call* c = Expression::cast<Call>(vd->e());
          GCLock lock;
          vd->e(nullptr);
          ASTString cid;
          FunctionI* decl(nullptr);

          std::vector<Expression*> args(c->argCount());
          for (unsigned int i = c->argCount(); (i--) != 0U;) {
            args[i] = c->arg(i);
          }
          if (c->id() == env.constants.ids.exists) {
            args.push_back(env.constants.emptyBoolArray);
          }
          if (is_fixed) {
            args.push_back(env.constants.literalFalse);
          } else {
            args.push_back(vd->id());
          }

          if (c->id() == env.constants.ids.exists || c->id() == env.constants.ids.clause) {
            cid = env.constants.ids.bool_reif.clause;
          } else if (c->id() == env.constants.ids.forall) {
            cid = env.constants.ids.bool_reif.array_and;
          } else {
            bool canHalfReify = env.fopts.enableHalfReification &&
                                Expression::ann(vd).contains(env.constants.ctx.pos);
            decl = env.model->matchReification(env, c->id(), args, canHalfReify, false);
            cid = (decl != nullptr) ? decl->id() : env.reifyId(c->id());
          }
          Call* nc = Call::a(Expression::loc(c).introduce(), cid, args);
          nc->type(c->type());
          if (decl == nullptr) {
            decl = env.model->matchFn(env, nc, false);
          }
          if (decl == nullptr) {
            std::ostringstream ss;
            ss << "'" << demonomorphise_identifier(c->id())
               << "' is used in a reified context but no reified version is available";
            throw FlatteningError(env, Expression::loc(c), ss.str());
          }
          nc->decl(decl);
          if (!is_fixed) {
            make_defined_var(env, vd, nc);
          }
          Expression::ann(nc).merge(Expression::ann(c));
          clear_internal_annotations(env, nc, keepDefinesVar);
          added_constraints.push_back(nc);
        } else {
          assert(Expression::eid(vd->e()) == Expression::E_ID ||
                 Expression::eid(vd->e()) == Expression::E_BOOLLIT);
        }
      }
      if (Expression::equal(vd->ti()->domain(), env.constants.literalFalse)) {
        vd->ti()->domain(nullptr);
        vd->e(env.constants.literalFalse);
      }
    }
    if (vdi != nullptr && is_fixed && env.varOccurrences.occurrences(vd) == 0) {
      if (is_output(vd)) {
        VarDecl* vd_output =
            (*env.output)[env.outputFlatVarOccurrences.find(vd)]->cast<VarDeclI>()->e();
        if (vd_output->e() == nullptr) {
          vd_output->e(vd->e());
        }
      }
      env.flatRemoveItem(vdi);
    }
  } else if (vd->type().isvar() && vd->type().dim() == 0) {
    // Int or Float var
    if (vd->e() != nullptr) {
      if (const Call* cc = Expression::dynamicCast<Call>(vd->e())) {
        // Remove RHS from vd
        vd->e(nullptr);

        std::vector<Expression*> args(cc->argCount());
        ASTString cid;
        if (cc->id() == env.constants.ids.lin_exp) {
          // a = lin_exp([1],[b],5) => int_lin_eq([1,-1],[b,a],-5):: defines_var(a)
          auto* le_c = Expression::cast<ArrayLit>(follow_id(cc->arg(0)));
          std::vector<Expression*> nc(le_c->size());
          for (auto i = static_cast<unsigned int>(nc.size()); (i--) != 0U;) {
            nc[i] = (*le_c)[i];
          }
          if (le_c->type().bt() == Type::BT_INT) {
            cid = env.constants.ids.int_.lin_eq;
            nc.push_back(IntLit::a(-1));
            args[0] = new ArrayLit(Location().introduce(), nc);
            Expression::type(args[0], Type::parint(1));
            auto* le_x = Expression::cast<ArrayLit>(follow_id(cc->arg(1)));
            std::vector<Expression*> nx(le_x->size());
            for (auto i = static_cast<unsigned int>(nx.size()); (i--) != 0U;) {
              nx[i] = (*le_x)[i];
            }
            nx.push_back(vd->id());
            args[1] = new ArrayLit(Location().introduce(), nx);
            Expression::type(args[1], le_x->type());
            IntVal d = IntLit::v(Expression::cast<IntLit>(cc->arg(2)));
            args[2] = IntLit::a(-d);
          } else {
            // float
            cid = env.constants.ids.float_.lin_eq;
            nc.push_back(FloatLit::a(-1.0));
            args[0] = new ArrayLit(Location().introduce(), nc);
            Expression::type(args[0], Type::parfloat(1));
            auto* le_x = Expression::cast<ArrayLit>(follow_id(cc->arg(1)));
            std::vector<Expression*> nx(le_x->size());
            for (auto i = static_cast<unsigned int>(nx.size()); (i--) != 0U;) {
              nx[i] = (*le_x)[i];
            }
            nx.push_back(vd->id());
            args[1] = new ArrayLit(Location().introduce(), nx);
            Expression::type(args[1], le_x->type());
            FloatVal d = FloatLit::v(Expression::cast<FloatLit>(cc->arg(2)));
            args[2] = FloatLit::a(-d);
          }
        } else {
          if (cc->id() == env.constants.ids.card) {
            // card is 'set_card' in old FlatZinc
            cid = env.constants.ids.set_.card;
          } else {
            cid = cc->id();
          }
          for (auto i = static_cast<unsigned int>(args.size()); (i--) != 0U;) {
            args[i] = cc->arg(i);
          }
          args.push_back(vd->id());
        }
        Call* nc = Call::a(Expression::loc(cc).introduce(), cid, args);
        nc->type(cc->type());
        make_defined_var(env, vd, nc);
        Expression::ann(nc).merge(Expression::ann(cc));

        clear_internal_annotations(env, nc, keepDefinesVar);
        added_constraints.push_back(nc);
      } else {
        // RHS must be literal or Id
        assert(Expression::eid(vd->e()) == Expression::E_ID ||
               Expression::eid(vd->e()) == Expression::E_INTLIT ||
               Expression::eid(vd->e()) == Expression::E_FLOATLIT ||
               Expression::eid(vd->e()) == Expression::E_BOOLLIT ||
               Expression::eid(vd->e()) == Expression::E_SETLIT);
      }
    }
  } else if (vd->type().dim() > 0) {
    // vd is an array

    // If RHS is an Id, follow id to RHS
    // a = [1,2,3]; b = a;
    // vd = b => vd = [1,2,3]
    if (!Expression::isa<ArrayLit>(vd->e())) {
      vd->e(follow_id(vd->e()));
    }

    // If empty array or 1 indexed, continue
    if (vd->ti()->ranges().size() == 1 && vd->ti()->ranges()[0]->domain() != nullptr &&
        Expression::isa<SetLit>(vd->ti()->ranges()[0]->domain())) {
      IntSetVal* isv = Expression::cast<SetLit>(vd->ti()->ranges()[0]->domain())->isv();
      if ((isv != nullptr) && (isv->empty() || isv->min(0) == 1)) {
        return added_constraints;
      }
    }

    // Array should be 1 indexed since ArrayLit is 1 indexed
    assert(vd->e() != nullptr);
    ArrayLit* al = nullptr;
    Expression* e = vd->e();
    while (al == nullptr) {
      switch (Expression::eid(e)) {
        case Expression::E_ARRAYLIT:
          al = Expression::cast<ArrayLit>(e);
          break;
        case Expression::E_ID:
          e = Expression::cast<Id>(e)->decl()->e();
          break;
        default:
          assert(false);
      }
    }
    al->make1d();
    IntSetVal* isv = IntSetVal::a(1, al->length());
    if (vd->ti()->ranges().size() == 1) {
      vd->ti()->ranges()[0]->domain(new SetLit(Location().introduce(), isv));
    } else {
      std::vector<TypeInst*> r(1);
      r[0] = new TypeInst(Expression::loc(vd->ti()->ranges()[0]), vd->ti()->ranges()[0]->type(),
                          new SetLit(Location().introduce(), isv));
      ASTExprVec<TypeInst> ranges(r);
      auto* ti =
          new TypeInst(Expression::loc(vd->ti()), vd->ti()->type(), ranges, vd->ti()->domain());
      vd->ti(ti);
    }
  }

  // Remove boolean context annotations used only on compilation
  for (auto* ann : env.constants.internalAnn()) {
    Expression::ann(vd).remove(ann);
  }
  Expression::ann(vd).removeCall(env.constants.ann.mzn_check_enum_var);

  return added_constraints;
}

Expression* cleanup_constraint(EnvI& env, std::unordered_set<Item*>& globals, Expression* ce,
                               bool keepDefinesVar) {
  clear_internal_annotations(env, ce, keepDefinesVar);

  if (Call* vc = Expression::dynamicCast<Call>(ce)) {
    for (unsigned int i = 0; i < vc->argCount(); i++) {
      // Change array indicies to be 1 indexed
      if (auto* al = Expression::dynamicCast<ArrayLit>(vc->arg(i))) {
        if (al->dims() > 1 || al->min(0) != 1) {
          al->make1d();
        }
      }
    }
    // Convert functions to relations:
    //   exists([x]) => bool_clause([x],[])
    //   forall([x]) => array_bool_and([x],true)  // TODO maybe assert false since this shouldn't
    //   really occur clause([x]) => bool_clause([x]) bool_xor([x],[y]) => bool_xor([x],[y],true)
    if (vc->id() == env.constants.ids.exists) {
      GCLock lock;
      vc->id(env.constants.ids.bool_.clause);
      std::vector<Expression*> args(2);
      args[0] = vc->arg(0);
      args[1] = env.constants.emptyBoolArray;
      vc->args(args);
      vc->decl(env.model->matchFn(env, vc, false));
    } else if (vc->id() == env.constants.ids.forall) {
      GCLock lock;
      vc->id(env.constants.ids.bool_reif.array_and);
      std::vector<Expression*> args(2);
      args[0] = vc->arg(0);
      args[1] = env.constants.literalTrue;
      vc->args(args);
      vc->decl(env.model->matchFn(env, vc, false));
    } else if (vc->id() == env.constants.ids.clause) {
      GCLock lock;
      vc->id(env.constants.ids.bool_.clause);
      vc->decl(env.model->matchFn(env, vc, false));
    } else if (vc->id() == env.constants.ids.bool_.ne && vc->argCount() == 2) {
      GCLock lock;
      std::vector<Expression*> args(3);
      args[0] = vc->arg(0);
      args[1] = vc->arg(1);
      args[2] = env.constants.literalTrue;
      vc->args(args);
      vc->decl(env.model->matchFn(env, vc, false));
    }

    // If vc->decl() is a solver builtin and has not been added to the
    // FlatZinc, add it
    if ((vc->decl() != nullptr) && vc->decl() != env.constants.varRedef &&
        !vc->decl()->fromStdLib() &&
        !vc->decl()->ann().contains(env.constants.ann.flatzinc_builtin) &&
        globals.find(vc->decl()) == globals.end()) {
      std::vector<VarDecl*> params(vc->decl()->paramCount());
      for (unsigned int i = 0; i < params.size(); i++) {
        params[i] = vc->decl()->param(i);
      }
      GCLock lock;
      auto* vc_decl_copy = new FunctionI(vc->decl()->loc(), vc->decl()->id(), vc->decl()->ti(),
                                         params, vc->decl()->e());
      env.flatAddItem(vc_decl_copy);
      globals.insert(vc->decl());
    }
    return ce;
  }
  if (Id* id = Expression::dynamicCast<Id>(ce)) {
    // Ex: constraint b; => constraint bool_eq(b, true);
    std::vector<Expression*> args(2);
    args[0] = id;
    args[1] = env.constants.literalTrue;
    GCLock lock;
    return Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
  }
  if (auto* bl = Expression::dynamicCast<BoolLit>(ce)) {
    // Ex: true => delete; false => bool_eq(false, true);
    if (!bl->v()) {
      GCLock lock;
      std::vector<Expression*> args(2);
      args[0] = env.constants.literalFalse;
      args[1] = env.constants.literalTrue;
      Call* neq = Call::a(Location().introduce(), env.constants.ids.bool_.eq, args);
      return neq;
    }
    return nullptr;
  }
  return ce;
}

void oldflatzinc(Env& e) {
  Model* m = e.flat();

  // Check wheter we need to keep defines_var annotations
  bool keepDefinesVar = true;
  {
    GCLock lock;
    Call* c = Call::a(Location().introduce(), "mzn_check_annotate_defines_var", {});
    c->type(Type::parbool());
    FunctionI* fi = e.model()->matchFn(e.envi(), c, true);
    if (fi != nullptr) {
      c->decl(fi);
      keepDefinesVar = eval_bool(e.envi(), c);
    }
  }

  // Check wheter we need to add domain_computed annotations
  bool addDomComputed = false;
  {
    GCLock lock;
    Call* c = Call::a(Location().introduce(), "mzn_check_annotate_computed_domains", {});
    c->type(Type::parbool());
    FunctionI* fi = e.model()->matchFn(e.envi(), c, true);
    if (fi != nullptr) {
      c->decl(fi);
      addDomComputed = eval_bool(e.envi(), c);
    }
  }

  // Mark annotations and optional variables for removal, and clear flags
  for (auto& vdi : m->vardecls()) {
    if (vdi.e()->type().ot() == Type::OT_OPTIONAL || vdi.e()->type().bt() == Type::BT_ANN ||
        vdi.e()->type().structBT()) {
      vdi.remove();
    }
  }

  EnvI& env = e.envi();

  // Predicate declarations of solver builtins
  std::unordered_set<Item*> globals;

  // Variables mapped to the index of the constraint that defines them
  enum DFS_STATUS { DFS_UNKNOWN, DFS_SEEN, DFS_DONE };
  std::unordered_map<VarDecl*, std::pair<int, DFS_STATUS>> definition_map;
  std::vector<VarDecl*> definitions;  // Make iteration over definition_map deterministic

  // Record indices of VarDeclIs with Id RHS for sorting & unification
  std::vector<unsigned int> declsWithIds;

  // Important: items are being added to m while iterating over it.
  // The loop therefore needs to check the size in each iteration to make
  // sure it also handles the new items.
  for (unsigned int i = 0; i < m->size(); i++) {
    if ((*m)[i]->removed()) {
      continue;
    }
    if (auto* vdi = (*m)[i]->dynamicCast<VarDeclI>()) {
      GCLock lock;
      VarDecl* vd = vdi->e();
      std::vector<Expression*> added_constraints =
          cleanup_vardecl(e.envi(), vdi, vd, keepDefinesVar);
      // Record whether this VarDecl is equal to an Id (aliasing)
      if ((vd->e() != nullptr) && Expression::isa<Id>(vd->e())) {
        declsWithIds.push_back(i);
        vdi->e()->payload(-static_cast<int>(i) - 1);
      } else {
        vdi->e()->payload(static_cast<int>(i));
      }
      for (auto* nc : added_constraints) {
        Expression* new_ce = cleanup_constraint(e.envi(), globals, nc, keepDefinesVar);
        if (new_ce != nullptr) {
          e.envi().flatAddItem(new ConstraintI(Location().introduce(), new_ce));
        }
      }
      if (Expression::ann(vd).contains(e.envi().constants.ann.is_defined_var)) {
        if (definition_map.find(vd) == definition_map.end()) {
          // We haven't seen this variable before
          definition_map.insert({vd, {-1, DFS_UNKNOWN}});
          definitions.push_back(vd);
        }
      }
    } else if (auto* ci = (*m)[i]->dynamicCast<ConstraintI>()) {
      Expression* new_ce = cleanup_constraint(e.envi(), globals, ci->e(), keepDefinesVar);
      if (new_ce != nullptr) {
        ci->e(new_ce);
        if (keepDefinesVar) {
          if (Call* defines_var = Expression::ann(new_ce).getCall(env.constants.ann.defines_var)) {
            if (Id* ident = Expression::dynamicCast<Id>(defines_var->arg(0))) {
              auto it = definition_map.find(ident->decl());
              if (it != definition_map.end()) {
                if (it->second.first == -1) {
                  // We've only seen the decl before, but not yet the defining constraint
                  it->second.first = static_cast<int>(i);
                } else {
                  // This is the second definition, remove it
                  Expression::ann(new_ce).removeCall(env.constants.ann.defines_var);
                }
              } else {
                definition_map.insert({ident->decl(), {i, DFS_UNKNOWN}});
                definitions.push_back(ident->decl());
              }
            }
          }
        }
      } else {
        ci->remove();
      }
    } else if (auto* fi = (*m)[i]->dynamicCast<FunctionI>()) {
      if (Let* let = Expression::dynamicCast<Let>(fi->e())) {
        GCLock lock;
        std::vector<Expression*> new_let;
        for (unsigned int i = 0; i < let->let().size(); i++) {
          Expression* let_e = let->let()[i];
          if (auto* vd = Expression::dynamicCast<VarDecl>(let_e)) {
            std::vector<Expression*> added_constraints =
                cleanup_vardecl(e.envi(), nullptr, vd, keepDefinesVar);
            new_let.push_back(vd);
            for (auto* nc : added_constraints) {
              new_let.push_back(nc);
            }
          } else {
            Expression* new_ce = cleanup_constraint(e.envi(), globals, let_e, keepDefinesVar);
            if (new_ce != nullptr) {
              new_let.push_back(new_ce);
            }
          }
        }
        fi->e(new Let(Expression::loc(let), new_let, let->in()));
      }
    } else if (auto* si = (*m)[i]->dynamicCast<SolveI>()) {
      if ((si->e() != nullptr) && Expression::type(si->e()).isPar()) {
        // Introduce VarDecl if objective expression is par
        GCLock lock;
        auto* ti = new TypeInst(Location().introduce(), Expression::type(si->e()), nullptr);
        auto* constantobj = new VarDecl(Location().introduce(), ti, e.envi().genId(), si->e());
        si->e(constantobj->id());
        e.envi().flatAddItem(VarDeclI::a(Location().introduce(), constantobj));
      }
    }
  }

  if (keepDefinesVar) {
    // Detect and break cycles in defines_var annotations
    std::vector<VarDecl*> definesStack;
    auto checkId = [&definesStack, &definition_map, &m](VarDecl* cur, Id* ident) {
      if (cur == ident->decl()) {
        // Never push the variable we're currently looking at
        return;
      }
      auto it = definition_map.find(ident->decl());
      if (it != definition_map.end()) {
        if (it->second.second == DFS_UNKNOWN) {
          // not yet visited, push
          definesStack.push_back(it->first);
        } else if (it->second.second == DFS_SEEN) {
          // Found a cycle through variable ident
          // Break cycle by removing annotations
          Expression::ann(ident->decl()).remove(Constants::constants().ann.is_defined_var);
          Call* c = Expression::cast<Call>((*m)[it->second.first]->cast<ConstraintI>()->e());
          Expression::ann(c).removeCall(Constants::constants().ann.defines_var);
        }
      }
    };
    for (auto* it : definitions) {
      if (definition_map[it].second == DFS_UNKNOWN) {
        // not yet visited
        definesStack.push_back(it);
        while (!definesStack.empty()) {
          VarDecl* cur = definesStack.back();
          if (definition_map[cur].second != DFS_UNKNOWN) {
            // already visited (or already finished), now finished
            definition_map[cur].second = DFS_DONE;
            definesStack.pop_back();
          } else {
            // now visited and on stack
            definition_map[cur].second = DFS_SEEN;
            if (definition_map[cur].first == -1) {
              // No associated call, remove annotation
              Expression::ann(cur).remove(Constants::constants().ann.is_defined_var);
            } else if (Call* c = Expression::dynamicCast<Call>(
                           (*m)[definition_map[cur].first]->cast<ConstraintI>()->e())) {
              // Variable is defined by a call, push all arguments
              unsigned int count_cur = 0;
              for (unsigned int i = 0; i < c->argCount(); i++) {
                if (Expression::type(c->arg(i)).isPar()) {
                  continue;
                }
                if (Id* ident = Expression::dynamicCast<Id>(c->arg(i))) {
                  if (ident->type().dim() > 0) {
                    if (auto* al = Expression::dynamicCast<ArrayLit>(ident->decl()->e())) {
                      for (auto* e : al->getVec()) {
                        if (auto* ident = Expression::dynamicCast<Id>(e)) {
                          if (cur == ident->decl()) {
                            count_cur++;
                          }
                          checkId(cur, ident);
                        }
                      }
                    }
                  } else if (ident->type().isvar()) {
                    if (cur == ident->decl()) {
                      count_cur++;
                    }
                    checkId(cur, ident);
                  }
                } else if (auto* al = Expression::dynamicCast<ArrayLit>(c->arg(i))) {
                  for (auto* e : al->getVec()) {
                    if (auto* ident = Expression::dynamicCast<Id>(e)) {
                      if (cur == ident->decl()) {
                        count_cur++;
                      }
                      checkId(cur, ident);
                    }
                  }
                }
              }
              if (count_cur != 1) {
                // We've seen the defined variable 0 times or more than once,
                // so this call cannot define the variable
                Expression::ann(cur).remove(Constants::constants().ann.is_defined_var);
                Expression::ann(c).removeCall(Constants::constants().ann.defines_var);
              }
            }
          }
        }
      }
    }
  }

  // Sort VarDecls in FlatZinc so that VarDecls are declared before use
  std::vector<VarDeclI*> sortedVarDecls(declsWithIds.size());
  int vdCount = 0;
  for (auto declsWithId : declsWithIds) {
    VarDecl* cur = (*m)[declsWithId]->cast<VarDeclI>()->e();
    std::vector<int> stack;
    while ((cur != nullptr) && cur->payload() < 0) {
      stack.push_back(cur->payload());
      if (Id* id = Expression::dynamicCast<Id>(cur->e())) {
        cur = id->decl();
      } else {
        cur = nullptr;
      }
    }
    for (auto i = static_cast<unsigned int>(stack.size()); (i--) != 0U;) {
      auto* vdi = (*m)[-stack[i] - 1]->cast<VarDeclI>();
      vdi->e()->payload(-vdi->e()->payload() - 1);
      sortedVarDecls[vdCount++] = vdi;
    }
  }
  for (unsigned int i = 0; i < declsWithIds.size(); i++) {
    (*m)[declsWithIds[i]] = sortedVarDecls[i];
  }

  // Remove marked items
  m->compact();
  e.envi().output->compact();

  // Add computed domain annotations
  if (addDomComputed) {
    for (auto& vdi : m->vardecls()) {
      auto* vd = vdi.e();
      if (vd->ti()->computedDomain() && vd->ti()->domain() != nullptr) {
        Expression::addAnnotation(vd, e.envi().constants.ann.computed_domain);
      }
    }
  }

  for (auto& it : env.varOccurrences.itemMap) {
    VarOccurrences::Items keptItems;
    for (auto* iit : it) {
      if (!iit->removed()) {
        keptItems.insert(iit);
      }
    }
    it = keptItems;
  }

  class Cmp {
  public:
    bool operator()(Item* i, Item* j) {
      if (i == j) {
        return false;
      }
      if (i->iid() == Item::II_FUN || j->iid() == Item::II_FUN) {
        if (i->iid() == j->iid()) {
          return false;
        }
        return i->iid() == Item::II_FUN;
      }
      if (i->iid() == Item::II_SOL) {
        assert(j->iid() != i->iid());
        return false;
      }
      if (j->iid() == Item::II_SOL) {
        assert(j->iid() != i->iid());
        return true;
      }
      if (i->iid() == Item::II_VD) {
        if (j->iid() != i->iid()) {
          return true;
        }
        if (i->cast<VarDeclI>()->e()->type().isPar() && j->cast<VarDeclI>()->e()->type().isvar()) {
          return true;
        }
        if (j->cast<VarDeclI>()->e()->type().isPar() && i->cast<VarDeclI>()->e()->type().isvar()) {
          return false;
        }
        if (i->cast<VarDeclI>()->e()->type().dim() == 0 &&
            j->cast<VarDeclI>()->e()->type().dim() != 0) {
          return true;
        }
        if (i->cast<VarDeclI>()->e()->type().dim() != 0 &&
            j->cast<VarDeclI>()->e()->type().dim() == 0) {
          return false;
        }
        if (i->cast<VarDeclI>()->e()->e() == nullptr && j->cast<VarDeclI>()->e()->e() != nullptr) {
          return true;
        }
        if ((i->cast<VarDeclI>()->e()->e() != nullptr) &&
            (j->cast<VarDeclI>()->e()->e() != nullptr) &&
            !Expression::isa<Id>(i->cast<VarDeclI>()->e()->e()) &&
            Expression::isa<Id>(j->cast<VarDeclI>()->e()->e())) {
          return true;
        }
      }
      return false;
    }
  } _cmp;
  // Perform final sorting
  std::stable_sort(m->begin(), m->end(), _cmp);
}

FlatModelStatistics statistics(Env& m) {
  Model* flat = m.flat();
  FlatModelStatistics stats;
  stats.n_reif_ct = m.envi().counters.reifConstraints;
  stats.n_imp_ct = m.envi().counters.impConstraints;
  stats.n_imp_del = m.envi().counters.impDel;
  stats.n_lin_del = m.envi().counters.linDel;
  for (auto& i : *flat) {
    if (!i->removed()) {
      if (auto* vdi = i->dynamicCast<VarDeclI>()) {
        Type t = vdi->e()->type();
        if (t.isvar() && t.dim() == 0) {
          if (t.isSet()) {
            stats.n_set_vars++;
          } else if (t.isint()) {
            stats.n_int_vars++;
          } else if (t.isbool()) {
            stats.n_bool_vars++;
          } else if (t.isfloat()) {
            stats.n_float_vars++;
          }
        }
      } else if (auto* ci = i->dynamicCast<ConstraintI>()) {
        if (Call* call = Expression::dynamicCast<Call>(ci->e())) {
          if (call->id().endsWith("_reif")) {
            stats.n_reif_ct++;
          } else if (call->id().endsWith("_imp")) {
            stats.n_imp_ct++;
          }
          if (call->argCount() > 0) {
            Type all_t;
            for (unsigned int i = 0; i < call->argCount(); i++) {
              Type t = Expression::type(call->arg(i));
              if (t.isvar()) {
                if (t.st() == Type::ST_SET ||
                    (t.bt() == Type::BT_FLOAT && all_t.st() != Type::ST_SET) ||
                    (t.bt() == Type::BT_INT && all_t.bt() != Type::BT_FLOAT &&
                     all_t.st() != Type::ST_SET) ||
                    (t.bt() == Type::BT_BOOL && all_t.bt() != Type::BT_INT &&
                     all_t.bt() != Type::BT_FLOAT && all_t.st() != Type::ST_SET)) {
                  all_t = t;
                }
              }
            }
            if (all_t.isvar()) {
              if (all_t.st() == Type::ST_SET) {
                stats.n_set_ct++;
              } else if (all_t.bt() == Type::BT_INT) {
                stats.n_int_ct++;
              } else if (all_t.bt() == Type::BT_BOOL) {
                stats.n_bool_ct++;
              } else if (all_t.bt() == Type::BT_FLOAT) {
                stats.n_float_ct++;
              }
            }
          }
        }
      }
    }
  }
  return stats;
}

ArrayLit* field_slice(EnvI& env, StructType* st, ArrayLit* al,
                      std::vector<std::pair<int, int>> dims, unsigned int field) {
  assert(GC::locked());
  assert(Expression::type(al).structBT() && Expression::type(al).dim() > 0);
  Type field_ty = (*st)[field - 1];
  // TODO: This could be done efficiently using slicing (if we change the memory layout for
  // arrays of tuples)
  std::vector<Expression*> tmp(al->size());
  for (unsigned int i = 0; i < al->size(); ++i) {
    tmp[i] = new FieldAccess(Expression::loc((*al)[i]).introduce(), (*al)[i], IntLit::a(field));
    Expression::type(tmp[i], field_ty);
  }
  auto* field_slice = new ArrayLit(Expression::loc(al).introduce(), tmp, dims);
  Expression::type(field_slice, Type::arrType(env, al->type(), field_ty));
  return field_slice;
}

std::vector<Expression*> field_slices(EnvI& env, Expression* arrExpr) {
  assert(GC::locked());
  assert(Expression::type(arrExpr).structBT() && Expression::type(arrExpr).dim() > 0);
  ArrayLit* al = eval_array_lit(env, arrExpr);

  StructType* st = env.getStructType(al->type());
  std::vector<std::pair<int, int>> dims(al->dims());
  for (unsigned int i = 0; i < al->dims(); i++) {
    dims[i] = std::make_pair(al->min(i), al->max(i));
  }

  std::vector<Expression*> field_al(st->size());
  for (unsigned int i = 0; i < st->size(); ++i) {
    field_al[i] = field_slice(env, st, al, dims, i + 1);
  }
  return field_al;
}

void cse_result_change_ctx(EnvI& env, Expression* cseRes, BCtx newCtx) {
  std::vector<Expression*> stack({cseRes});
  bool first = true;

  while (!stack.empty()) {
    Expression* back = stack.back();
    stack.pop_back();
    auto* ident = Expression::dynamicCast<Id>(back);
    if (ident != nullptr && ident->decl() != nullptr && ident->decl()->e() != nullptr) {
      if (auto* al = Expression::dynamicCast<ArrayLit>(ident->decl()->e())) {
        for (unsigned int i = 0; i < al->size(); i++) {
          stack.push_back((*al)[i]);
        }
      } else if (auto* redirect = Expression::dynamicCast<Id>(ident->decl()->e())) {
        stack.push_back(redirect);
      } else if (auto* c = Expression::dynamicCast<Call>(ident->decl()->e())) {
        BCtx vdCtx;
        bool annotated;
        std::tie(vdCtx, annotated) = env.annToCtx(ident->decl());
        if ((!first || vdCtx != newCtx) && vdCtx != C_ROOT && vdCtx != C_MIX) {
          env.addCtxAnn(ident->decl(), (first && newCtx == C_ROOT) ? C_ROOT : C_MIX);
          for (unsigned int i = 0; i < c->argCount(); i++) {
            if (auto* al = Expression::dynamicCast<ArrayLit>(c->arg(i))) {
              for (unsigned int i = 0; i < al->size(); i++) {
                stack.push_back((*al)[i]);
              }
            } else if (auto* arg = Expression::dynamicCast<Id>(c->arg(i))) {
              stack.push_back(redirect);
            }
          }
        }
      }
    }
    first = false;
  }
}

}  // namespace MiniZinc
