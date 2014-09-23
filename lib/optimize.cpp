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
    IdMap<Items>::iterator vi = _m.find(v->id());
    if (vi==_m.end()) {
      Items items; items.insert(i);
      _m.insert(v->id(), items);
    } else {
      vi->second.insert(i);
    }
  }
  
  int VarOccurrences::remove(VarDecl* v, Item* i) {
    IdMap<Items>::iterator vi = _m.find(v->id());
    assert(vi!=_m.end());
    vi->second.erase(i);
    return vi->second.size();
  }
  
  void VarOccurrences::unify(Model* m, Id* id0_0, Id *id1_0) {
    Id* id0 = id0_0->decl()->id();
    Id* id1 = id1_0->decl()->id();
    
    VarDecl* v0 = id0->decl();
    VarDecl* v1 = id1->decl();

    if (v0==v1)
      return;
    
    int v0idx = find(v0);
    assert(v0idx != -1);
    (*m)[v0idx]->remove();

    IdMap<Items>::iterator vi0 = _m.find(v0->id());
    if (vi0 != _m.end()) {
      IdMap<Items>::iterator vi1 = _m.find(v1->id());
      if (vi1 == _m.end()) {
        _m.insert(v1->id(), vi0->second);
      } else {
        vi1->second.insert(vi0->second.begin(), vi0->second.end());
      }
    }
    
    id0->redirect(id1);
    
    remove(v0);
  }
  
  void VarOccurrences::clear(void) {
    _m.clear();
    idx.clear();
  }
  
  int VarOccurrences::occurrences(VarDecl* v) {
    IdMap<Items>::iterator vi = _m.find(v->id());
    return (vi==_m.end() ? 0 : vi->second.size());
  }
  
  void CollectOccurrencesI::vVarDeclI(VarDeclI* v) {
    CollectOccurrencesE ce(vo,v);
    topDown(ce,v->e());
  }
  void CollectOccurrencesI::vConstraintI(ConstraintI* ci) {
    CollectOccurrencesE ce(vo,ci);
    topDown(ce,ci->e());
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
  
  void simplifyBoolConstraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::vector<int>& assignedBoolVars,
                              std::vector<Item*>& toRemove,
                              ExpressionMap<int>& nonFixedLiteralCount);

  
#define MZN_MODEL_INCONSISTENT std::cerr << "Model inconsistency detected\n";

  void unify(EnvI& env, Id* id0, Id* id1) {
    if (id0->decl() != id1->decl()) {
      if (isOutput(id0->decl())) {
        std::swap(id0,id1);
      }
      
      if (id0->decl()->e() != NULL) {
        id1->decl()->e(id0->decl()->e());
        id0->decl()->e(NULL);
      }
      
      // Compute intersection of domains
      if (id0->decl()->ti()->domain() != NULL) {
        if (id1->decl()->ti()->domain() != NULL) {
          
          if (id0->type().isint() || id0->type().isintset()) {
            IntSetVal* isv0 = eval_intset(id0->decl()->ti()->domain());
            IntSetVal* isv1 = eval_intset(id1->decl()->ti()->domain());
            IntSetRanges isv0r(isv0);
            IntSetRanges isv1r(isv1);
            Ranges::Inter<IntSetRanges,IntSetRanges> inter(isv0r,isv1r);
            IntSetVal* nd = IntSetVal::ai(inter);
            if (nd->size()==0) {
              MZN_MODEL_INCONSISTENT
            } else {
              id1->decl()->ti()->domain(new SetLit(Location(), nd));
            }
          } else if (id0->type().isbool()) {
            if (eval_bool(id0->decl()->ti()->domain()) != eval_bool(id1->decl()->ti()->domain()))
              MZN_MODEL_INCONSISTENT
              } else {
                // float
                BinOp* dom0 = id0->decl()->ti()->domain()->cast<BinOp>();
                BinOp* dom1 = id1->decl()->ti()->domain()->cast<BinOp>();
                FloatVal lb0 = dom0->lhs()->cast<FloatLit>()->v();
                FloatVal ub0 = dom0->rhs()->cast<FloatLit>()->v();
                FloatVal lb1 = dom1->lhs()->cast<FloatLit>()->v();
                FloatVal ub1 = dom1->rhs()->cast<FloatLit>()->v();
                FloatVal lb = std::max(lb0,lb1);
                FloatVal ub = std::min(ub0,ub1);
                if (lb != lb1 || ub != ub1) {
                  BinOp* newdom = new BinOp(Location(), new FloatLit(Location(),lb), BOT_DOTDOT, new FloatLit(Location(),ub));
                  newdom->type(Type::parsetfloat());
                  id1->decl()->ti()->domain(newdom);
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
      
      env.vo.unify(env.flat(), id0, id1);
    }
  }
  
  void optimize(Env& env) {
    EnvI& envi = env.envi();
    Model& m = *envi.flat();
    std::vector<int> toAssignBoolVars;
    std::vector<int> assignedBoolVars;
    std::vector<int> toRemoveConstraints;
    std::vector<VarDecl*> deletedVarDecls;

    GCLock lock;

    for (unsigned int i=0; i<m.size(); i++) {
      if (m[i]->removed())
        continue;
      if (ConstraintI* ci = m[i]->dyn_cast<ConstraintI>()) {
        if (!ci->removed()) {
          if (Call* c = ci->e()->dyn_cast<Call>()) {
            if ( (c->id() == constants().ids.int_.eq || c->id() == constants().ids.bool_eq || c->id() == constants().ids.float_.eq || c->id() == constants().ids.set_eq) &&
                c->args()[0]->isa<Id>() && c->args()[1]->isa<Id>() &&
                (c->args()[0]->cast<Id>()->decl()->e()==NULL || c->args()[1]->cast<Id>()->decl()->e()==NULL)) {
              unify(envi, c->args()[0]->cast<Id>(), c->args()[1]->cast<Id>());
              CollectDecls cd(envi.vo,deletedVarDecls,ci);
              topDown(cd,c);
              ci->e(constants().lit_true);
              ci->remove();
            } else if (c->decl()==constants().var_redef) {
              CollectDecls cd(envi.vo,deletedVarDecls,ci);
              topDown(cd,c);
              ci->remove();
            } else if (c->id()==constants().ids.forall) {
              ArrayLit* al = follow_id(c->args()[0])->cast<ArrayLit>();
              for (unsigned int i=al->v().size(); i--;) {
                if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                  if (id->decl()->ti()->domain()==NULL) {
                    toAssignBoolVars.push_back(envi.vo.idx.find(id->decl()->id())->second);
                  } else if (id->decl()->ti()->domain() == constants().lit_false) {
                    MZN_MODEL_INCONSISTENT
                    id->decl()->e(constants().lit_true);
                  }
                }
              }
              toRemoveConstraints.push_back(i);
            }
          } else if (Id* id = ci->e()->dyn_cast<Id>()) {
            if (id->decl()->ti()->domain() == constants().lit_false) {
              MZN_MODEL_INCONSISTENT
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
        if (!vdi->removed() && vdi->e()->e() && vdi->e()->e()->isa<Id>() && vdi->e()->type().dim()==0) {
          Id* id1 = vdi->e()->e()->cast<Id>();
          vdi->e()->e(NULL);
          unify(envi, vdi->e()->id(), id1);
        }
        if (!vdi->removed() &&
            vdi->e()->type().isbool() && vdi->e()->type().isvar() && vdi->e()->type().dim()==0
            && (vdi->e()->ti()->domain() == constants().lit_true || vdi->e()->ti()->domain() == constants().lit_false)) {
          assignedBoolVars.push_back(i);
        }
      }
    }
    for (unsigned int i=toAssignBoolVars.size(); i--;) {
      if (m[toAssignBoolVars[i]]->removed())
        continue;
      VarDeclI* vdi = m[toAssignBoolVars[i]]->cast<VarDeclI>();
      if (vdi->e()->ti()->domain()==NULL) {
        vdi->e()->ti()->domain(constants().lit_true);
        assignedBoolVars.push_back(toAssignBoolVars[i]);
      }
    }
    
    ExpressionMap<int> nonFixedLiteralCount;
    while (!assignedBoolVars.empty()) {
      int bv = assignedBoolVars.back();
      assignedBoolVars.pop_back();
      VarDecl* vd = m[bv]->cast<VarDeclI>()->e();
      bool isTrue = vd->ti()->domain() == constants().lit_true;
      bool remove = false;
      if (vd->e()) {
        if (Id* id = vd->e()->dyn_cast<Id>()) {
          if (id->decl()->ti()->domain()==NULL) {
            id->decl()->ti()->domain(vd->ti()->domain());
            assignedBoolVars.push_back(envi.vo.idx.find(id->decl()->id())->second);
            remove = true;
          } else if (id->decl()->ti()->domain() != vd->ti()->domain()) {
            MZN_MODEL_INCONSISTENT
            remove = false;
          } else {
            remove = true;
          }
        } else if (Call* c = vd->e()->dyn_cast<Call>()) {
          if (isTrue && c->id()==constants().ids.forall) {
            remove = true;
            ArrayLit* al = follow_id(c->args()[0])->cast<ArrayLit>();
            for (unsigned int i=0; i<al->v().size(); i++) {
              if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                if (id->decl()->ti()->domain()==NULL) {
                  id->decl()->ti()->domain(constants().lit_true);
                  assignedBoolVars.push_back(envi.vo.idx.find(id->decl()->id())->second);
                } else if (id->decl()->ti()->domain() == constants().lit_false) {
                  MZN_MODEL_INCONSISTENT
                  remove = false;
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
                    assignedBoolVars.push_back(envi.vo.idx.find(id->decl()->id())->second);
                  } else if (id->decl()->ti()->domain() == constants().boollit(ispos)) {
                    MZN_MODEL_INCONSISTENT
                    remove = false;
                  }
                }
              }
            }
          }
        }
      } else {
        remove = true;
      }
      std::vector<Item*> toRemove;
      IdMap<VarOccurrences::Items>::iterator it = envi.vo._m.find(vd->id());
      if (it != envi.vo._m.end()) {
        for (VarOccurrences::Items::iterator item = it->second.begin(); item != it->second.end(); ++item) {
          if (VarDeclI* vdi = (*item)->dyn_cast<VarDeclI>()) {
            if (vdi->e()->e() && vdi->e()->e()->isa<ArrayLit>()) {
              IdMap<VarOccurrences::Items>::iterator ait = envi.vo._m.find(vdi->e()->id());
              if (ait != envi.vo._m.end()) {
                for (VarOccurrences::Items::iterator aitem = ait->second.begin(); aitem != ait->second.end(); ++aitem) {
                  simplifyBoolConstraint(envi,*aitem,vd,remove,assignedBoolVars,toRemove,nonFixedLiteralCount);
                }
              }
              continue;
            }
          }
          simplifyBoolConstraint(envi,*item,vd,remove,assignedBoolVars,toRemove,nonFixedLiteralCount);
        }
      }
      for (unsigned int i=toRemove.size(); i--;) {
        if (ConstraintI* ci = toRemove[i]->dyn_cast<ConstraintI>()) {
          CollectDecls cd(envi.vo,deletedVarDecls,ci);
          topDown(cd,ci->e());
          ci->remove();
        } else {
          VarDeclI* vdi = toRemove[i]->cast<VarDeclI>();
          CollectDecls cd(envi.vo,deletedVarDecls,vdi);
          topDown(cd,vdi->e()->e());
          vdi->e()->e(NULL);
        }
      }
      if (remove) {
        deletedVarDecls.push_back(vd);
      }
    }
    for (unsigned int i=toRemoveConstraints.size(); i--;) {
      ConstraintI* ci = m[toRemoveConstraints[i]]->cast<ConstraintI>();
      CollectDecls cd(envi.vo,deletedVarDecls,ci);
      topDown(cd,ci->e());
      ci->remove();
    }
    
    while (!deletedVarDecls.empty()) {
      VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
      if (envi.vo.occurrences(cur) == 0 && !isOutput(cur)) {
        IdMap<int>::iterator cur_idx = envi.vo.idx.find(cur->id());
        if (cur_idx != envi.vo.idx.end() && !m[cur_idx->second]->removed()) {
          CollectDecls cd(envi.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
          topDown(cd,cur->e());
          m[cur_idx->second]->remove();
        }
      }
    }
  }

  int boolState(Expression* e) {
    if (e->type().ispar()) {
      return eval_bool(e);
    } else {
      Id* id = e->cast<Id>();
      if (id->decl()->ti()->domain()==NULL)
        return 2;
      return id->decl()->ti()->domain()==constants().lit_true;
    }
  }

  int decrementNonFixedVars(ExpressionMap<int>& nonFixedLiteralCount, Call* c) {
    ExpressionMap<int>::iterator it = nonFixedLiteralCount.find(c);
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
      nonFixedLiteralCount.insert(c, nonFixedVars);
      return nonFixedVars;
    } else {
      it->second--;
      return it->second;
    }
  }

  void simplifyBoolConstraint(EnvI& env, Item* ii, VarDecl* vd, bool& remove,
                              std::vector<int>& assignedBoolVars,
                              std::vector<Item*>& toRemove,
                              ExpressionMap<int>& nonFixedLiteralCount) {
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
          assignedBoolVars.push_back(env.vo.idx.find(vdi->e()->id())->second);
        } else if (id->decl()->ti()->domain() == constants().boollit(!isTrue)) {
          MZN_MODEL_INCONSISTENT
          remove = false;
        }
        return;
      }
    }
    if (Call* c = Expression::dyn_cast<Call>(e)) {
      if (c->id()==constants().ids.bool_eq) {
        Expression* b0 = c->args()[0];
        Expression* b1 = c->args()[1];
        int b0s = boolState(b0);
        int b1s = boolState(b1);
        if (b0s==2) {
          std::swap(b0,b1);
          std::swap(b0s,b1s);
        }
        assert(b0s!=2);
        if (ci || vdi->e()->ti()->domain()==constants().lit_true) {
          if (b0s != b1s) {
            if (b1s==2) {
              b1->cast<Id>()->decl()->ti()->domain(constants().boollit(isTrue));
              assignedBoolVars.push_back(env.vo.idx.find(b1->cast<Id>()->decl()->id())->second);
              if (ci)
                toRemove.push_back(ci);
            } else {
              MZN_MODEL_INCONSISTENT
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
              assignedBoolVars.push_back(env.vo.idx.find(b1->cast<Id>()->decl()->id())->second);
            }
          } else {
            MZN_MODEL_INCONSISTENT
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
              assignedBoolVars.push_back(env.vo.idx.find(vdi->e()->id())->second);
            } else if (vdi->e()->ti()->domain()!=constants().lit_true) {
              MZN_MODEL_INCONSISTENT
              vdi->e()->e(constants().lit_true);
            }
          }
        } else if (!isTrue && c->id()==constants().ids.forall) {
          if (ci) {
            MZN_MODEL_INCONSISTENT
            toRemove.push_back(ci);
          } else {
            if (vdi->e()->ti()->domain()==NULL) {
              vdi->e()->ti()->domain(constants().lit_false);
              assignedBoolVars.push_back(env.vo.idx.find(vdi->e()->id())->second);
            } else if (vdi->e()->ti()->domain()!=constants().lit_false) {
              MZN_MODEL_INCONSISTENT
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
                if (al->v()[j]->type().ispar() && eval_bool(al->v()[j]) != unit) {
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
                  MZN_MODEL_INCONSISTENT
                  ci->e(constants().lit_false);
                } else {
                  toRemove.push_back(ci);
                }
              } else {
                if (vdi->e()->ti()->domain()==NULL) {
                  vdi->e()->ti()->domain(constants().boollit(!isConjunction));
                  assignedBoolVars.push_back(env.vo.idx.find(vdi->e()->id())->second);
                } else if (vdi->e()->ti()->domain()!=constants().boollit(!isConjunction)) {
                  MZN_MODEL_INCONSISTENT
                  vdi->e()->e(constants().boollit(!isConjunction));
                }
              }
            } else if (realNonFixed==0) {
              if (ci) {
                if (isConjunction) {
                  toRemove.push_back(ci);
                } else {
                  MZN_MODEL_INCONSISTENT
                  ci->e(constants().lit_false);
                }
              } else {
                if (vdi->e()->ti()->domain()==NULL) {
                  vdi->e()->ti()->domain(constants().boollit(isConjunction));
                  assignedBoolVars.push_back(env.vo.idx.find(vdi->e()->id())->second);
                } else if (vdi->e()->ti()->domain()!=constants().boollit(isConjunction)) {
                  MZN_MODEL_INCONSISTENT
                  vdi->e()->e(constants().boollit(isConjunction));
                }
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
                  assignedBoolVars.push_back(env.vo.idx.find(vd->id())->second);
                } else if (vd->ti()->domain()!=constants().boollit(result)) {
                  MZN_MODEL_INCONSISTENT
                  vd->e(constants().lit_true);
                }
              } else {
                vdi->e()->e(id);
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
                  if (eval_bool(al->v()[0])==isTrue) {
                    toRemove.push_back(ci);
                  } else {
                    MZN_MODEL_INCONSISTENT
                    remove = false;
                  }
                } else {
                  Id* id = al->v()[0]->cast<Id>();
                  if (id->decl()->ti()->domain()==NULL) {
                    id->decl()->ti()->domain(constants().boollit(isTrue));
                    assignedBoolVars.push_back(env.vo.idx.find(id->decl()->id())->second);
                  } else {
                    if (id->decl()->ti()->domain()==constants().boollit(isTrue)) {
                      toRemove.push_back(ci);
                    } else {
                      MZN_MODEL_INCONSISTENT
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
                      assignedBoolVars.push_back(env.vo.idx.find(vdi->e()->id())->second);
                    } else if (vdi->e()->ti()->domain()!=constants().lit_true) {
                      MZN_MODEL_INCONSISTENT
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
    } else {
      remove = false;
    }
  }

}
