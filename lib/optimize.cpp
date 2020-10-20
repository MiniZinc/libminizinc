/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astiterator.hh>
#include <minizinc/chain_compressor.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/optimize.hh>
#include <minizinc/optimize_constraints.hh>
#include <minizinc/prettyprinter.hh>

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
  return it == idx.end() ? -1 : it->second;
}
void VarOccurrences::remove(VarDecl* vd) { idx.remove(vd->id()); }

void VarOccurrences::add(VarDecl* v, Item* i) {
  auto vi = itemMap.find(v->id()->decl()->id());
  if (vi == itemMap.end()) {
    Items items;
    items.insert(i);
    itemMap.insert(v->id()->decl()->id(), items);
  } else {
    vi->second.insert(i);
  }
}

int VarOccurrences::remove(VarDecl* v, Item* i) {
  auto vi = itemMap.find(v->id()->decl()->id());
  assert(vi != itemMap.end());
  vi->second.erase(i);
  return static_cast<int>(vi->second.size());
}

void VarOccurrences::removeAllOccurrences(VarDecl* v) {
  auto vi = itemMap.find(v->id()->decl()->id());
  assert(vi != itemMap.end());
  vi->second.clear();
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
  if (vi0 != itemMap.end()) {
    auto vi1 = itemMap.find(v1->id());
    if (vi1 == itemMap.end()) {
      itemMap.insert(v1->id(), vi0->second);
    } else {
      vi1->second.insert(vi0->second.begin(), vi0->second.end());
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
  return (vi == itemMap.end() ? 0 : static_cast<int>(vi->second.size()));
}

std::pair<int, bool> VarOccurrences::usages(VarDecl* v) {
  bool is_output = v->ann().contains(constants().ann.output_var) ||
                   v->ann().containsCall(constants().ann.output_array);
  auto vi = itemMap.find(v->id()->decl()->id());
  if (vi == itemMap.end()) {
    return std::make_pair(0, is_output);
  }
  int count = 0;
  for (Item* i : vi->second) {
    auto* vd = i->dynamicCast<VarDeclI>();
    if ((vd != nullptr) && (vd->e() != nullptr) && (vd->e()->e() != nullptr) &&
        (vd->e()->e()->isa<ArrayLit>() || vd->e()->e()->isa<SetLit>())) {
      auto u = usages(vd->e());
      is_output = is_output || u.second;
      count += u.first;
    } else {
      count++;
    }
  }
  return std::make_pair(count, is_output);
}

void CollectOccurrencesI::vVarDeclI(VarDeclI* v) {
  CollectOccurrencesE ce(vo, v);
  top_down(ce, v->e());
}
void CollectOccurrencesI::vConstraintI(ConstraintI* ci) {
  CollectOccurrencesE ce(vo, ci);
  top_down(ce, ci->e());
  for (ExpressionSetIter it = ci->e()->ann().begin(); it != ci->e()->ann().end(); ++it) {
    top_down(ce, *it);
  }
}
void CollectOccurrencesI::vSolveI(SolveI* si) {
  CollectOccurrencesE ce(vo, si);
  top_down(ce, si->e());
  for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++si) {
    top_down(ce, *it);
  }
}

bool is_output(VarDecl* vd) {
  for (ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
    if (*it != nullptr) {
      if (*it == constants().ann.output_var) {
        return true;
      }
      if (Call* c = (*it)->dynamicCast<Call>()) {
        if (c->id() == constants().ann.output_array) {
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
      CollectOccurrencesE ce(env.varOccurrences, vdi1);
      top_down(ce, rhs);

      id1->decl()->e(rhs);
      id0->decl()->e(nullptr);

      auto* vdi0 = (*env.flat())[env.varOccurrences.find(id0->decl())]->cast<VarDeclI>();
      CollectDecls cd(env.varOccurrences, deletedVarDecls, vdi0);
      top_down(cd, rhs);
    }
    if (Expression::equal(id1->decl()->e(), id0->decl()->id())) {
      auto* vdi1 = (*env.flat())[env.varOccurrences.find(id1->decl())]->cast<VarDeclI>();
      CollectDecls cd(env.varOccurrences, deletedVarDecls, vdi1);
      Expression* rhs = id1->decl()->e();
      top_down(cd, rhs);
      id1->decl()->e(nullptr);
    }
    // Compute intersection of domains
    if (id0->decl()->ti()->domain() != nullptr) {
      if (id1->decl()->ti()->domain() != nullptr) {
        if (id0->type().isint() || id0->type().isIntSet()) {
          IntSetVal* isv0 = eval_intset(env, id0->decl()->ti()->domain());
          IntSetVal* isv1 = eval_intset(env, id1->decl()->ti()->domain());
          IntSetRanges isv0r(isv0);
          IntSetRanges isv1r(isv1);
          Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(isv0r, isv1r);
          IntSetVal* nd = IntSetVal::ai(inter);
          if (nd->size() == 0) {
            env.fail();
          } else if (nd->card() != isv1->card()) {
            id1->decl()->ti()->domain(new SetLit(Location(), nd));
            if (nd->card() == isv0->card()) {
              id1->decl()->ti()->setComputedDomain(id0->decl()->ti()->computedDomain());
            } else {
              id1->decl()->ti()->setComputedDomain(false);
            }
          }
        } else if (id0->type().isbool()) {
          if (eval_bool(env, id0->decl()->ti()->domain()) !=
              eval_bool(env, id1->decl()->ti()->domain())) {
            env.fail();
          }
        } else {
          // float
          FloatSetVal* isv0 = eval_floatset(env, id0->decl()->ti()->domain());
          FloatSetVal* isv1 = eval_floatset(env, id1->decl()->ti()->domain());
          FloatSetRanges isv0r(isv0);
          FloatSetRanges isv1r(isv1);
          Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> inter(isv0r, isv1r);
          FloatSetVal* nd = FloatSetVal::ai(inter);

          FloatSetRanges nd_r(nd);
          FloatSetRanges isv1r_2(isv1);

          if (nd->size() == 0) {
            env.fail();
          } else if (!Ranges::equal(nd_r, isv1r_2)) {
            id1->decl()->ti()->domain(new SetLit(Location(), nd));
            FloatSetRanges nd_r_2(nd);
            FloatSetRanges isv0r_2(isv0);
            if (Ranges::equal(nd_r_2, isv0r_2)) {
              id1->decl()->ti()->setComputedDomain(id0->decl()->ti()->computedDomain());
            } else {
              id1->decl()->ti()->setComputedDomain(false);
            }
          }
        }

      } else {
        id1->decl()->ti()->domain(id0->decl()->ti()->domain());
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
  if (it != env.varOccurrences.itemMap.end()) {
    for (auto* item : it->second) {
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

void optimize(Env& env, bool chain_compression) {
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

    // Phase 1: initialise queues
    //  - remove equality constraints between identifiers
    //  - remove toplevel forall constraints
    //  - collect exists, clauses and reified foralls in boolConstraints
    //  - remove "constraint x" where x is a bool var
    //  - unify variables that are assigned to an identifier
    //  - push bool vars that are fixed and have a RHS (to propagate the RHS constraint)
    //  - push int vars that are fixed (either have a RHS or a singleton domain)
    for (unsigned int i = 0; i < m.size(); i++) {
      if (m[i]->removed()) {
        continue;
      }
      if (auto* ci = m[i]->dynamicCast<ConstraintI>()) {
        ci->flag(false);
        if (!ci->removed()) {
          if (Call* c = ci->e()->dynamicCast<Call>()) {
            if ((c->id() == constants().ids.int_.eq || c->id() == constants().ids.bool_eq ||
                 c->id() == constants().ids.float_.eq || c->id() == constants().ids.set_eq) &&
                c->arg(0)->isa<Id>() && c->arg(1)->isa<Id>() &&
                (c->arg(0)->cast<Id>()->decl()->e() == nullptr ||
                 c->arg(1)->cast<Id>()->decl()->e() == nullptr)) {
              // Equality constraint between two identifiers: unify

              if (Call* defVar = c->ann().getCall(constants().ann.defines_var)) {
                // First, remove defines_var/is_defined_var annotations if present
                if (Expression::equal(defVar->arg(0), c->arg(0))) {
                  c->arg(0)->cast<Id>()->decl()->ann().remove(constants().ann.is_defined_var);
                } else {
                  c->arg(1)->cast<Id>()->decl()->ann().remove(constants().ann.is_defined_var);
                }
              }
              unify(envi, deletedVarDecls, c->arg(0)->cast<Id>(), c->arg(1)->cast<Id>());
              {
                VarDecl* vd = c->arg(0)->cast<Id>()->decl();
                int v0idx = envi.varOccurrences.find(vd);
                push_vardecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
              }

              push_dependent_constraints(envi, c->arg(0)->cast<Id>(), constraintQueue);
              CollectDecls cd(envi.varOccurrences, deletedVarDecls, ci);
              top_down(cd, c);
              ci->e(constants().literalTrue);
              ci->remove();
            } else if (c->id() == constants().ids.int_.lin_eq &&
                       Expression::equal(c->arg(2), IntLit::a(0))) {
              auto* al_c = follow_id(c->arg(0))->cast<ArrayLit>();
              if (al_c->size() == 2 &&
                  (*al_c)[0]->cast<IntLit>()->v() == -(*al_c)[1]->cast<IntLit>()->v()) {
                auto* al_x = follow_id(c->arg(1))->cast<ArrayLit>();
                if ((*al_x)[0]->isa<Id>() && (*al_x)[1]->isa<Id>() &&
                    ((*al_x)[0]->cast<Id>()->decl()->e() == nullptr ||
                     (*al_x)[1]->cast<Id>()->decl()->e() == nullptr)) {
                  // Equality constraint between two identifiers: unify

                  if (Call* defVar = c->ann().getCall(constants().ann.defines_var)) {
                    // First, remove defines_var/is_defined_var annotations if present
                    if (Expression::equal(defVar->arg(0), (*al_x)[0])) {
                      (*al_x)[0]->cast<Id>()->decl()->ann().remove(constants().ann.is_defined_var);
                    } else {
                      (*al_x)[1]->cast<Id>()->decl()->ann().remove(constants().ann.is_defined_var);
                    }
                  }
                  unify(envi, deletedVarDecls, (*al_x)[0]->cast<Id>(), (*al_x)[1]->cast<Id>());
                  {
                    VarDecl* vd = (*al_x)[0]->cast<Id>()->decl();
                    int v0idx = envi.varOccurrences.find(vd);
                    push_vardecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
                  }

                  push_dependent_constraints(envi, (*al_x)[0]->cast<Id>(), constraintQueue);
                  CollectDecls cd(envi.varOccurrences, deletedVarDecls, ci);
                  top_down(cd, c);
                  ci->e(constants().literalTrue);
                  ci->remove();
                }
              }
            } else if (c->id() == constants().ids.forall) {
              // Remove forall constraints, assign variables inside the forall to true

              auto* al = follow_id(c->arg(0))->cast<ArrayLit>();
              for (unsigned int j = al->size(); (j--) != 0U;) {
                if (Id* id = (*al)[j]->dynamicCast<Id>()) {
                  if (id->decl()->ti()->domain() == nullptr) {
                    toAssignBoolVars.push_back(
                        envi.varOccurrences.idx.find(id->decl()->id())->second);
                  } else if (id->decl()->ti()->domain() == constants().literalFalse) {
                    env.envi().fail();
                    id->decl()->e(constants().literalTrue);
                  }
                }  // todo: check else case (fixed bool inside a forall at this stage)
              }
              toRemoveConstraints.push_back(i);
            } else if (c->id() == constants().ids.exists || c->id() == constants().ids.clause) {
              // Add disjunctive constraints to the boolConstraints list

              boolConstraints.push_back(i);
            }
          } else if (Id* id = ci->e()->dynamicCast<Id>()) {
            if (id->decl()->ti()->domain() == constants().literalFalse) {
              env.envi().fail();
              ci->e(constants().literalFalse);
            } else {
              if (id->decl()->ti()->domain() == nullptr) {
                toAssignBoolVars.push_back(envi.varOccurrences.idx.find(id->decl()->id())->second);
              }
              toRemoveConstraints.push_back(i);
            }
          }
        }
      } else if (auto* vdi = m[i]->dynamicCast<VarDeclI>()) {
        vdi->flag(false);
        if ((vdi->e()->e() != nullptr) && vdi->e()->e()->isa<Id>() && vdi->e()->type().dim() == 0) {
          // unify variable with the identifier it's assigned to
          Id* id1 = vdi->e()->e()->cast<Id>();
          vdi->e()->e(nullptr);

          // Transfer is_defined_var annotation
          if (id1->decl()->ann().contains(constants().ann.is_defined_var)) {
            vdi->e()->ann().add(constants().ann.is_defined_var);
          } else if (vdi->e()->ann().contains(constants().ann.is_defined_var)) {
            id1->decl()->ann().add(constants().ann.is_defined_var);
          }

          unify(envi, deletedVarDecls, vdi->e()->id(), id1);
          push_dependent_constraints(envi, id1, constraintQueue);
        }
        if (vdi->e()->type().isbool() && vdi->e()->type().dim() == 0 &&
            (vdi->e()->ti()->domain() == constants().literalTrue ||
             vdi->e()->ti()->domain() == constants().literalFalse)) {
          // push RHS onto constraint queue since this bool var is fixed
          push_vardecl(envi, vdi, i, vardeclQueue);
          push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
        }
        if (Call* c = Expression::dynamicCast<Call>(vdi->e()->e())) {
          if (c->id() == constants().ids.forall || c->id() == constants().ids.exists ||
              c->id() == constants().ids.clause) {
            // push reified foralls, exists, clauses
            boolConstraints.push_back(i);
          }
        }
        if (vdi->e()->type().isint()) {
          if (((vdi->e()->e() != nullptr) && vdi->e()->e()->isa<IntLit>()) ||
              ((vdi->e()->ti()->domain() != nullptr) && vdi->e()->ti()->domain()->isa<SetLit>() &&
               vdi->e()->ti()->domain()->cast<SetLit>()->isv()->size() == 1 &&
               vdi->e()->ti()->domain()->cast<SetLit>()->isv()->min() ==
                   vdi->e()->ti()->domain()->cast<SetLit>()->isv()->max())) {
            // Variable is assigned an integer, or has a singleton domain
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
      Item* bi = m[boolConstraints[i]];
      if (bi->removed()) {
        continue;
      }
      Call* c;

      if (bi->isa<ConstraintI>()) {
        c = bi->cast<ConstraintI>()->e()->dynamicCast<Call>();
      } else {
        c = bi->cast<VarDeclI>()->e()->e()->dynamicCast<Call>();
      }
      if (c == nullptr) {
        continue;
      }
      bool isConjunction = (c->id() == constants().ids.forall);
      bool subsumed = false;
      Id* finalId = nullptr;
      bool finalIdNeg = false;
      int idCount = 0;
      std::vector<VarDecl*> pos;
      std::vector<VarDecl*> neg;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        bool unit = (j == 0 ? isConjunction : !isConjunction);
        auto* al = follow_id(c->arg(j))->cast<ArrayLit>();
        for (unsigned int k = 0; k < al->size(); k++) {
          if (Id* ident = (*al)[k]->dynamicCast<Id>()) {
            if ((ident->decl()->ti()->domain() != nullptr) ||
                ((ident->decl()->e() != nullptr) && ident->decl()->e()->type().isPar())) {
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
            if ((*al)[k]->cast<BoolLit>()->v() != unit) {
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
              CollectDecls cd(envi.varOccurrences, deletedVarDecls, bi);
              top_down(cd, bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(constants().literalFalse);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(constants().literalFalse);
              push_vardecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              push_dependent_constraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        } else {
          if (bi->isa<ConstraintI>()) {
            CollectDecls cd(envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<ConstraintI>()->e());
            bi->remove();
          } else {
            if (bi->cast<VarDeclI>()->e()->ti()->domain() != nullptr) {
              if (!eval_bool(envi, bi->cast<VarDeclI>()->e()->ti()->domain())) {
                envi.fail();
              }
            } else {
              CollectDecls cd(envi.varOccurrences, deletedVarDecls, bi);
              top_down(cd, bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(constants().literalTrue);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(constants().literalTrue);
              push_vardecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              push_dependent_constraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        }
      } else if (idCount == 1 && bi->isa<ConstraintI>()) {
        assert(finalId->decl()->ti()->domain() == nullptr);
        finalId->decl()->ti()->domain(constants().boollit(!finalIdNeg));
        if (finalId->decl()->e() == nullptr) {
          finalId->decl()->e(constants().boollit(!finalIdNeg));
        }
        CollectDecls cd(envi.varOccurrences, deletedVarDecls, bi);
        top_down(cd, bi->cast<ConstraintI>()->e());
        bi->remove();
        push_vardecl(envi, envi.varOccurrences.idx.find(finalId->decl()->id())->second,
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
        vdi->e()->ti()->domain(constants().literalTrue);
        push_vardecl(envi, vdi, toAssignBoolVars[i], vardeclQueue);
        push_dependent_constraints(envi, vdi->e()->id(), constraintQueue);
      }
    }

    // Phase 3: fixpoint of constraint and variable simplification

    std::unordered_map<Expression*, int> nonFixedLiteralCount;
    while (!vardeclQueue.empty() || !constraintQueue.empty()) {
      while (!vardeclQueue.empty()) {
        int var_idx = vardeclQueue.front();
        vardeclQueue.pop_front();
        m[var_idx]->cast<VarDeclI>()->flag(false);
        VarDecl* vd = m[var_idx]->cast<VarDeclI>()->e();

        if (vd->type().isbool() && (vd->ti()->domain() != nullptr)) {
          bool isTrue = vd->ti()->domain() == constants().literalTrue;
          bool remove = false;
          if (vd->e() != nullptr) {
            if (Id* id = vd->e()->dynamicCast<Id>()) {
              // Variable assigned to id, so fix id
              if (id->decl()->ti()->domain() == nullptr) {
                id->decl()->ti()->domain(vd->ti()->domain());
                push_vardecl(envi, envi.varOccurrences.idx.find(id->decl()->id())->second,
                             vardeclQueue);
              } else if (id->decl()->ti()->domain() != vd->ti()->domain()) {
                env.envi().fail();
              }
              remove = true;
            } else if (Call* c = vd->e()->dynamicCast<Call>()) {
              if (isTrue && c->id() == constants().ids.forall) {
                // Reified forall is now fixed to true, so make all elements of the conjunction true
                remove = true;
                auto* al = follow_id(c->arg(0))->cast<ArrayLit>();
                for (unsigned int i = 0; i < al->size(); i++) {
                  if (Id* id = (*al)[i]->dynamicCast<Id>()) {
                    if (id->decl()->ti()->domain() == nullptr) {
                      id->decl()->ti()->domain(constants().literalTrue);
                      push_vardecl(envi, envi.varOccurrences.idx.find(id->decl()->id())->second,
                                   vardeclQueue);
                    } else if (id->decl()->ti()->domain() == constants().literalFalse) {
                      env.envi().fail();
                      remove = true;
                    }
                  }
                }
              } else if (!isTrue &&
                         (c->id() == constants().ids.exists || c->id() == constants().ids.clause)) {
                // Reified disjunction is now fixed to false, so make all elements of the
                // disjunction false
                remove = true;
                for (unsigned int i = 0; i < c->argCount(); i++) {
                  bool ispos = i == 0;
                  auto* al = follow_id(c->arg(i))->cast<ArrayLit>();
                  for (unsigned int j = 0; j < al->size(); j++) {
                    if (Id* id = (*al)[j]->dynamicCast<Id>()) {
                      if (id->decl()->ti()->domain() == nullptr) {
                        id->decl()->ti()->domain(constants().boollit(!ispos));
                        push_vardecl(envi, envi.varOccurrences.idx.find(id->decl()->id())->second,
                                     vardeclQueue);
                      } else if (id->decl()->ti()->domain() == constants().boollit(ispos)) {
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
          if (it != envi.varOccurrences.itemMap.end()) {
            for (auto item = it->second.begin(); item != it->second.end(); ++item) {
              if ((*item)->removed()) {
                continue;
              }
              if (auto* vdi = (*item)->dynamicCast<VarDeclI>()) {
                // The variable occurs in the RHS of another variable, so
                // if that is an array variable, simplify all constraints that
                // mention the array variable
                if ((vdi->e()->e() != nullptr) && vdi->e()->e()->isa<ArrayLit>()) {
                  auto ait = envi.varOccurrences.itemMap.find(vdi->e()->id()->decl()->id());
                  if (ait != envi.varOccurrences.itemMap.end()) {
                    for (auto* aitem : ait->second) {
                      simplify_bool_constraint(envi, aitem, vd, remove, vardeclQueue,
                                               constraintQueue, toRemove, deletedVarDecls,
                                               nonFixedLiteralCount);
                    }
                  }
                  continue;
                }
              }
              // Simplify the constraint *item (which depends on this variable)
              simplify_bool_constraint(envi, *item, vd, remove, vardeclQueue, constraintQueue,
                                       toRemove, deletedVarDecls, nonFixedLiteralCount);
            }
          }
          // Actually remove all items that have become unnecessary in the step above
          for (auto i = static_cast<unsigned int>(toRemove.size()); (i--) != 0U;) {
            if (auto* ci = toRemove[i]->dynamicCast<ConstraintI>()) {
              CollectDecls cd(envi.varOccurrences, deletedVarDecls, ci);
              top_down(cd, ci->e());
              ci->remove();
            } else {
              auto* vdi = toRemove[i]->cast<VarDeclI>();
              CollectDecls cd(envi.varOccurrences, deletedVarDecls, vdi);
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
          if (isv->size() == 1 && isv->card() == 1) {
            simplify_constraint(envi, m[var_idx], deletedVarDecls, constraintQueue, vardeclQueue);
          }
        }
      }  // end of processing of variable queue

      // Now handle all non-boolean constraints (i.e. anything except forall, clause, exists)
      bool handledConstraint = false;
      while (!handledConstraint && !constraintQueue.empty()) {
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
          if (al != nullptr) {
            // Substitute all fixed variables by their values in array literals, then
            // push all constraints that depend on the array
            substitute_fixed_vars(envi, item, deletedVarDecls);
            push_dependent_constraints(envi, item->cast<VarDeclI>()->e()->id(), constraintQueue);
          } else if ((c == nullptr) ||
                     !(c->id() == constants().ids.forall || c->id() == constants().ids.exists ||
                       c->id() == constants().ids.clause)) {
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
      CollectDecls cd(envi.varOccurrences, deletedVarDecls, ci);
      top_down(cd, ci->e());
      ci->remove();
    }

    // Phase 4: Chain Breaking
    if (chain_compression) {
      ImpCompressor imp(envi, m, deletedVarDecls, boolConstraints);
      LECompressor le(envi, m, deletedVarDecls);
      for (auto& item : m) {
        imp.trackItem(item);
        le.trackItem(item);
      }
      imp.compress();
      le.compress();
    }

    // Phase 5: handle boolean constraints again (todo: check if we can
    // refactor this into a separate function)
    //
    // Difference to phase 2: constraint argument arrays are actually shortened here if possible
    for (auto i = static_cast<unsigned int>(boolConstraints.size()); (i--) != 0U;) {
      Item* bi = m[boolConstraints[i]];
      if (bi->removed()) {
        continue;
      }
      Call* c;
      std::vector<VarDecl*> removedVarDecls;

      if (bi->isa<ConstraintI>()) {
        c = bi->cast<ConstraintI>()->e()->dynamicCast<Call>();
      } else {
        c = Expression::dynamicCast<Call>(bi->cast<VarDeclI>()->e()->e());
      }
      if (c == nullptr ||
          !(c->id() == constants().ids.forall || c->id() == constants().ids.exists ||
            c->id() == constants().ids.clause)) {
        continue;
      }
      bool isConjunction = (c->id() == constants().ids.forall);
      bool subsumed = false;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        bool unit = (j == 0 ? isConjunction : !isConjunction);
        auto* al = follow_id(c->arg(j))->cast<ArrayLit>();
        std::vector<Expression*> compactedAl;
        for (unsigned int k = 0; k < al->size(); k++) {
          if (Id* ident = (*al)[k]->dynamicCast<Id>()) {
            if (ident->decl()->ti()->domain() != nullptr) {
              if (!(ident->decl()->ti()->domain() == constants().boollit(unit))) {
                subsumed = true;
              }
              removedVarDecls.push_back(ident->decl());
            } else {
              compactedAl.push_back(ident);
            }
          } else {
            if ((*al)[k]->cast<BoolLit>()->v() != unit) {
              subsumed = true;
            }
          }
        }
        if (compactedAl.size() < al->size()) {
          c->arg(j, new ArrayLit(al->loc(), compactedAl));
          c->arg(j)->type(Type::varbool(1));
        }
      }
      if (subsumed) {
        if (isConjunction) {
          if (bi->isa<ConstraintI>()) {
            env.envi().fail();
          } else {
            auto* al = follow_id(c->arg(0))->cast<ArrayLit>();
            for (unsigned int j = 0; j < al->size(); j++) {
              removedVarDecls.push_back((*al)[j]->cast<Id>()->decl());
            }
            bi->cast<VarDeclI>()->e()->ti()->domain(constants().literalFalse);
            bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
            bi->cast<VarDeclI>()->e()->e(constants().literalFalse);
          }
        } else {
          if (bi->isa<ConstraintI>()) {
            CollectDecls cd(envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<ConstraintI>()->e());
            bi->remove();
          } else {
            CollectDecls cd(envi.varOccurrences, deletedVarDecls, bi);
            top_down(cd, bi->cast<VarDeclI>()->e()->e());
            bi->cast<VarDeclI>()->e()->ti()->domain(constants().literalTrue);
            bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
            bi->cast<VarDeclI>()->e()->e(constants().literalTrue);
          }
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

    // Phase 6: remove deleted variables if possible
    // TODO: The delayed deletion could be done eagerly by the creation of
    // env.optRemoveItem() which contains the logic in this while loop.
    while (!deletedVarDecls.empty()) {
      VarDecl* cur = deletedVarDecls.back();
      deletedVarDecls.pop_back();
      if (envi.varOccurrences.occurrences(cur) == 0) {
        auto cur_idx = envi.varOccurrences.idx.find(cur->id());
        if (cur_idx != envi.varOccurrences.idx.end() && !m[cur_idx->second]->removed()) {
          if (is_output(cur)) {
            // We have to change the output model if we remove this variable
            Expression* val = nullptr;
            if (cur->type().isbool() && (cur->ti()->domain() != nullptr)) {
              val = cur->ti()->domain();
            } else if (cur->type().isint()) {
              if ((cur->e() != nullptr) && cur->e()->isa<IntLit>()) {
                val = cur->e();
              } else if ((cur->ti()->domain() != nullptr) && cur->ti()->domain()->isa<SetLit>() &&
                         cur->ti()->domain()->cast<SetLit>()->isv()->size() == 1 &&
                         cur->ti()->domain()->cast<SetLit>()->isv()->min() ==
                             cur->ti()->domain()->cast<SetLit>()->isv()->max()) {
                val = IntLit::a(cur->ti()->domain()->cast<SetLit>()->isv()->min());
              }
            }
            if (val != nullptr) {
              // Find corresponding variable in output model and fix it
              VarDecl* vd_out =
                  (*envi.output)[envi.outputFlatVarOccurrences.find(cur)]->cast<VarDeclI>()->e();
              vd_out->e(val);
              CollectDecls cd(envi.varOccurrences, deletedVarDecls,
                              m[cur_idx->second]->cast<VarDeclI>());
              top_down(cd, cur->e());
              (*envi.flat())[cur_idx->second]->remove();
            }
          } else {
            CollectDecls cd(envi.varOccurrences, deletedVarDecls,
                            m[cur_idx->second]->cast<VarDeclI>());
            top_down(cd, cur->e());
            (*envi.flat())[cur_idx->second]->remove();
          }
        }
      }
    }
  } catch (ModelInconsistent&) {
  }
}

class SubstitutionVisitor : public EVisitor {
protected:
  std::vector<VarDecl*> _removed;
  Expression* subst(Expression* e) {
    if (auto* vd = follow_id_to_decl(e)->dynamicCast<VarDecl>()) {
      if (vd->type().isbool() && (vd->ti()->domain() != nullptr)) {
        _removed.push_back(vd);
        return vd->ti()->domain();
      }
      if (vd->type().isint()) {
        if ((vd->e() != nullptr) && vd->e()->isa<IntLit>()) {
          _removed.push_back(vd);
          return vd->e();
        }
        if ((vd->ti()->domain() != nullptr) && vd->ti()->domain()->isa<SetLit>() &&
            vd->ti()->domain()->cast<SetLit>()->isv()->size() == 1 &&
            vd->ti()->domain()->cast<SetLit>()->isv()->min() ==
                vd->ti()->domain()->cast<SetLit>()->isv()->max()) {
          _removed.push_back(vd);
          return IntLit::a(vd->ti()->domain()->cast<SetLit>()->isv()->min());
        }
      }
    }
    return e;
  }

public:
  /// Visit array literal
  void vArrayLit(ArrayLit& al) {
    for (unsigned int i = 0; i < al.size(); i++) {
      al.set(i, subst(al[i]));
    }
  }
  /// Visit call
  void vCall(Call& c) {
    for (unsigned int i = 0; i < c.argCount(); i++) {
      c.arg(i, subst(c.arg(i)));
    }
  }
  /// Determine whether to enter node
  static bool enter(Expression* e) { return !e->isa<Id>(); }
  void remove(EnvI& env, Item* item, std::vector<VarDecl*>& deletedVarDecls) {
    for (auto& i : _removed) {
      i->ann().remove(constants().ann.is_defined_var);
      if (env.varOccurrences.remove(i, item) == 0) {
        if ((i->e() == nullptr || i->ti()->domain() == nullptr || i->ti()->computedDomain()) &&
            !is_output(i)) {
          deletedVarDecls.push_back(i);
        }
      }
    }
  }
};

void substitute_fixed_vars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls) {
  SubstitutionVisitor sv;
  if (auto* ci = ii->dynamicCast<ConstraintI>()) {
    top_down(sv, ci->e());
    for (ExpressionSetIter it = ci->e()->ann().begin(); it != ci->e()->ann().end(); ++it) {
      top_down(sv, *it);
    }
  } else if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
    top_down(sv, vdi->e());
    for (ExpressionSetIter it = vdi->e()->ann().begin(); it != vdi->e()->ann().end(); ++it) {
      top_down(sv, *it);
    }
  } else {
    auto* si = ii->cast<SolveI>();
    top_down(sv, si->e());
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
    is_true = (vdi->e()->type().isbool() && vdi->e()->ti()->domain() == constants().literalTrue);
    is_false = (vdi->e()->type().isbool() && vdi->e()->ti()->domain() == constants().literalFalse);
    assert(is_true || is_false || !vdi->e()->type().isbool() ||
           vdi->e()->ti()->domain() == nullptr);
  }
  if (Call* c = Expression::dynamicCast<Call>(con_e)) {
    if (c->id() == constants().ids.int_.eq || c->id() == constants().ids.bool_eq ||
        c->id() == constants().ids.float_.eq) {
      if (is_true && c->arg(0)->isa<Id>() && c->arg(1)->isa<Id>() &&
          (c->arg(0)->cast<Id>()->decl()->e() == nullptr ||
           c->arg(1)->cast<Id>()->decl()->e() == nullptr)) {
        if (Call* defVar = c->ann().getCall(constants().ann.defines_var)) {
          // First, remove defines_var/is_defined_var annotations if present
          if (Expression::equal(defVar->arg(0), c->arg(0))) {
            c->arg(0)->cast<Id>()->decl()->ann().remove(constants().ann.is_defined_var);
          } else {
            c->arg(1)->cast<Id>()->decl()->ann().remove(constants().ann.is_defined_var);
          }
        }
        unify(env, deletedVarDecls, c->arg(0)->cast<Id>(), c->arg(1)->cast<Id>());
        push_dependent_constraints(env, c->arg(0)->cast<Id>(), constraintQueue);
        CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
        top_down(cd, c);
        ii->remove();
      } else if (c->arg(0)->type().isPar() && c->arg(1)->type().isPar()) {
        Expression* e0 = eval_par(env, c->arg(0));
        Expression* e1 = eval_par(env, c->arg(1));
        bool is_equal = Expression::equal(e0, e1);
        if ((is_true && is_equal) || (is_false && !is_equal)) {
          // do nothing
        } else if ((is_true && !is_equal) || (is_false && is_equal)) {
          env.fail();
        } else {
          auto* vdi = ii->cast<VarDeclI>();
          CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          vdi->e()->e(constants().boollit(is_equal));
          vdi->e()->ti()->domain(constants().boollit(is_equal));
          vdi->e()->ti()->setComputedDomain(true);
          push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
          push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
        }
        if (ii->isa<ConstraintI>()) {
          CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          ii->remove();
        }
      } else if (is_true && ((c->arg(0)->isa<Id>() && c->arg(1)->type().isPar()) ||
                             (c->arg(1)->isa<Id>() && c->arg(0)->type().isPar()))) {
        Id* ident = c->arg(0)->isa<Id>() ? c->arg(0)->cast<Id>() : c->arg(1)->cast<Id>();
        Expression* arg = c->arg(0)->isa<Id>() ? c->arg(1) : c->arg(0);
        bool canRemove = false;
        TypeInst* ti = ident->decl()->ti();
        switch (ident->type().bt()) {
          case Type::BT_BOOL:
            if (ti->domain() == nullptr) {
              ti->domain(constants().boollit(eval_bool(env, arg)));
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
            IntVal d = eval_int(env, arg);
            if (ti->domain() == nullptr) {
              ti->domain(new SetLit(Location().introduce(), IntSetVal::a(d, d)));
              ti->setComputedDomain(false);
              canRemove = true;
            } else {
              IntSetVal* isv = eval_intset(env, ti->domain());
              if (isv->contains(d)) {
                ident->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(d, d)));
                ident->decl()->ti()->setComputedDomain(false);
                canRemove = true;
              } else {
                env.fail();
                canRemove = true;
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
          ident->decl()->e(c->arg(0)->isa<Id>() ? c->arg(1) : c->arg(0));
          ti->setComputedDomain(true);
          canRemove = true;
        }

        if (ident->decl()->e()->isa<Call>()) {
          constraintQueue.push_back((*env.flat())[env.varOccurrences.find(ident->decl())]);
        }
        push_dependent_constraints(env, ident, constraintQueue);
        if (canRemove) {
          CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          ii->remove();
        }
      }
    } else if ((is_true || is_false) && c->id() == constants().ids.int_.le &&
               ((c->arg(0)->isa<Id>() && c->arg(1)->type().isPar()) ||
                (c->arg(1)->isa<Id>() && c->arg(0)->type().isPar()))) {
      Id* ident = c->arg(0)->isa<Id>() ? c->arg(0)->cast<Id>() : c->arg(1)->cast<Id>();
      Expression* arg = c->arg(0)->isa<Id>() ? c->arg(1) : c->arg(0);
      IntSetVal* domain = ident->decl()->ti()->domain() != nullptr
                              ? eval_intset(env, ident->decl()->ti()->domain())
                              : nullptr;
      if (domain != nullptr) {
        BinOpType bot =
            c->arg(0)->isa<Id>() ? (is_true ? BOT_LQ : BOT_GR) : (is_true ? BOT_GQ : BOT_LE);
        IntSetVal* newDomain = LinearTraits<IntLit>::limitDomain(bot, domain, eval_int(env, arg));
        if (newDomain->card() == 0) {
          env.fail();
        } else {
          ident->decl()->ti()->domain(new SetLit(Location().introduce(), newDomain));
          ident->decl()->ti()->setComputedDomain(false);

          if (newDomain->min() == newDomain->max()) {
            push_dependent_constraints(env, ident, constraintQueue);
          }
          CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);

          if (auto* vdi = ii->dynamicCast<VarDeclI>()) {
            vdi->e()->e(constants().boollit(is_true));
            push_dependent_constraints(env, vdi->e()->id(), constraintQueue);
            if (env.varOccurrences.occurrences(vdi->e()) == 0) {
              if (is_output(vdi->e())) {
                VarDecl* vd_out = (*env.output)[env.outputFlatVarOccurrences.find(vdi->e())]
                                      ->cast<VarDeclI>()
                                      ->e();
                vd_out->e(constants().boollit(is_true));
              }
              vdi->remove();
            }
          } else {
            ii->remove();
          }
        }
      }
    } else if (c->id() == constants().ids.bool2int) {
      auto* vdi = ii->dynamicCast<VarDeclI>();
      VarDecl* vd;
      bool fixed = false;
      bool b_val = false;
      if (vdi != nullptr) {
        vd = vdi->e();
      } else if (Id* ident = c->arg(1)->dynamicCast<Id>()) {
        vd = ident->decl();
      } else {
        vd = nullptr;
      }
      IntSetVal* vd_dom = nullptr;
      if (vd != nullptr) {
        if (vd->ti()->domain() != nullptr) {
          vd_dom = eval_intset(env, vd->ti()->domain());
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
        if (c->arg(0)->type().isPar()) {
          bool b2i_val = eval_bool(env, c->arg(0));
          if (b2i_val != b_val) {
            env.fail();
          } else {
            CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            ii->remove();
          }
        } else {
          Id* ident = c->arg(0)->cast<Id>();
          TypeInst* ti = ident->decl()->ti();
          if (ti->domain() == nullptr) {
            ti->domain(constants().boollit(b_val));
            ti->setComputedDomain(false);
          } else if (eval_bool(env, ti->domain()) != b_val) {
            env.fail();
          }
          CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
          top_down(cd, c);
          if (vd != nullptr) {
            vd->e(IntLit::a(static_cast<long long>(b_val)));
            vd->ti()->setComputedDomain(true);
          }
          push_dependent_constraints(env, ident, constraintQueue);
          if (vdi != nullptr) {
            if (env.varOccurrences.occurrences(vd) == 0) {
              vdi->remove();
            }
          } else {
            ii->remove();
          }
        }
      } else {
        IntVal v = -1;
        if (auto* bl = c->arg(0)->dynamicCast<BoolLit>()) {
          v = bl->v() ? 1 : 0;
        } else if (Id* ident = c->arg(0)->dynamicCast<Id>()) {
          if (ident->decl()->ti()->domain() != nullptr) {
            v = eval_bool(env, ident->decl()->ti()->domain()) ? 1 : 0;
          }
        }
        if (v != -1) {
          if ((vd_dom != nullptr) && !vd_dom->contains(v)) {
            env.fail();
          } else {
            CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
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
              CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
              top_down(cd, c);
              ii->remove();
            } else {
              deletedVarDecls.push_back(ii->cast<VarDeclI>()->e());
            }
            return true;
          } else {
            auto* vdi = ii->cast<VarDeclI>();
            vdi->e()->ti()->domain(constants().literalFalse);
            CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vdi->e()->e(constants().literalFalse);
            push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            return true;
          }
        case OptimizeRegistry::CS_ENTAILED:
          if (is_true) {
            if (ii->isa<ConstraintI>()) {
              CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
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
            vdi->e()->ti()->domain(constants().literalTrue);
            CollectDecls cd(env.varOccurrences, deletedVarDecls, ii);
            top_down(cd, c);
            vdi->e()->e(constants().literalTrue);
            push_vardecl(env, vdi, env.varOccurrences.find(vdi->e()), vardeclQueue);
            return true;
          }
        case OptimizeRegistry::CS_REWRITE: {
          std::vector<VarDecl*> tdv;
          CollectDecls cd(env.varOccurrences, tdv, ii);
          top_down(cd, c);

          CollectOccurrencesE ce(env.varOccurrences, ii);
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
            if ((vdi->e()->e() != nullptr) && vdi->e()->e()->isa<Id>() &&
                vdi->e()->type().dim() == 0) {
              Id* id1 = vdi->e()->e()->cast<Id>();
              vdi->e()->e(nullptr);
              // Transfer is_defined_var annotation
              if (id1->decl()->ann().contains(constants().ann.is_defined_var)) {
                vdi->e()->ann().add(constants().ann.is_defined_var);
              } else if (vdi->e()->ann().contains(constants().ann.is_defined_var)) {
                id1->decl()->ann().add(constants().ann.is_defined_var);
              }
              unify(env, deletedVarDecls, vdi->e()->id(), id1);
              push_dependent_constraints(env, id1, constraintQueue);
            }
            if ((vdi->e()->e() != nullptr) && vdi->e()->e()->type().isPar() &&
                (vdi->e()->ti()->domain() != nullptr)) {
              if (vdi->e()->e()->type().isint()) {
                IntVal iv = eval_int(env, vdi->e()->e());
                IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                if (!dom->contains(iv)) {
                  env.fail();
                }
              } else if (vdi->e()->e()->type().isIntSet()) {
                IntSetVal* isv = eval_intset(env, vdi->e()->e());
                IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                IntSetRanges isv_r(isv);
                IntSetRanges dom_r(dom);
                if (!Ranges::subset(isv_r, dom_r)) {
                  env.fail();
                }
              } else if (vdi->e()->e()->type().isfloat()) {
                FloatVal fv = eval_float(env, vdi->e()->e());
                FloatSetVal* dom = eval_floatset(env, vdi->e()->ti()->domain());
                if (!dom->contains(fv)) {
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
  if (e->type().isPar()) {
    return static_cast<int>(eval_bool(env, e));
  }
  Id* id = e->cast<Id>();
  if (id->decl()->ti()->domain() == nullptr) {
    return 2;
  }
  return static_cast<int>(id->decl()->ti()->domain() == constants().literalTrue);
}

int decrement_non_fixed_vars(std::unordered_map<Expression*, int>& nonFixedLiteralCount, Call* c) {
  auto it = nonFixedLiteralCount.find(c);
  if (it == nonFixedLiteralCount.end()) {
    int nonFixedVars = 0;
    for (unsigned int i = 0; i < c->argCount(); i++) {
      auto* al = follow_id(c->arg(i))->cast<ArrayLit>();
      nonFixedVars += static_cast<int>(al->size());
      for (unsigned int j = al->size(); (j--) != 0U;) {
        if ((*al)[j]->type().isPar()) {
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
  bool isTrue = vd->ti()->domain() == constants().literalTrue;
  Expression* e = nullptr;
  auto* ci = ii->dynamicCast<ConstraintI>();
  auto* vdi = ii->dynamicCast<VarDeclI>();
  if (ci != nullptr) {
    e = ci->e();

    if (vd->ti()->domain() != nullptr) {
      if (Call* definedVarCall = e->ann().getCall(constants().ann.defines_var)) {
        if (Expression::equal(definedVarCall->arg(0), vd->id())) {
          e->ann().removeCall(constants().ann.defines_var);
          vd->ann().remove(constants().ann.is_defined_var);
        }
      }
    }

  } else if (vdi != nullptr) {
    e = vdi->e()->e();
    if (e == nullptr) {
      return;
    }
    if (Id* id = e->dynamicCast<Id>()) {
      assert(id->decl() == vd);
      if (vdi->e()->ti()->domain() == nullptr) {
        vdi->e()->ti()->domain(constants().boollit(isTrue));
        vardeclQueue.push_back(env.varOccurrences.idx.find(vdi->e()->id())->second);
      } else if (id->decl()->ti()->domain() == constants().boollit(!isTrue)) {
        env.fail();
        remove = false;
      }
      return;
    }
  }
  if (Id* ident = e->dynamicCast<Id>()) {
    assert(ident->decl() == vd);
    return;
  }
  if (e->isa<BoolLit>()) {
    if (e == constants().literalTrue && (ci != nullptr)) {
      toRemove.push_back(ci);
    }
    return;
  }
  Call* c = e->cast<Call>();
  if (c->id() == constants().ids.bool_eq) {
    Expression* b0 = c->arg(0);
    Expression* b1 = c->arg(1);
    int b0s = bool_state(env, b0);
    int b1s = bool_state(env, b1);
    if (b0s == 2) {
      std::swap(b0, b1);
      std::swap(b0s, b1s);
    }
    assert(b0s != 2);
    if ((ci != nullptr) || vdi->e()->ti()->domain() == constants().literalTrue) {
      if (b0s != b1s) {
        if (b1s == 2) {
          b1->cast<Id>()->decl()->ti()->domain(constants().boollit(isTrue));
          vardeclQueue.push_back(env.varOccurrences.idx.find(b1->cast<Id>()->decl()->id())->second);
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
    } else if ((vdi != nullptr) && vdi->e()->ti()->domain() == constants().literalFalse) {
      if (b0s != b1s) {
        if (b1s == 2) {
          b1->cast<Id>()->decl()->ti()->domain(constants().boollit(isTrue));
          vardeclQueue.push_back(env.varOccurrences.idx.find(b1->cast<Id>()->decl()->id())->second);
        }
      } else {
        env.fail();
        remove = false;
      }
    } else {
      remove = false;
    }
  } else if (c->id() == constants().ids.forall || c->id() == constants().ids.exists ||
             c->id() == constants().ids.clause) {
    if (isTrue && c->id() == constants().ids.exists) {
      if (ci != nullptr) {
        toRemove.push_back(ci);
      } else {
        if (vdi->e()->ti()->domain() == nullptr) {
          vdi->e()->ti()->domain(constants().literalTrue);
          vardeclQueue.push_back(env.varOccurrences.idx.find(vdi->e()->id())->second);
        } else if (vdi->e()->ti()->domain() != constants().literalTrue) {
          env.fail();
          vdi->e()->e(constants().literalTrue);
        }
      }
    } else if (!isTrue && c->id() == constants().ids.forall) {
      if (ci != nullptr) {
        env.fail();
        toRemove.push_back(ci);
      } else {
        if (vdi->e()->ti()->domain() == nullptr) {
          vdi->e()->ti()->domain(constants().literalFalse);
          vardeclQueue.push_back(env.varOccurrences.idx.find(vdi->e()->id())->second);
        } else if (vdi->e()->ti()->domain() != constants().literalFalse) {
          env.fail();
          vdi->e()->e(constants().literalFalse);
        }
      }
    } else {
      int nonfixed = decrement_non_fixed_vars(nonFixedLiteralCount, c);
      bool isConjunction = (c->id() == constants().ids.forall);
      assert(nonfixed >= 0);
      if (nonfixed <= 1) {
        bool subsumed = false;
        int nonfixed_i = -1;
        int nonfixed_j = -1;
        int realNonFixed = 0;
        for (unsigned int i = 0; i < c->argCount(); i++) {
          bool unit = (i == 0 ? isConjunction : !isConjunction);
          auto* al = follow_id(c->arg(i))->cast<ArrayLit>();
          realNonFixed += static_cast<int>(al->size());
          for (unsigned int j = al->size(); (j--) != 0U;) {
            if ((*al)[j]->type().isPar() ||
                ((*al)[j]->cast<Id>()->decl()->ti()->domain() != nullptr)) {
              realNonFixed--;
            }
            if ((*al)[j]->type().isPar() && eval_bool(env, (*al)[j]) != unit) {
              subsumed = true;
              i = 2;  // break out of outer loop
              break;
            }
            if (Id* id = (*al)[j]->dynamicCast<Id>()) {
              if (id->decl()->ti()->domain() != nullptr) {
                bool idv = (id->decl()->ti()->domain() == constants().literalTrue);
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
              ci->e(constants().literalFalse);
            } else {
              toRemove.push_back(ci);
            }
          } else {
            if (vdi->e()->ti()->domain() == nullptr) {
              vdi->e()->ti()->domain(constants().boollit(!isConjunction));
              vardeclQueue.push_back(env.varOccurrences.idx.find(vdi->e()->id())->second);
            } else if (vdi->e()->ti()->domain() != constants().boollit(!isConjunction)) {
              env.fail();
              vdi->e()->e(constants().boollit(!isConjunction));
            }
          }
        } else if (realNonFixed == 0) {
          if (ci != nullptr) {
            if (isConjunction) {
              toRemove.push_back(ci);
            } else {
              env.fail();
              ci->e(constants().literalFalse);
            }
          } else {
            if (vdi->e()->ti()->domain() == nullptr) {
              vdi->e()->ti()->domain(constants().boollit(isConjunction));
              vardeclQueue.push_back(env.varOccurrences.idx.find(vdi->e()->id())->second);
            } else if (vdi->e()->ti()->domain() != constants().boollit(isConjunction)) {
              env.fail();
              vdi->e()->e(constants().boollit(isConjunction));
            }
            toRemove.push_back(vdi);
          }
        } else if (realNonFixed == 1) {
          // not subsumed, nonfixed==1
          assert(nonfixed_i != -1);
          auto* al = follow_id(c->arg(nonfixed_i))->cast<ArrayLit>();
          Id* ident = (*al)[nonfixed_j]->cast<Id>();
          if ((ci != nullptr) || (vdi->e()->ti()->domain() != nullptr)) {
            bool result = nonfixed_i == 0;
            if ((vdi != nullptr) && vdi->e()->ti()->domain() == constants().literalFalse) {
              result = !result;
            }
            VarDecl* decl = ident->decl();
            if (decl->ti()->domain() == nullptr) {
              decl->ti()->domain(constants().boollit(result));
              vardeclQueue.push_back(env.varOccurrences.idx.find(decl->id())->second);
            } else if (vd->ti()->domain() != constants().boollit(result)) {
              env.fail();
              decl->e(constants().literalTrue);
            }
          } else {
            if (nonfixed_i == 0) {
              // this is a clause, exists or forall with a single non-fixed variable,
              // assigned to a non-fixed variable => turn into simple equality
              vdi->e()->e(nullptr);
              // Transfer is_defined_var annotation
              if (ident->decl()->ann().contains(constants().ann.is_defined_var)) {
                vdi->e()->ann().add(constants().ann.is_defined_var);
              } else if (vdi->e()->ann().contains(constants().ann.is_defined_var)) {
                ident->decl()->ann().add(constants().ann.is_defined_var);
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

      } else if (c->id() == constants().ids.clause) {
        int posOrNeg = isTrue ? 0 : 1;
        auto* al = follow_id(c->arg(posOrNeg))->cast<ArrayLit>();
        auto* al_other = follow_id(c->arg(1 - posOrNeg))->cast<ArrayLit>();

        if ((ci != nullptr) && al->size() == 1 && (*al)[0] != vd->id() && al_other->size() == 1) {
          // simple implication
          assert((*al_other)[0] == vd->id());
          if (ci != nullptr) {
            if ((*al)[0]->type().isPar()) {
              if (eval_bool(env, (*al)[0]) == isTrue) {
                toRemove.push_back(ci);
              } else {
                env.fail();
                remove = false;
              }
            } else {
              Id* id = (*al)[0]->cast<Id>();
              if (id->decl()->ti()->domain() == nullptr) {
                id->decl()->ti()->domain(constants().boollit(isTrue));
                vardeclQueue.push_back(env.varOccurrences.idx.find(id->decl()->id())->second);
              } else {
                if (id->decl()->ti()->domain() == constants().boollit(isTrue)) {
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
                  vdi->e()->ti()->domain(constants().literalTrue);
                  vardeclQueue.push_back(env.varOccurrences.idx.find(vdi->e()->id())->second);
                } else if (vdi->e()->ti()->domain() != constants().literalTrue) {
                  env.fail();
                  vdi->e()->e(constants().literalTrue);
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

}  // namespace MiniZinc
