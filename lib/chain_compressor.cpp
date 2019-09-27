/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/chain_compressor.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/ast.hh>

namespace MiniZinc {

  void ChainCompressor::removeItem(Item *i) {
    CollectDecls cd(env.vo, deletedVarDecls, i);
    if (auto ci = i->dyn_cast<ConstraintI>()) {
      topDown(cd, ci->e());
    } else if (auto vdi = i->dyn_cast<VarDeclI>()) {
      topDown(cd, vdi->e());
    } else {
      assert(false); // CURRENTLY NOT SUPPORTED
    }
    env.flat_removeItem(i);
  }

  int ChainCompressor::addItem(Item *i) {
    env.flat_addItem(i);
    int item_idx = env.flat()->size()-1;
    trackItem(i);
    return item_idx;
  }

  void ChainCompressor::updateCount() {
    for (auto it = this->items.begin(); it != items.end();) {
      if (it->second->removed()) {
        it = this->items.erase(it);
      } else {
        ++it;
      }
    }
  }

  void ChainCompressor::replaceCallArgument(Item *i, Call *c, unsigned int n, Expression *e) {
    CollectDecls cd(env.vo, deletedVarDecls, i);
    topDown(cd, c->arg(n));
    c->arg(n, e);
    CollectOccurrencesE ce(env.vo, i);
    topDown(ce, e);
  }

  bool ImpCompressor::trackItem(Item *i) {
    if (i->removed()) {
      return false;
    }
    if (auto ci = i->dyn_cast<ConstraintI>()) {
      if (auto c = ci->e()->dyn_cast<Call>()) {
        // clause([y], [x]); i.e. x -> y
        if (c->id() == constants().ids.clause) {
          auto positive = c->arg(0)->cast<ArrayLit>();
          auto negative = c->arg(1)->cast<ArrayLit>();
          if (positive->length() == 1 && negative->length() == 1) {
            auto var = (*negative)[0]->cast<Id>();
            storeItem(var->decl(), i);
            return true;
          }
        } else if (c->id() == "mzn_reverse_map_var") {
          auto control = c->arg(0)->cast<Id>();
          assert(control->type().isvarbool());
          storeItem(control->decl(), i);
          return true;
          // pred_imp(..., b); i.e. b -> pred(...)
        } else if (c->id().endsWith("_imp")) {
          auto control = c->arg(c->n_args()-1)->cast<Id>();
          assert(control->type().isvarbool());
          storeItem(control->decl(), i);
          return true;
        }
      }
    } else if (auto vdi = i->dyn_cast<VarDeclI>()) {
      if (vdi->e()->type().isvarbool() && vdi->e() && vdi->e()->e()) {
        if (auto c = vdi->e()->e()->dyn_cast<Call>()) {
          // x = forall([y,z,...]); potentially: x -> (y /\ z /\ ...)
          if (c->id() == constants().ids.forall) {
            storeItem(vdi->e(), i);
            return true;
          // x ::ctx_pos = pred(...); potentially: pred_imp(..., x); i.e. x -> pred(...)
          } else if (env.fopts.enable_imp && vdi->e()->ann().contains(constants().ctx.pos)) {
            GCLock lock;
            auto cid = env.halfReifyId(c->id());
            std::vector<Type> args;
            args.reserve(c->n_args() +1);
            for (int j = 0; j < c->n_args(); ++j) {
              args.push_back(c->arg(j)->type());
            }
            args.push_back(Type::varbool());
            FunctionI* decl = env.model->matchFn(env,cid,args,false);

            if (decl) {
              storeItem(vdi->e(), i);
              return true;
            }
          }
        }
      }
    }
    return false;
  }

  void ImpCompressor::compress() {
    for (auto it = items.begin(); it != items.end();) {
      VarDecl *lhs = nullptr;
      VarDecl *rhs = nullptr;
      // Check if compression is possible
      if (auto ci = it->second->dyn_cast<ConstraintI>()) {
        auto c = ci->e()->cast<Call>();
        if (c->id() == constants().ids.clause) {
          auto positive = c->arg(0)->cast<ArrayLit>();
          auto var = (*positive)[0]->cast<Id>();
          bool output_var = var->decl()->ann().contains(constants().ann.output_var);
          auto usages = env.vo.usages(var->decl());
          output_var = output_var || usages.second;
          int occurrences = usages.first;
          unsigned long lhs_occurences = count(var->decl());

          // Compress if:
          // - There is one occurrence on the RHS of a clause and the others are on the LHS of a clause
          // - There is one occurrence on the RHS of a clause, that Id is a reified forall that has no
          // other occurrences
          // - There is one occurrence on the RHS of a clause, that Id is a reification in a positive
          // context, and all other occurrences are on the LHS of a clause
          bool compress = !output_var && lhs_occurences > 0;
          if (var->decl()->e() && var->decl()->e()->dyn_cast<Call>()) {
            auto call = var->decl()->e()->cast<Call>();
            if(call->id() == constants().ids.forall) {
              compress = compress && (occurrences == 1 && lhs_occurences == 1);
            } else {
              compress = compress && (occurrences == lhs_occurences);
            }
          } else {
            compress = compress && (occurrences == lhs_occurences + 1);
          }
          if (compress) {
            rhs = var->decl();
            auto negative = c->arg(1)->cast<ArrayLit>();
            lhs = (*negative)[0]->cast<Id>()->decl();
            if (lhs == rhs) {
              continue;
            }
          }
          // TODO: Detect equivalences for output variables.
        }
      }

      if (lhs && rhs) {
        assert(count(rhs) > 0);

        auto range = find(rhs);
        for (auto match = range.first; match != range.second;) {
          bool succes = compressItem(match->second, lhs);
          assert(succes);
          env.n_imp_del++;
          match = items.erase(match);
        }

        assert(!rhs->ann().contains(constants().ann.output_var));
        removeItem(it->second);
        it = items.erase(it);
      } else {
        ++it;
      }
    }
  }

  bool ImpCompressor::compressItem(Item *i, VarDecl *newLHS) {
    GCLock lock;
    if (auto ci = i->dyn_cast<ConstraintI>()) {
      auto c = ci->e()->cast<Call>();
      // Given (x -> y) /\ (y -> z), produce x -> z
      if (c->id() == constants().ids.clause) {
        auto positive = c->arg(0)->cast<ArrayLit>();
        auto rhs = (*positive)[0]->cast<Id>();
        if (rhs->decl() != newLHS) {
          ConstraintI *nci = constructClause(positive, newLHS->id());
          boolConstraints.push_back(addItem(nci));
        }
        removeItem(i);
        return true;
      // Given (x -> y) /\ (y -> pred(...)), produce x -> pred(...)
      } else if (c->id() == "mzn_reverse_map_var") {
        return true;
      } else if (c->id().endsWith("_imp")) {
        replaceCallArgument(i, c, c->n_args()-1, newLHS->id());
        trackItem(i);
        return true;
      }
    } else if (auto vdi = i->dyn_cast<VarDeclI>()) {
      auto c = vdi->e()->e()->dyn_cast<Call>();
      // Given: (x -> y) /\  (y -> (a /\ b /\ ...)), produce (x -> a) /\ (x -> b) /\ ...
      if (c->id() == constants().ids.forall) {
        auto exprs = c->arg(0)->cast<ArrayLit>();
        for (int j = 0; j < exprs->size(); ++j) {
          auto rhs = (*exprs)[j]->cast<Id>();
          if (rhs->decl() != newLHS) {
            ConstraintI *nci = constructClause(rhs, newLHS->id());
            boolConstraints.push_back(addItem(nci));
          }
        }
        return true;
        // x ::ctx_pos = pred(...); potentially: pred_imp(..., x); i.e. x -> pred(...)
      } else if (vdi->e()->ann().contains(constants().ctx.pos)) {
        ConstraintI *nci = constructHalfReif(c, newLHS->id());
        assert(nci);
        addItem(nci);
        return true;
      }
    }
    return false;
  }

  ConstraintI* ImpCompressor::constructClause(Expression *pos, Expression *neg) {
    assert(GC::locked());
    std::vector<Expression*> args(2);
    if (pos->dyn_cast<ArrayLit>()) {
      args[0] = pos;
    } else {
      assert(neg->type().isbool());
      std::vector<Expression*> eVec(1);
      eVec[0] = pos;
      args[0] = new ArrayLit(pos->loc().introduce(), eVec);
      args[0]->type(Type::varbool(1));
    }
    if (neg->dyn_cast<ArrayLit>()) {
      args[1] = neg;
    } else {
      assert(neg->type().isbool());
      std::vector<Expression*> eVec(1);
      eVec[0] = neg;
      args[1] = new ArrayLit(neg->loc().introduce(), eVec);
      args[1]->type(Type::varbool(1));
    }
    // NEVER CREATE (a -> a)
    assert( (*args[0]->dyn_cast<ArrayLit>())[0]->dyn_cast<Id>()->decl()
            != (*args[1]->dyn_cast<ArrayLit>())[0]->dyn_cast<Id>()->decl());
    auto nc = new Call(MiniZinc::Location().introduce(), constants().ids.clause, args);
    nc->type(Type::varbool());
    nc->decl(env.model->matchFn(env, nc, false));
    assert(nc->decl());

    return new ConstraintI(MiniZinc::Location().introduce(), nc);
  }

  ConstraintI *ImpCompressor::constructHalfReif(Call *call, Id *control) {
    assert(env.fopts.enable_imp);
    assert(GC::locked());
    auto cid = env.halfReifyId(call->id());
    std::vector<Expression*> args(call->n_args());
    for (int i = 0; i < call->n_args(); ++i) {
      args[i] = call->arg(i);
    }
    args.push_back(control);
    FunctionI* decl = env.model->matchFn(env, cid, args, false);
    if (decl) {
      auto nc = new Call(call->loc().introduce(), cid, args);
      nc->decl(decl);
      nc->type(Type::varbool());
      return new ConstraintI(call->loc().introduce(), nc);
    }
    return nullptr;
  }

  bool LECompressor::trackItem(Item *i) {
    if (i->removed()) {
      return false;
    }
    bool added = false;
    if (auto ci = i->dyn_cast<ConstraintI>()) {
      if (auto call = ci->e()->dyn_cast<Call>()) {
        // {int,float}_lin_le([c1,c2,...], [x, y,...], 0);
        if (call->id() == constants().ids.int_.lin_le
         || call->id() == constants().ids.float_.lin_le) {
          auto as = follow_id(call->arg(0))->cast<ArrayLit>();
          auto bs = follow_id(call->arg(1))->cast<ArrayLit>();
          assert(as->size() == bs->size());

          for (int j = 0; j < as->size(); ++j) {
            if (as->type().isintarray()) {
              if (follow_id((*as)[j])->cast<IntLit>()->v() > IntVal(0)) {
                // Check if left hand side is a variable (could be constant)
                if (auto decl = follow_id_to_decl((*bs)[j])->dyn_cast<VarDecl>()) {
                  storeItem(decl, i);
                  added = true;
                }
              }
            } else {
              if (follow_id((*as)[j])->cast<FloatLit>()->v() > FloatVal(0)) {
                // Check if left hand side is a variable (could be constant)
                if (auto decl = follow_id_to_decl((*bs)[j])->dyn_cast<VarDecl>()) {
                  storeItem(decl, i);
                  added = true;
                }
              }
            }
          }
        }
        assert(call->id() != constants().ids.int2float);
      }
    } else if(auto vdi = i->dyn_cast<VarDeclI>()) {
      assert(vdi->e());
      if (Expression* vde = vdi->e()->e()) {
        if (auto call = vde->dyn_cast<Call>()) {
          if (call->id() == constants().ids.int2float) {
            if (auto vd = follow_id_to_decl(call->arg(0))->dyn_cast<VarDecl>()) {
              auto alias = follow_id_to_decl(vdi->e())->cast<VarDecl>();
              aliasMap[vd] = alias;  
            }
          }
        }
      }
    }
    return added;
  }

  void LECompressor::compress() {
    for (auto it = items.begin(); it != items.end();) {
      VarDecl *lhs = nullptr;
      VarDecl *rhs = nullptr;
      VarDecl *alias = nullptr;

      // Check if compression is possible
      if (auto ci = it->second->dyn_cast<ConstraintI>()) {
        auto call = ci->e()->cast<Call>();
        if (call->id() == constants().ids.int_.lin_le) {
          auto as = follow_id(call->arg(0))->cast<ArrayLit>();
          auto bs = follow_id(call->arg(1))->cast<ArrayLit>();
          auto c = follow_id(call->arg(2))->cast<IntLit>();

          if (bs->size() == 2 && c->v() == IntVal(0)) {
            auto a0 = follow_id((*as)[0])->cast<IntLit>()->v();
            auto a1 = follow_id((*as)[1])->cast<IntLit>()->v();
            if (a0 == -a1 && eqBounds((*bs)[0], (*bs)[1])) {
              int i = a0 < a1 ? 0 : 1;
              if (!(*bs)[i]->isa<Id>()) {
                break;
              }
              auto neg = follow_id_to_decl((*bs)[i])->cast<VarDecl>();
              bool output_var = neg->ann().contains(constants().ann.output_var);

              auto usages = env.vo.usages(neg);
              int occurrences = usages.first;
              output_var = output_var || usages.second;
              unsigned long lhs_occurences = count(neg);
              bool compress = !output_var;
              auto search = aliasMap.find(neg);

              if (search != aliasMap.end()) {
                alias = search->second;
                auto alias_usages = env.vo.usages(alias);
                int alias_occ = alias_usages.first;
                compress = compress && (!alias_usages.second);
                unsigned long alias_lhs_occ = count(alias);
                // neg is only allowed to occur:
                // - once in the "implication"
                // - once in the aliasing
                // - on a lhs of other expressions
                // alias is only allowed to occur on a lhs of an expression.
                compress = compress && (lhs_occurences + alias_lhs_occ > 0)
                                    && (occurrences == lhs_occurences + 2)
                                    && (alias_occ == alias_lhs_occ);
              } else {
                // neg is only allowed to occur:
                // - once in the "implication"
                // - on a lhs of other expressions
                compress = compress && (lhs_occurences > 0)
                                    && (occurrences == lhs_occurences + 1);
              }

              auto pos = follow_id_to_decl((*bs)[1 - i])->dyn_cast<VarDecl>();
              if (pos && compress) {
                rhs = neg;
                lhs = pos;
                assert(lhs != rhs);
              }
              // TODO: Detect equivalences for output variables.
            }
          }
        }
      }

      if(lhs && rhs) {
        assert(count(rhs) + count(alias) > 0);

        auto range = find(rhs);
        for (auto match = range.first; match != range.second;) {
          LEReplaceVar<IntLit>(match->second, rhs, lhs);
          match = items.erase(match);
        }
        if (alias) {
          VarDecl* i2f_lhs;

          auto search = aliasMap.find(lhs);
          if (search != aliasMap.end()) {
            i2f_lhs = search->second;
          } else {
            // Create new int2float
            Call* i2f = new Call(lhs->loc().introduce(), constants().ids.int2float, {lhs->id()});
            i2f->decl(env.model->matchFn(env, i2f, false));
            assert(i2f->decl());
            i2f->type(Type::varfloat());
            auto domain = new SetLit(lhs->loc().introduce(), eval_floatset(env, lhs->ti()->domain()));
            auto i2f_ti = new TypeInst(lhs->loc().introduce(), Type::varfloat(), domain);
            i2f_lhs = new VarDecl(lhs->loc().introduce(), i2f_ti, env.genId(),i2f);
            i2f_lhs->type(Type::varfloat());
            addItem(new VarDeclI(lhs->loc().introduce(), i2f_lhs));
          }

          auto arange = find(alias);
          for (auto match = arange.first; match != arange.second;) {
            LEReplaceVar<FloatLit>(match->second, alias, i2f_lhs);
            match = items.erase(match);
          }
        }

        assert(!rhs->ann().contains(constants().ann.output_var));
        removeItem(it->second);
        env.n_lin_del++;
        it = items.erase(it);
      } else {
        ++it;
      }
    }
  }

  template <class Lit>
  void LECompressor::LEReplaceVar(Item *i, VarDecl *oldVar, VarDecl *newVar) {
    typedef typename LinearTraits<Lit>::Val Val;
    GCLock lock;

    auto ci = i->cast<ConstraintI>();
    auto call = ci->e()->cast<Call>();
    assert(call->id() == constants().ids.int_.lin_le || call->id() == constants().ids.float_.lin_le);

    // Remove old occurrences
    CollectDecls cd(env.vo, deletedVarDecls, i);
    topDown(cd, ci->e());

    ArrayLit* al_c = eval_array_lit(env, call->arg(0));
    std::vector<Val> coeffs(al_c->size());
    for (int j = 0; j < al_c->size(); j++) {
      coeffs[j] = LinearTraits<Lit>::eval(env, (*al_c)[j]);
    }
    ArrayLit* al_x = eval_array_lit(env, call->arg(1));
    std::vector<KeepAlive> x(al_x->size());
    for (int j = 0; j < al_x->size(); j++) {
      Expression* decl = follow_id_to_decl((*al_x)[j]);
      if (decl && decl->cast<VarDecl>() == oldVar) {
        x[j] = newVar->id();
      } else {
        x[j] = (*al_x)[j];
      }
    }
    Val d = LinearTraits<Lit>::eval(env, call->arg(2));

    simplify_lin<Lit>(coeffs, x, d);
    if (coeffs.empty()) {
      env.flat_removeItem(i);
      env.n_lin_del++;
      return;
    } else {
      std::vector<Expression*> coeffs_e(coeffs.size());
      std::vector<Expression*> x_e(coeffs.size());
      for (unsigned int j = 0; j < coeffs.size(); j++) {
        coeffs_e[j] = Lit::a(coeffs[j]);
        x_e[j] = x[j]();
        Expression* decl = follow_id_to_decl(x_e[j]);
        if (decl && decl->cast<VarDecl>() == newVar) {
          storeItem(newVar, i);
        }
      }

      if (auto arg0 = call->arg(0)->dyn_cast<ArrayLit>()) {
        arg0->setVec(coeffs_e);
      } else {
        auto al_c_new = new ArrayLit(al_c->loc().introduce(),coeffs_e);
        al_c_new->type(al_c->type());
        call->arg(0, al_c_new);
      }

      if (auto arg1 = call->arg(1)->dyn_cast<ArrayLit>()) {
        arg1->setVec(x_e);
      } else {
        auto al_x_new = new ArrayLit(al_x->loc().introduce(),x_e);
        al_x_new->type(al_x->type());
        call->arg(1, al_x_new);
      }

      call->arg(2, Lit::a(d));
    }

    // Add new occurences
    CollectOccurrencesE ce(env.vo, i);
    topDown(ce, ci->e());
  }

  bool LECompressor::eqBounds(Expression *a, Expression *b) {
    // TODO: (To optimise) Check lb(lhs) >= lb(rhs) and enforce ub(lhs) <= ub(rhs)
    IntSetVal* dom_a = nullptr;
    IntSetVal* dom_b = nullptr;

    if(auto a_decl = follow_id_to_decl(a)->dyn_cast<VarDecl>()) {
      if (a_decl->ti()->domain()) {
        dom_a = eval_intset(env, a_decl->ti()->domain());
      }
    } else {
      assert(a->dyn_cast<IntLit>());
      auto a_val = a->cast<IntLit>();
      dom_a = IntSetVal::a(a_val->v(), a_val->v());
    }

    if(auto b_decl = follow_id_to_decl(b)->dyn_cast<VarDecl>()) {
      if (b_decl->ti()->domain()) {
        dom_b = eval_intset(env, b_decl->ti()->domain());
      }
    } else {
      assert(b->dyn_cast<IntLit>());
      auto b_val = b->cast<IntLit>();
      dom_b = IntSetVal::a(b_val->v(), b_val->v());
    }

    return (dom_a && dom_b && (dom_a->min() == dom_b->min()) && (dom_a->max() == dom_b->max())) || (!dom_a && !dom_b);
  }

}
