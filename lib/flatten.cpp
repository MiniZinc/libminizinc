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
#include <minizinc/exception.hh>
#include <minizinc/optimize.hh>
#include <minizinc/astiterator.hh>

#include <unordered_set>

// temporary
#include <minizinc/prettyprinter.hh>

namespace MiniZinc {

  /// Boolean evaluation context
  enum BCtx { C_ROOT, C_POS, C_NEG, C_MIX };

  /// Evaluation context
  struct Ctx {
    /// Boolean context
    BCtx b;
    /// Integer context
    BCtx i;
    /// Boolen negation flag
    bool neg;
    /// Default constructor (root context)
    Ctx(void) : b(C_ROOT), i(C_POS), neg(false) {}
    /// Copy constructor
    Ctx(const Ctx& ctx) : b(ctx.b), i(ctx.i), neg(ctx.neg) {}
    /// Assignment operator
    Ctx& operator =(const Ctx& ctx) {
      if (this!=&ctx) {
        b = ctx.b;
        i = ctx.i;
        neg = ctx.neg;
      }
      return *this;
    }
  };

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

  /// Turn \a c into positive context
  BCtx operator +(const BCtx& c) {
    switch (c) {
    case C_ROOT: return C_POS;
    case C_POS: return C_POS;
    case C_NEG: return C_NEG;
    case C_MIX: return C_MIX;
    default: assert(false); return C_ROOT;
    }
  }

  /// Turn \a c into negative context
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

  /// Result of evaluation
  class EE {
  public:
    /// The result value
    KeepAlive r;
    /// Boolean expression representing whether result is defined
    KeepAlive b;
    /// Constructor
    explicit EE(Expression* r0=NULL, Expression* b0=NULL) : r(r0), b(b0) {}
  };

  void dumpEEb(const std::vector<EE>& ee) {
    for (int i=0; i<ee.size(); i++)
      std::cerr << *ee[i].b();
  }
  void dumpEEr(const std::vector<EE>& ee) {
    for (int i=0; i<ee.size(); i++)
      std::cerr << *ee[i].r();
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
    Call* c = new Call(Location(),constants().ann.defines_var,args);
    c->type(Type::ann());
    return c;
  }

  /// Check if \a e is NULL or true
  bool istrue(Expression* e) {
    return e==NULL || (e->type().ispar() && e->type().isbool()
                       && eval_bool(e));
  }  
  /// Check if \a e is non-NULL and false
  bool isfalse(Expression* e) {
    return e!=NULL && e->type().ispar() && e->type().isbool()
           && !eval_bool(e);
  }  

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);

  class EnvI {
  public:
    Model* orig;
    Model* output;
    std::string varPrefix;
    VarOccurrences vo;
    VarOccurrences output_vo;
    CopyMap cmap;
    ASTStringMap<KeepAlive>::t reverseMappers;
    struct WW {
      WeakRef r;
      WeakRef b;
      WW(WeakRef r0, WeakRef b0) : r(r0), b(b0) {}
    };
    typedef KeepAliveMap<WW> Map;
    bool ignorePartial;
    std::vector<const Expression*> callStack;
    std::vector<const Expression*> errorStack;
    std::vector<int> idStack;
  protected:
    Map map;
    Model* _flat;
    unsigned int ids;
  public:
    EnvI(Model* orig0) : orig(orig0), output(new Model), varPrefix("X_"), ignorePartial(false), _flat(new Model), ids(0) {
    }
    ~EnvI(void) {
      delete _flat;
      delete output;
    }
    ASTString genId(const std::string& s) {
      std::ostringstream oss; oss << varPrefix << s << "_" << ids++;
      return ASTString(oss.str());
    }
    void map_insert(Expression* e, const EE& ee) {
      KeepAlive ka(e);
      map.insert(ka,WW(ee.r(),ee.b()));
    }
    Map::iterator map_find(Expression* e) {
      KeepAlive ka(e);
      Map::iterator it = map.find(ka);
      if (it != map.end()) {
        if (it->second.r()) {
          if (it->second.r()->isa<VarDecl>()) {
            int idx = vo.find(it->second.r()->cast<VarDecl>());
            if (idx >= 0 && _flat->_items[idx]->removed())
              return map.end();
          }
        } else {
          return map.end();
        }
      }
      return it;
    }
    void map_remove(Expression* e) {
      KeepAlive ka(e);
      map.remove(ka);
    }
    Map::iterator map_end(void) {
      return map.end();
    }
    void dump(void) {
      struct EED {
        static std::string d(const WW& ee) {
          std::ostringstream oss;
          oss << ee.r() << " " << ee.b();
          return oss.str();
        }
      };
      map.dump<EED>();
    }

    void flat_addItem(Item* i) {
      _flat->addItem(i);
      Expression* toAnnotate = NULL;
      switch (i->iid()) {
      case Item::II_VD:
        {
          VarDeclI* vd = i->cast<VarDeclI>();
          toAnnotate = vd->e()->e();
          vo.add(vd, _flat->size()-1);
          CollectOccurrencesE ce(vo,vd);
          topDown(ce,vd->e());
        }
        break;
      case Item::II_CON:
        {
          ConstraintI* ci = i->cast<ConstraintI>();
          toAnnotate = ci->e();
          if (ci->e()->isa<BoolLit>() && !ci->e()->cast<BoolLit>()->v())
            std::cerr << "Warning: model inconsistency detected" << std::endl;
          CollectOccurrencesE ce(vo,ci);
          topDown(ce,ci->e());
        }
        break;
      case Item::II_SOL:
        {
          SolveI* si = i->cast<SolveI>();
          CollectOccurrencesE ce(vo,si);
          topDown(ce,si->e());
          for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it)
            topDown(ce,*it);
        }
        break;
      default:
        break;
      }
      if (toAnnotate && toAnnotate->isa<Call>()) {
        int prev = idStack.size() > 0 ? idStack.back() : 0;
        for (int i = callStack.size()-1; i >= prev; i--) {
          for (ExpressionSetIter it = callStack[i]->ann().begin(); it != callStack[i]->ann().end(); ++it) {
            EE ee_ann = flat_exp(*this, Ctx(), *it, NULL, constants().var_true);
            toAnnotate->addAnnotation(ee_ann.r());
          }
        }
      }
    }
    void flat_replaceItem(int idx, Item* i) {
      if (_flat->_items[idx]->isa<VarDeclI>() && !i->isa<VarDeclI>())
        vo.remove(_flat->_items[idx]->cast<VarDeclI>()->e());
      _flat->_items[idx] = i;
    }
    void vo_add_exp(VarDecl* vd) {
      if (vd->e() && vd->e()->isa<Call>()) {
        int prev = idStack.size() > 0 ? idStack.back() : 0;
        for (int i = callStack.size()-1; i >= prev; i--) {
          for (ExpressionSetIter it = callStack[i]->ann().begin(); it != callStack[i]->ann().end(); ++it) {
            EE ee_ann = flat_exp(*this, Ctx(), *it, NULL, constants().var_true);
            vd->e()->addAnnotation(ee_ann.r());
          }
        }
      }
      int idx = vo.find(vd);
      CollectOccurrencesE ce(vo,_flat->_items[idx]);
      topDown(ce, vd->e());
    }
    Model* flat(void) {
      return _flat;
    }
    
  };

  class CallStackItem {
  public:
    EnvI& env;
    CallStackItem(EnvI& env0, Expression* e) : env(env0) {
      env.errorStack.clear();
      if (e->isa<VarDecl>())
        env.idStack.push_back(env.callStack.size());
      env.callStack.push_back(e);
    }
    ~CallStackItem(void) {
      env.errorStack.push_back(env.callStack.back());
      if (env.callStack.back()->isa<VarDecl>())
        env.idStack.pop_back();
      env.callStack.pop_back();
    }
  };
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
  
  Env::Env(Model* m) : e(new EnvI(m)) {}
  Env::~Env(void) {
    delete e;
  }
  
  Model*
  Env::model(void) { return e->orig; }
  Model*
  Env::flat(void) { return e->flat(); }
  Model*
  Env::output(void) { return e->output; }
  EnvI&
  Env::envi(void) { return *e; }
  std::ostream&
  Env::dumpErrorStack(std::ostream& os) {
    if (e->errorStack.size() > 0 && !e->errorStack[0]->isa<Id>())
      std::cerr << "while evaluating" << std::endl;
    for (unsigned int i=0; i<e->errorStack.size(); i++) {
      if (e->errorStack[i]->isa<Id>())
        break;
      os << " " << *e->errorStack[i];
      os << " in file " << e->errorStack[i]->loc().toString() << std::endl;
    }
    return os;
  }

  bool isTotal(FunctionI* fi) {
    return fi->ann().contains(constants().ann.promise_total);
  }

  bool isReverseMap(BinOp* e) {
    return e->ann().contains(constants().ann.is_reverse_map);
  }

  Expression* follow_id(Expression* e) {
    for (;;) {
      if (e==NULL)
        return NULL;
      if (e->eid()==Expression::E_ID) {
        e = e->cast<Id>()->decl()->e();
      } else {
        return e;
      }
    }
  }
  
  Expression* follow_id_to_decl(Expression* e) {
    for (;;) {
      if (e==NULL)
        return NULL;
      switch (e->eid()) {
        case Expression::E_ID:
          e = e->cast<Id>()->decl();
          break;
        case Expression::E_VARDECL:
          if (e->cast<VarDecl>()->e() && e->cast<VarDecl>()->e()->isa<Id>())
            e = e->cast<VarDecl>()->e();
          else
            return e;
          break;
        default:
          return e;
      }
    }
  }

  Expression* follow_id_to_value(Expression* e) {
    Expression* decl = follow_id_to_decl(e);
    if (VarDecl* vd = decl->dyn_cast<VarDecl>()) {
      if (vd->e() && vd->e()->type().ispar())
        return vd->e();
      return vd->id();
    } else {
      return decl;
    }
  }

  void checkIndexSets(VarDecl* vd, Expression* e) {
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
            if (!eval_intset(tis[i]->domain())->equal(eval_intset(e_tis[i]->domain())))
              throw EvalError(vd->loc(), "Index set mismatch");
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
            newtis[i] = new TypeInst(Location(),Type(),new SetLit(Location(),IntSetVal::a(al->min(i),al->max(i))));
            needNewTypeInst = true;
          } else {
            IntSetVal* isv = eval_intset(tis[i]->domain());
            assert(isv->size()<=1);
            if ( (isv->size()==0 && al->min(i) <= al->max(i)) ||
                 (isv->size()!=0 && (isv->min(0) != al->min(i) || isv->max(0) != al->max(i))) )
              throw EvalError(vd->loc(), "Index set mismatch");
            newtis[i] = tis[i];
          }
        }
      }
        break;
      default:
        throw InternalError("not supported yet");
    }
    if (needNewTypeInst) {
      TypeInst* tic = copy(vd->ti())->cast<TypeInst>();
      tic->setRanges(newtis);
      vd->ti(tic);
    }
  }
  
  KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e) {
    assert(e==NULL || !e->isa<VarDecl>());
    if (ctx.neg) {
      assert(e->type()._bt == Type::BT_BOOL);
      if (vd==constants().var_true) {
        if (!isfalse(e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            while (id != NULL) {
              assert(id->decl() != NULL);
              if (id->decl()->ti()->domain() && istrue(id->decl()->ti()->domain())) {
                GCLock lock;
                env.flat_addItem(new ConstraintI(Location(),constants().lit_false));
              } else {
                id->decl()->ti()->domain(constants().lit_false);
                GCLock lock;
                std::vector<Expression*> args(2);
                args[0] = id;
                args[1] = constants().lit_false;
                Call* c = new Call(Location(),constants().ids.bool_eq,args);
                c->decl(env.orig->matchFn(c));
                c->type(c->decl()->rtype(args));
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
        if (!istrue(e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            assert(id->decl() != NULL);
            while (id != NULL) {
              if (id->decl()->ti()->domain() && isfalse(id->decl()->ti()->domain())) {
                GCLock lock;
                env.flat_addItem(new ConstraintI(Location(),constants().lit_false));
              } else {
                id->decl()->ti()->domain(constants().lit_true);
                GCLock lock;
                std::vector<Expression*> args(2);
                args[0] = id;
                args[1] = constants().lit_true;
                Call* c = new Call(Location(),constants().ids.bool_eq,args);
                c->decl(env.orig->matchFn(c));
                c->type(c->decl()->rtype(args));
                if (c->decl()->e()) {
                  flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                }
              }
              id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
            }
          } else {
            GCLock lock;
            env.flat_addItem(new ConstraintI(Location(),e));
          }
        }
        return constants().lit_true;
      } else if (vd==constants().var_false) {
        if (!isfalse(e)) {
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
          return e;
        case Expression::E_CALL:
          {
            if (e->type().isann())
              return e;
            GCLock lock;
            /// TODO: handle array types
            TypeInst* ti = new TypeInst(Location(),e->type());
            VarDecl* vd = new VarDecl(e->loc(),ti,env.genId("X"),e);
            vd->introduced(true);
            vd->flat(vd);

            if (vd->e()->type()._bt==Type::BT_INT && vd->e()->type()._dim==0) {
              IntSetVal* ibv = NULL;
              if (vd->e()->type().isset()) {
                ibv = compute_intset_bounds(vd->e());
              } else {
                IntBounds ib = compute_int_bounds(vd->e());
                if (ib.valid) {
                  ibv = IntSetVal::a(ib.l,ib.u);
                }
              }
              if (ibv) {
                Id* id = vd->id();
                while (id != NULL) {
                  if (id->decl()->ti()->domain()) {
                    IntSetVal* domain = eval_intset(id->decl()->ti()->domain());
                    IntSetRanges dr(domain);
                    IntSetRanges ibr(ibv);
                    Ranges::Inter<IntSetRanges,IntSetRanges> i(dr,ibr);
                    IntSetVal* newibv = IntSetVal::ai(i);
                    if (ibv->card() == newibv->card()) {
                      id->decl()->ti()->setComputedDomain(true);
                    } else {
                      ibv = newibv;
                    }
                  } else {
                    id->decl()->ti()->setComputedDomain(true);
                  }
                  if (id->type()._st==Type::ST_PLAIN && ibv->size()==0) {
                    std::cerr << "Warning: model inconsistency detected";
                    env.flat_addItem(new ConstraintI(Location(),constants().lit_false));
                  } else {
                    id->decl()->ti()->domain(new SetLit(Location(),ibv));
                  }
                  id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
                }
              }
            } else if (vd->e()->type().isbool()) {
              addCtxAnn(vd, ctx.b);
            }

            VarDeclI* nv = new VarDeclI(Location(),vd);
            env.flat_addItem(nv);

            EE ee(vd,NULL);
            env.map_insert(vd->id(),ee);

            return vd->id();
          }
        default:
          assert(false); return NULL;
        }
      } else {
        if (vd->e()==NULL) {
          Expression* ret = e;
          if (e==NULL || (e->type().ispar() && e->type().isbool())) {
            if (e==NULL || eval_bool(e)) {
              vd->e(constants().lit_true);
            } else {
              vd->e(constants().lit_false);
            }
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = vd->id();
            args[1] = vd->e();
            Call* c = new Call(Location(),constants().ids.bool_eq,args);
            c->decl(env.orig->matchFn(c));
            c->type(c->decl()->rtype(args));
            if (c->decl()->e()) {
              flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
              return vd->id();
            }            
          } else {
            if (e->type().dim() > 0) {
              // Check that index sets match
              checkIndexSets(vd,e);
            } else if (Id* e_id = e->dyn_cast<Id>()) {
              ASTString cid;
              if (e->type().isint()) {
                cid = constants().ids.int_eq;
              } else if (e->type().isbool()) {
                cid = constants().ids.bool_eq;
              } else if (e->type().isset()) {
                cid = constants().ids.set_eq;
              } else if (e->type().isfloat()) {
                cid = constants().ids.float_eq;
              }
              if (cid != "") {
                GCLock lock;
                std::vector<Expression*> args(2);
                args[0] = vd->id();
                args[1] = e_id;
                Call* c = new Call(Location(),cid,args);
                c->decl(env.orig->matchFn(c));
                c->type(c->decl()->rtype(args));
                if (c->decl()->e()) {
                  flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
                  ret = vd->id();
                  vd->e(e);
                  env.vo_add_exp(vd);
                }
              }
            }
            
            if (ret != vd->id()) {
              vd->e(ret);
              env.vo_add_exp(vd);
            }
            if (vd->e()->type()._bt==Type::BT_INT && vd->e()->type()._dim==0) {
              GCLock lock;
              IntSetVal* ibv = NULL;
              if (vd->e()->type().isset()) {
                ibv = compute_intset_bounds(vd->e());
              } else {
                IntBounds ib = compute_int_bounds(vd->e());
                if (ib.valid)
                  ibv = IntSetVal::a(ib.l,ib.u);
              }
              if (ibv) {
                if (vd->ti()->domain()) {
                  IntSetVal* domain = eval_intset(vd->ti()->domain());
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(ibv);
                  Ranges::Inter<IntSetRanges,IntSetRanges> i(dr,ibr);
                  IntSetVal* newibv = IntSetVal::ai(i);
                  if (ibv->card() == newibv->card()) {
                    vd->ti()->setComputedDomain(true);
                  } else {
                    ibv = newibv;
                  }
                } else {
                  vd->ti()->setComputedDomain(true);
                }
                vd->ti()->domain(new SetLit(Location(),ibv));
              }
            }
          }
          return ret;
        } else if (vd == e) {
          return vd->id();
        } else if (vd->e() != e) {
          e = follow_id_to_decl(e);
          switch (e->eid()) {
          case Expression::E_BOOLLIT:
            {
              Id* id = vd->id();
              while (id != NULL) {
                if (id->decl()->ti()->domain() && eval_bool(id->decl()->ti()->domain()) == e->cast<BoolLit>()->v()) {
                  return constants().lit_true;
                } else if (id->decl()->ti()->domain() && eval_bool(id->decl()->ti()->domain()) != e->cast<BoolLit>()->v()) {
                  GCLock lock;
                  env.flat_addItem(new ConstraintI(Location(),constants().lit_false));
                } else {
                  id->decl()->ti()->domain(e);
                  GCLock lock;
                  std::vector<Expression*> args(2);
                  args[0] = id;
                  args[1] = e;
                  Call* c = new Call(Location(),constants().ids.bool_eq,args);
                  c->decl(env.orig->matchFn(c));
                  c->type(c->decl()->rtype(args));
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
              if (e->type()._dim != 0)
                throw InternalError("not supported yet");
              GCLock lock;
              ASTString cid;
              if (e->type().isint()) {
                cid = constants().ids.int_eq;
              } else if (e->type().isbool()) {
                cid = constants().ids.bool_eq;
              } else if (e->type().isset()) {
                cid = constants().ids.set_eq;
              } else if (e->type().isfloat()) {
                cid = constants().ids.float_eq;
              } else {
                throw InternalError("not yet implemented");
              }
              std::vector<Expression*> args(2);
              args[0] = vd->id();
              args[1] = e_vd->id();
              Call* c = new Call(Location(),cid,args);
              c->decl(env.orig->matchFn(c));
              c->type(c->decl()->rtype(args));
              flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
              return vd->id();
            }
          case Expression::E_CALL:
            {
              Call* c = e->cast<Call>();
              std::vector<Expression*> args(c->args().size());
              GCLock lock;
              if (c->id() == constants().ids.lin_exp) {
                c->id(constants().ids.int_lin_eq);
                ArrayLit* le_c = follow_id(c->args()[0])->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),nc.begin());
                nc.push_back(new IntLit(Location(),-1));
                c->args()[0] = new ArrayLit(Location(),nc);
                ArrayLit* le_x = follow_id(c->args()[1])->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->v().size());
                std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                nx.push_back(vd->id());
                c->args()[1] = new ArrayLit(Location(),nx);
                IntVal d = c->args()[2]->cast<IntLit>()->v();
                c->args()[2] = new IntLit(Location(),-d);
              } else {
                vd->addAnnotation(constants().ann.is_defined_var);
                
                args.push_back(vd->id());

                if (c->id() == constants().ids.exists) {
                  c->id(ASTString("array_bool_or"));
                } else if (c->id() == constants().ids.forall) {
                  c->id(ASTString("array_bool_and"));
                } else if (vd->type().isbool()) {
                  c->id(ASTString(c->id().str()+"_reif"));
                }

              }
              std::copy(c->args().begin(),c->args().end(),args.begin());
              c->args(ASTExprVec<Expression>(args));
              c->decl(env.orig->matchFn(c));
              c->type(c->decl()->rtype(args));
              c->addAnnotation(definesVarAnn(vd->id()));
              flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
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
        if (istrue(e[i].b()))
          continue;
        if (isfalse(e[i].b())) {
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
          ArrayLit* al = new ArrayLit(Location(),nontrue);
          al->type(Type::varbool(1));
          args.push_back(al);
          Call* ret = new Call(Location(),constants().ids.forall,args);
          ret->decl(env.orig->matchFn(ret));
          ret->type(ret->decl()->rtype(args));
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
        if (istrue(e[i].b()))
          continue;
        if (isfalse(e[i].b())) {
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
          ArrayLit* al = new ArrayLit(Location(),nonfalse);
          al->type(Type::varbool(1));
          args.push_back(al);
          Call* ret = new Call(Location(),constants().ids.exists,args);
          ret->decl(env.orig->matchFn(ret));
          ret->type(ret->decl()->rtype(args));
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
        return new TypeInst(Location(),vd->e()->type());
      ArrayLit* al = eval_array_lit(vd->e());
      std::vector<TypeInst*> dims(al->dims());
      for (unsigned int i=0; i<dims.size(); i++) {
        dims[i] = new TypeInst(Location(), Type(), new SetLit(Location(),IntSetVal::a(al->min(i),al->max(i))));
      }
      return new TypeInst(Location(), vd->e()->type(), dims, eval_par(vd->ti()->domain()));
    } else {
      std::vector<TypeInst*> dims(vd->ti()->ranges().size());
      for (unsigned int i=0; i<vd->ti()->ranges().size(); i++) {
        if (vd->ti()->ranges()[i]->domain()) {
          IntSetVal* isv = eval_intset(vd->ti()->ranges()[i]->domain());
          if (isv->size() > 1)
            throw EvalError(vd->ti()->ranges()[i]->domain()->loc(),
                            "array index set must be contiguous range");
          SetLit* sl = new SetLit(vd->ti()->ranges()[i]->loc(),isv);
          sl->type(Type::parsetint());
          dims[i] = new TypeInst(vd->ti()->ranges()[i]->loc(), Type(),sl);
        } else {
          dims[i] = new TypeInst(vd->ti()->ranges()[i]->loc(), Type(), NULL);
        }
      }
      return new TypeInst(vd->ti()->loc(), vd->ti()->type(), dims, eval_par(vd->ti()->domain()));
    }
  }

  std::string opToBuiltin(BinOp* op, BinOpType bot) {
    std::string builtin;
    if (op->rhs()->type().isint()) {
      builtin = "int_";
    } else if (op->rhs()->type().isbool()) {
      builtin = "bool_";
    } else if (op->rhs()->type().isset()) {
      builtin = "set_";
    } else if (op->rhs()->type().isfloat()) {
      builtin = "float_";
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
      return "set_in";
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
      return builtin+"xor";
    default:
      assert(false); return "";
    }
  }
  
  Call* same_call(Expression* e, const std::string& id) {
    Expression* ce = follow_id(e);
    if (ce && ce->isa<Call>() && ce->cast<Call>()->id().str() == id)
      return ce->cast<Call>();
    return NULL;
  }
  Call* same_call(Expression* e, const ASTString& id) {
    Expression* ce = follow_id(e);
    if (ce && ce->isa<Call>() && ce->cast<Call>()->id() == id)
      return ce->cast<Call>();
    return NULL;
  }
  
  class CmpExpIdx {
  public:
    std::vector<KeepAlive>& x;
    CmpExpIdx(std::vector<KeepAlive>& x0) : x(x0) {}
    bool operator ()(int i, int j) const {
      if (Expression::equal(x[i](),x[j]()))
        return false;
      return x[i]()<x[j]();
    }
  };

  template<class Lit>
  class LinearTraits {
  };
  template<>
  class LinearTraits<IntLit> {
  public:
    typedef IntVal Val;
    static Val eval(Expression* e) { return eval_int(e); }
    static void constructLinBuiltin(BinOpType bot, ASTString& callid, int& coeff_sign, Val& d) {
      switch (bot) {
        case BOT_LE:
          callid = constants().ids.int_lin_le;
          coeff_sign = 1;
          d += 1;
          break;
        case BOT_LQ:
          callid = constants().ids.int_lin_le;
          coeff_sign = 1;
          break;
        case BOT_GR:
          callid = constants().ids.int_lin_le;
          coeff_sign = -1;
          d = -d+1;
          break;
        case BOT_GQ:
          callid = constants().ids.int_lin_le;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_EQ:
          callid = constants().ids.int_lin_eq;
          coeff_sign = 1;
          break;
        case BOT_NQ:
          callid = constants().ids.int_lin_ne;
          coeff_sign = 1;
          break;
        default: assert(false); break;
      }
    }
    static ASTString id_eq(void) { return constants().ids.int_eq; }
    typedef IntBounds Bounds;
    static Bounds compute_bounds(Expression* e) { return compute_int_bounds(e); }
    typedef IntSetVal* Domain;
    static Domain eval_domain(Expression* e) { return eval_intset(e); }
    static Expression* new_domain(Val v) { return new SetLit(Location(),IntSetVal::a(v,v)); }
    static Expression* new_domain(Domain d) { return new SetLit(Location(),d); }
    static bool domain_contains(Domain dom, Val v) { return dom->contains(v); }
    static bool domain_equals(Domain dom, Val v) { return dom->size()==1 && dom->min(0)==v && dom->max(0)==v; }
    static bool domain_equals(Domain dom1, Domain dom2) {
      IntSetRanges d1(dom1);
      IntSetRanges d2(dom2);
      return Ranges::equal(d1,d2);
    }
    static bool domain_empty(Domain dom) { return dom->size()==0; }
    static Domain limit_domain(BinOpType bot, Domain dom, Val v) {
      IntSetRanges dr(dom);
      IntSetVal* ndomain;
      switch (bot) {
        case BOT_LE:
          v -= 1;
          // fall through
        case BOT_LQ:
        {
          Ranges::Bounded<IntSetRanges> b = Ranges::Bounded<IntSetRanges>::maxiter(dr,v);
          ndomain = IntSetVal::ai(b);
        }
          break;
        case BOT_GR:
          v += 1;
          // fall through
        case BOT_GQ:
        {
          Ranges::Bounded<IntSetRanges> b = Ranges::Bounded<IntSetRanges>::miniter(dr,v);
          ndomain = IntSetVal::ai(b);
        }
          break;
        case BOT_NQ:
        {
          Ranges::Const c(v,v);
          Ranges::Diff<IntSetRanges,Ranges::Const> d(dr,c);
          ndomain = IntSetVal::ai(d);
        }
          break;
        default: assert(false);
      }
      return ndomain;
    }
  };
  template<>
  class LinearTraits<FloatLit> {
  public:
    typedef FloatVal Val;
    static Val eval(Expression* e) { return eval_float(e); }
    static void constructLinBuiltin(BinOpType bot, ASTString& callid, int& coeff_sign, Val& d) {
      switch (bot) {
        case BOT_LE:
          callid = constants().ids.float_lin_lt;
          coeff_sign = 1;
          break;
        case BOT_LQ:
          callid = constants().ids.float_lin_le;
          coeff_sign = 1;
          break;
        case BOT_GR:
          callid = constants().ids.float_lin_lt;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_GQ:
          callid = constants().ids.float_lin_le;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_EQ:
          callid = constants().ids.float_lin_eq;
          coeff_sign = 1;
          break;
        case BOT_NQ:
          callid = constants().ids.float_lin_ne;
          coeff_sign = 1;
          break;
        default: assert(false); break;
      }
    }
    static ASTString id_eq(void) { return constants().ids.float_eq; }
    typedef FloatBounds Bounds;
    static Bounds compute_bounds(Expression* e) { return compute_float_bounds(e); }
    typedef BinOp* Domain;
    static Domain eval_domain(Expression* e) {
      BinOp* bo = e->cast<BinOp>();
      assert(bo->op() == BOT_DOTDOT);
      if (bo->lhs()->isa<FloatLit>() && bo->rhs()->isa<FloatLit>())
        return bo;
      BinOp* ret = new BinOp(bo->loc(),eval_par(bo->lhs()),BOT_DOTDOT,eval_par(bo->rhs()));
      ret->type(bo->type());
      return ret;
    }
    static Expression* new_domain(Val v) {
      BinOp* ret = new BinOp(Location(),new FloatLit(Location(),v),BOT_DOTDOT,new FloatLit(Location(),v));
      ret->type(Type::parsetfloat());
      return ret;
    }
    static Expression* new_domain(Domain d) { return d; }
    static bool domain_contains(Domain dom, Val v) {
      return dom->lhs()->cast<FloatLit>()->v() <= v && dom->rhs()->cast<FloatLit>()->v() >= v;
    }
    static bool domain_equals(Domain dom, Val v) {
      return dom->lhs()->cast<FloatLit>()->v() == v && dom->rhs()->cast<FloatLit>()->v() == v;
    }
    static bool domain_equals(Domain dom1, Domain dom2) {
      return
        dom1->lhs()->cast<FloatLit>()->v() == dom2->lhs()->cast<FloatLit>()->v() &&
        dom1->rhs()->cast<FloatLit>()->v() == dom2->rhs()->cast<FloatLit>()->v();
    }
    static bool domain_empty(Domain dom) {
      return dom->lhs()->cast<FloatLit>()->v() > dom->rhs()->cast<FloatLit>()->v();
    }
    static Domain limit_domain(BinOpType bot, Domain dom, Val v) {
      return NULL;
    }
  };

  template<class Lit>
  KeepAlive mklinexp(EnvI& env, typename LinearTraits<Lit>::Val c0, typename LinearTraits<Lit>::Val c1,
                     Expression* e0, Expression* e1) {
    typedef typename LinearTraits<Lit>::Val Val;
    GCLock lock;
    Val d = 0;
    if (e0->type().ispar()) {
      d += c0*LinearTraits<Lit>::eval(e0);
      e0 = NULL;
    }
    if (e1 && e1->type().ispar()) {
      d += c1*LinearTraits<Lit>::eval(e1);
      e1 = NULL;
    }
    if (e0==NULL && e1==NULL)
      return new Lit(Location(),d);
    if (e0==NULL) {
      std::swap(e0,e1);
      std::swap(c0,c1);
    }
    std::vector<Expression*> bo_args(e1 ? 2 : 1);
    bo_args[0] = e0;
    if (e1)
      bo_args[1] = e1;
    std::vector<Expression*> coeffs(e1 ? 2 : 1);
    coeffs[0] = new Lit(e0->loc(),c0);
    if (e1) {
      if (c0==c1)
        coeffs[1] = coeffs[0];
      else
        coeffs[1] = new Lit(e0->loc(),c1);
    }
    std::vector<Expression*> args(3);
    args[0]=new ArrayLit(e0->loc(),coeffs);
    Type t = coeffs[0]->type();
    t._dim = 1;
    args[0]->type(t);
    args[1]=new ArrayLit(e0->loc(),bo_args);
    Type tt = e0->type();
    tt._dim = 1;
    if (e0->type()._ti==Type::TI_PAR && e1)
      tt._ti = e1->type()._ti;
    args[1]->type(tt);
    args[2] = new Lit(e0->loc(),d);
    Call* c = new Call(e0->loc(),constants().ids.lin_exp,args);
    tt = args[1]->type();
    tt._dim = 0;
    c->decl(env.orig->matchFn(c));
    c->type(c->decl()->rtype(args));
    KeepAlive ka = c;
    return ka;
  }

  template<class Lit>
  void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c,
                    std::vector<KeepAlive>& x,
                    typename LinearTraits<Lit>::Val& d) {
    std::vector<int> idx(c.size());
    for (unsigned int i=idx.size(); i--;) {
      idx[i]=i;
    }
    std::sort(idx.begin(),idx.end(),CmpExpIdx(x));
    int ci = 0;
    for (; ci<x.size(); ci++) {
      if (Lit* il = x[idx[ci]]()->dyn_cast<Lit>()) {
        d += c[idx[ci]]*il->v();
        c[idx[ci]] = 0;
      } else {
        break;
      }
    }
    for (unsigned int i=ci+1; i<x.size(); i++) {
      if (Expression::equal(x[idx[i]](),x[idx[ci]]())) {
        c[idx[ci]] += c[idx[i]];
        c[idx[i]] = 0;
      } else if (Lit* il = x[idx[i]]()->dyn_cast<Lit>()) {
        d += c[idx[i]]*il->v();
        c[idx[i]] = 0;
      } else {
        ci=i;
      }
    }
    ci = 0;
    for (unsigned int i=0; i<c.size(); i++) {
      if (c[i] != 0) {
        c[ci] = c[i];
        x[ci] = x[i];
        ci++;
      }
    }
    c.resize(ci);
    x.resize(ci);
  }

  class CmpExp {
  public:
    bool operator ()(const KeepAlive& i, const KeepAlive& j) const {
      if (Expression::equal(i(),j()))
        return false;
      return i()<j();
    }
  };

  void remove_dups(std::vector<KeepAlive>& x, bool identity) {
    for (unsigned int i=0; i<x.size(); i++) {
      x[i] = follow_id_to_value(x[i]());
    }
    std::sort(x.begin(),x.end(),CmpExp());
    int ci = 0;
    Expression* prev = NULL;
    for (unsigned int i=0; i<x.size(); i++) {
      if (!Expression::equal(x[i](),prev)) {
        prev = x[i]();
        if (x[i]()->isa<BoolLit>() && x[i]()->cast<BoolLit>()->v()==identity) {
          // skip
        } else {
          x[ci++] = x[i];
        }
      }
    }
    x.resize(ci);
  }

  /// Return a lin_exp or id if \a e is a lin_exp or id
  template<class Lit>
  Expression* get_linexp(Expression* e) {
    for (;;) {
      if (e && e->eid()==Expression::E_ID) {
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

  /// TODO: check if all expressions are total
  /// If yes, use element encoding
  /// If not, use implication encoding
  KeepAlive flat_ite(EnvI& env,ITE* ite,int i) {
    if (i>=ite->size())
      return ite->e_else();
    if (ite->e_if(i)->type()==Type::parbool()) {
      if (eval_bool(ite->e_if(i)))
        return ite->e_then(i);
      else
        return flat_ite(env,ite,i+1);
    } else {
      KeepAlive e_else = flat_ite(env,ite,i+1);
      GCLock lock;
      
      // TODO: translate to [e_else,e_then][bool2int(e_if)+1]
      // if both e_else and e_then do not contain partial function calls
//      std::vector<Expression*> alv(2);
//      alv[1] = ite->e_then(i);
//      alv[0] = flat_ite(env,ite,i+1)();
//      std::vector<std::pair<int,int> > aldims(1);
//      aldims[0].first = 0;
//      aldims[0].second = 1;
//      ArrayLit* al = new ArrayLit(ite->loc(),alv,aldims);
//      Type alt = ite->type();
//      alt._dim++;
//      al->type(alt);
//      std::vector<Expression*> b2iargs(1);
//      b2iargs[0] = ite->e_if(i);
//      Call* b2i = new Call(ite->loc(),"bool2int",b2iargs);
//      b2i->type(Type::varint());
//      b2i->decl(env.orig->matchFn(b2i));
//      std::vector<Expression*> aaidx(1);
//      aaidx[0] = b2i;
//      ArrayAccess* aa = new ArrayAccess(ite->loc(),al,aaidx);
//      aa->type(ite->type());
//      return aa;

      // Translate into the following expression:
      // let {
      //   var $T: r;
      //   constraint e_if -> r=e_then_arg;
      //   constraint (not e_if) -> r=e_else_arg;
      // } in r;
      
      SetLit* r_bounds = NULL;
      IntBounds ib_then = compute_int_bounds(ite->e_then(i));
      if (ib_then.valid) {
        IntBounds ib_else = compute_int_bounds(e_else());
        if (ib_else.valid) {
          r_bounds = new SetLit(Location(),
                                IntSetVal::a(std::min(ib_then.l,ib_else.l),
                                             std::max(ib_then.u,ib_else.u)));
          r_bounds->type(Type::parsetint());
        }
      }
      TypeInst* ti = new TypeInst(Location(),ite->type(),r_bounds);
      
      VarDecl* r = new VarDecl(ite->loc(),ti,env.genId("r_ite"));
      BinOp* eq_then = new BinOp(Location(),r->id(),BOT_EQ,ite->e_then(i));
      eq_then->type(Type::varbool());
      BinOp* eq_else = new BinOp(Location(),r->id(),BOT_EQ,e_else());
      eq_else->type(Type::varbool());
      std::vector<Expression*> clauseargs(2);
      std::vector<Expression*> posargs(1);
      posargs[0] = eq_then;
      clauseargs[0] = new ArrayLit(Location(),posargs);
      clauseargs[0]->type(Type::varbool(1));
      posargs[0] = ite->e_if(i);
      clauseargs[1] = new ArrayLit(Location(),posargs);
      clauseargs[1]->type(Type::varbool(1));
      Call* if_op = new Call(Location(), constants().ids.clause, clauseargs);
      if_op->decl(env.orig->matchFn(if_op));
      if_op->type(if_op->decl()->rtype(clauseargs));
      BinOp* else_op = new BinOp(Location(),ite->e_if(i),BOT_OR,eq_else);
      else_op->type(Type::varbool());
      std::vector<Expression*> e_let(3);
      e_let[0] = r;
      e_let[1] = if_op;
      e_let[2] = else_op;
      Let* let = new Let(Location(),e_let,r->id());
      let->type(r->id()->type());
      return let;
    }
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
    for (unsigned int i=0; i<2; i++) {
      Val sign = (i==0 ? 1 : -1);
      if (Lit* l = le[i]->dyn_cast<Lit>()) {
        d += sign*l->v();
      } else if (le[i]->isa<Id>()) {
        coeffv.push_back(sign);
        alv.push_back(le[i]);
      } else if (Call* sc = le[i]->dyn_cast<Call>()) {
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(sc->args()[0]);
        ArrayLit* sc_al = eval_array_lit(sc->args()[1]);
        d += sign*LinearTraits<Lit>::eval(sc->args()[2]);
        for (unsigned int j=0; j<sc_coeff->v().size(); j++) {
          coeffv.push_back(sign*LinearTraits<Lit>::eval(sc_coeff->v()[j]));
          alv.push_back(sc_al->v()[j]);
        }
      } else {
        throw EvalError(le[i]->loc(), "Internal error, unexpected expression inside linear expression");
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
      typename LinearTraits<Lit>::Bounds ib = LinearTraits<Lit>::compute_bounds(alv[0]());
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
          typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(vd->ti()->domain());
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
      } else if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && alv[0]()->cast<Id>()->decl()->ti()->domain()) {
        GCLock lock;
        VarDecl* vd = alv[0]()->cast<Id>()->decl();
        typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(vd->ti()->domain());
        typename LinearTraits<Lit>::Domain ndomain = LinearTraits<Lit>::limit_domain(bot,domain,d);
        if (domain) {
          if (LinearTraits<Lit>::domain_empty(ndomain)) {
            ret.r = bind(env,ctx,r,constants().lit_false);
          } else if (!LinearTraits<Lit>::domain_equals(domain,ndomain)) {
            ret.r = bind(env,ctx,r,constants().lit_true);
            vd->ti()->setComputedDomain(false);
            vd->ti()->domain(LinearTraits<Lit>::new_domain(ndomain));
          }
        } else {
          goto non_domain_binop;
        }
      } else {
      non_domain_binop:
        GCLock lock;
        Expression* e0;
        Expression* e1;
        switch (bot) {
          case BOT_LE:
            e0 = alv[0]();
            if (e0->type().isint()) {
              e1 = new Lit(Location(),d-1);
              bot = BOT_LQ;
            } else {
              e1 = new Lit(Location(),d);
            }
            break;
          case BOT_GR:
            e1 = alv[0]();
            if (e1->type().isint()) {
              e0 = new Lit(Location(),d+1);
              bot = BOT_LQ;
            } else {
              e0 = new Lit(Location(),d);
              bot = BOT_LE;
            }
            break;
          case BOT_GQ:
            e0 = new Lit(Location(),d);
            e1 = alv[0]();
            bot = BOT_LQ;
            break;
          default:
            e0 = alv[0]();
            e1 = new Lit(Location(),d);
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
      int coeff_sign;
      LinearTraits<Lit>::constructLinBuiltin(bot,callid,coeff_sign,d);
      GCLock lock;
      std::vector<Expression*> coeff_ev(coeffv.size());
      for (unsigned int i=coeff_ev.size(); i--;)
        coeff_ev[i] = new Lit(Location(),coeff_sign*coeffv[i]);
      ArrayLit* ncoeff = new ArrayLit(Location(),coeff_ev);
      Type t = coeff_ev[0]->type();
      t._dim = 1;
      ncoeff->type(t);
      args.push_back(ncoeff);
      std::vector<Expression*> alv_e(alv.size());
      Type tt = alv[0]()->type();
      tt._dim = 1;
      for (unsigned int i=alv.size(); i--;) {
        if (alv[i]()->type().isvar())
          tt._ti = Type::TI_VAR;
        alv_e[i] = alv[i]();
      }
      ArrayLit* nal = new ArrayLit(Location(),alv_e);
      nal->type(tt);
      args.push_back(nal);
      Lit* il = new Lit(Location(),-d);
      args.push_back(il);
    }
  }
  
  template<class Lit>
  void flatten_linexp_call(EnvI& env, Ctx ctx, Ctx nctx, ASTString& cid, Call* c,
                           EE& ret, VarDecl* b, VarDecl* r,
                           std::vector<EE>& args_ee, std::vector<KeepAlive>& args) {
    typedef typename LinearTraits<Lit>::Val Val;
    Expression* al_arg = (cid==constants().ids.sum ? c->args()[0] : c->args()[1]);
    EE flat_al = flat_exp(env,nctx,al_arg,NULL,NULL);
    ArrayLit* al = follow_id(flat_al.r())->template cast<ArrayLit>();
    Val d = (cid == constants().ids.sum ? Val(0) : LinearTraits<Lit>::eval(c->args()[2]));
    
    std::vector<Val> c_coeff(al->v().size());
    if (cid==constants().ids.sum) {
      for (unsigned int i=al->v().size(); i--;)
        c_coeff[i] = 1;
    } else {
      EE flat_coeff = flat_exp(env,nctx,c->args()[0],NULL,NULL);
      ArrayLit* coeff = follow_id(flat_coeff.r())->template cast<ArrayLit>();
      for (unsigned int i=coeff->v().size(); i--;)
        c_coeff[i] = LinearTraits<Lit>::eval(coeff->v()[i]);
    }
    cid = constants().ids.lin_exp;
    std::vector<Val> coeffv;
    std::vector<KeepAlive> alv;
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (Call* sc = same_call(al->v()[i],cid)) {
        Val cd = c_coeff[i];
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(sc->args()[0]);
        ArrayLit* sc_al = eval_array_lit(sc->args()[1]);
        Val sc_d = LinearTraits<Lit>::eval(sc->args()[2]);
        assert(sc_coeff->v().size() == sc_al->v().size());
        for (unsigned int j=0; j<sc_coeff->v().size(); j++) {
          coeffv.push_back(cd*LinearTraits<Lit>::eval(sc_coeff->v()[j]));
          alv.push_back(sc_al->v()[j]);
        }
        d += cd*sc_d;
      } else {
        coeffv.push_back(c_coeff[i]);
        alv.push_back(al->v()[i]);
      }
    }
    simplify_lin<Lit>(coeffv,alv,d);
    if (coeffv.size()==0) {
      GCLock lock;
      ret.b = conj(env,b,Ctx(),args_ee);
      ret.r = bind(env,ctx,r,new Lit(Location(),d));
      return;
    } else if (coeffv.size()==1 && coeffv[0]==1 && d==0) {
      ret.b = conj(env,b,Ctx(),args_ee);
      ret.r = bind(env,ctx,r,alv[0]());
      return;
    }
    GCLock lock;
    std::vector<Expression*> coeff_ev(coeffv.size());
    for (unsigned int i=coeff_ev.size(); i--;)
      coeff_ev[i] = new Lit(Location(),coeffv[i]);
    ArrayLit* ncoeff = new ArrayLit(Location(),coeff_ev);
    Type t = coeff_ev[0]->type();
    t._dim = 1;
    ncoeff->type(t);
    args.push_back(ncoeff);
    std::vector<Expression*> alv_e(alv.size());
    bool al_same_as_before = alv.size()==al->v().size();
    for (unsigned int i=alv.size(); i--;) {
      alv_e[i] = alv[i]();
      al_same_as_before = al_same_as_before && Expression::equal(alv_e[i],al->v()[i]);
    }
    if (al_same_as_before) {
      Expression* rd = follow_id_to_decl(flat_al.r());
      if (rd->isa<VarDecl>())
        rd = rd->cast<VarDecl>()->id();
      if (rd->type().dim()>1) {
        ArrayLit* al = eval_array_lit(rd);
        std::vector<std::pair<int,int> > dims(1);
        dims[0].first = 1;
        dims[0].second = al->v().size();
        rd = new ArrayLit(al->loc(),al->v(),dims);
        Type t = al->type();
        t._dim = 1;
        rd->type(t);
      }
      args.push_back(rd);
    } else {
      ArrayLit* nal = new ArrayLit(al->loc(),alv_e);
      nal->type(al->type());
      args.push_back(nal);
    }
    Lit* il = new Lit(Location(),d);
    args.push_back(il);
  }
  
  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
    CallStackItem _csi(env,e);
    EE ret;
    assert(!e->type().isunknown());
    if (e->type().ispar() && !e->isa<Let>() && !e->isa<VarDecl>() && e->type()._bt!=Type::BT_ANN) {
      ret.b = bind(env,Ctx(),b,constants().lit_true);
      if (e->type().dim() > 0) {
        EnvI::Map::iterator it;
        if (Id* id = e->dyn_cast<Id>()) {
          VarDecl* vd = id->decl()->flat();
          if (vd==NULL) {
            vd = flat_exp(env,Ctx(),id->decl(),NULL,constants().var_true).r()->cast<Id>()->decl();
            id->decl()->flat(vd);
          }
          ret.r = bind(env,ctx,r,e->cast<Id>()->decl()->flat()->id());
          return ret;
        } else if ( (it = env.map_find(e)) != env.map_end()) {
          ret.r = bind(env,ctx,r,it->second.r()->cast<VarDecl>()->id());
          return ret;
        } else {
          GCLock lock;
          ArrayLit* al = follow_id(eval_par(e))->cast<ArrayLit>();
          if (al->v().size()==0) {
            ret.r = bind(env,ctx,r,al);
            return ret;
          }
          if ( (it = env.map_find(al)) != env.map_end()) {
            ret.r = bind(env,ctx,r,it->second.r()->cast<VarDecl>()->id());
            return ret;
          }
          std::vector<TypeInst*> ranges(al->dims());
          for (unsigned int i=0; i<ranges.size(); i++) {
            ranges[i] = new TypeInst(e->loc(),
                                     Type(),
                                     new SetLit(Location(),IntSetVal::a(al->min(i),al->max(i))));
          }
          ASTExprVec<TypeInst> ranges_v(ranges);
          assert(!al->type().isbot());
          TypeInst* ti = new TypeInst(e->loc(),al->type(),ranges_v,NULL);
          VarDecl* vd = new VarDecl(e->loc(),ti,env.genId("a"),al);
          vd->introduced(true);
          vd->flat(vd);
          VarDeclI* ni = new VarDeclI(Location(),vd);
          env.flat_addItem(ni);
          EE ee(vd,NULL);
          env.map_insert(al,ee);
          env.map_insert(vd->e(),ee);
          env.map_insert(vd->id(),ee);
          
          ret.r = bind(env,ctx,r,vd->id());
          return ret;
        }
      }
      GCLock lock;
      ret.r = bind(env,ctx,r,eval_par(e));
      return ret;
    }
    switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_SETLIT:
    case Expression::E_STRINGLIT:
      {
        GCLock lock;
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,Ctx(),r,e);
        return ret;
      }
    case Expression::E_BOOLLIT:
      {
        GCLock lock;
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,ctx,r,e);
        return ret;
      }
      break;
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->decl()==NULL) {
          if (id->type().isann()) {
            ret.b = bind(env,Ctx(),b,constants().lit_true);
            ret.r = bind(env,ctx,r,e);
            return ret;
          } else {
            throw FlatteningError(e->loc(), "undefined identifier");
          }
        }
        if (ctx.neg && id->type()._dim > 0) {
          if (id->type()._dim > 1)
            throw InternalError("multi-dim arrays in negative positions not supported yet");
          KeepAlive ka;
          {
            GCLock lock;
            std::vector<ASTString> gen_id(1);
            gen_id[0] = env.genId(id->v().str()+"_idx");
            /// TODO: support arbitrary dimensions
            std::vector<Expression*> idxsetargs(1);
            idxsetargs[0] = id;
            Call* idxset = new Call(id->loc(),"index_set",idxsetargs);
            idxset->decl(env.orig->matchFn(idxset));
            idxset->type(idxset->decl()->rtype(idxsetargs));
            Generator gen(gen_id,idxset);
            std::vector<Expression*> idx(1);
            Generators gens;
            gens._g.push_back(gen);
            gens._w = NULL;
            UnOp* aanot = new UnOp(id->loc(),UOT_NOT,NULL);
            Comprehension* cp = new Comprehension(id->loc(),
              aanot, gens, false);
            Id* bodyidx = cp->decl(0,0)->id();
            idx[0] = bodyidx;
            ArrayAccess* aa = new ArrayAccess(id->loc(),id,idx);
            aanot->e(aa);
            Type tt = id->type();
            tt._dim = 0;
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
            // New top-level id, need to copy into env.m
            vd = flat_exp(env,Ctx(),id->decl(),NULL,constants().var_true).r()
                 ->cast<Id>()->decl();
          }
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          if (vd && vd->e()!=NULL) {
            switch (vd->e()->eid()) {
            case Expression::E_INTLIT:
            case Expression::E_BOOLLIT:
            case Expression::E_FLOATLIT:
            case Expression::E_ID:
              rete = vd->e();
              break;
            default: break;
            }
          } else if (vd && vd->ti()->ranges().size() > 0) {
            // create fresh variables and array literal
            std::vector<std::pair<int,int> > dims;
            IntVal asize = 1;
            for (unsigned int i=0; i<vd->ti()->ranges().size(); i++) {
              TypeInst* ti = vd->ti()->ranges()[i];
              if (ti->domain()==NULL)
                throw FlatteningError(ti->loc(),"array dimensions unknown");
              IntSetVal* isv = eval_intset(ti->domain());
              if (isv->size() == 0) {
                dims.push_back(std::pair<int,int>(1,0));
                asize = 0;
              } else {
                if (isv->size() != 1)
                  throw FlatteningError(ti->loc(),"invalid array index set");
                asize *= (isv->max(0)-isv->min(0)+1);
                dims.push_back(std::pair<int,int>(isv->min(0).toInt(),isv->max(0).toInt()));
              }
            }
            Type tt = vd->ti()->type();
            tt._dim = 0;
            TypeInst* vti = new TypeInst(Location(),tt,vd->ti()->domain());
            
            std::vector<Expression*> elems(asize.toInt());
            for (int i=0; i<asize; i++) {
              ASTString nid = env.genId("fresh_"+vd->id()->v().str());
              VarDecl* nvd = new VarDecl(vd->loc(),vti,nid);
              nvd->introduced(vd->introduced());
              EE root_vd = flat_exp(env,Ctx(),nvd,NULL,constants().var_true);
              Id* id = root_vd.r()->cast<Id>();
              elems[i] = id;
            }

            ArrayLit* al = new ArrayLit(Location(),elems,dims);
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
                if (vdea && vdea->v().size()==0) {
                  // Do not create names for empty arrays but return array literal directly
                  rete = vdea;
                } else {
                  VarDecl* nvd =
                  new VarDecl(vd->loc(),eval_typeinst(env,vd),
                              env.genId("tl_"+vd->id()->v().str()),vd->e());
                  nvd->introduced(true);
                  for (ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
                    EE ee_ann = flat_exp(env, Ctx(), *it, NULL, constants().var_true);
                    nvd->addAnnotation(ee_ann.r());
                  }
                  
                  VarDeclI* ni = new VarDeclI(Location(),nvd);
                  env.flat_addItem(ni);
                  vd = nvd;
                  EE ee(vd,NULL);
                  if (vd->e())
                    env.map_insert(vd->e(),ee);
                  env.map_insert(nvd->id(),ee);
                }
              } else {
                vd = it->second.r()->cast<VarDecl>();
              }
            }
            if (rete==NULL) {
              if (id->type()._bt == Type::BT_ANN && vd->e()) {
                rete = vd->e();
              } else {
                rete = vd->id();
              }
            }
          }
          ret.r = bind(env,ctx,r,rete);
        }
      }
      break;
    case Expression::E_ANON:
      {
        AnonVar* av = e->cast<AnonVar>();
        if (av->type().isbot()) {
          throw InternalError("type of anonymous variable could not be inferred");
        }
        GCLock lock;
        VarDecl* vd = new VarDecl(Location(), new TypeInst(Location(), av->type()),
                                  env.genId("Anon"));
        ret = flat_exp(env,Ctx(),vd,NULL,constants().var_true);
      }
      break;
    case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        std::vector<EE> elems_ee(al->v().size());
        for (unsigned int i=al->v().size(); i--;)
          elems_ee[i] = flat_exp(env,ctx,al->v()[i],NULL,NULL);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r();
        std::vector<std::pair<int,int> > dims(al->dims());
        for (unsigned int i=al->dims(); i--;)
          dims[i] = std::pair<int,int>(al->min(i), al->max(i));
        KeepAlive ka;
        {
          GCLock lock;
          ArrayLit* alr = new ArrayLit(Location(),elems,dims);
          alr->type(al->type());
          ka = alr;
        }
        ret.b = conj(env,b,Ctx(),elems_ee);
        ret.r = bind(env,ctx,r,ka());
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        
        std::vector<EE> ees(aa->idx().size());
        Ctx dimctx = ctx;
        dimctx.neg = false;
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          Expression* tmp = follow_id_to_decl(aa->idx()[i]);
          if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
            tmp = vd->id();
          ees[i] = flat_exp(env, dimctx, tmp, NULL, NULL);
        }
        
        bool parAccess=true;
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          if (!ees[i].r()->type().ispar()) {
            parAccess = false;
            break;
          }
        }
        Ctx nctx = ctx;
        nctx.b = +nctx.b;
        nctx.neg = false;
        EE eev = flat_exp(env,nctx,aa->v(),NULL,NULL);
        ees.push_back(EE(NULL,eev.b()));

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
              dims[i] = eval_int(ees[i].r());
            ka = eval_arrayaccess(al,dims,success);
          }
          ees.push_back(EE(NULL,constants().boollit(success)));
          if (aa->type().isbool()) {
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
            Call* cc = new Call(Location(),"element",args);
            cc->type(aa->type());
            FunctionI* fi = env.orig->matchFn(cc->id(),args);
            assert(fi);
            assert(cc->type() == fi->rtype(args));
            cc->decl(fi);
            ka = cc;
          }
          EE ee = flat_exp(env,ctx,ka(),NULL,NULL);
          ees.push_back(ee);
          if (aa->type().isbool()) {
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
        Comprehension* c = e->cast<Comprehension>();
        if (c->set()) {
          throw InternalError("not supported yet");
        }
        class EvalF {
        public:
          EnvI& env;
          Ctx ctx;
          EvalF(EnvI& env0, Ctx ctx0) : env(env0), ctx(ctx0) {}
          typedef EE ArrayVal;
          EE e(Expression* e) {
            if (ctx.b == C_ROOT && e->type().isbool()) {
              return flat_exp(env,ctx,e,constants().var_true,constants().var_true);
            } else {
              return flat_exp(env,ctx,e,NULL,NULL);
            }
          }
        } _evalf(env,ctx);
        std::vector<EE> elems_ee = eval_comp<EvalF>(_evalf,c);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r();
        KeepAlive ka;
        {
          GCLock lock;
          ArrayLit* alr = new ArrayLit(Location(),elems);
          alr->type(c->type());
          ka = alr;
        }
        ret.b = conj(env,b,Ctx(),elems_ee);
        ret.r = bind(env,Ctx(),r,ka());
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        KeepAlive ka = flat_ite(env,ite,0);
        ret = flat_exp(env,ctx,ka(),r,b);
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (isReverseMap(bo)) {
          CallArgItem cai(env);
          Id* id = bo->lhs()->dyn_cast<Id>();
          if (id==NULL)
            throw EvalError(bo->lhs()->loc(), "Reverse mappers are only defined for identifiers");
          if (bo->op() != BOT_EQ && bo->op() != BOT_EQUIV)
            throw EvalError(bo->loc(), "Reverse mappers have to use `=` as the operator");
          Call* c = bo->rhs()->dyn_cast<Call>();
          if (c==NULL)
            throw EvalError(bo->rhs()->loc(), "Reverse mappers require call on right hand side");

          std::vector<Expression*> args(c->args().size());
          for (unsigned int i=0; i<c->args().size(); i++) {
            Id* idi = c->args()[i]->dyn_cast<Id>();
            if (idi==NULL)
              throw EvalError(c->args()[i]->loc(), "Reverse mapper calls require identifiers as arguments");
            EE ee = flat_exp(env, Ctx(), idi, NULL, constants().var_true);
            args[i] = ee.r();
          }
          
          EE ee = flat_exp(env, Ctx(), id, NULL, constants().var_true);
          
          GCLock lock;
          Call* revMap = new Call(Location(),c->id(),args);
          
          args.push_back(ee.r());
          Call* keepAlive = new Call(Location(),constants().var_redef->id(),args);
          keepAlive->type(Type::varbool());
          keepAlive->decl(constants().var_redef);
          (void) flat_exp(env, Ctx(), keepAlive, constants().var_true, constants().var_true);
          
          env.reverseMappers.insert(std::pair<ASTString, KeepAlive>(ee.r()->cast<Id>()->v(),revMap));
          
          break;
        }
        if (bo->decl()) {
          std::vector<Expression*> args(2);
          args[0] = bo->lhs();
          args[1] = bo->rhs();
          KeepAlive ka;
          {
            GCLock lock;
            Call* cr = new Call(bo->loc(),bo->opToString().str(),args);
            cr->decl(env.orig->matchFn(cr));
            cr->type(cr->decl()->rtype(args));
            ka = cr;
          }
          ret = flat_exp(env,ctx,ka(),r,b);
        } else {
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
            {
              assert(!ctx0.neg);
              assert(!ctx1.neg);
              EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
              EE e1 = flat_exp(env,ctx1,boe1,NULL,NULL);

              if (bot==BOT_MULT) {
                Expression* e0r = e0.r();
                Expression* e1r = e1.r();
                if (e0r->type().ispar())
                  std::swap(e0r,e1r);
                if (e1r->type().ispar() && e1r->type().isint()) {
                  IntVal coeff = eval_int(e1r);
                  KeepAlive ka = mklinexp<IntLit>(env,coeff,0,e0r,NULL);
                  ret = flat_exp(env,ctx,ka(),r,b);
                  break;
                } else if (e1r->type().ispar() && e1r->type().isfloat()) {
                  FloatVal coeff = eval_float(e1r);
                  KeepAlive ka = mklinexp<FloatLit>(env,coeff,0.0,e0r,NULL);
                  ret = flat_exp(env,ctx,ka(),r,b);
                  break;
                }
              }
              
              GC::lock();
              std::vector<Expression*> args(2);
              args[0] = e0.r(); args[1] = e1.r();
              Call* cc = new Call(bo->loc(),opToBuiltin(bo,bot),args);
              cc->type(bo->type());

              if (FunctionI* fi = env.orig->matchFn(cc->id(),args)) {
                assert(cc->type() == fi->rtype(args));
                cc->decl(fi);
                cc->type(cc->decl()->rtype(args));
                KeepAlive ka(cc);
                GC::unlock();
                EE ee = flat_exp(env,ctx,cc,r,NULL);
                GC::lock();
                ret.r = ee.r;
                std::vector<EE> ees(3);
                ees[0].b = e0.b; ees[1].b = e1.b; ees[2].b = ee.b;
                ret.b = conj(env,b,Ctx(),ees);
              } else {
                ret.r = bind(env,ctx,r,cc);
                std::vector<EE> ees(2);
                ees[0].b = e0.b; ees[1].b = e1.b;
                ret.b = conj(env,b,Ctx(),ees);
              }
            }
            GC::unlock();
            break;

        case BOT_AND:
            {
              if (r==constants().var_true) {
                Ctx ctx;
                ctx.neg = negArgs;
                ctx.b = negArgs ? C_NEG : C_ROOT;
                (void) flat_exp(env,ctx,boe0,constants().var_true,constants().var_true);
                (void) flat_exp(env,ctx,boe1,constants().var_true,constants().var_true);
                break;
              } else {
                GC::lock();
                std::vector<Expression*> bo_args(2);
                if (negArgs) {
                  bo_args[0] = new UnOp(bo->loc(),UOT_NOT,boe0);
                  bo_args[0]->type(boe0->type());
                  bo_args[1] = new UnOp(bo->loc(),UOT_NOT,boe1);
                  bo_args[1]->type(boe1->type());
                } else {
                  bo_args[0] = boe0;
                  bo_args[1] = boe1;
                }
                std::vector<Expression*> args(1);
                args[0]=new ArrayLit(bo->loc(),bo_args);
                args[0]->type(Type::varbool(1));
                Call* c = new Call(bo->loc(),constants().ids.forall,args);
                c->decl(env.orig->matchFn(c));
                c->type(c->decl()->rtype(args));
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
              std::vector<Expression*> bo_args(2);
              if (negArgs) {
                bo_args[0] = new UnOp(bo->loc(),UOT_NOT,boe0);
                bo_args[0]->type(boe0->type());
                bo_args[1] = new UnOp(bo->loc(),UOT_NOT,boe1);
                bo_args[1]->type(boe1->type());
              } else {
                bo_args[0] = boe0;
                bo_args[1] = boe1;
              }
              std::vector<Expression*> args(1);
              args[0]= new ArrayLit(bo->loc(),bo_args);
              args[0]->type(Type::varbool(1));
              Call* c = new Call(bo->loc(),constants().ids.exists,args);
              c->decl(env.orig->matchFn(c));
              c->type(c->decl()->rtype(args));
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
              GC::lock();
              std::vector<Expression*> bo_args(2);
              ASTString id;
              if (ctx.neg) {
                bo_args[0] = boe0;
                bo_args[1] = new UnOp(bo->loc(),UOT_NOT,boe1);
                bo_args[1]->type(boe1->type());
                id = constants().ids.forall;
              } else {
                bo_args[0] = new UnOp(bo->loc(),UOT_NOT,boe0);
                bo_args[0]->type(boe0->type());
                bo_args[1] = boe1;
                id = constants().ids.exists;
              }
              ctx.neg = false;
              std::vector<Expression*> args(1);
              args[0]= new ArrayLit(bo->loc(),bo_args);
              args[0]->type(Type::varbool(1));
              Call* c = new Call(bo->loc(),id,args);
              c->decl(env.orig->matchFn(c));
              c->type(c->decl()->rtype(args));
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
              if (r && r==constants().var_true) {
                if (boe1->type().ispar() || boe1->isa<Id>())
                  std::swap(boe0,boe1);
                if (istrue(boe0)) {
                  return flat_exp(env,ctx1,boe1,r,b);
                } else if (isfalse(boe0)) {
                  ctx1.neg = true;
                  ctx1.b = -ctx1.b;
                  return flat_exp(env,ctx1,boe1,r,b);
                } else {
                  ctx0.b = C_MIX;
                  EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
                  if (istrue(e0.r())) {
                    return flat_exp(env,ctx1,boe1,r,b);
                  } else if (isfalse(e0.r())) {
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
              ctx0.b = ctx1.b = C_MIX;
            }
            goto flatten_bool_op;
          case BOT_LE:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_GQ;
              if (boe0->type().isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            } else {
              if (boe0->type().isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            }
            goto flatten_bool_op;
          case BOT_LQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_GR;
              if (boe0->type().isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            } else {
              if (boe0->type().isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            }
            goto flatten_bool_op;
          case BOT_GR:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_LQ;
              if (boe0->type().isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            } else {
              if (boe0->type().isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            }
            goto flatten_bool_op;
          case BOT_GQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_LE;
              if (boe0->type().isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            } else {
              if (boe0->type().isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().isint()) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            }
            goto flatten_bool_op;
          case BOT_EQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_NQ;
            }
            if (boe0->type().isbool()) {
              ctx0.b = ctx1.b = C_MIX;
            } else if (boe0->type().isint()) {
              ctx0.i = ctx1.i = C_MIX;
            }
            goto flatten_bool_op;
          case BOT_NQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_EQ;
            }
            if (boe0->type().isbool()) {
              ctx0.b = ctx1.b = C_MIX;
            } else if (boe0->type().isint()) {
              ctx0.i = ctx1.i = C_MIX;
            }
            goto flatten_bool_op;
          case BOT_IN:
          case BOT_SUBSET:
          case BOT_SUPERSET:
            ctx0.i = ctx1.i = C_MIX;
          flatten_bool_op:
            {
              EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
              EE e1 = flat_exp(env,ctx1,boe1,NULL,NULL);
              
              ret.b = bind(env,Ctx(),b,constants().lit_true);

              std::vector<EE> ees(3);
              ees[0].b = e0.b; ees[1].b = e1.b;

              if (e0.r()->type().ispar() && e1.r()->type().ispar()) {
                GCLock lock;
                BinOp* bo_par = new BinOp(e->loc(),e0.r(),bot,e1.r());
                bo_par->type(Type::parbool());
                bool bo_val = eval_bool(bo_par);
                if (doubleNeg)
                  bo_val = !bo_val;
                ees[2].b = constants().boollit(bo_val);
                ret.r = conj(env,r,ctx,ees);
                break;
              }

              if (ctx.b==C_ROOT && r==constants().var_true && e1.r()->type().ispar() &&
                  e0.r()->isa<Id>() && (bot==BOT_IN || bot==BOT_SUBSET) ) {
                VarDecl* vd = e0.r()->cast<Id>()->decl();
                if (vd->ti()->domain()==NULL) {
                  vd->ti()->domain(e1.r());
                } else {
                  GCLock lock;
                  IntSetVal* newdom = eval_intset(e1.r());
                  Id* id = vd->id();
                  while (id != NULL) {
                    bool changeDom = false;
                    if (id->decl()->ti()->domain()) {
                      IntSetVal* domain = eval_intset(id->decl()->ti()->domain());
                      IntSetRanges dr(domain);
                      IntSetRanges ibr(newdom);
                      Ranges::Inter<IntSetRanges,IntSetRanges> i(dr,ibr);
                      IntSetVal* newibv = IntSetVal::ai(i);
                      if (domain->card() != newibv->card()) {
                        newdom = newibv;
                        changeDom = true;
                      }
                    } else {
                      changeDom = true;
                    }
                    if (id->type()._st==Type::ST_PLAIN && newdom->size()==0) {
                      std::cerr << "Warning: model inconsistency detected";
                      env.flat_addItem(new ConstraintI(Location(),constants().lit_false));
                    } else if (changeDom) {
                      id->decl()->ti()->setComputedDomain(false);
                      id->decl()->ti()->domain(new SetLit(Location(),newdom));
                    }
                    id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
                  }
                  
                }
                break;
              }
              
              std::vector<KeepAlive> args;
              ASTString callid;

              Expression* le0 = NULL;
              Expression* le1 = NULL;
              
              if (boe0->type().isint() && bot != BOT_IN) {
                le0 = get_linexp<IntLit>(e0.r());
              } else if (boe0->type().isfloat() && bot != BOT_IN) {
                le0 = get_linexp<FloatLit>(e0.r());
              }
              if (le0) {
                if (boe1->type().isint()) {
                  le1 = get_linexp<IntLit>(e1.r());
                } else if (boe1->type().isfloat()) {
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
                args.push_back(e0.r);
                args.push_back(e1.r);
              }

              if (args.size() > 0) {
                GC::lock();
                
                if (callid=="") {
                  callid = opToBuiltin(bo,bot);
                }
                
                std::vector<Expression*> args_e(args.size());
                for (unsigned int i=args.size(); i--;)
                  args_e[i] = args[i]();
                Call* cc = new Call(Location(),callid,args_e);
                cc->type(bo->type());

                EnvI::Map::iterator cit = env.map_find(cc);
                if (cit != env.map_end()) {
                  ees[2].b = cit->second.r();
                  if (doubleNeg) {
                    Type t = ees[2].b()->type();
                    ees[2].b = new UnOp(Location(),UOT_NOT,ees[2].b());
                    ees[2].b()->type(t);
                  }
                  if (Id* id = ees[2].b()->dyn_cast<Id>()) {
                    addCtxAnn(id->decl(),ctx.b);
                  }
                  ret.r = conj(env,r,ctx,ees);
                  GC::unlock();
                } else {
                  cc->decl(env.orig->matchFn(cc->id(),args_e));
                  cc->type(cc->decl()->rtype(args_e));
                  assert(cc->decl());
                  bool singleExp = true;
                  for (unsigned int i=0; i<ees.size(); i++) {
                    if (!istrue(ees[i].b())) {
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
                      ees[2].b = new UnOp(Location(),UOT_NOT,ees[2].b());
                      ees[2].b()->type(t);
                    }
                    if (Id* id = ees[2].b()->dyn_cast<Id>()) {
                      addCtxAnn(id->decl(),ctx.b);
                    }
                    ret.r = conj(env,r,ctx,ees);
                  }
                  if (!ctx.neg)
                    env.map_insert(cc,ret);
                }
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
              std::vector<Expression*> v(al0->v().size()+al1->v().size());
              for (unsigned int i=al0->v().size(); i--;)
                v[i] = al0->v()[i];
              for (unsigned int i=al1->v().size(); i--;)
                v[al0->v().size()+i] = al1->v()[i];
              GCLock lock;
              ArrayLit* alret = new ArrayLit(e->loc(),v);
              alret->type(e->type());
              ret.b = conj(env,b,Ctx(),ee);
              ret.r = bind(env,ctx,r,alret);
            }
            break;

          case BOT_DOTDOT:
            throw InternalError("not yet implemented");
          }
        }
      }
      break;
    case Expression::E_UNOP:
      {
        GCLock lock;
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
            BinOp* bo = new BinOp(Location(),new IntLit(Location(),0),
                                 BOT_MINUS,uo->e());
            bo->type(uo->type());
            ret = flat_exp(env,ctx,bo,r,b);
          }
          break;
        default: break;
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        FunctionI* decl = env.orig->matchFn(c);
        if (decl == NULL) {
          throw InternalError("undeclared function or predicate "
                              +c->id().str());
        }

        Ctx nctx = ctx;
        nctx.neg = false;
        ASTString cid = c->id();
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
            Expression* callres = decl->_builtins.e(c->args());
            ret = flat_exp(env,ctx,callres,r,b);
            // This is all we need to do for assert, so break out of the E_CALL
            break;
          }
        }

        if (ctx.b==C_ROOT && decl->e()==NULL &&
            cid == constants().ids.forall && r==constants().var_true) {
          /// TODO: need generic array evaluation function
          ret.b = bind(env,ctx,b,constants().lit_true);
          EE flat_al = flat_exp(env,Ctx(),c->args()[0],NULL,constants().var_true);
          ArrayLit* al = follow_id(flat_al.r())->cast<ArrayLit>();
          nctx.b = C_ROOT;
          for (unsigned int i=0; i<al->v().size(); i++)
            (void) flat_exp(env,nctx,al->v()[i],r,b);
        } else {
          std::vector<EE> args_ee(c->args().size());
          bool mixContext = decl->e()!=NULL ||
            (cid != constants().ids.forall && cid != constants().ids.exists && cid != constants().ids.bool2int &&
             cid != constants().ids.sum && cid != constants().ids.lin_exp && cid != "assert");
          for (unsigned int i=c->args().size(); i--;) {
            Ctx argctx = nctx;
            if (mixContext) {
              if (c->args()[i]->type()._bt==Type::BT_BOOL) {
                argctx.b = C_MIX;
              } else if (c->args()[i]->type()._bt==Type::BT_INT) {
                argctx.i = C_MIX;
              }
            }
            Expression* tmp = follow_id_to_decl(c->args()[i]);
            if (VarDecl* vd = tmp->dyn_cast<VarDecl>())
              tmp = vd->id();
            CallArgItem cai(env);
            args_ee[i] = flat_exp(env,argctx,tmp,NULL,NULL);
          }

          std::vector<KeepAlive> args;
          if (decl->e()==NULL && (cid == constants().ids.forall || cid == constants().ids.exists)) {
            ArrayLit* al = follow_id(args_ee[0].r())->cast<ArrayLit>();
            std::vector<KeepAlive> alv;
            for (unsigned int i=0; i<al->v().size(); i++) {
              if (Call* sc = same_call(al->v()[i],cid)) {
                GCLock lock;
                ArrayLit* sc_c = eval_array_lit(sc->args()[0]);
                for (unsigned int j=0; j<sc_c->v().size(); j++) {
                  alv.push_back(sc_c->v()[j]);
                }
              } else {
                alv.push_back(al->v()[i]);
              }
            }
            if (cid == constants().ids.exists) {
              std::vector<KeepAlive> pos_alv;
              std::vector<KeepAlive> neg_alv;
              for (unsigned int i=0; i<alv.size(); i++) {
                Call* neg_call = same_call(alv[i](),constants().ids.bool_eq);
                if (neg_call && 
                    Expression::equal(neg_call->args()[1],constants().lit_false)) {
                  neg_alv.push_back(neg_call->args()[0]);
                } else {
                  Call* clause = same_call(alv[i](),constants().ids.clause);
                  if (clause) {
                    ArrayLit* clause_pos = eval_array_lit(clause->args()[0]);
                    for (unsigned int j=0; j<clause_pos->v().size(); j++) {
                      pos_alv.push_back(clause_pos->v()[j]);
                    }
                    ArrayLit* clause_neg = eval_array_lit(clause->args()[1]);
                    for (unsigned int j=0; j<clause_neg->v().size(); j++) {
                      neg_alv.push_back(clause_neg->v()[j]);
                    }
                  } else {
                    pos_alv.push_back(alv[i]);
                  }
                }
              }
              remove_dups(pos_alv,false);
              remove_dups(neg_alv,true);
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
                ArrayLit* nal = new ArrayLit(al->loc(),toExpVec(pos_alv));
                nal->type(al->type());
                args.push_back(nal);
              } else {
                GCLock lock;
                ArrayLit* pos_al = new ArrayLit(al->loc(),toExpVec(pos_alv));
                pos_al->type(al->type());
                ArrayLit* neg_al = new ArrayLit(al->loc(),toExpVec(neg_alv));
                neg_al->type(al->type());
                cid = constants().ids.clause;
                args.push_back(pos_al);
                args.push_back(neg_al);
              }
            } else /* cid=="forall" */ {
              remove_dups(alv,true);
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
            }
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
            Call* cr_c = new Call(Location(),cid,e_args);
            decl = env.orig->matchFn(cr_c);
            cr_c->type(decl->rtype(e_args));
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
                  if (args[i]()->type()._bt == Type::BT_INT) {
                    GCLock lock;
                    IntSetVal* isv = eval_intset(dom);
                    BinOpType bot;
                    bool needToConstrain;
                    if (args[i]()->type()._st == Type::ST_SET) {
                      bot = BOT_SUBSET;
                      needToConstrain = true;
                    } else {
                      bot = BOT_IN;
                      if (args[i]()->type().dim() > 0) {
                        needToConstrain = true;
                      } else {
                        IntBounds ib = compute_int_bounds(args[i]());
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
                        Call* c = new Call(Location(),"var_dom",domargs);
                        c->type(Type::varbool());
                        c->decl(env.orig->matchFn(c));
                        domconstraint = c;
                      } else {
                        domconstraint = new BinOp(Location(),args[i](),bot,dom);
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
                  } else if (args[i]()->type()._bt == Type::BT_BOT) {
                    // Nothing to be done for empty arrays/sets
                  } else {
                    throw EvalError(decl->params()[i]->loc(),"domain restrictions other than int not supported yet");
                  }
                }
              }
            }
            if (cr()->type().isbool() && (ctx.b != C_ROOT || r != constants().var_true)) {
              GCLock lock;
              VarDecl* reif_b = r;
              if (reif_b == NULL) {
                VarDecl* nvd = new VarDecl(Location(), new TypeInst(Location(),Type::varbool()), env.genId("reif"));
                nvd->type(Type::varbool());
                nvd->introduced(true);
                (void) flat_exp(env, Ctx(), nvd, NULL, constants().var_true);
                reif_b = nvd->flat();
              }
              args.push_back(reif_b->id());
              Call* cr_real = new Call(Location(),cid.str()+"_reif",toExpVec(args));
              cr_real->type(Type::varbool());
              FunctionI* decl_real = env.orig->matchFn(cr_real);
              if (decl_real && decl_real->e()) {
                cr_real->decl(decl_real);
                bool ignorePartial = env.ignorePartial;
                env.ignorePartial = true;
                flat_exp(env,Ctx(),cr_real,constants().var_true,constants().var_true);
                env.ignorePartial = ignorePartial;
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                args_ee.push_back(EE(NULL,reif_b->id()));
                ret.r = conj(env,NULL,ctx,args_ee);
              } else {
                args.pop_back();
                goto call_nonreif;
              }
            } else {
            call_nonreif:
              if (decl->e()==NULL) {
                Call* cr_c = cr()->cast<Call>();
                /// All builtins are total
                std::vector<Type> argt(cr_c->args().size());
                for (unsigned int i=argt.size(); i--;)
                  argt[i] = cr_c->args()[i]->type();
                Type callt = decl->rtype(argt);
                if (callt.ispar() && callt._bt!=Type::BT_ANN) {
                  GCLock lock;
                  ret.b = conj(env,b,Ctx(),args_ee);
                  ret.r = bind(env,ctx,r,eval_par(cr_c));
                  // Do not insert into map, since par results will quickly become
                  // garbage anyway and then disappear from the map
                } else if (decl->_builtins.e) {
                  Expression* callres = decl->_builtins.e(cr_c->args());
                  EE res = flat_exp(env,ctx,callres,r,b);
                  args_ee.push_back(res);
                  ret.b = conj(env,b,Ctx(),args_ee);
                  ret.r = bind(env,ctx,r,res.r());
                  if (!ctx.neg)
                    env.map_insert(cr_c,ret);
                } else {
                  ret.b = conj(env,b,Ctx(),args_ee);
                  ret.r = bind(env,ctx,r,cr_c);
                  if (!ctx.neg)
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
                
                if (decl->e()->type().isbool()) {
                  ret.b = bind(env,Ctx(),b,constants().lit_true);
                  if (ctx.b==C_ROOT && r==constants().var_true) {
                    (void) flat_exp(env,Ctx(),decl->e(),r,constants().var_true);
                  } else {
                    EE ee = flat_exp(env,Ctx(),decl->e(),NULL,constants().var_true);
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
                  }
                  ret.b = conj(env,b,Ctx(),args_ee);
                }
                if (!ctx.neg)
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
      }
      break;
    case Expression::E_VARDECL:
      {
        GCLock lock;
        if (ctx.b != C_ROOT)
          throw FlatteningError(e->loc(), "not in root context");
        VarDecl* v = e->cast<VarDecl>();
        VarDecl* it = v->flat();
        if (it==NULL) {
          VarDecl* vd = new VarDecl(v->loc(),
                                    eval_typeinst(env,v),
                                    v->id()->v().str());
          vd->introduced(v->introduced());
          vd->flat(vd);
          v->flat(vd);
          for (ExpressionSetIter it = v->ann().begin(); it != v->ann().end(); ++it) {
            vd->addAnnotation(flat_exp(env,Ctx(),*it,NULL,constants().var_true).r());
          }
          VarDeclI* nv = new VarDeclI(Location(),vd);
          env.flat_addItem(nv);
          Ctx nctx;
          if (v->e() && v->e()->type()._bt == Type::BT_BOOL)
            nctx.b = C_MIX;
          if (v->e()) {
            (void) flat_exp(env,nctx,v->e(),vd,constants().var_true);
            if (v->e()->type()._bt==Type::BT_INT && v->e()->type().dim()==0) {
              IntSetVal* ibv = NULL;
              if (v->e()->type().isset()) {
                ibv = compute_intset_bounds(v->e());
              } else {
                IntBounds ib = compute_int_bounds(v->e());
                if (ib.valid)
                  ibv = IntSetVal::a(ib.l,ib.u);
              }
              if (ibv) {
                if (vd->ti()->domain()) {
                  IntSetVal* domain = eval_intset(vd->ti()->domain());
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(ibv);
                  Ranges::Inter<IntSetRanges,IntSetRanges> i(dr,ibr);
                  IntSetVal* newibv = IntSetVal::ai(i);
                  if (ibv->card() == newibv->card()) {
                    vd->ti()->setComputedDomain(true);
                  } else {
                    ibv = newibv;
                  }
                } else {
                  vd->ti()->setComputedDomain(true);
                }
                vd->ti()->domain(new SetLit(Location(),ibv));
              }
            } else if (v->e()->type().dim() > 0) {
              Expression* ee = follow_id_to_decl(vd->e());
              if (ee->isa<VarDecl>())
                ee = ee->cast<VarDecl>()->e();
              assert(ee && ee->isa<ArrayLit>());
              ArrayLit* al = ee->cast<ArrayLit>();
              if (vd->ti()->domain()) {
                for (unsigned int i=0; i<al->v().size(); i++) {
                  if (Id* ali_id = al->v()[i]->dyn_cast<Id>()) {
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
              if (vd->e()->type()._bt==Type::BT_BOOL)
                nctx.b = C_MIX;

              EE ee = flat_exp(env,nctx,vd->e(),NULL,NULL);
              let_e = ee.r();
              cs.push_back(ee);
              if (vd->ti()->domain() != NULL) {
                GCLock lock;
                std::vector<Expression*> domargs(2);
                domargs[0] = ee.r();
                domargs[1] = vd->ti()->domain();
                Call* c = new Call(vd->ti()->loc(),"var_dom",domargs);
                c->type(Type::varbool());
                c->decl(env.orig->matchFn(c));
                VarDecl* b_b = (nctx.b==C_ROOT && b==constants().var_true) ? b : NULL;
                ee = flat_exp(env, nctx, c, NULL, b_b);
                cs.push_back(ee);
                ee.b = ee.r;
                cs.push_back(ee);
              }
            } else {
              if (ctx.b==C_NEG || ctx.b==C_MIX)
                throw FlatteningError(vd->loc(),
                  "free variable in non-positive context");
              GCLock lock;
              TypeInst* ti = eval_typeinst(env,vd);
              VarDecl* nvd = new VarDecl(vd->loc(),ti,
                                         env.genId("FromLet_"+vd->id()->v().str()));
              nvd->toplevel(true);
              nvd->introduced(true);
              nvd->flat(nvd);
              nvd->type(vd->type());
              for (ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
                EE ee_ann = flat_exp(env, Ctx(), *it, NULL, constants().var_true);
                nvd->addAnnotation(ee_ann.r());
              }
              VarDeclI* nv = new VarDeclI(Location(),nvd);
              env.flat_addItem(nv);
              let_e = nvd->id();
            }
            vd->e(let_e);
            flatmap.push_back(vd->flat());
            if (Id* id = let_e->dyn_cast<Id>()) {
              vd->flat(id->decl());
            } else {
              vd->flat(vd);
            }
          } else {
            if (ctx.b==C_ROOT) {
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
          ctx.neg = false;
          EE ee = flat_exp(env,nctx,let->in(),NULL,NULL);
          if (let->type().isbool()) {
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
    return ret;
  }
  
  bool cannotUseRHSForOutput(EnvI& env, Expression* e) {
    if (e==NULL)
      return true;

    class V : public EVisitor {
    public:
      EnvI& env;
      bool success;
      V(EnvI& env0) : env(env0), success(true) {}
      /// Visit anonymous variable
      void vAnonVar(const AnonVar&) { success = false; }
      /// Visit array literal
      void vArrayLit(const ArrayLit&) {}
      /// Visit array access
      void vArrayAccess(const ArrayAccess&) {}
      /// Visit array comprehension
      void vComprehension(const Comprehension&) {}
      /// Visit if-then-else
      void vITE(const ITE&) {}
      /// Visit binary operator
      void vBinOp(const BinOp&) {}
      /// Visit unary operator
      void vUnOp(const UnOp&) {}
      /// Visit call
      void vCall(Call& c) {
        std::vector<Type> tv(c.args().size());
        for (unsigned int i=c.args().size(); i--;) {
          tv[i] = c.args()[i]->type();
          tv[i]._ti = Type::TI_PAR;
        }
        FunctionI* decl = env.output->matchFn(c.id(), tv);
        Type t;
        if (decl==NULL) {
          FunctionI* origdecl = env.orig->matchFn(c.id(), tv);
          assert(origdecl != NULL);

          if (origdecl->e() && cannotUseRHSForOutput(env, origdecl->e())) {
            success = false;
          } else {
            decl = copy(env.cmap,origdecl)->cast<FunctionI>();
            env.output->registerFn(decl);
            env.output->addItem(decl);
            c.decl(decl);
          }
        }
        if (success) {
          t = decl->rtype(tv);
          if (!t.ispar())
            success = false;
        }
      }
      /// Visit let
      void vLet(const Let&) { success = false; }
      /// Visit variable declaration
      void vVarDecl(const VarDecl&) {}
      /// Visit type inst
      void vTypeInst(const TypeInst&) {}
      /// Visit TIId
      void vTIId(const TIId&) {}
      /// Determine whether to enter node
      bool enter(Expression* e) { return success; }
    } _v(env);
    topDown(_v, e);
    
    return !_v.success;
  }
  
  void removeIsOutput(VarDecl* vd) {
    if (vd==NULL)
      return;
    vd->ann().remove(constants().ann.output_var);
    vd->ann().removeCall(constants().ann.output_array);
  }
  
  void outputVarDecls(EnvI& env, Expression* e) {
    class O : public EVisitor {
    public:
      EnvI& env;
      O(EnvI& env0) : env(env0) {}
      void vId(Id& id) {
        VarDecl* vd = id.decl();
        VarDecl* reallyFlat = vd->flat();
        while (reallyFlat != reallyFlat->flat())
          reallyFlat = reallyFlat->flat();
        ExpressionMap<int>::iterator idx = env.output_vo.idx.find(reallyFlat);
        if (idx==env.output_vo.idx.end()) {
          VarDeclI* nvi = new VarDeclI(Location(), copy(env.cmap,vd)->cast<VarDecl>());
          Type t = nvi->e()->ti()->type();
          if (t._ti != Type::TI_PAR) {
            t._ti = Type::TI_PAR;
          }
          nvi->e()->ti()->type(t);
          nvi->e()->ti()->domain(NULL);
          nvi->e()->flat(vd->flat());
          nvi->e()->ann().clear();
          nvi->e()->introduced(false);
          id.decl(nvi->e());
          
          ASTStringMap<KeepAlive>::t::iterator it;
          if ( (it = env.reverseMappers.find(nvi->e()->id()->v())) != env.reverseMappers.end()) {
            Call* rhs = copy(env.cmap,it->second())->cast<Call>();
            {
              std::vector<Type> tv(rhs->args().size());
              for (unsigned int i=rhs->args().size(); i--;) {
                tv[i] = rhs->args()[i]->type();
                tv[i]._ti = Type::TI_PAR;
              }
              FunctionI* decl = env.output->matchFn(rhs->id(), tv);
              Type t;
              if (decl==NULL) {
                FunctionI* origdecl = env.orig->matchFn(rhs->id(), tv);
                assert(origdecl != NULL);
                decl = copy(env.cmap,origdecl)->cast<FunctionI>();
                env.output->registerFn(decl);
                env.output->addItem(decl);
                rhs->decl(decl);
              }
            }
            outputVarDecls(env,rhs);
            nvi->e()->e(rhs);
          } else if (cannotUseRHSForOutput(env, reallyFlat->e())) {
            assert(nvi->e()->flat());
            nvi->e()->e(NULL);
            if (nvi->e()->type().dim() == 0) {
              reallyFlat->addAnnotation(constants().ann.output_var);
            } else {
              std::vector<Expression*> args(reallyFlat->e()->type().dim());
              for (int i=0; i<args.size(); i++) {
                if (nvi->e()->ti()->ranges()[i]->domain() == NULL) {
                  args[i] = new SetLit(Location(), eval_intset(reallyFlat->ti()->ranges()[i]->domain()));
                } else {
                  args[i] = new SetLit(Location(), eval_intset(nvi->e()->ti()->ranges()[i]->domain()));
                }
              }
              ArrayLit* al = new ArrayLit(Location(), args);
              args.resize(1);
              args[0] = al;
              reallyFlat->addAnnotation(new Call(Location(),constants().ann.output_array,args,NULL));
            }
          } else {
            outputVarDecls(env, nvi->e()->e());
          }
          env.output_vo.add(reallyFlat, env.output->size());
          env.output_vo.add(nvi, env.output->size());
          CollectOccurrencesE ce(env.output_vo,nvi);
          topDown(ce, nvi->e());
          env.output->addItem(nvi);
        }
      }
    } _o(env);
    topDown(_o, e);
  }
  
  void createOutput(EnvI& e) {
    if (e.output->size() > 0) {
      // Adapt existing output model
      // (generated by repeated flattening)
      e.output_vo.clear();
      for (unsigned int i=0; i<e.output->size(); i++) {
        Item* item = (*e.output)[i];
        switch (item->iid()) {
          case Item::II_VD:
          {
            VarDecl* vd = item->cast<VarDeclI>()->e();
            ASTStringMap<KeepAlive>::t::iterator it;
            if (vd->e()==NULL) {
              if (vd->flat()->e() && vd->flat()->e()->type().ispar()) {
                VarDecl* reallyFlat = vd->flat();
                while (reallyFlat!=reallyFlat->flat())
                  reallyFlat=reallyFlat->flat();
                removeIsOutput(reallyFlat);
                Expression* flate = follow_id(reallyFlat->id());
                outputVarDecls(e,flate);
                vd->e(flate);
              } else if ( (it = e.reverseMappers.find(vd->id()->v())) != e.reverseMappers.end()) {
                Call* rhs = copy(e.cmap,it->second())->cast<Call>();
                std::vector<Type> tv(rhs->args().size());
                for (unsigned int i=rhs->args().size(); i--;) {
                  tv[i] = rhs->args()[i]->type();
                  tv[i]._ti = Type::TI_PAR;
                }
                FunctionI* decl = e.output->matchFn(rhs->id(), tv);
                Type t;
                if (decl==NULL) {
                  FunctionI* origdecl = e.orig->matchFn(rhs->id(), tv);
                  assert(origdecl != NULL);
                  decl = copy(e.cmap,origdecl)->cast<FunctionI>();
                  e.output->registerFn(decl);
                  e.output->addItem(decl);
                  rhs->decl(decl);
                }
                
                VarDecl* reallyFlat = vd->flat();
                while (reallyFlat!=reallyFlat->flat())
                  reallyFlat=reallyFlat->flat();
                removeIsOutput(reallyFlat);
                
                outputVarDecls(e,rhs);
                vd->e(rhs);
              } else {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                assert(vd->flat());
                if (!isOutput(vd->flat())) {
                  GCLock lock;
                  if (vd->type().dim() == 0) {
                    vd->flat()->addAnnotation(constants().ann.output_var);
                  } else {
                    std::vector<Expression*> args(vd->type().dim());
                    for (int i=0; i<args.size(); i++) {
                      if (vd->ti()->ranges()[i]->domain() == NULL) {
                        args[i] = new SetLit(Location(), eval_intset(vd->flat()->ti()->ranges()[i]->domain()));
                      } else {
                        args[i] = new SetLit(Location(), eval_intset(vd->ti()->ranges()[i]->domain()));
                      }
                    }
                    ArrayLit* al = new ArrayLit(Location(), args);
                    args.resize(1);
                    args[0] = al;
                    vd->flat()->addAnnotation(new Call(Location(),constants().ann.output_array,args,NULL));
                  }
                }
              }
            }
            e.output_vo.add(item->cast<VarDeclI>(), i);
            CollectOccurrencesE ce(e.output_vo,item);
            topDown(ce, vd);
          }
            break;
          case Item::II_OUT:
          {
            CollectOccurrencesE ce(e.output_vo,item);
            topDown(ce, item->cast<OutputI>()->e());
          }
            break;
          case Item::II_FUN:
          {
            CollectOccurrencesE ce(e.output_vo,item);
            topDown(ce, item->cast<FunctionI>()->e());
          }
            break;
          default:
            throw FlatteningError(item->loc(), "invalid item in output model");
        }
      }
    } else {
      // Create new output model
      OutputI* outputItem = NULL;
      
      class OV1 : public ItemVisitor {
      public:
        EnvI& env;
        VarOccurrences& vo;
        OutputI*& outputItem;
        OV1(EnvI& env0, VarOccurrences& vo0, OutputI*& outputItem0)
        : env(env0), vo(vo0), outputItem(outputItem0) {}
        void vOutputI(OutputI* oi) {
          GCLock lock;
          outputItem = copy(env.cmap, oi)->cast<OutputI>();
          env.output->addItem(outputItem);
        }
      } _ov1(e,e.output_vo,outputItem);
      iterItems(_ov1,e.orig);
      
      if (outputItem==NULL) {
        GCLock lock;
        outputItem = new OutputI(Location(),new ArrayLit(Location(),std::vector<Expression*>()));
        e.output->addItem(outputItem);
      }
      
      class OV2 : public ItemVisitor {
      public:
        EnvI& env;
        VarOccurrences& vo;
        OV2(EnvI& env0, VarOccurrences& vo0) : env(env0), vo(vo0) {}
        void vVarDeclI(VarDeclI* vdi) {
          if (Expression* vd_e = env.cmap.find(vdi->e())) {
            VarDecl* vd = vd_e->cast<VarDecl>();
            GCLock lock;
            VarDeclI* vdi_copy = copy(env.cmap,vdi)->cast<VarDeclI>();
            Type t = vdi_copy->e()->ti()->type();
            t._ti = Type::TI_PAR;
            vdi_copy->e()->ti()->type(t);
            vdi_copy->e()->ti()->domain(NULL);
            vdi_copy->e()->flat(vdi->e()->flat());
            vdi_copy->e()->ann().clear();
            vdi_copy->e()->introduced(false);
            ASTStringMap<KeepAlive>::t::iterator it;
            if (!vdi->e()->type().ispar()) {
              if (vd->flat()->e() && vd->flat()->e()->type().ispar()) {
                VarDecl* reallyFlat = vd->flat();
                while (reallyFlat!=reallyFlat->flat())
                  reallyFlat=reallyFlat->flat();
                Expression* flate = follow_id(reallyFlat->id());
                outputVarDecls(env,flate);
                vd->e(flate);
              } else if ( (it = env.reverseMappers.find(vd->id()->v())) != env.reverseMappers.end()) {
                Call* rhs = copy(env.cmap,it->second())->cast<Call>();
                {
                  std::vector<Type> tv(rhs->args().size());
                  for (unsigned int i=rhs->args().size(); i--;) {
                    tv[i] = rhs->args()[i]->type();
                    tv[i]._ti = Type::TI_PAR;
                  }
                  FunctionI* decl = env.output->matchFn(rhs->id(), tv);
                  Type t;
                  if (decl==NULL) {
                    FunctionI* origdecl = env.orig->matchFn(rhs->id(), tv);
                    assert(origdecl != NULL);
                    decl = copy(env.cmap,origdecl)->cast<FunctionI>();
                    env.output->registerFn(decl);
                    env.output->addItem(decl);
                    rhs->decl(decl);
                  }
                }
                outputVarDecls(env,rhs);
                vd->e(rhs);
              } else if (cannotUseRHSForOutput(env,vd->e())) {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                vd->e(NULL);
                assert(vdi->e()->flat());
                if (vdi->e()->type().dim() == 0) {
                  vdi->e()->flat()->addAnnotation(constants().ann.output_var);
                } else {
                  std::vector<Expression*> args(vdi->e()->type().dim());
                  for (int i=0; i<args.size(); i++) {
                    if (vdi->e()->ti()->ranges()[i]->domain() == NULL) {
                      args[i] = new SetLit(Location(), eval_intset(vdi->e()->flat()->ti()->ranges()[i]->domain()));
                    } else {
                      args[i] = new SetLit(Location(), eval_intset(vdi->e()->ti()->ranges()[i]->domain()));
                    }
                  }
                  ArrayLit* al = new ArrayLit(Location(), args);
                  args.resize(1);
                  args[0] = al;
                  vdi->e()->flat()->addAnnotation(new Call(Location(),constants().ann.output_array,args,NULL));
                }
              }
            }
            vo.add(vdi_copy, env.output->size());
            CollectOccurrencesE ce(vo,vdi_copy);
            topDown(ce, vdi_copy->e());
            env.output->addItem(vdi_copy);
          }
        }
      } _ov2(e,e.output_vo);
      iterItems(_ov2,e.orig);
      
      CollectOccurrencesE ce(e.output_vo,outputItem);
      topDown(ce, outputItem->e());

    }
    
    std::vector<VarDecl*> deletedVarDecls;
    for (unsigned int i=0; i<e.output->size(); i++) {
      if (VarDeclI* vdi = (*e.output)[i]->dyn_cast<VarDeclI>()) {
        if (e.output_vo.occurrences(vdi->e())==0) {
          CollectDecls cd(e.output_vo,deletedVarDecls,vdi);
          topDown(cd, vdi->e()->e());
          removeIsOutput(vdi->e()->flat());
          vdi->remove();
        }
      }
    }
    while (!deletedVarDecls.empty()) {
      VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
      if (e.output_vo.occurrences(cur) == 0) {
        ExpressionMap<int>::iterator cur_idx = e.output_vo.idx.find(cur);
        if (cur_idx != e.output_vo.idx.end()) {
          VarDeclI* vdi = (*e.output)[cur_idx->second]->cast<VarDeclI>();
          if (!vdi->removed()) {
            CollectDecls cd(e.output_vo,deletedVarDecls,vdi);
            topDown(cd,cur->e());
            removeIsOutput(vdi->e()->flat());
            vdi->remove();
          }
        }
      }
    }
    e.output->compact();
  }
  
  void flatten(Env& e, FlatteningOptions opt) {
    EnvI& env = e.envi();

    // Collect variable declarations to determine clean namespace for temporaries
    class DeclV : public ItemVisitor {
    public:
      std::string& prefix;
      DeclV(EnvI& envi) : prefix(envi.varPrefix) {}
      void vVarDeclI(VarDeclI* v) {
        while (v->e()->id()->v().beginsWith(prefix))
          prefix += "_";
      }
    } _declv(e.envi());
    iterItems(_declv, e.model());
    
    // Flatten main model
    class FV : public ItemVisitor {
    public:
      EnvI& env;
      FV(EnvI& env0) : env(env0) {}
      void vVarDeclI(VarDeclI* v) {
        if (v->e()->type().isvar() || v->e()->type().isann()) {
          (void) flat_exp(env,Ctx(),v->e()->id(),NULL,constants().var_true);
        } else {
          if (v->e()->e()==NULL) {
            if (!v->e()->type().isann())
              throw EvalError(v->e()->loc(), "Undefined parameter", v->e()->id()->v());
          } else {
            GCLock lock;
            v->e()->e(eval_par(v->e()->e()));
            if (v->e()->type().dim() > 0)
              checkIndexSets(v->e(), v->e()->e());
          }
        }
      }
      void vConstraintI(ConstraintI* ci) {
        (void) flat_exp(env,Ctx(),ci->e(),constants().var_true,constants().var_true);
      }
      void vSolveI(SolveI* si) {
        GCLock lock;
        SolveI* nsi = NULL;
        switch (si->st()) {
        case SolveI::ST_SAT:
          nsi = SolveI::sat(Location());
          break;
        case SolveI::ST_MIN:
          nsi = SolveI::min(Location(),flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r());
          break;
        case SolveI::ST_MAX:
          nsi = SolveI::max(Location(),flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r());
          break;
        }
        for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
          nsi->ann().add(flat_exp(env,Ctx(),*it,NULL,constants().var_true).r());
        }
        env.flat_addItem(nsi);
      }
    } _fv(env);
    iterItems<FV>(_fv,e.model());

    // Create output model
    createOutput(env);
    
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
      FunctionI* fi = env.orig->matchFn(constants().ids.int_lin_eq, int_lin_eq_t);
      int_lin_eq = (fi && fi->e()) ? fi : NULL;
    }
    FunctionI* array_bool_and;
    FunctionI* array_bool_or;
    FunctionI* array_bool_clause;
    FunctionI* array_bool_clause_reif;
    {
      std::vector<Type> array_bool_andor_t(2);
      array_bool_andor_t[0] = Type::varbool(1);
      array_bool_andor_t[1] = Type::varbool(0);
      GCLock lock;
      FunctionI* fi = env.orig->matchFn(ASTString("array_bool_and"), array_bool_andor_t);
      array_bool_and = (fi && fi->e()) ? fi : NULL;
      fi = env.orig->matchFn(ASTString("array_bool_or"), array_bool_andor_t);
      array_bool_or = (fi && fi->e()) ? fi : NULL;

      array_bool_andor_t[1] = Type::varbool(1);
      fi = env.orig->matchFn(ASTString("bool_clause"), array_bool_andor_t);
      array_bool_clause = (fi && fi->e()) ? fi : NULL;

      array_bool_andor_t.push_back(Type::varbool());
      fi = env.orig->matchFn(ASTString("bool_clause_reif"), array_bool_andor_t);
      array_bool_clause_reif = (fi && fi->e()) ? fi : NULL;
    }
    
    std::vector<VarDecl*> deletedVarDecls;
    while (startItem <= endItem) {
      for (unsigned int i=startItem; i<=endItem; i++) {
        VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>();
        bool keptVariable = true;
        if (vdi!=NULL && !isOutput(vdi->e()) && env.vo.occurrences(vdi->e())==0 ) {
          if (vdi->e()->e() && vdi->e()->ti()->domain()) {
            if (vdi->e()->type().isvar() && vdi->e()->type().isbool() &&
                Expression::equal(vdi->e()->ti()->domain(),constants().lit_true)) {
              GCLock lock;
              ConstraintI* ci = new ConstraintI(vdi->loc(),vdi->e()->e());
              if (vdi->e()->introduced()) {
                env.flat_replaceItem(i,ci);
                keptVariable = false;
              } else {
                vdi->e()->e(NULL);
                env.flat_addItem(ci);
              }
            } else if (vdi->e()->type().ispar() || vdi->e()->ti()->computedDomain()) {
              CollectDecls cd(env.vo,deletedVarDecls,vdi);
              topDown(cd,vdi->e()->e());
              vdi->remove();
              keptVariable = false;
            }
          } else {
            CollectDecls cd(env.vo,deletedVarDecls,vdi);
            topDown(cd,vdi->e()->e());
            vdi->remove();
            keptVariable = false;
          }
        }
        if (vdi && opt.onlyRangeDomains && keptVariable &&
            vdi->e()->type().isint() && vdi->e()->type().isvar() &&
            vdi->e()->ti()->domain() != NULL) {
          GCLock lock;
          IntSetVal* dom = eval_intset(vdi->e()->ti()->domain());
          if (dom->size() > 1) {
            SetLit* newDom = new SetLit(Location(),IntSetVal::a(dom->min(0),dom->max(dom->size()-1)));
            TypeInst* nti = copy(vdi->e()->ti())->cast<TypeInst>();
            nti->domain(newDom);
            vdi->e()->ti(nti);
            IntVal firstHole = dom->max(0)+1;
            IntSetRanges domr(dom);
            ++domr;
            for (; domr(); ++domr) {
              for (IntVal i=firstHole; i<domr.min(); i++) {
                std::vector<Expression*> args(2);
                args[0] = vdi->e()->id();
                args[1] = new IntLit(Location(),i);
                Call* call = new Call(Location(),constants().ids.int_ne,args);
                call->type(Type::varbool());
                call->decl(env.orig->matchFn(call));
                (void) flat_exp(env, Ctx(), call, constants().var_true, constants().var_true);
                firstHole = domr.max()+1;
              }
            }
          }
        }
      }
      while (!deletedVarDecls.empty()) {
        VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
        if (env.vo.occurrences(cur) == 0 && !isOutput(cur)) {
          ExpressionMap<int>::iterator cur_idx = env.vo.idx.find(cur);
          if (cur_idx != env.vo.idx.end() && !m[cur_idx->second]->removed()) {
            CollectDecls cd(env.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
            topDown(cd,cur->e());
            m[cur_idx->second]->remove();
          }
        }
      }
      for (unsigned int i=startItem; i<=endItem; i++) {
        if (VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>()) {
          VarDecl* vd = vdi->e();
          if (!vdi->removed() && vd->e()) {
            if (Call* c = vd->e()->dyn_cast<Call>()) {
              GCLock lock;
              Call* nc = NULL;
              if (c->id() == constants().ids.lin_exp) {
                if (int_lin_eq) {
                  std::vector<Expression*> args(c->args().size());
                  ArrayLit* le_c = follow_id(c->args()[0])->cast<ArrayLit>();
                  std::vector<Expression*> nc_c(le_c->v().size());
                  std::copy(le_c->v().begin(),le_c->v().end(),nc_c.begin());
                  nc_c.push_back(new IntLit(Location(),-1));
                  args[0] = new ArrayLit(Location(),nc_c);
                  args[0]->type(Type::parint(1));
                  ArrayLit* le_x = follow_id(c->args()[1])->cast<ArrayLit>();
                  std::vector<Expression*> nx(le_x->v().size());
                  std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location(),nx);
                  args[1]->type(Type::varint(1));
                  IntVal d = c->args()[2]->cast<IntLit>()->v();
                  args[2] = new IntLit(Location(),-d);
                  args[2]->type(Type::parint(0));
                  nc = new Call(c->loc(),ASTString("int_lin_eq"),args);
                  nc->type(Type::varbool());
                  nc->decl(int_lin_eq);
                }
              } else if (c->id() == constants().ids.exists) {
                if (array_bool_or) {
                  std::vector<Expression*> args(2);
                  args[0] = c->args()[0];
                  args[1] = vd->id();
                  nc = new Call(c->loc(),array_bool_or->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_or);
                }
              } else if (c->id() == constants().ids.forall) {
                if (array_bool_and) {
                  std::vector<Expression*> args(2);
                  args[0] = c->args()[0];
                  args[1] = vd->id();
                  nc = new Call(c->loc(),array_bool_and->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_and);
                }
              } else if (c->id() == constants().ids.clause && array_bool_clause_reif) {
                std::vector<Expression*> args(3);
                args[0] = c->args()[0];
                args[1] = c->args()[1];
                args[2] = vd->id();
                nc = new Call(c->loc(),array_bool_clause_reif->id(),args);
                nc->type(Type::varbool());
                nc->decl(array_bool_clause_reif);
              } else {
                if ( (!vd->type().isbool()) || (!Expression::equal(vd->ti()->domain(), constants().lit_true))) {
                  std::vector<Expression*> args(c->args().size());
                  std::copy(c->args().begin(),c->args().end(),args.begin());
                  args.push_back(vd->id());
                  ASTString cid = c->id();
                  if (cid == constants().ids.clause && array_bool_clause_reif) {
                    nc = new Call(c->loc(),array_bool_clause_reif->id(),args);
                    nc->type(Type::varbool());
                    nc->decl(array_bool_clause_reif);
                  } else {
                    if (c->type().isbool() && vd->type().isbool()) {
                      cid = ASTString(c->id().str()+"_reif");
                    }
                    FunctionI* decl = env.orig->matchFn(cid,args);
                    if (decl && decl->e()) {
                      nc = new Call(c->loc(),cid,args);
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
                (void) flat_exp(env, Ctx(), nc, constants().var_true, constants().var_true);
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
                args[0] = c->args()[0];
                args[1] = constants().lit_true;
                nc = new Call(c->loc(),array_bool_or->id(),args);
                nc->type(Type::varbool());
                nc->decl(array_bool_or);
              }
            } else if (c->id() == constants().ids.forall) {
              if (array_bool_and) {
                std::vector<Expression*> args(2);
                args[0] = c->args()[0];
                args[1] = constants().lit_true;
                nc = new Call(c->loc(),array_bool_and->id(),args);
                nc->type(Type::varbool());
                nc->decl(array_bool_and);
              }
            } else if (c->id() == constants().ids.clause) {
              if (array_bool_clause) {
                std::vector<Expression*> args(2);
                args[0] = c->args()[0];
                args[1] = c->args()[1];
                nc = new Call(c->loc(),array_bool_clause->id(),args);
                nc->type(Type::varbool());
                nc->decl(array_bool_clause);
              }
            } else {
              FunctionI* decl = env.orig->matchFn(c);
              if (decl && decl->e()) {
                nc = c;
                nc->decl(decl);
              }
            }
            if (nc != NULL) {
              CollectDecls cd(env.vo,deletedVarDecls,ci);
              topDown(cd,c);
              ci->e(constants().lit_true);
              ci->remove();
              (void) flat_exp(env, Ctx(), nc, constants().var_true, constants().var_true);
            }
          }
          
        }
      }

      while (!deletedVarDecls.empty()) {
        VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
        if (env.vo.occurrences(cur) == 0 && !isOutput(cur)) {
          ExpressionMap<int>::iterator cur_idx = env.vo.idx.find(cur);
          if (cur_idx != env.vo.idx.end() && !m[cur_idx->second]->removed()) {
            CollectDecls cd(env.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
            topDown(cd,cur->e());
            m[cur_idx->second]->remove();
          }
        }
      }

      startItem = endItem+1;
      endItem = m.size()-1;
    }

    // Add redefinitions for output variables that may have been redefined since createOutput
    for (unsigned int i=0; i<env.output->size(); i++) {
      if (VarDeclI* vdi = (*env.output)[i]->dyn_cast<VarDeclI>()) {
        ASTStringMap<KeepAlive>::t::iterator it;
        if (!vdi->e()->type().ispar() &&
            vdi->e()->e()==NULL &&
            (it = env.reverseMappers.find(vdi->e()->id()->v())) != env.reverseMappers.end()) {
          GCLock lock;
          Call* rhs = copy(env.cmap,it->second())->cast<Call>();
          std::vector<Type> tv(rhs->args().size());
          for (unsigned int i=rhs->args().size(); i--;) {
            tv[i] = rhs->args()[i]->type();
            tv[i]._ti = Type::TI_PAR;
          }
          FunctionI* decl = env.output->matchFn(rhs->id(), tv);
          Type t;
          if (decl==NULL) {
            FunctionI* origdecl = env.orig->matchFn(rhs->id(), tv);
            assert(origdecl != NULL);
            decl = copy(env.cmap,origdecl)->cast<FunctionI>();
            env.output->registerFn(decl);
            env.output->addItem(decl);
            rhs->decl(decl);
          }
          outputVarDecls(env,rhs);
          removeIsOutput(vdi->e()->flat());
          vdi->e()->e(rhs);
        }
      }
    }

    createOutput(env);
    
    for (unsigned int i=0; i<m.size(); i++) {
      if (ConstraintI* ci = m[i]->dyn_cast<ConstraintI>()) {
        if (Call* c = ci->e()->dyn_cast<Call>()) {
          if (c->decl()==constants().var_redef) {
            CollectDecls cd(env.vo,deletedVarDecls,ci);
            topDown(cd,c);
            ci->remove();
          }
        }
      }
    }
    while (!deletedVarDecls.empty()) {
      VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
      if (env.vo.occurrences(cur) == 0 && !isOutput(cur)) {
        ExpressionMap<int>::iterator cur_idx = env.vo.idx.find(cur);
        if (cur_idx != env.vo.idx.end() && !m[cur_idx->second]->removed()) {
          CollectDecls cd(env.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
          topDown(cd,cur->e());
          m[cur_idx->second]->remove();
        }
      }
    }
    
//    for (unsigned int i=0; i<m.size(); i++) {
//      GCLock lock;
//      if (VarDeclI* vdi = m[i]->dyn_cast<VarDeclI>()) {
//        vdi->e()->addAnnotation(new Annotation(Location(),new IntLit(Location(),env.vo.occurrences(vdi->e()))));
//      }
//    }
    
    m.compact();
    
  }
  
  void oldflatzinc(Env& e) {
    struct {
    public:
      bool operator() (const Item* i) {
        return i->isa<VarDeclI>() &&
          (i->cast<VarDeclI>()->e()->type()._ot == Type::OT_OPTIONAL ||
           i->cast<VarDeclI>()->e()->type()._bt == Type::BT_ANN);
      }
    } _isOptVar;
    Model* m = e.flat();
    m->_items.erase(remove_if(m->_items.begin(), m->_items.end(), _isOptVar),
      m->_items.end());
    
    int msize = m->size();
    std::unordered_set<Item*> globals;
    std::vector<int> declsWithIds;
    for (unsigned int i=0; i<msize; i++) {
      if (VarDeclI* vdi = (*m)[i]->dyn_cast<VarDeclI>()) {
        GCLock lock;
        VarDecl* vd = vdi->e();
        if (vd->type().ispar()) {
          vd->ann().clear();
          vd->introduced(false);
          vd->ti()->domain(NULL);
        }
        vd->ann().remove(constants().ctx.mix);
        vd->ann().remove(constants().ctx.pos);
        vd->ann().remove(constants().ctx.neg);
        vd->ann().remove(constants().ctx.root);
        
        if (vd->e() && vd->e()->isa<Id>()) {
          declsWithIds.push_back(i);
          vdi->e()->payload(-i-1);
        } else {
          vdi->e()->payload(i);
        }
        
        if (vd->type().isvar() && vd->type().isbool()) {
          if (Expression::equal(vd->ti()->domain(),constants().lit_true)) {
            Expression* ve = vd->e();
            vd->e(constants().lit_true);
            vd->ti()->domain(NULL);
            if (ve != NULL) {
              if (Call* vcc = ve->dyn_cast<Call>()) {
                std::string cid;
                std::vector<Expression*> args;
                if (vcc->id() == constants().ids.exists) {
                  cid = "array_bool_or";
                  args.push_back(vcc->args()[0]);
                  args.push_back(constants().lit_true);
                } else if (vcc->id() == constants().ids.forall) {
                  cid = "array_bool_and";
                  args.push_back(vcc->args()[0]);
                  args.push_back(constants().lit_true);
                } else if (vcc->id() == constants().ids.clause) {
                  cid = "bool_clause";
                  args.push_back(vcc->args()[0]);
                  args.push_back(vcc->args()[1]);
                }
                if (args.size()==0) {
                  ve = vcc;
                } else {
                  Call* nc = new Call(vcc->loc(),cid,args);
                  nc->type(vcc->type());
                  nc->ann().merge(vcc->ann());
                  ve = nc;
                }
              } else if (Id* id = ve->dyn_cast<Id>()) {
                std::vector<Expression*> args(2);
                args[0] = id;
                args[1] = constants().lit_true;
                GCLock lock;
                ve = new Call(Location(),constants().ids.bool_eq,args);
              }
              e.envi().flat_addItem(new ConstraintI(Location(),ve));
            }
          } else {
            if (vd->e() != NULL) {
              if (vd->e()->eid()==Expression::E_CALL) {
                const Call* c = vd->e()->cast<Call>();
                vd->e(NULL);
                vd->addAnnotation(constants().ann.is_defined_var);
                std::string cid;
                if (c->id() == constants().ids.exists) {
                  cid = "array_bool_or";
                } else if (c->id() == constants().ids.forall) {
                  cid = "array_bool_and";
                } else if (c->id() == constants().ids.clause) {
                  cid = "bool_clause_reif";
                } else {
                  cid = c->id().str()+"_reif";
                }
                std::vector<Expression*> args(c->args().size());
                std::copy(c->args().begin(),c->args().end(),args.begin());
                args.push_back(vd->id());
                Call * nc = new Call(c->loc(),cid,args);
                nc->type(c->type());
                nc->addAnnotation(definesVarAnn(vd->id()));
                nc->ann().merge(c->ann());
                e.envi().flat_addItem(new ConstraintI(Location(),nc));
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
        } else if (vd->type().isvar() && vd->type()._dim==0) {
          if (vd->e() != NULL) {
            if (const Call* cc = vd->e()->dyn_cast<Call>()) {
              vd->e(NULL);
              vd->addAnnotation(constants().ann.is_defined_var);
              std::vector<Expression*> args(cc->args().size());
              std::string cid;
              if (cc->id() == constants().ids.lin_exp) {
                ArrayLit* le_c = follow_id(cc->args()[0])->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),nc.begin());
                if (le_c->type()._bt==Type::BT_INT) {
                  cid = "int_lin_eq";
                  nc.push_back(new IntLit(Location(),-1));
                  args[0] = new ArrayLit(Location(),nc);
                  ArrayLit* le_x = follow_id(cc->args()[1])->cast<ArrayLit>();
                  std::vector<Expression*> nx(le_x->v().size());
                  std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location(),nx);
                  IntVal d = cc->args()[2]->cast<IntLit>()->v();
                  args[2] = new IntLit(Location(),-d);
                } else {
                  // float
                  cid = "float_lin_eq";
                  nc.push_back(new FloatLit(Location(),-1.0));
                  args[0] = new ArrayLit(Location(),nc);
                  ArrayLit* le_x = follow_id(cc->args()[1])->cast<ArrayLit>();
                  std::vector<Expression*> nx(le_x->v().size());
                  std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location(),nx);
                  FloatVal d = cc->args()[2]->cast<FloatLit>()->v();
                  args[2] = new FloatLit(Location(),-d);
                }
              } else {
                if (cc->id() == "card") {
                  // card is 'set_card' in old FlatZinc
                  cid = "set_card";
                } else {
                  cid = cc->id().str();
                }
                std::copy(cc->args().begin(),cc->args().end(),args.begin());
                args.push_back(vd->id());
              }
              Call* nc = new Call(cc->loc(),cid,args);
              nc->type(cc->type());
              nc->addAnnotation(definesVarAnn(vd->id()));
              nc->ann().merge(cc->ann());
              e.envi().flat_addItem(new ConstraintI(Location(),nc));
            } else {
              assert(vd->e()->eid() == Expression::E_ID ||
                     vd->e()->eid() == Expression::E_INTLIT ||
                     vd->e()->eid() == Expression::E_BOOLLIT ||
                     vd->e()->eid() == Expression::E_SETLIT);
              
            }
          }
        } else if (vd->type()._dim > 0) {
          if (!vd->e()->isa<ArrayLit>()) {
            vd->e(follow_id(vd->e()));
          }
          if (vd->ti()->ranges().size() == 1 &&
              vd->ti()->ranges()[0]->domain() != NULL &&
              vd->ti()->ranges()[0]->domain()->isa<SetLit>()) {
            IntSetVal* isv = vd->ti()->ranges()[0]->domain()->cast<SetLit>()->isv();
            if (isv && (isv->size()==0 || isv->min(0)==1))
              continue;
          }
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
          std::vector<int> dims(2);
          dims[0] = 1;
          dims[1] = al->length();
          al->setDims(ASTIntVec(dims));
          IntSetVal* isv = IntSetVal::a(1,al->length());
          if (vd->ti()->ranges().size() == 1) {
            vd->ti()->ranges()[0]->domain(new SetLit(Location(),isv));
          } else {
            std::vector<TypeInst*> r(1);
            r[0] = new TypeInst(vd->ti()->ranges()[0]->loc(),
                                vd->ti()->ranges()[0]->type(),
                                new SetLit(Location(),isv));
            ASTExprVec<TypeInst> ranges(r);
            TypeInst* ti = new TypeInst(vd->ti()->loc(),vd->ti()->type(),ranges,vd->ti()->domain());
            vd->ti(ti);
          }
        }
      } else if (ConstraintI* ci = (*m)[i]->dyn_cast<ConstraintI>()) {
        if (Call* vc = ci->e()->dyn_cast<Call>()) {
          if (vc->id() == constants().ids.exists) {
            GCLock lock;
            vc->id(ASTString("array_bool_or"));
            std::vector<Expression*> args(2);
            args[0] = vc->args()[0];
            args[1] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
            vc->decl(e.envi().orig->matchFn(vc));
          } else if (vc->id() == constants().ids.forall) {
            GCLock lock;
            vc->id(ASTString("array_bool_and"));
            std::vector<Expression*> args(2);
            args[0] = vc->args()[0];
            args[1] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
            vc->decl(e.envi().orig->matchFn(vc));
          } else if (vc->id() == constants().ids.clause) {
            GCLock lock;
            vc->id(ASTString("bool_clause"));
            vc->decl(e.envi().orig->matchFn(vc));
          }
          if (vc->decl() && vc->decl() != constants().var_redef &&
              !vc->decl()->loc().filename.endsWith("/builtins.mzn") &&
              globals.find(vc->decl())==globals.end()) {
            e.envi().flat_addItem(vc->decl());
            globals.insert(vc->decl());
          }
        } else if (Id* id = ci->e()->dyn_cast<Id>()) {
          std::vector<Expression*> args(2);
          args[0] = id;
          args[1] = constants().lit_true;
          GCLock lock;
          ci->e(new Call(Location(),constants().ids.bool_eq,args));
        } else if (BoolLit* bl = ci->e()->dyn_cast<BoolLit>()) {
          if (!bl->v()) {
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = constants().lit_false;
            args[1] = constants().lit_true;
            Call* neq = new Call(Location(),constants().ids.bool_eq,args);
            ci->e(neq);
          }
        }
      } else if (SolveI* si = (*m)[i]->dyn_cast<SolveI>()) {
        if (si->e() && si->e()->type().ispar()) {
          GCLock lock;
          TypeInst* ti = new TypeInst(Location(),si->e()->type(),NULL);
          VarDecl* constantobj = new VarDecl(Location(),ti,e.envi().genId("obj"),si->e());
          si->e(constantobj->id());
          e.envi().flat_addItem(new VarDeclI(Location(),constantobj));
        }
      }
    }
    
    std::vector<VarDeclI*> sortedVarDecls(declsWithIds.size());
    int vdCount = 0;
    for (unsigned int i=0; i<declsWithIds.size(); i++) {
      VarDecl* cur = m->_items[declsWithIds[i]]->cast<VarDeclI>()->e();
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
        VarDeclI* vdi = m->_items[-stack[i]-1]->cast<VarDeclI>();
        vdi->e()->payload(-vdi->e()->payload()-1);
        sortedVarDecls[vdCount++] = vdi;
      }
    }
    for (unsigned int i=0; i<declsWithIds.size(); i++) {
      m->_items[declsWithIds[i]] = sortedVarDecls[i];
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
          if (i->cast<VarDeclI>()->e()->type()._dim == 0 &&
              j->cast<VarDeclI>()->e()->type()._dim != 0)
            return true;
          if (i->cast<VarDeclI>()->e()->type()._dim != 0 &&
              j->cast<VarDeclI>()->e()->type()._dim == 0)
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
    std::stable_sort(m->_items.begin(),m->_items.end(),_cmp);

  }

}
