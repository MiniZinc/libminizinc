/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/optimize_constraints.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

  void
  OptimizeRegistry::reg(const MiniZinc::ASTString& call, optimizer opt) {
    _m.insert(std::make_pair(call, opt));
  }
  
  OptimizeRegistry::ConstraintStatus
  OptimizeRegistry::process(EnvI& env, MiniZinc::Item* i, MiniZinc::Call* c, Call*& rewrite) {
    ASTStringMap<optimizer>::t::iterator it = _m.find(c->id());
    if (it != _m.end()) {
      return it->second(env,i,c,rewrite);
    }
    return CS_NONE;
  }
  
  OptimizeRegistry&
  OptimizeRegistry::registry(void) {
    static OptimizeRegistry reg;
    return reg;
  }
  
  namespace Optimizers {
    
    /// TODO: optimizer must be able to rewrite and delete constraint
    
    OptimizeRegistry::ConstraintStatus o_linear(EnvI& env, Item* i, Call* c, Call*& rewrite) {
      ArrayLit* al_c = eval_array_lit(c->args()[0]);
      std::vector<IntVal> coeffs(al_c->v().size());
      for (unsigned int i=0; i<al_c->v().size(); i++) {
        coeffs[i] = eval_int(al_c->v()[i]);
      }
      ArrayLit* al_x = eval_array_lit(c->args()[1]);
      std::vector<KeepAlive> x(al_x->v().size());
      for (unsigned int i=0; i<al_x->v().size(); i++) {
        x[i] = al_x->v()[i];
      }
      IntVal d = 0;
      simplify_lin<IntLit>(coeffs, x, d);
      if (coeffs.size()==0) {
        bool failed;
        if (c->id()==constants().ids.int_.lin_le) {
          failed = (d <= eval_int(c->args()[2]));
        } else if (c->id()==constants().ids.int_.lin_eq) {
          failed = (d == eval_int(c->args()[2]));
        } else {
          failed = (d != eval_int(c->args()[2]));
        }
        if (failed) {
          return OptimizeRegistry::CS_FAILED;
        } else {
          return OptimizeRegistry::CS_ENTAILED;
        }
        //        } else if (coeffs.size()==1) {
        
      } else if (c->id()==constants().ids.int_.lin_eq && coeffs.size()==2  &&
                 ((coeffs[0]==1 && coeffs[1]==-1) || (coeffs[1]==1 && coeffs[0]==-1)) && eval_int(c->args()[2])-d==0) {
        std::vector<Expression*> args(2);
        args[0] = x[0](); args[1] = x[1]();
        Call* c = new Call(Location(), constants().ids.int_.eq, args);
        rewrite = c;
        return OptimizeRegistry::CS_REWRITE;
      } else
        if (coeffs.size() < al_c->v().size()) {
          std::vector<Expression*> coeffs_e(coeffs.size());
          std::vector<Expression*> x_e(coeffs.size());
          for (unsigned int i=0; i<coeffs.size(); i++) {
            coeffs_e[i] = new IntLit(Location().introduce(),coeffs[i]);
            x_e[i] = x[i]();
          }
          ArrayLit* al_c_new = new ArrayLit(al_c->loc(),coeffs_e);
          al_c_new->type(Type::parint(1));
          ArrayLit* al_x_new = new ArrayLit(al_x->loc(),x_e);
          al_x_new->type(al_x->type());
          c->args()[0] = al_c_new;
          c->args()[1] = al_x_new;
          if (d != 0) {
            c->args()[2] = new IntLit(Location().introduce(), eval_int(c->args()[2])-d);
          }
        }
      return OptimizeRegistry::CS_OK;
    }
 
    OptimizeRegistry::ConstraintStatus o_element(EnvI& env, Item* i, Call* c, Call*& rewrite) {
      if (c->args()[0]->isa<IntLit>()) {
        IntVal idx = eval_int(c->args()[0]);
        ArrayLit* al = eval_array_lit(c->args()[1]);
        Expression* result = al->v()[idx.toInt()-1];
        std::vector<Expression*> args(2);
        args[0] = result;
        args[1] = c->args()[2];
        Call* eq = new Call(Location(),constants().ids.int_.eq,args);
        rewrite = eq;
        return OptimizeRegistry::CS_REWRITE;
      }
      return OptimizeRegistry::CS_OK;
    }

    class Register {
    public:
      Register(void) {
        GC::init();
        GCLock lock;
        Model* m = new Model;
        ASTString id_element("array_int_element");
        ASTString id_var_element("array_var_int_element");
        std::vector<Expression*> e;
        e.push_back(new StringLit(Location(),id_element));
        e.push_back(new StringLit(Location(),id_var_element));
        m->addItem(new ConstraintI(Location(),new ArrayLit(Location(),e)));
        OptimizeRegistry::registry().reg(constants().ids.int_.lin_eq, o_linear);
        OptimizeRegistry::registry().reg(constants().ids.int_.lin_le, o_linear);
        OptimizeRegistry::registry().reg(constants().ids.int_.lin_ne, o_linear);
        OptimizeRegistry::registry().reg(id_element, o_element);
        OptimizeRegistry::registry().reg(id_var_element, o_element);
      }
    } _r;
    
  }
  
}