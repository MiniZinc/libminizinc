/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/copy.hh>
#include <minizinc/hash.hh>
#include <minizinc/astexception.hh>
#include <minizinc/optimize.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/output.hh>

#include <minizinc/flat_exp.hh>
#include <minizinc/flatten_internal.hh>

#include <unordered_set>
#include <unordered_map>

namespace MiniZinc {

  // Create domain constraints. Return true if successful.
  bool createExplicitDomainConstraints(EnvI& envi, VarDecl* vd, Expression* domain) {
    std::vector<Call*> calls;
    Location iloc = Location().introduce();

    if(domain->type().isfloat() || domain->type().isfloatset()) {
      FloatSetVal* fsv = eval_floatset(envi, domain);
      if(fsv->size() == 1) { // Range based
        if(fsv->min() == fsv->max()) {
          calls.push_back(new Call(iloc,
            constants().ids.float_.eq,
            {vd->id(), FloatLit::a(fsv->min())}));
        } else {
          FloatSetVal* cfsv;
          if(vd->ti()->domain()) {
            cfsv = eval_floatset(envi, vd->ti()->domain());
          } else {
            cfsv = FloatSetVal::a(-FloatVal::infinity(), FloatVal::infinity());
          }
          if(cfsv->min() < fsv->min()) {
            calls.push_back(new Call(iloc,
              constants().ids.float_.le,
              {FloatLit::a(fsv->min()), vd->id()}));
          }
          if(cfsv->max() > fsv->max()) {
            calls.push_back(new Call(iloc,
              constants().ids.float_.le,
              {vd->id(), FloatLit::a(fsv->max())}));
          }
        }
      } else {
        calls.push_back(new Call(iloc,
          constants().ids.set_in,
          {vd->id(), new SetLit(iloc, fsv)}));
      }
    } else if(domain->type().isint() || domain->type().isintset()) {
      IntSetVal* isv = eval_intset(envi, domain);
      if(isv->size() == 1) { // Range based
        if(isv->min() == isv->max()) {
          calls.push_back(new Call(iloc,
            constants().ids.int_.eq,
            {vd->id(), IntLit::a(isv->min())}));
        } else {
          IntSetVal* cisv;
          if(vd->ti()->domain()) {
            cisv = eval_intset(envi, vd->ti()->domain());
          } else {
            cisv = IntSetVal::a(-IntVal::infinity(), IntVal::infinity());
          }
          if(cisv->min() < isv->min()) {
            calls.push_back(new Call(iloc,
              constants().ids.int_.le,
              {IntLit::a(isv->min()), vd->id()}));
          }
          if(cisv->max() > isv->max()) {
            calls.push_back(new Call(iloc,
              constants().ids.int_.le,
              {vd->id(), IntLit::a(isv->max())}));
          }
        }
      } else {
        calls.push_back(new Call(iloc,
          constants().ids.set_in,
          {vd->id(), new SetLit(iloc, isv)}));
      }
    } else {
      std::cerr << "Warning: domain change not handled by -g mode: " << *vd->id() << " = " << *domain << std::endl;
      return false;
    }

    int counter = 0;
    for (Call* c : calls) {
      CallStackItem csi(envi, IntLit::a(counter++));
      c->type(Type::varbool());
      c->decl(envi.model->matchFn(envi, c, true));
      flat_exp(envi, Ctx(), c, constants().var_true, constants().var_true);
    }
    return true;
  }


  void setComputedDomain(EnvI& envi, VarDecl* vd, Expression* domain, bool is_computed) {
    bool forceChange = vd->ann().contains(constants().ann.is_defined_var) || vd->introduced();

    bool change_domain = forceChange || !envi.fopts.record_domain_changes;
    if (!forceChange && envi.fopts.record_domain_changes) {
      change_domain = !createExplicitDomainConstraints(envi, vd, domain);
    }

    if (change_domain) {
      vd->ti()->domain(domain);
      vd->ti()->setComputedDomain(is_computed);
    }
  }

  /// Output operator for contexts
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, Ctx& ctx) {
    switch (ctx.b) {
    case C_ROOT: os << "R"; break;
    case C_POS: os << "+"; break;
    case C_NEG: os << "-"; break;
    case C_MIX: os << "M"; break;
    default: assert(false); break;
    }
    switch (ctx.i) {
    case C_ROOT: os << "R"; break;
    case C_POS: os << "+"; break;
    case C_NEG: os << "-"; break;
    case C_MIX: os << "M"; break;
    default: assert(false); break;
    }
    if (ctx.neg) os << "!";
    return os;
  }

  BCtx operator +(const BCtx& c) {
    switch (c) {
    case C_ROOT: return C_POS;
    case C_POS: return C_POS;
    case C_NEG: return C_NEG;
    case C_MIX: return C_MIX;
    default: assert(false); return C_ROOT;
    }
  }

  BCtx operator -(const BCtx& c) {
    switch (c) {
    case C_ROOT: return C_NEG;
    case C_POS: return C_NEG;
    case C_NEG: return C_POS;
    case C_MIX: return C_MIX;
    default: assert(false); return C_ROOT;
    }
  }

  /// Check if \a c is non-positive
  bool nonpos(const BCtx& c) {
    return c==C_NEG || c==C_MIX;
  }
  /// Check if \a c is non-negative
  bool nonneg(const BCtx& c) {
    return c==C_ROOT || c==C_POS;
  }

  void dumpEEb(const std::vector<EE>& ee) {
    for (unsigned int i=0; i<ee.size(); i++)
      std::cerr << *ee[i].b() << "\n";
  }
  void dumpEEr(const std::vector<EE>& ee) {
    for (unsigned int i=0; i<ee.size(); i++)
      std::cerr << *ee[i].r() << "\n";
  }

  std::tuple<BCtx, bool> ann2Ctx(VarDecl* vd) {
    if (vd->ann().contains(constants().ctx.root)) {
      return std::make_tuple(C_ROOT, true);
    } else if (vd->ann().contains(constants().ctx.mix)) {
      return std::make_tuple(C_MIX, true);
    } else if (vd->ann().contains(constants().ctx.pos)) {
      return std::make_tuple(C_POS, true);
    } else if (vd->ann().contains(constants().ctx.neg)) {
      return std::make_tuple(C_NEG, true);
    } else {
      return std::make_tuple(C_MIX, false);
    }
  }

  void addCtxAnn(VarDecl* vd, BCtx& c) {
    if (vd) {
      Id* ctx_id = NULL;
      BCtx nc;
      bool annotated;
      std::tie(nc, annotated) = ann2Ctx(vd);
      // If previously annotated
      if (annotated) {
        // Early exit
        if (nc == c || nc == C_ROOT || (nc == C_MIX && c != C_ROOT)){
          return;
        }
        // Remove old annotation
        switch (nc) {
          case C_ROOT: vd->ann().remove(constants().ctx.root); break;
          case C_MIX: vd->ann().remove(constants().ctx.mix); break;
          case C_POS: vd->ann().remove(constants().ctx.pos); break;
          case C_NEG: vd->ann().remove(constants().ctx.neg); break;
          default: assert(false); break;
        }
        // Determine new context
        if (c == C_ROOT) {
          nc = C_ROOT;
        } else {
          nc = C_MIX;
        }
      } else {
        nc = c;
      }
      switch (nc) {
        case C_ROOT: ctx_id=constants().ctx.root; break;
        case C_POS: ctx_id=constants().ctx.pos; break;
        case C_NEG: ctx_id=constants().ctx.neg; break;
        case C_MIX: ctx_id=constants().ctx.mix; break;
        default: assert(false); break;
      }
      vd->addAnnotation(ctx_id);
    }
  }

  void makeDefinedVar(VarDecl* vd, Call* c) {
    if (!vd->ann().contains(constants().ann.is_defined_var)) {
      std::vector<Expression*> args(1);
      args[0] = vd->id();
      Call* dv = new Call(Location().introduce(),constants().ann.defines_var,args);
      dv->type(Type::ann());
      vd->addAnnotation(constants().ann.is_defined_var);
      c->addAnnotation(dv);
    }
  }
  
  bool isDefinesVarAnn(Expression* e) {
    return e->isa<Call>() && e->cast<Call>()->id()==constants().ann.defines_var;
  }
  
  /// Check if \a e is NULL or true
  bool istrue(EnvI& env, Expression* e) {
    GCLock lock;
    return e==NULL || (e->type().ispar() && e->type().isbool()
                       && eval_bool(env,e));
  }  
  /// Check if \a e is non-NULL and false
  bool isfalse(EnvI& env, Expression* e) {
    GCLock lock;
    return e!=NULL && e->type().ispar() && e->type().isbool()
           && !eval_bool(env,e);
  }  

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);
  KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e);

  /// Use bounds from ovd for vd if they are better.
  /// Returns true if ovd's bounds are better.
  bool updateBounds(EnvI& envi, VarDecl* ovd, VarDecl* vd) {
    bool tighter = false;
    bool fixed = false;
    if(ovd->ti()->domain() || ovd->e()) {
      IntVal intval;
      FloatVal doubleval;
      bool boolval;

      if(vd->type().isint()) {
        IntBounds oldbounds = compute_int_bounds(envi, ovd->id());
        IntBounds bounds(0,0,false);
        if(vd->ti()->domain() || vd->e())
          bounds = compute_int_bounds(envi, vd->id());

        if((vd->ti()->domain() || vd->e()) && bounds.valid && bounds.l.isFinite() && bounds.u.isFinite()) {
          if(oldbounds.valid && oldbounds.l.isFinite() && oldbounds.u.isFinite()) {
            fixed = oldbounds.u == oldbounds.l || bounds.u == bounds.l;
            if(fixed) {
              tighter = true;
              intval = oldbounds.u == oldbounds.l ? oldbounds.u : bounds.l;
              ovd->ti()->domain(new SetLit(ovd->loc(), IntSetVal::a(intval, intval)));
            } else {
              IntSetVal* olddom = ovd->ti()->domain() ? eval_intset(envi, ovd->ti()->domain()) : NULL;
              IntSetVal* newdom =  vd->ti()->domain() ? eval_intset(envi,  vd->ti()->domain()) : NULL;

              if(olddom) {
                if(!newdom) {
                  tighter = true;
                } else {
                  IntSetRanges oisr(olddom);
                  IntSetRanges nisr(newdom);
                  IntSetRanges nisr_card(newdom);

                  Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(oisr, nisr);

                  if(Ranges::cardinality(inter) < Ranges::cardinality(nisr_card)) {
                    IntSetRanges oisr_inter(olddom);
                    IntSetRanges nisr_inter(newdom);
                    Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter_card(oisr_inter, nisr_inter);
                    tighter = true;
                    ovd->ti()->domain(new SetLit(ovd->loc(), IntSetVal::ai(inter_card)));
                  }
                }
              }
            }
          }
        } else {
          if(oldbounds.valid && oldbounds.l.isFinite() && oldbounds.u.isFinite()) {
            tighter = true;
            fixed = oldbounds.u == oldbounds.l;
            if(fixed) {
              intval = oldbounds.u;
              ovd->ti()->domain(new SetLit(ovd->loc(), IntSetVal::a(intval, intval)));
            }
          }
        }
      } else if(vd->type().isfloat()) {
        FloatBounds oldbounds = compute_float_bounds(envi, ovd->id());
        FloatBounds bounds(0.0, 0.0, false);
        if(vd->ti()->domain() || vd->e())
          bounds = compute_float_bounds(envi, vd->id());
        if((vd->ti()->domain() || vd->e()) && bounds.valid) {
          if(oldbounds.valid) {
            fixed = oldbounds.u == oldbounds.l || bounds.u == bounds.l;
            if(fixed) doubleval = oldbounds.u == oldbounds.l ? oldbounds.u : bounds.l;
            tighter = fixed || (oldbounds.u - oldbounds.l < bounds.u - bounds.l);
          }
        } else {
          if(oldbounds.valid) {
            tighter = true;
            fixed = oldbounds.u == oldbounds.l;
            if(fixed) doubleval = oldbounds.u;
          }
        }
      } else if(vd->type().isbool()) {
        if(ovd->ti()->domain()) {
          fixed = tighter = true;
          boolval = eval_bool(envi, ovd->ti()->domain());
        } else {
          fixed = tighter = (ovd->e() && ovd->e()->isa<BoolLit>());
          if(fixed)
            boolval = ovd->e()->cast<BoolLit>()->v();
        }
      }

      if(tighter) {
        vd->ti()->domain(ovd->ti()->domain());
        if(vd->e() == NULL && fixed) {
          if(vd->ti()->type().isvarint()) {
            vd->type(Type::parint());
            vd->ti(new TypeInst(vd->loc(), Type::parint()));
            vd->e(IntLit::a(intval));
          } else if(vd->ti()->type().isvarfloat()) {
            vd->type(Type::parfloat());
            vd->ti(new TypeInst(vd->loc(), Type::parfloat()));
            vd->e(FloatLit::a(doubleval));
          } else if(vd->ti()->type().isvarbool()) {
            vd->type(Type::parbool());
            vd->ti(new TypeInst(vd->loc(), Type::parbool()));
            vd->ti()->domain(boolval ? constants().lit_true : constants().lit_false);
            vd->e(new BoolLit(vd->loc(), boolval));
          }
        }
      }
    }

    return tighter;
  }

  std::string getPath(EnvI& env) {
    std::string path;
    std::stringstream ss;
    if(env.dumpPath(ss))
      path = ss.str();

    return path;
  }

  inline Location getLoc(EnvI& env, Expression* e1, Expression* e2) {
    if(e1) {
      return e1->loc().introduce();
    } else if(e2) {
      return e2->loc().introduce();
    } else {
      return Location().introduce();
    }
  }
  inline Id* getId(EnvI& env, Id* origId) {
    return origId ? origId : new Id(Location().introduce(),env.genId(), NULL);
  }

  StringLit* getLongestMznPathAnnotation(EnvI& env, const Expression* e) {
    StringLit* sl = NULL;

    if(const VarDecl* vd = e->dyn_cast<const VarDecl>()) {
      EnvI::ReversePathMap& reversePathMap = env.getReversePathMap();
      KeepAlive vd_decl_ka (vd->id()->decl());
      EnvI::ReversePathMap::iterator it = reversePathMap.find(vd_decl_ka);
      if(it != reversePathMap.end()) {
        sl = new StringLit(Location(), it->second);
      }
    } else {
      for(ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
        if(Call* ca = (*it)->dyn_cast<Call>()) {
          if(ca->id() == constants().ann.mzn_path) {
            StringLit* sl1 = ca->arg(0)->cast<StringLit>();
            if(sl) {
              if(sl1->v().size() > sl->v().size())
                sl = sl1;
            } else {
              sl = sl1;
            }
          }
        }
      }
    }
    return sl;
  }

  void addPathAnnotation(EnvI& env, Expression* e) {
    if(!(e->type().isann() || e->isa<Id>()) && e->type().dim() == 0) {
      GCLock lock;
      Annotation& ann = e->ann();
      if(ann.containsCall(constants().ann.mzn_path))
        return;

      EnvI::ReversePathMap& reversePathMap = env.getReversePathMap();

      std::vector<Expression*> path_args(1);
      std::string p;
      KeepAlive e_ka (e);
      EnvI::ReversePathMap::iterator it = reversePathMap.find(e_ka);
      if(it == reversePathMap.end()) {
        p = getPath(env);
      } else {
        p = it->second;
      }

      if(p.size() != 0) {
        path_args[0] = new StringLit(Location(), p);
        Call* path_call = new Call(e->loc(), constants().ann.mzn_path, path_args);
        path_call->type(Type::ann());
        e->addAnnotation(path_call);
      }
    }
  }

  VarDecl* newVarDecl(EnvI& env, Ctx ctx, TypeInst* ti, Id* origId, VarDecl* origVd, Expression* rhs) {
    VarDecl* vd = nullptr;

    // Is this vardecl already in the FlatZinc (for unification)
    bool hasBeenAdded = false;

    // Don't use paths for arrays or annotations
    if(ti->type().dim() == 0 && !ti->type().isann()) {
      std::string path = getPath(env);
      if(!path.empty()) {
        EnvI::ReversePathMap& reversePathMap = env.getReversePathMap();
        EnvI::PathMap& pathMap = env.getPathMap();
        EnvI::PathMap::iterator it = pathMap.find(path);

        if(it != pathMap.end()) {
          VarDecl* ovd = Expression::cast<VarDecl>((it->second.decl)());
          unsigned int ovd_pass = it->second.pass_no;

          if(ovd) {
            // If ovd was introduced during the same pass, we can unify
            if(env.current_pass_no == ovd_pass) {
              vd = ovd;
              if(origId)
                origId->decl(vd);
              hasBeenAdded = true;
            } else {
              vd = new VarDecl(getLoc(env, origVd, rhs), ti, getId(env, origId));
              hasBeenAdded = false;
              updateBounds(env, ovd, vd);
            }

            // Check whether ovd was unified in a previous pass
            if(ovd->id() != ovd->id()->decl()->id()) {
              // We may not have seen the pointed to decl just yet
              KeepAlive ovd_decl_ka(ovd->id()->decl());
              EnvI::ReversePathMap::iterator path2It = reversePathMap.find(ovd_decl_ka);
              if(path2It != reversePathMap.end()) {
                std::string path2 = path2It->second;
                EnvI::PathVar vd_tup {vd, env.current_pass_no};

                pathMap[path] = vd_tup;
                pathMap[path2] = vd_tup;
                KeepAlive vd_ka(vd);
                reversePathMap.insert(vd_ka, path);
              }
            }
          }
        } else {
          // Create new VarDecl and add it to the maps
          vd = new VarDecl(getLoc(env, origVd, rhs), ti, getId(env, origId));
          hasBeenAdded = false;
          EnvI::PathVar vd_tup {vd, env.current_pass_no};
          pathMap       [path] = vd_tup;
          KeepAlive vd_ka(vd);
          reversePathMap.insert(vd_ka, path);
        }
      }
    }
    if(vd == nullptr) {
      vd = new VarDecl(getLoc(env, origVd, rhs), ti, getId(env, origId));
      hasBeenAdded = false;
    }

    // If vd has an e() use bind to turn rhs into a constraint
    if (vd->e()) {
      if(rhs) {
        bind(env, ctx, vd, rhs);
      }
    } else {
      vd->e(rhs);
    }
    assert(!vd->type().isbot());
    if (origVd && (origVd->id()->idn()!=-1 || origVd->toplevel())) {
      vd->introduced(origVd->introduced());
    } else {
      vd->introduced(true);
    }

    vd->flat(vd);

    // Copy annotations from origVd
    if (origVd) {
      for (ExpressionSetIter it = origVd->ann().begin(); it != origVd->ann().end(); ++it) {
        EE ee_ann = flat_exp(env, Ctx(), *it, NULL, constants().var_true);
        vd->addAnnotation(ee_ann.r());
      }
    }

    if (!hasBeenAdded) {
      if (FunctionI* fi = env.model->matchRevMap(env, vd->type())) {
        // We need to introduce a reverse mapper
        Call* revmap = new Call(Location().introduce(), fi->id(), {vd->id()});
        revmap->decl(fi);
        revmap->type(Type::varbool());
        env.flat_addItem(new ConstraintI(Location().introduce(), revmap));
      }

      VarDeclI* ni = new VarDeclI(Location().introduce(),vd);
      env.flat_addItem(ni);
      EE ee(vd,NULL);
      env.cse_map_insert(vd->id(),ee);
    }

    return vd;
  }

#define MZN_FILL_REIFY_MAP(T,ID) reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.T.ID,constants().ids.T ## reif.ID));

  EnvI::EnvI(Model* model0, std::ostream& outstream0, std::ostream& errstream0) :
    model(model0),
    orig_model(NULL),
    output(new Model),
    outstream(outstream0),
    errstream(errstream0),
    current_pass_no(0),
    final_pass_no(1),
    maxPathDepth(0),
    ignorePartial(false),
    ignoreUnknownIds(false),
    maxCallStack(0),
    collect_vardecls(false),
    in_redundant_constraint(0),
    in_maybe_partial(0),
    pathUse(0),
    _flat(new Model),
    _failed(false),
    ids(0) {
    MZN_FILL_REIFY_MAP(int_,lin_eq);
    MZN_FILL_REIFY_MAP(int_,lin_le);
    MZN_FILL_REIFY_MAP(int_,lin_ne);
    MZN_FILL_REIFY_MAP(int_,plus);
    MZN_FILL_REIFY_MAP(int_,minus);
    MZN_FILL_REIFY_MAP(int_,times);
    MZN_FILL_REIFY_MAP(int_,div);
    MZN_FILL_REIFY_MAP(int_,mod);
    MZN_FILL_REIFY_MAP(int_,lt);
    MZN_FILL_REIFY_MAP(int_,le);
    MZN_FILL_REIFY_MAP(int_,gt);
    MZN_FILL_REIFY_MAP(int_,ge);
    MZN_FILL_REIFY_MAP(int_,eq);
    MZN_FILL_REIFY_MAP(int_,ne);
    MZN_FILL_REIFY_MAP(float_,lin_eq);
    MZN_FILL_REIFY_MAP(float_,lin_le);
    MZN_FILL_REIFY_MAP(float_,lin_lt);
    MZN_FILL_REIFY_MAP(float_,lin_ne);
    MZN_FILL_REIFY_MAP(float_,plus);
    MZN_FILL_REIFY_MAP(float_,minus);
    MZN_FILL_REIFY_MAP(float_,times);
    MZN_FILL_REIFY_MAP(float_,div);
    MZN_FILL_REIFY_MAP(float_,mod);
    MZN_FILL_REIFY_MAP(float_,lt);
    MZN_FILL_REIFY_MAP(float_,le);
    MZN_FILL_REIFY_MAP(float_,gt);
    MZN_FILL_REIFY_MAP(float_,ge);
    MZN_FILL_REIFY_MAP(float_,eq);
    MZN_FILL_REIFY_MAP(float_,ne);
    reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.forall,constants().ids.forall_reif));
    reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.bool_eq,constants().ids.bool_eq_reif));
    reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.bool_clause,constants().ids.bool_clause_reif));
    reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.clause,constants().ids.bool_clause_reif));
  }
  EnvI::~EnvI(void) {
    delete _flat;
    delete output;
    delete model;
    delete orig_model;
  }
  long long int
  EnvI::genId(void) {
      return ids++;
    }
  void EnvI::cse_map_insert(Expression* e, const EE& ee) {
      KeepAlive ka(e);
      cse_map.insert(ka,WW(ee.r(),ee.b()));
    }
  EnvI::CSEMap::iterator EnvI::cse_map_find(Expression* e) {
    KeepAlive ka(e);
    CSEMap::iterator it = cse_map.find(ka);
    if (it != cse_map.end()) {
      if (it->second.r()) {
        if (it->second.r()->isa<VarDecl>()) {
          int idx = vo.find(it->second.r()->cast<VarDecl>());
          if (idx == -1 || (*_flat)[idx]->removed())
            return cse_map.end();
        }
      } else {
        return cse_map.end();
      }
    }
    return it;
  }
  void EnvI::cse_map_remove(Expression* e) {
    KeepAlive ka(e);
    cse_map.remove(ka);
  }
  EnvI::CSEMap::iterator EnvI::cse_map_end(void) {
    return cse_map.end();
  }
  void EnvI::dump(void) {
    struct EED {
      static std::string k(Expression* e) {
        std::ostringstream oss;
        oss << *e;
        return oss.str();
      }
      static std::string d(const WW& ee) {
        std::ostringstream oss;
        oss << *ee.r() << " " << ee.b();
        return oss.str();
      }
    };
    cse_map.dump<EED>();
  }
  
  void EnvI::flat_addItem(Item* i) {
    assert(_flat);
    if (_failed)
      return;
    _flat->addItem(i);

    Expression* toAnnotate = NULL;
    Expression* toAdd = NULL;
    switch (i->iid()) {
      case Item::II_VD:
      {
        VarDeclI* vd = i->cast<VarDeclI>();
        addPathAnnotation(*this, vd->e());
        toAnnotate = vd->e()->e();
        vo.add_idx(vd, _flat->size()-1);
        toAdd = vd->e();
        break;
      }
      case Item::II_CON:
      {
        ConstraintI* ci = i->cast<ConstraintI>();

        if (ci->e()->isa<BoolLit>() && !ci->e()->cast<BoolLit>()->v()) {
          fail();
        } else {
          toAnnotate = ci->e();
          addPathAnnotation(*this, ci->e());
          toAdd = ci->e();
        }
        break;
      }
      case Item::II_SOL:
      {
        SolveI* si = i->cast<SolveI>();
        CollectOccurrencesE ce(vo,si);
        topDown(ce,si->e());
        for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it)
          topDown(ce,*it);
        break;
      }
      case Item::II_OUT:
      {
        OutputI* si = i->cast<OutputI>();
        toAdd = si->e();
        break;
      }
      default:
        break;
    }
    if (toAnnotate && toAnnotate->isa<Call>()) {
      annotateFromCallStack(toAnnotate);
    }
    if (toAdd) {
      CollectOccurrencesE ce(vo,i);
      topDown(ce,toAdd);
    }
  }

  void EnvI::annotateFromCallStack(Expression* e) {
    int prev = idStack.size() > 0 ? idStack.back() : 0;
    bool allCalls = true;
    for (int i = static_cast<int>(callStack.size())-1; i >= prev; i--) {
      Expression* ee = callStack[i]->untag();
      allCalls = allCalls && (i==callStack.size()-1 || ee->isa<Call>());
      for (ExpressionSetIter it = ee->ann().begin(); it != ee->ann().end(); ++it) {
        EE ee_ann = flat_exp(*this, Ctx(), *it, NULL, constants().var_true);
        if (allCalls || !isDefinesVarAnn(ee_ann.r()))
          e->addAnnotation(ee_ann.r());
      }
    }

  }
  
  void EnvI::copyPathMapsAndState(EnvI& env) {
    final_pass_no = env.final_pass_no;
    maxPathDepth = env.maxPathDepth;
    current_pass_no = env.current_pass_no;
    filenameMap = env.filenameMap;
    maxPathDepth = env.maxPathDepth;
    pathMap = env.getPathMap();
    reversePathMap = env.getReversePathMap();
  }
  
  void EnvI::flat_removeItem(MiniZinc::Item* i) {
    i->remove();
  }
  void EnvI::flat_removeItem(int i) {
    flat_removeItem((*_flat)[i]);
  }
  
  void EnvI::fail(const std::string& msg) {
    if (!_failed) {
      addWarning(std::string("model inconsistency detected")+(msg.empty() ? std::string() : (": "+msg)));
      _failed = true;
      for (unsigned int i=0; i<_flat->size(); i++) {
        (*_flat)[i]->remove();
      }
      ConstraintI* failedConstraint = new ConstraintI(Location().introduce(),constants().lit_false);
      _flat->addItem(failedConstraint);
      _flat->addItem(SolveI::sat(Location().introduce()));
      for (unsigned int i=0; i<output->size(); i++) {
        (*output)[i]->remove();
      }
      output->addItem(new OutputI(Location().introduce(),new ArrayLit(Location(),std::vector<Expression*>())));
      throw ModelInconsistent(*this, Location().introduce());
    }
  }
  
  bool EnvI::failed() const {
    return _failed;
  }
  
  
  unsigned int EnvI::registerEnum(VarDeclI* vdi) {
    EnumMap::iterator it = enumMap.find(vdi);
    unsigned int ret;
    if (it == enumMap.end()) {
      ret = static_cast<unsigned int>(enumVarDecls.size());
      enumVarDecls.push_back(vdi);
      enumMap.insert(std::make_pair(vdi, ret));
    } else {
      ret = it->second;
    }
    return ret+1;
  }
  VarDeclI* EnvI::getEnum(unsigned int i) const {
    assert(i > 0 && i <= enumVarDecls.size());
    return enumVarDecls[i-1];
  }
  unsigned int EnvI::registerArrayEnum(const std::vector<unsigned int>& arrayEnum) {
    std::ostringstream oss;
    for (unsigned int i=0; i<arrayEnum.size(); i++) {
      assert(arrayEnum[i] <= enumVarDecls.size());
      oss << arrayEnum[i] << ".";
    }
    ArrayEnumMap::iterator it = arrayEnumMap.find(oss.str());
    unsigned int ret;
    if (it == arrayEnumMap.end()) {
      ret = static_cast<unsigned int>(arrayEnumDecls.size());
      arrayEnumDecls.push_back(arrayEnum);
      arrayEnumMap.insert(std::make_pair(oss.str(), ret));
    } else {
      ret = it->second;
    }
    return ret+1;
  }
  const std::vector<unsigned int>& EnvI::getArrayEnum(unsigned int i) const {
    assert(i > 0 && i <= arrayEnumDecls.size());
    return arrayEnumDecls[i-1];
  }
  bool EnvI::isSubtype(const Type& t1, const Type& t2, bool strictEnums) {
    if (!t1.isSubtypeOf(t2,strictEnums))
      return false;
    if (strictEnums && t1.dim()==0 && t2.dim()!=0 && t2.enumId() != 0) {
      // set assigned to an array
      const std::vector<unsigned int>& t2enumIds = getArrayEnum(t2.enumId());
      if (t2enumIds[t2enumIds.size()-1] != 0 && t1.enumId() != t2enumIds[t2enumIds.size()-1])
        return false;
    }
    if (strictEnums && t1.dim() > 0 && t1.enumId() != t2.enumId()) {
      if (t1.enumId()==0) {
        return t1.isbot();
      }
      if (t2.enumId()!=0) {
        const std::vector<unsigned int>& t1enumIds = getArrayEnum(t1.enumId());
        const std::vector<unsigned int>& t2enumIds = getArrayEnum(t2.enumId());
        assert(t1enumIds.size() == t2enumIds.size());
        for (unsigned int i=0; i<t1enumIds.size()-1; i++) {
          if (t2enumIds[i] != 0 && t1enumIds[i] != t2enumIds[i])
            return false;
        }
        if (!t1.isbot() && t2enumIds[t1enumIds.size()-1]!=0 && t1enumIds[t1enumIds.size()-1]!=t2enumIds[t2enumIds.size()-1])
          return false;
      }
    }
    return true;
  }
  
  void EnvI::collectVarDecls(bool b) {
    collect_vardecls = b;
  }
  void EnvI::vo_add_exp(VarDecl* vd) {
    if (vd->e() && vd->e()->isa<Call>() && !vd->e()->type().isann()) {
      int prev = idStack.size() > 0 ? idStack.back() : 0;
      for (int i = static_cast<int>(callStack.size())-1; i >= prev; i--) {
        Expression* ee = callStack[i]->untag();
        for (ExpressionSetIter it = ee->ann().begin(); it != ee->ann().end(); ++it) {
          Expression* ann = *it;
          if (ann != constants().ann.add_to_output && ann != constants().ann.rhs_from_assignment) {
            bool needAnnotation = true;
            if (Call* ann_c = ann->dyn_cast<Call>()) {
              if (ann_c->id()==constants().ann.defines_var) {
                // only add defines_var annotation if vd is the defined variable
                if (Id* defined_var = ann_c->arg(0)->dyn_cast<Id>()) {
                  if (defined_var->decl() != vd->id()->decl()) {
                    needAnnotation = false;
                  }
                }
              }
            }
            if (needAnnotation) {
              EE ee_ann = flat_exp(*this, Ctx(), *it, NULL, constants().var_true);
              vd->e()->addAnnotation(ee_ann.r());
            }
          }
        }
      }
    }
    int idx = vo.find(vd);
    CollectOccurrencesE ce(vo,(*_flat)[idx]);
    topDown(ce, vd->e());
    if (collect_vardecls)
      modifiedVarDecls.push_back(idx);
  }
  Model* EnvI::flat(void) {
    return _flat;
  }
  void EnvI::swap() {
    Model* tmp = model;
    model = _flat;
    _flat = tmp;
  }
  ASTString EnvI::reifyId(const ASTString& id) {
    ASTStringMap<ASTString>::t::iterator it = reifyMap.find(id);
    if (it == reifyMap.end()) {
      return id.str()+"_reif";
    } else {
      return it->second;
    }
  }
#undef MZN_FILL_REIFY_MAP
  ASTString EnvI::halfReifyId(const ASTString& id) {
    return id.str() + "_imp";
  }

  void EnvI::addWarning(const std::string& msg) {
    if (warnings.size()>20)
      return;
    if (warnings.size()==20) {
      warnings.push_back("Further warnings have been suppressed.\n");
    } else {
      std::ostringstream oss;
      createErrorStack();
      dumpStack(oss, true);
      warnings.push_back(msg+"\n"+oss.str());
    }
  }
  
  void EnvI::createErrorStack(void) {
    errorStack.clear();
    for (unsigned int i=static_cast<unsigned int>(callStack.size()); i--;) {
      Expression* e = callStack[i]->untag();
      bool isCompIter = callStack[i]->isTagged();
      KeepAlive ka(e);
      errorStack.push_back(std::make_pair(ka,isCompIter));
    }
  }
  
  Call* EnvI::surroundingCall(void) const {
    if (callStack.size() >= 2)
      return callStack[callStack.size()-2]->untag()->dyn_cast<Call>();
    return NULL;
  }
 
  void EnvI::cleanupExceptOutput() {
    cmap.clear();
    cse_map.clear();
    delete _flat;
    delete model;
    delete orig_model;
    _flat=0;
    model=0;
  }
 
  CallStackItem::CallStackItem(EnvI& env0, Expression* e) : env(env0) {
    if (e->isa<VarDecl>())
      env.idStack.push_back(static_cast<int>(env.callStack.size()));
    if (e->isa<Call>() && e->cast<Call>()->id()=="redundant_constraint")
      env.in_redundant_constraint++;
    if (e->ann().contains(constants().ann.maybe_partial))
      env.in_maybe_partial++;
    env.callStack.push_back(e);
    env.maxCallStack = std::max(env.maxCallStack, static_cast<unsigned int>(env.callStack.size()));
  }
  CallStackItem::CallStackItem(EnvI& env0, Id* ident, IntVal i) : env(env0) {
    Expression* ee = ident->tag();
    env.callStack.push_back(ee);
    env.maxCallStack = std::max(env.maxCallStack, static_cast<unsigned int>(env.callStack.size()));
  }
  CallStackItem::~CallStackItem(void) {
    Expression* e = env.callStack.back()->untag();
    if (e->isa<VarDecl>())
      env.idStack.pop_back();
    if (e->isa<Call>() && e->cast<Call>()->id()=="redundant_constraint")
      env.in_redundant_constraint--;
    if (e->ann().contains(constants().ann.maybe_partial))
      env.in_maybe_partial--;
    env.callStack.pop_back();
  }
  
  
  FlatteningError::FlatteningError(EnvI& env, const Location& loc, const std::string& msg)
  : LocationException(env,loc,msg) {}
  
  Env::Env(Model* m, std::ostream& outstream, std::ostream& errstream) : e(new EnvI(m,outstream,errstream)) {}
  Env::~Env(void) {
    delete e;
  }
  
  Model*
  Env::model(void) { return e->model; }
  void
  Env::model(Model* m) { e->model = m; }
  Model*
  Env::flat(void) { return e->flat(); }
  void
  Env::swap() { e->swap(); }
  Model*
  Env::output(void) { return e->output; }

  std::ostream& 
  Env::evalOutput(std::ostream& os) { return e->evalOutput(os); }
  EnvI&
  Env::envi(void) { return *e; }
  const EnvI&
  Env::envi(void) const { return *e; }
  std::ostream&
  Env::dumpErrorStack(std::ostream& os) {
    return e->dumpStack(os, true);
  }
 
  bool
  EnvI::dumpPath(std::ostream& os, bool force) {
    force = force ? force : fopts.collect_mzn_paths;
    if (callStack.size() > maxPathDepth) {
      if(!force && current_pass_no >= final_pass_no-1) {
        return false;
      }
      maxPathDepth = static_cast<int>(callStack.size());
    }

    unsigned int lastError = static_cast<unsigned int>(callStack.size());

    std::string major_sep = ";";
    std::string minor_sep = "|";
    for (unsigned int i=0; i<lastError; i++) {
      Expression* e = callStack[i]->untag();
      bool isCompIter = callStack[i]->isTagged();
      Location loc = e->loc();
      int filenameId;
      std::unordered_map<std::string, int>::iterator findFilename = filenameMap.find(loc.filename().str());
      if (findFilename == filenameMap.end()) {
        if(!force && current_pass_no >= final_pass_no-1)
          return false;
        filenameId = static_cast<int>(filenameMap.size());
        filenameMap.insert(std::make_pair(loc.filename().str(), static_cast<int>(filenameMap.size())));
      } else {
        filenameId = findFilename->second;
      }


      // If this call is not a dummy StringLit with empty Location (so that deferred compilation doesn't drop the paths)
      if(e->eid() != Expression::E_STRINGLIT || loc.first_line() || loc.first_column() || loc.last_line() || loc.last_column()) {
        os << loc.filename().str() << minor_sep
           << loc.first_line()     << minor_sep
           << loc.first_column()   << minor_sep
           << loc.last_line()      << minor_sep
           << loc.last_column()    << minor_sep;
      switch (e->eid()) {
        case Expression::E_INTLIT:
          os << "il" << minor_sep << *e;
          break;
        case Expression::E_FLOATLIT:
          os << "fl" << minor_sep << *e;
          break;
        case Expression::E_SETLIT:
          os << "sl" << minor_sep << *e;
          break;
        case Expression::E_BOOLLIT:
          os << "bl" << minor_sep << *e;
          break;
        case Expression::E_STRINGLIT:
          os << "stl" << minor_sep << *e;
          break;
        case Expression::E_ID:
          if (isCompIter) {
            //if (e->cast<Id>()->decl()->e()->type().ispar())
              os << *e << "=" << *e->cast<Id>()->decl()->e();
            //else
            //  os << *e << "=?";
          } else {
            os << "id" << minor_sep << *e;
          }
          break;
        case Expression::E_ANON:
          os << "anon";
          break;
        case Expression::E_ARRAYLIT:
          os << "al";
          break;
        case Expression::E_ARRAYACCESS:
          os << "aa";
          break;
        case Expression::E_COMP:
        {
          const Comprehension* cmp = e->cast<Comprehension>();
          if (cmp->set())
            os << "sc";
          else
            os << "ac";
        }
          break;
        case Expression::E_ITE:
          os << "ite";
          break;
        case Expression::E_BINOP:
          os << "bin" << minor_sep << e->cast<BinOp>()->opToString();
          break;
        case Expression::E_UNOP:
          os << "un" << minor_sep << e->cast<UnOp>()->opToString();
          break;
        case Expression::E_CALL:
          if(fopts.only_toplevel_paths)
            return false;
          os << "ca" << minor_sep << e->cast<Call>()->id();
          break;
        case Expression::E_VARDECL:
          os << "vd";
          break;
        case Expression::E_LET:
          os << "l";
          break;
        case Expression::E_TI:
          os << "ti";
          break;
        case Expression::E_TIID:
          os << "ty";
          break;
        default:
          assert(false);
          os << "unknown expression (internal error)";
          break;
      }
      os << major_sep;
      } else {
        os << e->cast<StringLit>()->v() << major_sep;
      }
    }
    return true;
  }
  
  std::ostream&
  EnvI::dumpStack(std::ostream& os, bool errStack) {
    int lastError = 0;
    
    std::vector<Expression*> errStackCopy;
    if (errStack) {
      errStackCopy.resize(errorStack.size());
      for (unsigned int i=0; i<errorStack.size(); i++) {
        Expression* e = errorStack[i].first();
        if (errorStack[i].second) {
          e = e->tag();
        }
        errStackCopy[i] = e;
      }
    }
    
    std::vector<Expression*>& stack = errStack ? errStackCopy : callStack;
    
    for (; lastError < stack.size(); lastError++) {
      Expression* e = stack[lastError]->untag();
      bool isCompIter = stack[lastError]->isTagged();
      if (e->loc().is_introduced())
        continue;
      if (!isCompIter && e->isa<Id>()) {
        break;
      }
    }

    ASTString curloc_f;
    int curloc_l = -1;

    for (int i=lastError-1; i>=0; i--) {
      Expression* e = stack[i]->untag();
      bool isCompIter = stack[i]->isTagged();
      ASTString newloc_f = e->loc().filename();
      if (e->loc().is_introduced())
        continue;
      int newloc_l = e->loc().first_line();
      if (newloc_f != curloc_f || newloc_l != curloc_l) {
        os << "  " << newloc_f << ":" << newloc_l << ":" << std::endl;
        curloc_f = newloc_f;
        curloc_l = newloc_l;
      }
      if (isCompIter)
        os << "    with ";
      else
        os << "  in ";
      switch (e->eid()) {
        case Expression::E_INTLIT:
          os << "integer literal" << std::endl;
          break;
        case Expression::E_FLOATLIT:
          os << "float literal" << std::endl;
          break;
        case Expression::E_SETLIT:
          os << "set literal" << std::endl;
          break;
        case Expression::E_BOOLLIT:
          os << "bool literal" << std::endl;
          break;
        case Expression::E_STRINGLIT:
          os << "string literal" << std::endl;
          break;
        case Expression::E_ID:
          if (isCompIter) {
            if (e->cast<Id>()->decl()->e()->type().ispar())
              os << *e << " = " << *e->cast<Id>()->decl()->e() << std::endl;
            else
              os << *e << " = <expression>" << std::endl;
          } else {
            os << "identifier" << *e << std::endl;
          }
          break;
        case Expression::E_ANON:
          os << "anonymous variable" << std::endl;
          break;
        case Expression::E_ARRAYLIT:
          os << "array literal" << std::endl;
          break;
        case Expression::E_ARRAYACCESS:
          os << "array access" << std::endl;
          break;
        case Expression::E_COMP:
        {
          const Comprehension* cmp = e->cast<Comprehension>();
          if (cmp->set())
            os << "set ";
          else
            os << "array ";
          os << "comprehension expression" << std::endl;
        }
          break;
        case Expression::E_ITE:
          os << "if-then-else expression" << std::endl;
          break;
        case Expression::E_BINOP:
          os << "binary " << e->cast<BinOp>()->opToString() << " operator expression" << std::endl;
          break;
        case Expression::E_UNOP:
          os << "unary " << e->cast<UnOp>()->opToString() << " operator expression" << std::endl;
          break;
        case Expression::E_CALL:
          os << "call '" << e->cast<Call>()->id() << "'" << std::endl;
          break;
        case Expression::E_VARDECL:
        {
          GCLock lock;
          os << "variable declaration for '" << e->cast<VarDecl>()->id()->str() << "'" << std::endl;
        }
          break;
        case Expression::E_LET:
          os << "let expression" << std::endl;
          break;
        case Expression::E_TI:
          os << "type-inst expression" << std::endl;
          break;
        case Expression::E_TIID:
          os << "type identifier" << std::endl;
          break;
        default:
          assert(false);
          os << "unknown expression (internal error)" << std::endl;
          break;
      }
    }
    return os;
  }

  void populateOutput(Env& env) {
    EnvI& envi = env.envi();
    Model* _flat = envi.flat();
    Model* _output = envi.output;
    std::vector<Expression*> outputVars;
    for (VarDeclIterator it = _flat->begin_vardecls();
         it != _flat->end_vardecls(); ++it) {
      VarDecl* vd = it->e();
      Annotation& ann = vd->ann();
      ArrayLit* dims = NULL;
      bool has_output_ann = false;
      if(!ann.isEmpty()) {
        for(ExpressionSetIter ait = ann.begin();
            ait != ann.end(); ++ait) {
          if (Call* c = (*ait)->dyn_cast<Call>()) {
            if (c->id() == constants().ann.output_array) {
              dims = c->arg(0)->cast<ArrayLit>();
              has_output_ann = true;
              break;
            }
          } else if ((*ait)->isa<Id>() && (*ait)->cast<Id>()->str() == constants().ann.output_var->str()) {
            has_output_ann = true;
          }
        }
        if(has_output_ann) {
          std::ostringstream s;
          s << vd->id()->str().str() << " = ";
          
          VarDecl* vd_output = copy(env.envi(), vd)->cast<VarDecl>();
          Type vd_t = vd_output->type();
          vd_t.ti(Type::TI_PAR);
          vd_output->type(vd_t);
          vd_output->ti()->type(vd_t);          
          _output->addItem(new VarDeclI(Location().introduce(), vd_output));

          if (dims) {
            s << "array" << dims->size() << "d(";
            for (unsigned int i=0; i<dims->size(); i++) {
              IntSetVal* idxset = eval_intset(envi,(*dims)[i]);
              s << *idxset << ",";
            }
          }
          StringLit* sl = new StringLit(Location().introduce(),s.str());
          outputVars.push_back(sl);

          std::vector<Expression*> showArgs(1);
          showArgs[0] = vd_output->id();
          Call* show = new Call(Location().introduce(),constants().ids.show,showArgs);
          show->type(Type::parstring());
          FunctionI* fi = _flat->matchFn(envi, show, false);
          assert(fi);
          show->decl(fi);
          outputVars.push_back(show);
          std::string ends = dims ? ")" : "";
          ends += ";\n";
          StringLit* eol = new StringLit(Location().introduce(),ends);
          outputVars.push_back(eol);
        }
      }
    }
    OutputI* newOutputItem = new OutputI(Location().introduce(),new ArrayLit(Location().introduce(),outputVars));
    _output->addItem(newOutputItem);
    envi.flat()->mergeStdLib(envi, _output);
  }

  std::ostream&
  EnvI::evalOutput(std::ostream &os) {
    GCLock lock;

    ArrayLit* al = eval_array_lit(*this,output->outputItem()->e());
    bool fLastEOL = true;
    for (unsigned int i=0; i<al->size(); i++) {
      std::string s = eval_string(*this, (*al)[i]);
      if (!s.empty()) {
        os << s;
        fLastEOL = ( '\n'==s.back() );
      }
    }
    if ( !fLastEOL )
      os << '\n';
    return os;
  }
  
  const std::vector<std::string>& Env::warnings(void) {
    return envi().warnings;
  }
  
  void Env::clearWarnings(void) {
    envi().warnings.clear();
  }
  
  unsigned int Env::maxCallStack(void) const {
    return envi().maxCallStack;
  }
  

  void checkIndexSets(EnvI& env, VarDecl* vd, Expression* e) {
    ASTExprVec<TypeInst> tis = vd->ti()->ranges();
    std::vector<TypeInst*> newtis(tis.size());
    bool needNewTypeInst = false;
    GCLock lock;
    switch (e->eid()) {
      case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        ASTExprVec<TypeInst> e_tis = id->decl()->ti()->ranges();
        assert(tis.size()==e_tis.size());
        for (unsigned int i=0; i<tis.size(); i++) {
          if (tis[i]->domain()==NULL) {
            newtis[i] = e_tis[i];
            needNewTypeInst = true;
          } else {
            if (!eval_intset(env,tis[i]->domain())->equal(eval_intset(env,e_tis[i]->domain())))
              throw EvalError(env, vd->loc(), "Index set mismatch");
            newtis[i] = tis[i];
          }
        }
      }
        break;
      case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        for (unsigned int i=0; i<tis.size(); i++) {
          if (tis[i]->domain()==NULL) {
            newtis[i] = new TypeInst(Location().introduce(),Type(),new SetLit(Location().introduce(),IntSetVal::a(al->min(i),al->max(i))));
            needNewTypeInst = true;
          } else if (i==0 || al->size() != 0) {
            IntSetVal* isv = eval_intset(env,tis[i]->domain());
            assert(isv->size()<=1);
            if ( (isv->size()==0 && al->min(i) <= al->max(i)) ||
                 (isv->size()!=0 && (isv->min(0) != al->min(i) || isv->max(0) != al->max(i))) )
              throw EvalError(env, vd->loc(), "Index set mismatch");
            newtis[i] = tis[i];
          }
        }
      }
        break;
      default:
        throw InternalError("not supported yet");
    }
    if (needNewTypeInst) {
      TypeInst* tic = copy(env,vd->ti())->cast<TypeInst>();
      tic->setRanges(newtis);
      vd->ti(tic);
    }
  }
  
  /// Turn \a c into domain constraints if possible.
  /// Return whether \a c is still required in the model.
  bool checkDomainConstraints(EnvI& env, Call* c) {
    if (env.fopts.record_domain_changes) return true;
    if (c->id()==constants().ids.int_.le) {
      Expression* e0 = c->arg(0);
      Expression* e1 = c->arg(1);
      if (e0->type().ispar() && e1->isa<Id>()) {
        // greater than
        Id* id = e1->cast<Id>();
        IntVal lb = eval_int(env,e0);
        if (id->decl()->ti()->domain()) {
          IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
          if (domain->min() >= lb)
            return false;
          if (domain->max() < lb) {
            env.fail();
            return false;
          }
          IntSetRanges dr(domain);
          Ranges::Const<IntVal> cr(lb,IntVal::infinity());
          Ranges::Inter<IntVal,IntSetRanges,Ranges::Const<IntVal> > i(dr,cr);
          IntSetVal* newibv = IntSetVal::ai(i);
          id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
          id->decl()->ti()->setComputedDomain(false);
        } else {
          id->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(lb,IntVal::infinity())));
        }
        return false;
      } else if (e1->type().ispar() && e0->isa<Id>()) {
        // less than
        Id* id = e0->cast<Id>();
        IntVal ub = eval_int(env,e1);
        if (id->decl()->ti()->domain()) {
          IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
          if (domain->max() <= ub)
            return false;
          if (domain->min() > ub) {
            env.fail();
            return false;
          }
          IntSetRanges dr(domain);
          Ranges::Const<IntVal> cr(-IntVal::infinity(), ub);
          Ranges::Inter<IntVal,IntSetRanges,Ranges::Const<IntVal> > i(dr,cr);
          IntSetVal* newibv = IntSetVal::ai(i);
          id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
          id->decl()->ti()->setComputedDomain(false);
        } else {
          id->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(-IntVal::infinity(), ub)));
        }
      }
    } else if (c->id()==constants().ids.int_.lin_le) {
      ArrayLit* al_c = follow_id(c->arg(0))->cast<ArrayLit>();
      if (al_c->size()==1) {
        ArrayLit* al_x = follow_id(c->arg(1))->cast<ArrayLit>();
        IntVal coeff = eval_int(env,(*al_c)[0]);
        IntVal y = eval_int(env,c->arg(2));
        IntVal lb = -IntVal::infinity();
        IntVal ub = IntVal::infinity();
        IntVal r = y % coeff;
        if (coeff >= 0) {
          ub = y / coeff;
          if (r<0) --ub;
        } else {
          lb = y / coeff;
          if (r<0) ++lb;
        }
        if (Id* id = (*al_x)[0]->dyn_cast<Id>()) {
          if (id->decl()->ti()->domain()) {
            IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
            if (domain->max() <= ub && domain->min() >= lb)
              return false;
            if (domain->min() > ub || domain->max() < lb) {
              env.fail();
              return false;
            }
            IntSetRanges dr(domain);
            Ranges::Const<IntVal> cr(lb, ub);
            Ranges::Inter<IntVal,IntSetRanges,Ranges::Const<IntVal> > i(dr,cr);
            IntSetVal* newibv = IntSetVal::ai(i);
            id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
            id->decl()->ti()->setComputedDomain(false);
          } else {
            id->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(lb, ub)));
          }
          return false;
        }
      }
    }
    return true;
  }
  
  
  KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e) {
    assert(e==NULL || !e->isa<VarDecl>());
    if (vd==constants().var_ignore)
      return e;
    if (Id* ident = e->dyn_cast<Id>()) {
      if (ident->decl()) {
        VarDecl* e_vd = follow_id_to_decl(ident)->cast<VarDecl>();
        e = e_vd->id();
      }
    }
    if (ctx.neg) {
      assert(e->type().bt() == Type::BT_BOOL);
      if (vd==constants().var_true) {
        if (!isfalse(env,e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            while (id != NULL) {
              assert(id->decl() != NULL);
              if (id->decl()->ti()->domain() && istrue(env,id->decl()->ti()->domain())) {
                GCLock lock;
                env.flat_addItem(new ConstraintI(Location().introduce(),constants().lit_false));
              } else {
                id->decl()->ti()->domain(constants().lit_false);
                GCLock lock;
                std::vector<Expression*> args(2);
                args[0] = id;
                args[1] = constants().lit_false;
                Call* c = new Call(Location().introduce(),constants().ids.bool_eq,args);
                c->decl(env.model->matchFn(env,c,false));
                c->type(c->decl()->rtype(env,args,false));
                if (c->decl()->e()) {
                  flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                }
              }
              id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
            }
            return constants().lit_true;
          } else {
            GC::lock();
            BinOp* bo = new BinOp(e->loc(),e,BOT_EQUIV,constants().lit_false);
            bo->type(e->type());
            KeepAlive ka(bo);
            GC::unlock();
            EE ee = flat_exp(env,Ctx(),bo,NULL,constants().var_true);
            return bind(env,Ctx(),vd,ee.r());
          }
        }
        return constants().lit_true;
      } else {
        GC::lock();
        BinOp* bo = new BinOp(e->loc(),e,BOT_EQUIV,constants().lit_false);
        bo->type(e->type());
        KeepAlive ka(bo);
        GC::unlock();
        EE ee = flat_exp(env,Ctx(),bo,NULL,constants().var_true);
        return bind(env,Ctx(),vd,ee.r());
      }
    } else {
      if (vd==constants().var_true) {
        if (!istrue(env,e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            assert(id->decl() != NULL);
            while (id != NULL) {
              if (id->decl()->ti()->domain() && isfalse(env,id->decl()->ti()->domain())) {
                GCLock lock;
                env.flat_addItem(new ConstraintI(Location().introduce(),constants().lit_false));
              } else if (id->decl()->ti()->domain()==NULL) {
                id->decl()->ti()->domain(constants().lit_true);
                GCLock lock;
                std::vector<Expression*> args(2);
                args[0] = id;
                args[1] = constants().lit_true;
                Call* c = new Call(Location().introduce(),constants().ids.bool_eq,args);
                c->decl(env.model->matchFn(env,c,false));
                c->type(c->decl()->rtype(env,args,false));
                if (c->decl()->e()) {
                  flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                }
              }
              id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
            }
          } else {
            GCLock lock;
            // extract domain information from added constraint if possible
            if (!e->isa<Call>() || checkDomainConstraints(env,e->cast<Call>())) {
              env.flat_addItem(new ConstraintI(Location().introduce(),e));
            }
          }
        }
        return constants().lit_true;
      } else if (vd==constants().var_false) {
        if (!isfalse(env,e)) {
          throw InternalError("not supported yet");
        }
        return constants().lit_true;
      } else if (vd==NULL) {
        if (e==NULL) return NULL;
        switch (e->eid()) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_STRINGLIT:
        case Expression::E_ANON:
        case Expression::E_ID:
        case Expression::E_TIID:
        case Expression::E_SETLIT:
        case Expression::E_VARDECL:
          return e;
        case Expression::E_BINOP:
        case Expression::E_UNOP:
          return e; /// TODO: should not happen once operators are evaluated
        case Expression::E_ARRAYACCESS:
        case Expression::E_COMP:
        case Expression::E_ITE:
        case Expression::E_LET:
        case Expression::E_TI:
          throw InternalError("unevaluated expression");
        case Expression::E_ARRAYLIT:
          {
            GCLock lock;
            ArrayLit* al = e->cast<ArrayLit>();
            /// TODO: review if limit of 10 is a sensible choice
            if (al->type().bt()==Type::BT_ANN || al->size() <= 10)
              return e;

            EnvI::CSEMap::iterator it = env.cse_map_find(al);
            if (it != env.cse_map_end()) {
              return it->second.r()->cast<VarDecl>()->id();
            }

            std::vector<TypeInst*> ranges(al->dims());
            for (unsigned int i=0; i<ranges.size(); i++) {
              ranges[i] = new TypeInst(e->loc(),
                                       Type(),
                                       new SetLit(Location().introduce(),IntSetVal::a(al->min(i),al->max(i))));
            }
            ASTExprVec<TypeInst> ranges_v(ranges);
            assert(!al->type().isbot());
            Expression* domain = NULL;
            if (al->size() > 0 && (*al)[0]->type().isint()) {
              IntVal min = IntVal::infinity();
              IntVal max = -IntVal::infinity();
              for (unsigned int i=0; i<al->size(); i++) {
                IntBounds ib = compute_int_bounds(env,(*al)[i]);
                if (!ib.valid) {
                  min = -IntVal::infinity();
                  max = IntVal::infinity();
                  break;
                }
                min = std::min(min, ib.l);
                max = std::max(max, ib.u);
              }
              if (min != -IntVal::infinity() && max != IntVal::infinity()) {
                domain = new SetLit(Location().introduce(), IntSetVal::a(min,max));
              }
            }
            TypeInst* ti = new TypeInst(e->loc(),al->type(),ranges_v,domain);
            if (domain)
              ti->setComputedDomain(true);

            VarDecl* vd = newVarDecl(env, ctx, ti, NULL, NULL, al);
            EE ee(vd,NULL);
            env.cse_map_insert(al,ee);
            env.cse_map_insert(vd->e(),ee);
            return vd->id();
          }
        case Expression::E_CALL:
          {
            if (e->type().isann())
              return e;
            GCLock lock;
            /// TODO: handle array types
            TypeInst* ti = new TypeInst(Location().introduce(),e->type());
            VarDecl* vd = newVarDecl(env, ctx, ti, NULL, NULL, e);
            if (vd->e()->type().bt()==Type::BT_INT && vd->e()->type().dim()==0) {
              IntSetVal* ibv = NULL;
              if (vd->e()->type().is_set()) {
                ibv = compute_intset_bounds(env,vd->e());
              } else {
                IntBounds ib = compute_int_bounds(env,vd->e());
                if (ib.valid) {
                  ibv = IntSetVal::a(ib.l,ib.u);
                }
              }
              if (ibv) {
                Id* id = vd->id();
                while (id != NULL) {
                  if (id->decl()->ti()->domain()) {
                    IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
                    IntSetRanges dr(domain);
                    IntSetRanges ibr(ibv);
                    Ranges::Inter<IntVal,IntSetRanges,IntSetRanges> i(dr,ibr);
                    IntSetVal* newibv = IntSetVal::ai(i);
                    if (ibv->card() == newibv->card()) {
                      id->decl()->ti()->setComputedDomain(true);
                    } else {
                      ibv = newibv;
                    }
                  } else {
                    id->decl()->ti()->setComputedDomain(true);
                  }
                  if (id->type().st()==Type::ST_PLAIN && ibv->size()==0) {
                    env.fail();
                  } else {
                    id->decl()->ti()->domain(new SetLit(Location().introduce(),ibv));
                  }
                  id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
                }
              }
            } else if (vd->e()->type().isbool()) {
              addCtxAnn(vd, ctx.b);
            } else if (vd->e()->type().bt()==Type::BT_FLOAT && vd->e()->type().dim()==0) {
              FloatBounds fb = compute_float_bounds(env,vd->e());
              FloatSetVal* ibv = LinearTraits<FloatLit>::intersect_domain(NULL, fb.l, fb.u);
              if (fb.valid) {
                Id* id = vd->id();
                while (id != NULL) {
                  if (id->decl()->ti()->domain()) {
                    FloatSetVal* domain = eval_floatset(env,id->decl()->ti()->domain());
                    FloatSetVal* ndomain = LinearTraits<FloatLit>::intersect_domain(domain, fb.l, fb.u);
                    if (ibv && ndomain==domain) {
                      id->decl()->ti()->setComputedDomain(true);
                    } else {
                      ibv = ndomain;
                    }
                  } else {
                    id->decl()->ti()->setComputedDomain(true);
                  }
                  if (LinearTraits<FloatLit>::domain_empty(ibv)) {
                    env.fail();
                  } else {
                    id->decl()->ti()->domain(new SetLit(Location(), ibv));
                  }
                  id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
                }
              }
            }

            return vd->id();
          }
        default:
          assert(false); return NULL;
        }
      } else {
        if (vd->e()==NULL) {
          Expression* ret = e;
          if (e==NULL || (e->type().ispar() && e->type().isbool())) {
            GCLock lock;
            bool isTrue = (e==NULL || eval_bool(env,e));

            // Check if redefinition of bool_eq exists, if yes call it
            std::vector<Expression*> args(2);
            args[0] = vd->id();
            args[1] = constants().boollit(isTrue);
            Call* c = new Call(Location().introduce(),constants().ids.bool_eq,args);
            c->decl(env.model->matchFn(env,c,false));
            c->type(c->decl()->rtype(env,args,false));
            bool didRewrite = false;
            if (c->decl()->e()) {
              flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
              didRewrite = true;
            }
            
            vd->e(constants().boollit(isTrue));
            if (vd->ti()->domain()) {
              if (vd->ti()->domain() != vd->e()) {
                env.fail();
                return vd->id();
              }
            } else {
              vd->ti()->domain(vd->e());
              vd->ti()->setComputedDomain(true);
            }
            if (didRewrite) {
              return vd->id();
            }
          } else {
            if (e->type().dim() > 0) {
              // Check that index sets match
              env.errorStack.clear();
              checkIndexSets(env,vd,e);
              if (vd->ti()->domain() && e->isa<ArrayLit>()) {
                ArrayLit* al = e->cast<ArrayLit>();
                if (e->type().bt()==Type::BT_INT) {
                  IntSetVal* isv = eval_intset(env, vd->ti()->domain());
                  for (unsigned int i=0; i<al->size(); i++) {
                    if (Id* id = (*al)[i]->dyn_cast<Id>()) {
                      VarDecl* vdi = id->decl();
                      if (vdi->ti()->domain()==NULL) {
                        vdi->ti()->domain(vd->ti()->domain());
                      } else {
                        IntSetVal* vdi_dom = eval_intset(env, vdi->ti()->domain());
                        IntSetRanges isvr(isv);
                        IntSetRanges vdi_domr(vdi_dom);
                        Ranges::Inter<IntVal,IntSetRanges, IntSetRanges> inter(isvr,vdi_domr);
                        IntSetVal* newdom = IntSetVal::ai(inter);
                        if (newdom->size()==0) {
                          env.fail();
                        } else {
                          IntSetRanges vdi_domr2(vdi_dom);
                          IntSetRanges newdomr(newdom);
                          if (!Ranges::equal(vdi_domr2, newdomr)) {
                            vdi->ti()->domain(new SetLit(Location().introduce(),newdom));
                            vdi->ti()->setComputedDomain(false);
                          }
                        }
                      }
                    } else {
                      // at this point, can only be a constant
                      assert((*al)[i]->type().ispar());
                      if (e->type().st()==Type::ST_PLAIN) {
                        IntVal iv = eval_int(env, (*al)[i]);
                        if (!isv->contains(iv)) {
                          std::ostringstream oss;
                          oss << "value " << iv << " outside declared array domain " << *isv;
                          env.fail(oss.str());
                        }
                      } else {
                        IntSetVal* aisv = eval_intset(env, (*al)[i]);
                        IntSetRanges aisv_r(aisv);
                        IntSetRanges isv_r(isv);
                        if (!Ranges::subset(aisv_r,isv_r)) {
                          std::ostringstream oss;
                          oss << "value " << *aisv << " outside declared array domain " << *isv;
                          env.fail(oss.str());
                        }
                      }
                    }
                  }
                  vd->ti()->setComputedDomain(true);
                } else if (e->type().bt()==Type::BT_FLOAT) {
                  FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
                  for (unsigned int i=0; i<al->size(); i++) {
                    if (Id* id = (*al)[i]->dyn_cast<Id>()) {
                      VarDecl* vdi = id->decl();
                      if (vdi->ti()->domain()==NULL) {
                        vdi->ti()->domain(vd->ti()->domain());
                      } else {
                        FloatSetVal* vdi_dom = eval_floatset(env, vdi->ti()->domain());
                        FloatSetRanges fsvr(fsv);
                        FloatSetRanges vdi_domr(vdi_dom);
                        Ranges::Inter<FloatVal,FloatSetRanges,FloatSetRanges> inter(fsvr,vdi_domr);
                        FloatSetVal* newdom = FloatSetVal::ai(inter);
                        if (newdom->size()==0) {
                          env.fail();
                        } else {
                          FloatSetRanges vdi_domr2(vdi_dom);
                          FloatSetRanges newdomr(newdom);
                          if (!Ranges::equal(vdi_domr2, newdomr)) {
                            vdi->ti()->domain(new SetLit(Location().introduce(),newdom));
                            vdi->ti()->setComputedDomain(false);
                          }
                        }
                      }
                    } else {
                      // at this point, can only be a constant
                      assert((*al)[i]->type().ispar());
                      FloatVal fv = eval_float(env, (*al)[i]);
                      if (!fsv->contains(fv)) {
                        std::ostringstream oss;
                        oss << "value " << fv << " outside declared array domain " << *fsv;
                        env.fail(oss.str());
                      }
                    }
                  }
                  vd->ti()->setComputedDomain(true);
                }
              }
            } else if (Id* e_id = e->dyn_cast<Id>()) {
              if (e_id == vd->id()) {
                ret = vd->id();
              } else {
                ASTString cid;
                if (e->type().isint()) {
                  if (e->type().isopt()) {
                    cid = ASTString("int_opt_eq");
                  } else {
                    cid = constants().ids.int_.eq;
                  }
                } else if (e->type().isbool()) {
                  if (e->type().isopt()) {
                    cid = ASTString("bool_opt_eq");
                  } else {
                    cid = constants().ids.bool_eq;
                  }
                } else if (e->type().is_set()) {
                  cid = constants().ids.set_eq;
                } else if (e->type().isfloat()) {
                  cid = constants().ids.float_.eq;
                }
                if (cid != "") {
                  GCLock lock;
                  std::vector<Expression*> args(2);
                  args[0] = vd->id();
                  args[1] = e_id;
                  Call* c = new Call(Location().introduce(),cid,args);
                  c->decl(env.model->matchFn(env,c,false));
                  c->type(c->decl()->rtype(env,args,false));
                  if (c->decl()->e()) {
                    flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                    ret = vd->id();
                    vd->e(e);
                    env.vo_add_exp(vd);
                  }
                }
              }
            }
            
            if (ret != vd->id()) {
              vd->e(ret);
              addPathAnnotation(env, ret);
              env.vo_add_exp(vd);
              ret = vd->id();
            }
            Id* vde_id = Expression::dyn_cast<Id>(vd->e());
            if (vde_id && vde_id->decl()->ti()->domain()==NULL) {
              if (vd->ti()->domain()) {
                GCLock lock;
                Expression* vd_dom = eval_par(env, vd->ti()->domain());
                vde_id->decl()->ti()->domain(vd_dom);
              }
            } else if (vd->e() && vd->e()->type().bt()==Type::BT_INT && vd->e()->type().dim()==0) {
              GCLock lock;
              IntSetVal* ibv = NULL;
              if (vd->e()->type().is_set()) {
                ibv = compute_intset_bounds(env,vd->e());
              } else {
                IntBounds ib = compute_int_bounds(env,vd->e());
                if (ib.valid) {
                  Call* call = vd->e()->dyn_cast<Call>();
                  if (call && call->id()==constants().ids.lin_exp) {
                    ArrayLit* al = eval_array_lit(env, call->arg(1));
                    if (al->size()==1) {
                      IntBounds check_zeroone = compute_int_bounds(env, (*al)[0]);
                      if (check_zeroone.l==0 && check_zeroone.u==1) {
                        ArrayLit* coeffs = eval_array_lit(env, call->arg(0));
                        std::vector<IntVal> newdom(2);
                        newdom[0] = 0;
                        newdom[1] = eval_int(env, (*coeffs)[0])+eval_int(env, call->arg(2));
                        ibv = IntSetVal::a(newdom);
                      }
                    }
                  }
                  if (ibv==NULL) {
                    ibv = IntSetVal::a(ib.l,ib.u);
                  }
                }
              }
              if (ibv) {
                if (vd->ti()->domain()) {
                  IntSetVal* domain = eval_intset(env,vd->ti()->domain());
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(ibv);
                  Ranges::Inter<IntVal,IntSetRanges,IntSetRanges> i(dr,ibr);
                  IntSetVal* newibv = IntSetVal::ai(i);
                  if (ibv->card() == newibv->card()) {
                    vd->ti()->setComputedDomain(true);
                  } else {
                    ibv = newibv;
                  }
                } else {
                  vd->ti()->setComputedDomain(true);
                }
                SetLit* ibv_l = new SetLit(Location().introduce(),ibv);
                vd->ti()->domain(ibv_l);
                if (vd->type().isopt()) {
                  std::vector<Expression*> args(2);
                  args[0] = vd->id();
                  args[1] = ibv_l;
                  Call* c = new Call(Location().introduce(), "var_dom", args);
                  c->type(Type::varbool());
                  c->decl(env.model->matchFn(env, c, false));
                  (void) flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                }
              }
            } else if (vd->e() && vd->e()->type().bt()==Type::BT_FLOAT && vd->e()->type().dim()==0) {
              GCLock lock;
              FloatSetVal* fbv = NULL;
              FloatBounds fb = compute_float_bounds(env,vd->e());
              if (fb.valid) {
                fbv = FloatSetVal::a(fb.l,fb.u);
              }
              if (fbv) {
                if (vd->ti()->domain()) {
                  FloatSetVal* domain = eval_floatset(env,vd->ti()->domain());
                  FloatSetRanges dr(domain);
                  FloatSetRanges fbr(fbv);
                  Ranges::Inter<FloatVal,FloatSetRanges,FloatSetRanges> i(dr,fbr);
                  FloatSetVal* newfbv = FloatSetVal::ai(i);
                  
                  FloatSetRanges dr_eq(domain);
                  FloatSetRanges newfbv_eq(newfbv);
                  if (Ranges::equal(dr_eq, newfbv_eq)) {
                    vd->ti()->setComputedDomain(true);
                  } else {
                    fbv = newfbv;
                  }
                } else {
                  vd->ti()->setComputedDomain(true);
                }
                SetLit* fbv_l = new SetLit(Location().introduce(),fbv);
                vd->ti()->domain(fbv_l);
                if (vd->type().isopt()) {
                  std::vector<Expression*> args(2);
                  args[0] = vd->id();
                  args[1] = fbv_l;
                  Call* c = new Call(Location().introduce(), "var_dom", args);
                  c->type(Type::varbool());
                  c->decl(env.model->matchFn(env, c, false));
                  (void) flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                }
              }
            }
          }
          return ret;
        } else if (vd == e) {
          return vd->id();
        } else if (vd->e() != e) {
          e = follow_id_to_decl(e);
          if (vd == e)
            return vd->id();
          switch (e->eid()) {
          case Expression::E_BOOLLIT:
            {
              Id* id = vd->id();
              while (id != NULL) {
                if (id->decl()->ti()->domain() && eval_bool(env,id->decl()->ti()->domain()) == e->cast<BoolLit>()->v()) {
                  return constants().lit_true;
                } else if (id->decl()->ti()->domain() && eval_bool(env,id->decl()->ti()->domain()) != e->cast<BoolLit>()->v()) {
                  GCLock lock;
                  env.flat_addItem(new ConstraintI(Location().introduce(),constants().lit_false));
                } else {
                  id->decl()->ti()->domain(e);
                  GCLock lock;
                  std::vector<Expression*> args(2);
                  args[0] = id;
                  args[1] = e;
                  Call* c = new Call(Location().introduce(),constants().ids.bool_eq,args);
                  c->decl(env.model->matchFn(env,c,false));
                  c->type(c->decl()->rtype(env,args,false));
                  if (c->decl()->e()) {
                    flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                  }
                }
                id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
              }
              return constants().lit_true;
            }
          case Expression::E_VARDECL:
            {
              VarDecl* e_vd = e->cast<VarDecl>();
              if (vd->e()==e_vd->id() || e_vd->e()==vd->id())
                return vd->id();
              if (e->type().dim() != 0)
                throw InternalError("not supported yet");
              GCLock lock;
              ASTString cid;
              if (e->type().isint()) {
                cid = constants().ids.int_.eq;
              } else if (e->type().isbool()) {
                cid = constants().ids.bool_eq;
              } else if (e->type().is_set()) {
                cid = constants().ids.set_eq;
              } else if (e->type().isfloat()) {
                cid = constants().ids.float_.eq;
              } else {
                throw InternalError("not yet implemented");
              }
              std::vector<Expression*> args(2);
              args[0] = vd->id();
              args[1] = e_vd->id();
              Call* c = new Call(vd->loc().introduce(),cid,args);
              c->decl(env.model->matchFn(env,c,false));
              c->type(c->decl()->rtype(env,args,false));
              flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
              return vd->id();
            }
          case Expression::E_CALL:
            {
              Call* c = e->cast<Call>();
              GCLock lock;
              Call* nc;
              std::vector<Expression*> args;
              if (c->id() == constants().ids.lin_exp) {
                ArrayLit* le_c = follow_id(c->arg(0))->cast<ArrayLit>();
                std::vector<Expression*> ncoeff(le_c->size());
                for (unsigned int i=static_cast<unsigned int>(ncoeff.size()); i--;)
                  ncoeff[i] = (*le_c)[i];
                ncoeff.push_back(IntLit::a(-1));
                args.push_back(new ArrayLit(Location().introduce(),ncoeff));
                args[0]->type(le_c->type());
                ArrayLit* le_x = follow_id(c->arg(1))->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->size());
                for (unsigned int i=static_cast<unsigned int>(nx.size()); i--;)
                  nx[i] = (*le_x)[i];
                nx.push_back(vd->id());
                args.push_back(new ArrayLit(Location().introduce(),nx));
                args[1]->type(le_x->type());
                args.push_back(c->arg(2));
                nc = new Call(c->loc().introduce(), constants().ids.lin_exp, args);
                nc->decl(env.model->matchFn(env,nc,false));
                if (nc->decl() == NULL) {
                  throw InternalError("undeclared function or predicate "
                                      +nc->id().str());
                }
                nc->type(nc->decl()->rtype(env,args,false));
                BinOp* bop = new BinOp(nc->loc(), nc, BOT_EQ, IntLit::a(0));
                bop->type(Type::varbool());
                flat_exp(env, Ctx(), bop, constants().var_true, constants().var_true);
                return vd->id();
              } else {
                args.resize(c->n_args());
                for (unsigned int i=static_cast<unsigned int>(args.size()); i--;)
                  args[i] = c->arg(i);
                args.push_back(vd->id());
                ASTString nid = c->id();

                if (c->id() == constants().ids.exists) {
                  nid = constants().ids.array_bool_or;
                } else if (c->id() == constants().ids.forall) {
                  nid = constants().ids.array_bool_and;
                } else if (vd->type().isbool()) {
                  if (vd->ann().contains(constants().ctx.pos)) {
                    nid = env.halfReifyId(c->id());
                    if (env.model->matchFn(env, nid, args, false) == NULL) {
                      nid = env.reifyId(c->id());
                    }
                  } else {
                    nid = env.reifyId(c->id());
                  }
                }
                nc = new Call(c->loc().introduce(), nid, args);
              }
              nc->decl(env.model->matchFn(env,nc,false));
              if (nc->decl() == NULL) {
                throw InternalError("undeclared function or predicate "
                                    +nc->id().str());
              }
              nc->type(nc->decl()->rtype(env,args,false));
              makeDefinedVar(vd, nc);
              flat_exp(env, Ctx(), nc, constants().var_true, constants().var_true);
              return vd->id();
            }
            break;
          default:
            throw InternalError("not supported yet");
          }
        } else {
          return e;
        }
      }
    }
  }

  KeepAlive conj(EnvI& env,VarDecl* b,Ctx ctx,const std::vector<EE>& e) {
    if (!ctx.neg) {
      std::vector<Expression*> nontrue;
      for (unsigned int i=0; i<e.size(); i++) {
        if (istrue(env,e[i].b()))
          continue;
        if (isfalse(env,e[i].b())) {
          return bind(env,Ctx(),b,constants().lit_false);
        }
        nontrue.push_back(e[i].b());
      }
      if (nontrue.empty()) {
        return bind(env,Ctx(),b,constants().lit_true);
      } else if (nontrue.size()==1) {
        return bind(env,ctx,b,nontrue[0]);
      } else {
        if (b==constants().var_true) {
          for (unsigned int i=0; i<nontrue.size(); i++)
            bind(env,ctx,b,nontrue[i]);
          return constants().lit_true;
        } else {
          GC::lock();
          std::vector<Expression*> args;
          ArrayLit* al = new ArrayLit(Location().introduce(),nontrue);
          al->type(Type::varbool(1));
          args.push_back(al);
          Call* ret = new Call(nontrue[0]->loc().introduce(),constants().ids.forall,args);
          ret->decl(env.model->matchFn(env,ret,false));
          ret->type(ret->decl()->rtype(env,args,false));
          KeepAlive ka(ret);
          GC::unlock();
          return flat_exp(env,ctx,ret,b,constants().var_true).r;
        }
      }
    } else {
      Ctx nctx = ctx;
      nctx.neg = false;
      // negated
      std::vector<Expression*> nonfalse;
      for (unsigned int i=0; i<e.size(); i++) {
        if (istrue(env,e[i].b()))
          continue;
        if (isfalse(env,e[i].b())) {
          return bind(env,Ctx(),b,constants().lit_true);
        }
        nonfalse.push_back(e[i].b());
      }
      if (nonfalse.empty()) {
        return bind(env,Ctx(),b,constants().lit_false);
      } else if (nonfalse.size()==1) {
        GC::lock();
        UnOp* uo = new UnOp(nonfalse[0]->loc(),UOT_NOT,nonfalse[0]);
        uo->type(Type::varbool());
        KeepAlive ka(uo);
        GC::unlock();
        return flat_exp(env,nctx,uo,b,constants().var_true).r;
      } else {
        if (b==constants().var_false) {
          for (unsigned int i=0; i<nonfalse.size(); i++)
            bind(env,nctx,b,nonfalse[i]);
          return constants().lit_false;
        } else {
          GC::lock();
          std::vector<Expression*> args;
          for (unsigned int i=0; i<nonfalse.size(); i++) {
            UnOp* uo = new UnOp(nonfalse[i]->loc(),UOT_NOT,nonfalse[i]);
            uo->type(Type::varbool());
            nonfalse[i] = uo;
          }
          ArrayLit* al = new ArrayLit(Location().introduce(),nonfalse);
          al->type(Type::varbool(1));
          args.push_back(al);
          Call* ret = new Call(Location().introduce(),constants().ids.exists,args);
          ret->decl(env.model->matchFn(env, ret, false));
          ret->type(ret->decl()->rtype(env, args, false));
          assert(ret->decl());
          KeepAlive ka(ret);
          GC::unlock();
          return flat_exp(env,nctx,ret,b,constants().var_true).r;
        }
      }
      
    }
  }

  TypeInst* eval_typeinst(EnvI& env, VarDecl* vd) {
    bool hasTiVars = vd->ti()->domain() && vd->ti()->domain()->isa<TIId>();
    for (unsigned int i=0; i<vd->ti()->ranges().size(); i++) {
      hasTiVars = hasTiVars || (vd->ti()->ranges()[i]->domain() && vd->ti()->ranges()[i]->domain()->isa<TIId>());
    }
    if (hasTiVars) {
      assert(vd->e());
      if (vd->e()->type().dim()==0)
        return new TypeInst(Location().introduce(),vd->e()->type());
      ArrayLit* al = eval_array_lit(env,vd->e());
      std::vector<TypeInst*> dims(al->dims());
      for (unsigned int i=0; i<dims.size(); i++) {
        dims[i] = new TypeInst(Location().introduce(), Type(), new SetLit(Location().introduce(),IntSetVal::a(al->min(i),al->max(i))));
      }
      return new TypeInst(Location().introduce(), vd->e()->type(), dims, eval_par(env,vd->ti()->domain()));
    } else {
      std::vector<TypeInst*> dims(vd->ti()->ranges().size());
      for (unsigned int i=0; i<vd->ti()->ranges().size(); i++) {
        if (vd->ti()->ranges()[i]->domain()) {
          IntSetVal* isv = eval_intset(env,vd->ti()->ranges()[i]->domain());
          if (isv->size() > 1)
            throw EvalError(env, vd->ti()->ranges()[i]->domain()->loc(),
                            "array index set must be contiguous range");
          SetLit* sl = new SetLit(vd->ti()->ranges()[i]->loc(),isv);
          sl->type(Type::parsetint());
          dims[i] = new TypeInst(vd->ti()->ranges()[i]->loc(), Type(),sl);
        } else {
          dims[i] = new TypeInst(vd->ti()->ranges()[i]->loc(), Type(), NULL);
        }
      }
      Type t = (vd->e() && !vd->e()->type().isbot()) ? vd->e()->type() : vd->ti()->type();
      return new TypeInst(vd->ti()->loc(), t, dims, eval_par(env,vd->ti()->domain()));
    }
  }
  
  bool isBuiltin(FunctionI* decl) {
    return (decl->loc().filename() == "builtins.mzn" ||
            decl->loc().filename().endsWith("/builtins.mzn") ||
            decl->loc().filename() == "stdlib.mzn" ||
            decl->loc().filename().endsWith("/stdlib.mzn") ||
            decl->loc().filename() == "flatzinc_builtins.mzn" ||
            decl->loc().filename().endsWith("/flatzinc_builtins.mzn"));
  }
  
  KeepAlive flat_cv_exp(EnvI& env, Ctx ctx, Expression* e) {
    GCLock lock;
    if (e->type().ispar() && !e->type().cv()) {
      return eval_par(env, e);
    }
    if (e->type().isvar()) {
      EE ee = flat_exp(env, ctx, e, NULL,NULL);
      if (isfalse(env, ee.b()))
        throw FlatteningError(env, e->loc(), "cannot flatten partial function in this position");
      return ee.r();
    }
    switch (e->eid()) {
      case Expression::E_INTLIT:
      case Expression::E_FLOATLIT:
      case Expression::E_BOOLLIT:
      case Expression::E_STRINGLIT:
      case Expression::E_TIID:
      case Expression::E_VARDECL:
      case Expression::E_TI:
      case Expression::E_ANON:
        assert(false);
        return NULL;
      case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        return flat_cv_exp(env, ctx, id->decl()->e());
      }
      case Expression::E_SETLIT:
      {
        SetLit* sl = e->cast<SetLit>();
        if (sl->isv() || sl->fsv())
          return sl;
        std::vector<Expression*> es(sl->v().size());
        GCLock lock;
        for (unsigned int i=0; i<sl->v().size(); i++) {
          es[i] = flat_cv_exp(env, ctx, sl->v()[i])();
        }
        SetLit* sl_ret = new SetLit(Location().introduce(),es);
        Type t = sl->type();
        t.cv(false);
        sl_ret->type(t);
        return eval_par(env, sl_ret);
      }
      case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        std::vector<Expression*> es(al->size());
        GCLock lock;
        for (unsigned int i=0; i<al->size(); i++) {
          es[i] = flat_cv_exp(env, ctx, (*al)[i])();
        }
        std::vector<std::pair<int,int> > dims(al->dims());
        for (int i=0; i<al->dims(); i++) {
          dims[i] = std::make_pair(al->min(i), al->max(i));
        }
        Expression* al_ret =  eval_par(env, new ArrayLit(Location().introduce(),es,dims));
        Type t = al->type();
        t.cv(false);
        al_ret->type(t);
        return al_ret;
      }
      case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        GCLock lock;
        Expression* av = flat_cv_exp(env, ctx, aa->v())();
        std::vector<Expression*> idx(aa->idx().size());
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          idx[i] = flat_cv_exp(env, ctx, aa->idx()[i])();
        }
        ArrayAccess* aa_ret = new ArrayAccess(Location().introduce(),av,idx);
        Type t = aa->type();
        t.cv(false);
        aa_ret->type(t);
        return eval_par(env, aa_ret);
      }
      case Expression::E_COMP:
      {
        Comprehension* c = e->cast<Comprehension>();
        GCLock lock;
        class EvalFlatCvExp {
        public:
          Ctx ctx;
          EvalFlatCvExp(Ctx& ctx0) : ctx(ctx0) {}
          typedef Expression* ArrayVal;
          Expression* e(EnvI& env, Expression* e) {
            return flat_cv_exp(env,ctx,e)();
          }
          static Expression* exp(Expression* e) { return e; }
        } eval(ctx);
        std::vector<Expression*> a = eval_comp<EvalFlatCvExp>(env,eval,c);

        Type t = Type::bot();
        bool allPar = true;
        for (unsigned int i=0; i<a.size(); i++) {
          if (t==Type::bot())
            t = a[i]->type();
          if (!a[i]->type().ispar())
            allPar = false;
        }
        if (!allPar)
          t.ti(Type::TI_VAR);
        if (c->set())
          t.st(Type::ST_SET);
        else
          t.dim(c->type().dim());
        t.cv(false);
        if (c->set()) {
          if (c->type().ispar() && allPar) {
            SetLit* sl = new SetLit(c->loc().introduce(), a);
            sl->type(t);
            Expression* slr = eval_par(env,sl);
            slr->type(t);
            return slr;
          } else {
            throw InternalError("var set comprehensions not supported yet");
          }
        } else {
          ArrayLit* alr = new ArrayLit(Location().introduce(),a);
          alr->type(t);
          alr->flat(true);
          return alr;
        }
      }
      case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        for (int i=0; i<ite->size(); i++) {
          KeepAlive ka = flat_cv_exp(env,ctx,ite->e_if(i));
          if (eval_bool(env,ka()))
            return flat_cv_exp(env,ctx,ite->e_then(i));
        }
        return flat_cv_exp(env,ctx,ite->e_else());
      }
      case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->op() == BOT_AND) {
          GCLock lock;
          Expression* lhs = flat_cv_exp(env, ctx, bo->lhs())();
          if (!eval_bool(env, lhs)) {
            return constants().lit_false;
          }
          return eval_par(env, flat_cv_exp(env, ctx, bo->rhs())());
        } else if (bo->op() == BOT_OR) {
          GCLock lock;
          Expression* lhs = flat_cv_exp(env, ctx, bo->lhs())();
          if (eval_bool(env, lhs)) {
            return constants().lit_true;
          }
          return eval_par(env, flat_cv_exp(env, ctx, bo->rhs())());
        }
        GCLock lock;
        BinOp* nbo = new BinOp(bo->loc().introduce(),flat_cv_exp(env, ctx, bo->lhs())(),bo->op(),flat_cv_exp(env, ctx, bo->rhs())());
        nbo->type(bo->type());
        return eval_par(env, nbo);
      }
      case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        GCLock lock;
        UnOp* nuo = new UnOp(uo->loc(), uo->op(), flat_cv_exp(env, ctx, uo->e())());
        nuo->type(uo->type());
        return eval_par(env, nuo);
      }
      case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        if (c->id()=="mzn_in_root_context") {
          return constants().boollit(ctx.b==C_ROOT);
        }
        if (ctx.b==C_ROOT && c->decl()->e() && c->decl()->e()->isa<BoolLit>()) {
          bool allBool = true;
          for (unsigned int i=0; i<c->n_args(); i++) {
            if (c->arg(i)->type().bt()!=Type::BT_BOOL) {
              allBool = false;
              break;
            }
          }
          if (allBool) {
            return c->decl()->e();
          }
        }
        std::vector<Expression*> args(c->n_args());
        GCLock lock;
        for (unsigned int i=0; i<c->n_args(); i++) {
          args[i] = flat_cv_exp(env, ctx, c->arg(i))();
        }
        Call* nc = new Call(c->loc(), c->id(), args);
        nc->decl(c->decl());
        nc->type(c->type());
        return eval_par(env, nc);
      }
      case Expression::E_LET:
      {
        Let* l = e->cast<Let>();
        l->pushbindings();
        KeepAlive ret = flat_cv_exp(env, ctx, l->in());
        l->popbindings();
        return ret;
      }
        
    }
    throw InternalError("internal error");    
  }
  
  void flatten(Env& e, FlatteningOptions opt) {
    try {
      EnvI& env = e.envi();
      env.fopts = opt;

      bool onlyRangeDomains = false;
      if ( opt.onlyRangeDomains ) {
        onlyRangeDomains = true;           // compulsory
      }
      else
      {
        GCLock lock;
        Call* check_only_range =
          new Call(Location(),"mzn_check_only_range_domains", std::vector<Expression*>());
        check_only_range->type(Type::parbool());
        check_only_range->decl(env.model->matchFn(e.envi(), check_only_range, false));
        onlyRangeDomains = eval_bool(e.envi(), check_only_range);
      }
    
      bool hadSolveItem = false;
      // Flatten main model
      class FV : public ItemVisitor {
      public:
        EnvI& env;
        bool& hadSolveItem;
        FV(EnvI& env0, bool& hadSolveItem0) : env(env0), hadSolveItem(hadSolveItem0) {}
        bool enter(Item* i) {
            return !(i->isa<ConstraintI>()  && env.failed());
        }
        void vVarDeclI(VarDeclI* v) {
          v->e()->ann().remove(constants().ann.output_var);
          v->e()->ann().removeCall(constants().ann.output_array);
          if (v->e()->ann().contains(constants().ann.output_only))
            return;
          if (v->e()->type().ispar() && !v->e()->type().isopt() && v->e()->type().dim() > 0 && v->e()->ti()->domain()==NULL
              && (v->e()->type().bt()==Type::BT_INT || v->e()->type().bt()==Type::BT_FLOAT)) {
            // Compute bounds for array literals
            GCLock lock;
            ArrayLit* al = eval_array_lit(env, v->e()->e());
            if (v->e()->type().bt()==Type::BT_INT && v->e()->type().st()==Type::ST_PLAIN) {
              IntVal lb = IntVal::infinity();
              IntVal ub = -IntVal::infinity();
              for (unsigned int i=0; i<al->size(); i++) {
                IntVal vi = eval_int(env, (*al)[i]);
                lb = std::min(lb, vi);
                ub = std::max(ub, vi);
              }
              GCLock lock;
              //v->e()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(lb, ub)));
              //v->e()->ti()->setComputedDomain(true);
              setComputedDomain(env, v->e(), new SetLit(Location().introduce(), IntSetVal::a(lb, ub)), true);
            } else if (v->e()->type().bt()==Type::BT_FLOAT && v->e()->type().st()==Type::ST_PLAIN) {
              FloatVal lb = FloatVal::infinity();
              FloatVal ub = -FloatVal::infinity();
              for (unsigned int i=0; i<al->size(); i++) {
                FloatVal vi = eval_float(env, (*al)[i]);
                lb = std::min(lb, vi);
                ub = std::max(ub, vi);
              }
              GCLock lock;
              //v->e()->ti()->domain(new SetLit(Location().introduce(), FloatSetVal::a(lb, ub)));
              //v->e()->ti()->setComputedDomain(true);
              setComputedDomain(env, v->e(), new SetLit(Location().introduce(), FloatSetVal::a(lb, ub)), true);
            }
          }
          if (v->e()->type().isvar() || v->e()->type().isann()) {
            (void) flat_exp(env,Ctx(),v->e()->id(),NULL,constants().var_true);
          } else {
            if (v->e()->e()==NULL) {
              if (!v->e()->type().isann())
                throw EvalError(env, v->e()->loc(), "Undefined parameter", v->e()->id()->v());
            } else {
              CallStackItem csi(env,v->e());
              GCLock lock;
              Location v_loc = v->e()->e()->loc();
              if (!v->e()->e()->type().cv()) {
                v->e()->e(eval_par(env,v->e()->e()));
              } else {
                EE ee = flat_exp(env, Ctx(), v->e()->e(), NULL, constants().var_true);
                v->e()->e(ee.r());
              }
              if (v->e()->type().dim() > 0) {
                checkIndexSets(env,v->e(), v->e()->e());
                if (v->e()->ti()->domain() != NULL) {
                  ArrayLit* al = eval_array_lit(env,v->e()->e());
                  for (unsigned int i=0; i<al->size(); i++) {
                    if (!checkParDomain(env,(*al)[i], v->e()->ti()->domain())) {
                      throw EvalError(env, v_loc, "parameter value out of range");
                    }
                  }
                }
              } else {
                if (v->e()->ti()->domain() != NULL) {
                  if (!checkParDomain(env,v->e()->e(), v->e()->ti()->domain())) {
                    throw EvalError(env, v_loc, "parameter value out of range");
                  }
                }
              }
            }
          }
        }
        void vConstraintI(ConstraintI* ci) {
          (void) flat_exp(env,Ctx(),ci->e(),constants().var_true,constants().var_true);
        }
        void vSolveI(SolveI* si) {
          if (hadSolveItem)
            throw FlatteningError(env,si->loc(), "Only one solve item allowed");
          hadSolveItem = true;
          GCLock lock;
          SolveI* nsi = NULL;
          switch (si->st()) {
          case SolveI::ST_SAT:
            nsi = SolveI::sat(Location());
            break;
          case SolveI::ST_MIN:
            {
              Ctx ctx;
              ctx.i = C_NEG;
              nsi = SolveI::min(Location().introduce(),flat_exp(env,ctx,si->e(),NULL,constants().var_true).r());
            }
            break;
          case SolveI::ST_MAX:
            {
              Ctx ctx;
              ctx.i = C_POS;
              nsi = SolveI::max(Location().introduce(),flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r());
            }
            break;
          }
          for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
            nsi->ann().add(flat_exp(env,Ctx(),*it,NULL,constants().var_true).r());
          }
          env.flat_addItem(nsi);
        }
      } _fv(env,hadSolveItem);
      iterItems<FV>(_fv,e.model());
    
      if (!hadSolveItem) {
        GCLock lock;
        e.envi().flat_addItem(SolveI::sat(Location().introduce()));
      }

      std::vector<VarDecl*> deletedVarDecls;

      // Create output model
      if (opt.keepOutputInFzn) {
        copyOutput(env);
      } else {
        createOutput(env, deletedVarDecls, opt.outputMode, opt.outputObjective);
      }
      
      // Flatten remaining redefinitions
      Model& m = *e.flat();
      int startItem = 0;
      int endItem = m.size()-1;
      
      FunctionI* int_lin_eq;
      {
        std::vector<Type> int_lin_eq_t(3);
        int_lin_eq_t[0] = Type::parint(1);
        int_lin_eq_t[1] = Type::varint(1);
        int_lin_eq_t[2] = Type::parint(0);
        GCLock lock;
        FunctionI* fi = env.model->matchFn(env, constants().ids.int_.lin_eq, int_lin_eq_t, false);
        int_lin_eq = (fi && fi->e()) ? fi : NULL;
      }
      FunctionI* array_bool_and;
      FunctionI* array_bool_or;
      FunctionI* array_bool_clause;
      FunctionI* array_bool_clause_reif;
      FunctionI* bool_xor;
      {
        std::vector<Type> array_bool_andor_t(2);
        array_bool_andor_t[0] = Type::varbool(1);
        array_bool_andor_t[1] = Type::varbool(0);
        GCLock lock;
        FunctionI* fi = env.model->matchFn(env, ASTString("array_bool_and"), array_bool_andor_t, false);
        array_bool_and = (fi && fi->e()) ? fi : NULL;
        fi = env.model->matchFn(env, ASTString("array_bool_or"), array_bool_andor_t, false);
        array_bool_or = (fi && fi->e()) ? fi : NULL;

        array_bool_andor_t[1] = Type::varbool(1);
        fi = env.model->matchFn(env, ASTString("bool_clause"), array_bool_andor_t, false);
        array_bool_clause = (fi && fi->e()) ? fi : NULL;

        array_bool_andor_t.push_back(Type::varbool());
        fi = env.model->matchFn(env, ASTString("bool_clause_reif"), array_bool_andor_t, false);
        array_bool_clause_reif = (fi && fi->e()) ? fi : NULL;
        
        std::vector<Type> bool_xor_t(3);
        bool_xor_t[0] = Type::varbool();
        bool_xor_t[1] = Type::varbool();
        bool_xor_t[2] = Type::varbool();
        fi = env.model->matchFn(env, constants().ids.bool_xor, bool_xor_t, false);
        bool_xor = (fi && fi->e()) ? fi : NULL;
      }
      
      std::vector<VarDeclI*> removedItems;
      env.collectVarDecls(true);

      while (startItem <= endItem || !env.modifiedVarDecls.empty()) {
          if (env.failed())
          return;
        std::vector<int> agenda;
        for (int i=startItem; i<=endItem; i++) {
          agenda.push_back(i);
        }
        for (unsigned int i=0; i<env.modifiedVarDecls.size(); i++) {
          agenda.push_back(env.modifiedVarDecls[i]);
        }
        env.modifiedVarDecls.clear();
        
        for (int ai=0; ai<agenda.size(); ai++) {
          int i=agenda[ai];
          VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>();
          bool keptVariable = true;
          if (vdi!=NULL && !isOutput(vdi->e()) && env.vo.occurrences(vdi->e())==0 ) {
            if (vdi->e()->e() && vdi->e()->ti()->domain()) {
              if (vdi->e()->type().isvar() && vdi->e()->type().isbool() &&
                  !vdi->e()->type().isopt() &&
                  Expression::equal(vdi->e()->ti()->domain(),constants().lit_true)) {
                GCLock lock;
                ConstraintI* ci = new ConstraintI(vdi->loc(),vdi->e()->e());
                if (vdi->e()->introduced()) {
                  removedItems.push_back(vdi);
                  env.flat_removeItem(vdi);
                  keptVariable = false;
                } else {
                  vdi->e()->e(NULL);
                }
                env.flat_addItem(ci);
              } else if (vdi->e()->type().ispar() || vdi->e()->ti()->computedDomain()) {
                removedItems.push_back(vdi);
                keptVariable = false;
              }
            } else {
              removedItems.push_back(vdi);
              env.flat_removeItem(vdi);
              keptVariable = false;
            }
          }
          if (vdi && keptVariable && vdi->e()->type().dim() > 0 && vdi->e()->type().isvar()) {
            vdi->e()->ti()->domain(NULL);
          }
          if (vdi && keptVariable &&
              vdi->e()->type().isint() && vdi->e()->type().isvar() &&
              vdi->e()->ti()->domain() != NULL) {

            GCLock lock;
            IntSetVal* dom = eval_intset(env,vdi->e()->ti()->domain());

            bool needRangeDomain = onlyRangeDomains;
            if (!needRangeDomain && dom->size() > 0) {
              if (dom->min(0).isMinusInfinity() || dom->max(dom->size()-1).isPlusInfinity())
                needRangeDomain = true;
            }
            if (needRangeDomain) {
              if (dom->min(0).isMinusInfinity() || dom->max(dom->size()-1).isPlusInfinity()) {
                TypeInst* nti = copy(env,vdi->e()->ti())->cast<TypeInst>();
                nti->domain(NULL);
                vdi->e()->ti(nti);
                if (dom->min(0).isFinite()) {
                  std::vector<Expression*> args(2);
                  args[0] = IntLit::a(dom->min(0));
                  args[1] = vdi->e()->id();
                  Call* call = new Call(Location().introduce(),constants().ids.int_.le,args);
                  call->type(Type::varbool());
                  call->decl(env.model->matchFn(env, call, false));
                  env.flat_addItem(new ConstraintI(Location().introduce(), call));
                } else if (dom->max(dom->size()-1).isFinite()) {
                  std::vector<Expression*> args(2);
                  args[0] = vdi->e()->id();
                  args[1] = IntLit::a(dom->max(dom->size()-1));
                  Call* call = new Call(Location().introduce(),constants().ids.int_.le,args);
                  call->type(Type::varbool());
                  call->decl(env.model->matchFn(env, call, false));
                  env.flat_addItem(new ConstraintI(Location().introduce(), call));
                }
              } else if (dom->size() > 1) {
                SetLit* newDom = new SetLit(Location().introduce(),IntSetVal::a(dom->min(0),dom->max(dom->size()-1)));
                  TypeInst* nti = copy(env,vdi->e()->ti())->cast<TypeInst>();
                  nti->domain(newDom);
                  vdi->e()->ti(nti); /// TODO: WHY WAS THIS COMMENTED OUT IN DEBUG???
              }
              if (dom->size() > 1) {
                IntVal firstHole = dom->max(0)+1;
                IntSetRanges domr(dom);
                ++domr;
                for (; domr(); ++domr) {
                  for (IntVal i=firstHole; i<domr.min(); i++) {
                    std::vector<Expression*> args(2);
                    args[0] = vdi->e()->id();
                    args[1] = IntLit::a(i);
                    Call* call = new Call(vdi->e()->loc(),constants().ids.int_.ne,args);
                    call->type(Type::varbool());
                    call->decl(env.model->matchFn(env, call, false));
                    // Give distinct call stacks for each int_ne added
                    CallStackItem csi(env, IntLit::a(i));
                    env.flat_addItem(new ConstraintI(Location().introduce(), call));
                    firstHole = domr.max().plus(1);
                  }
                }
              }
            }
          }
          if (vdi && keptVariable &&
              vdi->e()->type().isfloat() && vdi->e()->type().isvar() &&
              vdi->e()->ti()->domain() != NULL) {
            GCLock lock;
            FloatSetVal* vdi_dom = eval_floatset(env, vdi->e()->ti()->domain());
            FloatVal vmin = vdi_dom->min();
            FloatVal vmax = vdi_dom->max();
            if (vmin == -FloatVal::infinity() && vmax == FloatVal::infinity()) {
              vdi->e()->ti()->domain(NULL);
            } else if (vmin == -FloatVal::infinity()) {
              vdi->e()->ti()->domain(NULL);
              std::vector<Expression*> args(2);
              args[0] = vdi->e()->id();
              args[1] = FloatLit::a(vmax);
              Call* call = new Call(Location().introduce(),constants().ids.float_.le,args);
              call->type(Type::varbool());
              call->decl(env.model->matchFn(env, call, false));
              env.flat_addItem(new ConstraintI(Location().introduce(), call));
            } else if (vmax == FloatVal::infinity()) {
              vdi->e()->ti()->domain(NULL);
              std::vector<Expression*> args(2);
              args[0] = FloatLit::a(vmin);
              args[1] = vdi->e()->id();
              Call* call = new Call(Location().introduce(),constants().ids.float_.le,args);
              call->type(Type::varbool());
              call->decl(env.model->matchFn(env, call, false));
              env.flat_addItem(new ConstraintI(Location().introduce(), call));
            } else if (vdi_dom->size() > 1) {
              BinOp* dom_ranges = new BinOp(vdi->e()->ti()->domain()->loc().introduce(),
                                            FloatLit::a(vmin), BOT_DOTDOT, FloatLit::a(vmax));
              vdi->e()->ti()->domain(dom_ranges);
              
              std::vector<Expression*> ranges;
              for (FloatSetRanges vdi_r(vdi_dom); vdi_r(); ++vdi_r) {
                ranges.push_back(FloatLit::a(vdi_r.min()));
                ranges.push_back(FloatLit::a(vdi_r.max()));
              }
              ArrayLit* al = new ArrayLit(Location().introduce(), ranges);
              al->type(Type::parfloat(1));
              std::vector<Expression*> args(2);
              args[0] = vdi->e()->id();
              args[1] = al;
              Call* call = new Call(Location().introduce(),constants().ids.float_.dom,args);
              call->type(Type::varbool());
              call->decl(env.model->matchFn(env, call, false));
              env.flat_addItem(new ConstraintI(Location().introduce(), call));
            }
          }
        }
        
        // rewrite some constraints if there are redefinitions
        for (int ai=0; ai<agenda.size(); ai++) {
          int i=agenda[ai];
          if (VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>()) {
            VarDecl* vd = vdi->e();
            if (!vdi->removed() && vd->e()) {
              bool isTrueVar = vd->type().isbool() && Expression::equal(vd->ti()->domain(), constants().lit_true);
              if (Call* c = vd->e()->dyn_cast<Call>()) {
                GCLock lock;
                Call* nc = NULL;
                if (c->id() == constants().ids.lin_exp) {
                  if (int_lin_eq) {
                    std::vector<Expression*> args(c->n_args());
                    ArrayLit* le_c = follow_id(c->arg(0))->cast<ArrayLit>();
                    std::vector<Expression*> nc_c(le_c->size());
                    for (unsigned int i=static_cast<unsigned int>(nc_c.size()); i--;)
                      nc_c[i] = (*le_c)[i];
                    nc_c.push_back(IntLit::a(-1));
                    args[0] = new ArrayLit(Location().introduce(),nc_c);
                    args[0]->type(Type::parint(1));
                    ArrayLit* le_x = follow_id(c->arg(1))->cast<ArrayLit>();
                    std::vector<Expression*> nx(le_x->size());
                    for (unsigned int i=static_cast<unsigned int>(nx.size()); i--;)
                      nx[i] = (*le_x)[i];
                    nx.push_back(vd->id());
                    args[1] = new ArrayLit(Location().introduce(),nx);
                    args[1]->type(Type::varint(1));
                    IntVal d = c->arg(2)->cast<IntLit>()->v();
                    args[2] = IntLit::a(-d);
                    args[2]->type(Type::parint(0));
                    nc = new Call(c->loc().introduce(),ASTString("int_lin_eq"),args);
                    nc->type(Type::varbool());
                    nc->decl(int_lin_eq);
                  }
                } else if (c->id() == constants().ids.exists) {
                  if (array_bool_or) {
                    std::vector<Expression*> args(2);
                    args[0] = c->arg(0);
                    args[1] = vd->id();
                    nc = new Call(c->loc().introduce(),array_bool_or->id(),args);
                    nc->type(Type::varbool());
                    nc->decl(array_bool_or);
                  }
                } else if (!isTrueVar && c->id() == constants().ids.forall) {
                  if (array_bool_and) {
                    std::vector<Expression*> args(2);
                    args[0] = c->arg(0);
                    args[1] = vd->id();
                    nc = new Call(c->loc().introduce(),array_bool_and->id(),args);
                    nc->type(Type::varbool());
                    nc->decl(array_bool_and);
                  }
                } else if (isTrueVar && c->id() == constants().ids.clause && array_bool_clause) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = c->arg(1);
                  nc = new Call(c->loc().introduce(),array_bool_clause->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause);
                } else if (c->id() == constants().ids.clause && array_bool_clause_reif) {
                  std::vector<Expression*> args(3);
                  args[0] = c->arg(0);
                  args[1] = c->arg(1);
                  args[2] = vd->id();
                  nc = new Call(c->loc().introduce(),array_bool_clause_reif->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause_reif);
                } else {
                  if (isTrueVar) {
                    FunctionI* decl = env.model->matchFn(env,c,false);
                    env.cse_map_remove(c);
                    if (decl->e() || c->id() == constants().ids.forall) {
                      if (decl->e())
                        addPathAnnotation(env, decl->e());
                      c->decl(decl);
                      nc = c;
                    }
                  } else {
                    std::vector<Expression*> args(c->n_args());
                    for (unsigned int i=static_cast<unsigned int>(args.size()); i--;)
                      args[i] = c->arg(i);
                    args.push_back(vd->id());
                    ASTString cid = c->id();
                    if (cid == constants().ids.clause && array_bool_clause_reif) {
                      nc = new Call(c->loc().introduce(),array_bool_clause_reif->id(),args);
                      nc->type(Type::varbool());
                      nc->decl(array_bool_clause_reif);
                    } else {
                      if (c->type().isbool() && vd->type().isbool()) {
                        if (vd->ann().contains(constants().ctx.pos)) {
                          cid = env.halfReifyId(c->id());
                          if (env.model->matchFn(env, cid, args, false) == NULL) {
                            cid = env.reifyId(c->id());
                          }
                        } else {
                          cid = env.reifyId(c->id());
                        }
                      }
                      FunctionI* decl = env.model->matchFn(env,cid,args,false);
                      if (decl && decl->e()) {
                        addPathAnnotation(env, decl->e());
                        nc = new Call(c->loc().introduce(),cid,args);
                        nc->type(Type::varbool());
                        nc->decl(decl);
                      }
                    }
                  }
                }
                if (nc != NULL) {
                  CollectDecls cd(env.vo,deletedVarDecls,vdi);
                  topDown(cd,c);
                  vd->e(NULL);
                  // Need to remove right hand side from CSE map, otherwise
                  // flattening of nc could assume c has already been flattened
                  // to vd
                  env.cse_map_remove(c);
                  /// TODO: check if removing variables here makes sense:
//                  if (!isOutput(vd) && env.vo.occurrences(vd)==0) {
//                    removedItems.push_back(vdi);
//                  }
                  if (nc != c) {
                    makeDefinedVar(vd, nc);
                  }
                  StringLit* vsl = getLongestMznPathAnnotation(env, vdi->e());
                  StringLit* csl = getLongestMznPathAnnotation(env, c);
                  CallStackItem* vsi=NULL;
                  CallStackItem* csi=NULL;
                  if(vsl) vsi = new CallStackItem(env, vsl);
                  if(csl) csi = new CallStackItem(env, csl);
                  (void) flat_exp(env, Ctx(), nc, constants().var_true, constants().var_true);
                  if(csi) delete csi;
                  if(vsi) delete vsi;
                }
              }
            }
          } else if (ConstraintI* ci = m[i]->dyn_cast<ConstraintI>()) {
            if (Call* c = ci->e()->dyn_cast<Call>()) {
              GCLock lock;
              Call* nc = NULL;
              if (c->id() == constants().ids.exists) {
                if (array_bool_or) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = constants().lit_true;
                  nc = new Call(c->loc().introduce(),array_bool_or->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_or);
                }
              } else if (c->id() == constants().ids.forall) {
                if (array_bool_and) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = constants().lit_true;
                  nc = new Call(c->loc().introduce(),array_bool_and->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_and);
                }
              } else if (c->id() == constants().ids.clause) {
                if (array_bool_clause) {
                  std::vector<Expression*> args(2);
                  args[0] = c->arg(0);
                  args[1] = c->arg(1);
                  nc = new Call(c->loc().introduce(),array_bool_clause->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause);
                }
              } else if (c->id() == constants().ids.bool_xor) {
                if (bool_xor) {
                  std::vector<Expression*> args(3);
                  args[0] = c->arg(0);
                  args[1] = c->arg(1);
                  args[2] = c->n_args()==2 ? constants().lit_true : c->arg(2);
                  nc = new Call(c->loc().introduce(),bool_xor->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(bool_xor);
                }
              } else {
                FunctionI* decl = env.model->matchFn(env,c,false);
                if (decl && decl->e()) {
                  nc = c;
                  nc->decl(decl);
                }
              }
              if (nc != NULL) {
                CollectDecls cd(env.vo,deletedVarDecls,ci);
                topDown(cd,c);
                ci->e(constants().lit_true);
                env.flat_removeItem(i);
                StringLit* sl = getLongestMznPathAnnotation(env, c);
                CallStackItem* csi=NULL;
                if(sl)
                  csi = new CallStackItem(env, sl);
                (void) flat_exp(env, Ctx(), nc, constants().var_true, constants().var_true);
                if(csi) delete csi;
              }
            }
            
          }
        }

        startItem = endItem+1;
        endItem = m.size()-1;
      }

      for (unsigned int i=0; i<removedItems.size(); i++) {
        if (env.vo.occurrences(removedItems[i]->e())==0) {
          CollectDecls cd(env.vo,deletedVarDecls,removedItems[i]);
          topDown(cd,removedItems[i]->e()->e());
          env.flat_removeItem(removedItems[i]);
        }
      }
      
      // Add redefinitions for output variables that may have been redefined since createOutput
      for (unsigned int i=0; i<env.output->size(); i++) {
        if (VarDeclI* vdi = (*env.output)[i]->dyn_cast<VarDeclI>()) {
          IdMap<KeepAlive>::iterator it;
          if (vdi->e()->e()==NULL &&
              (it = env.reverseMappers.find(vdi->e()->id())) != env.reverseMappers.end()) {
            GCLock lock;
            Call* rhs = copy(env,env.cmap,it->second())->cast<Call>();
            std::vector<Type> tv(rhs->n_args());
            for (unsigned int i=rhs->n_args(); i--;) {
              tv[i] = rhs->arg(i)->type();
              tv[i].ti(Type::TI_PAR);
            }
            FunctionI* decl = env.output->matchFn(env, rhs->id(), tv, false);
            Type t;
            if (decl==NULL) {
              FunctionI* origdecl = env.model->matchFn(env, rhs->id(), tv, false);
              if (origdecl == NULL) {
                throw FlatteningError(env,rhs->loc(),"function "+rhs->id().str()+" is used in output, par version needed");
              }
                if (!isBuiltin(origdecl)) {
                decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
                CollectOccurrencesE ce(env.output_vo,decl);
                topDown(ce, decl->e());
                topDown(ce, decl->ti());
                for (unsigned int i = decl->params().size(); i--;)
                  topDown(ce, decl->params()[i]);
                env.output->registerFn(env, decl);
                env.output->addItem(decl);
              } else {
                decl = origdecl;
              }
            }
            rhs->decl(decl);
            outputVarDecls(env,vdi,rhs);
            
            removeIsOutput(vdi->e()->flat());
            vdi->e()->e(rhs);
          }
        }
      }

      for (unsigned int i=0; i<m.size(); i++) {
        if (ConstraintI* ci = m[i]->dyn_cast<ConstraintI>()) {
          if (Call* c = ci->e()->dyn_cast<Call>()) {
            if (c->decl()==constants().var_redef) {
              CollectDecls cd(env.vo,deletedVarDecls,ci);
              topDown(cd,c);
              env.flat_removeItem(i);
            }
          }
        }
      }
      
      while (!deletedVarDecls.empty()) {
        VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
        if (env.vo.occurrences(cur) == 0 && !isOutput(cur)) {
          if (CollectDecls::varIsFree(cur)) {
            IdMap<int>::iterator cur_idx = env.vo.idx.find(cur->id());
            if (cur_idx != env.vo.idx.end() && !m[cur_idx->second]->removed()) {
              CollectDecls cd(env.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
              topDown(cd,cur->e());
              env.flat_removeItem(cur_idx->second);
            }
          }
        }
      }

      if (!opt.keepOutputInFzn) {
        finaliseOutput(env, deletedVarDecls);
      }

      while (!deletedVarDecls.empty()) {
        VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
        if (env.vo.occurrences(cur) == 0 && !isOutput(cur)) {
          if (CollectDecls::varIsFree(cur)) {
            IdMap<int>::iterator cur_idx = env.vo.idx.find(cur->id());
            if (cur_idx != env.vo.idx.end() && !m[cur_idx->second]->removed()) {
              CollectDecls cd(env.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
              topDown(cd,cur->e());
              env.flat_removeItem(cur_idx->second);
            }
          }
        }
      }

      cleanupOutput(env);
    } catch (ModelInconsistent&) {
      
    }
  }
  
  void clearInternalAnnotations(Expression* e) {
    e->ann().remove(constants().ann.promise_total);
    e->ann().remove(constants().ann.maybe_partial);
    e->ann().remove(constants().ann.add_to_output);
    e->ann().remove(constants().ann.rhs_from_assignment);
    // Remove defines_var(x) annotation where x is par
    std::vector<Expression*> removeAnns;
    for (ExpressionSetIter anns = e->ann().begin(); anns != e->ann().end(); ++anns) {
      if (Call* c = (*anns)->dyn_cast<Call>()) {
        if (c->id() == constants().ann.defines_var && c->arg(0)->type().ispar()) {
          removeAnns.push_back(c);
        }
      }
    }
    for (unsigned int i=0; i<removeAnns.size(); i++) {
      e->ann().remove(removeAnns[i]);
    }
  }
  
  std::vector<Expression*> cleanup_vardecl(EnvI& env, VarDeclI* vdi, VarDecl* vd) {
    std::vector<Expression*> added_constraints;
    
    // In FlatZinc par variables have RHSs, not domains
    if (vd->type().ispar()) {
      vd->ann().clear();
      vd->introduced(false);
      vd->ti()->domain(NULL);
    }
    
    // In FlatZinc the RHS of a VarDecl must be a literal, Id or empty
    // Example:
    //   var 1..5: x = function(y)
    // becomes:
    //   var 1..5: x;
    //   relation(x, y);
    if (vd->type().isvar() && vd->type().isbool()) {
      bool is_fixed = ( vd->ti()->domain() != NULL );
      if (Expression::equal(vd->ti()->domain(),constants().lit_true)) {
        // Ex: var true: b = e()
        
        // Store RHS
        Expression* ve = vd->e();
        vd->e(constants().lit_true);
        vd->ti()->domain(NULL);
        // Ex: var bool: b = true
        
        // If vd had a RHS
        if (ve != NULL) {
          if (Call* vcc = ve->dyn_cast<Call>()) {
            // Convert functions to relations:
            //   exists([x]) => array_bool_or([x],true)
            //   forall([x]) => array_bool_and([x],true)
            //   clause([x]) => bool_clause([x])
            ASTString cid;
            std::vector<Expression*> args;
            if (vcc->id() == constants().ids.exists) {
              cid = constants().ids.array_bool_or;
              args.push_back(vcc->arg(0));
              args.push_back(constants().lit_true);
            } else if (vcc->id() == constants().ids.forall) {
              cid = constants().ids.array_bool_and;
              args.push_back(vcc->arg(0));
              args.push_back(constants().lit_true);
            } else if (vcc->id() == constants().ids.clause) {
              cid = constants().ids.bool_clause;
              args.push_back(vcc->arg(0));
              args.push_back(vcc->arg(1));
            }
            
            if (args.size()==0) {
              // Post original RHS as stand alone constraint
              ve = vcc;
            } else {
              // Create new call, retain annotations from original RHS
              Call* nc = new Call(vcc->loc().introduce(),cid,args);
              nc->type(vcc->type());
              nc->ann().merge(vcc->ann());
              ve = nc;
            }
          } else if (Id* id = ve->dyn_cast<Id>()) {
            if (id->decl()->ti()->domain() != constants().lit_true) {
              // Inconsistent assignment: post bool_eq(y, true)
              std::vector<Expression*> args(2);
              args[0] = id;
              args[1] = constants().lit_true;
              GCLock lock;
              ve = new Call(Location().introduce(),constants().ids.bool_eq,args);
            } else {
              // Don't post this
              ve = constants().lit_true;
            }
          }
          // Post new constraint
          if (ve != constants().lit_true) {
            clearInternalAnnotations(ve);
            added_constraints.push_back(ve);
          }
        }
      } else {
        // Ex: var false: b = e()
        if (vd->e() != NULL) {
          if (vd->e()->eid()==Expression::E_CALL) {
            // Convert functions to relations:
            //  var false: b = exists([x]) => array_bool_or([x], b)
            //  var false: b = forall([x]) => array_bool_and([x], b)
            //  var false: b = clause([x]) => bool_clause_reif([x], b)
            const Call* c = vd->e()->cast<Call>();
            GCLock lock;
            vd->e(NULL);
            ASTString cid;
            std::vector<Expression*> args(c->n_args());
            for (unsigned int i=args.size(); i--;)
              args[i] = c->arg(i);
            if (is_fixed) {
              args.push_back(constants().lit_false);
            } else {
              args.push_back(vd->id());
            }
            if (c->id() == constants().ids.exists) {
              cid = constants().ids.array_bool_or;
            } else if (c->id() == constants().ids.forall) {
              cid = constants().ids.array_bool_and;
            } else if (c->id() == constants().ids.clause) {
              cid = constants().ids.bool_clause_reif;
            } else {
              if (vd->ann().contains(constants().ctx.pos)) {
                cid = env.halfReifyId(c->id());
                if (env.model->matchFn(env, cid, args, false) == NULL) {
                  cid = env.reifyId(c->id());
                }
              } else {
                cid = env.reifyId(c->id());
              }
            }
            Call * nc = new Call(c->loc().introduce(),cid,args);
            nc->type(c->type());
            FunctionI* decl = env.model->matchFn(env, nc, false);
            if (decl==NULL) {
              throw FlatteningError(env,c->loc(),"'"+c->id().str()+"' is used in a reified context but no reified version is available");
            }
            nc->decl(decl);
            if (!is_fixed) {
              makeDefinedVar(vd, nc);
            }
            nc->ann().merge(c->ann());
            clearInternalAnnotations(nc);
            added_constraints.push_back(nc);
          } else {
            assert(vd->e()->eid() == Expression::E_ID ||
                   vd->e()->eid() == Expression::E_BOOLLIT);
          }
        }
        if (Expression::equal(vd->ti()->domain(),constants().lit_false)) {
          vd->ti()->domain(NULL);
          vd->e(constants().lit_false);
        }
      }
      if (vdi != NULL && is_fixed && env.vo.occurrences(vd)==0) {
        if (isOutput(vd)) {
          VarDecl* vd_output = (*env.output)[env.output_vo_flat.find(vd)]->cast<VarDeclI>()->e();
          if (vd_output->e() == NULL) {
            vd_output->e(vd->e());
          }
        }
        env.flat_removeItem(vdi);
      }
    } else if (vd->type().isvar() && vd->type().dim()==0) {
      // Int or Float var
      if (vd->e() != NULL) {
        if (const Call* cc = vd->e()->dyn_cast<Call>()) {
          // Remove RHS from vd
          vd->e(NULL);
          
          std::vector<Expression*> args(cc->n_args());
          ASTString cid;
          if (cc->id() == constants().ids.lin_exp) {
            // a = lin_exp([1],[b],5) => int_lin_eq([1,-1],[b,a],-5):: defines_var(a)
            ArrayLit* le_c = follow_id(cc->arg(0))->cast<ArrayLit>();
            std::vector<Expression*> nc(le_c->size());
            for (unsigned int i=static_cast<unsigned int>(nc.size()); i--;)
              nc[i] = (*le_c)[i];
            if (le_c->type().bt()==Type::BT_INT) {
              cid = constants().ids.int_.lin_eq;
              nc.push_back(IntLit::a(-1));
              args[0] = new ArrayLit(Location().introduce(),nc);
              args[0]->type(Type::parint(1));
              ArrayLit* le_x = follow_id(cc->arg(1))->cast<ArrayLit>();
              std::vector<Expression*> nx(le_x->size());
              for (unsigned int i=static_cast<unsigned int>(nx.size()); i--;)
                nx[i] = (*le_x)[i];
              nx.push_back(vd->id());
              args[1] = new ArrayLit(Location().introduce(),nx);
              args[1]->type(le_x->type());
              IntVal d = cc->arg(2)->cast<IntLit>()->v();
              args[2] = IntLit::a(-d);
            } else {
              // float
              cid = constants().ids.float_.lin_eq;
              nc.push_back(FloatLit::a(-1.0));
              args[0] = new ArrayLit(Location().introduce(),nc);
              args[0]->type(Type::parfloat(1));
              ArrayLit* le_x = follow_id(cc->arg(1))->cast<ArrayLit>();
              std::vector<Expression*> nx(le_x->size());
              for (unsigned int i=static_cast<unsigned int>(nx.size()); i--;)
                nx[i] = (*le_x)[i];
              nx.push_back(vd->id());
              args[1] = new ArrayLit(Location().introduce(),nx);
              args[1]->type(le_x->type());
              FloatVal d = cc->arg(2)->cast<FloatLit>()->v();
              args[2] = FloatLit::a(-d);
            }
          } else {
            if (cc->id() == "card") {
              // card is 'set_card' in old FlatZinc
              cid = constants().ids.set_card;
            } else {
              cid = cc->id();
            }
            for (unsigned int i=static_cast<unsigned int>(args.size()); i--;)
              args[i] = cc->arg(i);
            args.push_back(vd->id());
          }
          Call* nc = new Call(cc->loc().introduce(),cid,args);
          nc->type(cc->type());
          makeDefinedVar(vd, nc);
          nc->ann().merge(cc->ann());
          
          clearInternalAnnotations(nc);
          added_constraints.push_back(nc);
        } else {
          // RHS must be literal or Id
          assert(vd->e()->eid() == Expression::E_ID ||
                 vd->e()->eid() == Expression::E_INTLIT ||
                 vd->e()->eid() == Expression::E_FLOATLIT ||
                 vd->e()->eid() == Expression::E_BOOLLIT ||
                 vd->e()->eid() == Expression::E_SETLIT);
        }
      }
    } else if (vd->type().dim() > 0) {
      // vd is an array
      
      // If RHS is an Id, follow id to RHS
      // a = [1,2,3]; b = a;
      // vd = b => vd = [1,2,3]
      if (!vd->e()->isa<ArrayLit>()) {
        vd->e(follow_id(vd->e()));
      }
      
      // If empty array or 1 indexed, continue
      if (vd->ti()->ranges().size() == 1 &&
          vd->ti()->ranges()[0]->domain() != NULL &&
          vd->ti()->ranges()[0]->domain()->isa<SetLit>()) {
        IntSetVal* isv = vd->ti()->ranges()[0]->domain()->cast<SetLit>()->isv();
        if (isv && (isv->size()==0 || isv->min(0)==1))
          return added_constraints;
      }
      
      // Array should be 1 indexed since ArrayLit is 1 indexed
      assert(vd->e() != NULL);
      ArrayLit* al = NULL;
      Expression* e = vd->e();
      while (al==NULL) {
        switch (e->eid()) {
          case Expression::E_ARRAYLIT:
            al = e->cast<ArrayLit>();
            break;
          case Expression::E_ID:
            e = e->cast<Id>()->decl()->e();
            break;
          default:
            assert(false);
        }
      }
      al->make1d();
      IntSetVal* isv = IntSetVal::a(1,al->length());
      if (vd->ti()->ranges().size() == 1) {
        vd->ti()->ranges()[0]->domain(new SetLit(Location().introduce(),isv));
      } else {
        std::vector<TypeInst*> r(1);
        r[0] = new TypeInst(vd->ti()->ranges()[0]->loc(),
                            vd->ti()->ranges()[0]->type(),
                            new SetLit(Location().introduce(),isv));
        ASTExprVec<TypeInst> ranges(r);
        TypeInst* ti = new TypeInst(vd->ti()->loc(),vd->ti()->type(),ranges,vd->ti()->domain());
        vd->ti(ti);
      }
    }

    // Remove boolean context annotations used only on compilation
    vd->ann().remove(constants().ctx.mix);
    vd->ann().remove(constants().ctx.pos);
    vd->ann().remove(constants().ctx.neg);
    vd->ann().remove(constants().ctx.root);
    vd->ann().remove(constants().ann.promise_total);
    vd->ann().remove(constants().ann.add_to_output);
    vd->ann().remove(constants().ann.mzn_check_var);
    vd->ann().remove(constants().ann.rhs_from_assignment);
    vd->ann().removeCall(constants().ann.mzn_check_enum_var);

    return added_constraints;
  }
  
  Expression* cleanup_constraint(EnvI& env, std::unordered_set<Item*>& globals, Expression* ce) {
    clearInternalAnnotations(ce);
    
    if (Call* vc = ce->dyn_cast<Call>()) {
      for (unsigned int i=0; i<vc->n_args(); i++) {
        // Change array indicies to be 1 indexed
        if (ArrayLit* al = vc->arg(i)->dyn_cast<ArrayLit>()) {
          if (al->dims()>1 || al->min(0)!= 1) {
            al->make1d();
          }
        }
      }
      // Convert functions to relations:
      //   exists([x]) => array_bool_or([x],true)
      //   forall([x]) => array_bool_and([x],true)
      //   clause([x]) => bool_clause([x])
      //   bool_xor([x],[y]) => bool_xor([x],[y],true)
      if (vc->id() == constants().ids.exists) {
        GCLock lock;
        vc->id(constants().ids.array_bool_or);
        std::vector<Expression*> args(2);
        args[0] = vc->arg(0);
        args[1] = constants().lit_true;
        ASTExprVec<Expression> argsv(args);
        vc->args(argsv);
        vc->decl(env.model->matchFn(env, vc, false));
      } else if (vc->id() == constants().ids.forall) {
        GCLock lock;
        vc->id(constants().ids.array_bool_and);
        std::vector<Expression*> args(2);
        args[0] = vc->arg(0);
        args[1] = constants().lit_true;
        ASTExprVec<Expression> argsv(args);
        vc->args(argsv);
        vc->decl(env.model->matchFn(env, vc, false));
      } else if (vc->id() == constants().ids.clause) {
        GCLock lock;
        vc->id(constants().ids.bool_clause);
        vc->decl(env.model->matchFn(env, vc, false));
      } else if (vc->id() == constants().ids.bool_xor && vc->n_args()==2) {
        GCLock lock;
        std::vector<Expression*> args(3);
        args[0] = vc->arg(0);
        args[1] = vc->arg(1);
        args[2] = constants().lit_true;
        ASTExprVec<Expression> argsv(args);
        vc->args(argsv);
        vc->decl(env.model->matchFn(env, vc, false));
      }
      
      // If vc->decl() is a solver builtin and has not been added to the
      // FlatZinc, add it
      if (vc->decl() && vc->decl() != constants().var_redef &&
          !vc->decl()->from_stdlib() &&
          globals.find(vc->decl())==globals.end()) {
        env.flat_addItem(vc->decl());
        globals.insert(vc->decl());
      }
      return ce;
    } else if (Id* id = ce->dyn_cast<Id>()) {
      // Ex: constraint b; => constraint bool_eq(b, true);
      std::vector<Expression*> args(2);
      args[0] = id;
      args[1] = constants().lit_true;
      GCLock lock;
      return new Call(Location().introduce(),constants().ids.bool_eq,args);
    } else if (BoolLit* bl = ce->dyn_cast<BoolLit>()) {
      // Ex: true => delete; false => bool_eq(false, true);
      if (!bl->v()) {
        GCLock lock;
        std::vector<Expression*> args(2);
        args[0] = constants().lit_false;
        args[1] = constants().lit_true;
        Call* neq = new Call(Location().introduce(),constants().ids.bool_eq,args);
        return neq;
      } else {
        return NULL;
      }
    } else {
      return ce;
    }
  }
  
  void oldflatzinc(Env& e) {
    Model* m = e.flat();

    // Mark annotations and optional variables for removal
    for (unsigned int i=0; i<m->size(); i++) {
      Item* item = (*m)[i];
      if(VarDeclI* vdi = item->dyn_cast<VarDeclI>()) {
        if(item->cast<VarDeclI>()->e()->type().ot() == Type::OT_OPTIONAL ||
            item->cast<VarDeclI>()->e()->type().bt() == Type::BT_ANN) {
          e.envi().flat_removeItem(i);
        }
      }
    }

    EnvI& env = e.envi();

    int msize = m->size();

    // Predicate declarations of solver builtins
    std::unordered_set<Item*> globals;

    // Record indices of VarDeclIs with Id RHS for sorting & unification
    std::vector<int> declsWithIds;
    for (int i=0; i<msize; i++) {
      if ((*m)[i]->removed())
        continue;
      if (VarDeclI* vdi = (*m)[i]->dyn_cast<VarDeclI>()) {
        GCLock lock;
        VarDecl* vd = vdi->e();
        std::vector<Expression*> added_constraints = cleanup_vardecl(e.envi(), vdi, vd);
        // Record whether this VarDecl is equal to an Id (aliasing)
        if (vd->e() && vd->e()->isa<Id>()) {
          declsWithIds.push_back(i);
          vdi->e()->payload(-static_cast<int>(i)-1);
        } else {
          vdi->e()->payload(i);
        }
        for (auto nc : added_constraints) {
          Expression* new_ce = cleanup_constraint(e.envi(), globals, nc);
          if (new_ce) {
            e.envi().flat_addItem(new ConstraintI(Location().introduce(),new_ce));
          }
        }
      } else if (ConstraintI* ci = (*m)[i]->dyn_cast<ConstraintI>()) {
        Expression* new_ce = cleanup_constraint(e.envi(), globals, ci->e());
        if (new_ce) {
          ci->e(new_ce);
        } else {
          ci->remove();
        }
      } else if (FunctionI* fi = (*m)[i]->dyn_cast<FunctionI>()) {
        if (Let* let = Expression::dyn_cast<Let>(fi->e())) {
          GCLock lock;
          std::vector<Expression*> new_let;
          for (unsigned int i=0; i<let->let().size(); i++) {
            Expression* let_e = let->let()[i];
            if (VarDecl* vd = let_e->dyn_cast<VarDecl>()) {
              std::vector<Expression*> added_constraints = cleanup_vardecl(e.envi(), NULL, vd);
              new_let.push_back(vd);
              for (auto nc : added_constraints)
                new_let.push_back(nc);
            } else {
              Expression* new_ce = cleanup_constraint(e.envi(), globals, let_e);
              if (new_ce) {
                new_let.push_back(new_ce);
              }
            }
          }
          fi->e(new Let(let->loc(), new_let, let->in()));
        }
      } else if (SolveI* si = (*m)[i]->dyn_cast<SolveI>()) {
        if (si->e() && si->e()->type().ispar()) {
          // Introduce VarDecl if objective expression is par
          GCLock lock;
          TypeInst* ti = new TypeInst(Location().introduce(),si->e()->type(),NULL);
          VarDecl* constantobj = new VarDecl(Location().introduce(),ti,e.envi().genId(),si->e());
          si->e(constantobj->id());
          e.envi().flat_addItem(new VarDeclI(Location().introduce(),constantobj));
        }
      }
    }

    // Sort VarDecls in FlatZinc so that VarDecls are declared before use
    std::vector<VarDeclI*> sortedVarDecls(declsWithIds.size());
    int vdCount = 0;
    for (unsigned int i=0; i<declsWithIds.size(); i++) {
      VarDecl* cur = (*m)[declsWithIds[i]]->cast<VarDeclI>()->e();
      std::vector<int> stack;
      while (cur && cur->payload() < 0) {
        stack.push_back(cur->payload());
        if (Id* id = cur->e()->dyn_cast<Id>()) {
          cur = id->decl();
        } else {
          cur = NULL;
        }
      }
      for (unsigned int i=static_cast<unsigned int>(stack.size()); i--;) {
        VarDeclI* vdi = (*m)[-stack[i]-1]->cast<VarDeclI>();
        vdi->e()->payload(-vdi->e()->payload()-1);
        sortedVarDecls[vdCount++] = vdi;
      }
    }
    for (unsigned int i=0; i<declsWithIds.size(); i++) {
      (*m)[declsWithIds[i]] = sortedVarDecls[i];
    }

    // Remove marked items
    m->compact();
    e.envi().output->compact();

    for (IdMap<VarOccurrences::Items>::iterator it = env.vo._m.begin();
         it != env.vo._m.end(); ++it) {
      std::vector<Item*> toRemove;
      for (VarOccurrences::Items::iterator iit = it->second.begin();
           iit != it->second.end(); ++iit) {
        if ((*iit)->removed()) {
          toRemove.push_back(*iit);
        }
      }
      for (unsigned int i=0; i<toRemove.size(); i++) {
        it->second.erase(toRemove[i]);
      }
    }

    class Cmp {
    public:
      bool operator() (Item* i, Item* j) {
        if (i->iid()==Item::II_FUN || j->iid()==Item::II_FUN) {
          if (i->iid()==j->iid())
            return false;
          return i->iid()==Item::II_FUN;
        }
        if (i->iid()==Item::II_SOL) {
          assert(j->iid() != i->iid());
          return false;
        }
        if (j->iid()==Item::II_SOL) {
          assert(j->iid() != i->iid());
          return true;
        }
        if (i->iid()==Item::II_VD) {
          if (j->iid() != i->iid())
            return true;
          if (i->cast<VarDeclI>()->e()->type().ispar() &&
              j->cast<VarDeclI>()->e()->type().isvar())
            return true;
          if (j->cast<VarDeclI>()->e()->type().ispar() &&
              i->cast<VarDeclI>()->e()->type().isvar())
            return false;
          if (i->cast<VarDeclI>()->e()->type().dim() == 0 &&
              j->cast<VarDeclI>()->e()->type().dim() != 0)
            return true;
          if (i->cast<VarDeclI>()->e()->type().dim() != 0 &&
              j->cast<VarDeclI>()->e()->type().dim() == 0)
            return false;
          if (i->cast<VarDeclI>()->e()->e()==NULL &&
              j->cast<VarDeclI>()->e()->e() != NULL)
            return true;
          if (i->cast<VarDeclI>()->e()->e() &&
              j->cast<VarDeclI>()->e()->e() &&
              !i->cast<VarDeclI>()->e()->e()->isa<Id>() &&
              j->cast<VarDeclI>()->e()->e()->isa<Id>())
            return true;
        }
        return false;
      }
    } _cmp;
    // Perform final sorting
    std::stable_sort(m->begin(),m->end(),_cmp);
  }

  FlatModelStatistics statistics(Env& m) {
    Model* flat = m.flat();
    FlatModelStatistics stats;
    for (unsigned int i=0; i<flat->size(); i++) {
      if (!(*flat)[i]->removed()) {
        if (VarDeclI* vdi = (*flat)[i]->dyn_cast<VarDeclI>()) {
          Type t = vdi->e()->type();
          if (t.isvar() && t.dim()==0) {
            if (t.is_set())
              stats.n_set_vars++;
            else if (t.isint())
              stats.n_int_vars++;
            else if (t.isbool())
              stats.n_bool_vars++;
            else if (t.isfloat())
              stats.n_float_vars++;
          }
        } else if (ConstraintI* ci = (*flat)[i]->dyn_cast<ConstraintI>()) {
          if (Call* call = ci->e()->dyn_cast<Call>()) {
            if (call->n_args() > 0) {
              Type all_t;
              for (unsigned int i=0; i<call->n_args(); i++) {
                Type t = call->arg(i)->type();
                if (t.isvar()) {
                  if (t.st()==Type::ST_SET)
                    all_t = t;
                  else if (t.bt()==Type::BT_FLOAT && all_t.st()!=Type::ST_SET)
                    all_t = t;
                  else if (t.bt()==Type::BT_INT && all_t.bt()!=Type::BT_FLOAT && all_t.st()!=Type::ST_SET)
                    all_t = t;
                  else if (t.bt()==Type::BT_BOOL && all_t.bt()!=Type::BT_INT && all_t.bt()!=Type::BT_FLOAT && all_t.st()!=Type::ST_SET)
                    all_t = t;
                }
              }
              if (all_t.isvar()) {
                if (all_t.st()==Type::ST_SET)
                  stats.n_set_ct++;
                else if (all_t.bt()==Type::BT_INT)
                  stats.n_int_ct++;
                else if (all_t.bt()==Type::BT_BOOL)
                  stats.n_bool_ct++;
                else if (all_t.bt()==Type::BT_FLOAT)
                  stats.n_float_ct++;
              }
            }
          }
        }
      }
    }
    return stats;
  }
  
}
