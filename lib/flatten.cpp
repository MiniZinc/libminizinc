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

#include <minizinc/stl_map_set.hh>

#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

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
  std::vector<Expression*> toExpVec(std::vector<KeepAlive>& v) {
    std::vector<Expression*> r(v.size());
    for (unsigned int i=v.size(); i--;)
      r[i] = v[i]();
    return r;
  }

  void addCtxAnn(VarDecl* vd, BCtx& c) {
    if (vd) {
      Id* ctx_id = NULL;
      switch (c) {
        case C_ROOT: ctx_id=constants().ctx.root; break;
        case C_POS: ctx_id=constants().ctx.pos; break;
        case C_NEG: ctx_id=constants().ctx.neg; break;
        case C_MIX: ctx_id=constants().ctx.mix; break;
        default: assert(false);;
      }
      vd->addAnnotation(ctx_id);
    }
  }

  Expression* definesVarAnn(Id* id) {
    std::vector<Expression*> args(1);
    args[0] = id;
    Call* c = new Call(Location().introduce(),constants().ann.defines_var,args);
    c->type(Type::ann());
    return c;
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
      if (FunctionI* fi = env.orig->matchRevMap(env, vd->type())) {
        // We need to introduce a reverse mapper
        Call* revmap = new Call(Location().introduce(), fi->id(), {vd->id()});
        revmap->decl(fi);
        revmap->type(Type::varbool());
        env.flat_addItem(new ConstraintI(Location().introduce(), revmap));
      }

      VarDeclI* ni = new VarDeclI(Location().introduce(),vd);
      env.flat_addItem(ni);
      EE ee(vd,NULL);
      env.map_insert(vd->id(),ee);
    }

    return vd;
  }

#define MZN_FILL_REIFY_MAP(T,ID) reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.T.ID,constants().ids.T ## reif.ID));

  EnvI::EnvI(Model* orig0) :
    orig(orig0),
    output(new Model),
    current_pass_no(0),
    final_pass_no(1),
    maxPathDepth(0),
    ignorePartial(false),
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
  }
  long long int
  EnvI::genId(void) {
      return ids++;
    }
  void EnvI::map_insert(Expression* e, const EE& ee) {
      KeepAlive ka(e);
      map.insert(ka,WW(ee.r(),ee.b()));
    }
  EnvI::Map::iterator EnvI::map_find(Expression* e) {
    KeepAlive ka(e);
    Map::iterator it = map.find(ka);
    if (it != map.end()) {
      if (it->second.r()) {
        if (it->second.r()->isa<VarDecl>()) {
          int idx = vo.find(it->second.r()->cast<VarDecl>());
          if (idx == -1 || (*_flat)[idx]->removed())
            return map.end();
        }
      } else {
        return map.end();
      }
    }
    return it;
  }
  void EnvI::map_remove(Expression* e) {
    KeepAlive ka(e);
    map.remove(ka);
  }
  EnvI::Map::iterator EnvI::map_end(void) {
    return map.end();
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
    map.dump<EED>();
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
      int prev = idStack.size() > 0 ? idStack.back() : 0;
      bool allCalls = true;
      for (int i = callStack.size()-1; i >= prev; i--) {
        Expression* ee = callStack[i]->untag();
        allCalls = allCalls && (i==callStack.size()-1 || ee->isa<Call>());
        for (ExpressionSetIter it = ee->ann().begin(); it != ee->ann().end(); ++it) {
          EE ee_ann = flat_exp(*this, Ctx(), *it, NULL, constants().var_true);
          if (allCalls || !isDefinesVarAnn(ee_ann.r()))
            toAnnotate->addAnnotation(ee_ann.r());
        }
      }
    }
    if (toAdd) {
      CollectOccurrencesE ce(vo,i);
      topDown(ce,toAdd);
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
      ret = enumVarDecls.size();
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
      ret = arrayEnumDecls.size();
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
    if (vd->e() && vd->e()->isa<Call>()) {
      int prev = idStack.size() > 0 ? idStack.back() : 0;
      for (int i = callStack.size()-1; i >= prev; i--) {
        Expression* ee = callStack[i]->untag();
        for (ExpressionSetIter it = ee->ann().begin(); it != ee->ann().end(); ++it) {
          EE ee_ann = flat_exp(*this, Ctx(), *it, NULL, constants().var_true);
          vd->e()->addAnnotation(ee_ann.r());
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
    Model* tmp = orig;
    orig = _flat;
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
    for (unsigned int i=callStack.size(); i--;) {
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
    map.clear();
    delete _flat;
    delete orig;
    _flat=0;
    orig=0;
  }
 
  CallStackItem::CallStackItem(EnvI& env0, Expression* e) : env(env0) {
    if (e->isa<VarDecl>())
      env.idStack.push_back(env.callStack.size());
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
  
  class CallArgItem {
  public:
    EnvI& env;
    CallArgItem(EnvI& env0) : env(env0) {
      env.idStack.push_back(env.callStack.size());
    }
    ~CallArgItem(void) {
      env.idStack.pop_back();
    }
  };
  
  FlatteningError::FlatteningError(EnvI& env, const Location& loc, const std::string& msg)
  : LocationException(env,loc,msg) {}
  
  Env::Env(void) : e(new EnvI(NULL)) {}
  Env::Env(Model* m) : e(new EnvI(m)) {}
  Env::~Env(void) {
    delete e;
  }
  
  Model*
  Env::model(void) { return e->orig; }
  void
  Env::model(Model* m) { e->orig = m; }
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
      maxPathDepth = callStack.size();
    }

    unsigned int lastError = callStack.size();

    std::string major_sep = ";";
    std::string minor_sep = "|";
    for (unsigned int i=0; i<lastError; i++) {
      Expression* e = callStack[i]->untag();
      bool isCompIter = callStack[i]->isTagged();
      Location loc = e->loc();
      int filenameId;
      UNORDERED_NAMESPACE::unordered_map<std::string, int>::iterator findFilename = filenameMap.find(loc.filename().str());
      if (findFilename == filenameMap.end()) {
        if(!force && current_pass_no >= final_pass_no-1)
          return false;
        filenameId = filenameMap.size();
        filenameMap.insert(std::make_pair(loc.filename().str(), filenameMap.size()));
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
          _output->addItem(new VarDeclI(Location().introduce(), vd));

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
          showArgs[0] = vd->id();
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
    for (int i=0; i<al->size(); i++) {
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
  
  bool isTotal(FunctionI* fi) {
    return fi->ann().contains(constants().ann.promise_total);
  }

  bool isReverseMap(BinOp* e) {
    return e->ann().contains(constants().ann.is_reverse_map);
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
          } else {
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
  
  Expression* createDummyValue(EnvI& env, const Type& t) {
    if (t.dim()>0) {
      Expression* ret = new ArrayLit(Location().introduce(), std::vector<Expression*>());
      Type ret_t = t;
      ret_t.ti(Type::TI_PAR);
      ret->type(ret_t);
      return ret;
    }
    if (t.st()==Type::ST_SET) {
      Expression* ret = new SetLit(Location().introduce(), std::vector<Expression*>());
      Type ret_t = t;
      ret_t.ti(Type::TI_PAR);
      ret->type(ret_t);
      return ret;
    }
    switch (t.bt()) {
      case Type::BT_INT:
        return IntLit::a(0);
      case Type::BT_BOOL:
        return constants().boollit(false);
      case Type::BT_FLOAT:
        return FloatLit::a(0);
      case Type::BT_STRING:
        return new StringLit(Location().introduce(), "");
      case Type::BT_ANN:
        return constants().ann.promise_total;
      default:
        return NULL;
    }
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
                c->decl(env.orig->matchFn(env,c,false));
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
                c->decl(env.orig->matchFn(env,c,false));
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

            EnvI::Map::iterator it = env.map_find(al);
            if (it != env.map_end()) {
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
            env.map_insert(al,ee);
            env.map_insert(vd->e(),ee);
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
            c->decl(env.orig->matchFn(env,c,false));
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
                  c->decl(env.orig->matchFn(env,c,false));
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
                  c->decl(env.orig->matchFn(env, c, false));
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
                  c->decl(env.orig->matchFn(env,c,false));
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
              c->decl(env.orig->matchFn(env,c,false));
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
                for (unsigned int i=ncoeff.size(); i--;)
                  ncoeff[i] = (*le_c)[i];
                ncoeff.push_back(IntLit::a(-1));
                args.push_back(new ArrayLit(Location().introduce(),ncoeff));
                args[0]->type(le_c->type());
                ArrayLit* le_x = follow_id(c->arg(1))->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->size());
                for (unsigned int i=nx.size(); i--;)
                  nx[i] = (*le_x)[i];
                nx.push_back(vd->id());
                args.push_back(new ArrayLit(Location().introduce(),nx));
                args[1]->type(le_x->type());
                args.push_back(c->arg(2));
                nc = new Call(c->loc().introduce(), constants().ids.lin_exp, args);
                nc->decl(env.orig->matchFn(env,nc,false));
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
                for (unsigned int i=args.size(); i--;)
                  args[i] = c->arg(i);
                args.push_back(vd->id());
                ASTString nid = c->id();

                if (c->id() == constants().ids.exists) {
                  nid = constants().ids.array_bool_or;
                } else if (c->id() == constants().ids.forall) {
                  nid = constants().ids.array_bool_and;
                } else if (vd->type().isbool()) {
                  nid = env.reifyId(c->id());
                }
                nc = new Call(c->loc().introduce(), nid, args);
              }
              nc->decl(env.orig->matchFn(env,nc,false));
              if (nc->decl() == NULL) {
                throw InternalError("undeclared function or predicate "
                                    +nc->id().str());
              }
              nc->type(nc->decl()->rtype(env,args,false));
              nc->addAnnotation(definesVarAnn(vd->id()));
              vd->addAnnotation(constants().ann.is_defined_var);
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
          ret->decl(env.orig->matchFn(env,ret,false));
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
          ret->decl(env.orig->matchFn(env, ret, false));
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
  
  ASTString opToBuiltin(BinOp* op, BinOpType bot) {
    std::string builtin;
    if (op->rhs()->type().isint()) {
      switch (bot) {
        case BOT_PLUS: return constants().ids.int_.plus;
        case BOT_MINUS: return constants().ids.int_.minus;
        case BOT_MULT: return constants().ids.int_.times;
        case BOT_IDIV: return constants().ids.int_.div;
        case BOT_MOD: return constants().ids.int_.mod;
        case BOT_LE: return constants().ids.int_.lt;
        case BOT_LQ: return constants().ids.int_.le;
        case BOT_GR: return constants().ids.int_.gt;
        case BOT_GQ: return constants().ids.int_.ge;
        case BOT_EQ: return constants().ids.int_.eq;
        case BOT_NQ: return constants().ids.int_.ne;
        default:
          throw InternalError("not yet implemented");
      }
    } else if (op->rhs()->type().isbool()) {
      if (bot==BOT_EQ || bot==BOT_EQUIV)
        return constants().ids.bool_eq;
      builtin = "bool_";
    } else if (op->rhs()->type().is_set()) {
      builtin = "set_";
    } else if (op->rhs()->type().isfloat()) {
      switch (bot) {
        case BOT_PLUS: return constants().ids.float_.plus;
        case BOT_MINUS: return constants().ids.float_.minus;
        case BOT_MULT: return constants().ids.float_.times;
        case BOT_DIV: return constants().ids.float_.div;
        case BOT_MOD: return constants().ids.float_.mod;
        case BOT_LE: return constants().ids.float_.lt;
        case BOT_LQ: return constants().ids.float_.le;
        case BOT_GR: return constants().ids.float_.gt;
        case BOT_GQ: return constants().ids.float_.ge;
        case BOT_EQ: return constants().ids.float_.eq;
        case BOT_NQ: return constants().ids.float_.ne;
        default:
          throw InternalError("not yet implemented");
      }
    } else if (op->rhs()->type().isopt() &&
               (bot==BOT_EQUIV || bot==BOT_EQ)) {
      /// TODO: extend to all option type operators
      switch (op->lhs()->type().bt()) {
        case Type::BT_BOOL: return constants().ids.bool_eq;
        case Type::BT_FLOAT: return constants().ids.float_.eq;
        case Type::BT_INT:
          if (op->lhs()->type().st()==Type::ST_PLAIN)
            return constants().ids.int_.eq;
          else
            return constants().ids.set_eq;
        default:
          throw InternalError("not yet implemented");
      }
      
    } else {
      throw InternalError(op->opToString().str()+" not yet implemented");
    }
    switch (bot) {
    case BOT_PLUS:
      return builtin+"plus";
    case BOT_MINUS:
      return builtin+"minus";
    case BOT_MULT:
      return builtin+"times";
    case BOT_DIV:
      return builtin+"div";
    case BOT_IDIV:
      return builtin+"div";
    case BOT_MOD:
      return builtin+"mod";
    case BOT_LE:
      return builtin+"lt";
    case BOT_LQ:
      return builtin+"le";
    case BOT_GR:
      return builtin+"gt";
    case BOT_GQ:
      return builtin+"ge";
    case BOT_EQ:
      return builtin+"eq";
    case BOT_NQ:
      return builtin+"ne";
    case BOT_IN:
      return constants().ids.set_in;
    case BOT_SUBSET:
      return builtin+"subset";
    case BOT_SUPERSET:
      return builtin+"superset";
    case BOT_UNION:
      return builtin+"union";
    case BOT_DIFF:
      return builtin+"diff";
    case BOT_SYMDIFF:
      return builtin+"symdiff";
    case BOT_INTERSECT:
      return builtin+"intersect";
    case BOT_PLUSPLUS:
    case BOT_DOTDOT:
      throw InternalError("not yet implemented");
    case BOT_EQUIV:
      return builtin+"eq";
    case BOT_IMPL:
      return builtin+"le";
    case BOT_RIMPL:
      return builtin+"ge";
    case BOT_OR:
      return builtin+"or";
    case BOT_AND:
      return builtin+"and";
    case BOT_XOR:
      return constants().ids.bool_xor;
    default:
      assert(false); return ASTString("");
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
  
  Call* same_call(Expression* e, const ASTString& id) {
    Expression* ce = follow_id(e);
    if (ce && ce->isa<Call>() && ce->cast<Call>()->id() == id)
      return ce->cast<Call>();
    return NULL;
  }
  
  template<class Lit>
  void collectLinExps(EnvI& env,
                      typename LinearTraits<Lit>::Val c, Expression* exp,
                      std::vector<typename LinearTraits<Lit>::Val>& coeffs,
                      std::vector<KeepAlive>& vars,
                      typename LinearTraits<Lit>::Val& constval) {
    typedef typename LinearTraits<Lit>::Val Val;
    struct StackItem {
      Expression* e;
      Val c;
      StackItem(Expression* e0, Val c0) : e(e0), c(c0) {}
    };
    std::vector<StackItem> stack;
    stack.push_back(StackItem(exp,c));
    while (!stack.empty()) {
      Expression* e = stack.back().e;
      Val c = stack.back().c;
      stack.pop_back();
      if (e==NULL)
        continue;
      if (e->type().ispar()) {
        constval += c * LinearTraits<Lit>::eval(env,e);
      } else if (Lit* l = e->dyn_cast<Lit>()) {
        constval += c * l->v();
      } else if (BinOp* bo = e->dyn_cast<BinOp>()) {
        switch (bo->op()) {
          case BOT_PLUS:
            stack.push_back(StackItem(bo->lhs(),c));
            stack.push_back(StackItem(bo->rhs(),c));
            break;
          case BOT_MINUS:
            stack.push_back(StackItem(bo->lhs(),c));
            stack.push_back(StackItem(bo->rhs(),-c));
            break;
          case BOT_MULT:
            if (bo->lhs()->type().ispar()) {
              stack.push_back(StackItem(bo->rhs(),c*LinearTraits<Lit>::eval(env,bo->lhs())));
            } else if (bo->rhs()->type().ispar()) {
              stack.push_back(StackItem(bo->lhs(),c*LinearTraits<Lit>::eval(env,bo->rhs())));
            } else {
              coeffs.push_back(c);
              vars.push_back(e);
            }
            break;
          case BOT_DIV:
            if (bo->rhs()->isa<FloatLit>() && bo->rhs()->cast<FloatLit>()->v()==1.0) {
              stack.push_back(StackItem(bo->lhs(),c));
            } else {
              coeffs.push_back(c);
              vars.push_back(e);
            }
            break;
          case BOT_IDIV:
            if (bo->rhs()->isa<IntLit>() && bo->rhs()->cast<IntLit>()->v()==1) {
              stack.push_back(StackItem(bo->lhs(),c));
            } else {
              coeffs.push_back(c);
              vars.push_back(e);
            }
            break;
          default:
            coeffs.push_back(c);
            vars.push_back(e);
            break;
        }
//      } else if (Call* call = e->dyn_cast<Call>()) {
//        /// TODO! Handle sum, lin_exp (maybe not that important?)
      } else {
        coeffs.push_back(c);
        vars.push_back(e);
      }
    }
  }

  template<class Lit>
  KeepAlive mklinexp(EnvI& env, typename LinearTraits<Lit>::Val c0, typename LinearTraits<Lit>::Val c1,
                     Expression* e0, Expression* e1) {
    typedef typename LinearTraits<Lit>::Val Val;
    GCLock lock;
    
    std::vector<Val> coeffs;
    std::vector<KeepAlive> vars;
    Val constval = 0;
    collectLinExps<Lit>(env, c0, e0, coeffs, vars, constval);
    collectLinExps<Lit>(env, c1, e1, coeffs, vars, constval);
    simplify_lin<Lit>(coeffs, vars, constval);
    KeepAlive ka;
    if (coeffs.size()==0) {
      ka = LinearTraits<Lit>::newLit(constval);
    } else if (coeffs.size()==1 && coeffs[0]==1 && constval==0) {
      ka = vars[0];
    } else {
      std::vector<Expression*> coeffs_e(coeffs.size());
      for (unsigned int i=coeffs.size(); i--;) {
        if (!LinearTraits<Lit>::finite(coeffs[i])) {
          throw FlatteningError(env,e0->loc(), "unbounded coefficient in linear expression");
        }
        coeffs_e[i] = LinearTraits<Lit>::newLit(coeffs[i]);
      }
      std::vector<Expression*> vars_e(vars.size());
      for (unsigned int i=vars.size(); i--;)
        vars_e[i] = vars[i]();
      
      std::vector<Expression*> args(3);
      args[0]=new ArrayLit(e0->loc(),coeffs_e);
      Type t = coeffs_e[0]->type();
      t.dim(1);
      args[0]->type(t);
      args[1]=new ArrayLit(e0->loc(),vars_e);
      Type tt = vars_e[0]->type();
      tt.dim(1);
      args[1]->type(tt);
      args[2] = LinearTraits<Lit>::newLit(constval);
      Call* c = new Call(e0->loc().introduce(),constants().ids.lin_exp,args);
      addPathAnnotation(env, c);
      tt = args[1]->type();
      tt.dim(0);
      c->decl(env.orig->matchFn(env, c, false));
      if (c->decl()==NULL) {
        throw FlatteningError(env,c->loc(), "cannot find matching declaration");
      }
      c->type(c->decl()->rtype(env, args, false));
      ka = c;
    }
    assert(ka());
    return ka;
  }

  class CmpExp {
  public:
    bool operator ()(const KeepAlive& i, const KeepAlive& j) const {
      if (Expression::equal(i(),j()))
        return false;
      return i()<j();
    }
  };

  bool remove_dups(std::vector<KeepAlive>& x, bool identity) {
    for (unsigned int i=0; i<x.size(); i++) {
      x[i] = follow_id_to_value(x[i]());
    }
    std::sort(x.begin(),x.end(),CmpExp());
    int ci = 0;
    Expression* prev = NULL;
    for (unsigned int i=0; i<x.size(); i++) {
      if (!Expression::equal(x[i](),prev)) {
        prev = x[i]();
        if (x[i]()->isa<BoolLit>()) {
          if (x[i]()->cast<BoolLit>()->v()==identity) {
            // skip
          } else {
            return true;
          }
        } else {
          x[ci++] = x[i];
        }
      }
    }
    x.resize(ci);
    return false;
  }
  bool contains_dups(std::vector<KeepAlive>& x, std::vector<KeepAlive>& y) {
    if (x.size()==0 || y.size()==0)
      return false;
    unsigned int ix=0;
    unsigned int iy=0;
    for (;;) {
      if (x[ix]()==y[iy]())
        return true;
      if (x[ix]() < y[iy]()) {
        ix++;
      } else {
        iy++;
      }
      if (ix==x.size() || iy==y.size())
        return false;
    }
  }

  /// Return a lin_exp or id if \a e is a lin_exp or id
  template<class Lit>
  Expression* get_linexp(Expression* e) {
    for (;;) {
      if (e && e->eid()==Expression::E_ID && e != constants().absent) {
        if (e->cast<Id>()->decl()->e()) {
          e = e->cast<Id>()->decl()->e();
        } else {
          break;
        }
      } else {
        break;
      }
    }
    if (e && (e->isa<Id>() || e->isa<Lit>() ||
              (e->isa<Call>() && e->cast<Call>()->id() == constants().ids.lin_exp)))
      return e;
    return NULL;
  }


  KeepAlive flat_cv_exp(EnvI& env, Ctx ctx, Expression* e);
  
  /// TODO: check if all expressions are total
  /// If yes, use element encoding
  /// If not, use implication encoding
  EE flat_ite(EnvI& env,Ctx ctx, ITE* ite, VarDecl* r, VarDecl* b) {
    
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
              clause->decl(env.orig->matchFn(env, clause, false));
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
            clause->decl(env.orig->matchFn(env, clause, false));
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
        clause->decl(env.orig->matchFn(env, clause, false));
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
          clause->decl(env.orig->matchFn(env, clause, false));
          clause->type(clause->decl()->rtype(env, clauseArgs, false));
          clause->ann().add(constants().ann.promise_total);
          (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
        }
        pos.pop_back();
        pos.push_back(b->id());
        neg[1] = defined[i];
        clauseArgs[0] = new ArrayLit(Location().introduce(),pos);
        clauseArgs[0]->type(Type::varbool(1));
        clauseArgs[1] = new ArrayLit(Location().introduce(),neg);
        clauseArgs[1]->type(Type::varbool(1));
        {
          // b[i] /\ c -> b
          Call* clause = new Call(Location().introduce(), constants().ids.clause, clauseArgs);
          clause->decl(env.orig->matchFn(env, clause, false));
          clause->type(clause->decl()->rtype(env, clauseArgs, false));
          clause->ann().add(constants().ann.promise_total);
          (void) flat_exp(env, Ctx(), clause, constants().var_true, constants().var_true);
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

  template<class Lit>
  void flatten_linexp_binop(EnvI& env, Ctx ctx, VarDecl* r, VarDecl* b, EE& ret,
                            Expression* le0, Expression* le1, BinOpType& bot, bool doubleNeg,
                            std::vector<EE>& ees, std::vector<KeepAlive>& args, ASTString& callid) {
    typedef typename LinearTraits<Lit>::Val Val;
    std::vector<Val> coeffv;
    std::vector<KeepAlive> alv;
    Val d = 0;
    Expression* le[2] = {le0,le1};
    
    Id* assignTo = NULL;
    if (bot==BOT_EQ && ctx.b == C_ROOT) {
      if (le0->isa<Id>()) {
        assignTo = le0->cast<Id>();
      } else if (le1->isa<Id>()) {
        assignTo = le1->cast<Id>();
      }
    }
    
    for (unsigned int i=0; i<2; i++) {
      Val sign = (i==0 ? 1 : -1);
      if (Lit* l = le[i]->dyn_cast<Lit>()) {
        try {
          d += sign*l->v();
        } catch (ArithmeticError& e) {
          throw EvalError(env,l->loc(),e.msg());
        }
      } else if (le[i]->isa<Id>()) {
        coeffv.push_back(sign);
        alv.push_back(le[i]);
      } else if (Call* sc = le[i]->dyn_cast<Call>()) {
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(env,sc->arg(0));
        ArrayLit* sc_al = eval_array_lit(env,sc->arg(1));
        try {
          d += sign*LinearTraits<Lit>::eval(env,sc->arg(2));
          for (unsigned int j=0; j<sc_coeff->size(); j++) {
            coeffv.push_back(sign*LinearTraits<Lit>::eval(env,(*sc_coeff)[j]));
            alv.push_back((*sc_al)[j]);
          }
        } catch (ArithmeticError& e) {
          throw EvalError(env,sc->loc(),e.msg());
        }
        
      } else {
        throw EvalError(env, le[i]->loc(), "Internal error, unexpected expression inside linear expression");
      }
    }
    simplify_lin<Lit>(coeffv,alv,d);
    if (coeffv.size()==0) {
      bool result;
      switch (bot) {
        case BOT_LE: result = (0<-d); break;
        case BOT_LQ: result = (0<=-d); break;
        case BOT_GR: result = (0>-d); break;
        case BOT_GQ: result = (0>=-d); break;
        case BOT_EQ: result = (0==-d); break;
        case BOT_NQ: result = (0!=-d); break;
        default: assert(false); break;
      }
      if (doubleNeg)
        result = !result;
      ees[2].b = constants().boollit(result);
      ret.r = conj(env,r,ctx,ees);
      return;
    } else if (coeffv.size()==1 &&
               std::abs(coeffv[0])==1) {
      if (coeffv[0]==-1) {
        switch (bot) {
          case BOT_LE: bot = BOT_GR; break;
          case BOT_LQ: bot = BOT_GQ; break;
          case BOT_GR: bot = BOT_LE; break;
          case BOT_GQ: bot = BOT_LQ; break;
          default: break;
        }
      } else {
        d = -d;
      }
      typename LinearTraits<Lit>::Bounds ib = LinearTraits<Lit>::compute_bounds(env,alv[0]());
      if (ib.valid) {
        bool failed = false;
        bool subsumed = false;
        switch (bot) {
          case BOT_LE:
            subsumed = ib.u < d;
            failed = ib.l >= d;
            break;
          case BOT_LQ:
            subsumed = ib.u <= d;
            failed = ib.l > d;
            break;
          case BOT_GR:
            subsumed = ib.l > d;
            failed = ib.u <= d;
            break;
          case BOT_GQ:
            subsumed = ib.l >= d;
            failed = ib.u < d;
            break;
          case BOT_EQ:
            subsumed = ib.l==d && ib.u==d;
            failed = ib.u < d || ib.l > d;
            break;
          case BOT_NQ:
            subsumed = ib.u < d || ib.l > d;
            failed = ib.l==d && ib.u==d;
            break;
          default: break;
        }
        if (doubleNeg) {
          std::swap(subsumed, failed);
        }
        if (subsumed) {
          ees[2].b = constants().lit_true;
          ret.r = conj(env,r,ctx,ees);
          return;
        } else if (failed) {
          ees[2].b = constants().lit_false;
          ret.r = conj(env,r,ctx,ees);
          return;
        }
      }
      
      if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && bot==BOT_EQ) {
        GCLock lock;
        VarDecl* vd = alv[0]()->cast<Id>()->decl();
        if (vd->ti()->domain()) {
          typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(env,vd->ti()->domain());
          if (LinearTraits<Lit>::domain_contains(domain,d)) {
            if (!LinearTraits<Lit>::domain_equals(domain,d)) {
              vd->ti()->setComputedDomain(false);
              vd->ti()->domain(LinearTraits<Lit>::new_domain(d));
            }
            ret.r = bind(env,ctx,r,constants().lit_true);
          } else {
            ret.r = bind(env,ctx,r,constants().lit_false);
          }
        } else {
          vd->ti()->setComputedDomain(false);
          vd->ti()->domain(LinearTraits<Lit>::new_domain(d));
          ret.r = bind(env,ctx,r,constants().lit_true);
        }
      } else {
        
        GCLock lock;
        Expression* e0;
        Expression* e1;
        BinOpType old_bot = bot;
        Val old_d = d;
        switch (bot) {
          case BOT_LE:
            e0 = alv[0]();
            if (e0->type().isint()) {
              d--;
              bot = BOT_LQ;
            }
            e1 = LinearTraits<Lit>::newLit(d);
            break;
          case BOT_GR:
            e1 = alv[0]();
            if (e1->type().isint()) {
              d++;
              bot = BOT_LQ;
            } else {
              bot = BOT_LE;
            }
            e0 = LinearTraits<Lit>::newLit(d);
            break;
          case BOT_GQ:
            e0 = LinearTraits<Lit>::newLit(d);
            e1 = alv[0]();
            bot = BOT_LQ;
            break;
          default:
            e0 = alv[0]();
            e1 = LinearTraits<Lit>::newLit(d);
        }
        if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && alv[0]()->cast<Id>()->decl()->ti()->domain()) {
          VarDecl* vd = alv[0]()->cast<Id>()->decl();
          typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(env,vd->ti()->domain());
          typename LinearTraits<Lit>::Domain ndomain = LinearTraits<Lit>::limit_domain(old_bot,domain,old_d);
          if (domain && ndomain) {
            if (LinearTraits<Lit>::domain_empty(ndomain)) {
              ret.r = bind(env,ctx,r,constants().lit_false);
              return;
            } else if (!LinearTraits<Lit>::domain_equals(domain,ndomain)) {
              ret.r = bind(env,ctx,r,constants().lit_true);
              vd->ti()->setComputedDomain(false);
              vd->ti()->domain(LinearTraits<Lit>::new_domain(ndomain));

              if (r==constants().var_true) {
                BinOp* bo = new BinOp(Location().introduce(), e0, bot, e1);
                bo->type(Type::varbool());
                std::vector<Expression*> boargs(2);
                boargs[0] = e0;
                boargs[1] = e1;
                Call* c = new Call(Location(), opToBuiltin(bo, bot), boargs);
                c->type(Type::varbool());
                c->decl(env.orig->matchFn(env, c, false));
                EnvI::Map::iterator it = env.map_find(c);
                if (it != env.map_end()) {
                  if (Id* ident = it->second.r()->template dyn_cast<Id>()) {
                    bind(env, Ctx(), ident->decl(), constants().lit_true);
                    it->second.r = constants().lit_true;
                  }
                  if (Id* ident = it->second.b()->template dyn_cast<Id>()) {
                    bind(env, Ctx(), ident->decl(), constants().lit_true);
                    it->second.b = constants().lit_true;
                  }
                }
              }
            }
            return;
          }
        }
        args.push_back(e0);
        args.push_back(e1);
      }
    } else if (bot==BOT_EQ && coeffv.size()==2 && coeffv[0]==-coeffv[1] && d==0) {
      Id* id0 = alv[0]()->cast<Id>();
      Id* id1 = alv[1]()->cast<Id>();
      if (ctx.b == C_ROOT && r==constants().var_true &&
          (id0->decl()->e()==NULL || id1->decl()->e()==NULL)) {
        if (id0->decl()->e())
          (void) bind(env,ctx,id1->decl(),id0);
        else
          (void) bind(env,ctx,id0->decl(),id1);
      } else {
        callid = LinearTraits<Lit>::id_eq();
        args.push_back(alv[0]());
        args.push_back(alv[1]());
      }
    } else {
      GCLock lock;
      if (assignTo != NULL) {
        Val resultCoeff = 0;
        typename LinearTraits<Lit>::Bounds bounds(d,d,true);
        for (unsigned int i=coeffv.size(); i--;) {
          if (alv[i]()==assignTo) {
            resultCoeff = coeffv[i];
            continue;
          }
          typename LinearTraits<Lit>::Bounds b = LinearTraits<Lit>::compute_bounds(env,alv[i]());

          if (b.valid && LinearTraits<Lit>::finite(b)) {
            if (coeffv[i] > 0) {
              bounds.l += coeffv[i]*b.l;
              bounds.u += coeffv[i]*b.u;
            } else {
              bounds.l += coeffv[i]*b.u;
              bounds.u += coeffv[i]*b.l;
            }
          } else {
            bounds.valid = false;
            break;
          }
        }
        if (bounds.valid && resultCoeff!=0) {
          if (resultCoeff < 0) {
            bounds.l = LinearTraits<Lit>::floor_div(bounds.l,-resultCoeff);
            bounds.u = LinearTraits<Lit>::ceil_div(bounds.u,-resultCoeff);
          } else {
            Val bl = bounds.l;
            bounds.l = LinearTraits<Lit>::ceil_div(bounds.u,-resultCoeff);
            bounds.u = LinearTraits<Lit>::floor_div(bl,-resultCoeff);
          }
          VarDecl* vd = assignTo->decl();
          if (vd->ti()->domain()) {
            typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(env,vd->ti()->domain());
            if (LinearTraits<Lit>::domain_intersects(domain,bounds.l,bounds.u)) {
              typename LinearTraits<Lit>::Domain new_domain = LinearTraits<Lit>::intersect_domain(domain,bounds.l,bounds.u);
              if (!LinearTraits<Lit>::domain_equals(domain,new_domain)) {
                vd->ti()->setComputedDomain(false);
                vd->ti()->domain(LinearTraits<Lit>::new_domain(new_domain));
              }
            } else {
              ret.r = bind(env,ctx,r,constants().lit_false);
            }
          } else {
            vd->ti()->setComputedDomain(true);
            vd->ti()->domain(LinearTraits<Lit>::new_domain(bounds.l,bounds.u));
          }
        }
      }

      int coeff_sign;
      LinearTraits<Lit>::constructLinBuiltin(bot,callid,coeff_sign,d);
      std::vector<Expression*> coeff_ev(coeffv.size());
      for (unsigned int i=coeff_ev.size(); i--;)
        coeff_ev[i] = LinearTraits<Lit>::newLit(coeff_sign*coeffv[i]);
      ArrayLit* ncoeff = new ArrayLit(Location().introduce(),coeff_ev);
      Type t = coeff_ev[0]->type();
      t.dim(1);
      ncoeff->type(t);
      args.push_back(ncoeff);
      std::vector<Expression*> alv_e(alv.size());
      Type tt = alv[0]()->type();
      tt.dim(1);
      for (unsigned int i=alv.size(); i--;) {
        if (alv[i]()->type().isvar())
          tt.ti(Type::TI_VAR);
        alv_e[i] = alv[i]();
      }
      ArrayLit* nal = new ArrayLit(Location().introduce(),alv_e);
      nal->type(tt);
      args.push_back(nal);
      Lit* il = LinearTraits<Lit>::newLit(-d);
      args.push_back(il);
    }
  }
  
  template<class Lit>
  void flatten_linexp_call(EnvI& env, Ctx ctx, Ctx nctx, ASTString& cid, Call* c,
                           EE& ret, VarDecl* b, VarDecl* r,
                           std::vector<EE>& args_ee, std::vector<KeepAlive>& args) {
    typedef typename LinearTraits<Lit>::Val Val;
    Expression* al_arg = (cid==constants().ids.sum ? args_ee[0].r() : args_ee[1].r());
    EE flat_al = flat_exp(env,nctx,al_arg,NULL,NULL);
    ArrayLit* al = follow_id(flat_al.r())->template cast<ArrayLit>();
    KeepAlive al_ka = al;
    if (al->dims()>1) {
      Type alt = al->type();
      alt.dim(1);
      GCLock lock;
      al = new ArrayLit(al->loc(),*al);
      al->type(alt);
      al_ka = al;
    }
    Val d = (cid == constants().ids.sum ? Val(0) : LinearTraits<Lit>::eval(env,args_ee[2].r()));
    
    std::vector<Val> c_coeff(al->size());
    if (cid==constants().ids.sum) {
      for (unsigned int i=al->size(); i--;)
        c_coeff[i] = 1;
    } else {
      EE flat_coeff = flat_exp(env,nctx,args_ee[0].r(),NULL,NULL);
      ArrayLit* coeff = follow_id(flat_coeff.r())->template cast<ArrayLit>();
      for (unsigned int i=coeff->size(); i--;)
        c_coeff[i] = LinearTraits<Lit>::eval(env,(*coeff)[i]);
    }
    cid = constants().ids.lin_exp;
    std::vector<Val> coeffv;
    std::vector<KeepAlive> alv;
    for (unsigned int i=0; i<al->size(); i++) {
      if (Call* sc = same_call((*al)[i],cid)) {
        if (VarDecl* alvi_decl = follow_id_to_decl((*al)[i])->dyn_cast<VarDecl>()) {
          if (alvi_decl->ti()->domain()) {
            typename LinearTraits<Lit>::Domain sc_dom = LinearTraits<Lit>::eval_domain(env,alvi_decl->ti()->domain());
            typename LinearTraits<Lit>::Bounds sc_bounds = LinearTraits<Lit>::compute_bounds(env,sc);
            if (LinearTraits<Lit>::domain_tighter(sc_dom, sc_bounds)) {
              coeffv.push_back(c_coeff[i]);
              alv.push_back((*al)[i]);
              continue;
            }
          }
        }
        
        Val cd = c_coeff[i];
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(env,sc->arg(0));
        ArrayLit* sc_al = eval_array_lit(env,sc->arg(1));
        Val sc_d = LinearTraits<Lit>::eval(env,sc->arg(2));
        assert(sc_coeff->size() == sc_al->size());
        for (unsigned int j=0; j<sc_coeff->size(); j++) {
          coeffv.push_back(cd*LinearTraits<Lit>::eval(env,(*sc_coeff)[j]));
          alv.push_back((*sc_al)[j]);
        }
        d += cd*sc_d;
      } else {
        coeffv.push_back(c_coeff[i]);
        alv.push_back((*al)[i]);
      }
    }
    simplify_lin<Lit>(coeffv,alv,d);
    if (coeffv.size()==0) {
      GCLock lock;
      ret.b = conj(env,b,Ctx(),args_ee);
      ret.r = bind(env,ctx,r,LinearTraits<Lit>::newLit(d));
      return;
    } else if (coeffv.size()==1 && coeffv[0]==1 && d==0) {
      ret.b = conj(env,b,Ctx(),args_ee);
      ret.r = bind(env,ctx,r,alv[0]());
      return;
    }
    GCLock lock;
    std::vector<Expression*> coeff_ev(coeffv.size());
    for (unsigned int i=coeff_ev.size(); i--;)
      coeff_ev[i] = LinearTraits<Lit>::newLit(coeffv[i]);
    ArrayLit* ncoeff = new ArrayLit(Location().introduce(),coeff_ev);
    Type t = coeff_ev[0]->type();
    t.dim(1);
    ncoeff->type(t);
    args.push_back(ncoeff);
    std::vector<Expression*> alv_e(alv.size());
    bool al_same_as_before = alv.size()==al->size();
    for (unsigned int i=alv.size(); i--;) {
      alv_e[i] = alv[i]();
      al_same_as_before = al_same_as_before && Expression::equal(alv_e[i],(*al)[i]);
    }
    if (al_same_as_before) {
      Expression* rd = follow_id_to_decl(flat_al.r());
      if (rd->isa<VarDecl>())
        rd = rd->cast<VarDecl>()->id();
      if (rd->type().dim()>1) {
        ArrayLit* al = eval_array_lit(env,rd);
        std::vector<std::pair<int,int> > dims(1);
        dims[0].first = 1;
        dims[0].second = al->size();
        rd = new ArrayLit(al->loc(),*al,dims);
        Type t = al->type();
        t.dim(1);
        rd->type(t);
      }
      args.push_back(rd);
    } else {
      ArrayLit* nal = new ArrayLit(al->loc(),alv_e);
      nal->type(al->type());
      args.push_back(nal);
    }
    Lit* il = LinearTraits<Lit>::newLit(d);
    args.push_back(il);
  }

  Call* aggregateAndOrOps(EnvI& env, BinOp* bo, bool negateArgs, BinOpType bot) {
    assert(bot == BOT_AND || bot == BOT_OR);
    BinOpType negbot = (bot == BOT_AND ? BOT_OR : BOT_AND);
    typedef std::pair<Expression*,bool> arg_literal;
    std::vector<arg_literal> bo_args(2);
    bo_args[0] = arg_literal(bo->lhs(), !negateArgs);
    bo_args[1] = arg_literal(bo->rhs(), !negateArgs);
    std::vector<Expression*> output_pos;
    std::vector<Expression*> output_neg;
    unsigned int processed = 0;
    while (processed < bo_args.size()) {
      BinOp* bo_arg = bo_args[processed].first->dyn_cast<BinOp>();
      UnOp* uo_arg = bo_args[processed].first->dyn_cast<UnOp>();
      bool positive = bo_args[processed].second;
      if (bo_arg && positive && bo_arg->op() == bot) {
        bo_args[processed].first = bo_arg->lhs();
        bo_args.push_back(arg_literal(bo_arg->rhs(),true));
      } else if (bo_arg && !positive && bo_arg->op() == negbot) {
        bo_args[processed].first = bo_arg->lhs();
        bo_args.push_back(arg_literal(bo_arg->rhs(),false));
      } else if (uo_arg && !positive && uo_arg->op() == UOT_NOT) {
        bo_args[processed].first = uo_arg->e();
        bo_args[processed].second = true;
      } else if (bot==BOT_OR && uo_arg && positive && uo_arg->op() == UOT_NOT) {
        output_neg.push_back(uo_arg->e());
        processed++;
      } else {
        if (positive) {
          output_pos.push_back(bo_args[processed].first);
        } else {
          output_neg.push_back(bo_args[processed].first);
        }
        processed++;
      }
    }
    Call* c;
    std::vector<Expression*> c_args(1);
    if (bot == BOT_AND) {
      for (unsigned int i=0; i<output_neg.size(); i++) {
        UnOp* neg_arg = new UnOp(output_neg[i]->loc(),UOT_NOT,output_neg[i]);
        neg_arg->type(output_neg[i]->type());
        output_pos.push_back(neg_arg);
      }
      ArrayLit* al = new ArrayLit(bo->loc().introduce(), output_pos);
      Type al_t = bo->type();
      al_t.dim(1);
      al->type(al_t);
      c_args[0] = al;
      c = new Call(bo->loc().introduce(), bot==BOT_AND ? constants().ids.forall : constants().ids.exists, c_args);
    } else {
      ArrayLit* al_pos = new ArrayLit(bo->loc().introduce(), output_pos);
      Type al_t = bo->type();
      al_t.dim(1);
      al_pos->type(al_t);
      c_args[0] = al_pos;
      if (output_neg.size() > 0) {
        ArrayLit* al_neg = new ArrayLit(bo->loc().introduce(), output_neg);
        al_neg->type(al_t);
        c_args.push_back(al_neg);
      }
      c = new Call(bo->loc().introduce(), output_neg.empty() ? constants().ids.exists : constants().ids.clause, c_args);
    }
    c->decl(env.orig->matchFn(env, c, false));
    assert(c->decl());
    Type t = c->decl()->rtype(env, c_args, false);
    t.cv(bo->type().cv());
    c->type(t);
    return c;
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
        for (unsigned int i=0; i<al->dims(); i++) {
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
    
  }

#ifndef NDEBUG
  void mzn_break_here(Expression* e) {
    std::cerr << "% mzn_break_here: " << *e << "\n";
  }
#endif

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();

#ifndef NDEBUG
    Annotation& e_ann = e->ann();
    if(e_ann.contains(constants().ann.mzn_break_here))
        mzn_break_here(e);
#endif

    EE ret;
    assert(!e->type().isunknown());
    if (e->type().ispar() && !e->isa<Let>() && !e->isa<VarDecl>() && e->type().bt()!=Type::BT_ANN) {

      if (e->type().cv()) {
        KeepAlive ka = flat_cv_exp(env,ctx,e);
        ret.r = bind(env,ctx,r,ka());
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        return ret;
      }
      if (e->type().dim() > 0) {
        EnvI::Map::iterator it;
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
        } else if ( (it = env.map_find(e)) != env.map_end()) {
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
          if ( (it = env.map_find(al)) != env.map_end()) {
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
          env.map_insert(al,ee);
          env.map_insert(vd->e(),ee);
          
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
    switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_STRINGLIT:
      {
        CallStackItem _csi(env,e);
        GCLock lock;
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,Ctx(),r,e);
        return ret;
      }
    case Expression::E_SETLIT:
      {
        CallStackItem _csi(env,e);
        SetLit* sl = e->cast<SetLit>();
        assert(sl->isv()==NULL && sl->fsv()==NULL);
        std::vector<EE> elems_ee(sl->v().size());
        for (unsigned int i=sl->v().size(); i--;)
          elems_ee[i] = flat_exp(env,ctx,sl->v()[i],NULL,NULL);
        std::vector<Expression*> elems(elems_ee.size());
        bool allPar = true;
        for (unsigned int i=elems.size(); i--;) {
          elems[i] = elems_ee[i].r();
          allPar = allPar && elems[i]->type().ispar();
        }

        ret.b = conj(env,b,Ctx(),elems_ee);
        if (allPar) {
          GCLock lock;
          Expression* ee = eval_par(env,e);
          ret.r = bind(env,Ctx(),r,ee);
        } else {
          GCLock lock;
          ArrayLit* al = new ArrayLit(sl->loc(),elems);
          al->type(Type::varint(1));
          std::vector<Expression*> args(1);
          args[0] = al;
          Call* cc = new Call(sl->loc().introduce(), "array2set", args);
          cc->type(Type::varsetint());
          FunctionI* fi = env.orig->matchFn(env, cc->id(), args, false);
          if (fi==NULL) {
            throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
          }
          assert(fi);
          assert(env.isSubtype(fi->rtype(env, args, false),cc->type(),false));
          cc->decl(fi);
          EE ee = flat_exp(env, Ctx(), cc, NULL, constants().var_true);
          ret.r = bind(env,Ctx(),r,ee.r());
        }
      }
      break;
    case Expression::E_BOOLLIT:
      {
        CallStackItem _csi(env,e);
        GCLock lock;
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,ctx,r,e);
        return ret;
      }
      break;
    case Expression::E_ID:
      {
        CallStackItem _csi(env,e);
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
        id = follow_id_to_decl(id)->cast<VarDecl>()->id();
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
            idxset->decl(env.orig->matchFn(env, idxset, false));
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
            if (id->decl()->e()==NULL || id->decl()->e()->type().isvar() || id->decl()->e()->type().dim() > 0) {
              // New top-level id, need to copy into env.m
              vd = flat_exp(env,Ctx(),id->decl(),NULL,constants().var_true).r()
                   ->cast<Id>()->decl();
            } else {
              vd = id->decl();
            }
          }
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          if (vd->e()!=NULL) {
            if (vd->e()->type().ispar() && vd->e()->type().dim()==0)
              rete = eval_par(env, vd->e());
            if (vd->e()->isa<Id>())
              rete = vd->e();
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
            env.map_insert(vd->e(), ee);
          }
          if (rete==NULL) {
            if (!vd->toplevel()) {
              // create new VarDecl in toplevel, if decl doesnt exist yet
              EnvI::Map::iterator it = env.map_find(vd->e());
              if (it==env.map_end()) {
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
                    env.map_insert(vd->e(),ee);
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
      }
      break;
    case Expression::E_ANON:
      {
        CallStackItem _csi(env,e);
        AnonVar* av = e->cast<AnonVar>();
        if (av->type().isbot()) {
          throw InternalError("type of anonymous variable could not be inferred");
        }
        GCLock lock;
        VarDecl* vd = newVarDecl(env, Ctx(), new TypeInst(Location().introduce(), av->type()), NULL, NULL, NULL);
        ret.b = bind(env, Ctx(), b, constants().lit_true);
        ret.r = bind(env, ctx, r, vd->id());
      }
      break;
    case Expression::E_ARRAYLIT:
      {
        CallStackItem _csi(env,e);
        ArrayLit* al = e->cast<ArrayLit>();
        if (al->flat()) {
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          ret.r = bind(env,Ctx(),r,al);
        } else {
          std::vector<EE> elems_ee(al->size());
          for (unsigned int i=al->size(); i--;)
            elems_ee[i] = flat_exp(env,ctx,(*al)[i],NULL,NULL);
          std::vector<Expression*> elems(elems_ee.size());
          for (unsigned int i=elems.size(); i--;)
            elems[i] = elems_ee[i].r();
          std::vector<std::pair<int,int> > dims(al->dims());
          for (unsigned int i=al->dims(); i--;)
            dims[i] = std::pair<int,int>(al->min(i), al->max(i));
          KeepAlive ka;
          {
            GCLock lock;
            ArrayLit* alr = new ArrayLit(Location().introduce(),elems,dims);
            alr->type(al->type());
            alr->flat(true);
            ka = alr;
          }
          ret.b = conj(env,b,Ctx(),elems_ee);
          ret.r = bind(env,Ctx(),r,ka());
        }
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        CallStackItem _csi(env,e);
        ArrayAccess* aa = e->cast<ArrayAccess>();
        KeepAlive aa_ka = aa;

        Ctx nctx = ctx;
        nctx.b = +nctx.b;
        nctx.neg = false;
        EE eev = flat_exp(env,nctx,aa->v(),NULL,NULL);
        std::vector<EE> ees;

      start_flatten_arrayaccess:
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          Expression* tmp = follow_id_to_decl(aa->idx()[i]);
          if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
            tmp = vd->id();
          if (tmp->type().ispar()) {
            ArrayLit* al;
            if (eev.r()->isa<ArrayLit>()) {
              al = eev.r()->cast<ArrayLit>();
            } else {
              Id* id = eev.r()->cast<Id>();
              if (id->decl()==NULL) {
                throw InternalError("undefined identifier");
              }
              if (id->decl()->e()==NULL) {
                throw InternalError("array without initialiser not supported");
              }
              Expression* id_e = follow_id(id);
              if (id_e->isa<ArrayLit>()) {
                al = id_e->cast<ArrayLit>();
              } else {
                throw InternalError("builtin function returning array not supported");
              }
            }
            
            std::vector<KeepAlive> elems;
            std::vector<IntVal> idx(aa->idx().size());
            std::vector<std::pair<int,int> > dims;
            std::vector<Expression*> newaccess;
            std::vector<int> nonpar;
            std::vector<int> stack;
            for (unsigned int j=0; j<aa->idx().size(); j++) {
              Expression* tmp = follow_id_to_decl(aa->idx()[j]);
              if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
                tmp = vd->id();
              if (tmp->type().ispar()) {
                GCLock lock;
                idx[j] = eval_int(env, tmp).toInt();
              } else {
                idx[j] = al->min(j);
                stack.push_back(nonpar.size());
                nonpar.push_back(j);
                dims.push_back(std::make_pair(al->min(j), al->max(j)));
                newaccess.push_back(aa->idx()[j]);
              }
            }
            if (stack.empty()) {
              bool success;
              KeepAlive ka;
              {
                GCLock lock;
                ka = eval_arrayaccess(env, al, idx, success);
                if (!success && env.in_maybe_partial==0) {
                  ResultUndefinedError e(env,al->loc(),"array access out of bounds");
                }
              }
              ees.push_back(EE(NULL,constants().boollit(success)));
              ees.push_back(EE(NULL,eev.b()));
              if (aa->type().isbool() && !aa->type().isopt()) {
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                ees.push_back(EE(NULL,ka()));
                ret.r = conj(env,r,ctx,ees);
              } else {
                ret.b = conj(env,b,ctx,ees);
                ret.r = bind(env,ctx,r,ka());
              }
              return ret;
            }
            while (!stack.empty()) {
              int cur = stack.back();
              if (cur==nonpar.size()-1) {
                stack.pop_back();
                for (int i = al->min(nonpar[cur]); i <= al->max(nonpar[cur]); i++) {
                  idx[nonpar[cur]] = i;
                  bool success;
                  GCLock lock;
                  Expression* al_idx = eval_arrayaccess(env, al, idx, success);
                  if (!success) {
                    if (env.in_maybe_partial==0) {
                      ResultUndefinedError e(env,al->loc(),"array access out of bounds");
                    }
                    ees.push_back(EE(NULL,constants().lit_false));
                    ees.push_back(EE(NULL,eev.b()));
                    if (aa->type().isbool() && !aa->type().isopt()) {
                      ret.b = bind(env,Ctx(),b,constants().lit_true);
                      ret.r = conj(env,r,ctx,ees);
                    } else {
                      ret.b = conj(env,b,ctx,ees);
                      ret.r = bind(env,ctx,r,al_idx);
                    }
                    return ret;
                  }
                  elems.push_back(al_idx);
                }
              } else {
                if (idx[nonpar[cur]].toInt()==al->max(nonpar[cur])) {
                  idx[nonpar[cur]]=al->min(nonpar[cur]);
                  stack.pop_back();
                } else {
                  idx[nonpar[cur]]++;
                  for (unsigned int j=cur+1; j<nonpar.size(); j++)
                    stack.push_back(j);
                }
              }
            }
            std::vector<Expression*> elems_e(elems.size());
            for (unsigned int i=0; i<elems.size(); i++)
              elems_e[i] = elems[i]();
            {
              GCLock lock;
              Expression* newal = new ArrayLit(al->loc(), elems_e, dims);
              Type t = al->type();
              t.dim(dims.size());
              newal->type(t);
              eev.r = newal;
              ArrayAccess* n_aa = new ArrayAccess(aa->loc(), newal, newaccess);
              n_aa->type(aa->type());
              aa = n_aa;
              aa_ka = aa;
            }
          }
        }
        
        if (aa->idx().size()==1 && aa->idx()[0]->isa<ArrayAccess>()) {
          ArrayAccess* aa_inner = aa->idx()[0]->cast<ArrayAccess>();
          ArrayLit* al;
          if (eev.r()->isa<ArrayLit>()) {
            al = eev.r()->cast<ArrayLit>();
          } else {
            Id* id = eev.r()->cast<Id>();
            if (id->decl()==NULL) {
              throw InternalError("undefined identifier");
            }
            if (id->decl()->e()==NULL) {
              throw InternalError("array without initialiser not supported");
            }
            al = follow_id(id)->cast<ArrayLit>();
          }
          if (aa_inner->v()->type().ispar()) {
            KeepAlive ka_al_inner = flat_cv_exp(env, ctx, aa_inner->v());
            ArrayLit* al_inner = ka_al_inner()->cast<ArrayLit>();
            std::vector<Expression*> composed_e(al_inner->size());
            for (unsigned int i=0; i<al_inner->size(); i++) {
              GCLock lock;
              IntVal inner_idx = eval_int(env, (*al_inner)[i]);
              if (inner_idx < al->min(0) || inner_idx > al->max(0))
                goto flatten_arrayaccess;
              composed_e[i] = (*al)[inner_idx.toInt()-al->min(0)];
            }
            std::vector<std::pair<int,int> > dims(al_inner->dims());
            for (unsigned int i=0; i<al_inner->dims(); i++) {
              dims[i] = std::make_pair(al_inner->min(i), al_inner->max(i));
            }
            {
              GCLock lock;
              Expression* newal = new ArrayLit(al->loc(), composed_e, dims);
              Type t = al->type();
              t.dim(dims.size());
              newal->type(t);
              eev.r = newal;
              ArrayAccess* n_aa = new ArrayAccess(aa->loc(), newal, aa_inner->idx());
              n_aa->type(aa->type());
              aa = n_aa;
              aa_ka = aa;
              goto start_flatten_arrayaccess;
            }
            
          }
        }
      flatten_arrayaccess:
        Ctx dimctx = ctx;
        dimctx.neg = false;
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          Expression* tmp = follow_id_to_decl(aa->idx()[i]);
          if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
            tmp = vd->id();
          ees.push_back(flat_exp(env, dimctx, tmp, NULL, NULL));
        }
        ees.push_back(EE(NULL,eev.b()));
        
        bool parAccess=true;
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          if (!ees[i].r()->type().ispar()) {
            parAccess = false;
            break;
          }
        }

        if (parAccess) {
          ArrayLit* al;
          if (eev.r()->isa<ArrayLit>()) {
            al = eev.r()->cast<ArrayLit>();
          } else {
            Id* id = eev.r()->cast<Id>();
            if (id->decl()==NULL) {
              throw InternalError("undefined identifier");
            }
            if (id->decl()->e()==NULL) {
              throw InternalError("array without initialiser not supported");
            }
            al = follow_id(id)->cast<ArrayLit>();
          }
          KeepAlive ka;
          bool success;
          {
            GCLock lock;
            std::vector<IntVal> dims(aa->idx().size());
            for (unsigned int i=aa->idx().size(); i--;)
              dims[i] = eval_int(env,ees[i].r());
            ka = eval_arrayaccess(env,al,dims,success);
          }
          if (!success && env.in_maybe_partial==0) {
            ResultUndefinedError e(env,al->loc(),"array access out of bounds");
          }
          ees.push_back(EE(NULL,constants().boollit(success)));
          if (aa->type().isbool() && !aa->type().isopt()) {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            ees.push_back(EE(NULL,ka()));
            ret.r = conj(env,r,ctx,ees);
          } else {
            ret.b = conj(env,b,ctx,ees);
            ret.r = bind(env,ctx,r,ka());
          }
        } else {
          std::vector<Expression*> args(aa->idx().size()+1);
          for (unsigned int i=aa->idx().size(); i--;)
            args[i] = ees[i].r();
          args[aa->idx().size()] = eev.r();
          KeepAlive ka;
          {
            GCLock lock;
            Call* cc = new Call(e->loc().introduce(),constants().ids.element,args);
            cc->type(aa->type());
            FunctionI* fi = env.orig->matchFn(env,cc->id(),args,false);
            if (fi==NULL) {
              throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
            }
            assert(fi);
            assert(env.isSubtype(fi->rtype(env,args,false),cc->type(),false));
            cc->decl(fi);
            ka = cc;
          }
          Ctx elemctx = ctx;
          elemctx.neg = false;
          EE ee = flat_exp(env,elemctx,ka(),NULL,NULL);
          ees.push_back(ee);
          if (aa->type().isbool() && !aa->type().isopt()) {
            ee.b = ee.r;
            ees.push_back(ee);
            ret.r = conj(env,r,ctx,ees);
            ret.b = bind(env,ctx,b,constants().boollit(!ctx.neg));
          } else {
            ret.r = bind(env,ctx,r,ee.r());
            ret.b = conj(env,b,ctx,ees);
          }
        }
      }
      break;
    case Expression::E_COMP:
      {
        CallStackItem _csi(env,e);
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
                ub->decl(env.orig->matchFn(env, ub, false));
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
                    default:
                    {
                      Call* forall = new Call(c->where(i)->loc(), constants().ids.forall, parWhere);
                      forall->type(Type::parbool());
                      forall->decl(env.orig->matchFn(env, forall, false));
                      orig_where[i] = forall;
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
              forall->decl(env.orig->matchFn(env, forall, false));
              cond = forall;
            } else {
              cond = where[0];
            }
            
            Expression* new_e;

            Call* surround = env.surroundingCall();
            
            Type ntype = c->type();
            if (surround->id()==constants().ids.forall) {
              new_e = new BinOp(Location().introduce(), cond, BOT_IMPL, c->e());
              new_e->type(Type::varbool());
              ntype.ot(Type::OT_PRESENT);
            } else if (surround->id()==constants().ids.exists) {
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
                  bc->decl(env.orig->matchFn(env, bc, false));
                  r_bounds = bc;
                } else {
                  Call* lbc = new Call(Location().introduce(),"lb",ubargs);
                  lbc->type(Type::parint());
                  lbc->decl(env.orig->matchFn(env, lbc, false));
                  Call* ubc = new Call(Location().introduce(),"ub",ubargs);
                  ubc->type(Type::parint());
                  ubc->decl(env.orig->matchFn(env, ubc, false));
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
              absent_r->decl(env.orig->matchFn(env, absent_r, false));
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
        for (unsigned int i=elems.size(); i--;) {
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
      }
      break;
    case Expression::E_ITE:
      {
        CallStackItem _csi(env,e);
        ITE* ite = e->cast<ITE>();
        ret = flat_ite(env,ctx,ite,r,b);
      }
      break;
    case Expression::E_BINOP:
      {
        CallStackItem _csi(env,e);
        BinOp* bo = e->cast<BinOp>();
        if (isReverseMap(bo)) {
          CallArgItem cai(env);
          Id* id = bo->lhs()->dyn_cast<Id>();
          if (id==NULL)
            throw EvalError(env, bo->lhs()->loc(), "Reverse mappers are only defined for identifiers");
          if (bo->op() != BOT_EQ && bo->op() != BOT_EQUIV)
            throw EvalError(env, bo->loc(), "Reverse mappers have to use `=` as the operator");
          Call* c = bo->rhs()->dyn_cast<Call>();
          if (c==NULL)
            throw EvalError(env, bo->rhs()->loc(), "Reverse mappers require call on right hand side");

          std::vector<Expression*> args(c->n_args());
          for (unsigned int i=0; i<c->n_args(); i++) {
            Id* idi = c->arg(i)->dyn_cast<Id>();
            if (idi==NULL)
              throw EvalError(env, c->arg(i)->loc(), "Reverse mapper calls require identifiers as arguments");
            EE ee = flat_exp(env, Ctx(), idi, NULL, constants().var_true);
            args[i] = ee.r();
          }
          
          EE ee = flat_exp(env, Ctx(), id, NULL, constants().var_true);
          
          GCLock lock;
          Call* revMap = new Call(Location().introduce(),c->id(),args);
          
          args.push_back(ee.r());
          Call* keepAlive = new Call(Location().introduce(),constants().var_redef->id(),args);
          keepAlive->type(Type::varbool());
          keepAlive->decl(constants().var_redef);
          ret = flat_exp(env, Ctx(), keepAlive, constants().var_true, constants().var_true);
          
          if (ee.r()->isa<Id>()) {
            env.reverseMappers.insert(ee.r()->cast<Id>(),revMap);
          }
          break;
        }
        if ( (bo->op()==BOT_EQ ||  bo->op()==BOT_EQUIV) && (bo->lhs()==constants().absent || bo->rhs()==constants().absent) ) {
          GCLock lock;
          std::vector<Expression*> args(1);
          args[0] = bo->lhs()==constants().absent ? bo->rhs() : bo->lhs();
          if (args[0] != constants().absent) {
            Call* cr = new Call(bo->loc().introduce(),"absent",args);
            cr->decl(env.orig->matchFn(env, cr, false));
            cr->type(cr->decl()->rtype(env, args, false));
            ret = flat_exp(env, ctx, cr, r, b);
          } else {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            ret.r = bind(env,ctx,r,constants().lit_true);
          }
          break;
        }
        Ctx ctx0 = ctx;
        ctx0.neg = false;
        Ctx ctx1 = ctx;
        ctx1.neg = false;
        BinOpType bot = bo->op();
        if (bo->lhs()->type().isbool()) {
          switch (bot) {
            case BOT_EQ: bot = BOT_EQUIV; break;
            case BOT_NQ: bot = BOT_XOR; break;
            case BOT_LQ: bot = BOT_IMPL; break;
            case BOT_GQ: bot = BOT_RIMPL; break;
            default: break;
          }
        }
        bool negArgs = false;
        bool doubleNeg = false;
        if (ctx.neg) {
          switch (bot) {
            case BOT_AND:
              ctx.b = -ctx.b;
              ctx.neg = false;
              negArgs = true;
              bot = BOT_OR;
              break;
            case BOT_OR:
              ctx.b = -ctx.b;
              ctx.neg = false;
              negArgs = true;
              bot = BOT_AND;
              break;
            default: break;
          }
        }
        Expression* boe0 = bo->lhs();
        Expression* boe1 = bo->rhs();
        switch (bot) {
          case BOT_PLUS:
          {
            KeepAlive ka;
            if (boe0->type().isint()) {
              ka = mklinexp<IntLit>(env,1,1,boe0,boe1);
            } else {
              ka = mklinexp<FloatLit>(env,1.0,1.0,boe0,boe1);
            }
            ret = flat_exp(env,ctx,ka(),r,b);
          }
            break;
          case BOT_MINUS:
          {
            KeepAlive ka;
            if (boe0->type().isint()) {
              ka = mklinexp<IntLit>(env,1,-1,boe0,boe1);
            } else {
              ka = mklinexp<FloatLit>(env,1.0,-1.0,boe0,boe1);
            }
            ret = flat_exp(env,ctx,ka(),r,b);
          }
            break;
          case BOT_MULT:
          case BOT_IDIV:
          case BOT_MOD:
          case BOT_DIV:
          case BOT_UNION:
          case BOT_DIFF:
          case BOT_SYMDIFF:
          case BOT_INTERSECT:
          case BOT_DOTDOT:
          {
            assert(!ctx0.neg);
            assert(!ctx1.neg);
            EE e0 = flat_exp(env,ctx0,boe0,NULL,b);
            EE e1 = flat_exp(env,ctx1,boe1,NULL,b);
            
            if (e0.r()->type().ispar() && e1.r()->type().ispar()) {
              GCLock lock;
              BinOp* parbo = new BinOp(bo->loc(),e0.r(),bo->op(),e1.r());
              Type tt = bo->type();
              tt.ti(Type::TI_PAR);
              parbo->type(tt);
              try {
                Expression* res = eval_par(env,parbo);
                assert(!res->type().isunknown());
                ret.r = bind(env,ctx,r,res);
                std::vector<EE> ees(2);
                ees[0].b = e0.b; ees[1].b = e1.b;
                ret.b = conj(env,b,Ctx(),ees);
              } catch (ResultUndefinedError&) {
                ret.r = createDummyValue(env, e->type());
                ret.b = bind(env,Ctx(),b,constants().lit_false);
              }
              break;
            }
            
            if (bot==BOT_MULT) {
              Expression* e0r = e0.r();
              Expression* e1r = e1.r();
              if (e0r->type().ispar())
                std::swap(e0r,e1r);
              if (e1r->type().ispar() && e1r->type().isint()) {
                IntVal coeff = eval_int(env,e1r);
                KeepAlive ka = mklinexp<IntLit>(env,coeff,0,e0r,NULL);
                ret = flat_exp(env,ctx,ka(),r,b);
                break;
              } else if (e1r->type().ispar() && e1r->type().isfloat()) {
                FloatVal coeff = eval_float(env,e1r);
                KeepAlive ka = mklinexp<FloatLit>(env,coeff,0.0,e0r,NULL);
                ret = flat_exp(env,ctx,ka(),r,b);
                break;
              }
            } else if (bot==BOT_DIV || bot==BOT_IDIV) {
              Expression* e0r = e0.r();
              Expression* e1r = e1.r();
              if (e1r->type().ispar() && e1r->type().isint()) {
                IntVal coeff = eval_int(env,e1r);
                if (coeff==1) {
                  ret = flat_exp(env,ctx,e0r,r,b);
                  break;
                }
              } else if (e1r->type().ispar() && e1r->type().isfloat()) {
                FloatVal coeff = eval_float(env,e1r);
                if (coeff==1.0) {
                  ret = flat_exp(env,ctx,e0r,r,b);
                  break;
                } else {
                  KeepAlive ka = mklinexp<FloatLit>(env,1.0/coeff,0.0,e0r,NULL);
                  ret = flat_exp(env,ctx,ka(),r,b);
                  break;
                }
              }
            }

            
            GC::lock();
            std::vector<Expression*> args(2);
            args[0] = e0.r(); args[1] = e1.r();
            Call* cc;
            if (bo->decl()) {
              cc = new Call(bo->loc().introduce(),bo->opToString(),args);
            } else {
              cc = new Call(bo->loc().introduce(),opToBuiltin(bo,bot),args);
            }
            cc->type(bo->type());

            EnvI::Map::iterator cit;
            if ( (cit = env.map_find(cc)) != env.map_end()) {
              ret.b = bind(env,Ctx(),b,env.ignorePartial ? constants().lit_true : cit->second.b());
              ret.r = bind(env,ctx,r,cit->second.r());
            } else {
              if (FunctionI* fi = env.orig->matchFn(env,cc->id(),args,false)) {
                assert(cc->type() == fi->rtype(env,args,false));
                cc->decl(fi);
                cc->type(cc->decl()->rtype(env,args,false));
                KeepAlive ka(cc);
                GC::unlock();
                EE ee = flat_exp(env,ctx,cc,r,NULL);
                GC::lock();
                ret.r = ee.r;
                std::vector<EE> ees(3);
                ees[0].b = e0.b; ees[1].b = e1.b; ees[2].b = ee.b;
                ret.b = conj(env,b,Ctx(),ees);
              } else {
                addPathAnnotation(env, cc);
                ret.r = bind(env,ctx,r,cc);
                std::vector<EE> ees(2);
                ees[0].b = e0.b; ees[1].b = e1.b;
                ret.b = conj(env,b,Ctx(),ees);
                if (!ctx.neg)
                  env.map_insert(cc,ret);
              }
            }
          }
            GC::unlock();
            break;
            
          case BOT_AND:
          {
            if (r==constants().var_true) {
              Ctx nctx;
              nctx.neg = negArgs;
              nctx.b = negArgs ? C_NEG : C_ROOT;
              std::vector<Expression*> todo;
              todo.push_back(boe1);
              todo.push_back(boe0);
              while (!todo.empty()) {
                Expression* e_todo = todo.back();
                todo.pop_back();
                BinOp* e_bo = e_todo->dyn_cast<BinOp>();
                if (e_bo && e_bo->op()==BOT_AND) {
                  todo.push_back(e_bo->rhs());
                  todo.push_back(e_bo->lhs());
                } else {
                  (void) flat_exp(env,nctx,e_todo,constants().var_true,constants().var_true);
                }
              }
              ret.r = bind(env,ctx,r,constants().lit_true);
              break;
            } else {
              GC::lock();
              Call* c = aggregateAndOrOps(env, bo, negArgs, bot);
              KeepAlive ka(c);
              GC::unlock();
              ret = flat_exp(env,ctx,c,r,b);
              if (Id* id = ret.r()->dyn_cast<Id>()) {
                addCtxAnn(id->decl(), ctx.b);
              }
            }
            break;
          }
          case BOT_OR:
          {
            GC::lock();
            Call* c = aggregateAndOrOps(env, bo, negArgs, bot);
            KeepAlive ka(c);
            GC::unlock();
            ret = flat_exp(env,ctx,c,r,b);
            if (Id* id = ret.r()->dyn_cast<Id>()) {
              addCtxAnn(id->decl(), ctx.b);
            }
          }
            break;
          case BOT_RIMPL:
          {
            std::swap(boe0,boe1);
          }
            // fall through
          case BOT_IMPL:
          {
            if (ctx.b==C_ROOT && r==constants().var_true && boe0->type().ispar()) {
              bool b;
              {
                GCLock lock;
                b = eval_bool(env,boe0);
              }
              if (b) {
                Ctx nctx = ctx;
                nctx.neg = negArgs;
                nctx.b = negArgs ? C_NEG : C_ROOT;
                ret = flat_exp(env,nctx,boe1,constants().var_true,constants().var_true);
              } else {
                Ctx nctx = ctx;
                nctx.neg = negArgs;
                nctx.b = negArgs ? C_NEG : C_ROOT;
                ret = flat_exp(env,nctx,constants().lit_true,constants().var_true,constants().var_true);
              }
              break;
            }
            if (ctx.b==C_ROOT && r==constants().var_true && boe1->type().ispar()) {
              bool b;
              {
                GCLock lock;
                b = eval_bool(env,boe1);
              }
              if (b) {
                Ctx nctx = ctx;
                nctx.neg = negArgs;
                nctx.b = negArgs ? C_NEG : C_ROOT;
                ret = flat_exp(env,nctx,constants().lit_true,constants().var_true,constants().var_true);
                break;
              } else {
                Ctx nctx = ctx;
                nctx.neg = !negArgs;
                nctx.b = !negArgs ? C_NEG : C_ROOT;
                ret = flat_exp(env,nctx,boe0,constants().var_true,constants().var_true);
                break;
              }
            }
            GC::lock();
            std::vector<Expression*> args;
            ASTString id;
            if (ctx.neg) {
              std::vector<Expression*> bo_args(2);
              bo_args[0] = boe0;
              bo_args[1] = new UnOp(bo->loc(),UOT_NOT,boe1);
              bo_args[1]->type(boe1->type());
              id = constants().ids.forall;
              args.push_back(new ArrayLit(bo->loc(),bo_args));
              args[0]->type(Type::varbool(1));
            } else {
              std::vector<Expression*> clause_pos(1);
              clause_pos[0] = boe1;
              std::vector<Expression*> clause_neg(1);
              clause_neg[0] = boe0;
              args.push_back(new ArrayLit(boe1->loc().introduce(), clause_pos));
              Type t0 = boe1->type();
              t0.dim(1);
              args[0]->type(t0);
              args.push_back(new ArrayLit(boe0->loc().introduce(), clause_neg));
              Type t1 = boe0->type();
              t1.dim(1);
              args[1]->type(t1);
              id = constants().ids.clause;
            }
            ctx.neg = false;
            Call* c = new Call(bo->loc().introduce(),id,args);
            c->decl(env.orig->matchFn(env,c,false));
            if (c->decl()==NULL) {
              throw FlatteningError(env,c->loc(), "cannot find matching declaration");
            }
            c->type(c->decl()->rtype(env,args,false));
            KeepAlive ka(c);
            GC::unlock();
            ret = flat_exp(env,ctx,c,r,b);
            if (Id* id = ret.r()->dyn_cast<Id>()) {
              addCtxAnn(id->decl(),ctx.b);
            }
          }
            break;
          case BOT_EQUIV:
            if (ctx.neg) {
              ctx.neg = false;
              ctx.b = -ctx.b;
              bot = BOT_XOR;
              ctx0.b = ctx1.b = C_MIX;
              goto flatten_bool_op;
            } else {
              if (!boe1->type().isopt() && istrue(env, boe0)) {
                return flat_exp(env, ctx, boe1, r, b);
              }
              if (!boe0->type().isopt() && istrue(env, boe1)) {
                return flat_exp(env, ctx, boe0, r, b);
              }
              if (r && r==constants().var_true) {
                if (boe1->type().ispar() || boe1->isa<Id>())
                  std::swap(boe0,boe1);
                if (istrue(env,boe0)) {
                  return flat_exp(env,ctx1,boe1,r,b);
                } else if (isfalse(env,boe0)) {
                  ctx1.neg = true;
                  ctx1.b = -ctx1.b;
                  return flat_exp(env,ctx1,boe1,r,b);
                } else {
                  ctx0.b = C_MIX;
                  EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
                  if (istrue(env,e0.r())) {
                    return flat_exp(env,ctx1,boe1,r,b);
                  } else if (isfalse(env,e0.r())) {
                    ctx1.neg = true;
                    ctx1.b = -ctx1.b;
                    return flat_exp(env,ctx1,boe1,r,b);
                  } else {
                    Id* id = e0.r()->cast<Id>();
                    ctx1.b = C_MIX;
                    (void) flat_exp(env,ctx1,boe1,id->decl(),constants().var_true);
                    ret.b = bind(env,Ctx(),b,constants().lit_true);
                    ret.r = bind(env,Ctx(),r,constants().lit_true);
                  }
                }
                break;
              } else {
                ctx0.b = ctx1.b = C_MIX;
                goto flatten_bool_op;
              }
            }
          case BOT_XOR:
            if (ctx.neg) {
              ctx.neg = false;
              ctx.b = -ctx.b;
              bot = BOT_EQUIV;
            }
            ctx0.b = ctx1.b = C_MIX;
            goto flatten_bool_op;
          case BOT_LE:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_GQ;
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = +ctx0.b;
                ctx1.i = -ctx1.b;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = -ctx0.b;
                ctx1.i = +ctx1.b;
              }
            }
            goto flatten_bool_op;
          case BOT_LQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_GR;
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = +ctx0.b;
                ctx1.i = -ctx1.b;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = -ctx0.b;
                ctx1.i = +ctx1.b;
              }
            }
            goto flatten_bool_op;
          case BOT_GR:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_LQ;
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = -ctx0.b;
                ctx1.i = +ctx1.b;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = +ctx0.b;
                ctx1.i = -ctx1.b;
              }
            }
            goto flatten_bool_op;
          case BOT_GQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_LE;
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = -ctx0.b;
                ctx1.i = +ctx1.b;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = +ctx0.b;
                ctx1.i = -ctx1.b;
              }
            }
            goto flatten_bool_op;
          case BOT_EQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_NQ;
            }
            if (boe0->type().bt()==Type::BT_BOOL) {
              ctx0.b = ctx1.b = C_MIX;
            } else if (boe0->type().bt()==Type::BT_INT) {
              ctx0.i = ctx1.i = C_MIX;
            }
            goto flatten_bool_op;
          case BOT_NQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_EQ;
            }
            if (boe0->type().bt()==Type::BT_BOOL) {
              ctx0.b = ctx1.b = C_MIX;
            } else if (boe0->type().bt()==Type::BT_INT) {
              ctx0.i = ctx1.i = C_MIX;
            }
            goto flatten_bool_op;
          case BOT_IN:
          case BOT_SUBSET:
          case BOT_SUPERSET:
            ctx0.i = ctx1.i = C_MIX;
          flatten_bool_op:
          {
            bool inRootCtx = (ctx0.b==ctx1.b && ctx0.b==C_ROOT && b==constants().var_true);
            EE e0 = flat_exp(env,ctx0,boe0,NULL,inRootCtx ? b : NULL);
            EE e1 = flat_exp(env,ctx1,boe1,NULL,inRootCtx ? b : NULL);
            
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            
            std::vector<EE> ees(3);
            ees[0].b = e0.b; ees[1].b = e1.b;

            if (isfalse(env, e0.b()) || isfalse(env, e1.b())) {
              ees.resize(2);
              ret.r = conj(env,r,ctx,ees);
              break;
            }
            
            if (e0.r()->type().ispar() && e1.r()->type().ispar()) {
              GCLock lock;
              BinOp* bo_par = new BinOp(e->loc(),e0.r(),bot,e1.r());
              bo_par->type(Type::parbool());
              bool bo_val = eval_bool(env,bo_par);
              if (doubleNeg)
                bo_val = !bo_val;
              ees[2].b = constants().boollit(bo_val);
              ret.r = conj(env,r,ctx,ees);
              break;
            }
            
            if (e0.r()->type().bt()==Type::BT_INT && e1.r()->type().ispar() && e0.r()->isa<Id>() && (bot==BOT_IN || bot==BOT_SUBSET)) {
              VarDecl* vd = e0.r()->cast<Id>()->decl();
              Id* ident = vd->id();
              if (ctx.b==C_ROOT && r==constants().var_true) {
                if (vd->ti()->domain()==NULL) {
                  vd->ti()->domain(e1.r());
                } else {
                  GCLock lock;
                  IntSetVal* newdom = eval_intset(env,e1.r());
                  while (ident != NULL) {
                    bool changeDom = false;
                    if (ident->decl()->ti()->domain()) {
                      IntSetVal* domain = eval_intset(env,ident->decl()->ti()->domain());
                      IntSetRanges dr(domain);
                      IntSetRanges ibr(newdom);
                      Ranges::Inter<IntVal,IntSetRanges,IntSetRanges> i(dr,ibr);
                      IntSetVal* newibv = IntSetVal::ai(i);
                      if (domain->card() != newibv->card()) {
                        newdom = newibv;
                        changeDom = true;
                      }
                    } else {
                      changeDom = true;
                    }
                    if (ident->type().st()==Type::ST_PLAIN && newdom->size()==0) {
                      env.fail();
                    } else if (changeDom) {
                      ident->decl()->ti()->setComputedDomain(false);
                      ident->decl()->ti()->domain(new SetLit(Location().introduce(),newdom));
                      if (ident->decl()->e()==NULL && newdom->min()==newdom->max()) {
                        ident->decl()->e(IntLit::a(newdom->min()));
                      }
                    }
                    ident = ident->decl()->e() ? ident->decl()->e()->dyn_cast<Id>() : NULL;
                  }
                }
                ret.r = bind(env,ctx,r,constants().lit_true);
                break;
              } else if (vd->ti()->domain()!=NULL) {
                // check if current domain is already subsumed or falsified by this constraint
                GCLock lock;
                IntSetVal* check_dom = eval_intset(env,e1.r());
                IntSetVal* domain = eval_intset(env,ident->decl()->ti()->domain());
                {
                  IntSetRanges cdr(check_dom);
                  IntSetRanges dr(domain);
                  if (Ranges::subset(dr,cdr)) {
                    // the constraint is subsumed
                    ret.r = bind(env,ctx,r,constants().lit_true);
                    break;
                  }
                }
                if (vd->type().st()==Type::ST_PLAIN) {
                  // only for var int (for var set of int, subset can never fail because of the domain)
                  IntSetRanges cdr(check_dom);
                  IntSetRanges dr(domain);
                  if (Ranges::disjoint(cdr, dr)) {
                    // the constraint is false
                    ret.r = bind(env,ctx,r,constants().lit_false);
                    break;
                  }
                }
              }
            }
            
            std::vector<KeepAlive> args;
            ASTString callid;
            
            Expression* le0 = NULL;
            Expression* le1 = NULL;
            
            if (boe0->type().isint() && !boe0->type().isopt() && bot != BOT_IN) {
              le0 = get_linexp<IntLit>(e0.r());
            } else if (boe0->type().isfloat() && !boe0->type().isopt() && bot != BOT_IN) {
              le0 = get_linexp<FloatLit>(e0.r());
            }
            if (le0) {
              if (boe0->type().isint() && boe1->type().isint() && !boe1->type().isopt()) {
                le1 = get_linexp<IntLit>(e1.r());
              } else if (boe0->type().isfloat() && boe1->type().isfloat() && !boe1->type().isopt()) {
                le1 = get_linexp<FloatLit>(e1.r());
              }
            }
            if (le1) {
              if (boe0->type().isint()) {
                flatten_linexp_binop<IntLit>(env,ctx,r,b,ret,le0,le1,bot,doubleNeg,ees,args,callid);
              } else {
                flatten_linexp_binop<FloatLit>(env,ctx,r,b,ret,le0,le1,bot,doubleNeg,ees,args,callid);
              }
            } else {
              if (bo->decl()==NULL) {
                switch (bot) {
                  case BOT_GR:
                    std::swap(e0,e1);
                    bot = BOT_LE;
                    break;
                  case BOT_GQ:
                    std::swap(e0,e1);
                    bot = BOT_LQ;
                    break;
                  default:
                    break;
                }
              }
              args.push_back(e0.r);
              args.push_back(e1.r);
            }
            
            if (args.size() > 0) {
              GC::lock();
              
              if (callid=="") {
                if (bo->decl()) {
                  callid = bo->decl()->id();
                } else {
                  callid = opToBuiltin(bo,bot);
                }
              }
              
              std::vector<Expression*> args_e(args.size());
              for (unsigned int i=args.size(); i--;)
                args_e[i] = args[i]();
              Call* cc = new Call(e->loc().introduce(),callid,args_e);
              cc->decl(env.orig->matchFn(env,cc->id(),args_e,false));
              if (cc->decl()==NULL) {
                throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
              }
              cc->type(cc->decl()->rtype(env,args_e,false));

              // add defines_var annotation if applicable
              Id* assignTo = NULL;
              if (bot==BOT_EQ && ctx.b == C_ROOT) {
                if (le0 && le0->isa<Id>()) {
                  assignTo = le0->cast<Id>();
                } else if (le1 && le1->isa<Id>()) {
                  assignTo = le1->cast<Id>();
                }
                if (assignTo) {
                  cc->addAnnotation(definesVarAnn(assignTo));
                  assignTo->decl()->flat()->addAnnotation(constants().ann.is_defined_var);
                }
              }

              EnvI::Map::iterator cit = env.map_find(cc);
              if (cit != env.map_end()) {
                ees[2].b = cit->second.r();
                if (doubleNeg) {
                  Type t = ees[2].b()->type();
                  ees[2].b = new UnOp(Location().introduce(),UOT_NOT,ees[2].b());
                  ees[2].b()->type(t);
                }
                if (Id* id = ees[2].b()->dyn_cast<Id>()) {
                  addCtxAnn(id->decl(),ctx.b);
                }
                ret.r = conj(env,r,ctx,ees);
                GC::unlock();
              } else {
                bool singleExp = true;
                for (unsigned int i=0; i<ees.size(); i++) {
                  if (!istrue(env,ees[i].b())) {
                    singleExp = false;
                    break;
                  }
                }
                KeepAlive ka(cc);
                GC::unlock();
                if (singleExp) {
                  if (doubleNeg) {
                    ctx.b = -ctx.b;
                    ctx.neg = !ctx.neg;
                  }
                  ret.r = flat_exp(env,ctx,cc,r,NULL).r;
                } else {
                  ees[2].b = flat_exp(env,Ctx(),cc,NULL,NULL).r;
                  if (doubleNeg) {
                    GCLock lock;
                    Type t = ees[2].b()->type();
                    ees[2].b = new UnOp(Location().introduce(),UOT_NOT,ees[2].b());
                    ees[2].b()->type(t);
                  }
                  if (Id* id = ees[2].b()->dyn_cast<Id>()) {
                    addCtxAnn(id->decl(),ctx.b);
                  }
                  ret.r = conj(env,r,ctx,ees);
                }
              }
            } else {
              ret.r = conj(env,r,ctx,ees);
            }
          }
            break;
            
          case BOT_PLUSPLUS:
          {
            std::vector<EE> ee(2);
            EE eev = flat_exp(env,ctx,boe0,NULL,NULL);
            ee[0] = eev;
            ArrayLit* al;
            if (eev.r()->isa<ArrayLit>()) {
              al = eev.r()->cast<ArrayLit>();
            } else {
              Id* id = eev.r()->cast<Id>();
              if (id->decl()==NULL) {
                throw InternalError("undefined identifier");
              }
              if (id->decl()->e()==NULL) {
                throw InternalError("array without initialiser not supported");
              }
              al = follow_id(id)->cast<ArrayLit>();
            }
            ArrayLit* al0 = al;
            eev = flat_exp(env,ctx,boe1,NULL,NULL);
            ee[1] = eev;
            if (eev.r()->isa<ArrayLit>()) {
              al = eev.r()->cast<ArrayLit>();
            } else {
              Id* id = eev.r()->cast<Id>();
              if (id->decl()==NULL) {
                throw InternalError("undefined identifier");
              }
              if (id->decl()->e()==NULL) {
                throw InternalError("array without initialiser not supported");
              }
              al = follow_id(id)->cast<ArrayLit>();
            }
            ArrayLit* al1 = al;
            std::vector<Expression*> v(al0->size()+al1->size());
            for (unsigned int i=al0->size(); i--;)
              v[i] = (*al0)[i];
            for (unsigned int i=al1->size(); i--;)
              v[al0->size()+i] = (*al1)[i];
            GCLock lock;
            ArrayLit* alret = new ArrayLit(e->loc(),v);
            alret->type(e->type());
            ret.b = conj(env,b,Ctx(),ee);
            ret.r = bind(env,ctx,r,alret);
          }
            break;            
        }
      }
      break;
    case Expression::E_UNOP:
      {
        CallStackItem _csi(env,e);
        UnOp* uo = e->cast<UnOp>();
        switch (uo->op()) {
        case UOT_NOT:
          {
            Ctx nctx = ctx;
            nctx.b = -nctx.b;
            nctx.neg = !nctx.neg;
            ret = flat_exp(env,nctx,uo->e(),r,b);
          }
          break;
        case UOT_PLUS:
          ret = flat_exp(env,ctx,uo->e(),r,b);
          break;
        case UOT_MINUS:
          {
            GC::lock();
            if (UnOp* uo_inner = uo->e()->dyn_cast<UnOp>()) {
              if (uo_inner->op()==UOT_MINUS) {
                ret = flat_exp(env,ctx,uo_inner->e(),r,b);
                break;
              }
            }
            Expression* zero;
            if (uo->e()->type().bt()==Type::BT_INT)
              zero = IntLit::a(0);
            else
              zero = FloatLit::a(0.0);
            BinOp* bo = new BinOp(Location().introduce(),zero,BOT_MINUS,uo->e());
            bo->type(uo->type());
            KeepAlive ka(bo);
            GC::unlock();
            ret = flat_exp(env,ctx,ka(),r,b);
          }
          break;
        default: break;
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        FunctionI* decl = env.orig->matchFn(env,c,false);
        if (decl == NULL) {
          throw InternalError("undeclared function or predicate "
                              +c->id().str());
        }
        
        if (decl->params().size()==1) {
          if (Call* call_body = Expression::dyn_cast<Call>(decl->e())) {
            if (call_body->n_args()==1 && Expression::equal(call_body->arg(0),decl->params()[0]->id())) {
              c->id(call_body->id());
              c->decl(call_body->decl());
              decl = c->decl();
              for (ExpressionSetIter esi = call_body->ann().begin(); esi != call_body->ann().end(); ++esi) {
                c->addAnnotation(*esi);
              }
            }
          }
        }
        
        Ctx nctx = ctx;
        nctx.neg = false;
        ASTString cid = c->id();
        CallStackItem _csi(env,e);

        if (decl->e()==NULL) {
          if (cid == constants().ids.forall) {
            nctx.b = +nctx.b;
            if (ctx.neg) {
              ctx.neg = false;
              nctx.neg = true;
              cid = constants().ids.exists;
            }
          } else if (cid == constants().ids.exists) {
            nctx.b = +nctx.b;
            if (ctx.neg) {
              ctx.neg = false;
              nctx.neg = true;
              cid = constants().ids.forall;
            }
          } else if (cid == constants().ids.bool2int) {
            if (ctx.neg) {
              ctx.neg = false;
              nctx.neg = true;
              nctx.b = -ctx.i;
            } else {
              nctx.b = ctx.i;
            }
          } else if (cid == constants().ids.assert || cid == constants().ids.trace) {
            if (cid == constants().ids.assert && c->n_args()==2) {
              (void) decl->_builtins.b(env,c);
              ret = flat_exp(env,ctx,constants().lit_true,r,b);
            } else {
              KeepAlive callres = decl->_builtins.e(env,c);
              ret = flat_exp(env,ctx,callres(),r,b);
              // This is all we need to do for assert, so break out of the E_CALL
            }
            break;
          }
        } else if (ctx.b==C_ROOT && decl->e()->isa<BoolLit>() && eval_bool(env,decl->e())) {
          bool allBool = true;
          for (unsigned int i=0; i<c->n_args(); i++) {
            if (c->arg(i)->type().bt()!=Type::BT_BOOL) {
              allBool = false;
              break;
            }
          }
          if (allBool) {
            ret.r = bind(env,ctx,r,constants().lit_true);
            ret.b = bind(env,ctx,b,constants().lit_true);
            break;
          }
        }

        if (ctx.b==C_ROOT && decl->e()==NULL &&
            cid == constants().ids.forall && r==constants().var_true) {
          ret.b = bind(env,ctx,b,constants().lit_true);
          EE flat_al = flat_exp(env,Ctx(),c->arg(0),constants().var_ignore,constants().var_true);
          ArrayLit* al = follow_id(flat_al.r())->cast<ArrayLit>();
          nctx.b = C_ROOT;
          for (unsigned int i=0; i<al->size(); i++)
            (void) flat_exp(env,nctx,(*al)[i],r,b);
          ret.r = bind(env,ctx,r,constants().lit_true);
        } else {
          if (decl->e() && decl->params().size()==1 && decl->e()->isa<Id>() &&
              decl->params()[0]->ti()->domain()==NULL &&
              decl->e()->cast<Id>()->decl() == decl->params()[0]) {
            Expression* arg = c->arg(0);
            for (ExpressionSetIter esi = decl->e()->ann().begin(); esi != decl->e()->ann().end(); ++esi) {
              arg->addAnnotation(*esi);
            }
            for (ExpressionSetIter esi = c->ann().begin(); esi != c->ann().end(); ++esi) {
              arg->addAnnotation(*esi);
            }
            ret = flat_exp(env, ctx, c->arg(0), r, b);
            break;
          }
          
          std::vector<EE> args_ee(c->n_args());
          bool isPartial = false;

          if (cid == constants().ids.lin_exp && c->type().isint()) {
            // Linear expressions need special context handling:
            // the context of a variable expression depends on the corresponding coefficient
            
            // flatten the coefficient array
            Expression* tmp = follow_id_to_decl(c->arg(0));
            ArrayLit* coeffs;
            if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
              tmp = vd->id();
            {
              CallArgItem cai(env);
              args_ee[0] = flat_exp(env,nctx,tmp,NULL,NULL);
              isPartial |= isfalse(env, args_ee[0].b());
              coeffs = eval_array_lit(env, args_ee[0].r());
            }

            ArrayLit* vars = eval_array_lit(env, c->arg(1));
            if (vars->flat()) {
              args_ee[1].r = vars;
              args_ee[1].b = constants().var_true;
            } else {
              CallArgItem cai(env);
              CallStackItem _csi(env,c->arg(1));
              std::vector<EE> elems_ee(vars->size());
              for (unsigned int i=vars->size(); i--;) {
                Ctx argctx = nctx;
                argctx.i = eval_int(env,(*coeffs)[i])<0 ? -nctx.i : +nctx.i;
                elems_ee[i] = flat_exp(env,argctx,(*vars)[i],NULL,NULL);
              }
              std::vector<Expression*> elems(elems_ee.size());
              for (unsigned int i=elems.size(); i--;)
                elems[i] = elems_ee[i].r();
              KeepAlive ka;
              {
                GCLock lock;
                ArrayLit* alr = new ArrayLit(Location().introduce(),elems);
                alr->type(vars->type());
                alr->flat(true);
                ka = alr;
              }
              args_ee[1].r = ka();
              args_ee[1].b = conj(env,b,Ctx(),elems_ee);
            }

            {
              Expression* constant = follow_id_to_decl(c->arg(2));
              if (VarDecl* vd = constant->dyn_cast<VarDecl>())
                constant = vd->id();
              CallArgItem cai(env);
              args_ee[2] = flat_exp(env,nctx,constant,NULL,NULL);
              isPartial |= isfalse(env, args_ee[2].b());
            }

          } else {
            bool mixContext = decl->e()!=NULL ||
              (cid != constants().ids.forall && cid != constants().ids.exists && cid != constants().ids.bool2int &&
               cid != constants().ids.sum && cid != "assert");
            for (unsigned int i=c->n_args(); i--;) {
              Ctx argctx = nctx;
              if (mixContext) {
                if (cid==constants().ids.clause) {
                  argctx.b = (i==0 ? +nctx.b : -nctx.b);
                } else if (c->arg(i)->type().bt()==Type::BT_BOOL) {
                  argctx.b = C_MIX;
                } else if (c->arg(i)->type().bt()==Type::BT_INT) {
                  argctx.i = C_MIX;
                }
              }
              Expression* tmp = follow_id_to_decl(c->arg(i));
              if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
                tmp = vd->id();
              CallArgItem cai(env);
              args_ee[i] = flat_exp(env,argctx,tmp,NULL,NULL);
              isPartial |= isfalse(env, args_ee[i].b());
            }
          }
          if (isPartial && c->type().isbool() && !c->type().isopt()) {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            args_ee.resize(1);
            args_ee[0] = EE(NULL, constants().lit_false);
            ret.r = conj(env, r, ctx, args_ee);
            break;
          }

          std::vector<KeepAlive> args;
          if (decl->e()==NULL && (cid == constants().ids.exists || cid == constants().ids.clause)) {

            std::vector<KeepAlive> pos_alv;
            std::vector<KeepAlive> neg_alv;
            for (unsigned int i=0; i<args_ee.size(); i++) {
              std::vector<KeepAlive>& local_pos = i==0 ? pos_alv : neg_alv;
              std::vector<KeepAlive>& local_neg = i==1 ? pos_alv : neg_alv;
              ArrayLit* al = follow_id(args_ee[i].r())->cast<ArrayLit>();
              std::vector<KeepAlive> alv;
              for (unsigned int i=0; i<al->size(); i++) {
                if (Call* sc = same_call((*al)[i],cid)) {
                  if (sc->id()==constants().ids.clause) {
                    alv.push_back(sc);
                  } else {
                    GCLock lock;
                    ArrayLit* sc_c = eval_array_lit(env,sc->arg(0));
                    for (unsigned int j=0; j<sc_c->size(); j++) {
                      alv.push_back((*sc_c)[j]);
                    }
                  }
                } else {
                  alv.push_back((*al)[i]);
                }
              }

              for (unsigned int j=0; j<alv.size(); j++) {
                Call* neg_call = same_call(alv[j](),constants().ids.bool_eq);
                if (neg_call &&
                    Expression::equal(neg_call->arg(1),constants().lit_false)) {
                  local_neg.push_back(neg_call->arg(0));
                } else {
                  Call* clause = same_call(alv[j](),constants().ids.clause);
                  if (clause) {
                    ArrayLit* clause_pos = eval_array_lit(env,clause->arg(0));
                    for (unsigned int k=0; k<clause_pos->size(); k++) {
                      local_pos.push_back((*clause_pos)[k]);
                    }
                    ArrayLit* clause_neg = eval_array_lit(env,clause->arg(1));
                    for (unsigned int k=0; k<clause_neg->size(); k++) {
                      local_neg.push_back((*clause_neg)[k]);
                    }
                  } else {
                    local_pos.push_back(alv[j]);
                  }
                }
              }
            }
            bool subsumed = remove_dups(pos_alv,false);
            subsumed = subsumed || remove_dups(neg_alv,true);
            subsumed = subsumed || contains_dups(pos_alv, neg_alv);
            if (subsumed) {
              ret.b = bind(env,Ctx(),b,constants().lit_true);
              ret.r = bind(env,ctx,r,constants().lit_true);
              return ret;
            }
            if (neg_alv.empty()) {
              if (pos_alv.size()==0) {
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                ret.r = bind(env,ctx,r,constants().lit_false);
                return ret;
              } else if (pos_alv.size()==1) {
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                ret.r = bind(env,ctx,r,pos_alv[0]());
                return ret;
              }
              GCLock lock;
              ArrayLit* nal = new ArrayLit(Location().introduce(),toExpVec(pos_alv));
              nal->type(Type::varbool(1));
              args.push_back(nal);
              cid = constants().ids.exists;
            } else {
              GCLock lock;
              ArrayLit* pos_al = new ArrayLit(Location().introduce(),toExpVec(pos_alv));
              pos_al->type(Type::varbool(1));
              ArrayLit* neg_al = new ArrayLit(Location().introduce(),toExpVec(neg_alv));
              neg_al->type(Type::varbool(1));
              cid = constants().ids.clause;
              args.push_back(pos_al);
              args.push_back(neg_al);
            }

          } else if (decl->e()==NULL && cid == constants().ids.forall) {
            ArrayLit* al = follow_id(args_ee[0].r())->cast<ArrayLit>();
            std::vector<KeepAlive> alv;
            for (unsigned int i=0; i<al->size(); i++) {
              if (Call* sc = same_call((*al)[i],cid)) {
                GCLock lock;
                ArrayLit* sc_c = eval_array_lit(env,sc->arg(0));
                for (unsigned int j=0; j<sc_c->size(); j++) {
                  alv.push_back((*sc_c)[j]);
                }
              } else {
                alv.push_back((*al)[i]);
              }
            }
            bool subsumed = remove_dups(alv,true);
            if (subsumed) {
              ret.b = bind(env,Ctx(),b,constants().lit_true);
              ret.r = bind(env,ctx,r,constants().lit_false);
              return ret;
            }
            if (alv.size()==0) {
              ret.b = bind(env,Ctx(),b,constants().lit_true);
              ret.r = bind(env,ctx,r,constants().lit_true);
              return ret;
            } else if (alv.size()==1) {
              ret.b = bind(env,Ctx(),b,constants().lit_true);
              ret.r = bind(env,ctx,r,alv[0]());
              return ret;
            }
            GCLock lock;
            ArrayLit* nal = new ArrayLit(al->loc(),toExpVec(alv));
            nal->type(al->type());
            args.push_back(nal);
          } else if (decl->e()==NULL && (cid == constants().ids.lin_exp || cid==constants().ids.sum)) {
            if (e->type().isint()) {
              flatten_linexp_call<IntLit>(env,ctx,nctx,cid,c,ret,b,r,args_ee,args);
            } else {
              flatten_linexp_call<FloatLit>(env,ctx,nctx,cid,c,ret,b,r,args_ee,args);
            }
            if (args.size()==0)
              break;
          } else {
            for (unsigned int i=0; i<args_ee.size(); i++)
              args.push_back(args_ee[i].r());
          }
          KeepAlive cr;
          {
            GCLock lock;
            std::vector<Expression*> e_args = toExpVec(args);
            Call* cr_c = new Call(c->loc().introduce(),cid,e_args);
            decl = env.orig->matchFn(env,cr_c,false);
            if (decl==NULL)
              throw FlatteningError(env,cr_c->loc(), "cannot find matching declaration");
            cr_c->type(decl->rtype(env,e_args,false));
            assert(decl);
            cr_c->decl(decl);
            cr = cr_c;
          }
          EnvI::Map::iterator cit = env.map_find(cr());
          if (cit != env.map_end()) {
            ret.b = bind(env,Ctx(),b,env.ignorePartial ? constants().lit_true : cit->second.b());
            ret.r = bind(env,ctx,r,cit->second.r());
          } else {
            for (unsigned int i=0; i<decl->params().size(); i++) {
              if (Expression* dom = decl->params()[i]->ti()->domain()) {
                if (!dom->isa<TIId>()) {
                  // May have to constrain actual argument
                  if (args[i]()->type().bt() == Type::BT_INT) {
                    GCLock lock;
                    IntSetVal* isv = eval_intset(env,dom);
                    BinOpType bot;
                    bool needToConstrain;
                    if (args[i]()->type().st() == Type::ST_SET) {
                      bot = BOT_SUBSET;
                      needToConstrain = true;
                    } else {
                      bot = BOT_IN;
                      if (args[i]()->type().dim() > 0) {
                        needToConstrain = true;
                      } else {
                        IntBounds ib = compute_int_bounds(env,args[i]());
                        needToConstrain = !ib.valid || isv->size()==0 || ib.l < isv->min(0) || ib.u > isv->max(isv->size()-1);
                      }
                    }
                    if (needToConstrain) {
                      GCLock lock;
                      Expression* domconstraint;
                      if (args[i]()->type().dim() > 0) {
                        std::vector<Expression*> domargs(2);
                        domargs[0] = args[i]();
                        domargs[1] = dom;
                        Call* c = new Call(Location().introduce(),"var_dom",domargs);
                        c->type(Type::varbool());
                        c->decl(env.orig->matchFn(env,c,false));
                        if (c->decl()==NULL)
                          throw InternalError("no matching declaration found for var_dom");
                        domconstraint = c;
                      } else {
                        domconstraint = new BinOp(Location().introduce(),args[i](),bot,dom);
                      }
                      domconstraint->type(args[i]()->type().ispar() ? Type::parbool() : Type::varbool());
                      if (ctx.b == C_ROOT) {
                        (void) flat_exp(env, Ctx(), domconstraint, constants().var_true, constants().var_true);
                      } else {
                        EE ee = flat_exp(env, Ctx(), domconstraint, NULL, constants().var_true);
                        ee.b = ee.r;
                        args_ee.push_back(ee);
                      }
                    }
                  } else if (args[i]()->type().bt() == Type::BT_FLOAT) {
                    GCLock lock;

                    FloatSetVal* fsv = eval_floatset(env,dom);
                    bool needToConstrain;
                    if (args[i]()->type().dim() > 0) {
                      needToConstrain = true;
                    } else {
                      FloatBounds fb = compute_float_bounds(env,args[i]());
                      needToConstrain = !fb.valid || fsv->size()==0 || fb.l < fsv->min(0) || fb.u > fsv->max(fsv->size()-1);
                    }

                    if (needToConstrain) {
                      GCLock lock;
                      Expression* domconstraint;
                      if (args[i]()->type().dim() > 0) {
                        std::vector<Expression*> domargs(2);
                        domargs[0] = args[i]();
                        domargs[1] = dom;
                        Call* c = new Call(Location().introduce(),"var_dom",domargs);
                        c->type(Type::varbool());
                        c->decl(env.orig->matchFn(env,c,false));
                        if (c->decl()==NULL)
                          throw InternalError("no matching declaration found for var_dom");
                        domconstraint = c;
                      } else {
                        domconstraint = new BinOp(Location().introduce(),args[i](),BOT_IN,dom);
                      }
                      domconstraint->type(args[i]()->type().ispar() ? Type::parbool() : Type::varbool());
                      if (ctx.b == C_ROOT) {
                        (void) flat_exp(env, Ctx(), domconstraint, constants().var_true, constants().var_true);
                      } else {
                        EE ee = flat_exp(env, Ctx(), domconstraint, NULL, constants().var_true);
                        ee.b = ee.r;
                        args_ee.push_back(ee);
                      }
                    }
                  } else if (args[i]()->type().bt() == Type::BT_BOT) {
                    // Nothing to be done for empty arrays/sets
                  } else {
                    throw EvalError(env,decl->params()[i]->loc(),"domain restrictions other than int and float not supported yet");
                  }
                }
              }
            }
            if (cr()->type().isbool() &&  !cr()->type().ispar() && !cr()->type().isopt() && (ctx.b != C_ROOT || r != constants().var_true)) {
              std::vector<Type> argtypes(args.size());
              for (unsigned int i=0; i<args.size(); i++)
                argtypes[i] = args[i]()->type();
              argtypes.push_back(Type::varbool());
              GCLock lock;
              ASTString r_cid = env.reifyId(cid);
              FunctionI* reif_decl = env.orig->matchFn(env, r_cid, argtypes, false);
              if (reif_decl && reif_decl->e()) {
                addPathAnnotation(env, reif_decl->e());
                VarDecl* reif_b;
                if (r==NULL || (r != NULL && r->e() != NULL)) {
                  reif_b = newVarDecl(env, Ctx(), new TypeInst(Location().introduce(),Type::varbool()), NULL, NULL, NULL);
                  if (reif_b->ti()->domain()) {
                    if (reif_b->ti()->domain() == constants().lit_true) {
                      bind(env,ctx,r,constants().lit_true);
                      r = constants().var_true;
                      ctx.b = C_ROOT;
                      goto call_nonreif;
                    } else {
                      std::vector<Expression*> args_e(args.size()+1);
                      for (unsigned int i=0; i<args.size(); i++)
                        args_e[i] = args[i]();
                      args_e[args.size()] = constants().lit_false;
                      Call* reif_call = new Call(Location().introduce(), r_cid, args_e);
                      reif_call->type(Type::varbool());
                      reif_call->decl(reif_decl);
                      flat_exp(env, Ctx(), reif_call, constants().var_true, constants().var_true);
                      args_ee.push_back(EE(NULL,constants().lit_false));
                      ret.r = conj(env,r,ctx,args_ee);
                      ret.b = bind(env,ctx,b,constants().lit_true);
                      return ret;
                    }
                  }
                } else {
                  reif_b = r;
                }
                // Annotate cr() with getPath()
                addPathAnnotation(env, cr());
                reif_b->e(cr());
                if (r != NULL && r->e() != NULL) {
                  Ctx reif_ctx;
                  reif_ctx.neg = ctx.neg;
                  bind(env,reif_ctx,r,reif_b->id());
                }
                env.vo_add_exp(reif_b);
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                args_ee.push_back(EE(NULL,reif_b->id()));
                ret.r = conj(env,NULL,ctx,args_ee);
                env.map_insert(cr(),ret);
                return ret;
              }
            }
          call_nonreif:
            if ( (cr()->type().ispar() && !cr()->type().isann()) || decl->e()==NULL) {
              Call* cr_c = cr()->cast<Call>();
              /// All builtins are total
              std::vector<Type> argt(cr_c->n_args());
              for (unsigned int i=argt.size(); i--;)
                argt[i] = cr_c->arg(i)->type();
              Type callt = decl->rtype(env,argt,false);
              if (callt.ispar() && callt.bt()!=Type::BT_ANN) {
                GCLock lock;
                try {
                  ret.r = bind(env,ctx,r,eval_par(env,cr_c));
                ret.b = conj(env,b,Ctx(),args_ee);
                } catch (ResultUndefinedError&) {
                  ret.r = createDummyValue(env, cr_c->type());
                  ret.b = bind(env,Ctx(),b,constants().lit_false);
                  return ret;
                }
                // Do not insert into map, since par results will quickly become
                // garbage anyway and then disappear from the map
              } else if (decl->_builtins.e) {
                KeepAlive callres;
                {
                  GCLock lock;
                  callres = decl->_builtins.e(env,cr_c);
                }
                EE res = flat_exp(env,ctx,callres(),r,b);
                args_ee.push_back(res);
                ret.b = conj(env,b,Ctx(),args_ee);
                addPathAnnotation(env, res.r());
                ret.r = bind(env,ctx,r,res.r());
                if (!ctx.neg && !cr_c->type().isann())
                  env.map_insert(cr_c,ret);
              } else {
                ret.b = conj(env,b,Ctx(),args_ee);
                addPathAnnotation(env, cr_c);
                ret.r = bind(env,ctx,r,cr_c);
                if (!ctx.neg && !cr_c->type().isann())
                  env.map_insert(cr_c,ret);
              }
            } else {
              std::vector<KeepAlive> previousParameters(decl->params().size());
              for (unsigned int i=decl->params().size(); i--;) {
                VarDecl* vd = decl->params()[i];
                previousParameters[i] = vd->e();
                vd->flat(vd);
                vd->e(args[i]());
              }
              
              if (decl->e()->type().isbool() && !decl->e()->type().isopt()) {
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                if (ctx.b==C_ROOT && r==constants().var_true) {
                  (void) flat_exp(env,Ctx(),decl->e(),r,constants().var_true);
                } else {
                  Ctx nctx;
                  if (!isTotal(decl)) {
                    nctx = ctx;
                    nctx.neg = false;
                  }
                  EE ee = flat_exp(env,nctx,decl->e(),NULL,constants().var_true);
                  ee.b = ee.r;
                  args_ee.push_back(ee);
                }
                ret.r = conj(env,r,ctx,args_ee);
              } else {
                if (isTotal(decl)) {
                  EE ee = flat_exp(env,Ctx(),decl->e(),r,constants().var_true);
                  ret.r = bind(env,ctx,r,ee.r());
                } else {
                  ret = flat_exp(env,ctx,decl->e(),r,NULL);
                  args_ee.push_back(ret);
                  if (decl->e()->type().dim() > 0) {
                    ArrayLit* al = follow_id(ret.r())->cast<ArrayLit>();
                    assert(al->dims() == decl->e()->type().dim());
                    for (unsigned int i=0; i<decl->ti()->ranges().size(); i++) {
                      if (decl->ti()->ranges()[i]->domain() &&
                          !decl->ti()->ranges()[i]->domain()->isa<TIId>()) {
                        GCLock lock;
                        IntSetVal* isv = eval_intset(env, decl->ti()->ranges()[i]->domain());
                        if (al->min(i) != isv->min() || al->max(i) != isv->max()) {
                          EE ee;
                          ee.b = constants().lit_false;
                          args_ee.push_back(ee);
                        }
                      }
                    }
                  }
                  if (decl->ti()->domain() && !decl->ti()->domain()->isa<TIId>()) {
                    BinOpType bot;
                    if (ret.r()->type().st() == Type::ST_SET) {
                      bot = BOT_SUBSET;
                    } else {
                      bot = BOT_IN;
                    }
                    
                    KeepAlive domconstraint;
                    if (decl->e()->type().dim() > 0) {
                      GCLock lock;
                      std::vector<Expression*> domargs(2);
                      domargs[0] = ret.r();
                      domargs[1] = decl->ti()->domain();
                      Call* c = new Call(Location().introduce(),"var_dom",domargs);
                      c->type(Type::varbool());
                      c->decl(env.orig->matchFn(env,c,false));
                      if (c->decl()==NULL)
                        throw InternalError("no matching declaration found for var_dom");
                      domconstraint = c;
                    } else {
                      GCLock lock;
                      domconstraint = new BinOp(Location().introduce(),ret.r(),bot,decl->ti()->domain());
                    }
                    domconstraint()->type(ret.r()->type().ispar() ? Type::parbool() : Type::varbool());
                    if (ctx.b == C_ROOT) {
                      (void) flat_exp(env, Ctx(), domconstraint(), constants().var_true, constants().var_true);
                    } else {
                      EE ee = flat_exp(env, Ctx(), domconstraint(), NULL, constants().var_true);
                      ee.b = ee.r;
                      args_ee.push_back(ee);
                    }
                  }
                }
                ret.b = conj(env,b,Ctx(),args_ee);
              }
              if (!ctx.neg && !cr()->type().isann())
                env.map_insert(cr(),ret);

              // Restore previous mapping
              for (unsigned int i=decl->params().size(); i--;) {
                VarDecl* vd = decl->params()[i];
                vd->e(previousParameters[i]());
                vd->flat(vd->e() ? vd : NULL);
              }
            }
          }
        }
      }
      break;
    case Expression::E_VARDECL:
      {
        CallStackItem _csi(env,e);
        GCLock lock;
        if (ctx.b != C_ROOT)
          throw FlatteningError(env,e->loc(), "not in root context");
        VarDecl* v = e->cast<VarDecl>();
        VarDecl* it = v->flat();
        if (it==NULL) {
          TypeInst* ti = eval_typeinst(env,v);
          bool reuseVarId = v->type().isann() || ( v->toplevel() && v->id()->idn()==-1 && v->id()->v().c_str()[0]!='\'' && v->id()->v().c_str()[0]!='_' );
          VarDecl* vd = newVarDecl(env, ctx, ti, reuseVarId ? v->id() : NULL, v, NULL);
          v->flat(vd);
          Ctx nctx;
          if (v->e() && v->e()->type().bt() == Type::BT_BOOL)
            nctx.b = C_MIX;
          if (v->e()) {
            (void) flat_exp(env,nctx,v->e(),vd,constants().var_true);
            if (v->e()->type().dim() > 0) {
              Expression* ee = follow_id_to_decl(vd->e());
              if (ee->isa<VarDecl>())
                ee = ee->cast<VarDecl>()->e();
              assert(ee && ee->isa<ArrayLit>());
              ArrayLit* al = ee->cast<ArrayLit>();
              if (vd->ti()->domain()) {
                for (unsigned int i=0; i<al->size(); i++) {
                  if (Id* ali_id = (*al)[i]->dyn_cast<Id>()) {
                    if (ali_id->decl()->ti()->domain()==NULL) {
                      ali_id->decl()->ti()->domain(vd->ti()->domain());
                    }
                  }
                }
              }
            }
          }

          ret.r = bind(env,Ctx(),r,vd->id());
        } else {
          ret.r = bind(env,Ctx(),r,it);
        }
        ret.b = bind(env,Ctx(),b,constants().lit_true);
      }
      break;
    case Expression::E_LET:
      {
        CallStackItem _csi(env,e);
        Let* let = e->cast<Let>();
        GC::mark();
        std::vector<EE> cs;
        std::vector<KeepAlive> flatmap;
        let->pushbindings();
        for (unsigned int i=0; i<let->let().size(); i++) {
          Expression* le = let->let()[i];
          if (VarDecl* vd = le->dyn_cast<VarDecl>()) {
            Expression* let_e = NULL;
            if (vd->e()) {
              Ctx nctx = ctx;
              nctx.neg = false;
              if (vd->e()->type().bt()==Type::BT_BOOL)
                nctx.b = C_MIX;

              EE ee = flat_exp(env,nctx,vd->e(),NULL,NULL);
              let_e = ee.r();
              cs.push_back(ee);
              if (vd->ti()->domain() != NULL) {
                GCLock lock;
                std::vector<Expression*> domargs(2);
                domargs[0] = ee.r();
                if (vd->ti()->type().isfloat()) {
                  FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
                  if (fsv->size()==1) {
                    domargs[1] = FloatLit::a(fsv->min());
                    domargs.push_back(FloatLit::a(fsv->max()));
                  } else {
                    domargs[1] = vd->ti()->domain();
                  }
                } else {
                  domargs[1] = vd->ti()->domain();
                }
                Call* c = new Call(vd->ti()->loc().introduce(),"var_dom",domargs);
                c->type(Type::varbool());
                c->decl(env.orig->matchFn(env,c,false));
                if (c->decl()==NULL)
                  throw InternalError("no matching declaration found for var_dom");
                VarDecl* b_b = (nctx.b==C_ROOT && b==constants().var_true) ? b : NULL;
                VarDecl* r_r = (nctx.b==C_ROOT && b==constants().var_true) ? b : NULL;
                ee = flat_exp(env, nctx, c, r_r, b_b);
                cs.push_back(ee);
                ee.b = ee.r;
                cs.push_back(ee);
              }
              if (vd->type().dim() > 0) {
                checkIndexSets(env, vd, let_e);
              }
            } else {
              if ((ctx.b==C_NEG || ctx.b==C_MIX) && !vd->ann().contains(constants().ann.promise_total)) {
                CallStackItem csi_vd(env, vd);
                throw FlatteningError(env,vd->loc(),
                                      "free variable in non-positive context");
              }
              CallStackItem csi_vd(env, vd);
              GCLock lock;
              TypeInst* ti = eval_typeinst(env,vd);
              VarDecl* nvd = newVarDecl(env, ctx, ti, NULL, vd, NULL);
              let_e = nvd->id();
            }
            vd->e(let_e);
            flatmap.push_back(vd->flat());
            if (Id* id = Expression::dyn_cast<Id>(let_e)) {
              vd->flat(id->decl());
            } else {
              vd->flat(vd);
            }
          } else {
            if (ctx.b==C_ROOT || le->ann().contains(constants().ann.promise_total)) {
              (void) flat_exp(env,Ctx(),le,constants().var_true,constants().var_true);
            } else {
              EE ee = flat_exp(env,ctx,le,NULL,constants().var_true);
              ee.b = ee.r;
              cs.push_back(ee);
            }
          }
        }
        if (r==constants().var_true && ctx.b==C_ROOT && !ctx.neg) {
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          (void) flat_exp(env,ctx,let->in(),r,b);
          ret.r = conj(env,r,Ctx(),cs);
        } else {
          Ctx nctx = ctx;
          nctx.neg = false;
          EE ee = flat_exp(env,nctx,let->in(),NULL,NULL);
          if (let->type().isbool() && !let->type().isopt()) {
            ee.b = ee.r;
            cs.push_back(ee);
            ret.r = conj(env,r,ctx,cs);
            ret.b = bind(env,Ctx(),b,constants().lit_true);
          } else {
            cs.push_back(ee);
            ret.r = bind(env,Ctx(),r,ee.r());
            ret.b = conj(env,b,Ctx(),cs);
          }
        }
        let->popbindings();
        // Restore previous mapping
        for (unsigned int i=0; i<let->let().size(); i++) {
          if (VarDecl* vd = let->let()[i]->dyn_cast<VarDecl>()) {
            vd->flat(Expression::cast<VarDecl>(flatmap.back()()));
            flatmap.pop_back();
          }
        }
      }
      break;
    case Expression::E_TI:
      throw InternalError("not supported yet");
      break;
    case Expression::E_TIID:
      throw InternalError("not supported yet");
      break;
    }
    assert(ret.r());
    return ret;
  }
  

  

  bool checkParDomain(EnvI& env, Expression* e, Expression* domain) {
    if (e->type()==Type::parint()) {
      IntSetVal* isv = eval_intset(env,domain);
      if (!isv->contains(eval_int(env,e)))
        return false;
    } else if (e->type()==Type::parfloat()) {
      FloatSetVal* fsv = eval_floatset(env,domain);
      if (!fsv->contains(eval_float(env, e)))
        return false;
    } else if (e->type()==Type::parsetint()) {
      IntSetVal* isv = eval_intset(env,domain);
      IntSetRanges ir(isv);
      IntSetVal* rsv = eval_intset(env,e);
      IntSetRanges rr(rsv);
      if (!Ranges::subset(rr, ir))
        return false;
    } else if (e->type()==Type::parsetfloat()) {
      FloatSetVal* fsv = eval_floatset(env,domain);
      FloatSetRanges fr(fsv);
      FloatSetVal* rsv = eval_floatset(env,e);
      FloatSetRanges rr(rsv);
      if (!Ranges::subset(rr, fr))
        return false;
    }
    return true;
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
        check_only_range->decl(env.orig->matchFn(e.envi(), check_only_range, false));
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
              v->e()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(lb, ub)));
              v->e()->ti()->setComputedDomain(true);
            } else if (v->e()->type().bt()==Type::BT_FLOAT && v->e()->type().st()==Type::ST_PLAIN) {
              FloatVal lb = FloatVal::infinity();
              FloatVal ub = -FloatVal::infinity();
              for (unsigned int i=0; i<al->size(); i++) {
                FloatVal vi = eval_float(env, (*al)[i]);
                lb = std::min(lb, vi);
                ub = std::max(ub, vi);
              }
              GCLock lock;
              v->e()->ti()->domain(new SetLit(Location().introduce(), FloatSetVal::a(lb, ub)));
              v->e()->ti()->setComputedDomain(true);
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
        e.envi().errorStack.clear();
        Location modelLoc(e.model()->filepath(),0,0,0,0);
        throw FlatteningError(e.envi(),modelLoc, "Model does not have a solve item");
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
        FunctionI* fi = env.orig->matchFn(env, constants().ids.int_.lin_eq, int_lin_eq_t, false);
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
        FunctionI* fi = env.orig->matchFn(env, ASTString("array_bool_and"), array_bool_andor_t, false);
        array_bool_and = (fi && fi->e()) ? fi : NULL;
        fi = env.orig->matchFn(env, ASTString("array_bool_or"), array_bool_andor_t, false);
        array_bool_or = (fi && fi->e()) ? fi : NULL;

        array_bool_andor_t[1] = Type::varbool(1);
        fi = env.orig->matchFn(env, ASTString("bool_clause"), array_bool_andor_t, false);
        array_bool_clause = (fi && fi->e()) ? fi : NULL;

        array_bool_andor_t.push_back(Type::varbool());
        fi = env.orig->matchFn(env, ASTString("bool_clause_reif"), array_bool_andor_t, false);
        array_bool_clause_reif = (fi && fi->e()) ? fi : NULL;
        
        std::vector<Type> bool_xor_t(3);
        bool_xor_t[0] = Type::varbool();
        bool_xor_t[1] = Type::varbool();
        bool_xor_t[2] = Type::varbool();
        fi = env.orig->matchFn(env, constants().ids.bool_xor, bool_xor_t, false);
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
                  call->decl(env.orig->matchFn(env, call, false));
                  env.flat_addItem(new ConstraintI(Location().introduce(), call));
                } else if (dom->max(dom->size()-1).isFinite()) {
                  std::vector<Expression*> args(2);
                  args[0] = vdi->e()->id();
                  args[1] = IntLit::a(dom->max(dom->size()-1));
                  Call* call = new Call(Location().introduce(),constants().ids.int_.le,args);
                  call->type(Type::varbool());
                  call->decl(env.orig->matchFn(env, call, false));
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
                    call->decl(env.orig->matchFn(env, call, false));
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
              call->decl(env.orig->matchFn(env, call, false));
              env.flat_addItem(new ConstraintI(Location().introduce(), call));
            } else if (vmax == FloatVal::infinity()) {
              vdi->e()->ti()->domain(NULL);
              std::vector<Expression*> args(2);
              args[0] = FloatLit::a(vmin);
              args[1] = vdi->e()->id();
              Call* call = new Call(Location().introduce(),constants().ids.float_.le,args);
              call->type(Type::varbool());
              call->decl(env.orig->matchFn(env, call, false));
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
              call->decl(env.orig->matchFn(env, call, false));
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
                    for (unsigned int i=nc_c.size(); i--;)
                      nc_c[i] = (*le_c)[i];
                    nc_c.push_back(IntLit::a(-1));
                    args[0] = new ArrayLit(Location().introduce(),nc_c);
                    args[0]->type(Type::parint(1));
                    ArrayLit* le_x = follow_id(c->arg(1))->cast<ArrayLit>();
                    std::vector<Expression*> nx(le_x->size());
                    for (unsigned int i=nx.size(); i--;)
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
                    FunctionI* decl = env.orig->matchFn(env,c,false);
                    env.map_remove(c);
                    if (decl->e() || c->id() == constants().ids.forall) {
                      if (decl->e())
                        addPathAnnotation(env, decl->e());
                      c->decl(decl);
                      nc = c;
                    }
                  } else {
                    std::vector<Expression*> args(c->n_args());
                    for (unsigned int i=args.size(); i--;)
                      args[i] = c->arg(i);
                    args.push_back(vd->id());
                    ASTString cid = c->id();
                    if (cid == constants().ids.clause && array_bool_clause_reif) {
                      nc = new Call(c->loc().introduce(),array_bool_clause_reif->id(),args);
                      nc->type(Type::varbool());
                      nc->decl(array_bool_clause_reif);
                    } else {
                      if (c->type().isbool() && vd->type().isbool()) {
                        cid = env.reifyId(c->id());
                      }
                      FunctionI* decl = env.orig->matchFn(env,cid,args,false);
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
                  env.map_remove(c);
                  /// TODO: check if removing variables here makes sense:
//                  if (!isOutput(vd) && env.vo.occurrences(vd)==0) {
//                    removedItems.push_back(vdi);
//                  }
                  if (nc != c) {
                    vd->addAnnotation(constants().ann.is_defined_var);
                    nc->addAnnotation(definesVarAnn(vd->id()));
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
                FunctionI* decl = env.orig->matchFn(env,c,false);
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
              FunctionI* origdecl = env.orig->matchFn(env, rhs->id(), tv, false);
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
    } catch (ModelInconsistent& e) {
      
    }
  }
  
  void clearInternalAnnotations(Expression* e) {
    e->ann().remove(constants().ann.promise_total);
    e->ann().remove(constants().ann.maybe_partial);
    e->ann().remove(constants().ann.add_to_output);
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
    
    // Remove boolean context annotations used only on compilation
    vd->ann().remove(constants().ctx.mix);
    vd->ann().remove(constants().ctx.pos);
    vd->ann().remove(constants().ctx.neg);
    vd->ann().remove(constants().ctx.root);
    vd->ann().remove(constants().ann.promise_total);
    vd->ann().remove(constants().ann.add_to_output);
    vd->ann().remove(constants().ann.mzn_check_var);
    
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
            if (!is_fixed)
              vd->addAnnotation(constants().ann.is_defined_var);
            ASTString cid;
            if (c->id() == constants().ids.exists) {
              cid = constants().ids.array_bool_or;
            } else if (c->id() == constants().ids.forall) {
              cid = constants().ids.array_bool_and;
            } else if (c->id() == constants().ids.clause) {
              cid = constants().ids.bool_clause_reif;
            } else {
              cid = env.reifyId(c->id());
            }
            std::vector<Expression*> args(c->n_args());
            for (unsigned int i=args.size(); i--;)
              args[i] = c->arg(i);
            if (is_fixed) {
              args.push_back(constants().lit_false);
            } else {
              args.push_back(vd->id());
            }
            Call * nc = new Call(c->loc().introduce(),cid,args);
            nc->type(c->type());
            FunctionI* decl = env.orig->matchFn(env, nc, false);
            if (decl==NULL) {
              throw FlatteningError(env,c->loc(),"'"+c->id().str()+"' is used in a reified context but no reified version is available");
            }
            nc->decl(decl);
            if (!is_fixed)
              nc->addAnnotation(definesVarAnn(vd->id()));
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
          vd->addAnnotation(constants().ann.is_defined_var);
          
          std::vector<Expression*> args(cc->n_args());
          ASTString cid;
          if (cc->id() == constants().ids.lin_exp) {
            // a = lin_exp([1],[b],5) => int_lin_eq([1,-1],[b,a],-5):: defines_var(a)
            ArrayLit* le_c = follow_id(cc->arg(0))->cast<ArrayLit>();
            std::vector<Expression*> nc(le_c->size());
            for (unsigned int i=nc.size(); i--;)
              nc[i] = (*le_c)[i];
            if (le_c->type().bt()==Type::BT_INT) {
              cid = constants().ids.int_.lin_eq;
              nc.push_back(IntLit::a(-1));
              args[0] = new ArrayLit(Location().introduce(),nc);
              args[0]->type(Type::parint(1));
              ArrayLit* le_x = follow_id(cc->arg(1))->cast<ArrayLit>();
              std::vector<Expression*> nx(le_x->size());
              for (unsigned int i=nx.size(); i--;)
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
              for (unsigned int i=nx.size(); i--;)
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
            for (unsigned int i=args.size(); i--;)
              args[i] = cc->arg(i);
            args.push_back(vd->id());
          }
          Call* nc = new Call(cc->loc().introduce(),cid,args);
          nc->type(cc->type());
          nc->addAnnotation(definesVarAnn(vd->id()));
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
    return added_constraints;
  }
  
  Expression* cleanup_constraint(EnvI& env, UNORDERED_NAMESPACE::unordered_set<Item*>& globals, Expression* ce) {
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
        vc->decl(env.orig->matchFn(env, vc, false));
      } else if (vc->id() == constants().ids.forall) {
        GCLock lock;
        vc->id(constants().ids.array_bool_and);
        std::vector<Expression*> args(2);
        args[0] = vc->arg(0);
        args[1] = constants().lit_true;
        ASTExprVec<Expression> argsv(args);
        vc->args(argsv);
        vc->decl(env.orig->matchFn(env, vc, false));
      } else if (vc->id() == constants().ids.clause) {
        GCLock lock;
        vc->id(constants().ids.bool_clause);
        vc->decl(env.orig->matchFn(env, vc, false));
      } else if (vc->id() == constants().ids.bool_xor && vc->n_args()==2) {
        GCLock lock;
        std::vector<Expression*> args(3);
        args[0] = vc->arg(0);
        args[1] = vc->arg(1);
        args[2] = constants().lit_true;
        ASTExprVec<Expression> argsv(args);
        vc->args(argsv);
        vc->decl(env.orig->matchFn(env, vc, false));
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
    UNORDERED_NAMESPACE::unordered_set<Item*> globals;

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
          e.envi().flat_addItem(new ConstraintI(Location().introduce(),nc));
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
      for (unsigned int i=stack.size(); i--;) {
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
