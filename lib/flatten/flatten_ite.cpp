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
  
  EE flatten_ite(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    ITE* ite = e->cast<ITE>();

    // The conditions of each branch of the if-then-else
    std::vector<KeepAlive> conditions;
    // Whether the right hand side of each branch is defined
    std::vector<KeepAlive> defined;
    // The right hand side of each branch
    std::vector<KeepAlive> branches;
    // Whether all branches are fixed
    bool allBranchesPar = true;
    
    // Compute bounds of result as union bounds of all branches
    IntBounds r_bounds(IntVal::infinity(),-IntVal::infinity(),true);
    std::vector<IntBounds> r_bounds_int;
    bool r_bounds_valid_int = true;
    std::vector<IntSetVal*> r_bounds_set;
    bool r_bounds_valid_set = true;
    std::vector<FloatBounds> r_bounds_float;
    bool r_bounds_valid_float = true;
    
    bool allConditionsPar = true;
    bool allDefined = true;

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
          EE ethen = flat_exp(env, cmix, ite->e_then(i), NULL, NULL);
          
          // add another condition and definedness variable
          conditions.push_back(constants().lit_true);
          assert(ethen.b());
          defined.push_back(ethen.b);
          allDefined = allDefined && (ethen.b()==constants().lit_true);
          branches.push_back(ethen.r);
          if (ethen.r()->type().isvar()) {
            allBranchesPar = false;
          }
          break;
        }
      } else {
        allConditionsPar = false;
        // flatten the then branch
        EE ethen = flat_exp(env, cmix, ite->e_then(i), NULL, NULL);

        // add current condition and definedness variable
        conditions.push_back(e_if.r);
        assert(ethen.b());
        defined.push_back(ethen.b);
        allDefined = allDefined && (ethen.b()==constants().lit_true);
        branches.push_back(ethen.r);
        if (ethen.r()->type().isvar()) {
          allBranchesPar = false;
        }
      }
      // update bounds
      
      if (cond) {
        if (r_bounds_valid_int && ite->e_then(i)->type().isint()) {
          GCLock lock;
          IntBounds ib_then = compute_int_bounds(env,ite->e_then(i));
          if (ib_then.valid)
            r_bounds_int.push_back(ib_then);
          r_bounds_valid_int = r_bounds_valid_int && ib_then.valid;
        } else if (r_bounds_valid_set && ite->e_then(i)->type().isintset()) {
          GCLock lock;
          IntSetVal* isv = compute_intset_bounds(env, ite->e_then(i));
          if (isv)
            r_bounds_set.push_back(isv);
          r_bounds_valid_set = r_bounds_valid_set && isv;
        } else if (r_bounds_valid_float && ite->e_then(i)->type().isfloat()) {
          GCLock lock;
          FloatBounds fb_then = compute_float_bounds(env, ite->e_then(i));
          if (fb_then.valid)
            r_bounds_float.push_back(fb_then);
          r_bounds_valid_float = r_bounds_valid_float && fb_then.valid;
        }
      }
      
    }
    
    if (allConditionsPar) {
      // no var condition, and all par conditions were false,
      // so simply emit else branch
      return flat_exp(env,ctx,ite->e_else(),r,b);
    }
    
    VarDecl* nr = r;
    if (nr==NULL) {
      // need to introduce new result variable
      GCLock lock;
      TypeInst* ti = new TypeInst(Location().introduce(),ite->type(),NULL);
      nr = newVarDecl(env, Ctx(), ti, NULL, NULL, NULL);
    }

    if (conditions.back()() != constants().lit_true) {
      // The last condition wasn't fixed to true, we need to look at the else branch

      // update bounds of result with bounds of else branch
      
      if (r_bounds_valid_int && ite->e_else()->type().isint()) {
        GCLock lock;
        IntBounds ib_else = compute_int_bounds(env,ite->e_else());
        if (ib_else.valid) {
          r_bounds_int.push_back(ib_else);
          IntVal lb = IntVal::infinity();
          IntVal ub = -IntVal::infinity();
          for (unsigned int i=0; i<r_bounds_int.size(); i++) {
            lb = std::min(lb, r_bounds_int[i].l);
            ub = std::max(ub, r_bounds_int[i].u);
          }
          if (r) {
            IntBounds orig_r_bounds = compute_int_bounds(env,r->id());
            if (orig_r_bounds.valid) {
              lb = std::max(lb,orig_r_bounds.l);
              ub = std::min(ub,orig_r_bounds.u);
            }
          }
          SetLit* r_dom = new SetLit(Location().introduce(), IntSetVal::a(lb,ub));
          nr->ti()->domain(r_dom);
        }
      } else if (r_bounds_valid_set && ite->e_else()->type().isintset()) {
        GCLock lock;
        IntSetVal* isv_else = compute_intset_bounds(env, ite->e_else());
        if (isv_else) {
          IntSetVal* isv = isv_else;
          for (unsigned int i=0; i<r_bounds_set.size(); i++) {
            IntSetRanges i0(isv);
            IntSetRanges i1(r_bounds_set[i]);
            Ranges::Union<IntVal,IntSetRanges, IntSetRanges> u(i0,i1);
            isv = IntSetVal::ai(u);
          }
          if (r) {
            IntSetVal* orig_r_bounds = compute_intset_bounds(env,r->id());
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
      } else if (r_bounds_valid_float && ite->e_else()->type().isfloat()) {
        GCLock lock;
        FloatBounds fb_else = compute_float_bounds(env, ite->e_else());
        if (fb_else.valid) {
          FloatVal lb = fb_else.l;
          FloatVal ub = fb_else.u;
          for (unsigned int i=0; i<r_bounds_float.size(); i++) {
            lb = std::min(lb, r_bounds_float[i].l);
            ub = std::max(ub, r_bounds_float[i].u);
          }
          if (r) {
            FloatBounds orig_r_bounds = compute_float_bounds(env,r->id());
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
      EE eelse = flat_exp(env, cmix, ite->e_else(), NULL, NULL);
      conditions.push_back(constants().lit_true);
      assert(eelse.b());
      defined.push_back(eelse.b);
      allDefined = allDefined && (eelse.b()==constants().lit_true);
      branches.push_back(eelse.r);
      if (eelse.r()->type().isvar()) {
        allBranchesPar = false;
      }
    }

    GCLock lock;

    // Create ite predicate call
    ArrayLit* al_cond = new ArrayLit(Location().introduce(), conditions);
    al_cond->type(Type::varbool(1));
    ArrayLit* al_branches = new ArrayLit(Location().introduce(), branches);
    Type branches_t = ite->type();
    branches_t.dim(1);
    if (allBranchesPar) {
      branches_t.ti(Type::TI_PAR);
    }
    al_branches->type(branches_t);
    Call* ite_pred = new Call(ite->loc().introduce(),ASTString("if_then_else"),{al_cond,al_branches,nr->id()});
    ite_pred->decl(env.model->matchFn(env, ite_pred, false));
    ite_pred->type(Type::varbool());
    (void) flat_exp(env, Ctx(), ite_pred, constants().var_true, constants().var_true);

    EE ret;
    ret.r = nr->id();
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

      ArrayLit* al_defined = new ArrayLit(Location().introduce(), defined);
      al_defined->type(Type::varbool(1));
      Call* ite_defined_pred = new Call(ite->loc().introduce(),ASTString("if_then_else_partiality"),{al_cond,al_defined,b->id()});
      ite_defined_pred->decl(env.model->matchFn(env, ite_defined_pred, false));
      ite_defined_pred->type(Type::varbool());
      (void) flat_exp(env, Ctx(), ite_defined_pred, constants().var_true, constants().var_true);
      
    }
    return ret;
  }

}
