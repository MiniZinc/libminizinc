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

  Constants::Constants(void) {
    GC::init();
    GCLock lock;
    TypeInst* ti = new TypeInst(Location(), Type::parbool());
    lit_true = new BoolLit(Location(), true);
    var_true = new VarDecl(Location(), ti, "_bool_true", lit_true);
    lit_false = new BoolLit(Location(), false);
    var_false = new VarDecl(Location(), ti, "_bool_false", lit_false);
    
    ids.forall = ASTString("forall");
    ids.exists = ASTString("exists");
    ids.bool2int = ASTString("bool2int");
    ids.sum = ASTString("sum");
    ids.lin_exp = ASTString("lin_exp");
    ids.bool_eq = ASTString("bool_eq");
    ids.bool_clause = ASTString("bool_clause");
    
    ctx.root = new Id(Location(),ASTString("ctx_root"),NULL);
    ctx.root->type(Type::ann());
    ctx.pos = new Id(Location(),ASTString("ctx_pos"),NULL);
    ctx.pos->type(Type::ann());
    ctx.neg = new Id(Location(),ASTString("ctx_neg"),NULL);
    ctx.neg->type(Type::ann());
    ctx.mix = new Id(Location(),ASTString("ctx_mix"),NULL);
    ctx.mix->type(Type::ann());

    ann.output_var = new Id(Location(), ASTString("output_var"), NULL);
    ann.output_var->type(Type::ann());
    ann.output_array = ASTString("output_array");
    
    m = new Model;
    std::vector<Expression*> v;
    v.push_back(ti);
    v.push_back(lit_true);
    v.push_back(var_true);
    v.push_back(lit_false);
    v.push_back(var_false);
    v.push_back(new StringLit(Location(),ids.forall));
    v.push_back(new StringLit(Location(),ids.exists));
    v.push_back(new StringLit(Location(),ids.bool2int));
    v.push_back(new StringLit(Location(),ids.sum));
    v.push_back(new StringLit(Location(),ids.lin_exp));
    v.push_back(new StringLit(Location(),ids.bool_eq));
    v.push_back(new StringLit(Location(),ids.bool_clause));
    v.push_back(ctx.root);
    v.push_back(ctx.pos);
    v.push_back(ctx.neg);
    v.push_back(ctx.mix);
    v.push_back(ann.output_var);
    v.push_back(new StringLit(Location(),ann.output_array));
    m->_items.push_back(
      new ConstraintI(Location(),new ArrayLit(Location(),v)));
  }
  
  Constants& constants(void) {
    static Constants _c;
    return _c;
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
      Annotation* vdann = vd->ann();
      while (vdann) {
        if (Id* id = vdann->e()->dyn_cast<Id>()) {
          if (id->v()==ctx_id->v())
            return;
        }
        vdann = vdann->next();
      }
      vd->annotate(new Annotation(Location(),ctx_id));
    }
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

  class EnvI {
  public:
    Model* orig;
    Model* flat;
    Model* output;
    struct WW {
      WeakRef r;
      WeakRef b;
      WW(WeakRef r0, WeakRef b0) : r(r0), b(b0) {}
    };
    typedef KeepAliveMap<WW> Map;
  protected:
    Map map;
    unsigned int ids;
  public:
    EnvI(Model* orig0) : orig(orig0), flat(new Model), output(new Model), ids(0) {
    }
    ~EnvI(void) {
      delete flat;
      delete output;
    }
    ASTString genId(const std::string& s) {
      std::ostringstream oss; oss << "_" << s << "_" << ids++;
      return ASTString(oss.str());
    }
    void map_insert(Expression* e, const EE& ee) {
      KeepAlive ka(e);
      map.insert(ka,WW(ee.r(),ee.b()));
    }
    Map::iterator map_find(Expression* e) {
      KeepAlive ka(e);
      return map.find(ka);
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
  };

  Env::Env(Model* m) : e(new EnvI(m)) {}
  Env::~Env(void) {
    delete e;
  }
  
  Model*
  Env::model(void) { return e->orig; }
  Model*
  Env::flat(void) { return e->flat; }
  Model*
  Env::output(void) { return e->output; }
  EnvI&
  Env::envi(void) { return *e; }

  bool isTotal(FunctionI* fi) {
    Annotation* a = fi->ann();
    for (; a!=NULL; a=a->next()) {
      VarDecl* vd = NULL;
      Expression * ae = a->e();
      while (ae && ae->eid()==Expression::E_ID &&
             ae->cast<Id>()->decl()!=NULL) {
        vd = ae->cast<Id>()->decl();
        ae = vd->e();
      }
      
      if (vd && vd->type().isann() && vd->id()->v() == "total") {
        return true;
      }
    }
    return false;
  }

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);

  KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e) {
    if (ctx.neg) {
      assert(e->type()._bt == Type::BT_BOOL);
      if (vd==constants().var_true) {
        if (!isfalse(e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            assert(id->decl() != NULL);
            if (id->decl()->ti()->domain() && istrue(id->decl()->ti()->domain())) {
              GCLock lock;
              env.flat->addItem(new ConstraintI(Location(),constants().lit_false));
            } else {
              id->decl()->ti()->domain(constants().lit_false);
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
            if (id->decl()->ti()->domain() && isfalse(id->decl()->ti()->domain())) {
              GCLock lock;
              env.flat->addItem(new ConstraintI(Location(),constants().lit_false));
            } else {
              id->decl()->ti()->domain(constants().lit_true);
            }
          } else {
            GCLock lock;
            env.flat->addItem(new ConstraintI(Location(),e));
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
        case Expression::E_ANN:
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
            VarDecl* vd = new VarDecl(Location(),ti,env.genId("X"),e);
            vd->introduced(true);

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
            } else if (vd->e()->type().isbool()) {
              addCtxAnn(vd, ctx.b);
            }

            VarDeclI* nv = new VarDeclI(Location(),vd);
            env.flat->addItem(nv);

            EE ee(vd,NULL);
            env.map_insert(vd->id(),ee);

            return vd->id();
          }
        default:
          assert(false); return NULL;
        }
      } else {
        if (vd->e()==NULL) {
          if (e==NULL) {
            vd->e(constants().lit_true);
          } else if (e->type().ispar() && e->type().isbool()) {
            if (eval_bool(e)) {
              vd->e(constants().lit_true);
            } else {
              vd->e(constants().lit_false);
            }
          } else {
            vd->e(e);
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
          return e;
        } else if (vd == e) {
          return vd;
        } else if (vd->e() != e) {
          switch (e->eid()) {
          case Expression::E_VARDECL:
            {
              VarDecl* e_vd = e->cast<VarDecl>();
              if (e->type()._dim != 0)
                throw InternalError("not supported yet");
              std::string cid;
              if (e->type().isint()) {
                cid = "int_eq";
              } else if (e->type().isbool()) {
                cid = "bool_eq";
              } else if (e->type().isset()) {
                cid = "set_eq";
              } else {
                throw InternalError("not yet implemented");
              }
              GCLock lock;
              std::vector<Expression*> args(2);
              args[0] = vd->id();
              args[1] = e_vd->id();
              Call* c = new Call(Location(),cid,args);
              env.flat->addItem(new ConstraintI(Location(),c));
              return vd;
            }
          case Expression::E_CALL:
            {
              Call* c = e->cast<Call>();
              std::vector<Expression*> args(c->args().size());
              GCLock lock;
              if (c->id() == "lin_exp") {
                c->id(ASTString("int_lin_eq"));
                ArrayLit* le_c = c->args()[0]->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),nc.begin());
                nc.push_back(new IntLit(Location(),-1));
                c->args()[0] = new ArrayLit(Location(),nc);
                ArrayLit* le_x = c->args()[1]->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->v().size());
                std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                nx.push_back(vd->id());
                c->args()[1] = new ArrayLit(Location(),nx);
                IntVal d = c->args()[2]->cast<IntLit>()->v();
                c->args()[2] = new IntLit(Location(),-d);
              } else {
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
              ConstraintI* ci = new ConstraintI(Location(),c);
              env.flat->addItem(ci);
              return vd;
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
          ret->type(Type::varbool());
          ret->decl(env.orig->matchFn(ret));
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
          ret->type(Type::varbool());
          ret->decl(env.orig->matchFn(ret));
          assert(ret->decl());
          KeepAlive ka(ret);
          GC::unlock();
          return flat_exp(env,nctx,ret,b,constants().var_true).r;
        }
      }
      
    }
  }

  TypeInst* eval_typeinst(EnvI& env, TypeInst* ti) {
    /// TODO: evaluate all par components in the domain. This probably
    ///       needs the VarDecl to compute the actual dimensions of
    ///       array[int] expressions
    return eval_par(ti)->cast<TypeInst>();
  }

  std::string opToBuiltin(BinOp* op, BinOpType bot) {
    std::string builtin;
    if (op->rhs()->type().isint()) {
      builtin = "int_";
    } else if (op->rhs()->type().isbool()) {
      builtin = "bool_";
    } else if (op->rhs()->type().isset()) {
      builtin = "set_";
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

  KeepAlive mklinexp(EnvI& env, IntVal c0, IntVal c1,
                     Expression* e0, Expression* e1) {
    GCLock lock;
    IntVal d = 0;
    if (e0->type().ispar() && e0->type().isint()) {
      d += c0*eval_int(e0);
      e0 = NULL;
    }
    if (e1 && e1->type().ispar() && e1->type().isint()) {
      d += c1*eval_int(e1);
      e1 = NULL;
    }
    if (e0==NULL && e1==NULL)
      return new IntLit(e0->loc(),d);
    if (e0==NULL) {
      std::swap(e0,e1);
      std::swap(c0,c1);
    }
    std::vector<Expression*> bo_args(e1 ? 2 : 1);
    bo_args[0] = e0;
    if (e1)
      bo_args[1] = e1;
    std::vector<Expression*> coeffs(e1 ? 2 : 1);
    coeffs[0] = new IntLit(e0->loc(),c0);
    if (e1) {
      if (c0==c1)
        coeffs[1] = coeffs[0];
      else
        coeffs[1] = new IntLit(e0->loc(),c1);
    }
    std::vector<Expression*> args(3);
    args[0]=new ArrayLit(e0->loc(),coeffs);
    args[0]->type(Type::parint(1));
    args[1]=new ArrayLit(e0->loc(),bo_args);
    Type tt = e0->type();
    tt._dim = 1;
    if (e0->type()._ti==Type::TI_PAR && e1)
      tt._ti = e1->type()._ti;
    args[1]->type(tt);
    args[2] = new IntLit(e0->loc(),d);
    args[2]->type(Type::parint());
    Call* c = new Call(e0->loc(),"lin_exp",args);
    tt = args[1]->type();
    tt._dim = 0;
    c->type(tt);
    c->decl(env.orig->matchFn(c));
    KeepAlive ka = c;
    return ka;
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

  void simplify_lin(std::vector<IntVal>& c,
                    std::vector<KeepAlive>& x,
                    IntVal& d) {
    std::vector<int> idx(c.size());
    for (unsigned int i=idx.size(); i--;) {
      idx[i]=i;
    }
    std::sort(idx.begin(),idx.end(),CmpExpIdx(x));
    int ci = 0;
    for (; ci<x.size(); ci++) {
      if (IntLit* il = x[idx[ci]]()->dyn_cast<IntLit>()) {
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
      } else if (IntLit* il = x[idx[i]]()->dyn_cast<IntLit>()) {
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
    bool operator ()(const Expression* i, const Expression* j) const {
      if (Expression::equal(i,j))
        return false;
      return i<j;
    }
  };

  void remove_dups(std::vector<Expression*>& x, bool identity) {
    std::sort(x.begin(),x.end(),CmpExp());
    int ci = 0;
    Expression* prev = NULL;
    for (unsigned int i=0; i<x.size(); i++) {
      if (!Expression::equal(x[i],prev)) {
        prev = x[i];
        if (x[i]->isa<BoolLit>() && x[i]->cast<BoolLit>()->v()==identity) {
          // skip
        } else {
          x[ci++] = x[i];
        }
      }
    }
    x.resize(ci);
  }

  /// Return a lin_exp or id if \a e is a lin_exp or id
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
    if (e && (e->isa<Id>() || e->isa<IntLit>() ||
              (e->isa<Call>() && e->cast<Call>()->id() == constants().ids.lin_exp)))
      return e;
    return NULL;
  }

  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
    EE ret;
    assert(!e->type().isunknown());
    if (e->type().ispar() && !e->isa<Let>() && e->type()._bt!=Type::BT_ANN) {
      GCLock lock;
      ret.b = bind(env,Ctx(),b,constants().lit_true);
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
            idxset->type(Type::parsetint());
            idxset->decl(env.orig->matchFn(idxset));
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
                 ->cast<VarDecl>();
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
            unsigned int asize = 1;
            for (unsigned int i=0; i<vd->ti()->ranges().size(); i++) {
              TypeInst* ti = vd->ti()->ranges()[i];
              if (ti->domain()==NULL)
                throw FlatteningError(ti->loc(),"array dimensions unknown");
              IntSetVal* isv = eval_intset(ti->domain());
              if (isv->size() != 1)
                throw FlatteningError(ti->loc(),"invalid array index set");
              asize *= (isv->max(0)-isv->min(0)+1);
              dims.push_back(std::pair<int,int>(isv->min(0),isv->max(0)));
            }
            Type tt = vd->ti()->type();
            tt._dim = 0;
            TypeInst* vti = new TypeInst(Location(),tt,vd->ti()->domain());
            
            std::vector<Expression*> elems(asize);
            for (int i=0; i<asize; i++) {
              ASTString nid = env.genId("fresh_"+vd->id()->v().str());
              VarDecl* nvd = new VarDecl(Location(),vti,nid);
              nvd->introduced(true);
              EE root_vd = flat_exp(env,Ctx(),nvd,NULL,constants().var_true);
              Id* id = root_vd.r()->cast<VarDecl>()->id();
              elems[i] = id;
            }

            ArrayLit* al = new ArrayLit(Location(),elems,dims);
            al->type(vd->type());
            vd->e(al);
          }
          if (rete==NULL) {
            if (!vd->toplevel()) {
              // create new VarDecl in toplevel, if decl doesnt exist yet
              EnvI::Map::iterator it = env.map_find(vd->e());
              if (it==env.map_end()) {
                VarDecl* nvd = 
                  new VarDecl(Location(),eval_typeinst(env,vd->ti()),
                             env.genId("tl_"+vd->id()->v().str()),vd->e());
                nvd->introduced(true);
                VarDeclI* ni = new VarDeclI(Location(),nvd);
                env.flat->addItem(ni);
                vd = nvd;
                EE ee(vd,NULL);
                if (vd->e())
                  env.map_insert(vd->e(),ee);
                env.map_insert(nvd->id(),ee);
              } else {
                vd = it->second.r()->cast<VarDecl>();
              }
            }
            if (id->type()._bt == Type::BT_ANN && vd->e()) {
              rete = vd->e();
            } else {
              rete = vd->id();
              rete->type(id->type());
            }
          }
          ret.r = bind(env,ctx,r,rete);
        }
      }
      break;
    case Expression::E_ANON:
      throw InternalError("anonymous variables not supported yet");
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
        bool parAccess=true;
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          if (!aa->idx()[i]->type().ispar()) {
            parAccess = false;
            break;
          }
        }
        if (parAccess) {
          Ctx nctx = ctx;
          nctx.b = +nctx.b;
          nctx.neg = false;
          EE eev = flat_exp(env,nctx,aa->v(),NULL,NULL);
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
            al = id->decl()->e()->cast<ArrayLit>();
          }
          KeepAlive ka;
          {
            GCLock lock;
            std::vector<IntVal> dims(aa->idx().size());
            for (unsigned int i=aa->idx().size(); i--;)
              dims[i] = eval_int(aa->idx()[i]);
            ka = eval_arrayaccess(al,dims);
          }
          ret.b = bind(env,Ctx(),b,constants().lit_true);
          ret.r = bind(env,ctx,r,ka());
        } else {
          std::vector<Expression*> args(aa->idx().size()+1);
          for (unsigned int i=aa->idx().size(); i--;)
            args[i] = aa->idx()[i];
          args[aa->idx().size()] = aa->v();
          KeepAlive ka;
          {
            GCLock lock;
            Call* cc = new Call(Location(),"element",args);
            cc->type(aa->type());
            FunctionI* fi = env.orig->matchFn(cc->id(),args);
            assert(cc->type() == fi->rtype(args));
            cc->decl(fi);
            ka = cc;
          }
          ret = flat_exp(env,ctx,ka(),r,b);
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
        bool done = false;
        for (int i=0; i<ite->size(); i++) {
          if (eval_bool(ite->e_if(i))) {
            ret = flat_exp(env,ctx,ite->e_then(i),r,b);
            done = true;
            break;
          }
        }
        if (!done) {
          ret = flat_exp(env,ctx,ite->e_else(),r,b);
        }
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->decl()) {
          std::vector<Expression*> args(2);
          args[0] = bo->lhs();
          args[1] = bo->rhs();
          KeepAlive ka;
          {
            GCLock lock;
            Call* cr = new Call(Location(),bo->opToString().str(),args);
            cr->type(bo->type());
            cr->decl(env.orig->matchFn(cr));
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
              KeepAlive ka = mklinexp(env,1,1,boe0,boe1);
              ret = flat_exp(env,ctx,ka(),r,b);
            }
            break;
          case BOT_MINUS:
            {
              KeepAlive ka = mklinexp(env,1,-1,boe0,boe1);
              ret = flat_exp(env,ctx,ka(),r,b);
            }
            break;
          case BOT_MULT:
            {
              if (boe0->type().ispar())
                std::swap(boe0,boe1);
              if (boe1->type().ispar() && boe1->type().isint()) {
                IntVal coeff = eval_int(boe1);
                KeepAlive ka = mklinexp(env,coeff,0,boe0,NULL);
                ret = flat_exp(env,ctx,ka(),r,b);
                break;
              }
              // else fall through
            }
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

              GC::lock();
              std::vector<Expression*> args(2);
              args[0] = e0.r(); args[1] = e1.r();
              Call* cc = new Call(Location(),opToBuiltin(bo,bot),args);
              cc->type(bo->type());

              if (FunctionI* fi = env.orig->matchFn(cc->id(),args)) {
                assert(cc->type() == fi->rtype(args));
                cc->decl(fi);
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
                c->type(bo->type());
                c->decl(env.orig->matchFn(c));
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
              c->type(bo->type());
              c->decl(env.orig->matchFn(c));
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
              c->type(bo->type());
              c->decl(env.orig->matchFn(c));
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
              ctx0.b = ctx1.b = C_MIX;
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
                  EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
                  Id* id = e0.r()->cast<Id>();
                  (void) flat_exp(env,ctx1,boe1,id->decl(),NULL);
                  ret.b = bind(env,Ctx(),b,constants().lit_true);
                  ret.r = bind(env,Ctx(),r,constants().lit_true);
                }
                break;
              } else {
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
              ctx0.b = +ctx0.b;
              ctx1.b = -ctx1.b;
            } else {
              ctx0.b = -ctx0.b;
              ctx1.b = +ctx1.b;
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

              std::vector<KeepAlive> args;
              std::string callid;

              std::vector<EE> ees(3);
              ees[0].b = e0.b; ees[1].b = e1.b;

              Expression* le0 = 
                (boe0->type()._bt == Type::BT_INT && bot != BOT_IN) ?
                  get_linexp(e0.r()) : NULL;
              Expression* le1 = le0 ? get_linexp(e1.r()) : NULL;

              if (le1) {
                std::vector<IntVal> coeffv;
                std::vector<KeepAlive> alv;
                IntVal d = 0;
                Expression* le[2] = {le0,le1};
                for (unsigned int i=0; i<2; i++) {
                  IntVal sign = (i==0 ? 1 : -1);
                  switch (le[i]->eid()) {
                  case Expression::E_INTLIT:
                    d += sign*(le[i]->cast<IntLit>()->v());
                    break;
                  case Expression::E_ID:
                    coeffv.push_back(sign);
                    alv.push_back(le[i]);
                    break;
                  case Expression::E_CALL:
                    {
                      Call* sc = le[i]->cast<Call>();
                      GCLock lock;
                      ArrayLit* sc_coeff = eval_array_lit(sc->args()[0]);
                      ArrayLit* sc_al = eval_array_lit(sc->args()[1]);
                      d += sign*eval_int(sc->args()[2]);
                      for (unsigned int j=0; j<sc_coeff->v().size(); j++) {
                        coeffv.push_back(sign*eval_int(sc_coeff->v()[j]));
                        alv.push_back(sc_al->v()[j]);
                      }
                    }
                    break;
                  default: assert(false); break;
                  }
                }
                simplify_lin(coeffv,alv,d);
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
                  if (result || doubleNeg) {
                    ret.r = conj(env,r,ctx,ees);
                  } else {
                    ret.r = bind(env,ctx,r,constants().lit_false);
                  }
                  break;
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
                  if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && bot==BOT_EQ) {
                    GCLock lock;
                    VarDecl* vd = alv[0]()->cast<Id>()->decl();
                    if (vd->ti()->domain()) {
                      IntSetVal* domain = eval_intset(vd->ti()->domain());
                      if (domain->contains(d)) {
                        if (domain->size()!=1 || domain->min(0)!=d || domain->max(0)!=d) {
                          vd->ti()->setComputedDomain(false);
                          vd->ti()->domain(new SetLit(Location(),IntSetVal::a(d,d)));
                        }
                        ret.r = bind(env,ctx,r,constants().lit_true);
                      } else {
                        ret.r = bind(env,ctx,r,constants().lit_false);
                      }
                    } else {
                      vd->ti()->setComputedDomain(false);
                      vd->ti()->domain(new SetLit(Location(),IntSetVal::a(d,d)));
                      ret.r = bind(env,ctx,r,constants().lit_true);
                    }
                  } else if (ctx.b == C_ROOT && alv[0]()->isa<Id>() && alv[0]()->cast<Id>()->decl()->ti()->domain()) {
                    GCLock lock;
                    VarDecl* vd = alv[0]()->cast<Id>()->decl();
                    IntSetVal* domain = eval_intset(vd->ti()->domain());
                    IntSetRanges dr(domain);
                    IntSetVal* ndomain;
                    switch (bot) {
                    case BOT_LE:
                      d -= 1;
                      // fall through
                    case BOT_LQ:
                      {
                        Ranges::Bounded<IntSetRanges> b = Ranges::Bounded<IntSetRanges>::maxiter(dr,d);
                        ndomain = IntSetVal::ai(b);
                      }
                      break;
                    case BOT_GR:
                      d += 1;
                      // fall through
                    case BOT_GQ:
                      {
                        Ranges::Bounded<IntSetRanges> b = Ranges::Bounded<IntSetRanges>::miniter(dr,d);
                        ndomain = IntSetVal::ai(b);
                      }
                      break;
                    case BOT_NQ:
                      {
                        Ranges::Const c(d,d);
                        Ranges::Diff<IntSetRanges,Ranges::Const> d(dr,c);
                        ndomain = IntSetVal::ai(d);
                      }
                      break;
                    default: assert(false);
                    }
                    IntSetRanges dr2(domain);
                    IntSetRanges ndr(ndomain);
                    Ranges::Inter<IntSetRanges,IntSetRanges> i(dr2,ndr);
                    IntSetVal* newibv = IntSetVal::ai(i);
                    if (domain->card() != newibv->card()) {
                      if (newibv->card() == 0) {
                        ret.r = bind(env,ctx,r,constants().lit_false);
                      } else {
                        ret.r = bind(env,ctx,r,constants().lit_true);
                        vd->ti()->setComputedDomain(false);
                        vd->ti()->domain(new SetLit(Location(),newibv));
                      }
                    }
                  } else {
                    GCLock lock;
                    Expression* e0;
                    Expression* e1;
                    switch (bot) {
                    case BOT_LE:
                      e0 = alv[0]();
                      e1 = new IntLit(Location(),d-1);
                      bot = BOT_LQ;
                      break;
                    case BOT_GR:
                      e0 = new IntLit(Location(),d+1);
                      e1 = alv[0]();
                      bot = BOT_LQ;
                      break;
                    case BOT_GQ:
                      e0 = new IntLit(Location(),d);
                      e1 = alv[0]();
                      bot = BOT_LQ;
                      break;
                    default:
                      e0 = alv[0]();
                      e1 = new IntLit(Location(),d);
                    }
                    args.push_back(e0);
                    args.push_back(e1);
                    callid = opToBuiltin(bo,bot);
                  }
                } else {
                  int coeff_sign;
                  switch (bot) {
                  case BOT_LE:
                    callid = "int_lin_le";
                    coeff_sign = 1;
                    d += 1;
                    break;
                  case BOT_LQ:
                    callid = "int_lin_le";
                    coeff_sign = 1;
                    break;
                  case BOT_GR:
                    callid = "int_lin_le";
                    coeff_sign = -1;
                    d = -d+1;
                    break;
                  case BOT_GQ:
                    callid = "int_lin_le";
                    coeff_sign = -1;
                    d = -d;
                    break;
                  case BOT_EQ:
                    callid = "int_lin_eq";
                    coeff_sign = 1;
                    break;
                  case BOT_NQ:
                    callid = "int_lin_ne";
                    coeff_sign = 1;
                    break;
                  default: assert(false); break;
                  }

                  {
                    GCLock lock;
                    std::vector<Expression*> coeff_ev(coeffv.size());
                    for (unsigned int i=coeff_ev.size(); i--;)
                      coeff_ev[i] = new IntLit(Location(),coeff_sign*coeffv[i]);
                    ArrayLit* ncoeff = new ArrayLit(Location(),coeff_ev);
                    ncoeff->type(Type::parint(1));
                    args.push_back(ncoeff);
                    std::vector<Expression*> alv_e(alv.size());
                    for (unsigned int i=alv.size(); i--;)
                      alv_e[i] = alv[i]();
                    ArrayLit* nal = new ArrayLit(Location(),alv_e);
                    Type tt = alv[0]()->type();
                    tt._dim = 1;
                    nal->type(tt);
                    args.push_back(nal);
                    IntLit* il = new IntLit(Location(),-d);
                    il->type(Type::parint());
                    args.push_back(il);
                  }
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
                callid = opToBuiltin(bo,bot);
              }

              if (args.size() > 0) {
                GC::lock();
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
                al = id->decl()->e()->cast<ArrayLit>();
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
                al = id->decl()->e()->cast<ArrayLit>();
              }
              ArrayLit* al1 = al;
              std::vector<Expression*> v(al0->v().size()+al1->v().size());
              for (unsigned int i=al0->v().size(); i--;)
                v[i] = al0->v()[i];
              for (unsigned int i=al1->v().size(); i--;)
                v[al0->v().size()+i] = al1->v()[i];
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
        if (c->decl() == NULL) {
          throw InternalError("undeclared function or predicate "
                              +c->id().str());
        }
        FunctionI* decl = env.orig->matchFn(c);

        Ctx nctx = ctx;
        nctx.neg = false;
        ASTString cid = c->id();
        if (decl->e()==NULL && cid == constants().ids.forall) {
          nctx.b = +nctx.b;
          if (ctx.neg) {
            ctx.neg = false;
            nctx.neg = true;
            cid = constants().ids.exists;
          }
        } else if (decl->e()==NULL && cid == constants().ids.exists) {
          nctx.b = +nctx.b;
          if (ctx.neg) {
            ctx.neg = false;
            nctx.neg = true;
            cid = constants().ids.forall;
          }
        } else if (decl->e()==NULL && cid == constants().ids.bool2int) {
          if (ctx.neg) {
            ctx.neg = false;
            nctx.neg = true;
            nctx.b = -ctx.i;
          } else {
            nctx.b = ctx.i;
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
          for (unsigned int i=c->args().size(); i--;) {
            Ctx argctx = nctx;
            if (decl->e()!=NULL ||
                (cid != constants().ids.forall && cid != constants().ids.exists && cid != constants().ids.bool2int &&
                 cid != constants().ids.sum && cid != constants().ids.lin_exp && cid != "assert")) {
              if (c->args()[i]->type()._bt==Type::BT_BOOL) {
                argctx.b = C_MIX;
              } else if (c->args()[i]->type()._bt==Type::BT_INT) {
                argctx.i = C_MIX;
              }
            }
            Expression* tmp = c->args()[i];
            args_ee[i] = flat_exp(env,argctx,tmp,NULL,NULL);
          }

          GCLock lock;
          std::vector<Expression*> args;
          if (decl->e()==NULL && (cid == constants().ids.forall || cid == constants().ids.exists)) {
            ArrayLit* al = follow_id(args_ee[0].r())->cast<ArrayLit>();
            std::vector<Expression*> alv;
            for (unsigned int i=0; i<al->v().size(); i++) {
              if (Call* sc = same_call(al->v()[i],cid)) {
                ArrayLit* sc_c = eval_array_lit(sc->args()[0]);
                for (unsigned int j=0; j<sc_c->v().size(); j++) {
                  alv.push_back(sc_c->v()[j]);
                }
              } else {
                alv.push_back(al->v()[i]);
              }
            }
            if (cid == constants().ids.exists) {
              std::vector<Expression*> pos_alv;
              std::vector<Expression*> neg_alv;
              for (unsigned int i=0; i<alv.size(); i++) {
                Call* neg_call = same_call(alv[i],"bool_eq");
                if (neg_call && 
                    Expression::equal(neg_call->args()[1],constants().lit_false)) {
                  neg_alv.push_back(neg_call->args()[0]);
                } else {
                  Call* clause = same_call(alv[i],"bool_clause");
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
                  ret.r = bind(env,ctx,r,pos_alv[0]);
                  return ret;
                }
                ArrayLit* nal = new ArrayLit(al->loc(),pos_alv);
                nal->type(al->type());
                args.push_back(nal);
              } else {
                ArrayLit* pos_al = new ArrayLit(al->loc(),pos_alv);
                pos_al->type(al->type());
                ArrayLit* neg_al = new ArrayLit(al->loc(),neg_alv);
                neg_al->type(al->type());
                cid = constants().ids.bool_clause;
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
                ret.r = bind(env,ctx,r,alv[0]);
                return ret;
              }
              ArrayLit* nal = new ArrayLit(al->loc(),alv);
              nal->type(al->type());
              args.push_back(nal);
            }
          } else if (decl->e()==NULL && (cid == constants().ids.lin_exp || cid==constants().ids.sum)) {

            Expression* al_arg = (cid==constants().ids.sum ? c->args()[0] : c->args()[1]);
            EE flat_al = flat_exp(env,nctx,al_arg,NULL,NULL);
            ArrayLit* al = follow_id(flat_al.r())->cast<ArrayLit>();
            IntVal d = (cid == constants().ids.sum ? 0 : eval_int(c->args()[2]));
            
            std::vector<IntVal> c_coeff(al->v().size());
            if (cid==constants().ids.sum) {
              for (unsigned int i=al->v().size(); i--;)
                c_coeff[i] = 1;
            } else {
              EE flat_coeff = flat_exp(env,nctx,c->args()[0],NULL,NULL);
              ArrayLit* coeff = follow_id(flat_coeff.r())->cast<ArrayLit>();
              for (unsigned int i=coeff->v().size(); i--;)
                c_coeff[i] = eval_int(coeff->v()[i]);
            }
            cid = constants().ids.lin_exp;
            std::vector<IntVal> coeffv;
            std::vector<KeepAlive> alv;
            for (unsigned int i=0; i<al->v().size(); i++) {
              if (Call* sc = same_call(al->v()[i],cid)) {
                IntVal cd = c_coeff[i];
                ArrayLit* sc_coeff = eval_array_lit(sc->args()[0]);
                ArrayLit* sc_al = eval_array_lit(sc->args()[1]);
                IntVal sc_d = eval_int(sc->args()[2]);
                assert(sc_coeff->v().size() == sc_al->v().size());
                for (unsigned int j=0; j<sc_coeff->v().size(); j++) {
                  coeffv.push_back(cd*eval_int(sc_coeff->v()[j]));
                  alv.push_back(sc_al->v()[j]);
                }
                d += cd*sc_d;
              } else {
                coeffv.push_back(c_coeff[i]);
                alv.push_back(al->v()[i]);
              }
            }
            simplify_lin(coeffv,alv,d);
            if (coeffv.size()==0) {
              ret.b = conj(env,b,Ctx(),args_ee);
              ret.r = bind(env,ctx,r,new IntLit(Location(),d));
              break;
            } else if (coeffv.size()==1 && coeffv[0]==1 && d==0) {
              ret.b = conj(env,b,Ctx(),args_ee);
              ret.r = bind(env,ctx,r,alv[0]());
              break;
            }
            std::vector<Expression*> coeff_ev(coeffv.size());
            for (unsigned int i=coeff_ev.size(); i--;)
              coeff_ev[i] = new IntLit(Location(),coeffv[i]);
            ArrayLit* ncoeff = new ArrayLit(Location(),coeff_ev);
            ncoeff->type(Type::parint(1));
            args.push_back(ncoeff);
            std::vector<Expression*> alv_e(alv.size());
            for (unsigned int i=alv.size(); i--;)
              alv_e[i] = alv[i]();
            ArrayLit* nal = new ArrayLit(al->loc(),alv_e);
            nal->type(al->type());
            args.push_back(nal);
            IntLit* il = new IntLit(Location(),d);
            il->type(Type::parint());
            args.push_back(il);
          } else {
            for (unsigned int i=0; i<args_ee.size(); i++)
              args.push_back(args_ee[i].r());
          }
          Call* cr = new Call(Location(),cid,args);
          cr->type(c->type());
          decl = env.orig->matchFn(cr);
          assert(decl);
          cr->decl(decl);
          EnvI::Map::iterator cit = env.map_find(cr);
          if (cit != env.map_end()) {
            ret.b = bind(env,Ctx(),b,cit->second.b());
            ret.r = bind(env,ctx,r,cit->second.r());
          } else {
            if (decl->e()==NULL) {
              /// For now assume that all builtins are total
              if (decl->_builtins.e) {
                Expression* callres =
                decl->_builtins.e(cr->args());
                EE res = flat_exp(env,ctx,callres,r,b);
                args_ee.push_back(res);
                ret.b = conj(env,b,Ctx(),args_ee);
                ret.r = bind(env,ctx,r,res.r());
                env.map_insert(cr,ret);
              } else {
                ret.b = conj(env,b,Ctx(),args_ee);
                ret.r = bind(env,ctx,r,cr);
                env.map_insert(cr,ret);
              }
            } else {
              std::vector<std::pair<Id*,Expression*> > idmap;
              // Save mapping from Ids to VarDecls and set to parameters
              /// TODO: save vd->_e as well (if we want to support recursive functions)
              for (unsigned int i=decl->params().size(); i--;) {
                VarDecl* vd = decl->params()[i];
                vd->flat(vd);
                vd->e(args[i]);
              }
              if (isTotal(decl)) {
                EE ee = flat_exp(env,Ctx(),decl->e(),r,constants().var_true);
                ret.r = bind(env,ctx,r,ee.r());
                ret.b = conj(env,b,Ctx(),args_ee);
                env.map_insert(cr,ret);
              } else {
                ret = flat_exp(env,ctx,decl->e(),r,NULL);
                args_ee.push_back(ret);
                ret.b = conj(env,b,Ctx(),args_ee);
                env.map_insert(cr,ret);
              }
              // Restore previous mapping
              for (unsigned int i=decl->params().size(); i--;) {
                VarDecl* vd = decl->params()[i];
                vd->flat(NULL);
                vd->e(NULL);
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
          VarDecl* vd = new VarDecl(Location(),
                                    eval_typeinst(env,v->ti()),
                                    v->id()->v().str());
          vd->introduced(v->introduced());
          if (v->ann()) {
            vd->annotate(
              static_cast<Annotation*>(
              flat_exp(env,Ctx(),v->ann(),NULL,constants().var_true).r())
            );
          }
          VarDeclI* nv = new VarDeclI(Location(),vd);
          Ctx nctx;
          if (v->e() && v->e()->type().isbool())
            nctx.b = C_MIX;
          if (v->e()) {
            (void) flat_exp(env,nctx,v->e(),vd,constants().var_true);
            if (v->e()->type()._bt==Type::BT_INT && v->e()->type()._dim==0) {
              IntSetVal* ibv = NULL;
              if (v->e()->type().isset()) {
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
          env.flat->addItem(nv);

          vd->flat(vd);
          v->flat(vd);
          ret.r = bind(env,Ctx(),r,vd);
        } else {
          ret.r = bind(env,Ctx(),r,it);
        }
        ret.b = bind(env,Ctx(),b,constants().lit_true);
      }
      break;
    case Expression::E_LET:
      {
        GCLock lock;
        Let* let = e->cast<Let>();
        GC::mark();
        std::vector<EE> cs;
        std::vector<std::pair<Id*,Expression*> > idmap;
        let->pushbindings();
        for (unsigned int i=0; i<let->let().size(); i++) {
          Expression* le = let->let()[i];
          if (VarDecl* vd = le->dyn_cast<VarDecl>()) {
            EE ee;
            TypeInst* ti = eval_typeinst(env,vd->ti());
            VarDecl* nvd = new VarDecl(Location(),ti,
                                      env.genId("FromLet_"+vd->id()->v().str()));
            nvd->toplevel(true);
            nvd->introduced(true);
            nvd->type(vd->type());
            VarDeclI* nv = new VarDeclI(Location(),nvd);
            env.flat->addItem(nv);
            if (vd->e()) {
              ee = flat_exp(env,ctx,vd->e(),nvd,NULL);
              cs.push_back(ee);
            } else {
              if (ctx.b==C_NEG || ctx.b==C_MIX)
                throw FlatteningError(vd->loc(),
                  "free variable in non-positive context");
            }
            Id* nid = nvd->id();
            nvd->flat(nvd);
            vd->e(nid);
            (void) flat_exp(env,Ctx(),nid,NULL,constants().var_true);
            assert(vd->flat()==NULL);
            vd->flat(vd);
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
          EE ee = flat_exp(env,ctx,let->in(),NULL,NULL);
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
            vd->flat(NULL);
          }
        }
      }
      break;
    case Expression::E_ANN:
      {
        GCLock lock;
        Annotation* ann = e->cast<Annotation>();
        EE ee = flat_exp(env,Ctx(),ann->e(),NULL,constants().var_true);
        EE ea = flat_exp(env,Ctx(),ann->next(),NULL,constants().var_true);
        ret.r = new Annotation(Location(),ee.r(),
                               static_cast<Annotation*>(ea.r()));
        ret.b = b;
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
  
  void createOutput(EnvI& e) {
    CopyMap cmap;
    class OV1 : public ItemVisitor {
    public:
      EnvI& env;
      CopyMap& cmap;
      OV1(EnvI& env0, CopyMap& cmap0) : env(env0), cmap(cmap0) {}
      void vOutputI(OutputI* oi) {
        GCLock lock;
        env.output->addItem(copy(cmap, oi));
      }
    } _ov1(e,cmap);
    iterItems(_ov1,e.orig);

    class OV2 : public ItemVisitor {
    public:
      EnvI& env;
      CopyMap& cmap;
      OV2(EnvI& env0, CopyMap& cmap0) : env(env0), cmap(cmap0) {}
      void vVarDeclI(VarDeclI* vdi) {
        if (Expression* vd = cmap.find(vdi->e())) {
          GCLock lock;
          // Delete domain constraint, not needed in ozn
          vd->cast<VarDecl>()->ti()->domain(NULL);
          env.output->addItem(copy(cmap,vdi));
          if (!vdi->e()->type().ispar()) {
            // Remove right hand side
            // This will need to be changed, so that if there is a right hand side
            // the FlatZinc does not need to contain the output variable at all.
            vd->cast<VarDecl>()->e(NULL);
            assert(vdi->e()->flat());
            if (vdi->e()->type().dim() == 0) {
              vdi->e()->flat()->annotate(new Annotation(Location(),constants().ann.output_var));
            } else {
              std::vector<Expression*> args(vdi->e()->type().dim());
              for (int i=0; i<args.size(); i++)
                args[i] = new SetLit(Location(), eval_intset(vdi->e()->ti()->ranges()[i]->domain()));
              ArrayLit* al = new ArrayLit(Location(), args);
              args.resize(1);
              args[0] = al;
              Annotation* ann = new Annotation(Location(),
                                               new Call(Location(),constants().ann.output_array,args,NULL));
              vdi->e()->flat()->annotate(ann);
            }
          }
        }
      }
    } _ov2(e,cmap);
    iterItems(_ov2,e.orig);
  }
  
  void flatten(Env& e) {
    EnvI& env = e.envi();
    
    class FV : public ItemVisitor {
    public:
      EnvI& env;
      FV(EnvI& env0) : env(env0) {}
      void vVarDeclI(VarDeclI* v) {
        if (v->e()->type().isvar()) {
          (void) flat_exp(env,Ctx(),v->e(),NULL,constants().var_true);
        }
      }
      void vConstraintI(ConstraintI* ci) {
        (void) flat_exp(env,Ctx(),ci->e(),constants().var_true,constants().var_true);
      }
      void vSolveI(SolveI* si) {
        EE ee = flat_exp(env,Ctx(),si->ann(),NULL,constants().var_true);
        Annotation* ann = static_cast<Annotation*>(ee.r());
        GCLock lock;
        switch (si->st()) {
        case SolveI::ST_SAT:
          env.flat->addItem(SolveI::sat(Location(),ann));
          break;
        case SolveI::ST_MIN:
          env.flat->addItem(SolveI::min(Location(),
            flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r(),
            ann));
          break;
        case SolveI::ST_MAX:
          env.flat->addItem(SolveI::max(Location(),
            flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r(),
            ann));
          break;
        }
      }
    } _fv(env);
    iterItems<FV>(_fv,e.model());
    createOutput(env);
  }

  void oldflatzinc(Model* m) {
    struct {
    public:
      bool operator() (const Item* i) {
        return i->isa<VarDeclI>() &&
          (i->cast<VarDeclI>()->e()->type()._ot == Type::OT_OPTIONAL ||
           i->cast<VarDeclI>()->e()->type()._bt == Type::BT_ANN);
      }
    } _isOptVar;

    m->_items.erase(remove_if(m->_items.begin(), m->_items.end(), _isOptVar),
      m->_items.end());
        
    Model tmp;
    class FV : public ItemVisitor {
    public:
      Model& tmp;
      FV(Model& tmp0)
        : tmp(tmp0) {}
      void vVarDeclI(VarDeclI* v) {
        GCLock lock;
        VarDecl* vd = v->e();
        if (vd->type().isvar() && vd->type().isbool()) {
          if (Expression::equal(vd->ti()->domain(),constants().lit_true)) {
            Expression* ve = vd->e();
            vd->e(constants().lit_true);
            vd->ti()->domain(NULL);
            if (ve != NULL) {
              if (Call* vc = ve->dyn_cast<Call>()) {
                if (vc->id() == constants().ids.exists) {
                  vc->id(ASTString("array_bool_or"));
                  std::vector<Expression*> args(2);
                  args[0] = vc->args()[0];
                  args[1] = constants().lit_true;
                  ASTExprVec<Expression> argsv(args);
                  vc->args(argsv);
                } else if (vc->id() == constants().ids.forall) {
                  vc->id(ASTString("array_bool_and"));
                  std::vector<Expression*> args(2);
                  args[0] = vc->args()[0];
                  args[1] = constants().lit_true;
                  ASTExprVec<Expression> argsv(args);
                  vc->args(argsv);
                }
              }
              tmp._items.push_back(new ConstraintI(Location(),ve));
            }
          } else {
            if (vd->e() != NULL) {
              if (vd->e()->eid()==Expression::E_CALL) {
                Call* c = vd->e()->cast<Call>();
                vd->e(NULL);
                if (c->id() == constants().ids.exists) {
                  c->id(ASTString("array_bool_or"));
                } else if (c->id() == constants().ids.forall) {
                  c->id(ASTString("array_bool_and"));
                } else {
                  c->id(ASTString(c->id().str()+"_reif"));
                }
                std::vector<Expression*> args(c->args().size());
                std::copy(c->args().begin(),c->args().end(),args.begin());
                args.push_back(vd->id());
                c->args(ASTExprVec<Expression>(args));
                tmp._items.push_back(new ConstraintI(Location(),c));
              } else {
                assert(vd->e()->eid() == Expression::E_ID ||
                       vd->e()->eid() == Expression::E_BOOLLIT);
              }
            }
          }
        } else if (vd->type().isvar() && vd->type()._dim==0) {
          if (vd->e() != NULL) {
            if (vd->e()->eid()==Expression::E_CALL) {
              Call* c = vd->e()->cast<Call>();
              vd->e(NULL);
              std::vector<Expression*> args(c->args().size());
              if (c->id() == "lin_exp") {
                c->id(ASTString("int_lin_eq"));
                ArrayLit* le_c = c->args()[0]->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),nc.begin());
                nc.push_back(new IntLit(Location(),-1));
                c->args()[0] = new ArrayLit(Location(),nc);
                ArrayLit* le_x = c->args()[1]->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->v().size());
                std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                nx.push_back(vd->id());
                c->args()[1] = new ArrayLit(Location(),nx);
                IntVal d = c->args()[2]->cast<IntLit>()->v();
                c->args()[2] = new IntLit(Location(),-d);
              } else {
                if (c->id() == "card") {
                  // card is 'set_card' in old FlatZinc
                  c->id(ASTString("set_card"));
                }
                args.push_back(vd->id());
              }
              std::copy(c->args().begin(),c->args().end(),args.begin());
              c->args(ASTExprVec<Expression>(args));
              tmp._items.push_back(new ConstraintI(Location(),c));
            } else {
              assert(vd->e()->eid() == Expression::E_ID ||
                     vd->e()->eid() == Expression::E_INTLIT ||
                     vd->e()->eid() == Expression::E_BOOLLIT ||
                     vd->e()->eid() == Expression::E_SETLIT);
              
            }
          }
        } else if (vd->type()._dim > 0) {
          if (vd->ti()->ranges().size() == 1 &&
              vd->ti()->ranges()[0]->domain() != NULL &&
              vd->ti()->ranges()[0]->domain()->isa<SetLit>()) {
            IntSetVal* isv = vd->ti()->ranges()[0]->domain()->cast<SetLit>()->isv();
            if (isv && isv->min(0)==1)
              return;
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
      }
      void vConstraintI(ConstraintI* ci) {
        if (Call* vc = ci->e()->dyn_cast<Call>()) {
          if (vc->id() == constants().ids.exists) {
            GCLock lock;
            vc->id(ASTString("array_bool_or"));
            std::vector<Expression*> args(2);
            args[0] = vc->args()[0];
            args[1] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
          } else if (vc->id() == constants().ids.forall) {
            GCLock lock;
            vc->id(ASTString("array_bool_and"));
            std::vector<Expression*> args(2);
            args[0] = vc->args()[0];
            args[1] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
          }
        }
      }
    } _fv(tmp);
    iterItems<FV>(_fv,m);
    for (unsigned int i=0; i<tmp._items.size(); i++)
      m->addItem(tmp._items[i]);
    
    class Cmp {
    public:
      bool operator() (Item* i, Item* j) {
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
          if (i->cast<VarDeclI>()->e()->e()==NULL &&
              j->cast<VarDeclI>()->e()->e() != NULL)
            return true;
        }
        if (j->iid()==Item::II_VD) {
          if (j->iid() != i->iid())
            return false;
          if (j->cast<VarDeclI>()->e()->type()._dim == 0 &&
              i->cast<VarDeclI>()->e()->type()._dim != 0)
            return false;
          if (j->cast<VarDeclI>()->e()->e()==NULL &&
              i->cast<VarDeclI>()->e()->e() != NULL)
            return false;
        }
        return i<j;
      }
    } _cmp;
    std::stable_sort(m->_items.begin(),m->_items.end(),_cmp);

  }

}
