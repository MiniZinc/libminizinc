/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/chain_compressor.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/iter.hh>
#include <minizinc/optimize.hh>
#include <minizinc/optimize_constraints.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/values.hh>

#include <deque>
#include <vector>

namespace MiniZinc {

void VarOccurrences::addIndex(VarDeclI* i, int idx_i) { idx.insert(i->e()->id(), idx_i); }
void VarOccurrences::addIndex(VarDecl* e, int idx_i) {
  assert(find(e) == -1);
  idx.insert(e->id(), idx_i);
}
int VarOccurrences::find(VarDecl* vd) {
  auto it = idx.find(vd->id());
  return it.first ? *it.second : -1;
}
void VarOccurrences::remove(VarDecl* vd) { idx.remove(vd->id()); }

void VarOccurrences::add(VarDecl* v, Item* i) {
  auto vi = itemMap.find(v->id()->decl()->id());
  if (vi.first) {
    vi.second->insert(i);
  } else {
    Items items({i});
    itemMap.insert(v->id()->decl()->id(), items);
  }
}

int VarOccurrences::remove(VarDecl* v, Item* i) {
  auto vi = itemMap.find(v->id()->decl()->id());
  assert(vi.first);
  vi.second->erase(i);
  return static_cast<int>(vi.second->size());
}

void VarOccurrences::removeAllOccurrences(VarDecl* v) {
  auto vi = itemMap.find(v->id()->decl()->id());
  assert(vi.first);
  vi.second->clear();
}

void VarOccurrences::unify(EnvI& env, Model* m, Id* id0_0, Id* id1_0) {
  Id* id0 = id0_0->decl()->id();
  Id* id1 = id1_0->decl()->id();

  VarDecl* v0 = id0->decl();
  VarDecl* v1 = id1->decl();

  if (v0 == v1) {
    return;
  }

  int v0idx = find(v0);
  assert(v0idx != -1);
  (*env.flat())[v0idx]->remove();

  auto vi0 = itemMap.find(v0->id());
  if (vi0.first) {
    auto vi1 = itemMap.find(v1->id());
    if (vi1.first) {
      for (auto* item : *vi0.second) {
        vi1.second->insert(item);
      }
    } else {
      itemMap.insert(v1->id(), *vi0.second);
    }
    itemMap.remove(v0->id());
  }

  remove(v0);
  id0->redirect(id1);
}

void VarOccurrences::clear() {
  itemMap.clear();
  idx.clear();
}

int VarOccurrences::occurrences(VarDecl* v) {
  auto vi = itemMap.find(v->id()->decl()->id());
  return vi.first ? static_cast<int>(vi.second->size()) : 0;
}

std::pair<int, bool> VarOccurrences::usages(VarDecl* v) {
  bool is_output = Expression::ann(v).contains(Constants::constants().ann.output_var) ||
                   Expression::ann(v).containsCall(Constants::constants().ann.output_array);
  auto vi = itemMap.find(v->id()->decl()->id());
  if (!vi.first) {
    return std::make_pair(0, is_output);
  }
  int count = 0;
  for (Item* i : *vi.second) {
    auto* vd = i->dynamicCast<VarDeclI>();
    if ((vd != nullptr) && (vd->e() != nullptr) && (vd->e()->e() != nullptr) &&
        (Expression::isa<ArrayLit>(vd->e()->e()) || Expression::isa<SetLit>(vd->e()->e()))) {
      auto u = usages(vd->e());
      is_output = is_output || u.second;
      count += u.first;
    } else {
      count++;
    }
  }
  return std::make_pair(count, is_output);
}

void CollectOccurrencesE::vId(const Id* id) {
  if (id->decl() == nullptr) {
    return;
  }
  // TODO: Consider a better fix to not count internal annotations!
  for (const auto* ann : env.constants.internalAnn()) {
    if (id->decl() == ann->decl()) {
      return;
    }
  }
  // ENDTODO
  vo.add(id->decl(), ci);
}

void CollectOccurrencesI::vVarDeclI(VarDeclI* v) {
  CollectOccurrencesE ce(env, vo, v);
  top_down(ce, v->e());
}
void CollectOccurrencesI::vConstraintI(ConstraintI* ci) {
  CollectOccurrencesE ce(env, vo, ci);
  top_down(ce, ci->e());
  for (ExpressionSetIter it = Expression::ann(ci->e()).begin();
       it != Expression::ann(ci->e()).end(); ++it) {
    top_down(ce, *it);
  }
}
void CollectOccurrencesI::vSolveI(SolveI* si) {
  CollectOccurrencesE ce(env, vo, si);
  top_down(ce, si->e());
  for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++si) {
    top_down(ce, *it);
  }
}

void CollectDecls::vId(Id* id) {
  if (id->decl() == nullptr) {
    return;
  }
  // TODO: Consider a better fix to not count internal annotations!
  for (const auto* ann : env.constants.internalAnn()) {
    if (id->decl() == ann->decl()) {
      return;
    }
  }
  // ENDTODO
  int count = vo.remove(id->decl(), item);
  if (count == 0 && varIsFree(id->decl())) {
    vd.push_back(id->decl());
  }
}

bool is_output(VarDecl* vd) {
  for (ExpressionSetIter it = Expression::ann(vd).begin(); it != Expression::ann(vd).end(); ++it) {
    if (*it != nullptr) {
      if (*it == Constants::constants().ann.output_var) {
        return true;
      }
      if (Call* c = Expression::dynamicCast<Call>(*it)) {
        if (c->id() == Constants::constants().ann.output_array) {
          return true;
        }
      }
    }
  }
  return false;
}

void unify(EnvI& env, std::vector<VarDecl*>& deletedVarDecls, Id* id0, Id* id1) {
  if (id0->decl() != id1->decl()) {
    if (is_output(id0->decl())) {
      std::swap(id0, id1);
    }

    if (id0->decl()->e() != nullptr && !Expression::equal(id0->decl()->e(), id1->decl()->id())) {
      Expression* rhs = id0->decl()->e();

      auto* vdi1 = (*env.flat())[env.varOccurrences.find(id1->decl())]->cast<VarDeclI>();
      CollectOccurrencesE ce(env, env.varOccurrences, vdi1);
      top_down(ce, rhs);

      id1->decl()->e(rhs);
      id0->decl()->e(nullptr);

      auto* vdi0 = (*env.flat())[env.varOccurrences.find(id0->decl())]->cast<VarDeclI>();
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, vdi0);
      top_down(cd, rhs);
    }
    if (Expression::equal(id1->decl()->e(), id0->decl()->id())) {
      auto* vdi1 = (*env.flat())[env.varOccurrences.find(id1->decl())]->cast<VarDeclI>();
      CollectDecls cd(env, env.varOccurrences, deletedVarDecls, vdi1);
      Expression* rhs = id1->decl()->e();
      top_down(cd, rhs);
      id1->decl()->e(nullptr);
    }
    // Compute intersection of domains
    std::vector<std::pair<TypeInst*, TypeInst*>> todo({{id0->decl()->ti(), id1->decl()->ti()}});
    while (!todo.empty()) {
      auto it = todo.back();
      todo.pop_back();
      auto* ti0 = it.first;
      auto* ti1 = it.second;
      if (ti0->domain() != nullptr) {
        if (ti1->domain() != nullptr) {
          if (ti0->type().structBT()) {
            auto* tis0 = Expression::cast<ArrayLit>(ti0->domain());
            auto* tis1 = Expression::cast<ArrayLit>(ti1->domain());
            for (unsigned int i = 0; i < tis0->size(); i++) {
              todo.emplace_back(Expression::cast<TypeInst>((*tis0)[i]),
                                Expression::cast<TypeInst>((*tis1)[i]));
            }
          } else if (ti0->type().bt() == Type::BT_INT) {
            IntSetVal* isv0 = eval_intset(env, ti0->domain());
            IntSetVal* isv1 = eval_intset(env, ti1->domain());
            IntSetRanges isv0r(isv0);
            IntSetRanges isv1r(isv1);
            Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(isv0r, isv1r);
            IntSetVal* nd = IntSetVal::ai(inter);
            if (nd->empty() && !ti0->type().isSet()) {
              env.fail();
            } else if (!nd->equal(isv1)) {
              ti1->domain(new SetLit(Location(), nd));
              if (nd->equal(isv0)) {
                ti1->setComputedDomain(ti0->computedDomain());
              } else {
                ti1->setComputedDomain(false);
              }
            }
          } else if (ti0->type().bt() == Type::BT_BOOL) {
            if (eval_bool(env, ti0->domain()) != eval_bool(env, ti1->domain())) {
              env.fail();
            }
          } else if (ti0->type().bt() == Type::BT_FLOAT) {
            // float
            FloatSetVal* isv0 = eval_floatset(env, ti0->domain());
            FloatSetVal* isv1 = eval_floatset(env, ti1->domain());
            FloatSetRanges isv0r(isv0);
            FloatSetRanges isv1r(isv1);
            Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> inter(isv0r, isv1r);
            FloatSetVal* nd = FloatSetVal::ai(inter);

            FloatSetRanges nd_r(nd);
            FloatSetRanges isv1r_2(isv1);

            if (nd->empty()) {
              env.fail();
            } else if (!Ranges::equal(nd_r, isv1r_2)) {
              ti1->domain(new SetLit(Location(), nd));
              FloatSetRanges nd_r_2(nd);
              FloatSetRanges isv0r_2(isv0);
              if (Ranges::equal(nd_r_2, isv0r_2)) {
                ti1->setComputedDomain(ti0->computedDomain());
              } else {
                ti1->setComputedDomain(false);
              }
            }
          } else {
            throw InternalError("Failed to unify identifiers during optimisation");
          }
        } else {
          ti1->domain(ti0->domain());
        }
      }
    }

    // If both variables are output variables, unify them in the output model
    if (is_output(id0->decl())) {
      assert(env.outputFlatVarOccurrences.find(id0->decl()) != -1);
      VarDecl* id0_output =
          (*env.output)[env.outputFlatVarOccurrences.find(id0->decl())]->cast<VarDeclI>()->e();
      assert(env.outputFlatVarOccurrences.find(id1->decl()) != -1);
      VarDecl* id1_output =
          (*env.output)[env.outputFlatVarOccurrences.find(id1->decl())]->cast<VarDeclI>()->e();
      if (id0_output->e() == nullptr) {
        id0_output->e(id1_output->id());
      }
    }

    env.varOccurrences.unify(env, env.flat(), id0, id1);
  }
}

void substitute_fixed_vars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls);
void simplify_bool_constraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::deque<unsigned int>& vardeclQueue,
                              std::deque<Item*>& constraintQueue, std::vector<Item*>& toRemove,
                              std::vector<VarDecl*>& deletedVarDecls,
                              std::unordered_map<Expression*, int>& nonFixedLiteralCount);

bool simplify_constraint(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls,
                         std::deque<Item*>& constraintQueue,
                         std::deque<unsigned int>& vardeclQueue);

void push_vardecl(EnvI& env, VarDeclI* vdi, unsigned int vd_idx, std::deque<unsigned int>& q) {
  if (!vdi->removed() && !vdi->flag()) {
    vdi->flag(true);
    q.push_back(vd_idx);
  }
}
void push_vardecl(EnvI& env, unsigned int vd_idx, std::deque<unsigned int>& q) {
  push_vardecl(env, (*env.flat())[vd_idx]->cast<VarDeclI>(), vd_idx, q);
}

void push_dependent_constraints(EnvI& env, Id* id, std::deque<Item*>& q) {
  auto it = env.varOccurrences.itemMap.find(id->decl()->id());
  if (it.first) {
    for (auto* item : *it.second) {
      if (auto* ci = item->dynamicCast<ConstraintI>()) {
        if (!ci->removed() && !ci->flag()) {
          ci->flag(true);
          q.push_back(ci);
        }
      } else if (auto* vdi = item->dynamicCast<VarDeclI>()) {
        if (vdi->e()->id()->decl() != vdi->e()) {
          vdi = (*env.flat())[env.varOccurrences.find(vdi->e()->id()->decl())]->cast<VarDeclI>();
        }
        if (!vdi->removed() && !vdi->flag() && (vdi->e()->e() != nullptr)) {
          vdi->flag(true);
          q.push_back(vdi);
        }
      }
    }
  }
}

void remove_deleted_items(EnvI& envi, std::vector<VarDecl*>& deletedVarDecls) {
  // TODO: The delayed deletion could be done eagerly by the creation of
  // env.optRemoveItem() which contains the logic in this while loop.
  auto& m = *envi.flat();
  while (!deletedVarDecls.empty()) {
    envi.checkCancel();
    VarDecl* cur = deletedVarDecls.back();
    deletedVarDecls.pop_back();
    if (envi.varOccurrences.occurrences(cur) == 0) {
      auto cur_idx = envi.varOccurrences.idx.find(cur->id());
      if (cur_idx.first && !m[*cur_idx.second]->removed()) {
        if (is_output(cur)) {
          // We have to change the output model if we remove this variable
          Expression* val = nullptr;
          if (cur->type().isbool() && (cur->ti()->domain() != nullptr)) {
            val = cur->ti()->domain();
          } else if (cur->type().isint()) {
            if ((cur->e() != nullptr) && Expression::isa<IntLit>(cur->e())) {
              val = cur->e();
            } else if ((cur->ti()->domain() != nullptr) &&
                       Expression::isa<SetLit>(cur->ti()->domain()) &&
                       Expression::cast<SetLit>(cur->ti()->domain())->isv()->size() == 1 &&
                       Expression::cast<SetLit>(cur->ti()->domain())->isv()->min() ==
                           Expression::cast<SetLit>(cur->ti()->domain())->isv()->max()) {
              val = IntLit::a(Expression::cast<SetLit>(cur->ti()->domain())->isv()->min());
            }
          } else if (cur->type().isfloat()) {
            if ((cur->e() != nullptr) && Expression::isa<FloatLit>(cur->e())) {
              val = cur->e();
            } else if ((cur->ti()->domain() != nullptr) &&
                       Expression::isa<SetLit>(cur->ti()->domain()) &&
                       Expression::cast<SetLit>(cur->ti()->domain())->fsv()->size() == 1 &&
                       Expression::cast<SetLit>(cur->ti()->domain())->fsv()->min() ==
                           Expression::cast<SetLit>(cur->ti()->domain())->fsv()->max()) {
              val = FloatLit::a(Expression::cast<SetLit>(cur->ti()->domain())->fsv()->min());
            }
          } else if (cur->type().isIntSet()) {
            if (cur->e() != nullptr && Expression::isa<SetLit>(cur->e())) {
              val = cur->e();
            }
          } else if (cur->type().dim() > 0 && cur->type().isPar()) {
            if (cur->e() != nullptr && Expression::isa<ArrayLit>(cur->e())) {
              val = cur->e();
            }
          }
          if (val != nullptr) {
            // Find corresponding variable in output model and fix it
            VarDecl* vd_out =
                (*envi.output)[envi.outputFlatVarOccurrences.find(cur)]->cast<VarDeclI>()->e();
            vd_out->e(val);
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls,
                            m[*cur_idx.second]->cast<VarDeclI>());
            top_down(cd, cur->e());
            (*envi.flat())[*cur_idx.second]->remove();
          }
        } else {
          CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls,
                          m[*cur_idx.second]->cast<VarDeclI>());
          top_down(cd, cur->e());
          (*envi.flat())[*cur_idx.second]->remove();
        }
      }
    }
  }
}

void optimize(Env& env, bool chain_compression) {
  env.envi().checkCancel();

  if (env.envi().failed()) {
    return;
  }
  try {
    EnvI& envi = env.envi();
    Model& m = *envi.flat();
    std::vector<int> toAssignBoolVars;
    std::vector<int> toRemoveConstraints;
    std::vector<VarDecl*> deletedVarDecls;

    // Queue of constraint and variable items that still need to be optimised
    std::deque<Item*> constraintQueue;
    // Queue of variable declarations (indexes into the model) that still need to be optimised
    std::deque<unsigned int> vardeclQueue;

    std::vector<int> boolConstraints;

    GCLock lock;

    // Phase 0: clean up
    // - clear flags for all constraint and variable declaration items
    //   (flags are used to indicate whether an item is already queued or not)
    for (auto& i : m) {
      if (!i->removed()) {
        if (auto* ci = i->dynamicCast<ConstraintI>()) {
          ci->flag(false);
        } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
          vdi->flag(false);
        }
      }
    }

    envi.checkCancel();

    // Phase 1: initialise queues
    //  - remove equality constraints between identifiers
    //  - remove toplevel forall constraints
    //  - collect exists, clauses and reified foralls in boolConstraints
    //  - remove "constraint x" where x is a bool var
    //  - unify variables that are assigned to an identifier
    //  - push bool vars that are fixed and have a RHS (to propagate the RHS constraint)
    //  - push int/float vars that are fixed (either have a RHS or a singleton domain)
    for (int i = 0; i < m.size(); i++) {
      env.envi().checkCancel();
      if (m[i]->removed()) {
        continue;
      }

      if (auto* ci = m[i]->dynamicCast<ConstraintI>()) {
        ci->flag(false);
        if (!ci->removed()) {
          if (Call* c = Expression::dynamicCast<Call>(ci->e())) {
            if ((c->id() == envi.constants.ids.int_.eq || c->id() == envi.constants.ids.bool_.eq ||
                 c->id() == envi.constants.ids.float_.eq ||
                 c->id() == envi.constants.ids.set_.eq) &&
                Expression::isa<Id>(c->arg(0)) && Expression::isa<Id>(c->arg(1)) &&
                (Expression::cast<Id>(c->arg(0))->decl()->e() == nullptr ||
                 Expression::cast<Id>(c->arg(1))->decl()->e() == nullptr)) {
              // Equality constraint between two identifiers: unify

              if (Call* defVar = Expression::ann(c).getCall(envi.constants.ann.defines_var)) {
                // First, remove defines_var/is_defined_var annotations if present
                if (Expression::equal(defVar->arg(0), c->arg(0))) {
                  Expression::ann(Expression::cast<Id>(c->arg(0))->decl())
                      .remove(envi.constants.ann.is_defined_var);
                } else {
                  Expression::ann(Expression::cast<Id>(c->arg(1))->decl())
                      .remove(envi.constants.ann.is_defined_var);
                }
              }
              unify(envi, deletedVarDecls, Expression::cast<Id>(c->arg(0)),
                    Expression::cast<Id>(c->arg(1)));
              {
                VarDecl* vd = Expression::cast<Id>(c->arg(0))->decl();
                int v0idx = envi.varOccurrences.find(vd);
                push_vardecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
              }

              push_dependent_constraints(envi, Expression::cast<Id>(c->arg(0)), constraintQueue);
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
              top_down(cd, c);
              ci->e(envi.constants.literalTrue);
              ci->remove();
            } else if ((c->id() == envi.constants.ids.int_.eq ||
                        c->id() == envi.constants.ids.bool_.eq ||
                        c->id() == envi.constants.ids.float_.eq ||
                        c->id() == envi.constants.ids.set_.eq) &&
                       ((Expression::isa<Id>(c->arg(0)) &&
                         Expression::cast<Id>(c->arg(0))->decl()->e() == nullptr &&
                         Expression::type(c->arg(1)).isPar()) ||
                        (Expression::isa<Id>(c->arg(1)) &&
                         Expression::cast<Id>(c->arg(1))->decl()->e() == nullptr &&
                         Expression::type(c->arg(0)).isPar()))) {
              // equality constraint with one fixed var can be resolved later by
              // simplify_bool_constraint
              auto* id = Expression::cast<Id>(c->arg(Expression::isa<Id>(c->arg(0)) ? 0 : 1));
              int idx = envi.varOccurrences.find(id->decl());
              push_vardecl(envi, m[idx]->cast<VarDeclI>(), idx, vardeclQueue);
              push_dependent_constraints(envi, id, constraintQueue);
            } else if (c->id() == envi.constants.ids.int_.lin_eq &&
                       Expression::equal(c->arg(2), IntLit::a(0))) {
              auto* al_c = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
              if (al_c->size() == 2 && IntLit::v(Expression::cast<IntLit>((*al_c)[0])) ==
                                           -IntLit::v(Expression::cast<IntLit>((*al_c)[1]))) {
                auto* al_x = Expression::cast<ArrayLit>(follow_id(c->arg(1)));
                if (Expression::isa<Id>((*al_x)[0]) && Expression::isa<Id>((*al_x)[1]) &&
                    (Expression::cast<Id>((*al_x)[0])->decl()->e() == nullptr ||
                     Expression::cast<Id>((*al_x)[1])->decl()->e() == nullptr)) {
                  // Equality constraint between two identifiers: unify

                  if (Call* defVar = Expression::ann(c).getCall(envi.constants.ann.defines_var)) {
                    // First, remove defines_var/is_defined_var annotations if present
                    if (Expression::equal(defVar->arg(0), (*al_x)[0])) {
                      Expression::ann(Expression::cast<Id>((*al_x)[0])->decl())
                          .remove(envi.constants.ann.is_defined_var);
                    } else {
                      Expression::ann(Expression::cast<Id>((*al_x)[1])->decl())
                          .remove(envi.constants.ann.is_defined_var);
                    }
                  }
                  unify(envi, deletedVarDecls, Expression::cast<Id>((*al_x)[0]),
                        Expression::cast<Id>((*al_x)[1]));
                  {
                    VarDecl* vd = Expression::cast<Id>((*al_x)[0])->decl();
                    int v0idx = envi.varOccurrences.find(vd);
                    push_vardecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
                  }

                  push_dependent_constraints(envi, Expression::cast<Id>((*al_x)[0]),
                                             constraintQueue);
                  CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
                  top_down(cd, c);
                  ci->e(envi.constants.literalTrue);
                  ci->remove();
                }
              }
            } else if (c->id() == envi.constants.ids.forall) {
              // Remove forall constraints, assign variables inside the forall to true

              auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
              for (unsigned int j = al->size(); (j--) != 0U;) {
                if (Id* id = Expression::dynamicCast<Id>((*al)[j])) {
                  if (id->decl()->ti()->domain() == nullptr) {
                    toAssignBoolVars.push_back(
                        *envi.varOccurrences.idx.find(id->decl()->id()).second);
                  } else if (id->decl()->ti()->domain() == envi.constants.literalFalse) {
                    env.envi().fail();
                    id->decl()->e(envi.constants.literalTrue);
                  }
                }  // todo: check else case (fixed bool inside a forall at this stage)
              }
              toRemoveConstraints.push_back(i);
            } else if (c->id() == envi.constants.ids.exists ||
                       c->id() == envi.constants.ids.clause) {
              // Add disjunctive constraints to the boolConstraints list

              boolConstraints.push_back(i);
            }
          } else if (Id* id = Expression::dynamicCast<Id>(ci->e())) {
            if (id->decl()->ti()->domain() == envi.constants.literalFalse) {
              env.envi().fail();
              ci->e(envi.constants.literalFalse);
            } else {
              if (id->decl()->ti()->domain() == nullptr) {
                toAssignBoolVars.push_back(*envi.varOccurrences.idx.find(id->decl()->id()).second);
              }
              toRemoveConstraints.push_back(i);
            }
          }
        }
      } else if (auto* vdi = m[i]->dynamicCast<VarDeclI>()) {
        vdi->flag(false);
        if ((vdi->e()->e() != nullptr) && Expression::isa<Id>(vdi->e()->e()) &&
            Expression::type(vdi->e()).dim() == 0) {
          // unify variable with the identifier it's assigned to
          Id* id1 = Expression::cast<Id>(vdi->e()->e());
          vdi->e()->e(nullptr);

          // Transfer is_defined_var annotation
          if (Expression::ann(id1->decl()).contains(envi.constants.ann.is_defined_var)) {
            Expression::addAnnotation(vdi->e(), envi.constants.ann.is_defined_var);
          } else if (Expression::ann(vdi->e()).contains(envi.constants.ann.is_defined_var)) {
            Expression::addAnnotation(id1->decl(), envi.constants.ann.is_defined_var);
          }

          unify(envi, deletedVarDecls, vdi->e()->id(), id1);
          push_dependent_constraints(envi, id1, constraintQueue);
        }
        if (vdi->e()->type().isbool() && vdi->e()->type().dim() == 0 &&
            (vdi->e()->ti()->domain() == envi.constants.literalTrue ||
             vdi->e()->ti()->domain() == envi.constants.literalFalse ||
             (vdi->e()->e() != nullptr && Expression::isa<BoolLit>(vdi->e()->e())))) {
          // push RHS onto constraint queue since this bool var is fixed
          push_vardecl(envi, vdi, i, vardeclQueue);
          push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
        }
        if (Call* c = Expression::dynamicCast<Call>(vdi->e()->e())) {
          if (c->id() == envi.constants.ids.forall || c->id() == envi.constants.ids.exists ||
              c->id() == envi.constants.ids.clause) {
            // push reified foralls, exists, clauses
            boolConstraints.push_back(i);
          }
        }
        if (vdi->e()->type().isint()) {
          if (((vdi->e()->e() != nullptr) && Expression::isa<IntLit>(vdi->e()->e())) ||
              ((vdi->e()->ti()->domain() != nullptr) &&
               Expression::isa<SetLit>(vdi->e()->ti()->domain()) &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->isv()->size() == 1 &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->isv()->min() ==
                   Expression::cast<SetLit>(vdi->e()->ti()->domain())->isv()->max())) {
            // Variable is assigned an integer, or has a singleton domain
            push_vardecl(envi, vdi, i, vardeclQueue);
            push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
          }
        }
        if (vdi->e()->type().isfloat()) {
          if (((vdi->e()->e() != nullptr) && Expression::isa<FloatLit>(vdi->e()->e())) ||
              ((vdi->e()->ti()->domain() != nullptr) &&
               Expression::isa<SetLit>(vdi->e()->ti()->domain()) &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->fsv()->size() == 1 &&
               Expression::cast<SetLit>(vdi->e()->ti()->domain())->fsv()->min() ==
                   Expression::cast<SetLit>(vdi->e()->ti()->domain())->fsv()->max())) {
            // Variable is assigned a float, or has a singleton domain
            push_vardecl(envi, vdi, i, vardeclQueue);
            push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
          }
        }
        if (vdi->e()->type().isIntSet()) {
          if (vdi->e()->e() != nullptr && Expression::isa<SetLit>(vdi->e()->e())) {
            // Set variable is assigned a literal
            push_vardecl(envi, vdi, i, vardeclQueue);
            push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
          }
        }
      }
    }

    // Phase 2: handle boolean constraints
    //  - check if any boolean constraint is subsumed (e.g. a fixed false in a forall, or a fixed
    //  true in a disjunction)
    //  - check if any boolean constraint has a single non-fixed literal left, then fix that literal
    for (auto i = static_cast<unsigned int>(boolConstraints.size()); (i--) != 0U;) {
      env.envi().checkCancel();

      Item* bi = m[boolConstraints[i]];
      if (bi->removed()) {
        continue;
      }
      Call* c;

      if (bi->isa<ConstraintI>()) {
        c = Expression::dynamicCast<Call>(bi->cast<ConstraintI>()->e());
      } else {
        c = Expression::dynamicCast<Call>(bi->cast<VarDeclI>()->e()->e());
      }
      if (c == nullptr) {
        continue;
      }
      bool isConjunction = (c->id() == envi.constants.ids.forall);
      bool subsumed = false;
      Id* finalId = nullptr;
      bool finalIdNeg = false;
      int idCount = 0;
      std::vector<VarDecl*> pos;
      std::vector<VarDecl*> neg;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        bool unit = (j == 0 ? isConjunction : !isConjunction);
        auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(j)));
        for (unsigned int k = 0; k < al->size(); k++) {
          if (Id* ident = Expression::dynamicCast<Id>((*al)[k])) {
            if ((ident->decl()->ti()->domain() != nullptr) ||
                ((ident->decl()->e() != nullptr) && Expression::type(ident->decl()->e()).isPar())) {
              bool identValue = ident->decl()->ti()->domain() != nullptr
                                    ? eval_bool(envi, ident->decl()->ti()->domain())
                                    : eval_bool(envi, ident->decl()->e());
              if (identValue != unit) {
                subsumed = true;
                goto subsumed_check_done;
              }
            } else {
              idCount++;
              finalId = ident;
              finalIdNeg = (j == 1);
              if (j == 0) {
                pos.push_back(ident->decl());
              } else {
                neg.push_back(ident->decl());
              }
            }
          } else {
            if (Expression::cast<BoolLit>((*al)[k])->v() != unit) {
              subsumed = true;
              goto subsumed_check_done;
            }
          }
        }
      }
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

    subsumed_check_done:
      if (subsumed) {
        if (isConjunction) {
          if (bi->isa<ConstraintI>()) {
            env.envi().fail();
          } else {
            if (bi->cast<VarDeclI>()->e()->ti()->domain() != nullptr) {
              if (eval_bool(envi, bi->cast<VarDeclI>()->e()->ti()->domain())) {
                envi.fail();
              }
            } else {
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
              top_down(cd, bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(envi.constants.literalFalse);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(envi.constants.literalFalse);
              push_vardecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              push_dependent_constraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        } else {
          if (bi->isa<ConstraintI>()) {
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<ConstraintI>()->e());
            bi->remove();
          } else {
            if (bi->cast<VarDeclI>()->e()->ti()->domain() != nullptr) {
              if (!eval_bool(envi, bi->cast<VarDeclI>()->e()->ti()->domain())) {
                envi.fail();
              }
            } else {
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
              top_down(cd, bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(envi.constants.literalTrue);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(envi.constants.literalTrue);
              push_vardecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              push_dependent_constraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        }
      } else if (idCount == 1 && bi->isa<ConstraintI>()) {
        assert(finalId->decl()->ti()->domain() == nullptr);
        finalId->decl()->ti()->domain(envi.constants.boollit(!finalIdNeg));
        if (finalId->decl()->e() == nullptr) {
          finalId->decl()->e(envi.constants.boollit(!finalIdNeg));
        }
        CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
        top_down(cd, bi->cast<ConstraintI>()->e());
        bi->remove();
        push_vardecl(envi, *envi.varOccurrences.idx.find(finalId->decl()->id()).second,
                     vardeclQueue);
        push_dependent_constraints(envi, finalId, constraintQueue);
      }  // todo: for var decls, we could unify the variable with the remaining finalId (the RHS)
    }

    // Fix all bool vars in toAssignBoolVars to true and push their declarations and constraints
    for (unsigned int i = static_cast<int>(toAssignBoolVars.size()); (i--) != 0U;) {
      if (m[toAssignBoolVars[i]]->removed()) {
        continue;
      }
      auto* vdi = m[toAssignBoolVars[i]]->cast<VarDeclI>();
      if (vdi->e()->ti()->domain() == nullptr) {
        vdi->e()->ti()->domain(envi.constants.literalTrue);
        push_vardecl(envi, vdi, toAssignBoolVars[i], vardeclQueue);
        push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
      }
    }

    // Phase 3: fixpoint of constraint and variable simplification

    std::unordered_map<Expression*, int> nonFixedLiteralCount;
    while (!vardeclQueue.empty() || !constraintQueue.empty()) {
      while (!vardeclQueue.empty()) {
        env.envi().checkCancel();

        unsigned int var_idx = vardeclQueue.front();
        vardeclQueue.pop_front();
        m[var_idx]->cast<VarDeclI>()->flag(false);
        VarDecl* vd = m[var_idx]->cast<VarDeclI>()->e();

        if (vd->type().isbool() && (vd->ti()->domain() != nullptr)) {
          bool isTrue = vd->ti()->domain() == envi.constants.literalTrue;
          bool remove = false;
          if (vd->e() != nullptr) {
            if (Id* id = Expression::dynamicCast<Id>(vd->e())) {
              // Variable assigned to id, so fix id
              if (id->decl()->ti()->domain() == nullptr) {
                id->decl()->ti()->domain(vd->ti()->domain());
                push_vardecl(envi, envi.varOccurrences.idx.get(id->decl()->id()), vardeclQueue);
              } else if (id->decl()->ti()->domain() != vd->ti()->domain()) {
                env.envi().fail();
              }
              remove = true;
            } else if (Call* c = Expression::dynamicCast<Call>(vd->e())) {
              if (isTrue && c->id() == envi.constants.ids.forall) {
                // Reified forall is now fixed to true, so make all elements of the conjunction true
                remove = true;
                auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
                for (unsigned int i = 0; i < al->size(); i++) {
                  if (Id* id = Expression::dynamicCast<Id>((*al)[i])) {
                    if (id->decl()->ti()->domain() == nullptr) {
                      id->decl()->ti()->domain(envi.constants.literalTrue);
                      push_vardecl(envi, envi.varOccurrences.idx.get(id->decl()->id()),
                                   vardeclQueue);
                    } else if (id->decl()->ti()->domain() == envi.constants.literalFalse) {
                      env.envi().fail();
                      remove = true;
                    }
                  }
                }
              } else if (!isTrue && (c->id() == envi.constants.ids.exists ||
                                     c->id() == envi.constants.ids.clause)) {
                // Reified disjunction is now fixed to false, so make all elements of the
                // disjunction false
                remove = true;
                for (unsigned int i = 0; i < c->argCount(); i++) {
                  bool ispos = i == 0;
                  auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(i)));
                  for (unsigned int j = 0; j < al->size(); j++) {
                    if (Id* id = Expression::dynamicCast<Id>((*al)[j])) {
                      if (id->decl()->ti()->domain() == nullptr) {
                        id->decl()->ti()->domain(envi.constants.boollit(!ispos));
                        push_vardecl(envi, envi.varOccurrences.idx.get(id->decl()->id()),
                                     vardeclQueue);
                      } else if (id->decl()->ti()->domain() == envi.constants.boollit(ispos)) {
                        env.envi().fail();
                        remove = true;
                      }
                    }
                  }
                }
              }
            }
          } else {
            // If bool variable doesn't have a RHS, just remove it
            remove = true;
          }
          push_dependent_constraints(envi, vd->id(), constraintQueue);
          std::vector<Item*> toRemove;
          auto it = envi.varOccurrences.itemMap.find(vd->id()->decl()->id());

          // Handle all boolean constraints that involve this variable
          if (it.first) {
            for (auto* item : *it.second) {
              if (item->removed()) {
                continue;
              }
              if (auto* vdi = item->dynamicCast<VarDeclI>()) {
                // The variable occurs in the RHS of another variable, so
                // if that is an array variable, push it onto the stack for processing
                if (vdi->e()->e() != nullptr && Expression::isa<ArrayLit>(vdi->e()->e())) {
                  push_vardecl(envi, envi.varOccurrences.idx.get(vdi->e()->id()), vardeclQueue);
                  continue;
                }
              }
              // Simplify the constraint *item (which depends on this variable)
              simplify_bool_constraint(envi, item, vd, remove, vardeclQueue, constraintQueue,
                                       toRemove, deletedVarDecls, nonFixedLiteralCount);
            }
          }
          // Actually remove all items that have become unnecessary in the step above
          for (auto i = static_cast<unsigned int>(toRemove.size()); (i--) != 0U;) {
            if (auto* ci = toRemove[i]->dynamicCast<ConstraintI>()) {
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
              top_down(cd, ci->e());
              ci->remove();
            } else {
              auto* vdi = toRemove[i]->cast<VarDeclI>();
              CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, vdi);
              top_down(cd, vdi->e()->e());
              vdi->e()->e(nullptr);
            }
          }
          if (remove) {
            deletedVarDecls.push_back(vd);
          } else {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        } else if (vd->type().isint() && (vd->ti()->domain() != nullptr)) {
          IntSetVal* isv = eval_intset(envi, vd->ti()->domain());
          if (auto* il = Expression::dynamicCast<IntLit>(vd->e())) {
            auto iv = IntLit::v(il);
            if (!isv->contains(iv)) {
              env.envi().fail();
            } else if (isv->size() != 1 || isv->card() != 1) {
              isv = IntSetVal::a(iv, iv);
              vd->ti()->domain(new SetLit(Location().introduce(), isv));
              push_dependent_constraints(envi, vd->id(), constraintQueue);
            }
          }
          if (isv->size() == 1 && isv->card() == 1) {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        } else if (vd->type().isfloat() && (vd->ti()->domain() != nullptr)) {
          FloatSetVal* fsv = eval_floatset(envi, vd->ti()->domain());
          if (fsv->size() == 1 && fsv->card() == 1) {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        } else if (vd->type().isIntSet() && (vd->e() != nullptr)) {
          simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
        }
      }  // end of processing of variable queue

      // Now handle all non-boolean constraints (i.e. anything except forall, clause, exists)
      bool handledConstraint = false;
      while (!handledConstraint && !constraintQueue.empty()) {
        envi.checkCancel();

        Item* item = constraintQueue.front();
        constraintQueue.pop_front();
        Call* c;
        ArrayLit* al = nullptr;
        if (auto* ci = item->dynamicCast<ConstraintI>()) {
          ci->flag(false);
          c = Expression::dynamicCast<Call>(ci->e());
        } else {
          if (item->removed()) {
            // This variable was removed because of unification, so we look up the
            // variable it was unified to
            item = m[envi.varOccurrences.find(item->cast<VarDeclI>()->e()->id()->decl())]
                       ->cast<VarDeclI>();
          }
          item->cast<VarDeclI>()->flag(false);
          c = Expression::dynamicCast<Call>(item->cast<VarDeclI>()->e()->e());
          al = Expression::dynamicCast<ArrayLit>(item->cast<VarDeclI>()->e()->e());
        }
        if (!item->removed()) {
          if (al != nullptr && !al->type().structBT()) {
            // Substitute all fixed variables by their values in array literals, then
            // push all constraints that depend on the array
            substitute_fixed_vars(envi, item, deletedVarDecls);
            push_dependent_constraints(envi, item->cast<VarDeclI>()->e()->id(), constraintQueue);
          } else {
            // For any constraint that is not forall, exists or clause,
            // substitute fixed arguments, then simplify it
            substitute_fixed_vars(envi, item, deletedVarDecls);
            handledConstraint =
                simplify_constraint(envi, item, deletedVarDecls, constraintQueue, vardeclQueue);
          }
        }
      }
    }

    // Clean up constraints that have been removed in the previous phase
    for (auto i = static_cast<unsigned int>(toRemoveConstraints.size()); (i--) != 0U;) {
      auto* ci = m[toRemoveConstraints[i]]->cast<ConstraintI>();
      CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, ci);
      top_down(cd, ci->e());
      ci->remove();
    }

    // Phase 4: handle boolean constraints again (todo: check if we can
    // refactor this into a separate function)
    //
    // Difference to phase 2: constraint argument arrays are actually shortened here if possible
    for (auto i = static_cast<unsigned int>(boolConstraints.size()); (i--) != 0U;) {
      env.envi().checkCancel();
      Item* bi = m[boolConstraints[i]];
      if (bi->removed()) {
        continue;
      }
      Call* c;
      std::vector<VarDecl*> removedVarDecls;

      if (bi->isa<ConstraintI>()) {
        c = Expression::dynamicCast<Call>(bi->cast<ConstraintI>()->e());
      } else {
        c = Expression::dynamicCast<Call>(bi->cast<VarDeclI>()->e()->e());
      }
      if (c == nullptr ||
          !(c->id() == envi.constants.ids.forall || c->id() == envi.constants.ids.exists ||
            c->id() == envi.constants.ids.clause)) {
        continue;
      }
      bool isConjunction = (c->id() == envi.constants.ids.forall);
      bool subsumed = false;
      bool empty = true;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        bool unit = (j == 0 ? isConjunction : !isConjunction);
        auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(j)));
        std::vector<Expression*> compactedAl;
        for (unsigned int k = 0; k < al->size(); k++) {
          if (Id* ident = Expression::dynamicCast<Id>((*al)[k])) {
            if (ident->decl()->ti()->domain() != nullptr) {
              if (!(ident->decl()->ti()->domain() == envi.constants.boollit(unit))) {
                subsumed = true;
              }
              removedVarDecls.push_back(ident->decl());
            } else {
              compactedAl.push_back(ident);
            }
          } else {
            if (Expression::cast<BoolLit>((*al)[k])->v() != unit) {
              subsumed = true;
            }
          }
        }
        if (compactedAl.size() < al->size()) {
          c->arg(j, new ArrayLit(Expression::loc(al), compactedAl));
          Expression::type(c->arg(j), Type::varbool(1));
        }
        empty = empty && compactedAl.empty();
      }
      if (subsumed) {
        if (bi->isa<ConstraintI>()) {
          if (isConjunction) {
            env.envi().fail();
          } else {
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<ConstraintI>()->e());
            bi->remove();
          }
        } else {
          if (isConjunction) {
            auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(0)));
            for (unsigned int j = 0; j < al->size(); j++) {
              removedVarDecls.push_back(Expression::cast<Id>((*al)[j])->decl());
            }
          } else {
            CollectDecls cd(envi, envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<VarDeclI>()->e()->e());
          }
          bool result = !isConjunction;
          auto* ti = bi->cast<VarDeclI>()->e()->ti();
          if (ti->domain() != nullptr) {
            if (Expression::equal(ti->domain(), env.envi().constants.boollit(!result))) {
              env.envi().fail();
            }
          } else {
            ti->domain(envi.constants.boollit(result));
          }
          ti->setComputedDomain(true);
          bi->cast<VarDeclI>()->e()->e(envi.constants.boollit(result));
        }
      } else if (empty) {
        bool result = isConjunction;
        if (bi->isa<ConstraintI>()) {
          if (result) {
            bi->remove();
          } else {
            env.envi().fail();
          }
        } else {
          auto* ti = bi->cast<VarDeclI>()->e()->ti();
          if (ti->domain() != nullptr) {
            if (Expression::equal(ti->domain(), env.envi().constants.boollit(!result))) {
              env.envi().fail();
            }
          } else {
            ti->domain(envi.constants.boollit(result));
          }
          ti->setComputedDomain(true);
          bi->cast<VarDeclI>()->e()->e(envi.constants.boollit(result));
        }
      }

      for (auto& removedVarDecl : removedVarDecls) {
        if (env.envi().varOccurrences.remove(removedVarDecl, bi) == 0) {
          if ((removedVarDecl->e() == nullptr || removedVarDecl->ti()->domain() == nullptr ||
               removedVarDecl->ti()->computedDomain()) &&
              !is_output(removedVarDecl)) {
            deletedVarDecls.push_back(removedVarDecl);
          }
        }
      }
      if (auto* vdi = bi->dynamicCast<VarDeclI>()) {
        if (envi.varOccurrences.occurrences(vdi->e()) == 0) {
          if ((vdi->e()->e() == nullptr || vdi->e()->ti()->domain() == nullptr ||
               vdi->e()->ti()->computedDomain()) &&
              !is_output(vdi->e())) {
            deletedVarDecls.push_back(vdi->e());
          }
        }
      }
    }
    // Phase 5: Chain Breaking
    env.envi().checkCancel();
    if (chain_compression) {
      ImpCompressor imp(envi, m, deletedVarDecls, boolConstraints);
      LECompressor le(envi, m, deletedVarDecls);
      for (auto& item : m) {
        imp.trackItem(item);
        le.trackItem(item);
      }
      envi.checkCancel();
      imp.compress();

      envi.checkCancel();
      le.compress();
    }

    // Phase 6: remove deleted variables if possible
    remove_deleted_items(envi, deletedVarDecls);
  } catch (ModelInconsistent&) { /* NOLINT(bugprone-empty-catch) */
  }
}

class SubstitutionVisitor : public EVisitor {
protected:
  std::vector<VarDecl*> _removed;

public:
  Expression* subst(Expression* e) {
    if (auto* vd = Expression::dynamicCast<VarDecl>(follow_id_to_decl(e))) {
      if (vd->type().isbool() && (vd->ti()->domain() != nullptr)) {
        _removed.push_back(vd);
        return vd->ti()->domain();
      }
      if (vd->type().isint()) {
        if ((vd->e() != nullptr) && Expression::isa<IntLit>(vd->e())) {
          _removed.push_back(vd);
          return vd->e();
        }
        if ((vd->ti()->domain() != nullptr) && Expression::isa<SetLit>(vd->ti()->domain()) &&
            Expression::cast<SetLit>(vd->ti()->domain())->isv()->size() == 1 &&
            Expression::cast<SetLit>(vd->ti()->domain())->isv()->min() ==
                Expression::cast<SetLit>(vd->ti()->domain())->isv()->max()) {
          _removed.push_back(vd);
          return IntLit::a(Expression::cast<SetLit>(vd->ti()->domain())->isv()->min());
        }
      }
      if (vd->type().isfloat()) {
        if ((vd->e() != nullptr) && Expression::isa<FloatLit>(vd->e())) {
          _removed.push_back(vd);
          return vd->e();
        }
        if ((vd->ti()->domain() != nullptr) && Expression::isa<SetLit>(vd->ti()->domain()) &&
            Expression::cast<SetLit>(vd->ti()->domain())->fsv()->size() == 1 &&
            Expression::cast<SetLit>(vd->ti()->domain())->fsv()->min() ==
                Expression::cast<SetLit>(vd->ti()->domain())->fsv()->max()) {
          _removed.push_back(vd);
          return FloatLit::a(Expression::cast<SetLit>(vd->ti()->domain())->fsv()->min());
        }
      }
      if (vd->type().isIntSet()) {
        if ((vd->e() != nullptr) && Expression::isa<SetLit>(vd->e())) {
          _removed.push_back(vd);
          return vd->e();
        }
      }
    }
    return e;
  }
  /// Visit array literal
  void vArrayLit(ArrayLit* al) {
    for (unsigned int i = 0; i < al->size(); i++) {
      al->set(i, subst((*al)[i]));
    }
  }
  /// Visit call
  void vCall(Call* c) {
    for (unsigned int i = 0; i < c->argCount(); i++) {
      c->arg(i, subst(c->arg(i)));
    }
  }
  /// Determine whether to enter node
  static bool enter(Expression* e) { return !Expression::isa<Id>(e); }
  void remove(EnvI& env, Item* item, std::vector<VarDecl*>& deletedVarDecls) {
    for (auto& i : _removed) {
      Expression::ann(i).remove(env.constants.ann.is_defined_var);
      if (env.varOccurrences.remove(i, item) == 0) {
        if ((i->e() == nullptr || i->ti()->domain() == nullptr || i->ti()->computedDomain()) &&
            !is_output(i)) {
          deletedVarDecls.push_back(i);
        }
      }
    }
    // If we are replacing in an array and it is now all par, then remove it (or
    // move to output model)
    if (auto* vdi = Item::dynamicCast<VarDeclI>(item)) {
      if (env.varOccurrences.occurrences(vdi->e()) == 0) {
        if (auto* al = Expression::dynamicCast<ArrayLit>(vdi->e()->e())) {
          for (unsigned int i = 0; i < al->size(); i++) {
            if (!Expression::type((*al)[i]).isPar()) {
              return;
            }
          }
          Type nt = vdi->e()->type();
          nt.mkPar(env);
          vdi->e()->ti()->type(nt);
          vdi->e()->type(nt);
          Expression::type(vdi->e()->e(), nt);
          deletedVarDecls.push_back(vdi->e());
        }
      }
    }
  }
};

void substitute_fixed_vars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls) {
  SubstitutionVisitor sv;
  if (auto* ci = ii->dynamicCast<ConstraintI>()) {
    top_down(sv, ci->e());
    for (ExpressionSetIter it = Expression::ann(ci->e()).begin();
         it != Expression::ann(ci->e()).end(); ++it) {
      top_down(sv, *it);
    }
  } else if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
    top_down(sv, vdi->e());
    for (ExpressionSetIter it = Expression::ann(vdi->e()).begin();
         it != Expression::ann(vdi->e()).end(); ++it) {
      top_down(sv, *it);
    }
  } else {
    auto* si = ii->cast<SolveI>();
    if (si->e() != nullptr) {
      si->e(sv.subst(si->e()));
      top_down(sv, si->e());
    }
    for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
      top_down(sv, *it);
    }
  }
  sv.remove(env, ii, deletedVarDecls);
}

bool simplify_constraint(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls,
                         std::deque<Item*>& constraintQueue,
                         std::deque<unsigned int>& vardeclQueue) {
  Expression* con_e;
  bool is_true;
  bool is_false;
  if (auto* ci = ii->dynamicCast<ConstraintI>()) {
    con_e = ci->e();
    is_true = true;
    is_false = false;
  } else {
    auto* vdi = ii->cast<VarDeclI>();
    con_e = vdi->e()->e();
    is_true = (vdi->e()->type().isbool() && vdi->e()->ti()->domain() == env.constants.literalTrue);
    is_false =
        (vdi->e()->type().isbool() && vdi->e()->ti()->domain() == env.constants.literalFalse);
    assert(is_true || is_false || !vdi->e()->type().isbool() ||
           vdi->e()->ti()->domain() == nullptr);
  }
  if (Call* c = Expression::dynamicCast<Call>(con_e)) {
    if (c->id() == env.constants.ids.int_.eq || c->id() == env.constants.ids.bool_.eq ||
        c->id() == env.constants.ids.float_.eq || c->id() == env.constants.ids.set_.eq) {
      if (is_true && Expression::isa<Id>(c->arg(0)) && Expression::isa<Id>(c->arg(1)) &&
          (Expression::cast<Id>(c->arg(0))->decl()->e() == nullptr ||
           Expression::cast<Id>(c->arg(1))->decl()->e() == nullptr)) {
        if (Call* defVar = Expression::ann(c).getCall(env.constants.ann.defines_var)) {
          // First, remove defines_var/is_defined_var annotations if present
          if (Expression::equal(defVar->arg(0), c->arg(0))) {
            Expression::ann(Expression::cast<Id>(c->arg(0))->decl())
                .remove(env.constants.ann.is_defined_var);
          } else {
            Expression::ann(Expression::cast<Id>(c->arg(1))->decl())
                .remove(env.constants.ann.is_defined_var);
          }
        }
        unify(env, deletedVarDecls, Expression::cast<Id>(c->arg(0)),
              Expression::cast<Id>(c->arg(1)));
        push_dependent_constraints(env, Expression::cast<Id>(c->arg(0)), constraintQueue);
        CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
        top_down(cd, c);
        if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
          vdi->e()->e(env.constants.literalTrue);
          deletedVarDecls.push_back(vdi->e());
        } else {
          ii->remove();
        }
      } else if (Expression::type(c->arg(0)).isPar() && Expression::type(c->arg(1)).isPar()) {
        Expression* e0 = eval_par(env, c->arg(0));
        Expression* e1 = eval_par(env, c->arg(1));
        bool is_equal = Expression::equal(e0, e1);
        if ((is_true && is_equal) || (is_false && !is_equal)) {
          // do nothing
        } else if ((is_true && !is_equal) || (is_false && is_equal)) {
          env.fail();
        } else {
          auto* vdi = ii->cast<VarDeclI>();
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          vdi->e()->e(env.constants.boollit(is_equal));
          vdi->e()->ti()->domain(env.constants.boollit(is_equal));
          vdi->e()->ti()->setComputedDomain(true);
          push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
          push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
        }
        if (ii->isa<ConstraintI>()) {
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          ii->remove();
        }
      } else if (is_true &&
                 ((Expression::isa<Id>(c->arg(0)) && Expression::type(c->arg(1)).isPar()) ||
                  (Expression::isa<Id>(c->arg(1)) && Expression::type(c->arg(0)).isPar()))) {
        Id* ident = Expression::isa<Id>(c->arg(0)) ? Expression::cast<Id>(c->arg(0))
                                                   : Expression::cast<Id>(c->arg(1));
        Expression* arg = Expression::isa<Id>(c->arg(0)) ? c->arg(1) : c->arg(0);
        bool canRemove = false;
        TypeInst* ti = ident->decl()->ti();
        switch (ident->type().bt()) {
          case Type::BT_BOOL:
            if (ti->domain() == nullptr) {
              ti->domain(env.constants.boollit(eval_bool(env, arg)));
              ti->setComputedDomain(false);
              canRemove = true;
            } else {
              if (eval_bool(env, ti->domain()) == eval_bool(env, arg)) {
                canRemove = true;
              } else {
                env.fail();
                canRemove = true;
              }
            }
            break;
          case Type::BT_INT: {
            if (ident->type().st() == Type::ST_SET) {
              GCLock lock;
              IntSetVal* isv = eval_intset(env, arg);
              if (ti->domain() != nullptr) {
                IntSetVal* dom = eval_intset(env, ti->domain());
                IntSetRanges domr(dom);
                IntSetRanges slr(isv);
                if (!Ranges::subset(slr, domr)) {
                  env.fail();
                  canRemove = true;
                }
              }
              if (ident->decl()->e() == nullptr) {
                ident->decl()->e(new SetLit(Expression::loc(arg), isv));
                canRemove = true;
              } else if (auto* call = Expression::dynamicCast<Call>(ident->decl()->e())) {
                // Remove call from RHS and add it as new constraint with the literal
                auto* sl = new SetLit(Expression::loc(arg), isv);
                std::vector<Expression*> args(call->argCount() + 1);
                for (unsigned int i = 0; i < call->argCount(); ++i) {
                  args[i] = call->arg(i);
                }
                args[call->argCount()] = sl;
                auto* nc = Call::a(Expression::loc(call), call->id(), args);
                nc->type(Type::varbool());
                nc->decl(env.model->matchFn(env, nc, false));
                env.flatAddItem(new ConstraintI(Expression::loc(call), nc));

                // Add literal as new RHS
                ident->decl()->e(sl);
                canRemove = true;
              } else {
                IntSetVal* rhs = eval_intset(env, ident->decl()->e());
                if (!rhs->equal(isv)) {
                  env.fail();
                }
                canRemove = true;
              }
            } else {
              IntVal d = eval_int(env, arg);
              if (ti->domain() == nullptr) {
                ti->domain(new SetLit(Location().introduce(), IntSetVal::a(d, d)));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                IntSetVal* isv = eval_intset(env, ti->domain());
                if (isv->contains(d)) {
                  ident->decl()->ti()->domain(
                      new SetLit(Location().introduce(), IntSetVal::a(d, d)));
                  ident->decl()->ti()->setComputedDomain(false);
                  canRemove = true;
                } else {
                  env.fail();
                  canRemove = true;
                }
              }
            }
          } break;
          case Type::BT_FLOAT: {
            if (ti->domain() == nullptr) {
              ti->domain(new BinOp(Location().introduce(), arg, BOT_DOTDOT, arg));
              ti->setComputedDomain(false);
              canRemove = true;
            } else {
              FloatVal value = eval_float(env, arg);
              if (LinearTraits<FloatLit>::domainContains(eval_floatset(env, ti->domain()), value)) {
                ti->domain(new BinOp(Location().introduce(), arg, BOT_DOTDOT, arg));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                env.fail();
                canRemove = true;
              }
            }
          } break;
          default:
            break;
        }
        if (ident->decl()->e() == nullptr) {
          ident->decl()->e(Expression::isa<Id>(c->arg(0)) ? c->arg(1) : c->arg(0));
          ti->setComputedDomain(true);
          canRemove = true;
        }

        if (Expression::isa<Call>(ident->decl()->e())) {
          constraintQueue.push_back((*env.flat())[env.varOccurrences.find(ident->decl())]);
        }
        push_dependent_constraints(env, ident, constraintQueue);
        if (canRemove) {
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
            if (env.varOccurrences.occurrences(vdi->e()) == 0) {
              if (is_output(vdi->e())) {
                VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                      ->cast<VarDeclI>()
                                      ->e();
                vd_out->e(env.constants.boollit(is_true));
              }
              vdi->remove();
            } else {
              vdi->e()->e(env.constants.boollit(is_true));
            }
          } else {
            ii->remove();
          }
        }
      }
    } else if ((is_true || is_false) && c->id() == env.constants.ids.int_.le &&
               ((Expression::isa<Id>(c->arg(0)) && Expression::type(c->arg(1)).isPar()) ||
                (Expression::isa<Id>(c->arg(1)) && Expression::type(c->arg(0)).isPar()))) {
      Id* ident = Expression::isa<Id>(c->arg(0)) ? Expression::cast<Id>(c->arg(0))
                                                 : Expression::cast<Id>(c->arg(1));
      Expression* arg = Expression::isa<Id>(c->arg(0)) ? c->arg(1) : c->arg(0);
      IntSetVal* domain = ident->decl()->ti()->domain() != nullptr
                              ? eval_intset(env, ident->decl()->ti()->domain())
                              : nullptr;
      if (domain != nullptr) {
        BinOpType bot = Expression::isa<Id>(c->arg(0)) ? (is_true ? BOT_LQ : BOT_GR)
                                                       : (is_true ? BOT_GQ : BOT_LE);
        IntSetVal* newDomain = LinearTraits<IntLit>::limitDomain(bot, domain, eval_int(env, arg));
        if (newDomain->empty()) {
          env.fail();
        } else {
          ident->decl()->ti()->domain(new SetLit(Location().introduce(), newDomain));
          ident->decl()->ti()->setComputedDomain(false);

          if (newDomain->min() == newDomain->max()) {
            push_dependent_constraints(env, ident, constraintQueue);
          }
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);

          if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
            vdi->e()->e(env.constants.boollit(is_true));
            push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
            if (env.varOccurrences.occurrences(vdi->e()) == 0) {
              if (is_output(vdi->e())) {
                VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                      ->cast<VarDeclI>()
                                      ->e();
                vd_out->e(env.constants.boollit(is_true));
              }
              vdi->remove();
            }
          } else {
            ii->remove();
          }
        }
      }
    } else if (c->id() == env.constants.ids.bool2int) {
      auto* vdi = ii->dynamicCast<VarDeclI>();
      VarDecl* vd;
      bool fixed = false;
      bool b_val = false;
      if (vdi != nullptr) {
        vd = vdi->e();
      } else if (Id* ident = Expression::dynamicCast<Id>(c->arg(1))) {
        vd = ident->decl();
      } else {
        vd = nullptr;
      }
      IntSetVal* vd_dom = nullptr;
      if (vd != nullptr) {
        if (vd->ti()->domain() != nullptr) {
          vd_dom = eval_intset(env, vd->ti()->domain());
          assert(!vd_dom->empty());
          if (vd_dom->max() < 0 || vd_dom->min() > 1) {
            env.fail();
            return true;
          }
          fixed = vd_dom->min() == vd_dom->max();
          b_val = (vd_dom->min() == 1);
        }
      } else {
        fixed = true;
        b_val = (eval_int(env, c->arg(1)) == 1);
      }
      if (fixed) {
        if (Expression::type(c->arg(0)).isPar()) {
          bool b2i_val = eval_bool(env, c->arg(0));
          if (b2i_val != b_val) {
            env.fail();
          } else {
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
              auto* v = IntLit::a(b2i_val ? 1 : 0);
              if (env.varOccurrences.occurrences(vdi->e()) == 0) {
                if (is_output(vdi->e())) {
                  VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                        ->cast<VarDeclI>()
                                        ->e();
                  vd_out->e(v);
                }
                vdi->remove();
              } else {
                vdi->e()->e(v);
              }
            } else {
              ii->remove();
            }
          }
        } else {
          Id* ident = Expression::cast<Id>(c->arg(0));
          TypeInst* ti = ident->decl()->ti();
          if (ti->domain() == nullptr) {
            ti->domain(env.constants.boollit(b_val));
            ti->setComputedDomain(false);
          } else if (eval_bool(env, ti->domain()) != b_val) {
            env.fail();
          }
          CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          if (vd != nullptr) {
            vd->e(IntLit::a(static_cast<long long>(b_val)));
            vd->ti()->setComputedDomain(true);
          }
          push_dependent_constraints(env, ident, constraintQueue);
          if (vdi != nullptr) {
            if (env.varOccurrences.occurrences(vd) == 0) {
              deletedVarDecls.push_back(vdi->e());
            }
          } else {
            ii->remove();
          }
        }
      } else {
        IntVal v = -1;
        if (auto* bl = Expression::dynamicCast<BoolLit>(c->arg(0))) {
          v = bl->v() ? 1 : 0;
        } else if (Id* ident = Expression::dynamicCast<Id>(c->arg(0))) {
          if (ident->decl()->ti()->domain() != nullptr) {
            v = eval_bool(env, ident->decl()->ti()->domain()) ? 1 : 0;
          }
        }
        if (v != -1) {
          if ((vd_dom != nullptr) && !vd_dom->contains(v)) {
            env.fail();
          } else {
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vd->e(IntLit::a(v));
            vd->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(v, v)));
            vd->ti()->setComputedDomain(true);
            push_vardecl(env, env.varOccurrences.find(vd), vardeclQueue);
            push_dependent_constraints(env, vd->id(), constraintQueue);
          }
        }
      }

    } else {
      // General propagation: call a propagator registered for this constraint type
      Expression* rewrite = nullptr;
      GCLock lock;
      switch (OptimizeRegistry::registry().process(env, ii, c, rewrite)) {
        case OptimizeRegistry::CS_NONE:
          return false;
        case OptimizeRegistry::CS_OK:
          return true;
        case OptimizeRegistry::CS_FAILED:
          if (is_true) {
            env.fail();
            return true;
          } else if (is_false) {
            if (ii->isa<ConstraintI>()) {
              CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
              top_down(cd, c);
              ii->remove();
            } else {
              deletedVarDecls.push_back(ii->cast<VarDeclI>()->e());
            }
            return true;
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->ti()->domain(env.constants.literalFalse);
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vdi->e()->e(env.constants.literalFalse);
            push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            return true;
          }
        case OptimizeRegistry::CS_ENTAILED:
          if (is_true) {
            if (ii->isa<ConstraintI>()) {
              CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
              top_down(cd, c);
              ii->remove();
            } else {
              deletedVarDecls.push_back(ii->cast<VarDeclI>()->e());
            }
            return true;
          } else if (is_false) {
            env.fail();
            return true;
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->ti()->domain(env.constants.literalTrue);
            CollectDecls cd(env, env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vdi->e()->e(env.constants.literalTrue);
            push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            return true;
          }
        case OptimizeRegistry::CS_REWRITE: {
          std::vector<VarDecl*> tdv;
          CollectDecls cd(env, env.varOccurrences, tdv, ii);
          top_down(cd, c);

          CollectOccurrencesE ce(env, env.varOccurrences, ii);
          top_down(ce, rewrite);

          for (auto& i : tdv) {
            if (env.varOccurrences.occurrences(i) == 0) {
              deletedVarDecls.push_back(i);
            }
          }

          assert(rewrite != nullptr);
          if (auto* ci = ii->dynamicCast<ConstraintI>()) {
            ci->e(rewrite);
            constraintQueue.push_back(ii);
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->e(rewrite);
            if ((vdi->e()->e() != nullptr) && Expression::isa<Id>(vdi->e()->e()) &&
                vdi->e()->type().dim() == 0) {
              Id* id1 = Expression::cast<Id>(vdi->e()->e());
              vdi->e()->e(nullptr);
              // Transfer is_defined_var annotation
              if (Expression::ann(id1->decl()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(vdi->e(), env.constants.ann.is_defined_var);
              } else if (Expression::ann(vdi->e()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(id1->decl(), env.constants.ann.is_defined_var);
              }
              unify(env, deletedVarDecls, vdi->e()->id(), id1);
              push_dependent_constraints(env, id1, constraintQueue);
            }
            if ((vdi->e()->e() != nullptr) && Expression::type(vdi->e()->e()).isPar() &&
                (vdi->e()->ti()->domain() != nullptr)) {
              if (Expression::type(vdi->e()->e()).isint()) {
                IntVal iv = eval_int(env, vdi->e()->e());
                IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                if (!dom->contains(iv)) {
                  env.fail();
                }
              } else if (Expression::type(vdi->e()->e()).isIntSet()) {
                IntSetVal* isv = eval_intset(env, vdi->e()->e());
                IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                IntSetRanges isv_r(isv);
                IntSetRanges dom_r(dom);
                if (!Ranges::subset(isv_r, dom_r)) {
                  env.fail();
                }
              } else if (Expression::type(vdi->e()->e()).isfloat()) {
                FloatVal fv = eval_float(env, vdi->e()->e());
                FloatSetVal* dom = eval_floatset(env, vdi->e()->ti()->domain());
                if (!dom->contains(fv)) {
                  env.fail();
                }
              } else if (Expression::type(vdi->e()->e()).isFloatSet()) {
                FloatSetVal* fsv = eval_floatset(env, vdi->e()->e());
                FloatSetVal* dom = eval_floatset(env, vdi->e()->ti()->domain());
                FloatSetRanges fsv_r(fsv);
                FloatSetRanges dom_r(dom);
                if (!Ranges::subset(fsv_r, dom_r)) {
                  env.fail();
                }
              }
            }
            if (vdi->e()->ti()->type() != Type::varbool() || vdi->e()->ti()->domain() == nullptr) {
              push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            }

            if (is_true) {
              constraintQueue.push_back(ii);
            }
          }
          return true;
        }
      }
    }
  }
  return false;
}

int bool_state(EnvI& env, Expression* e) {
  if (Expression::type(e).isPar()) {
    return static_cast<int>(eval_bool(env, e));
  }
  Id* id = Expression::cast<Id>(e);
  if (id->decl()->ti()->domain() == nullptr) {
    return 2;
  }
  return static_cast<int>(id->decl()->ti()->domain() == env.constants.literalTrue);
}

int decrement_non_fixed_vars(std::unordered_map<Expression*, int>& nonFixedLiteralCount, Call* c) {
  auto it = nonFixedLiteralCount.find(c);
  if (it == nonFixedLiteralCount.end()) {
    int nonFixedVars = 0;
    for (unsigned int i = 0; i < c->argCount(); i++) {
      auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(i)));
      nonFixedVars += static_cast<int>(al->size());
      for (unsigned int j = al->size(); (j--) != 0U;) {
        if (Expression::type((*al)[j]).isPar() ||
            (Expression::isa<Id>((*al)[j]) &&
             Expression::cast<Id>((*al)[j])->decl()->ti()->domain() != nullptr)) {
          nonFixedVars--;
        }
      }
    }
    nonFixedLiteralCount.insert(std::make_pair(c, nonFixedVars));
    return nonFixedVars;
  }
  it->second--;
  return it->second;
}

void simplify_bool_constraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::deque<unsigned int>& vardeclQueue,
                              std::deque<Item*>& constraintQueue, std::vector<Item*>& toRemove,
                              std::vector<VarDecl*>& deletedVarDecls,
                              std::unordered_map<Expression*, int>& nonFixedLiteralCount) {
  if (ii->isa<SolveI>()) {
    remove = false;
    return;
  }
  bool isTrue = vd->ti()->domain() == env.constants.literalTrue;
  Expression* e = nullptr;
  auto* ci = ii->dynamicCast<ConstraintI>();
  auto* vdi = ii->dynamicCast<VarDeclI>();

  Call* call;
  if (ci != nullptr) {
    call = Expression::dynamicCast<Call>(ci->e());
  } else {
    call = Expression::dynamicCast<Call>(vdi->e()->e());
  }

  if (call != nullptr) {
    // Check that the vd actually occurs in the arguments of the call,
    // and not just in an annotation
    bool foundVd = false;
    for (unsigned int i = 0; !foundVd && i < call->argCount(); i++) {
      if (call->arg(i) == vd->id()) {
        foundVd = true;
        break;
      }
      auto* a = Expression::dynamicCast<ArrayLit>(call->arg(i));
      if (a != nullptr) {
        for (unsigned int j = 0; j < a->size(); j++) {
          if (Expression::equal((*a)[j], vd->id())) {
            foundVd = true;
            break;
          }
        }
      }
    }

    if (!foundVd) {
      remove = false;
      return;
    }
  }

  if (ci != nullptr) {
    e = ci->e();

    if (vd->ti()->domain() != nullptr) {
      if (Call* definedVarCall = Expression::ann(e).getCall(env.constants.ann.defines_var)) {
        if (Expression::equal(definedVarCall->arg(0), vd->id())) {
          Expression::ann(e).removeCall(env.constants.ann.defines_var);
          Expression::ann(vd).remove(env.constants.ann.is_defined_var);
        }
      }
    }

  } else if (vdi != nullptr) {
    e = vdi->e()->e();
    if (e == nullptr) {
      return;
    }
    if (Id* id = Expression::dynamicCast<Id>(e)) {
      assert(id->decl() == vd);
      if (vdi->e()->ti()->domain() == nullptr) {
        vdi->e()->ti()->domain(env.constants.boollit(isTrue));
        vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
      } else if (id->decl()->ti()->domain() == env.constants.boollit(!isTrue)) {
        env.fail();
        remove = false;
      }
      return;
    }
  }
  if (Id* ident = Expression::dynamicCast<Id>(e)) {
    assert(ident->decl() == vd);
    return;
  }
  if (Expression::isa<BoolLit>(e)) {
    if (e == env.constants.literalTrue && (ci != nullptr)) {
      toRemove.push_back(ci);
    }
    return;
  }
  Call* c = Expression::cast<Call>(e);
  if (c->id() == env.constants.ids.bool_.eq) {
    Expression* b0 = c->arg(0);
    Expression* b1 = c->arg(1);
    int b0s = bool_state(env, b0);
    int b1s = bool_state(env, b1);
    if (b0s == 2) {
      std::swap(b0, b1);
      std::swap(b0s, b1s);
    }
    assert(b0s != 2);
    if (b0s == 2) {
      // Should never happen, but to be safe, just do nothing in release mode
      remove = false;
      return;
    }
    if ((ci != nullptr) || vdi->e()->ti()->domain() == env.constants.literalTrue) {
      if (b0s != b1s) {
        if (b1s == 2) {
          /// b0 is fixed, b1 is not fixed, so make them equal so that the ci/vdi is true
          Expression::cast<Id>(b1)->decl()->ti()->domain(env.constants.boollit(b0s == 1));
          vardeclQueue.push_back(
              env.varOccurrences.idx.get(Expression::cast<Id>(b1)->decl()->id()));
          if (ci != nullptr) {
            toRemove.push_back(ci);
          }
        } else {
          env.fail();
          remove = false;
        }
      } else {
        if (ci != nullptr) {
          toRemove.push_back(ci);
        }
      }
    } else if ((vdi != nullptr) && vdi->e()->ti()->domain() == env.constants.literalFalse) {
      if (b0s != b1s) {
        if (b1s == 2) {
          /// b0 is fixed, b1 is not fixed, so make them different so that vdi is false
          Expression::cast<Id>(b1)->decl()->ti()->domain(env.constants.boollit(b0s == 0));
          vardeclQueue.push_back(
              env.varOccurrences.idx.get(Expression::cast<Id>(b1)->decl()->id()));
        }
      } else {
        env.fail();
        remove = false;
      }
    } else {
      remove = false;
    }
  } else if (c->id() == env.constants.ids.forall || c->id() == env.constants.ids.exists ||
             c->id() == env.constants.ids.clause) {
    if (isTrue && c->id() == env.constants.ids.exists) {
      if (ci != nullptr) {
        toRemove.push_back(ci);
      } else {
        if (vdi->e()->ti()->domain() == nullptr) {
          vdi->e()->ti()->domain(env.constants.literalTrue);
          vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
        } else if (vdi->e()->ti()->domain() != env.constants.literalTrue) {
          env.fail();
          vdi->e()->e(env.constants.literalTrue);
        }
      }
    } else if (!isTrue && c->id() == env.constants.ids.forall) {
      if (ci != nullptr) {
        env.fail();
        toRemove.push_back(ci);
      } else {
        if (vdi->e()->ti()->domain() == nullptr) {
          vdi->e()->ti()->domain(env.constants.literalFalse);
          vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
        } else if (vdi->e()->ti()->domain() != env.constants.literalFalse) {
          env.fail();
          vdi->e()->e(env.constants.literalFalse);
        }
      }
    } else {
      int nonfixed = decrement_non_fixed_vars(nonFixedLiteralCount, c);
      bool isConjunction = (c->id() == env.constants.ids.forall);
      assert(nonfixed >= 0);
      if (nonfixed <= 1) {
        bool subsumed = false;
        int nonfixed_i = -1;
        int nonfixed_j = -1;
        int realNonFixed = 0;
        for (unsigned int i = 0; i < c->argCount(); i++) {
          bool unit = (i == 0 ? isConjunction : !isConjunction);
          auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(i)));
          realNonFixed += static_cast<int>(al->size());
          for (unsigned int j = al->size(); (j--) != 0U;) {
            if (Expression::type((*al)[j]).isPar() ||
                (Expression::cast<Id>((*al)[j])->decl()->ti()->domain() != nullptr)) {
              realNonFixed--;
            }
            if (Expression::type((*al)[j]).isPar() && eval_bool(env, (*al)[j]) != unit) {
              subsumed = true;
              i = 2;  // break out of outer loop
              break;
            }
            if (Id* id = Expression::dynamicCast<Id>((*al)[j])) {
              if (id->decl()->ti()->domain() != nullptr) {
                bool idv = (id->decl()->ti()->domain() == env.constants.literalTrue);
                if (unit != idv) {
                  subsumed = true;
                  i = 2;  // break out of outer loop
                  break;
                }
              } else {
                nonfixed_i = static_cast<int>(i);
                nonfixed_j = static_cast<int>(j);
              }
            }
          }
        }

        if (subsumed) {
          if (ci != nullptr) {
            if (isConjunction) {
              env.fail();
              ci->e(env.constants.literalFalse);
            } else {
              toRemove.push_back(ci);
            }
          } else {
            if (vdi->e()->ti()->domain() == nullptr) {
              vdi->e()->ti()->domain(env.constants.boollit(!isConjunction));
              vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
            } else if (vdi->e()->ti()->domain() != env.constants.boollit(!isConjunction)) {
              env.fail();
              vdi->e()->e(env.constants.boollit(!isConjunction));
            }
          }
        } else if (realNonFixed == 0) {
          if (ci != nullptr) {
            if (isConjunction) {
              toRemove.push_back(ci);
            } else {
              env.fail();
              ci->e(env.constants.literalFalse);
            }
          } else {
            if (vdi->e()->ti()->domain() == nullptr) {
              vdi->e()->ti()->domain(env.constants.boollit(isConjunction));
              vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
            } else if (vdi->e()->ti()->domain() != env.constants.boollit(isConjunction)) {
              env.fail();
              vdi->e()->e(env.constants.boollit(isConjunction));
            }
            toRemove.push_back(vdi);
          }
        } else if (realNonFixed == 1) {
          // not subsumed, nonfixed==1
          assert(nonfixed_i != -1);
          auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(nonfixed_i)));
          Id* ident = Expression::cast<Id>((*al)[nonfixed_j]);
          if ((ci != nullptr) || (vdi->e()->ti()->domain() != nullptr)) {
            bool result = nonfixed_i == 0;
            if ((vdi != nullptr) && vdi->e()->ti()->domain() == env.constants.literalFalse) {
              result = !result;
            }
            VarDecl* decl = ident->decl();
            if (decl->ti()->domain() == nullptr) {
              decl->ti()->domain(env.constants.boollit(result));
              vardeclQueue.push_back(env.varOccurrences.idx.get(decl->id()));
            } else if (vd->ti()->domain() != env.constants.boollit(result)) {
              env.fail();
              decl->e(env.constants.literalTrue);
            }
          } else {
            if (nonfixed_i == 0) {
              // this is a clause, exists or forall with a single non-fixed variable,
              // assigned to a non-fixed variable => turn into simple equality
              vdi->e()->e(nullptr);
              // Transfer is_defined_var annotation
              if (Expression::ann(ident->decl()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(vdi->e(), env.constants.ann.is_defined_var);
              } else if (Expression::ann(vdi->e()).contains(env.constants.ann.is_defined_var)) {
                Expression::addAnnotation(ident->decl(), env.constants.ann.is_defined_var);
              }
              unify(env, deletedVarDecls, vdi->e()->id(), ident);
              push_dependent_constraints(env, ident, constraintQueue);
            } else {
              remove = false;
            }
          }
        } else {
          remove = false;
        }

      } else if (c->id() == env.constants.ids.clause) {
        int posOrNeg = isTrue ? 0 : 1;
        auto* al = Expression::cast<ArrayLit>(follow_id(c->arg(posOrNeg)));
        auto* al_other = Expression::cast<ArrayLit>(follow_id(c->arg(1 - posOrNeg)));

        if ((ci != nullptr) && al->size() == 1 && (*al)[0] != vd->id() && al_other->size() == 1) {
          // simple implication
          assert((*al_other)[0] == vd->id());
          if (ci != nullptr) {
            if (Expression::type((*al)[0]).isPar()) {
              if (eval_bool(env, (*al)[0]) == isTrue) {
                toRemove.push_back(ci);
              } else {
                env.fail();
                remove = false;
              }
            } else {
              Id* id = Expression::cast<Id>((*al)[0]);
              if (id->decl()->ti()->domain() == nullptr) {
                id->decl()->ti()->domain(env.constants.boollit(isTrue));
                vardeclQueue.push_back(env.varOccurrences.idx.get(id->decl()->id()));
              } else {
                if (id->decl()->ti()->domain() == env.constants.boollit(isTrue)) {
                  toRemove.push_back(ci);
                } else {
                  env.fail();
                  remove = false;
                }
              }
            }
          }
        } else {
          // proper clause
          for (unsigned int i = 0; i < al->size(); i++) {
            if ((*al)[i] == vd->id()) {
              if (ci != nullptr) {
                toRemove.push_back(ci);
              } else {
                if (vdi->e()->ti()->domain() == nullptr) {
                  vdi->e()->ti()->domain(env.constants.literalTrue);
                  vardeclQueue.push_back(env.varOccurrences.idx.get(vdi->e()->id()));
                } else if (vdi->e()->ti()->domain() != env.constants.literalTrue) {
                  env.fail();
                  vdi->e()->e(env.constants.literalTrue);
                }
              }
              break;
            }
          }
        }
      }
    }
  } else {
    remove = false;
  }
}

void substitute_fixed_vars(Env& env) {
  env.envi().checkCancel();
  if (env.envi().failed()) {
    return;
  }
  try {
    EnvI& envi = env.envi();
    Model& m = *envi.flat();
    std::vector<VarDecl*> deletedVarDecls;
    for (auto* item : m) {
      if (!item->removed()) {
        substitute_fixed_vars(envi, item, deletedVarDecls);
      }
    }
    remove_deleted_items(envi, deletedVarDecls);
  } catch (ModelInconsistent&) { /* NOLINT(bugprone-empty-catch) */
  }
}

}  // namespace MiniZinc
