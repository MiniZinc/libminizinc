/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flat_exp.hh>

namespace MiniZinc {

  std::vector<Expression*> get_conjuncts(Expression* e) {
    std::vector<Expression*> conj_stack;
    std::vector<Expression*> conjuncts;
    conj_stack.push_back(e);
    while (!conj_stack.empty()) {
      Expression* e = conj_stack.back();
      conj_stack.pop_back();
      if (BinOp* bo = e->dyn_cast<BinOp>()) {
        if (bo->op()==BOT_AND) {
          conj_stack.push_back(bo->rhs());
          conj_stack.push_back(bo->lhs());
        } else {
          conjuncts.push_back(e);
        }
      } else {
        conjuncts.push_back(e);
      }
    }
    return conjuncts;
  }
  
  void classify_conjunct(Expression* e, IdMap<int>& eq_occurrences, IdMap<std::pair<Expression*,Expression*> >& eq_branches, std::vector<Expression*>& other_branches) {
    if (BinOp* bo = e->dyn_cast<BinOp>()) {
      if (bo->op()==BOT_EQ) {
        if (Id* ident = bo->lhs()->dyn_cast<Id>()) {
          if (eq_branches.find(ident)==eq_branches.end()) {
            IdMap<int>::iterator it = eq_occurrences.find(ident);
            if (it==eq_occurrences.end()) {
              eq_occurrences.insert(ident, 1);
            } else {
              eq_occurrences.get(ident)++;
            }
            eq_branches.insert(ident, std::make_pair(bo->rhs(),bo));
            return;
          }
        } else if (Id* ident = bo->rhs()->dyn_cast<Id>()) {
          if (eq_branches.find(ident)==eq_branches.end()) {
            IdMap<int>::iterator it = eq_occurrences.find(ident);
            if (it==eq_occurrences.end()) {
              eq_occurrences.insert(ident, 1);
            } else {
              eq_occurrences.get(ident)++;
            }
            eq_branches.insert(ident, std::make_pair(bo->lhs(),bo));
            return;
          }
        }
      }
    }
    other_branches.push_back(e);
  }
  
  EE flatten_ite(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    ITE* ite = e->cast<ITE>();

    // The conditions of each branch of the if-then-else
    std::vector<KeepAlive> conditions;
    // Whether the right hand side of each branch is defined
    std::vector<std::vector<KeepAlive> > defined;
    // The right hand side of each branch
    std::vector<std::vector<KeepAlive> > branches;
    // Whether all branches are fixed
    std::vector<bool> allBranchesPar;
    
    // Compute bounds of result as union bounds of all branches
    std::vector<std::vector<IntBounds>> r_bounds_int;
    std::vector<bool> r_bounds_valid_int;
    std::vector<std::vector<IntSetVal*>> r_bounds_set;
    std::vector<bool> r_bounds_valid_set;
    std::vector<std::vector<FloatBounds>> r_bounds_float;
    std::vector<bool> r_bounds_valid_float;
    
    bool allConditionsPar = true;
    bool allDefined = true;

    // The result variables of each generated conditional
    std::vector<VarDecl*> results;
    // The then-expressions of each generated conditional
    std::vector<std::vector<Expression*> > e_then;
    // The else-expressions of each generated conditional
    std::vector<Expression*> e_else;

    bool noOtherBranches = true;
    if (ite->type()==Type::varbool() && ctx.b==C_ROOT && r==constants().var_true) {
      // Check if all branches are of the form x1=e1 /\ ... /\ xn=en
      IdMap<int> eq_occurrences;
      std::vector<IdMap<std::pair<Expression*,Expression*> > > eq_branches(ite->size()+1);
      std::vector<std::vector<Expression*> > other_branches(ite->size()+1);
      for (int i=0; i<ite->size(); i++) {
        auto conjuncts = get_conjuncts(ite->e_then(i));
        for (auto c : conjuncts) {
          classify_conjunct(c, eq_occurrences, eq_branches[i], other_branches[i]);
        }
        noOtherBranches = noOtherBranches && other_branches[i].empty();
      }
      {
        auto conjuncts = get_conjuncts(ite->e_else());
        for (auto c : conjuncts) {
          classify_conjunct(c, eq_occurrences, eq_branches[ite->size()], other_branches[ite->size()]);
        }
        noOtherBranches = noOtherBranches && other_branches[ite->size()].empty();
      }
      for (auto& e : eq_occurrences) {
        if (e.second>=ite->size()) {
          // Any identifier that occurs in all or all but one branch gets its own conditional
          results.push_back(e.first->decl());
          e_then.push_back(std::vector<Expression*>());
          for (int i=0; i<ite->size(); i++) {
            IdMap<std::pair<Expression*,Expression*> >::iterator it = eq_branches[i].find(e.first);
            if (it==eq_branches[i].end()) {
              // not found, simply push x=x
              e_then.back().push_back(e.first);
            } else {
              e_then.back().push_back(it->second.first);
            }
          }
          {
            IdMap<std::pair<Expression*,Expression*> >::iterator it = eq_branches[ite->size()].find(e.first);
            if (it==eq_branches[ite->size()].end()) {
              // not found, simply push x=x
              e_else.push_back(e.first);
            } else {
              e_else.push_back(it->second.first);
            }
          }
        } else {
          // All other identifiers are put in the vector of "other" branches
          for (int i=0; i<=ite->size(); i++) {
            IdMap<std::pair<Expression*,Expression*> >::iterator it = eq_branches[i].find(e.first);
            if (it!=eq_branches[i].end()) {
              other_branches[i].push_back(it->second.second);
              noOtherBranches = false;
              eq_branches[i].remove(e.first);
            }
          }
        }
      }
      if (!noOtherBranches) {
        results.push_back(r);
        e_then.push_back(std::vector<Expression*>());
        for (int i=0; i<ite->size(); i++) {
          if (eq_branches[i].size()==0) {
            e_then.back().push_back(ite->e_then(i));
          } else if (other_branches[i].size()==0) {
            e_then.back().push_back(constants().lit_true);
          } else if (other_branches[i].size()==1) {
            e_then.back().push_back(other_branches[i][0]);
          } else {
            ArrayLit* al = new ArrayLit(Location().introduce(), other_branches[i]);
            al->type(Type::varbool(1));
            Call* forall = new Call(Location().introduce(),constants().ids.forall,{al});
            forall->decl(env.model->matchFn(env,forall,false));
            forall->type(forall->decl()->rtype(env,{al},false));
            e_then.back().push_back(forall);
          }
        }
        {
          if (eq_branches[ite->size()].size()==0) {
            e_else.push_back(ite->e_else());
          } else if (other_branches[ite->size()].size()==0) {
            e_else.push_back(constants().lit_true);
          } else if (other_branches[ite->size()].size()==1) {
            e_else.push_back(other_branches[ite->size()][0]);
          } else {
            ArrayLit* al = new ArrayLit(Location().introduce(), other_branches[ite->size()]);
            al->type(Type::varbool(1));
            Call* forall = new Call(Location().introduce(),constants().ids.forall,{al});
            forall->decl(env.model->matchFn(env,forall,false));
            forall->type(forall->decl()->rtype(env,{al},false));
            e_else.push_back(forall);
          }
        }
      }
    } else {
      noOtherBranches = false;
    }
    allBranchesPar.resize(results.size());
    r_bounds_valid_int.resize(results.size());
    r_bounds_int.resize(results.size());
    r_bounds_valid_float.resize(results.size());
    r_bounds_float.resize(results.size());
    r_bounds_valid_set.resize(results.size());
    r_bounds_set.resize(results.size());
    defined.resize(results.size());
    branches.resize(results.size());
    for (unsigned int i=0; i<results.size(); i++) {
      allBranchesPar[i] = true;
      r_bounds_valid_int[i] = true;
      r_bounds_valid_float[i] = true;
      r_bounds_valid_set[i] = true;
    }

    Ctx cmix;
    cmix.b = C_MIX;
    cmix.i = C_MIX;
    
    for (int i=0; i<ite->size(); i++) {
      bool cond = true;
      EE e_if = flat_exp(env,cmix,ite->e_if(i),NULL,constants().var_true);
      if (e_if.r()->type()==Type::parbool()) {
        {
          GCLock lock;
          cond = eval_bool(env,e_if.r());
        }
        if (cond) {
          if (allConditionsPar || conditions.size()==0) {
            // no var conditions before this one, so we can simply emit
            // the then branch
            return flat_exp(env,ctx,ite->e_then(i),r,b);
          }
          // had var conditions, so we have to take them into account
          // and emit new conditional clause
          Ctx cmix;
          cmix.b = C_MIX;
          cmix.i = C_MIX;
          // add another condition and definedness variable
          conditions.push_back(constants().lit_true);
          for (unsigned int j=0; j<results.size(); j++) {
            EE ethen = flat_exp(env, cmix, e_then[j][i], NULL, NULL);
            assert(ethen.b());
            defined[j].push_back(ethen.b);
            allDefined = allDefined && (ethen.b()==constants().lit_true);
            branches[j].push_back(ethen.r);
            if (ethen.r()->type().isvar()) {
              allBranchesPar[j] = false;
            }
          }
          break;
        }
      } else {
        allConditionsPar = false;
        // add current condition and definedness variable
        conditions.push_back(e_if.r);

        for (unsigned int j=0; j<results.size(); j++) {
          // flatten the then branch
          EE ethen = flat_exp(env, cmix, e_then[j][i], NULL, NULL);

          assert(ethen.b());
          defined[j].push_back(ethen.b);
          allDefined = allDefined && (ethen.b()==constants().lit_true);
          branches[j].push_back(ethen.r);
          if (ethen.r()->type().isvar()) {
            allBranchesPar[j] = false;
          }
        }
      }
      // update bounds
      
      if (cond) {
        for (unsigned int j=0; j<results.size(); j++) {
          if (r_bounds_valid_int[j] && e_then[j][i]->type().isint()) {
            GCLock lock;
            IntBounds ib_then = compute_int_bounds(env,e_then[j][i]);
            if (ib_then.valid)
              r_bounds_int[j].push_back(ib_then);
            r_bounds_valid_int[j] = r_bounds_valid_int[j] && ib_then.valid;
          } else if (r_bounds_valid_set[j] && e_then[j][i]->type().isintset()) {
            GCLock lock;
            IntSetVal* isv = compute_intset_bounds(env, e_then[j][i]);
            if (isv)
              r_bounds_set[j].push_back(isv);
            r_bounds_valid_set[j] = r_bounds_valid_set[j] && isv;
          } else if (r_bounds_valid_float[j] && e_then[j][i]->type().isfloat()) {
            GCLock lock;
            FloatBounds fb_then = compute_float_bounds(env, e_then[j][i]);
            if (fb_then.valid)
              r_bounds_float[j].push_back(fb_then);
            r_bounds_valid_float[j] = r_bounds_valid_float[j] && fb_then.valid;
          }
        }
      }
      
    }
    
    if (allConditionsPar) {
      // no var condition, and all par conditions were false,
      // so simply emit else branch
      return flat_exp(env,ctx,ite->e_else(),r,b);
    }
    
    for (unsigned int j=0; j<results.size(); j++) {
      if (results[j]==NULL) {
        // need to introduce new result variable
        GCLock lock;
        TypeInst* ti = new TypeInst(Location().introduce(),ite->type(),NULL);
        results[j] = newVarDecl(env, Ctx(), ti, NULL, NULL, NULL);
      }
    }
    
    if (conditions.back()() != constants().lit_true) {
      // The last condition wasn't fixed to true, we need to look at the else branch
      conditions.push_back(constants().lit_true);

      for (unsigned int j=0; j<results.size(); j++) {
        VarDecl* nr = results[j];
      
        // update bounds of result with bounds of else branch
        
        if (r_bounds_valid_int[j] && e_else[j]->type().isint()) {
          GCLock lock;
          IntBounds ib_else = compute_int_bounds(env,e_else[j]);
          if (ib_else.valid) {
            r_bounds_int[j].push_back(ib_else);
            IntVal lb = IntVal::infinity();
            IntVal ub = -IntVal::infinity();
            for (unsigned int i=0; i<r_bounds_int[j].size(); i++) {
              lb = std::min(lb, r_bounds_int[j][i].l);
              ub = std::max(ub, r_bounds_int[j][i].u);
            }
            if (results[j]) {
              IntBounds orig_r_bounds = compute_int_bounds(env,results[j]->id());
              if (orig_r_bounds.valid) {
                lb = std::max(lb,orig_r_bounds.l);
                ub = std::min(ub,orig_r_bounds.u);
              }
            }
            SetLit* r_dom = new SetLit(Location().introduce(), IntSetVal::a(lb,ub));
            nr->ti()->domain(r_dom);
          }
        } else if (r_bounds_valid_set[j] && e_else[j]->type().isintset()) {
          GCLock lock;
          IntSetVal* isv_else = compute_intset_bounds(env, e_else[j]);
          if (isv_else) {
            IntSetVal* isv = isv_else;
            for (unsigned int i=0; i<r_bounds_set[j].size(); i++) {
              IntSetRanges i0(isv);
              IntSetRanges i1(r_bounds_set[j][i]);
              Ranges::Union<IntVal,IntSetRanges, IntSetRanges> u(i0,i1);
              isv = IntSetVal::ai(u);
            }
            if (results[j]) {
              IntSetVal* orig_r_bounds = compute_intset_bounds(env,results[j]->id());
              if (orig_r_bounds) {
                IntSetRanges i0(isv);
                IntSetRanges i1(orig_r_bounds);
                Ranges::Inter<IntVal,IntSetRanges, IntSetRanges> inter(i0,i1);
                isv = IntSetVal::ai(inter);
              }
            }
            SetLit* r_dom = new SetLit(Location().introduce(),isv);
            nr->ti()->domain(r_dom);
          }
        } else if (r_bounds_valid_float[j] && e_else[j]->type().isfloat()) {
          GCLock lock;
          FloatBounds fb_else = compute_float_bounds(env, e_else[j]);
          if (fb_else.valid) {
            FloatVal lb = fb_else.l;
            FloatVal ub = fb_else.u;
            for (unsigned int i=0; i<r_bounds_float[j].size(); i++) {
              lb = std::min(lb, r_bounds_float[j][i].l);
              ub = std::max(ub, r_bounds_float[j][i].u);
            }
            if (results[j]) {
              FloatBounds orig_r_bounds = compute_float_bounds(env,results[j]->id());
              if (orig_r_bounds.valid) {
                lb = std::max(lb,orig_r_bounds.l);
                ub = std::min(ub,orig_r_bounds.u);
              }
            }
            BinOp* r_dom = new BinOp(Location().introduce(), FloatLit::a(lb), BOT_DOTDOT, FloatLit::a(ub));
            r_dom->type(Type::parfloat(1));
            nr->ti()->domain(r_dom);
          }
        }
        
        // flatten else branch
        EE eelse = flat_exp(env, cmix, e_else[j], NULL, NULL);
        assert(eelse.b());
        defined[j].push_back(eelse.b);
        allDefined = allDefined && (eelse.b()==constants().lit_true);
        branches[j].push_back(eelse.r);
        if (eelse.r()->type().isvar()) {
          allBranchesPar[j] = false;
        }
      }
    }

    // Create ite predicate calls
    GCLock lock;
    ArrayLit* al_cond = new ArrayLit(Location().introduce(), conditions);
    al_cond->type(Type::varbool(1));
    for (unsigned int j=0; j<results.size(); j++) {
      
      ArrayLit* al_branches = new ArrayLit(Location().introduce(), branches[j]);
      Type branches_t = results[j]->type();
      branches_t.dim(1);
      branches_t.ti(allBranchesPar[j] ? Type::TI_PAR : Type::TI_VAR);
      al_branches->type(branches_t);
      Call* ite_pred = new Call(ite->loc().introduce(),ASTString("if_then_else"),{al_cond,al_branches,results[j]->id()});
      ite_pred->decl(env.model->matchFn(env, ite_pred, false));
      ite_pred->type(Type::varbool());
      (void) flat_exp(env, Ctx(), ite_pred, constants().var_true, constants().var_true);
    }
    EE ret;
    if (noOtherBranches) {
      ret.r = constants().var_true->id();
    } else {
      ret.r = results.back()->id();
    }
    if (allDefined) {
      bind(env, ctx, b, constants().lit_true);
      ret.b = constants().lit_true;
    } else {
      // Otherwise, constraint linking conditions, b and the definedness variables
      if (b==NULL) {
        CallStackItem _csi(env, new StringLit(Location().introduce(), "b"));
        b = newVarDecl(env, Ctx(), new TypeInst(Location().introduce(),Type::varbool()), NULL, NULL, NULL);
      }
      ret.b = b->id();
     
      std::vector<Expression*> defined_conjunctions(ite->size()+1);
      for (unsigned int i=0; i<ite->size(); i++) {
        std::vector<Expression*> def_i;
        for (unsigned int j=0; j<defined.size(); j++) {
          if (defined[j][i]() != constants().lit_true) {
            def_i.push_back(defined[j][i]());
          }
        }
        if (def_i.size()==0) {
          defined_conjunctions[i] = constants().lit_true;
        } else if (def_i.size()==1) {
          defined_conjunctions[i] = def_i[0];
        } else {
          ArrayLit* al = new ArrayLit(Location().introduce(), def_i);
          al->type(Type::varbool(1));
          Call* forall = new Call(Location().introduce(),constants().ids.forall,{al});
          forall->decl(env.model->matchFn(env,forall,false));
          forall->type(forall->decl()->rtype(env,{al},false));
          defined_conjunctions[i] = forall;
        }
      }
      ArrayLit* al_defined = new ArrayLit(Location().introduce(), defined_conjunctions);
      al_defined->type(Type::varbool(1));
      Call* ite_defined_pred = new Call(ite->loc().introduce(),ASTString("if_then_else_partiality"),{al_cond,al_defined,b->id()});
      ite_defined_pred->decl(env.model->matchFn(env, ite_defined_pred, false));
      ite_defined_pred->type(Type::varbool());
      (void) flat_exp(env, Ctx(), ite_defined_pred, constants().var_true, constants().var_true);
    }

    return ret;
  }

}
