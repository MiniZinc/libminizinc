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

  void ChainCompressor::addItem(Item *i) {
    env.flat_addItem(i);
    trackItem(i);
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
          } else if (vdi->e()->ann().contains(constants().ctx.pos)) {
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
          int occurrences = env.vo.occurrences(var->decl());
          unsigned long lhs_occurences = count(var->decl());

          // Compress if:
          // - There is one occurrence on the RHS of a clause and the others are on the LHS of a clause
          // - There is one occurrence on the RHS of a clause, that Id is a reified forall that has no
          // other occurrences
          // - There is one occurrence on the RHS of a clause, that Id is a reification in a positive
          // context, and all other occurrences are on the LHS of a clause
          bool compress = lhs_occurences > 0;
          if (var->decl()->e()) {
            auto call = var->decl()->e()->dyn_cast<Call>();
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
        }
      }

      if (lhs && rhs) {
        assert(count(rhs) > 0);

        auto range = find(rhs);
        for (auto match = range.first; match != range.second;) {
          bool succes = compressItem(match->second, lhs);
          assert(succes);
          match = items.erase(match);
        }
        if(!rhs->ann().contains(constants().ann.output_var)) {
          removeItem(it->second);
          it = items.erase(it);
        } else {
          ++it;
        }
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
          addItem(nci);
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
            addItem(nci);
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

}
