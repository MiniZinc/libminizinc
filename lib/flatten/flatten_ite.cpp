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
    std::vector<Expression*> conditions;
    // Whether the right hand side of each branch is defined
    std::vector<Expression*> defined;
    
    GC::lock();
    
    // Compute bounds of result as union bounds of all branches
    IntBounds r_bounds(IntVal::infinity(),-IntVal::infinity(),true);
    std::vector<IntBounds> r_bounds_int;
    bool r_bounds_valid_int = true;
    std::vector<IntSetVal*> r_bounds_set;
    bool r_bounds_valid_set = true;
    std::vector<FloatBounds> r_bounds_float;
    bool r_bounds_valid_float = true;
    
    VarDecl* nr = r;
    
    Ctx cmix;
    cmix.b = C_MIX;
    cmix.i = C_MIX;
    
    for (int i=0; i<ite->size(); i++) {
      bool cond = true;
      if (ite->e_if(i)->type()==Type::parbool()) {
        // par bool case: evaluate condition statically
        if (ite->e_if(i)->type().cv()) {
          KeepAlive ka = flat_cv_exp(env, ctx, ite->e_if(i));
          cond = eval_bool(env, ka());
        } else {
          cond = eval_bool(env,ite->e_if(i));
        }
        if (cond) {
          if (nr==NULL || conditions.size()==0) {
            // no var conditions before this one, so we can simply emit
            // the then branch
            GC::unlock();
            return flat_exp(env,ctx,ite->e_then(i),r,b);
          }
          // had var conditions, so we have to take them into account
          // and emit new conditional clause
          Ctx cmix;
          cmix.b = C_MIX;
          cmix.i = C_MIX;
          EE ethen = flat_exp(env, cmix, ite->e_then(i), NULL, NULL);
          
          Expression* eq_then;
          if (nr == constants().var_true) {
            eq_then = ethen.r();
          } else {
            eq_then = new BinOp(Location().introduce(),nr->id(),BOT_EQ,ethen.r());
            eq_then->type(Type::varbool());
          }
          
          {
            std::vector<Expression*> neg;
            std::vector<Expression*> clauseArgs(2);
            if (b != constants().var_true)
              neg.push_back(b->id());
            // temporarily push the then part onto the conditions
            conditions.push_back(eq_then);
            clauseArgs[0] = new ArrayLit(Location().introduce(),conditions);
            clauseArgs[0]->type(Type::varbool(1));
            clauseArgs[1] = new ArrayLit(Location().introduce(),neg);
            clauseArgs[1]->type(Type::varbool(1));
            {
              // b -> r=r[i]
              Call* clause = new Call(Location().introduce(), constants().ids.clause, clauseArgs);
              clause->decl(env.model->matchFn(env, clause, false));
              clause->type(clause->decl()->rtype(env, clauseArgs, false));
              (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
            }
            conditions.pop_back();
          }
          
          // add another condition and definedness variable
          conditions.push_back(constants().lit_true);
          assert(ethen.b());
          defined.push_back(ethen.b());
        }
      } else {
        if (nr==NULL) {
          // need to introduce new result variable
          TypeInst* ti = new TypeInst(Location().introduce(),ite->type(),NULL);
          
          nr = newVarDecl(env, Ctx(), ti, NULL, NULL, NULL);
        }
        
        // flatten the then branch
        EE ethen = flat_exp(env, cmix, ite->e_then(i), NULL, NULL);
        
        Expression* eq_then;
        if (nr == constants().var_true) {
          eq_then = ethen.r();
        } else {
          eq_then = new BinOp(Location().introduce(),nr->id(),BOT_EQ,ethen.r());
          eq_then->type(Type::varbool());
        }
        
        if (b==NULL) {
          CallStackItem _csi(env, new StringLit(Location().introduce(), "b"));
          b = newVarDecl(env, Ctx(), new TypeInst(Location().introduce(),Type::varbool()), NULL, NULL, NULL);
        }
        
        {
          // Create a clause with all the previous conditions negated, the
          // current condition, and the then branch.
          // Also take partiality into account.
          std::vector<Expression*> neg(1);
          std::vector<Expression*> clauseArgs(2);
          neg[0] = ite->e_if(i);
          if (b != constants().var_true)
            neg.push_back(b->id());
          // temporarily push the then part onto the conditions
          conditions.push_back(eq_then);
          clauseArgs[0] = new ArrayLit(Location().introduce(),conditions);
          clauseArgs[0]->type(Type::varbool(1));
          clauseArgs[1] = new ArrayLit(Location().introduce(),neg);
          clauseArgs[1]->type(Type::varbool(1));
          {
            // b /\ c[i] -> r=r[i]
            Call* clause = new Call(Location().introduce(), constants().ids.clause, clauseArgs);
            clause->decl(env.model->matchFn(env, clause, false));
            clause->type(clause->decl()->rtype(env, clauseArgs, false));
            CallStackItem _csi(env, clause);
            (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
          }
          conditions.pop_back();
        }
        
        // add current condition and definedness variable
        conditions.push_back(ite->e_if(i));
        assert(ethen.b());
        defined.push_back(ethen.b());
        
      }
      // update bounds
      
      if (cond) {
        if (r_bounds_valid_int && ite->e_then(i)->type().isint()) {
          IntBounds ib_then = compute_int_bounds(env,ite->e_then(i));
          if (ib_then.valid)
            r_bounds_int.push_back(ib_then);
          r_bounds_valid_int = r_bounds_valid_int && ib_then.valid;
        } else if (r_bounds_valid_set && ite->e_then(i)->type().isintset()) {
          IntSetVal* isv = compute_intset_bounds(env, ite->e_then(i));
          if (isv)
            r_bounds_set.push_back(isv);
          r_bounds_valid_set = r_bounds_valid_set && isv;
        } else if (r_bounds_valid_float && ite->e_then(i)->type().isfloat()) {
          FloatBounds fb_then = compute_float_bounds(env, ite->e_then(i));
          if (fb_then.valid)
            r_bounds_float.push_back(fb_then);
          r_bounds_valid_float = r_bounds_valid_float && fb_then.valid;
        }
      }
      
    }
    
    if (nr==NULL || conditions.size()==0) {
      // no var condition, and all par conditions were false,
      // so simply emit else branch
      GC::unlock();
      return flat_exp(env,ctx,ite->e_else(),r,b);
    }
    
    // update bounds of result with bounds of else branch
    
    if (r_bounds_valid_int && ite->e_else()->type().isint()) {
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
    
    Expression* eq_else;
    if (nr == constants().var_true) {
      eq_else = eelse.r();
    } else {
      eq_else = new BinOp(Location().introduce(),nr->id(),BOT_EQ,eelse.r());
      eq_else->type(Type::varbool());
    }
    
    {
      // Create a clause with all the previous conditions negated, and
      // the else branch.
      // Also take partiality into account.
      std::vector<Expression*> neg;
      std::vector<Expression*> clauseArgs(2);
      if (b != constants().var_true)
        neg.push_back(b->id());
      // temporarily push the then part onto the conditions
      conditions.push_back(eq_else);
      clauseArgs[0] = new ArrayLit(Location().introduce(),conditions);
      clauseArgs[0]->type(Type::varbool(1));
      clauseArgs[1] = new ArrayLit(Location().introduce(),neg);
      clauseArgs[1]->type(Type::varbool(1));
      {
        // b /\ c[i] -> r=r[i]
        Call* clause = new Call(Location().introduce(), constants().ids.clause, clauseArgs);
        clause->decl(env.model->matchFn(env, clause, false));
        clause->type(clause->decl()->rtype(env, clauseArgs, false));
        (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
      }
      conditions.pop_back();
    }
    
    conditions.push_back(constants().lit_true);
    assert(eelse.b());
    defined.push_back(eelse.b());
    
    // If all branches are defined, then the result is also defined
    bool allDefined = true;
    for (unsigned int i=0; i<defined.size(); i++) {
      if (! (defined[i]->type().ispar() && defined[i]->type().isbool()
             && eval_bool(env,defined[i])) ) {
        allDefined = false;
        break;
      }
    }
    if (allDefined) {
      bind(env, ctx, b, constants().lit_true);
    } else {
      // Otherwise, generate clauses linking b and the definedness variables
      std::vector<Expression*> pos;
      std::vector<Expression*> neg(2);
      std::vector<Expression*> clauseArgs(2);
      
      for (unsigned int i=0; i<conditions.size(); i++) {
        neg[0] = conditions[i];
        neg[1] = b->id();
        pos.push_back(defined[i]);
        clauseArgs[0] = new ArrayLit(Location().introduce(),pos);
        clauseArgs[0]->type(Type::varbool(1));
        clauseArgs[1] = new ArrayLit(Location().introduce(),neg);
        clauseArgs[1]->type(Type::varbool(1));
        {
          // b /\ c[i] -> b[i]
          Call* clause = new Call(Location().introduce(), constants().ids.clause, clauseArgs);
          clause->decl(env.model->matchFn(env, clause, false));
          clause->type(clause->decl()->rtype(env, clauseArgs, false));
          clause->ann().add(constants().ann.promise_total);
          (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
        }
        pos.pop_back();
        if (b != constants().var_true) {
          pos.push_back(b->id());
          neg[1] = defined[i];
          clauseArgs[0] = new ArrayLit(Location().introduce(),pos);
          clauseArgs[0]->type(Type::varbool(1));
          clauseArgs[1] = new ArrayLit(Location().introduce(),neg);
          clauseArgs[1]->type(Type::varbool(1));
          {
            // b[i] /\ c -> b
            Call* clause = new Call(Location().introduce(), constants().ids.clause, clauseArgs);
            clause->decl(env.model->matchFn(env, clause, false));
            clause->type(clause->decl()->rtype(env, clauseArgs, false));
            clause->ann().add(constants().ann.promise_total);
            (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
          }
          pos.pop_back();
        }
        pos.push_back(conditions[i]);
      }
      
    }
    
    EE ret;
    ret.r = nr->id();
    ret.b = b->id();
    GC::unlock();
    return ret;
  }

}
