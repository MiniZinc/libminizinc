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

  EE flatten_comp(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    Comprehension* c = e->cast<Comprehension>();
    KeepAlive c_ka(c);
    
    if (c->type().isopt()) {
      std::vector<Expression*> in(c->n_generators());
      std::vector<Expression*> orig_where(c->n_generators());
      std::vector<Expression*> where;
      GCLock lock;
      for (int i=0; i<c->n_generators(); i++) {
        if (c->in(i)==NULL) {
          in[i] = NULL;
          orig_where[i] = c->where(i);
        } else {
          if (c->in(i)->type().isvar() && c->in(i)->type().dim()==0) {
            std::vector<Expression*> args(1);
            args[0] = c->in(i);
            Call* ub = new Call(Location().introduce(),"ub",args);
            ub->type(Type::parsetint());
            ub->decl(env.model->matchFn(env, ub, false));
            in[i] = ub;
            for (int j=0; j<c->n_decls(i); j++) {
              BinOp* bo = new BinOp(Location().introduce(),c->decl(i,j)->id(), BOT_IN, c->in(i));
              bo->type(Type::varbool());
              where.push_back(bo);
            }
          } else {
            in[i] = c->in(i);
          }
          if (c->where(i) && c->where(i)->type().isvar()) {
            // This is a generalised where clause. Split into par and var part.
            // The par parts can remain in where clause. The var parts are translated
            // into optionality constraints.
            if (c->where(i)->isa<BinOp>() && c->where(i)->cast<BinOp>()->op()==BOT_AND) {
              std::vector<Expression*> parWhere;
              std::vector<BinOp*> todo;
              todo.push_back(c->where(i)->cast<BinOp>());
              while (!todo.empty()) {
                BinOp* bo = todo.back();
                todo.pop_back();
                if (bo->rhs()->type().ispar()) {
                  parWhere.push_back(bo->rhs());
                } else if (bo->rhs()->isa<BinOp>() && bo->rhs()->cast<BinOp>()->op()==BOT_AND) {
                  todo.push_back(bo->rhs()->cast<BinOp>());
                } else {
                  where.push_back(bo->rhs());
                }
                if (bo->lhs()->type().ispar()) {
                  parWhere.push_back(bo->lhs());
                } else if (bo->lhs()->isa<BinOp>() && bo->lhs()->cast<BinOp>()->op()==BOT_AND) {
                  todo.push_back(bo->lhs()->cast<BinOp>());
                } else {
                  where.push_back(bo->lhs());
                }
              }
              switch (parWhere.size()) {
                case 0:
                  orig_where[i] = NULL;
                  break;
                case 1:
                  orig_where[i] = parWhere[0];
                  break;
                case 2:
                  orig_where[i] = new BinOp(c->where(i)->loc(), parWhere[0], BOT_AND, parWhere[1]);
                  orig_where[i]->type(Type::parbool());
                  break;
                default:
                {
                  ArrayLit* parWhereAl = new ArrayLit(c->where(i)->loc(), parWhere);
                  parWhereAl->type(Type::parbool(1));
                  Call* forall = new Call(c->where(i)->loc(), constants().ids.forall, {parWhereAl});
                  forall->type(Type::parbool());
                  forall->decl(env.model->matchFn(env, forall, false));
                  orig_where[i] = forall;
                  break;
                }
              }
            } else {
              orig_where[i] = NULL;
              where.push_back(c->where(i));
            }
          } else {
            orig_where[i] = c->where(i);
          }
        }
      }
      if (where.size() > 0) {
        Generators gs;
        for (int i=0; i<c->n_generators(); i++) {
          std::vector<VarDecl*> vds(c->n_decls(i));
          for (int j=0; j<c->n_decls(i); j++)
            vds[j] = c->decl(i, j);
          gs._g.push_back(Generator(vds,in[i],orig_where[i]));
        }
        Expression* cond;
        if (where.size() > 1) {
          ArrayLit* al = new ArrayLit(Location().introduce(), where);
          al->type(Type::varbool(1));
          std::vector<Expression*> args(1);
          args[0] = al;
          Call* forall = new Call(Location().introduce(), constants().ids.forall, args);
          forall->type(Type::varbool());
          forall->decl(env.model->matchFn(env, forall, false));
          cond = forall;
        } else {
          cond = where[0];
        }
        
        Expression* new_e;
        
        Call* surround = env.surroundingCall();
        
        Type ntype = c->type();
        if (surround && surround->id()==constants().ids.forall) {
          new_e = new BinOp(Location().introduce(), cond, BOT_IMPL, c->e());
          new_e->type(Type::varbool());
          ntype.ot(Type::OT_PRESENT);
        } else if (surround && surround->id()==constants().ids.exists) {
          new_e = new BinOp(Location().introduce(), cond, BOT_AND, c->e());
          new_e->type(Type::varbool());
          ntype.ot(Type::OT_PRESENT);
        } else {
          Expression* r_bounds = NULL;
          if (c->e()->type().bt()==Type::BT_INT && c->e()->type().dim() == 0) {
            std::vector<Expression*> ubargs(1);
            ubargs[0] = c->e();
            if (c->e()->type().st()==Type::ST_SET) {
              Call* bc = new Call(Location().introduce(),"ub",ubargs);
              bc->type(Type::parsetint());
              bc->decl(env.model->matchFn(env, bc, false));
              r_bounds = bc;
            } else {
              Call* lbc = new Call(Location().introduce(),"lb",ubargs);
              lbc->type(Type::parint());
              lbc->decl(env.model->matchFn(env, lbc, false));
              Call* ubc = new Call(Location().introduce(),"ub",ubargs);
              ubc->type(Type::parint());
              ubc->decl(env.model->matchFn(env, ubc, false));
              r_bounds = new BinOp(Location().introduce(),lbc,BOT_DOTDOT,ubc);
              r_bounds->type(Type::parsetint());
            }
            r_bounds->addAnnotation(constants().ann.maybe_partial);
          }
          Type tt;
          tt = c->e()->type();
          tt.ti(Type::TI_VAR);
          tt.ot(Type::OT_OPTIONAL);
          
          TypeInst* ti = new TypeInst(Location().introduce(),tt,r_bounds);
          VarDecl* r = new VarDecl(c->loc(),ti,env.genId());
          r->addAnnotation(constants().ann.promise_total);
          r->introduced(true);
          r->flat(r);
          
          std::vector<Expression*> let_exprs(3);
          let_exprs[0] = r;
          BinOp* r_eq_e = new BinOp(Location().introduce(),r->id(),BOT_EQ,c->e());
          r_eq_e->type(Type::varbool());
          let_exprs[1] = new BinOp(Location().introduce(),cond,BOT_IMPL,r_eq_e);
          let_exprs[1]->type(Type::varbool());
          let_exprs[1]->addAnnotation(constants().ann.promise_total);
          let_exprs[1]->addAnnotation(constants().ann.maybe_partial);
          std::vector<Expression*> absent_r_args(1);
          absent_r_args[0] = r->id();
          Call* absent_r = new Call(Location().introduce(), "absent", absent_r_args);
          absent_r->type(Type::varbool());
          absent_r->decl(env.model->matchFn(env, absent_r, false));
          let_exprs[2] = new BinOp(Location().introduce(),cond,BOT_OR,absent_r);
          let_exprs[2]->type(Type::varbool());
          let_exprs[2]->addAnnotation(constants().ann.promise_total);
          Let* let = new Let(Location().introduce(), let_exprs, r->id());
          let->type(r->type());
          new_e = let;
        }
        Comprehension* nc = new Comprehension(c->loc(),new_e,gs,c->set());
        nc->type(ntype);
        c = nc;
        c_ka = c;
      }
    }
    
    class EvalF {
    public:
      Ctx ctx;
      EvalF(Ctx ctx0) : ctx(ctx0) {}
      typedef EE ArrayVal;
      EE e(EnvI& env, Expression* e0) {
        VarDecl* b = ctx.b==C_ROOT ? constants().var_true : NULL;
        VarDecl* r = (ctx.b == C_ROOT && e0->type().isbool() && !e0->type().isopt()) ? constants().var_true : NULL;
        return flat_exp(env,ctx,e0,r,b);
      }
    } _evalf(ctx);
    std::vector<EE> elems_ee = eval_comp<EvalF>(env,_evalf,c);
    std::vector<Expression*> elems(elems_ee.size());
    Type elemType = Type::bot();
    bool allPar = true;
    for (unsigned int i=static_cast<unsigned int>(elems.size()); i--;) {
      elems[i] = elems_ee[i].r();
      if (elemType==Type::bot())
        elemType = elems[i]->type();
      if (!elems[i]->type().ispar())
        allPar = false;
    }
    if (elemType.isbot()) {
      elemType = c->type();
      elemType.ti(Type::TI_PAR);
    }
    if (!allPar)
      elemType.ti(Type::TI_VAR);
    if (c->set())
      elemType.st(Type::ST_SET);
    else
      elemType.dim(c->type().dim());
    KeepAlive ka;
    {
      GCLock lock;
      if (c->set()) {
        if (c->type().ispar() && allPar) {
          SetLit* sl = new SetLit(c->loc(), elems);
          sl->type(elemType);
          Expression* slr = eval_par(env,sl);
          slr->type(elemType);
          ka = slr;
        } else {
          throw InternalError("var set comprehensions not supported yet");
        }
      } else {
        ArrayLit* alr = new ArrayLit(Location().introduce(),elems);
        alr->type(elemType);
        alr->flat(true);
        ka = alr;
      }
    }
    assert(!ka()->type().isbot());
    ret.b = conj(env,b,Ctx(),elems_ee);
    ret.r = bind(env,Ctx(),r,ka());
    return ret;
  }
  
}
