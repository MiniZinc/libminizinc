/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/optimize_constraints.hh>

namespace MiniZinc {

void OptimizeRegistry::reg(const MiniZinc::ASTString& call, optimizer opt) {
  _m.insert(std::make_pair(call, opt));
}

OptimizeRegistry::ConstraintStatus OptimizeRegistry::process(EnvI& env, MiniZinc::Item* i,
                                                             MiniZinc::Call* c,
                                                             Expression*& rewrite) {
  auto it = _m.find(c->id());
  if (it != _m.end()) {
    return it->second(env, i, c, rewrite);
  }
  return CS_NONE;
}

OptimizeRegistry& OptimizeRegistry::registry() {
  static OptimizeRegistry reg;
  return reg;
}

namespace Optimizers {

OptimizeRegistry::ConstraintStatus o_linear(EnvI& env, Item* ii, Call* c, Expression*& rewrite) {
  ArrayLit* al_c = eval_array_lit(env, c->arg(0));
  std::vector<IntVal> coeffs(al_c->size());
  for (unsigned int i = 0; i < al_c->size(); i++) {
    coeffs[i] = eval_int(env, (*al_c)[i]);
  }
  ArrayLit* al_x = eval_array_lit(env, c->arg(1));
  std::vector<KeepAlive> x(al_x->size());
  for (unsigned int i = 0; i < al_x->size(); i++) {
    x[i] = (*al_x)[i];
  }
  IntVal d = 0;
  simplify_lin<IntLit>(coeffs, x, d);
  if (coeffs.empty()) {
    bool failed;
    if (c->id() == constants().ids.int_.lin_le) {
      failed = (d > eval_int(env, c->arg(2)));
    } else if (c->id() == constants().ids.int_.lin_eq) {
      failed = (d != eval_int(env, c->arg(2)));
    } else {
      failed = (d == eval_int(env, c->arg(2)));
    }
    if (failed) {
      return OptimizeRegistry::CS_FAILED;
    }
    return OptimizeRegistry::CS_ENTAILED;
  }
  if (coeffs.size() == 1 && (ii->isa<ConstraintI>() || ii->cast<VarDeclI>()->e()->ti()->domain() ==
                                                           constants().literalTrue)) {
    VarDecl* vd = x[0]()->cast<Id>()->decl();
    IntSetVal* domain =
        vd->ti()->domain() != nullptr ? eval_intset(env, vd->ti()->domain()) : nullptr;
    if (c->id() == constants().ids.int_.lin_eq) {
      IntVal rd = eval_int(env, c->arg(2)) - d;
      if (rd % coeffs[0] == 0) {
        IntVal nd = rd / coeffs[0];
        if ((domain != nullptr) && !domain->contains(nd)) {
          return OptimizeRegistry::CS_FAILED;
        }
        std::vector<Expression*> args(2);
        args[0] = x[0]();
        args[1] = IntLit::a(nd);
        Call* nc = new Call(Location(), constants().ids.int_.eq, args);
        nc->type(Type::varbool());
        rewrite = nc;
        return OptimizeRegistry::CS_REWRITE;
      }
      return OptimizeRegistry::CS_FAILED;
    }
    if (c->id() == constants().ids.int_.lin_le) {
      IntVal ac = std::abs(coeffs[0]);
      IntVal rd = eval_int(env, c->arg(2)) - d;
      IntVal ad = std::abs(rd);
      IntVal nd;
      if (ad % ac == 0) {
        nd = rd / coeffs[0];
      } else {
        double nd_d = static_cast<double>(ad.toInt()) / static_cast<double>(ac.toInt());
        if (coeffs[0] >= 0 && rd >= 0) {
          nd = static_cast<long long int>(std::floor(nd_d));
        } else if (rd >= 0) {
          nd = -static_cast<long long int>(std::floor(nd_d));
        } else if (coeffs[0] >= 0) {
          nd = -static_cast<long long int>(std::ceil(nd_d));
        } else {
          nd = static_cast<long long int>(std::ceil(nd_d));
        }
      }
      bool swapSign = coeffs[0] < 0;
      if (domain != nullptr) {
        if (swapSign) {
          if (domain->max() < nd) {
            return OptimizeRegistry::CS_FAILED;
          }
          if (domain->min() >= nd) {
            return OptimizeRegistry::CS_ENTAILED;
          }
        } else {
          if (domain->min() > nd) {
            return OptimizeRegistry::CS_FAILED;
          }
          if (domain->max() <= nd) {
            return OptimizeRegistry::CS_ENTAILED;
          }
        }
        std::vector<Expression*> args(2);
        args[0] = x[0]();
        args[1] = IntLit::a(nd);
        if (swapSign) {
          std::swap(args[0], args[1]);
        }
        Call* nc = new Call(Location(), constants().ids.int_.le, args);
        nc->type(Type::varbool());
        rewrite = nc;
        return OptimizeRegistry::CS_REWRITE;
      }
    }
  } else if (c->id() == constants().ids.int_.lin_eq && coeffs.size() == 2 &&
             ((coeffs[0] == 1 && coeffs[1] == -1) || (coeffs[1] == 1 && coeffs[0] == -1)) &&
             eval_int(env, c->arg(2)) - d == 0) {
    std::vector<Expression*> args(2);
    args[0] = x[0]();
    args[1] = x[1]();
    Call* nc = new Call(Location(), constants().ids.int_.eq, args);
    rewrite = nc;
    return OptimizeRegistry::CS_REWRITE;
  }
  if (coeffs.size() < al_c->size()) {
    std::vector<Expression*> coeffs_e(coeffs.size());
    std::vector<Expression*> x_e(coeffs.size());
    for (unsigned int i = 0; i < coeffs.size(); i++) {
      coeffs_e[i] = IntLit::a(coeffs[i]);
      x_e[i] = x[i]();
    }
    auto* al_c_new = new ArrayLit(al_c->loc(), coeffs_e);
    al_c_new->type(Type::parint(1));
    auto* al_x_new = new ArrayLit(al_x->loc(), x_e);
    al_x_new->type(al_x->type());

    std::vector<Expression*> args(3);
    args[0] = al_c_new;
    args[1] = al_x_new;
    args[2] = IntLit::a(eval_int(env, c->arg(2)) - d);
    Call* nc = new Call(Location(), c->id(), args);
    nc->type(Type::varbool());
    for (ExpressionSetIter it = c->ann().begin(); it != c->ann().end(); ++it) {
      nc->addAnnotation(*it);
    }

    rewrite = nc;
    return OptimizeRegistry::CS_REWRITE;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_lin_exp(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->type().isint()) {
    ArrayLit* al_c = eval_array_lit(env, c->arg(0));
    std::vector<IntVal> coeffs(al_c->size());
    for (unsigned int j = 0; j < al_c->size(); j++) {
      coeffs[j] = eval_int(env, (*al_c)[j]);
    }
    ArrayLit* al_x = eval_array_lit(env, c->arg(1));
    std::vector<KeepAlive> x(al_x->size());
    for (unsigned int j = 0; j < al_x->size(); j++) {
      x[j] = (*al_x)[j];
    }
    IntVal d = eval_int(env, c->arg(2));
    simplify_lin<IntLit>(coeffs, x, d);
    if (coeffs.empty()) {
      rewrite = IntLit::a(d);
      return OptimizeRegistry::CS_REWRITE;
    }
    if (coeffs.size() < al_c->size()) {
      if (coeffs.size() == 1 && coeffs[0] == 1 && d == 0) {
        rewrite = x[0]();
        return OptimizeRegistry::CS_REWRITE;
      }

      std::vector<Expression*> coeffs_e(coeffs.size());
      std::vector<Expression*> x_e(coeffs.size());
      for (unsigned int j = 0; j < coeffs.size(); j++) {
        coeffs_e[j] = IntLit::a(coeffs[j]);
        x_e[j] = x[j]();
      }
      auto* al_c_new = new ArrayLit(al_c->loc(), coeffs_e);
      al_c_new->type(Type::parint(1));
      auto* al_x_new = new ArrayLit(al_x->loc(), x_e);
      al_x_new->type(al_x->type());

      std::vector<Expression*> args(3);
      args[0] = al_c_new;
      args[1] = al_x_new;
      args[2] = IntLit::a(d);
      Call* nc = new Call(Location(), c->id(), args);
      nc->type(c->type());
      for (ExpressionSetIter it = c->ann().begin(); it != c->ann().end(); ++it) {
        nc->addAnnotation(*it);
      }
      rewrite = nc;
      return OptimizeRegistry::CS_REWRITE;
    }
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_element(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->arg(0)->isa<IntLit>()) {
    IntVal idx = eval_int(env, c->arg(0));
    ArrayLit* al = eval_array_lit(env, c->arg(1));
    if (idx < 1 || idx > al->size()) {
      return OptimizeRegistry::CS_FAILED;
    }
    Expression* result = (*al)[static_cast<int>(idx.toInt()) - 1];
    std::vector<Expression*> args(2);
    args[0] = result;
    args[1] = c->arg(2);
    Call* eq = new Call(Location(), constants().ids.int_.eq, args);
    rewrite = eq;
    return OptimizeRegistry::CS_REWRITE;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_clause(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  std::vector<VarDecl*> pos;
  std::vector<VarDecl*> neg;
  ArrayLit* al_pos = eval_array_lit(env, c->arg(0));
  for (unsigned int j = 0; j < al_pos->size(); j++) {
    if (Id* ident = (*al_pos)[j]->dynamicCast<Id>()) {
      if (ident->decl()->ti()->domain() == nullptr) {
        pos.push_back(ident->decl());
      }
    }
  }
  ArrayLit* al_neg = eval_array_lit(env, c->arg(1));
  for (unsigned int j = 0; j < al_neg->size(); j++) {
    if (Id* ident = (*al_neg)[j]->dynamicCast<Id>()) {
      if (ident->decl()->ti()->domain() == nullptr) {
        neg.push_back(ident->decl());
      }
    }
  }
  bool subsumed = false;
  if (!pos.empty() && !neg.empty()) {
    std::sort(pos.begin(), pos.end());
    std::sort(neg.begin(), neg.end());
    unsigned int ix = 0;
    unsigned int iy = 0;
    for (;;) {
      if (pos[ix] == neg[iy]) {
        subsumed = true;
        break;
      }
      if (pos[ix] < neg[iy]) {
        ix++;
      } else {
        iy++;
      }
      if (ix == pos.size() || iy == neg.size()) {
        break;
      }
    }
  }
  if (subsumed) {
    return OptimizeRegistry::CS_ENTAILED;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_not(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->argCount() == 2) {
    Expression* e0 = c->arg(0);
    Expression* e1 = c->arg(1);
    if (e0->type().isPar() && e1->type().isPar()) {
      return eval_bool(env, e0) == eval_bool(env, e1) ? OptimizeRegistry::CS_FAILED
                                                      : OptimizeRegistry::CS_ENTAILED;
    }
    if (e1->type().isPar()) {
      std::swap(e0, e1);
    }
    if (e0->type().isPar()) {
      Call* eq = new Call(Location(), constants().ids.bool_eq,
                          {e1, constants().boollit(!eval_bool(env, e0))});
      rewrite = eq;
      return OptimizeRegistry::CS_REWRITE;
    }
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_div(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->arg(1)->type().isPar()) {
    IntVal c1v = eval_int(env, c->arg(1));
    if (c->arg(0)->type().isPar() && c->argCount() == 3 && c->arg(2)->type().isPar()) {
      IntVal c0v = eval_int(env, c->arg(0));
      IntVal c2v = eval_int(env, c->arg(2));
      return (c0v / c1v == c2v) ? OptimizeRegistry::CS_ENTAILED : OptimizeRegistry::CS_FAILED;
    }
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_times(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  Expression* result = nullptr;
  Expression* arg0 = c->arg(0);
  Expression* arg1 = c->arg(1);
  if (arg0->type().isPar() && arg1->type().isPar()) {
    IntVal c0v = eval_int(env, arg0);
    IntVal c1v = eval_int(env, arg1);
    result = IntLit::a(c0v * c1v);
  } else if (arg0->type().isPar()) {
    IntVal c0v = eval_int(env, arg0);
    if (c0v == 0) {
      result = IntLit::a(0);
    } else if (c0v == 1) {
      result = arg1;
    }
  } else if (arg1->type().isPar()) {
    IntVal c1v = eval_int(env, arg1);
    if (c1v == 0) {
      result = IntLit::a(0);
    }
    if (c1v == 1) {
      result = arg0;
    }
  }

  if (result != nullptr) {
    if (c->argCount() == 2) {
      // this is the functional version of times
      rewrite = result;
      return OptimizeRegistry::CS_REWRITE;
    }  // this is the relational version of times
    assert(c->argCount() == 3);
    rewrite = new Call(Location().introduce(), constants().ids.int_.eq, {c->arg(2), result});
    return OptimizeRegistry::CS_REWRITE;
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_set_in(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  if (c->arg(1)->type().isPar()) {
    if (c->arg(0)->type().isPar()) {
      IntSetVal* isv = eval_intset(env, c->arg(1));
      return isv->contains(eval_int(env, c->arg(0))) ? OptimizeRegistry::CS_ENTAILED
                                                     : OptimizeRegistry::CS_FAILED;
    }
    if (Id* ident = c->arg(0)->dynamicCast<Id>()) {
      VarDecl* vd = ident->decl();
      IntSetVal* isv = eval_intset(env, c->arg(1));
      if (vd->ti()->domain() != nullptr) {
        IntSetVal* dom = eval_intset(env, vd->ti()->domain());
        {
          IntSetRanges isv_r(isv);
          IntSetRanges dom_r(dom);
          if (Ranges::subset(dom_r, isv_r)) {
            return OptimizeRegistry::CS_ENTAILED;
          }
        }
        {
          IntSetRanges isv_r(isv);
          IntSetRanges dom_r(dom);
          if (Ranges::disjoint(dom_r, isv_r)) {
            return OptimizeRegistry::CS_FAILED;
          }
        }
      } else if (isv->min() == isv->max()) {
        std::vector<Expression*> args(2);
        args[0] = vd->id();
        args[1] = IntLit::a(isv->min());
        Call* eq = new Call(Location(), constants().ids.int_.eq, args);
        rewrite = eq;
        return OptimizeRegistry::CS_REWRITE;
      }
    }
  }
  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_int_ne(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  Expression* e0 = c->arg(0);
  Expression* e1 = c->arg(1);
  if (e0->type().isPar() && e1->type().isPar()) {
    return eval_int(env, e0) != eval_int(env, e1) ? OptimizeRegistry::CS_ENTAILED
                                                  : OptimizeRegistry::CS_FAILED;
  }
  if (e1->isa<Id>()) {
    std::swap(e0, e1);
  }
  if (Id* ident = e0->dynamicCast<Id>()) {
    if (e1->type().isPar()) {
      if (ident->decl()->ti()->domain() != nullptr) {
        IntVal e1v = eval_int(env, e1);
        IntSetVal* isv = eval_intset(env, ident->decl()->ti()->domain());
        if (!isv->contains(e1v)) {
          return OptimizeRegistry::CS_ENTAILED;
        }
        if (e1v == isv->min() && e1v == isv->max()) {
          return OptimizeRegistry::CS_FAILED;
        }
      }
    }
  }

  return OptimizeRegistry::CS_OK;
}

OptimizeRegistry::ConstraintStatus o_int_le(EnvI& env, Item* i, Call* c, Expression*& rewrite) {
  Expression* e0 = c->arg(0);
  Expression* e1 = c->arg(1);
  if (e0->type().isPar() && e1->type().isPar()) {
    return eval_int(env, e0) <= eval_int(env, e1) ? OptimizeRegistry::CS_ENTAILED
                                                  : OptimizeRegistry::CS_FAILED;
  }
  bool swapped = false;
  if (e1->isa<Id>()) {
    std::swap(e0, e1);
    swapped = true;
  }
  if (Id* ident = e0->dynamicCast<Id>()) {
    if (e1->type().isPar()) {
      if (ident->decl()->ti()->domain() != nullptr) {
        IntVal e1v = eval_int(env, e1);
        IntSetVal* isv = eval_intset(env, ident->decl()->ti()->domain());
        if (!swapped) {
          if (isv->max() <= e1v) {
            return OptimizeRegistry::CS_ENTAILED;
          }
          if (isv->min() > e1v) {
            return OptimizeRegistry::CS_FAILED;
          }
        } else {
          if (e1v <= isv->min()) {
            return OptimizeRegistry::CS_ENTAILED;
          }
          if (e1v > isv->max()) {
            return OptimizeRegistry::CS_FAILED;
          }
        }
      }
    }
  }

  return OptimizeRegistry::CS_OK;
}

class Register {
private:
  Model* _keepAliveModel;

public:
  Register() {
    GCLock lock;
    _keepAliveModel = new Model;
    ASTString id_element("array_int_element");
    ASTString id_var_element("array_var_int_element");
    std::vector<Expression*> e;
    e.push_back(new StringLit(Location(), id_element));
    e.push_back(new StringLit(Location(), id_var_element));
    _keepAliveModel->addItem(new ConstraintI(Location(), new ArrayLit(Location(), e)));
    OptimizeRegistry::registry().reg(constants().ids.int_.lin_eq, o_linear);
    OptimizeRegistry::registry().reg(constants().ids.int_.lin_le, o_linear);
    OptimizeRegistry::registry().reg(constants().ids.int_.lin_ne, o_linear);
    OptimizeRegistry::registry().reg(constants().ids.int_.div, o_div);
    OptimizeRegistry::registry().reg(constants().ids.int_.times, o_times);
    OptimizeRegistry::registry().reg(id_element, o_element);
    OptimizeRegistry::registry().reg(constants().ids.lin_exp, o_lin_exp);
    OptimizeRegistry::registry().reg(id_var_element, o_element);
    OptimizeRegistry::registry().reg(constants().ids.clause, o_clause);
    OptimizeRegistry::registry().reg(constants().ids.bool_clause, o_clause);
    OptimizeRegistry::registry().reg(constants().ids.bool_not, o_not);
    OptimizeRegistry::registry().reg(constants().ids.set_in, o_set_in);
    OptimizeRegistry::registry().reg(constants().ids.int_.ne, o_int_ne);
    OptimizeRegistry::registry().reg(constants().ids.int_.le, o_int_le);
  }
  ~Register() { delete _keepAliveModel; }
} _r;

}  // namespace Optimizers

}  // namespace MiniZinc
