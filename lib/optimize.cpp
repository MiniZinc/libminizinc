/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/optimize.hh>
#include <minizinc/hash.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/optimize_constraints.hh>

#include <vector>

namespace MiniZinc {

  void VarOccurrences::add(VarDeclI *i, int idx_i)
  {
    idx.insert(i->e()->id(), idx_i);
  }
  void VarOccurrences::add(VarDecl *e, int idx_i)
  {
    assert(find(e) == -1);
    idx.insert(e->id(), idx_i);
  }
  int VarOccurrences::find(VarDecl* vd)
  {
    IdMap<int>::iterator it = idx.find(vd->id());
    return it==idx.end() ? -1 : it->second;
  }
  void VarOccurrences::remove(VarDecl *vd)
  {
    idx.remove(vd->id());
  }
  
  void VarOccurrences::add(VarDecl* v, Item* i) {
    IdMap<Items>::iterator vi = _m.find(v->id()->decl()->id());
    if (vi==_m.end()) {
      Items items; items.insert(i);
      _m.insert(v->id()->decl()->id(), items);
    } else {
      vi->second.insert(i);
    }
  }
  
  int VarOccurrences::remove(VarDecl* v, Item* i) {
    IdMap<Items>::iterator vi = _m.find(v->id()->decl()->id());
    assert(vi!=_m.end());
    vi->second.erase(i);
    return vi->second.size();
  }
  
  void VarOccurrences::unify(EnvI& env, Model* m, Id* id0_0, Id *id1_0) {
    Id* id0 = id0_0->decl()->id();
    Id* id1 = id1_0->decl()->id();
    
    VarDecl* v0 = id0->decl();
    VarDecl* v1 = id1->decl();

    if (v0==v1)
      return;
    
    int v0idx = find(v0);
    assert(v0idx != -1);
    env.flat_removeItem(v0idx);

    IdMap<Items>::iterator vi0 = _m.find(v0->id());
    if (vi0 != _m.end()) {
      IdMap<Items>::iterator vi1 = _m.find(v1->id());
      if (vi1 == _m.end()) {
        _m.insert(v1->id(), vi0->second);
      } else {
        vi1->second.insert(vi0->second.begin(), vi0->second.end());
      }
      _m.remove(v0->id());
    }
    
    remove(v0);
    id0->redirect(id1);    
  }
  
  void VarOccurrences::clear(void) {
    _m.clear();
    idx.clear();
  }
  
  int VarOccurrences::occurrences(VarDecl* v) {
    IdMap<Items>::iterator vi = _m.find(v->id()->decl()->id());
    return (vi==_m.end() ? 0 : vi->second.size());
  }
  
  void CollectOccurrencesI::vVarDeclI(VarDeclI* v) {
    CollectOccurrencesE ce(vo,v);
    topDown(ce,v->e());
  }
  void CollectOccurrencesI::vConstraintI(ConstraintI* ci) {
    CollectOccurrencesE ce(vo,ci);
    topDown(ce,ci->e());
    for (ExpressionSetIter it = ci->e()->ann().begin(); it != ci->e()->ann().end(); ++it)
      topDown(ce, *it);
  }
  void CollectOccurrencesI::vSolveI(SolveI* si) {
    CollectOccurrencesE ce(vo,si);
    topDown(ce,si->e());
    for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++si)
      topDown(ce,*it);
  }

  bool isOutput(VarDecl* vd) {
    for (ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
      if (*it) {
        if (*it==constants().ann.output_var)
          return true;
        if (Call* c = (*it)->dyn_cast<Call>()) {
          if (c->id() == constants().ann.output_array)
            return true;
        }
      }
      
    }
    return false;
  }
  
  void unify(EnvI& env, std::vector<VarDecl*>& deletedVarDecls, Id* id0, Id* id1) {
    if (id0->decl() != id1->decl()) {
      if (isOutput(id0->decl())) {
        std::swap(id0,id1);
      }
      
      if (id0->decl()->e() != NULL) {
        Expression* rhs = id0->decl()->e();

        VarDeclI* vdi1 = (*env.flat())[env.vo.find(id1->decl())]->cast<VarDeclI>();
        CollectOccurrencesE ce(env.vo, vdi1);
        topDown(ce, rhs);

        id1->decl()->e(rhs);
        id0->decl()->e(NULL);
        
        VarDeclI* vdi0 = (*env.flat())[env.vo.find(id0->decl())]->cast<VarDeclI>();
        CollectDecls cd(env.vo, deletedVarDecls, vdi0);
        topDown(cd, rhs);
      }
      
      // Compute intersection of domains
      if (id0->decl()->ti()->domain() != NULL) {
        if (id1->decl()->ti()->domain() != NULL) {
          
          if (id0->type().isint() || id0->type().isintset()) {
            IntSetVal* isv0 = eval_intset(env,id0->decl()->ti()->domain());
            IntSetVal* isv1 = eval_intset(env,id1->decl()->ti()->domain());
            IntSetRanges isv0r(isv0);
            IntSetRanges isv1r(isv1);
            Ranges::Inter<IntVal,IntSetRanges,IntSetRanges> inter(isv0r,isv1r);
            IntSetVal* nd = IntSetVal::ai(inter);
            if (nd->size()==0) {
              env.fail();
            } else if (nd->card() != isv1->card()) {
              id1->decl()->ti()->domain(new SetLit(Location(), nd));
              if (nd->card()==isv0->card()) {
                id1->decl()->ti()->setComputedDomain(id0->decl()->ti()->computedDomain());
              } else {
                id1->decl()->ti()->setComputedDomain(false);
              }
            }
          } else if (id0->type().isbool()) {
            if (eval_bool(env,id0->decl()->ti()->domain()) != eval_bool(env,id1->decl()->ti()->domain())) {
              env.fail();
            }
          } else {
            // float
            FloatSetVal* isv0 = eval_floatset(env,id0->decl()->ti()->domain());
            FloatSetVal* isv1 = eval_floatset(env,id1->decl()->ti()->domain());
            FloatSetRanges isv0r(isv0);
            FloatSetRanges isv1r(isv1);
            Ranges::Inter<FloatVal,FloatSetRanges,FloatSetRanges> inter(isv0r,isv1r);
            FloatSetVal* nd = FloatSetVal::ai(inter);
            
            FloatSetRanges nd_r(nd);
            FloatSetRanges isv1r_2(isv1);
            
            if (nd->size()==0) {
              env.fail();
            } else if (!Ranges::equal(nd_r,isv1r_2)) {
              id1->decl()->ti()->domain(new SetLit(Location(), nd));
              FloatSetRanges nd_r_2(nd);
              FloatSetRanges isv0r_2(isv0);
              if (Ranges::equal(nd_r_2,isv0r_2)) {
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
      if (isOutput(id0->decl())) {
        assert(env.output_vo.find(id0->decl()) != -1);
        VarDecl* id0_output = (*env.output)[env.output_vo.find(id0->decl())]->cast<VarDeclI>()->e();
        assert(env.output_vo.find(id1->decl()) != -1);
        VarDecl* id1_output = (*env.output)[env.output_vo.find(id1->decl())]->cast<VarDeclI>()->e();
        if (id0_output->e() == NULL) {
          id0_output->e(id1_output->id());
        }
      }
      
      env.vo.unify(env, env.flat(), id0, id1);
    }
  }
  
  void substituteFixedVars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls);
  void simplifyBoolConstraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::vector<int>& vardeclQueue,
                              std::vector<Item*>& constraintQueue,
                              std::vector<Item*>& toRemove,
                              UNORDERED_NAMESPACE::unordered_map<Expression*, int>& nonFixedLiteralCount);

  bool simplifyConstraint(EnvI& env, Item* ii,
                          std::vector<VarDecl*>& deletedVarDecls,
                          std::vector<Item*>& constraintQueue,
                          std::vector<int>& vardeclQueue);
  
  void pushVarDecl(EnvI& env, VarDeclI* vdi, int vd_idx, std::vector<int>& q) {
    if (!vdi->removed() && !vdi->flag()) {
      vdi->flag(true);
      q.push_back(vd_idx);
    }
  }
  void pushVarDecl(EnvI& env, int vd_idx, std::vector<int>& q) {
    pushVarDecl(env, (*env.flat())[vd_idx]->cast<VarDeclI>(), vd_idx, q);
  }
  
  void pushDependentConstraints(EnvI& env, Id* id, std::vector<Item*>& q) {
    IdMap<VarOccurrences::Items>::iterator it = env.vo._m.find(id->decl()->id());
    if (it != env.vo._m.end()) {
      for (VarOccurrences::Items::iterator item = it->second.begin(); item != it->second.end(); ++item) {
        if (ConstraintI* ci = (*item)->dyn_cast<ConstraintI>()) {
          if (!ci->removed() && !ci->flag()) {
            ci->flag(true);
            q.push_back(ci);
          }
        } else if (VarDeclI* vdi = (*item)->dyn_cast<VarDeclI>()) {
          if (vdi->e()->id()->decl() != vdi->e()) {
            vdi = (*env.flat())[env.vo.find(vdi->e()->id()->decl())]->cast<VarDeclI>();
          }
          if (!vdi->removed() && !vdi->flag() && vdi->e()->e()) {
            vdi->flag(true);
            q.push_back(vdi);
          }
        }
      }
    }
    
  }
  
  void optimize(Env& env) {
    if (env.envi().failed())
      return;
    try {
      EnvI& envi = env.envi();
      Model& m = *envi.flat();
      std::vector<int> toAssignBoolVars;
      std::vector<int> toRemoveConstraints;
      std::vector<VarDecl*> deletedVarDecls;

      std::vector<Item*> constraintQueue;
      std::vector<int> vardeclQueue;
      
      std::vector<int> boolConstraints;
      
      GCLock lock;

      for (unsigned int i=0; i<m.size(); i++) {
        if (!m[i]->removed()) {
          if (ConstraintI* ci = m[i]->dyn_cast<ConstraintI>()) {
            ci->flag(false);
          } else if (VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>()) {
            vdi->flag(false);
          }
        }
      }

      
      for (unsigned int i=0; i<m.size(); i++) {
        if (m[i]->removed())
          continue;
        if (ConstraintI* ci = m[i]->dyn_cast<ConstraintI>()) {
          ci->flag(false);
          if (!ci->removed()) {
            if (Call* c = ci->e()->dyn_cast<Call>()) {
              if ( (c->id() == constants().ids.int_.eq || c->id() == constants().ids.bool_eq || c->id() == constants().ids.float_.eq || c->id() == constants().ids.set_eq) &&
                  c->args()[0]->isa<Id>() && c->args()[1]->isa<Id>() &&
                  (c->args()[0]->cast<Id>()->decl()->e()==NULL || c->args()[1]->cast<Id>()->decl()->e()==NULL) ) {
                unify(envi, deletedVarDecls, c->args()[0]->cast<Id>(), c->args()[1]->cast<Id>());
                {
                  VarDecl* vd = c->args()[0]->cast<Id>()->decl();
                  int v0idx = envi.vo.find(vd);
                  pushVarDecl(envi, m[v0idx]->cast<VarDeclI>(), v0idx, vardeclQueue);
                }
                
                pushDependentConstraints(envi, c->args()[0]->cast<Id>(), constraintQueue);
                CollectDecls cd(envi.vo,deletedVarDecls,ci);
                topDown(cd,c);
                ci->e(constants().lit_true);
                envi.flat_removeItem(i);
              } else if (c->id()==constants().ids.forall) {
                ArrayLit* al = follow_id(c->args()[0])->cast<ArrayLit>();
                for (unsigned int j=al->v().size(); j--;) {
                  if (Id* id = al->v()[j]->dyn_cast<Id>()) {
                    if (id->decl()->ti()->domain()==NULL) {
                      toAssignBoolVars.push_back(envi.vo.idx.find(id->decl()->id())->second);
                    } else if (id->decl()->ti()->domain() == constants().lit_false) {
                      env.envi().fail();
                      id->decl()->e(constants().lit_true);
                    }
                  }
                }
                toRemoveConstraints.push_back(i);
              } else if (c->id()==constants().ids.exists || c->id()==constants().ids.clause) {
                boolConstraints.push_back(i);
              }
            } else if (Id* id = ci->e()->dyn_cast<Id>()) {
              if (id->decl()->ti()->domain() == constants().lit_false) {
                env.envi().fail();
                ci->e(constants().lit_false);
              } else {
                if (id->decl()->ti()->domain()==NULL) {
                  toAssignBoolVars.push_back(envi.vo.idx.find(id->decl()->id())->second);
                }
                toRemoveConstraints.push_back(i);
              }
            }
          }
        } else if (VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>()) {
          vdi->flag(false);
          if (vdi->e()->e() && vdi->e()->e()->isa<Id>() && vdi->e()->type().dim()==0) {
            Id* id1 = vdi->e()->e()->cast<Id>();
            vdi->e()->e(NULL);
            unify(envi, deletedVarDecls, vdi->e()->id(), id1);
            pushDependentConstraints(envi, id1, constraintQueue);
          }
          if (vdi->e()->type().isbool() && vdi->e()->type().isvar() && vdi->e()->type().dim()==0
              && (vdi->e()->ti()->domain() == constants().lit_true || vdi->e()->ti()->domain() == constants().lit_false)) {
            pushVarDecl(envi, vdi, i, vardeclQueue);
            pushDependentConstraints(envi, vdi->e()->id(), constraintQueue);
          }
          if (Call* c = Expression::dyn_cast<Call>(vdi->e()->e())) {
            if (c->id()==constants().ids.forall || c->id()==constants().ids.exists || c->id()==constants().ids.clause) {
              boolConstraints.push_back(i);
            }
          }
          if (vdi->e()->type().isint()) {
            if ((vdi->e()->e() && vdi->e()->e()->isa<IntLit>()) ||
                (vdi->e()->ti()->domain() && vdi->e()->ti()->domain()->isa<SetLit>() &&
                 vdi->e()->ti()->domain()->cast<SetLit>()->isv()->size()==1 &&
                 vdi->e()->ti()->domain()->cast<SetLit>()->isv()->min()==vdi->e()->ti()->domain()->cast<SetLit>()->isv()->max())) {
                  pushVarDecl(envi, vdi, i, vardeclQueue);
                  pushDependentConstraints(envi, vdi->e()->id(), constraintQueue);
            }
          }

          
        }
      }
      
      for (unsigned int i=boolConstraints.size(); i--;) {
        Item* bi = m[boolConstraints[i]];
        if (bi->removed())
          continue;
        Call* c;
        
        if (bi->isa<ConstraintI>()) {
          c = bi->cast<ConstraintI>()->e()->dyn_cast<Call>();
        } else {
          c = bi->cast<VarDeclI>()->e()->e()->dyn_cast<Call>();
        }
        if (c==NULL)
          continue;
        bool isConjunction = (c->id() == constants().ids.forall);
        bool subsumed = false;
        Id* finalId = NULL;
        bool finalIdNeg = false;
        int idCount = 0;
        std::vector<VarDecl*> pos;
        std::vector<VarDecl*> neg;
        
        for (unsigned int j=0; j<c->args().size(); j++) {
          bool unit = (j==0 ? isConjunction : !isConjunction);
          ArrayLit* al = follow_id(c->args()[j])->cast<ArrayLit>();
          for (unsigned int k=0; k<al->v().size(); k++) {
            if (Id* ident = al->v()[k]->dyn_cast<Id>()) {
              if (ident->decl()->ti()->domain() ||
                  (ident->decl()->e() && ident->decl()->e()->type().ispar()) ) {
                bool identValue = ident->decl()->ti()->domain() ?
                  eval_bool(envi, ident->decl()->ti()->domain()) :
                  eval_bool(envi, ident->decl()->e());
                if (identValue != unit) {
                  subsumed = true;
                  goto subsumed_check_done;
                }
              } else {
                idCount++;
                finalId = ident;
                finalIdNeg = (j==1);
                if (j==0)
                  pos.push_back(ident->decl());
                else
                  neg.push_back(ident->decl());
              }
            } else {
              if (al->v()[k]->cast<BoolLit>()->v()!=unit) {
                subsumed = true;
                goto subsumed_check_done;
              }
            }
          }
        }
        if (pos.size() > 0 && neg.size() > 0) {
          std::sort(pos.begin(),pos.end());
          std::sort(neg.begin(), neg.end());
          unsigned int ix=0;
          unsigned int iy=0;
          for (;;) {
            if (pos[ix]==neg[iy]) {
              subsumed = true;
              break;
            }
            if (pos[ix] < neg[iy]) {
              ix++;
            } else {
              iy++;
            }
            if (ix==pos.size() || iy==neg.size())
              break;
          }
        }

      subsumed_check_done:
        if (subsumed) {
          if (isConjunction) {
            if (bi->isa<ConstraintI>()) {
              env.envi().fail();
            } else {
              CollectDecls cd(envi.vo,deletedVarDecls,bi);
              topDown(cd,bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(constants().lit_false);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(constants().lit_false);
              pushVarDecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              pushDependentConstraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          } else {
            if (bi->isa<ConstraintI>()) {
              CollectDecls cd(envi.vo,deletedVarDecls,bi);
              topDown(cd,bi->cast<ConstraintI>()->e());
              bi->remove();
            } else {
              CollectDecls cd(envi.vo,deletedVarDecls,bi);
              topDown(cd,bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(constants().lit_true);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(constants().lit_true);
              pushVarDecl(envi, bi->cast<VarDeclI>(), boolConstraints[i], vardeclQueue);
              pushDependentConstraints(envi, bi->cast<VarDeclI>()->e()->id(), constraintQueue);
            }
          }
        }
        else if (idCount==1 && bi->isa<ConstraintI>()) {
          assert(finalId->decl()->ti()->domain()==NULL);
          finalId->decl()->ti()->domain(constants().boollit(!finalIdNeg));
          if (finalId->decl()->e()==NULL)
            finalId->decl()->e(constants().boollit(!finalIdNeg));
          CollectDecls cd(envi.vo,deletedVarDecls,bi);
          topDown(cd,bi->cast<ConstraintI>()->e());
          bi->remove();
          pushVarDecl(envi, envi.vo.idx.find(finalId->decl()->id())->second, vardeclQueue);
          pushDependentConstraints(envi, finalId, constraintQueue);
        }
      }
      
      for (unsigned int i=toAssignBoolVars.size(); i--;) {
        if (m[toAssignBoolVars[i]]->removed())
          continue;
        VarDeclI* vdi = m[toAssignBoolVars[i]]->cast<VarDeclI>();
        if (vdi->e()->ti()->domain()==NULL) {
          vdi->e()->ti()->domain(constants().lit_true);
          pushVarDecl(envi, vdi, toAssignBoolVars[i], vardeclQueue);
          pushDependentConstraints(envi, vdi->e()->id(), constraintQueue);
        }
      }
      
      UNORDERED_NAMESPACE::unordered_map<Expression*, int> nonFixedLiteralCount;
      while (!vardeclQueue.empty() || !constraintQueue.empty()) {
        while (!vardeclQueue.empty()) {
          int var_idx = vardeclQueue.back();
          vardeclQueue.pop_back();
          m[var_idx]->cast<VarDeclI>()->flag(false);
          VarDecl* vd = m[var_idx]->cast<VarDeclI>()->e();
          
          if (vd->type().isbool() && vd->ti()->domain()) {
            bool isTrue = vd->ti()->domain() == constants().lit_true;
            bool remove = false;
            if (vd->e()) {
              if (Id* id = vd->e()->dyn_cast<Id>()) {
                if (id->decl()->ti()->domain()==NULL) {
                  id->decl()->ti()->domain(vd->ti()->domain());
                  pushVarDecl(envi, envi.vo.idx.find(id->decl()->id())->second, vardeclQueue);
                } else if (id->decl()->ti()->domain() != vd->ti()->domain()) {
                  env.envi().fail();
                }
                remove = true;
              } else if (Call* c = vd->e()->dyn_cast<Call>()) {
                if (isTrue && c->id()==constants().ids.forall) {
                  remove = true;
                  ArrayLit* al = follow_id(c->args()[0])->cast<ArrayLit>();
                  for (unsigned int i=0; i<al->v().size(); i++) {
                    if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                      if (id->decl()->ti()->domain()==NULL) {
                        id->decl()->ti()->domain(constants().lit_true);
                        pushVarDecl(envi, envi.vo.idx.find(id->decl()->id())->second, vardeclQueue);
                      } else if (id->decl()->ti()->domain() == constants().lit_false) {
                        env.envi().fail();
                        remove = true;
                      }
                    }
                  }
                } else if (!isTrue && (c->id()==constants().ids.exists || c->id()==constants().ids.clause)) {
                  remove = true;
                  for (unsigned int i=0; i<c->args().size(); i++) {
                    bool ispos = i==0;
                    ArrayLit* al = follow_id(c->args()[i])->cast<ArrayLit>();
                    for (unsigned int j=0; j<al->v().size(); j++) {
                      if (Id* id = al->v()[j]->dyn_cast<Id>()) {
                        if (id->decl()->ti()->domain()==NULL) {
                          id->decl()->ti()->domain(constants().boollit(!ispos));
                          pushVarDecl(envi, envi.vo.idx.find(id->decl()->id())->second, vardeclQueue);
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
              remove = true;
            }
            pushDependentConstraints(envi, vd->id(), constraintQueue);
            std::vector<Item*> toRemove;
            IdMap<VarOccurrences::Items>::iterator it = envi.vo._m.find(vd->id()->decl()->id());
            if (it != envi.vo._m.end()) {
              for (VarOccurrences::Items::iterator item = it->second.begin(); item != it->second.end(); ++item) {
                if ((*item)->removed())
                  continue;
                if (VarDeclI* vdi = (*item)->dyn_cast<VarDeclI>()) {
                  if (vdi->e()->e() && vdi->e()->e()->isa<ArrayLit>()) {
                    IdMap<VarOccurrences::Items>::iterator ait = envi.vo._m.find(vdi->e()->id()->decl()->id());
                    if (ait != envi.vo._m.end()) {
                      for (VarOccurrences::Items::iterator aitem = ait->second.begin(); aitem != ait->second.end(); ++aitem) {
                        simplifyBoolConstraint(envi,*aitem,vd,remove,vardeclQueue,constraintQueue,toRemove,nonFixedLiteralCount);
                      }
                    }
                    continue;
                  }
                }
                simplifyBoolConstraint(envi,*item,vd,remove,vardeclQueue,constraintQueue,toRemove,nonFixedLiteralCount);
              }
            }
            for (unsigned int i=toRemove.size(); i--;) {
              if (ConstraintI* ci = toRemove[i]->dyn_cast<ConstraintI>()) {
                CollectDecls cd(envi.vo,deletedVarDecls,ci);
                topDown(cd,ci->e());
                envi.flat_removeItem(ci);
              } else {
                VarDeclI* vdi = toRemove[i]->cast<VarDeclI>();
                CollectDecls cd(envi.vo,deletedVarDecls,vdi);
                topDown(cd,vdi->e()->e());
                vdi->e()->e(NULL);
              }
            }
            if (remove) {
              deletedVarDecls.push_back(vd);
            } else {
              simplifyConstraint(envi,m[var_idx],deletedVarDecls,constraintQueue,vardeclQueue);
            }
          }
          else if (vd->type().isint() && vd->ti()->domain()) {
            IntSetVal* isv = eval_intset(envi, vd->ti()->domain());
            if (isv->size()==1 && isv->card()==1) {
              simplifyConstraint(envi,m[var_idx],deletedVarDecls,constraintQueue,vardeclQueue);
            }
          }
        }
        bool handledConstraint = false;
        while (!handledConstraint && !constraintQueue.empty()) {
          Item* item = constraintQueue.back();
          constraintQueue.pop_back();
          Call* c;
          ArrayLit* al = NULL;
          if (ConstraintI* ci = item->dyn_cast<ConstraintI>()) {
            ci->flag(false);
            c = Expression::dyn_cast<Call>(ci->e());
          } else {
            if (item->removed()) {
              item = m[envi.vo.find(item->cast<VarDeclI>()->e()->id()->decl())]->cast<VarDeclI>();
            }
            item->cast<VarDeclI>()->flag(false);
            c = Expression::dyn_cast<Call>(item->cast<VarDeclI>()->e()->e());
            al = Expression::dyn_cast<ArrayLit>(item->cast<VarDeclI>()->e()->e());
          }
          if (!item->removed()) {
            if (al) {
              substituteFixedVars(envi, item, deletedVarDecls);
              pushDependentConstraints(envi, item->cast<VarDeclI>()->e()->id(), constraintQueue);
            } else if (!c || !(c->id()==constants().ids.forall || c->id()==constants().ids.exists ||
                               c->id()==constants().ids.clause) ) {
              substituteFixedVars(envi, item, deletedVarDecls);
              handledConstraint = simplifyConstraint(envi,item,deletedVarDecls,constraintQueue,vardeclQueue);
            }
          }
        }
      }
      for (unsigned int i=toRemoveConstraints.size(); i--;) {
        ConstraintI* ci = m[toRemoveConstraints[i]]->cast<ConstraintI>();
        CollectDecls cd(envi.vo,deletedVarDecls,ci);
        topDown(cd,ci->e());
        envi.flat_removeItem(toRemoveConstraints[i]);
      }
      
      for (unsigned int i=boolConstraints.size(); i--;) {
        Item* bi = m[boolConstraints[i]];
        if (bi->removed())
          continue;
        Call* c;
        std::vector<VarDecl*> removedVarDecls;
        
        if (bi->isa<ConstraintI>()) {
          c = bi->cast<ConstraintI>()->e()->dyn_cast<Call>();
        } else {
          c = Expression::dyn_cast<Call>(bi->cast<VarDeclI>()->e()->e());
        }
        if (c==NULL)
          continue;
        bool isConjunction = (c->id() == constants().ids.forall);
        bool subsumed = false;
        for (unsigned int j=0; j<c->args().size(); j++) {
          bool unit = (j==0 ? isConjunction : !isConjunction);
          ArrayLit* al = follow_id(c->args()[j])->cast<ArrayLit>();
          std::vector<Expression*> compactedAl;
          for (unsigned int k=0; k<al->v().size(); k++) {
            if (Id* ident = al->v()[k]->dyn_cast<Id>()) {
              if (ident->decl()->ti()->domain()) {
                if (!(ident->decl()->ti()->domain()==constants().boollit(unit))) {
                  subsumed = true;
                }
                removedVarDecls.push_back(ident->decl());
              } else {
                compactedAl.push_back(ident);
              }
            } else {
              if (al->v()[k]->cast<BoolLit>()->v()!=unit) {
                subsumed = true;
              }
            }
          }
          if (compactedAl.size() < al->v().size()) {
            c->args()[j] = new ArrayLit(al->loc(), compactedAl);
            c->args()[j]->type(Type::varbool(1));
          }
        }
        if (subsumed) {
          if (isConjunction) {
            if (bi->isa<ConstraintI>()) {
              env.envi().fail();
            } else {
              ArrayLit* al = follow_id(c->args()[0])->cast<ArrayLit>();
              for (unsigned int j=0; j<al->v().size(); j++) {
                removedVarDecls.push_back(al->v()[j]->cast<Id>()->decl());
              }
              bi->cast<VarDeclI>()->e()->ti()->domain(constants().lit_false);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(constants().lit_false);
            }
          } else {
            if (bi->isa<ConstraintI>()) {
              CollectDecls cd(envi.vo,deletedVarDecls,bi);
              topDown(cd,bi->cast<ConstraintI>()->e());
              bi->remove();
            } else {
              CollectDecls cd(envi.vo,deletedVarDecls,bi);
              topDown(cd,bi->cast<VarDeclI>()->e()->e());
              bi->cast<VarDeclI>()->e()->ti()->domain(constants().lit_true);
              bi->cast<VarDeclI>()->e()->ti()->setComputedDomain(true);
              bi->cast<VarDeclI>()->e()->e(constants().lit_true);
            }
          }
        }
        for (unsigned int j=0; j<removedVarDecls.size(); j++) {
          if (env.envi().vo.remove(removedVarDecls[j], bi) == 0) {
            if ( (removedVarDecls[j]->e()==NULL || removedVarDecls[j]->ti()->domain()==NULL ||
                  removedVarDecls[j]->ti()->computedDomain())
                && !isOutput(removedVarDecls[j]) ) {
              deletedVarDecls.push_back(removedVarDecls[j]);
            }
          }
        }
        if (VarDeclI* vdi = bi->dyn_cast<VarDeclI>()) {
          if (envi.vo.occurrences(vdi->e())==0) {
            if ( (vdi->e()->e()==NULL || vdi->e()->ti()->domain()==NULL ||
                  vdi->e()->ti()->computedDomain())
                && !isOutput(vdi->e()) ) {
              deletedVarDecls.push_back(vdi->e());
            }
          }
        }

      }
      
      while (!deletedVarDecls.empty()) {
        VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
        if (envi.vo.occurrences(cur) == 0) {
          IdMap<int>::iterator cur_idx = envi.vo.idx.find(cur->id());
          if (cur_idx != envi.vo.idx.end() && !m[cur_idx->second]->removed()) {
            if (isOutput(cur)) {
              Expression* val = NULL;
              if (cur->type().isbool() && cur->ti()->domain()) {
                val = cur->ti()->domain();
              } else if (cur->type().isint()) {
                if (cur->e() && cur->e()->isa<IntLit>()) {
                  val = cur->e();
                } else if (cur->ti()->domain() && cur->ti()->domain()->isa<SetLit>() &&
                           cur->ti()->domain()->cast<SetLit>()->isv()->size()==1 &&
                           cur->ti()->domain()->cast<SetLit>()->isv()->min()==cur->ti()->domain()->cast<SetLit>()->isv()->max()) {
                  val = IntLit::a(cur->ti()->domain()->cast<SetLit>()->isv()->min());
                }
              }
              if (val) {
                VarDecl* vd_out = (*envi.output)[envi.output_vo.find(cur)]->cast<VarDeclI>()->e();
                vd_out->e(val);
                CollectDecls cd(envi.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
                topDown(cd,cur->e());
                envi.flat_removeItem(cur_idx->second);
              }
            } else {
              CollectDecls cd(envi.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
              topDown(cd,cur->e());
              envi.flat_removeItem(cur_idx->second);
            }
          }
        }
      }
    } catch (ModelInconsistent&) {
      
    }
  }

  class SubstitutionVisitor : public EVisitor {
  protected:
    std::vector<VarDecl*> removed;
    Expression* subst(Expression* e) {
      if (VarDecl* vd = follow_id_to_decl(e)->dyn_cast<VarDecl>()) {
        if (vd->type().isbool() && vd->ti()->domain()) {
          removed.push_back(vd);
          return vd->ti()->domain();
        }
        if (vd->type().isint()) {
          if (vd->e() && vd->e()->isa<IntLit>()) {
            removed.push_back(vd);
            return vd->e();
          }
          if (vd->ti()->domain() && vd->ti()->domain()->isa<SetLit>() &&
              vd->ti()->domain()->cast<SetLit>()->isv()->size()==1 &&
              vd->ti()->domain()->cast<SetLit>()->isv()->min()==vd->ti()->domain()->cast<SetLit>()->isv()->max()) {
            removed.push_back(vd);
            return IntLit::a(vd->ti()->domain()->cast<SetLit>()->isv()->min());
          }
        }
      }
      return e;
    }
  public:
    /// Visit array literal
    void vArrayLit(const ArrayLit& al) {
      for (unsigned int i=0; i<al.v().size(); i++) {
        al.v()[i] = subst(al.v()[i]);
      }
    }
    /// Visit call
    void vCall(const Call& c) {
      for (unsigned int i=0; i<c.args().size(); i++) {
        c.args()[i] = subst(c.args()[i]);
      }
    }
    /// Determine whether to enter node
    bool enter(Expression* e) {
      return !e->isa<Id>();
    }
    void remove(EnvI& env, Item* item, std::vector<VarDecl*>& deletedVarDecls) {
      for (unsigned int i=0; i<removed.size(); i++) {
        removed[i]->ann().remove(constants().ann.is_defined_var);
        if (env.vo.remove(removed[i], item) == 0) {
          if ( (removed[i]->e()==NULL || removed[i]->ti()->domain()==NULL || removed[i]->ti()->computedDomain())
               && !isOutput(removed[i]) ) {
            deletedVarDecls.push_back(removed[i]);
          }
        }
      }
    }
  };
  
  void substituteFixedVars(EnvI& env, Item* ii, std::vector<VarDecl*>& deletedVarDecls) {
    SubstitutionVisitor sv;
    if (ConstraintI* ci = ii->dyn_cast<ConstraintI>()) {
      topDown(sv, ci->e());
      for (ExpressionSetIter it = ci->e()->ann().begin(); it != ci->e()->ann().end(); ++it) {
        topDown(sv, *it);
      }
    } else if (VarDeclI* vdi = ii->dyn_cast<VarDeclI>()) {
      topDown(sv, vdi->e());
      for (ExpressionSetIter it = vdi->e()->ann().begin(); it != vdi->e()->ann().end(); ++it) {
        topDown(sv, *it);
      }
    } else {
      SolveI* si = ii->cast<SolveI>();
      topDown(sv, si->e());
      for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
        topDown(sv, *it);
      }
    }
    sv.remove(env, ii, deletedVarDecls);
  }
  
  bool simplifyConstraint(EnvI& env, Item* ii,
                          std::vector<VarDecl*>& deletedVarDecls,
                          std::vector<Item*>& constraintQueue,
                          std::vector<int>& vardeclQueue) {
    Expression* con_e;
    bool is_true;
    bool is_false;
    if (ConstraintI* ci = ii->dyn_cast<ConstraintI>()) {
      con_e = ci->e();
      is_true = true;
      is_false = false;
    } else {
      VarDeclI* vdi = ii->cast<VarDeclI>();
      con_e = vdi->e()->e();
      is_true = (vdi->e()->type().isbool() && vdi->e()->ti()->domain()==constants().lit_true);
      is_false = (vdi->e()->type().isbool() && vdi->e()->ti()->domain()==constants().lit_false);
      assert(is_true || is_false || !vdi->e()->type().isbool() || vdi->e()->ti()->domain()==NULL);
    }
    if (Call* c = Expression::dyn_cast<Call>(con_e)) {
      if (c->id()==constants().ids.int_.eq || c->id()==constants().ids.bool_eq ||
          c->id()==constants().ids.float_.eq) {
        if (is_true && c->args()[0]->isa<Id>() && c->args()[1]->isa<Id>() &&
            (c->args()[0]->cast<Id>()->decl()->e()==NULL || c->args()[1]->cast<Id>()->decl()->e()==NULL) ) {
          unify(env, deletedVarDecls, c->args()[0]->cast<Id>(), c->args()[1]->cast<Id>());
          pushDependentConstraints(env, c->args()[0]->cast<Id>(), constraintQueue);
          CollectDecls cd(env.vo,deletedVarDecls,ii);
          topDown(cd,c);
          env.flat_removeItem(ii);
        } else if (c->args()[0]->type().ispar() && c->args()[1]->type().ispar()) {
          Expression* e0 = eval_par(env,c->args()[0]);
          Expression* e1 = eval_par(env,c->args()[1]);
          bool is_equal = Expression::equal(e0, e1);
          if ((is_true && is_equal) || (is_false && !is_equal)) {
            // do nothing
          } else if ((is_true && !is_equal) || (is_false && is_equal)) {
            env.fail();
          } else {
            VarDeclI* vdi = ii->cast<VarDeclI>();
            CollectDecls cd(env.vo,deletedVarDecls,ii);
            topDown(cd,c);
            vdi->e()->e(constants().boollit(is_equal));
            vdi->e()->ti()->domain(constants().boollit(is_equal));
            vdi->e()->ti()->setComputedDomain(true);
            pushVarDecl(env, vdi, env.vo.find(vdi->e()), vardeclQueue);
            pushDependentConstraints(env, vdi->e()->id(), constraintQueue);
          }
          if (ii->isa<ConstraintI>()) {
            CollectDecls cd(env.vo,deletedVarDecls,ii);
            topDown(cd,c);
            env.flat_removeItem(ii);
          }
        } else if (is_true &&
                   ((c->args()[0]->isa<Id>() && c->args()[1]->type().ispar()) ||
                    (c->args()[1]->isa<Id>() && c->args()[0]->type().ispar())) )
        {
          Id* ident = c->args()[0]->isa<Id>() ? c->args()[0]->cast<Id>() : c->args()[1]->cast<Id>();
          Expression* arg = c->args()[0]->isa<Id>() ? c->args()[1] : c->args()[0];
          bool canRemove = false;
          TypeInst* ti = ident->decl()->ti();
          switch (ident->type().bt()) {
            case Type::BT_BOOL:
              if (ti->domain() == NULL) {
                ti->domain(constants().boollit(eval_bool(env,arg)));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                if (eval_bool(env,ti->domain())==eval_bool(env,arg)) {
                  canRemove = true;
                } else {
                  env.fail();
                  canRemove = true;
                }
              }
              break;
            case Type::BT_INT:
            {
              IntVal d = eval_int(env,arg);
              if (ti->domain() == NULL) {
                ti->domain(new SetLit(Location().introduce(), IntSetVal::a(d,d)));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                IntSetVal* isv = eval_intset(env,ti->domain());
                if (isv->contains(d)) {
                  ident->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(d,d)));
                  ident->decl()->ti()->setComputedDomain(false);
                  canRemove = true;
                } else {
                  env.fail();
                  canRemove = true;
                }
              }
            }
              break;
            case Type::BT_FLOAT:
            {
              if (ti->domain() == NULL) {
                ti->domain(new BinOp(Location().introduce(), arg, BOT_DOTDOT, arg));
                ti->setComputedDomain(false);
                canRemove = true;
              } else {
                FloatVal value = eval_float(env,arg);
                if (LinearTraits<FloatLit>::domain_contains(eval_floatset(env,ti->domain()), value))  {
                  ti->domain(new BinOp(Location().introduce(), arg, BOT_DOTDOT, arg));
                  ti->setComputedDomain(false);
                  canRemove = true;
                } else {
                  env.fail();
                  canRemove = true;
                }
              }
            }
              break;
            default:
              break;
          }
          if (ident->decl()->e()==NULL) {
            ident->decl()->e(c->args()[0]->isa<Id>() ? c->args()[1] : c->args()[0]);
            ti->setComputedDomain(true);
            canRemove = true;
          }

          if (ident->decl()->e()->isa<Call>())
            constraintQueue.push_back((*env.flat())[env.vo.find(ident->decl())]);
          pushDependentConstraints(env, ident, constraintQueue);
          if (canRemove) {
            CollectDecls cd(env.vo,deletedVarDecls,ii);
            topDown(cd,c);
            env.flat_removeItem(ii);
          }
          
        }
      } else if ((is_true || is_false) &&
                 c->id()==constants().ids.int_.le && ((c->args()[0]->isa<Id>() && c->args()[1]->type().ispar()) ||
                                                      (c->args()[1]->isa<Id>() && c->args()[0]->type().ispar())) ) {
        Id* ident = c->args()[0]->isa<Id>() ? c->args()[0]->cast<Id>() : c->args()[1]->cast<Id>();
        Expression* arg = c->args()[0]->isa<Id>() ? c->args()[1] : c->args()[0];
        IntSetVal* domain = ident->decl()->ti()->domain() ? eval_intset(env,ident->decl()->ti()->domain()) : NULL;
        if (domain) {
          BinOpType bot = c->args()[0]->isa<Id>() ? (is_true ? BOT_LQ : BOT_GR) : (is_true ? BOT_GQ: BOT_LE);
          IntSetVal* newDomain = LinearTraits<IntLit>::limit_domain(bot, domain, eval_int(env,arg));
          ident->decl()->ti()->domain(new SetLit(Location().introduce(), newDomain));
          ident->decl()->ti()->setComputedDomain(false);
          
          if (newDomain->min()==newDomain->max()) {
            pushDependentConstraints(env, ident, constraintQueue);
          }
          CollectDecls cd(env.vo,deletedVarDecls,ii);
          topDown(cd,c);

          if (VarDeclI* vdi = ii->dyn_cast<VarDeclI>()) {
            vdi->e()->e(constants().boollit(is_true));
            pushDependentConstraints(env, vdi->e()->id(), constraintQueue);
            if (env.vo.occurrences(vdi->e())==0) {
              vdi->remove();
            }
          } else {
            env.flat_removeItem(ii);
          }
        }
      } else if (c->id()==constants().ids.bool2int) {
        VarDeclI* vdi = ii->cast<VarDeclI>();
        bool fixed = false;
        bool b_val = false;
        IntSetVal* vdi_dom = NULL;
        if (vdi->e()->ti()->domain()) {
          vdi_dom = eval_intset(env, vdi->e()->ti()->domain());
          if (vdi_dom->max()<0 || vdi_dom->min()>1) {
            env.fail();
            return true;
          }
          fixed = vdi_dom->min()==vdi_dom->max();
          b_val = (vdi_dom->min() == 1);
        }
        if (fixed) {
          if (c->args()[0]->type().ispar()) {
            
          } else {
            Id* ident = c->args()[0]->cast<Id>();
            TypeInst* ti = ident->decl()->ti();
            if (ti->domain() == NULL) {
              ti->domain(constants().boollit(b_val));
              ti->setComputedDomain(false);
            } else if (eval_bool(env,ti->domain())!=b_val) {
              env.fail();
            }
            CollectDecls cd(env.vo,deletedVarDecls,ii);
            topDown(cd,c);
            vdi->e()->e(IntLit::a(b_val));
            vdi->e()->ti()->setComputedDomain(true);
            pushDependentConstraints(env, ident, constraintQueue);
            if (env.vo.occurrences(vdi->e())==0) {
              vdi->remove();
            }
          }
        } else {
          IntVal v = -1;
          if (BoolLit* bl = c->args()[0]->dyn_cast<BoolLit>()) {
            v =  bl->v() ? 1 : 0;
          } else if (Id* ident = c->args()[0]->dyn_cast<Id>()) {
            if (ident->decl()->ti()->domain()) {
              v = eval_bool(env,ident->decl()->ti()->domain()) ? 1 : 0;
            }
          }
          if (v != -1) {
            if (vdi_dom && !vdi_dom->contains(v)) {
              env.fail();
            } else {
              CollectDecls cd(env.vo,deletedVarDecls,ii);
              topDown(cd,c);
              vdi->e()->e(IntLit::a(v));
              vdi->e()->ti()->domain(new SetLit(Location().introduce(),IntSetVal::a(v, v)));
              vdi->e()->ti()->setComputedDomain(true);
              pushVarDecl(env, vdi, env.vo.find(vdi->e()), vardeclQueue);
              pushDependentConstraints(env, vdi->e()->id(), constraintQueue);
            }
          }
        }

      } else {
        Expression* rewrite = NULL;
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
              CollectDecls cd(env.vo,deletedVarDecls,ii);
              topDown(cd,c);
              env.flat_removeItem(ii);
              return true;
            } else {
              VarDeclI* vdi = ii->cast<VarDeclI>();
              vdi->e()->ti()->domain(constants().lit_false);
              pushVarDecl(env, vdi, env.vo.find(vdi->e()), vardeclQueue);
              return true;
            }
          case OptimizeRegistry::CS_ENTAILED:
            if (is_true) {
              CollectDecls cd(env.vo,deletedVarDecls,ii);
              topDown(cd,c);
              env.flat_removeItem(ii);
              return true;
            } else if (is_false) {
              env.fail();
              return true;
            } else {
              VarDeclI* vdi = ii->cast<VarDeclI>();
              vdi->e()->ti()->domain(constants().lit_true);
              pushVarDecl(env, vdi, env.vo.find(vdi->e()), vardeclQueue);
              return true;
            }
          case OptimizeRegistry::CS_REWRITE:
          {

            std::vector<VarDecl*> tdv;
            CollectDecls cd(env.vo,tdv,ii);
            topDown(cd,c);

            CollectOccurrencesE ce(env.vo,ii);
            topDown(ce,rewrite);

            for (unsigned int i=0; i<tdv.size(); i++) {
              if (env.vo.occurrences(tdv[i])==0)
                deletedVarDecls.push_back(tdv[i]);
            }
            
            assert(rewrite != NULL);
            if (ConstraintI* ci = ii->dyn_cast<ConstraintI>()) {
              ci->e(rewrite);
              constraintQueue.push_back(ii);
            } else {
              VarDeclI* vdi = ii->cast<VarDeclI>();
              vdi->e()->e(rewrite);
              if (vdi->e()->e() && vdi->e()->e()->isa<Id>() && vdi->e()->type().dim()==0) {
                Id* id1 = vdi->e()->e()->cast<Id>();
                vdi->e()->e(NULL);
                unify(env, deletedVarDecls, vdi->e()->id(), id1);
                pushDependentConstraints(env, id1, constraintQueue);
              }
              if (vdi->e()->e() && vdi->e()->e()->type().ispar() && vdi->e()->ti()->domain()) {
                if (vdi->e()->e()->type().isint()) {
                  IntVal iv = eval_int(env, vdi->e()->e());
                  IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                  if (!dom->contains(iv))
                    env.fail();
                } else if (vdi->e()->e()->type().isintset()) {
                  IntSetVal* isv = eval_intset(env, vdi->e()->e());
                  IntSetVal* dom = eval_intset(env, vdi->e()->ti()->domain());
                  IntSetRanges isv_r(isv);
                  IntSetRanges dom_r(dom);
                  if (!Ranges::subset(isv_r, dom_r))
                    env.fail();
                } else if (vdi->e()->e()->type().isfloat()) {
                  FloatVal fv = eval_float(env, vdi->e()->e());
                  FloatSetVal* dom = eval_floatset(env, vdi->e()->ti()->domain());
                  if (!dom->contains(fv))
                    env.fail();
                }
              }
              if (vdi->e()->ti()->type()!=Type::varbool() || vdi->e()->ti()->domain()==NULL)
                pushVarDecl(env, vdi, env.vo.find(vdi->e()), vardeclQueue);
            }
            return true;
          }
        }
      }
    }
    return false;
  }
  
  int boolState(EnvI& env, Expression* e) {
    if (e->type().ispar()) {
      return eval_bool(env,e);
    } else {
      Id* id = e->cast<Id>();
      if (id->decl()->ti()->domain()==NULL)
        return 2;
      return id->decl()->ti()->domain()==constants().lit_true;
    }
  }

  int decrementNonFixedVars(UNORDERED_NAMESPACE::unordered_map<Expression*, int>& nonFixedLiteralCount, Call* c) {
    UNORDERED_NAMESPACE::unordered_map<Expression*,int>::iterator it = nonFixedLiteralCount.find(c);
    if (it==nonFixedLiteralCount.end()) {
      int nonFixedVars = 0;
      for (unsigned int i=0; i<c->args().size(); i++) {
        ArrayLit* al = follow_id(c->args()[i])->cast<ArrayLit>();
        nonFixedVars += al->v().size();
        for (unsigned int j=al->v().size(); j--;) {
          if (al->v()[j]->type().ispar())
            nonFixedVars--;
        }
      }
      nonFixedVars--; // for the identifier we're currently processing
      nonFixedLiteralCount.insert(std::make_pair(c, nonFixedVars));
      return nonFixedVars;
    } else {
      it->second--;
      return it->second;
    }
  }

  void simplifyBoolConstraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::vector<int>& vardeclQueue,
                              std::vector<Item*>& constraintQueue,
                              std::vector<Item*>& toRemove,
                              UNORDERED_NAMESPACE::unordered_map<Expression*, int>& nonFixedLiteralCount) {
    if (ii->isa<SolveI>()) {
      remove = false;
      return;
    }
    bool isTrue = vd->ti()->domain()==constants().lit_true;
    Expression* e = NULL;
    ConstraintI* ci = ii->dyn_cast<ConstraintI>();
    VarDeclI* vdi = ii->dyn_cast<VarDeclI>();
    if (ci) {
      e = ci->e();
    } else if (vdi) {
      e = vdi->e()->e();
      if (e==NULL)
        return;
      if (Id* id = e->dyn_cast<Id>()) {
        assert(id->decl()==vd);
        if (vdi->e()->ti()->domain()==NULL) {
          vdi->e()->ti()->domain(constants().boollit(isTrue));
          vardeclQueue.push_back(env.vo.idx.find(vdi->e()->id())->second);
        } else if (id->decl()->ti()->domain() == constants().boollit(!isTrue)) {
          env.fail();
          remove = false;
        }
        return;
      }
    }
    if (Id* ident = e->dyn_cast<Id>()) {
      assert(ident->decl() == vd);
      return;
    }
    if (e->isa<BoolLit>()) {
      if (e == constants().lit_true && ci) {
        toRemove.push_back(ci);
      }
      return;
    }
    Call* c = e->cast<Call>();
    if (c->id()==constants().ids.bool_eq) {
      Expression* b0 = c->args()[0];
      Expression* b1 = c->args()[1];
      int b0s = boolState(env,b0);
      int b1s = boolState(env,b1);
      if (b0s==2) {
        std::swap(b0,b1);
        std::swap(b0s,b1s);
      }
      assert(b0s!=2);
      if (ci || vdi->e()->ti()->domain()==constants().lit_true) {
        if (b0s != b1s) {
          if (b1s==2) {
            b1->cast<Id>()->decl()->ti()->domain(constants().boollit(isTrue));
            vardeclQueue.push_back(env.vo.idx.find(b1->cast<Id>()->decl()->id())->second);
            if (ci)
              toRemove.push_back(ci);
          } else {
            env.fail();
            remove = false;
          }
        } else {
          if (ci)
            toRemove.push_back(ci);
        }
      } else if (vdi && vdi->e()->ti()->domain()==constants().lit_false) {
        if (b0s != b1s) {
          if (b1s==2) {
            b1->cast<Id>()->decl()->ti()->domain(constants().boollit(isTrue));
            vardeclQueue.push_back(env.vo.idx.find(b1->cast<Id>()->decl()->id())->second);
          }
        } else {
          env.fail();
          remove = false;
        }
      } else {
        remove = false;
      }
    } else if (c->id()==constants().ids.forall || c->id()==constants().ids.exists || c->id()==constants().ids.clause) {
      if (isTrue && c->id()==constants().ids.exists) {
        if (ci) {
          toRemove.push_back(ci);
        } else {
          if (vdi->e()->ti()->domain()==NULL) {
            vdi->e()->ti()->domain(constants().lit_true);
            vardeclQueue.push_back(env.vo.idx.find(vdi->e()->id())->second);
          } else if (vdi->e()->ti()->domain()!=constants().lit_true) {
            env.fail();
            vdi->e()->e(constants().lit_true);
          }
        }
      } else if (!isTrue && c->id()==constants().ids.forall) {
        if (ci) {
          env.fail();
          toRemove.push_back(ci);
        } else {
          if (vdi->e()->ti()->domain()==NULL) {
            vdi->e()->ti()->domain(constants().lit_false);
            vardeclQueue.push_back(env.vo.idx.find(vdi->e()->id())->second);
          } else if (vdi->e()->ti()->domain()!=constants().lit_false) {
            env.fail();
            vdi->e()->e(constants().lit_false);
          }
        }
      } else {
        int nonfixed = decrementNonFixedVars(nonFixedLiteralCount,c);
        bool isConjunction = (c->id() == constants().ids.forall);
        assert(nonfixed >=0);
        if (nonfixed<=1) {
          bool subsumed = false;
          int nonfixed_i=-1;
          int nonfixed_j=-1;
          int realNonFixed = 0;
          for (unsigned int i=0; i<c->args().size(); i++) {
            bool unit = (i==0 ? isConjunction : !isConjunction);
            ArrayLit* al = follow_id(c->args()[i])->cast<ArrayLit>();
            realNonFixed += al->v().size();
            for (unsigned int j=al->v().size(); j--;) {
              if (al->v()[j]->type().ispar() || al->v()[j]->cast<Id>()->decl()->ti()->domain())
                realNonFixed--;
              if (al->v()[j]->type().ispar() && eval_bool(env,al->v()[j]) != unit) {
                subsumed = true;
                i=2; // break out of outer loop
                break;
              } else if (Id* id = al->v()[j]->dyn_cast<Id>()) {
                if (id->decl()->ti()->domain()) {
                  bool idv = (id->decl()->ti()->domain()==constants().lit_true);
                  if (unit != idv) {
                    subsumed = true;
                    i=2; // break out of outer loop
                    break;
                  }
                } else {
                  nonfixed_i = i;
                  nonfixed_j = j;
                }
              }
            }
          }
          
          if (subsumed) {
            if (ci) {
              if (isConjunction) {
                env.fail();
                ci->e(constants().lit_false);
              } else {
                toRemove.push_back(ci);
              }
            } else {
              if (vdi->e()->ti()->domain()==NULL) {
                vdi->e()->ti()->domain(constants().boollit(!isConjunction));
                vardeclQueue.push_back(env.vo.idx.find(vdi->e()->id())->second);
              } else if (vdi->e()->ti()->domain()!=constants().boollit(!isConjunction)) {
                env.fail();
                vdi->e()->e(constants().boollit(!isConjunction));
              }
            }
          } else if (realNonFixed==0) {
            if (ci) {
              if (isConjunction) {
                toRemove.push_back(ci);
              } else {
                env.fail();
                ci->e(constants().lit_false);
              }
            } else {
              if (vdi->e()->ti()->domain()==NULL) {
                vdi->e()->ti()->domain(constants().boollit(isConjunction));
                vardeclQueue.push_back(env.vo.idx.find(vdi->e()->id())->second);
              } else if (vdi->e()->ti()->domain()!=constants().boollit(isConjunction)) {
                env.fail();
                vdi->e()->e(constants().boollit(isConjunction));
              }
              toRemove.push_back(vdi);
            }
          } else {
            // not subsumed, nonfixed==1
            assert(nonfixed_i != -1);
            ArrayLit* al = follow_id(c->args()[nonfixed_i])->cast<ArrayLit>();
            Id* id = al->v()[nonfixed_j]->cast<Id>();
            if (ci || vdi->e()->ti()->domain()) {
              bool result = nonfixed_i==0;
              if (vdi && vdi->e()->ti()->domain()==constants().lit_false)
                result = !result;
              VarDecl* vd = id->decl();
              if (vd->ti()->domain()==NULL) {
                vd->ti()->domain(constants().boollit(result));
                vardeclQueue.push_back(env.vo.idx.find(vd->id())->second);
              } else if (vd->ti()->domain()!=constants().boollit(result)) {
                env.fail();
                vd->e(constants().lit_true);
              }
            } else {
              remove = false;
            }
          }
          
        } else if (c->id()==constants().ids.clause) {
          int posOrNeg = isTrue ? 0 : 1;
          ArrayLit* al = follow_id(c->args()[posOrNeg])->cast<ArrayLit>();
          ArrayLit* al_other = follow_id(c->args()[1-posOrNeg])->cast<ArrayLit>();
          
          if (ci && al->v().size()==1 && al->v()[0]!=vd->id() && al_other->v().size()==1) {
            // simple implication
            assert(al_other->v()[0]==vd->id());
            if (ci) {
              if (al->v()[0]->type().ispar()) {
                if (eval_bool(env,al->v()[0])==isTrue) {
                  toRemove.push_back(ci);
                } else {
                  env.fail();
                  remove = false;
                }
              } else {
                Id* id = al->v()[0]->cast<Id>();
                if (id->decl()->ti()->domain()==NULL) {
                  id->decl()->ti()->domain(constants().boollit(isTrue));
                  vardeclQueue.push_back(env.vo.idx.find(id->decl()->id())->second);
                } else {
                  if (id->decl()->ti()->domain()==constants().boollit(isTrue)) {
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
            for (unsigned int i=0; i<al->v().size(); i++) {
              if (al->v()[i]==vd->id()) {
                if (ci) {
                  toRemove.push_back(ci);
                } else {
                  if (vdi->e()->ti()->domain()==NULL) {
                    vdi->e()->ti()->domain(constants().lit_true);
                    vardeclQueue.push_back(env.vo.idx.find(vdi->e()->id())->second);
                  } else if (vdi->e()->ti()->domain()!=constants().lit_true) {
                    env.fail();
                    vdi->e()->e(constants().lit_true);
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

}
