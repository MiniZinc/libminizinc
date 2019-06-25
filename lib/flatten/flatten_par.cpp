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
  
  EE flatten_par(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    EE ret;
    if (e->type().cv()) {
      KeepAlive ka = flat_cv_exp(env,ctx,e);
      ret.r = bind(env,ctx,r,ka());
      ret.b = bind(env,Ctx(),b,constants().lit_true);
      return ret;
    }
    if (e->type().dim() > 0) {
      EnvI::CSEMap::iterator it;
      Id* id = e->dyn_cast<Id>();
      if (id && (id->decl()->flat()==NULL || id->decl()->toplevel())) {
        VarDecl* vd = id->decl()->flat();
        if (vd==NULL) {
          vd = flat_exp(env,Ctx(),id->decl(),NULL,constants().var_true).r()->cast<Id>()->decl();
          id->decl()->flat(vd);
          ArrayLit* al = follow_id(vd->id())->cast<ArrayLit>();
          if (al->size()==0) {
            if (r==NULL)
              ret.r = al;
            else
              ret.r = bind(env,ctx,r,al);
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            return ret;
          }
        }
        ret.r = bind(env,ctx,r,e->cast<Id>()->decl()->flat()->id());
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        return ret;
      } else if ( (it = env.cse_map_find(e)) != env.cse_map_end()) {
        ret.r = bind(env,ctx,r,it->second.r()->cast<VarDecl>()->id());
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        return ret;
      } else {
        GCLock lock;
        ArrayLit* al = follow_id(eval_par(env,e))->cast<ArrayLit>();
        if (al->size()==0 || (r && r->e()==NULL)) {
          if (r==NULL)
            ret.r = al;
          else
            ret.r = bind(env,ctx,r,al);
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          return ret;
        }
        if ( (it = env.cse_map_find(al)) != env.cse_map_end()) {
          ret.r = bind(env,ctx,r,it->second.r()->cast<VarDecl>()->id());
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          return ret;
        }
        std::vector<TypeInst*> ranges(al->dims());
        for (unsigned int i=0; i<ranges.size(); i++) {
          ranges[i] = new TypeInst(e->loc(),
                                   Type(),
                                   new SetLit(Location().introduce(),IntSetVal::a(al->min(i),al->max(i))));
        }
        ASTExprVec<TypeInst> ranges_v(ranges);
        assert(!al->type().isbot());
        TypeInst* ti = new TypeInst(e->loc(),al->type(),ranges_v,NULL);
        VarDecl* vd = newVarDecl(env, ctx, ti, NULL, NULL, al);
        EE ee(vd,NULL);
        env.cse_map_insert(al,ee);
        env.cse_map_insert(vd->e(),ee);
        
        ret.r = bind(env,ctx,r,vd->id());
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        return ret;
      }
    }
    GCLock lock;
    try {
      ret.r = bind(env,ctx,r,eval_par(env,e));
      ret.b = bind(env,Ctx(),b,constants().lit_true);
    } catch (ResultUndefinedError&) {
      ret.r = createDummyValue(env, e->type());
      ret.b = bind(env,Ctx(),b,constants().lit_false);
    }
    return ret;
  }

}
