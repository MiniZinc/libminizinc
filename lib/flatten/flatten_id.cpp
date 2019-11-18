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

  EE flatten_id(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b, bool doNotFollowChains) {
    CallStackItem _csi(env,e);
    EE ret;
    Id* id = e->cast<Id>();
    if (id->decl()==NULL) {
      if (id->type().isann()) {
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,ctx,r,e);
        return ret;
      } else {
        throw FlatteningError(env,e->loc(), "undefined identifier");
      }
    }
    if (!doNotFollowChains) {
      Expression* id_f = follow_id_to_decl(id);
      if (id_f == constants().absent) {
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,ctx,r,id_f);
      } else {
        id = id_f->cast<VarDecl>()->id();
      }
    }
    if (ctx.neg && id->type().dim() > 0) {
      if (id->type().dim() > 1)
        throw InternalError("multi-dim arrays in negative positions not supported yet");
      KeepAlive ka;
      {
        GCLock lock;
        std::vector<VarDecl*> gen_id(1);
        gen_id[0] = new VarDecl(id->loc(), new TypeInst(id->loc(),Type::parint()),env.genId(),
                                IntLit::a(0));
        
        /// TODO: support arbitrary dimensions
        std::vector<Expression*> idxsetargs(1);
        idxsetargs[0] = id;
        Call* idxset = new Call(id->loc().introduce(),"index_set",idxsetargs);
        idxset->decl(env.model->matchFn(env, idxset, false));
        idxset->type(idxset->decl()->rtype(env, idxsetargs, false));
        Generator gen(gen_id,idxset,NULL);
        std::vector<Expression*> idx(1);
        Generators gens;
        gens._g.push_back(gen);
        UnOp* aanot = new UnOp(id->loc(),UOT_NOT,NULL);
        Comprehension* cp = new Comprehension(id->loc(),
                                              aanot, gens, false);
        Id* bodyidx = cp->decl(0,0)->id();
        idx[0] = bodyidx;
        ArrayAccess* aa = new ArrayAccess(id->loc(),id,idx);
        aanot->e(aa);
        Type tt = id->type();
        tt.dim(0);
        aa->type(tt);
        aanot->type(aa->type());
        cp->type(id->type());
        ctx.neg = false;
        ka = cp;
      }
      ret = flat_exp(env,ctx,ka(),r,b);
    } else {
      GCLock lock;
      VarDecl* vd = id->decl()->flat();
      Expression* rete = NULL;
      if (vd==NULL) {
        if (id->decl()->e()==NULL || id->decl()->e()->type().isann() || id->decl()->e()->type().isvar() || id->decl()->e()->type().cv() || id->decl()->e()->type().dim() > 0) {
          // New top-level id, need to copy into env.m
          vd = flat_exp(env,Ctx(),id->decl(),NULL,constants().var_true).r()
          ->cast<Id>()->decl();
        } else {
          vd = id->decl();
        }
      }
      ret.b = bind(env,Ctx(),b,constants().lit_true);
      if (vd->e()!=NULL) {
        if (vd->e()->type().ispar() && vd->e()->type().dim()==0) {
          rete = eval_par(env, vd->e());
        } else if (vd->e()->isa<Id>()) {
          rete = vd->e();
        }
      } else if (vd->ti()->ranges().size() > 0) {
        // create fresh variables and array literal
        std::vector<std::pair<int,int> > dims;
        IntVal asize = 1;
        for (unsigned int i=0; i<vd->ti()->ranges().size(); i++) {
          TypeInst* ti = vd->ti()->ranges()[i];
          if (ti->domain()==NULL)
            throw FlatteningError(env,ti->loc(),"array dimensions unknown");
          IntSetVal* isv = eval_intset(env,ti->domain());
          if (isv->size() == 0) {
            dims.push_back(std::pair<int,int>(1,0));
            asize = 0;
          } else {
            if (isv->size() != 1)
              throw FlatteningError(env,ti->loc(),"invalid array index set");
            asize *= (isv->max(0)-isv->min(0)+1);
            dims.push_back(std::pair<int,int>(static_cast<int>(isv->min(0).toInt()),
                                              static_cast<int>(isv->max(0).toInt())));
          }
        }
        Type tt = vd->ti()->type();
        tt.dim(0);
        
        if (asize > Constants::max_array_size) {
          std::ostringstream oss;
          oss << "array size (" << asize << ") exceeds maximum allowed size (" << Constants::max_array_size << ")";
          throw FlatteningError(env,vd->loc(),oss.str());
        }
        
        std::vector<Expression*> elems(static_cast<int>(asize.toInt()));
        for (int i=0; i<static_cast<int>(asize.toInt()); i++) {
          CallStackItem csi(env, IntLit::a(i));
          TypeInst* vti = new TypeInst(Location().introduce(),tt,vd->ti()->domain());
          VarDecl* nvd = newVarDecl(env,Ctx(),vti,NULL,vd,NULL);
          elems[i] = nvd->id();
        }
        // After introducing variables for each array element, the original domain can be
        // set to "computed" (since it is a consequence of the individual variable domains)
        vd->ti()->setComputedDomain(true);
        
        ArrayLit* al = new ArrayLit(Location().introduce(),elems,dims);
        al->type(vd->type());
        vd->e(al);
        env.vo_add_exp(vd);
        EE ee;
        ee.r = vd;
        env.cse_map_insert(vd->e(), ee);
      }
      if (rete==NULL) {
        if (!vd->toplevel()) {
          // create new VarDecl in toplevel, if decl doesnt exist yet
          EnvI::CSEMap::iterator it = env.cse_map_find(vd->e());
          if (it==env.cse_map_end()) {
            Expression* vde = follow_id(vd->e());
            ArrayLit* vdea = vde ? vde->dyn_cast<ArrayLit>() : NULL;
            if (vdea && vdea->size()==0) {
              // Do not create names for empty arrays but return array literal directly
              rete = vdea;
            } else {
              VarDecl* nvd = newVarDecl(env, ctx, eval_typeinst(env,vd), NULL, vd, NULL);
              
              if (vd->e()) {
                (void) flat_exp(env, Ctx(), vd->e(), nvd, constants().var_true);
              }
              vd = nvd;
              EE ee(vd,NULL);
              if (vd->e())
                env.cse_map_insert(vd->e(),ee);
            }
          } else {
            if (it->second.r()->isa<VarDecl>()) {
              vd = it->second.r()->cast<VarDecl>();
            } else {
              rete = it->second.r();
            }
          }
        }
        if (rete==NULL) {
          if (id->type().bt() == Type::BT_ANN && vd->e()) {
            rete = vd->e();
          } else {
            ArrayLit* vda = vd->dyn_cast<ArrayLit>();
            if (vda && vda->size()==0) {
              // Do not create names for empty arrays but return array literal directly
              rete = vda;
            } else {
              rete = vd->id();
            }
          }
        }
      }
      ret.r = bind(env,ctx,r,rete);
    }
    return ret;
  }

  EE flatten_id(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    return flatten_id(env,ctx,e,r,b,false);
  }

}
