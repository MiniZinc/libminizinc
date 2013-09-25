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

  /// Output operator for locations
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

  const char* ctx_ann(BCtx& c) {
    std::string ctx;
    switch (c) {
    case C_ROOT: return "ctx_root";
    case C_POS: return "ctx_pos";
    case C_NEG: return "ctx_neg";
    case C_MIX: return "ctx_mix";
    default: assert(false); return NULL;
    }
  }
  
  void addCtxAnn(VarDecl* vd, BCtx& c) {
    if (vd) {
      const char* ctx = ctx_ann(c);
      Annotation* vdann = vd->_ann;
      while (vdann) {
        if (Id* id = vdann->_e->dyn_cast<Id>()) {
          if (id->_v==ctx)
            return;
        }
        vdann = vdann->_a;
      }
      Id* id = Id::a(Location(),ctx,NULL);
      id->_type = Type::ann();
      vd->annotate(Annotation::a(Location(),id));
    }
  }

  /// Result of evaluation
  class EE {
  public:
    /// The result value
    Expression* r;
    /// Boolean expression representing whether result is defined
    Expression* b;
    /// Constructor
    explicit EE(Expression* r0=NULL, Expression* b0=NULL) : r(r0), b(b0) {}
  };

  Constants::Constants(void) {
    GC::init();
    GCLock lock;
    TypeInst* ti = TypeInst::a(Location(), Type::parbool());
    lt = BoolLit::a(Location(), true);
    t = VarDecl::a(Location(), ti, "_bool_true", lt);
    lf = BoolLit::a(Location(), false);
    f = VarDecl::a(Location(), ti, "_bool_false", lf);
    m = new Model;
    std::vector<Expression*> v;
    v.push_back(ti);
    v.push_back(lt);
    v.push_back(t);
    v.push_back(lf);
    v.push_back(f);
    m->_items.push_back(
      ConstraintI::a(Location(),ArrayLit::a(Location(),v)));
  }
  
  Constants& constants(void) {
    static Constants _c;
    return _c;
  }

  /// Check if \a e is NULL or true
  bool istrue(Expression* e) {
    return e==NULL || (e->_type.ispar() && e->_type.isbool()
                       && eval_bool(e));
  }  
  /// Check if \a e is non-NULL and false
  bool isfalse(Expression* e) {
    return e!=NULL && e->_type.ispar() && e->_type.isbool()
           && !eval_bool(e);
  }  

  class EnvIter;

  class Env : public ASTRootSet {
  public:
    Model* orig;
    Model* m;
    typedef ExpressionMap<EE> Map;
    Map map;
    unsigned int ids;
    Env(Model* orig0, Model* m0) : orig(orig0), m(m0), ids(0) {
      GC::addRootSet(this);
    }
    ASTString genId(const std::string& s) {
      std::ostringstream oss; oss << "_" << s << "_" << ids++;
      return ASTString(oss.str());
    }
    ~Env(void) {
      GC::removeRootSet(this);
    }
    ASTRootSetIter* rootSet(void);
  };

  class EnvIter : public ASTRootSetIter {
  public:
    std::vector<Expression*> r;
    EnvIter(std::vector<Expression*>& r0) : r(r0) {}
    virtual Expression** begin(void) {
      return &r[0];
    }
    virtual Expression** end(void) {
      return &r[r.size()-1]+1;
    }
    virtual ~EnvIter(void) {}
  };

  ASTRootSetIter*
  Env::rootSet(void) {
    std::vector<Expression*> r;
    for (Map::iterator it = map.begin(); it != map.end(); ++it) {
      r.push_back(it->first);
      if (it->second.r)
        r.push_back(it->second.r);
      if (it->second.b)
        r.push_back(it->second.b);
    }
    return new EnvIter(r);
  }

  bool isTotal(FunctionI* fi) {
    Annotation* a = fi->_ann;
    for (; a!=NULL; a=a->_a) {
      VarDecl* vd = NULL;
      Expression * ae = a->_e;
      while (ae && ae->eid()==Expression::E_ID &&
             ae->cast<Id>()->_decl!=NULL) {
        vd = ae->cast<Id>()->_decl;
        ae = vd->_e;
      }
      
      if (vd && vd->_type.isann() && vd->_id == "total") {
        return true;
      }
    }
    return false;
  }

  EE flat_exp(Env& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b);

  Expression* bind(Env& env, Ctx ctx, VarDecl* vd, Expression* e) {
    if (ctx.neg) {
      assert(e->_type._bt == Type::BT_BOOL);
      if (vd==constants().t) {
        if (!isfalse(e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            assert(id->_decl != NULL);
            if (id->_decl->_ti->_domain && istrue(id->_decl->_ti->_domain)) {
              env.m->addItem(ConstraintI::a(Location(),constants().lf));
            } else {
              id->_decl->_ti->_domain = constants().lf;
            }
            return constants().lt;
          } else {
            BinOp* bo = BinOp::a(e->_loc,e,BOT_EQUIV,constants().lf);
            bo->_type = e->_type;
            EE ee = flat_exp(env,Ctx(),bo,NULL,constants().t);
            return bind(env,Ctx(),vd,ee.r);
          }
        }
        return constants().lt;
      } else {
        BinOp* bo = BinOp::a(e->_loc,e,BOT_EQUIV,constants().lf);
        bo->_type = e->_type;
        EE ee = flat_exp(env,Ctx(),bo,NULL,constants().t);
        return bind(env,Ctx(),vd,ee.r);
      }
    } else {
      if (vd==constants().t) {
        if (!istrue(e)) {
          if (Id* id = e->dyn_cast<Id>()) {
            assert(id->_decl != NULL);
            if (id->_decl->_ti->_domain && isfalse(id->_decl->_ti->_domain)) {
              env.m->addItem(ConstraintI::a(Location(),constants().lf));
            } else {
              id->_decl->_ti->_domain = constants().lt;
            }
          } else {
            env.m->addItem(ConstraintI::a(Location(),e));
          }
        }
        return constants().lt;
      } else if (vd==constants().f) {
        if (!isfalse(e)) {
          throw InternalError("not supported yet");
        }
        return constants().lt;
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
            if (e->_type.isann())
              return e;
            /// TODO: handle array types
            TypeInst* ti = TypeInst::a(Location(),e->_type);
            VarDecl* vd = VarDecl::a(Location(),ti,env.genId("X"),e);

            if (vd->_e->_type._bt==Type::BT_INT && vd->_e->_type._dim==0) {
              IntBounds ib = compute_int_bounds(vd->_e);
              if (ib.valid) {
                IntSetVal* ibv = IntSetVal::a(ib.l,ib.u);
                if (vd->_ti->_domain) {
                  IntSetVal* domain = eval_intset(vd->_ti->_domain);
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(ibv);
                  Ranges::Inter<IntSetRanges,IntSetRanges> i(dr,ibr);
                  ibv = IntSetVal::ai(i);
                }
                vd->_ti->_domain = SetLit::a(Location(),ibv);
              }
            } else if (vd->_e->_type.isbool()) {
              addCtxAnn(vd, ctx.b);
            }

            vd->introduced(true);
            VarDeclI* nv = VarDeclI::a(Location(),vd);
            env.m->addItem(nv);
            Id* id = Id::a(Location(),vd->_id,vd);
            id->_type = e->_type;

            EE ee(vd,NULL);
            env.map.insert(id,ee);

            return id;
          }
        default:
          assert(false); return NULL;
        }
      } else {
        if (vd->_e==NULL) {
          if (e==NULL) {
            vd->_e = constants().lt;
          } else if (e->_type.ispar() && e->_type.isbool()) {
            if (eval_bool(e)) {
              vd->_e = constants().lt;
            } else {
              vd->_e = constants().lf;
            }
          } else {
            vd->_e = e;
            if (vd->_e->_type._bt==Type::BT_INT && vd->_e->_type._dim==0) {
              IntBounds ib = compute_int_bounds(vd->_e);
              if (ib.valid) {
                IntSetVal* ibv = IntSetVal::a(ib.l,ib.u);
                if (vd->_ti->_domain) {
                  IntSetVal* domain = eval_intset(vd->_ti->_domain);
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(ibv);
                  Ranges::Inter<IntSetRanges,IntSetRanges> inter(dr,ibr);
                  ibv = IntSetVal::ai(inter);
                }
                vd->_ti->_domain = SetLit::a(Location(),ibv);
              }
            }
          }
          return e;
        } else if (vd->_e != e) {
          if (Call* c = e->dyn_cast<Call>()) {
            std::vector<Expression*> args(c->_args.size());
            std::copy(c->_args.begin(),c->_args.end(),args.begin());
            args.push_back(Id::a(Location(),vd->_id,vd));
            c->_args = ASTExprVec<Expression>(args);
            env.m->addItem(ConstraintI::a(Location(),c));
            return vd;
          } else {
            throw InternalError("not supported yet");
          }
        } else {
          return e;
        }
      }
    }
  }

  Expression* conj(Env& env,VarDecl* b,Ctx ctx,const std::vector<EE>& e) {
    if (!ctx.neg) {
      std::vector<Expression*> nontrue;
      for (unsigned int i=0; i<e.size(); i++) {
        if (istrue(e[i].b))
          continue;
        if (isfalse(e[i].b)) {
          return bind(env,Ctx(),b,constants().lf);
        }
        nontrue.push_back(e[i].b);
      }
      if (nontrue.empty()) {
        return bind(env,Ctx(),b,constants().lt);
      } else if (nontrue.size()==1) {
        return bind(env,ctx,b,nontrue[0]);
      } else {
        if (b==constants().t) {
          for (unsigned int i=0; i<nontrue.size(); i++)
            bind(env,ctx,b,nontrue[i]);
          return constants().lt;
        } else {
          std::vector<Expression*> args;
          ArrayLit* al = ArrayLit::a(Location(),nontrue);
          al->_type = Type::varbool(1);
          args.push_back(al);
          Call* ret = Call::a(Location(),"forall",args);
          ret->_type = Type::varbool();
          ret->_decl = env.orig->matchFn(ret);
          return flat_exp(env,ctx,ret,b,constants().t).r;
        }
      }
    } else {
      Ctx nctx = ctx;
      nctx.neg = false;
      // negated
      std::vector<Expression*> nonfalse;
      for (unsigned int i=0; i<e.size(); i++) {
        if (istrue(e[i].b))
          continue;
        if (isfalse(e[i].b)) {
          return bind(env,Ctx(),b,constants().lt);
        }
        nonfalse.push_back(e[i].b);
      }
      if (nonfalse.empty()) {
        return bind(env,Ctx(),b,constants().lf);
      } else if (nonfalse.size()==1) {
        UnOp* uo = UnOp::a(nonfalse[0]->_loc,UOT_NOT,nonfalse[0]);
        uo->_type = Type::varbool();
        return flat_exp(env,nctx,uo,b,constants().t).r;
      } else {
        if (b==constants().f) {
          for (unsigned int i=0; i<nonfalse.size(); i++)
            bind(env,nctx,b,nonfalse[i]);
          return constants().lf;
        } else {
          std::vector<Expression*> args;
          for (unsigned int i=0; i<nonfalse.size(); i++) {
            UnOp* uo = UnOp::a(nonfalse[i]->_loc,UOT_NOT,nonfalse[i]);
            uo->_type = Type::varbool();
            nonfalse[i] = uo;
          }
          ArrayLit* al = ArrayLit::a(Location(),nonfalse);
          al->_type = Type::varbool(1);
          args.push_back(al);
          Call* ret = Call::a(Location(),"exists",args);
          ret->_type = Type::varbool();
          ret->_decl = env.orig->matchFn(ret);
          assert(ret->_decl);
          return flat_exp(env,nctx,ret,b,constants().t).r;
        }
      }
      
    }
  }

  TypeInst* eval_typeinst(Env& env, TypeInst* ti) {
    /// TODO: evaluate all par components in the domain. This probably
    ///       needs the VarDecl to compute the actual dimensions of
    ///       array[int] expressions
    return eval_par(ti)->cast<TypeInst>();
  }

  std::string opToBuiltin(BinOp* op, BinOpType bot) {
    std::string builtin;
    if (op->_e1->_type.isint()) {
      builtin = "int_";
    } else if (op->_e1->_type.isbool()) {
      builtin = "bool_";
    } else if (op->_e1->_type.isset()) {
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
        e = e->cast<Id>()->_decl->_e;
      } else {
        return e;
      }
    }
  }

  Call* same_call(Expression* e, const std::string& id) {
    Expression* ce = follow_id(e);
    if (ce && ce->isa<Call>() && ce->cast<Call>()->_id.str() == id)
      return ce->cast<Call>();
    return NULL;
  }

  Expression* mklinexp(Env& env, IntVal c0, IntVal c1,
                       Expression* e0, Expression* e1) {
    IntVal d = 0;
    if (e0->_type.ispar() && e0->_type.isint()) {
      d += c0*eval_int(e0);
      e0 = NULL;
    }
    if (e1 && e1->_type.ispar() && e1->_type.isint()) {
      d += c1*eval_int(e1);
      e1 = NULL;
    }
    if (e0==NULL && e1==NULL)
      return IntLit::a(e0->_loc,d);
    if (e0==NULL) {
      std::swap(e0,e1);
      std::swap(c0,c1);
    }
    std::vector<Expression*> bo_args(e1 ? 2 : 1);
    bo_args[0] = e0;
    if (e1)
      bo_args[1] = e1;
    std::vector<Expression*> coeffs(e1 ? 2 : 1);
    coeffs[0] = IntLit::a(e0->_loc,c0);
    if (e1) {
      if (c0==c1)
        coeffs[1] = coeffs[0];
      else
        coeffs[1] = IntLit::a(e0->_loc,c1);
    }
    std::vector<Expression*> args(3);
    args[0]=ArrayLit::a(e0->_loc,coeffs);
    args[0]->_type = Type::parint(1);
    args[1]=ArrayLit::a(e0->_loc,bo_args);
    args[1]->_type = e0->_type;
    args[1]->_type._dim=1;
    if (e0->_type._ti==Type::TI_PAR && e1)
      args[1]->_type._ti = e1->_type._ti;
    args[2] = IntLit::a(e0->_loc,d);
    args[2]->_type = Type::parint();
    Call* c = Call::a(e0->_loc,"lin_exp",args);
    c->_type = args[1]->_type;
    c->_type._dim = 0;
    c->_decl = env.orig->matchFn(c);
    return c;
  }

  
  class CmpExpIdx {
  public:
    std::vector<Expression*>& x;
    CmpExpIdx(std::vector<Expression*>& x0) : x(x0) {}
    bool operator ()(int i, int j) const {
      if (Expression::equal(x[i],x[j]))
        return false;
      return x[i]<x[j];
    }
  };

  void simplify_lin(std::vector<IntVal>& c,
                    std::vector<Expression*>& x,
                    IntVal& d) {
    std::vector<int> idx(c.size());
    for (unsigned int i=idx.size(); i--;) {
      idx[i]=i;
    }
    std::sort(idx.begin(),idx.end(),CmpExpIdx(x));
    int ci = 0;
    for (; ci<x.size(); ci++) {
      if (IntLit* il = x[idx[ci]]->dyn_cast<IntLit>()) {
        d += c[idx[ci]]*il->_v;
        c[idx[ci]] = 0;
      } else {
        break;
      }
    }
    for (unsigned int i=ci+1; i<x.size(); i++) {
      if (Expression::equal(x[idx[i]],x[idx[ci]])) {
        c[idx[ci]] += c[idx[i]];
        c[idx[i]] = 0;
      } else if (IntLit* il = x[idx[i]]->dyn_cast<IntLit>()) {
        d += c[idx[i]]*il->_v;
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
        if (x[i]->isa<BoolLit>() && x[i]->cast<BoolLit>()->_v==identity) {
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
        if (e->cast<Id>()->_decl->_e) {
          e = e->cast<Id>()->_decl->_e;
        } else {
          break;
        }
      } else {
        break;
      }
    }
    if (e && (e->isa<Id>() || e->isa<IntLit>() ||
              (e->isa<Call>() && e->cast<Call>()->_id.str() == "lin_exp")))
      return e;
    return NULL;
  }

  EE flat_exp(Env& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
    EE ret;
    assert(!e->_type.isunknown());
    if (e->_type.ispar() && !e->isa<Let>() && e->_type._bt!=Type::BT_ANN) {
      ret.b = bind(env,Ctx(),b,constants().lt);
      ret.r = bind(env,ctx,r,eval_par(e));
      return ret;
    }
    switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_SETLIT:
    case Expression::E_STRINGLIT:
      ret.b = bind(env,Ctx(),b,constants().lt);
      ret.r = bind(env,Ctx(),r,e);
      return ret;
    case Expression::E_BOOLLIT:
      {
        ret.b = bind(env,Ctx(),b,constants().lt);
        ret.r = bind(env,ctx,r,e);
        return ret;
      }
      break;
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->_decl==NULL)
          throw FlatteningError(e->_loc, "undefined identifier");
        if (ctx.neg && id->_type._dim > 0) {
          if (id->_type._dim > 1)
            throw InternalError("multi-dim arrays in negative positions not supported yet");
          std::vector<ASTString> gen_id(1);
          gen_id[0] = env.genId(id->_v.str()+"_idx");
          /// TODO: support arbitrary dimensions
          std::vector<Expression*> idxsetargs(1);
          idxsetargs[0] = id;
          Call* idxset = Call::a(id->_loc,"index_set",idxsetargs);
          idxset->_type = Type::parsetint();
          idxset->_decl = env.orig->matchFn(idxset);
          Generator gen(gen_id,idxset);
          std::vector<Expression*> idx(1);
          idx[0] = Id::a(id->_loc,gen._v[0]->_id,gen._v[0]);
          Generators gens;
          gens._g.push_back(gen);
          gens._w = NULL;
          ArrayAccess* aa = ArrayAccess::a(id->_loc,id,idx);
          aa->_type = id->_type; aa->_type._dim = 0;
          UnOp* aanot = UnOp::a(id->_loc,UOT_NOT,aa);
          aanot->_type = aa->_type;
          Comprehension* cp = Comprehension::a(id->_loc,
            aanot, gens, false);
          cp->_type = id->_type;
          ctx.neg = false;
          ret = flat_exp(env,ctx,cp,r,b);
        } else {
          Env::Map::iterator it = env.map.find(id);
          VarDecl* vd = NULL;
          Expression* rete = NULL;
          if (it==env.map.end()) {
            // New top-level id, need to copy into env.m
            vd = flat_exp(env,Ctx(),id->_decl,NULL,constants().t).r
                 ->cast<VarDecl>();
          } else {
            switch (it->second.r->eid()) {
            case Expression::E_VARDECL:
              vd = it->second.r->cast<VarDecl>();
              break;
            case Expression::E_ID:
              vd = it->second.r->cast<Id>()->_decl;
              break;
            default:
              rete = it->second.r;
              break;
            }
          }
          ret.b = bind(env,Ctx(),b,constants().lt);
          if (vd && vd->_e!=NULL) {
            switch (vd->_e->eid()) {
            case Expression::E_INTLIT:
            case Expression::E_BOOLLIT:
            case Expression::E_FLOATLIT:
            case Expression::E_ID:
              rete = vd->_e;
              break;
            default: break;
            }
          } else if (vd && vd->_ti->_ranges.size() > 0) {
            // create fresh variables and array literal
            std::vector<std::pair<int,int> > dims;
            TypeInst* vti =
              TypeInst::a(Location(),vd->_ti->_type,vd->_ti->_domain);
            unsigned int asize = 1;
            for (unsigned int i=0; i<vd->_ti->_ranges.size(); i++) {
              TypeInst* ti = vd->_ti->_ranges[i];
              if (ti->_domain==NULL)
                throw FlatteningError(ti->_loc,"array dimensions unknown");
              IntSetVal* isv = eval_intset(ti->_domain);
              if (isv->size() != 1)
                throw FlatteningError(ti->_loc,"invalid array index set");
              asize *= (isv->max(0)-isv->min(0)+1);
              dims.push_back(std::pair<int,int>(isv->min(0),isv->max(0)));
            }
            std::vector<Expression*> elems(asize);
            for (int i=0; i<asize; i++) {
              ASTString nid = env.genId("fresh_"+vd->_id.str());
              VarDecl* nvd = VarDecl::a(Location(),vti,nid);
              nvd->introduced(true);
              (void) flat_exp(env,Ctx(),nvd,NULL,constants().t);
              Id* id = Id::a(Location(),nid,nvd);
              id->_type = vti->_type;
              id->_type._dim = 0;
              elems[i] = id;
            }

            ArrayLit* al = ArrayLit::a(Location(),elems,dims);
            al->_type = vd->_type;
            vd->_e = al;
          }
          if (rete==NULL) {
            if (!vd->toplevel()) {
              // create new VarDecl in toplevel, if decl doesnt exist yet
              Env::Map::iterator it = env.map.find(vd->_e);
              if (it==env.map.end()) {
                VarDecl* nvd = 
                  VarDecl::a(Location(),eval_typeinst(env,vd->_ti),
                             env.genId("tl_"+vd->_id.str()),vd->_e);
                nvd->introduced(true);
                VarDeclI* ni = VarDeclI::a(Location(),nvd);
                // std::cerr << "create new toplevel " << nvd->_id.c_str() << " for " << vd->_id.str() << " with definition " << vd->_e << "\n";
                env.m->addItem(ni);
                vd = nvd;
                EE ee(vd,NULL);
                if (vd->_e)
                  env.map.insert(vd->_e,ee);
                Id* nid = Id::a(Location(),nvd->_id,NULL);
                nid->_type = nvd->_type;
                env.map.insert(nid,ee);
              } else {
                vd = it->second.r->cast<VarDecl>();
              }
            }
            if (id->_type._bt == Type::BT_ANN && vd->_e) {
              rete = vd->_e;
            } else {
              rete = Id::a(Location(),vd->_id,vd);
              rete->_type = id->_type;
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
        std::vector<EE> elems_ee(al->_v.size());
        for (unsigned int i=al->_v.size(); i--;)
          elems_ee[i] = flat_exp(env,ctx,al->_v[i],NULL,NULL);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r;
        std::vector<std::pair<int,int> > dims(al->dims());
        for (unsigned int i=al->dims(); i--;)
          dims[i] = std::pair<int,int>(al->min(i), al->max(i));
        ArrayLit* alr = ArrayLit::a(Location(),elems,dims);
        alr->_type = al->_type;
        ret.b = conj(env,b,Ctx(),elems_ee);
        ret.r = bind(env,ctx,r,alr);
      }
      break;
    case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        bool parAccess=true;
        for (unsigned int i=0; i<aa->_idx.size(); i++) {
          if (!aa->_idx[i]->_type.ispar()) {
            parAccess = false;
            break;
          }
        }
        if (parAccess) {
          Ctx nctx = ctx;
          nctx.b = +nctx.b;
          nctx.neg = false;
          EE eev = flat_exp(env,nctx,aa->_v,NULL,NULL);
          ArrayLit* al;
          if (eev.r->isa<ArrayLit>()) {
            al = eev.r->cast<ArrayLit>();
          } else {
            Id* id = eev.r->cast<Id>();
            if (id->_decl==NULL) {
              throw InternalError("undefined identifier");
            }
            if (id->_decl->_e==NULL) {
              throw InternalError("array without initialiser not supported");
            }
            al = id->_decl->_e->cast<ArrayLit>();
          }
          std::vector<IntVal> dims(aa->_idx.size());
          for (unsigned int i=aa->_idx.size(); i--;)
            dims[i] = eval_int(aa->_idx[i]);
          Expression* val = eval_arrayaccess(al,dims);
          ret.b = bind(env,Ctx(),b,constants().lt);
          ret.r = bind(env,ctx,r,val);
        } else {
          std::vector<Expression*> args(aa->_idx.size()+1);
          for (unsigned int i=aa->_idx.size(); i--;)
            args[i] = aa->_idx[i];
          args[aa->_idx.size()] = aa->_v;
          Call* cc = Call::a(Location(),"element",args);
          cc->_type = aa->_type;
          FunctionI* fi = env.orig->matchFn(cc->_id,args);
          assert(cc->_type == fi->rtype(args));
          cc->_decl = fi;
          ret = flat_exp(env,ctx,cc,r,b);
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
          Env& env;
          Ctx ctx;
          EvalF(Env& env0, Ctx ctx0) : env(env0), ctx(ctx0) {}
          typedef EE ArrayVal;
          EE e(Expression* e) {
            return flat_exp(env,ctx,e,NULL,NULL);
          }
        } _evalf(env,ctx);
        std::vector<EE> elems_ee = eval_comp<EvalF>(_evalf,c);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r;
        ArrayLit* alr = ArrayLit::a(Location(),elems);
        alr->_type = c->_type;
        ret.b = conj(env,b,Ctx(),elems_ee);
        ret.r = bind(env,Ctx(),r,alr);
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        bool done = false;
        for (int i=0; i<ite->_e_if_then.size(); i+=2) {
          if (eval_bool(ite->_e_if_then[i])) {
            ret = flat_exp(env,ctx,ite->_e_if_then[i+1],r,b);
            done = true;
            break;
          }
        }
        if (!done) {
          ret = flat_exp(env,ctx,ite->_e_else,r,b);
        }
      }
      break;
    case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        if (bo->_decl) {
          std::vector<Expression*> args(2);
          args[0] = bo->_e0;
          args[1] = bo->_e1;
          Call* cr = Call::a(Location(),bo->opToString().str(),args);
          cr->_type = bo->_type;
          cr->_decl = env.orig->matchFn(cr);
          ret = flat_exp(env,ctx,cr,r,b);
        } else {
          Ctx ctx0 = ctx;
          ctx0.neg = false;
          Ctx ctx1 = ctx;
          ctx1.neg = false;
          BinOpType bot = bo->op();
          if (bo->_e0->_type.isbool()) {
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
          Expression* boe0 = bo->_e0;
          Expression* boe1 = bo->_e1;
          switch (bot) {
          case BOT_PLUS:
            {
              Expression* le = mklinexp(env,1,1,boe0,boe1);
              ret = flat_exp(env,ctx,le,r,b);
            }
            break;
          case BOT_MINUS:
            {
              Expression* le = mklinexp(env,1,-1,boe0,boe1);
              ret = flat_exp(env,ctx,le,r,b);
            }
            break;
          case BOT_MULT:
            {
              if (boe0->_type.ispar())
                std::swap(boe0,boe1);
              if (boe1->_type.ispar() && boe1->_type.isint()) {
                IntVal coeff = eval_int(boe1);
                Expression* le = mklinexp(env,coeff,0,boe0,NULL);
                ret = flat_exp(env,ctx,le,r,b);
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

              std::vector<Expression*> args(2);
              args[0] = e0.r; args[1] = e1.r;
              Call* cc = Call::a(Location(),opToBuiltin(bo,bot),args);
              cc->_type = bo->_type;

              if (FunctionI* fi = env.orig->matchFn(cc->_id,args)) {
                assert(cc->_type == fi->rtype(args));
                cc->_decl = fi;
                EE ee = flat_exp(env,ctx,cc,r,NULL);
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
            break;

        case BOT_AND:
            {
              if (r==constants().t) {
                Ctx ctx;
                ctx.neg = negArgs;
                ctx.b = negArgs ? C_NEG : C_ROOT;
                (void) flat_exp(env,ctx,boe0,constants().t,constants().t);
                (void) flat_exp(env,ctx,boe1,constants().t,constants().t);
                break;
              } else {
                std::vector<Expression*> bo_args(2);
                if (negArgs) {
                  bo_args[0] = UnOp::a(bo->_loc,UOT_NOT,boe0);
                  bo_args[0]->_type = boe0->_type;
                  bo_args[1] = UnOp::a(bo->_loc,UOT_NOT,boe1);
                  bo_args[1]->_type = boe1->_type;
                } else {
                  bo_args[0] = boe0;
                  bo_args[1] = boe1;
                }
                std::vector<Expression*> args(1);
                args[0]=ArrayLit::a(bo->_loc,bo_args);
                args[0]->_type = Type::varbool(1);
                Call* c = Call::a(bo->_loc,"forall",args);
                c->_type = bo->_type;
                c->_decl = env.orig->matchFn(c);
                ret = flat_exp(env,ctx,c,r,b);
                if (Id* id = ret.r->dyn_cast<Id>()) {
                  addCtxAnn(id->_decl, ctx.b);
                }
              }
              break;
            }
          case BOT_OR:
            {
              std::vector<Expression*> bo_args(2);
              if (negArgs) {
                bo_args[0] = UnOp::a(bo->_loc,UOT_NOT,boe0);
                bo_args[0]->_type = boe0->_type;
                bo_args[1] = UnOp::a(bo->_loc,UOT_NOT,boe1);
                bo_args[1]->_type = boe1->_type;
              } else {
                bo_args[0] = boe0;
                bo_args[1] = boe1;
              }
              std::vector<Expression*> args(1);
              args[0]= ArrayLit::a(bo->_loc,bo_args);
              args[0]->_type = Type::varbool(1);
              Call* c = Call::a(bo->_loc,"exists",args);
              c->_type = bo->_type;
              c->_decl = env.orig->matchFn(c);
              ret = flat_exp(env,ctx,c,r,b);
              if (Id* id = ret.r->dyn_cast<Id>()) {
                addCtxAnn(id->_decl, ctx.b);
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
              std::vector<Expression*> bo_args(2);
              std::string id;
              if (ctx.neg) {
                bo_args[0] = boe0;
                bo_args[1] = UnOp::a(bo->_loc,UOT_NOT,boe1);
                bo_args[1]->_type = boe1->_type;
                id = "forall";
              } else {
                bo_args[0] = UnOp::a(bo->_loc,UOT_NOT,boe0);
                bo_args[0]->_type = boe0->_type;
                bo_args[1] = boe1;
                id = "exists";
              }
              ctx.neg = false;
              std::vector<Expression*> args(1);
              args[0]= ArrayLit::a(bo->_loc,bo_args);
              args[0]->_type = Type::varbool(1);
              Call* c = Call::a(bo->_loc,id,args);
              c->_type = bo->_type;
              c->_decl = env.orig->matchFn(c);
              ret = flat_exp(env,ctx,c,r,b);
              if (Id* id = ret.r->dyn_cast<Id>()) {
                addCtxAnn(id->_decl,ctx.b);
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
              if (r && r==constants().t) {
                if (boe1->_type.ispar() || boe1->isa<Id>())
                  std::swap(boe0,boe1);
                if (istrue(boe0)) {
                  return flat_exp(env,ctx1,boe1,r,b);
                } else if (isfalse(boe0)) {
                  ctx1.neg = true;
                  ctx1.b = -ctx1.b;
                  return flat_exp(env,ctx1,boe1,r,b);
                } else {
                  EE e0 = flat_exp(env,ctx0,boe0,NULL,NULL);
                  Id* id = e0.r->cast<Id>();
                  (void) flat_exp(env,ctx1,boe1,id->_decl,NULL);
                  ret.b = bind(env,Ctx(),b,constants().lt);
                  ret.r = bind(env,Ctx(),b,constants().lt);
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
              if (boe0->_type.isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->_type.isint()) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            } else {
              if (boe0->_type.isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->_type.isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            }
            goto flatten_bool_op;
          case BOT_GR:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_LQ;
              if (boe0->_type.isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->_type.isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            } else {
              if (boe0->_type.isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->_type.isint()) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            }
            goto flatten_bool_op;
          case BOT_GQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_LE;
              if (boe0->_type.isbool()) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->_type.isint()) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            } else {
              if (boe0->_type.isbool()) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->_type.isint()) {
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
            if (boe0->_type.isbool()) {
              ctx0.b = ctx1.b = C_MIX;
            } else if (boe0->_type.isint()) {
              ctx0.i = ctx1.i = C_MIX;
            }
            goto flatten_bool_op;
          case BOT_NQ:
            if (ctx.neg) {
              doubleNeg = true;
              bot = BOT_EQ;
            }
            if (boe0->_type.isbool()) {
              ctx0.b = ctx1.b = C_MIX;
            } else if (boe0->_type.isint()) {
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
              ret.b = bind(env,Ctx(),b,constants().lt);

              std::vector<Expression*> args;
              std::string callid;

              std::vector<EE> ees(3);
              ees[0].b = e0.b; ees[1].b = e1.b;

              Expression* le0 = 
                (boe0->_type._bt == Type::BT_INT && bot != BOT_IN) ?
                  get_linexp(e0.r) : NULL;
              Expression* le1 = le0 ? get_linexp(e1.r) : NULL;

              if (le1) {
                std::vector<IntVal> coeffv;
                std::vector<Expression*> alv;
                IntVal d = 0;
                Expression* le[2] = {le0,le1};
                for (unsigned int i=0; i<2; i++) {
                  IntVal sign = (i==0 ? 1 : -1);
                  switch (le[i]->eid()) {
                  case Expression::E_INTLIT:
                    d += sign*(le[i]->cast<IntLit>()->_v);
                    break;
                  case Expression::E_ID:
                    coeffv.push_back(sign);
                    alv.push_back(le[i]);
                    break;
                  case Expression::E_CALL:
                    {
                      Call* sc = le[i]->cast<Call>();
                      ArrayLit* sc_coeff = eval_array_lit(sc->_args[0]);
                      ArrayLit* sc_al = eval_array_lit(sc->_args[1]);
                      d += sign*eval_int(sc->_args[2]);
                      for (unsigned int j=0; j<sc_coeff->_v.size(); j++) {
                        coeffv.push_back(sign*eval_int(sc_coeff->_v[j]));
                        alv.push_back(sc_al->_v[j]);
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
                    ret.r = bind(env,ctx,r,constants().lf);
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
                  Expression* e0;
                  Expression* e1;
                  switch (bot) {
                  case BOT_LE:
                    e0 = alv[0];
                    e1 = IntLit::a(Location(),d-1);
                    bot = BOT_LQ;
                    break;
                  case BOT_GR:
                    e0 = IntLit::a(Location(),d+1);
                    e1 = alv[0];
                    bot = BOT_LQ;
                    break;
                  case BOT_GQ:
                    e0 = IntLit::a(Location(),d);
                    e1 = alv[0];
                    bot = BOT_LQ;
                    break;
                  default:
                    e0 = alv[0];
                    e1 = IntLit::a(Location(),d);
                  }
                  args.push_back(e0);
                  args.push_back(e1);
                  callid = opToBuiltin(bo,bot);
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
                  
                  std::vector<Expression*> coeff_ev(coeffv.size());
                  for (unsigned int i=coeff_ev.size(); i--;)
                    coeff_ev[i] = IntLit::a(Location(),coeff_sign*coeffv[i]);
                  ArrayLit* ncoeff = ArrayLit::a(Location(),coeff_ev);
                  ncoeff->_type = Type::parint(1);
                  args.push_back(ncoeff);
                  ArrayLit* nal = ArrayLit::a(Location(),alv);
                  nal->_type = alv[0]->_type;
                  nal->_type._dim = 1;
                  args.push_back(nal);
                  IntLit* il = IntLit::a(Location(),-d);
                  il->_type = Type::parint();
                  args.push_back(il);
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

              Call* cc = Call::a(Location(),callid,args);
              cc->_type = bo->_type;

              Env::Map::iterator cit = env.map.find(cc);
              if (cit != env.map.end()) {
                ees[2].b = cit->second.r;
                if (doubleNeg) {
                  Type t = ees[2].b->_type;
                  ees[2].b = UnOp::a(Location(),UOT_NOT,ees[2].b);
                  ees[2].b->_type = t;
                }
                if (Id* id = ees[2].b->dyn_cast<Id>()) {
                  addCtxAnn(id->_decl,ctx.b);
                }
                ret.r = conj(env,r,ctx,ees);
              } else {
                cc->_decl = env.orig->matchFn(cc->_id.str(),args);
                assert(cc->_decl);
                bool singleExp = true;
                for (unsigned int i=0; i<ees.size(); i++) {
                  if (!istrue(ees[i].b)) {
                    singleExp = false;
                    break;
                  }
                }
                if (singleExp) {
                  if (doubleNeg) {
                    ctx.b = -ctx.b;
                    ctx.neg = !ctx.neg;
                  }
                  ret.r = flat_exp(env,ctx,cc,r,NULL).r;
                } else {
                  Expression* res = flat_exp(env,Ctx(),cc,NULL,NULL).r;
                  ees[2].b = res;
                  if (doubleNeg) {
                    Type t = ees[2].b->_type;
                    ees[2].b = UnOp::a(Location(),UOT_NOT,ees[2].b);
                    ees[2].b->_type = t;
                  }
                  if (Id* id = ees[2].b->dyn_cast<Id>()) {
                    addCtxAnn(id->_decl,ctx.b);
                  }
                  ret.r = conj(env,r,ctx,ees);
                }
                env.map.insert(cc,ret);
              }
            }
            break;

          case BOT_PLUSPLUS:
            {
              std::vector<EE> ee(2);
              EE eev = flat_exp(env,ctx,boe0,NULL,NULL);
              ee[0] = eev;
              ArrayLit* al;
              if (eev.r->isa<ArrayLit>()) {
                al = eev.r->cast<ArrayLit>();
              } else {
                Id* id = eev.r->cast<Id>();
                if (id->_decl==NULL) {
                  throw InternalError("undefined identifier");
                }
                if (id->_decl->_e==NULL) {
                  throw InternalError("array without initialiser not supported");
                }
                al = id->_decl->_e->cast<ArrayLit>();
              }
              ArrayLit* al0 = al;
              eev = flat_exp(env,ctx,boe1,NULL,NULL);
              ee[1] = eev;
              if (eev.r->isa<ArrayLit>()) {
                al = eev.r->cast<ArrayLit>();
              } else {
                Id* id = eev.r->cast<Id>();
                if (id->_decl==NULL) {
                  throw InternalError("undefined identifier");
                }
                if (id->_decl->_e==NULL) {
                  throw InternalError("array without initialiser not supported");
                }
                al = id->_decl->_e->cast<ArrayLit>();
              }
              ArrayLit* al1 = al;
              std::vector<Expression*> v(al0->_v.size()+al1->_v.size());
              for (unsigned int i=al0->_v.size(); i--;)
                v[i] = al0->_v[i];
              for (unsigned int i=al1->_v.size(); i--;)
                v[al0->_v.size()+i] = al1->_v[i];
              ArrayLit* alret = ArrayLit::a(e->_loc,v);
              alret->_type = e->_type;
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
        UnOp* uo = e->cast<UnOp>();
        switch (uo->op()) {
        case UOT_NOT:
          {
            Ctx nctx = ctx;
            nctx.b = -nctx.b;
            nctx.neg = !nctx.neg;
            ret = flat_exp(env,nctx,uo->_e0,r,b);
          }
          break;
        case UOT_PLUS:
          ret = flat_exp(env,ctx,uo->_e0,r,b);
          break;
        case UOT_MINUS:
          {
            BinOp* bo = BinOp::a(Location(),IntLit::a(Location(),0),
                                 BOT_MINUS,uo->_e0);
            bo->_type = uo->_type;
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
        if (c->_decl == NULL) {
          throw InternalError("undeclared function or predicate "
                              +c->_id.str());
        }
        FunctionI* decl = env.orig->matchFn(c);

        Ctx nctx = ctx;
        nctx.neg = false;
        std::string cid = c->_id.str();
        if (decl->_e==NULL && cid == "forall") {
          nctx.b = +nctx.b;
          if (ctx.neg) {
            ctx.neg = false;
            nctx.neg = true;
            cid = "exists";
          }
        } else if (decl->_e==NULL && cid == "exists") {
          nctx.b = +nctx.b;
          if (ctx.neg) {
            ctx.neg = false;
            nctx.neg = true;
            cid = "forall";
          }
        } else if (decl->_e==NULL && cid == "bool2int") {
          if (ctx.neg) {
            ctx.neg = false;
            nctx.neg = true;
            nctx.b = -ctx.i;
          } else {
            nctx.b = ctx.i;
          }
        }

        if (ctx.b==C_ROOT && decl->_e==NULL &&
            cid == "forall" && r==constants().t) {
          /// TODO: need generic array evaluation function
          ret.b = bind(env,ctx,b,constants().lt);
          EE flat_al = flat_exp(env,Ctx(),c->_args[0],NULL,constants().t);
          ArrayLit* al = follow_id(flat_al.r)->cast<ArrayLit>();
          nctx.b = C_ROOT;
          for (unsigned int i=0; i<al->_v.size(); i++)
            (void) flat_exp(env,nctx,al->_v[i],r,b);
        } else {
          std::vector<EE> args_ee(c->_args.size());
          for (unsigned int i=c->_args.size(); i--;) {
            Ctx argctx = nctx;
            if (decl->_e!=NULL ||
                (cid != "forall" && cid != "exists" && cid != "bool2int" &&
                 cid != "sum" && cid != "lin_exp" )) {
              if (c->_args[i]->_type._bt==Type::BT_BOOL) {
                argctx.b = C_MIX;
              } else if (c->_args[i]->_type._bt==Type::BT_INT) {
                argctx.i = C_MIX;
              }
            }
            args_ee[i] = flat_exp(env,argctx,c->_args[i],NULL,NULL);
          }

          std::vector<Expression*> args;
          if (decl->_e==NULL && (cid == "forall" || cid == "exists")) {
            ArrayLit* al = follow_id(args_ee[0].r)->cast<ArrayLit>();
            std::vector<Expression*> alv;
            for (unsigned int i=0; i<al->_v.size(); i++) {
              if (Call* sc = same_call(al->_v[i],cid)) {
                ArrayLit* sc_c = eval_array_lit(sc->_args[0]);
                for (unsigned int j=0; j<sc_c->_v.size(); j++) {
                  alv.push_back(sc_c->_v[j]);
                }
              } else {
                alv.push_back(al->_v[i]);
              }
            }
            if (cid == "exists") {
              std::vector<Expression*> pos_alv;
              std::vector<Expression*> neg_alv;
              unsigned int cur = 0;
              for (unsigned int i=0; i<alv.size(); i++) {
                Call* neg_call = same_call(alv[i],"bool_eq");
                if (neg_call && 
                    Expression::equal(neg_call->_args[1],constants().lf)) {
                  neg_alv.push_back(neg_call->_args[0]);
                } else {
                  Call* clause = same_call(alv[i],"bool_clause");
                  if (clause) {
                    ArrayLit* clause_pos = eval_array_lit(clause->_args[0]);
                    for (unsigned int j=0; j<clause_pos->_v.size(); j++) {
                      pos_alv.push_back(clause_pos->_v[j]);
                    }
                    ArrayLit* clause_neg = eval_array_lit(clause->_args[1]);
                    for (unsigned int j=0; j<clause_neg->_v.size(); j++) {
                      neg_alv.push_back(clause_neg->_v[j]);
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
                  ret.b = bind(env,Ctx(),b,constants().lt);
                  ret.r = bind(env,ctx,r,constants().lf);
                  return ret;
                } else if (pos_alv.size()==1) {
                  ret.b = bind(env,Ctx(),b,constants().lt);
                  ret.r = bind(env,ctx,r,pos_alv[0]);
                  return ret;
                }
                ArrayLit* nal = ArrayLit::a(al->_loc,pos_alv);
                nal->_type = al->_type;
                args.push_back(nal);
              } else {
                ArrayLit* pos_al = ArrayLit::a(al->_loc,pos_alv);
                pos_al->_type = al->_type;
                ArrayLit* neg_al = ArrayLit::a(al->_loc,neg_alv);
                neg_al->_type = al->_type;
                cid = "bool_clause";
                args.push_back(pos_al);
                args.push_back(neg_al);
              }
            } else /* cid=="forall" */ {
              remove_dups(alv,true);
              if (alv.size()==0) {
                ret.b = bind(env,Ctx(),b,constants().lt);
                ret.r = bind(env,ctx,r,constants().lt);
                return ret;
              } else if (alv.size()==1) {
                ret.b = bind(env,Ctx(),b,constants().lt);
                ret.r = bind(env,ctx,r,alv[0]);
                return ret;
              }
              ArrayLit* nal = ArrayLit::a(al->_loc,alv);
              nal->_type = al->_type;
              args.push_back(nal);
            }
          } else if (decl->_e==NULL && (cid == "lin_exp" || cid=="sum")) {

            Expression* al_arg = (cid=="sum" ? c->_args[0] : c->_args[1]);
            EE flat_al = flat_exp(env,nctx,al_arg,NULL,NULL);
            ArrayLit* al = follow_id(flat_al.r)->cast<ArrayLit>();
            IntVal d = (cid == "sum" ? 0 : eval_int(c->_args[2]));
            
            std::vector<IntVal> c_coeff(al->_v.size());
            if (cid=="sum") {
              for (unsigned int i=al->_v.size(); i--;)
                c_coeff[i] = 1;
            } else {
              EE flat_coeff = flat_exp(env,nctx,c->_args[0],NULL,NULL);
              ArrayLit* coeff = follow_id(flat_coeff.r)->cast<ArrayLit>();
              for (unsigned int i=coeff->_v.size(); i--;)
                c_coeff[i] = eval_int(coeff->_v[i]);
            }
            cid = "lin_exp";
            std::vector<IntVal> coeffv;
            std::vector<Expression*> alv;
            for (unsigned int i=0; i<al->_v.size(); i++) {
              if (Call* sc = same_call(al->_v[i],cid)) {
                IntVal cd = c_coeff[i];
                ArrayLit* sc_coeff = eval_array_lit(sc->_args[0]);
                ArrayLit* sc_al = eval_array_lit(sc->_args[1]);
                IntVal sc_d = eval_int(sc->_args[2]);
                assert(sc_coeff->_v.size() == sc_al->_v.size());
                for (unsigned int j=0; j<sc_coeff->_v.size(); j++) {
                  coeffv.push_back(cd*eval_int(sc_coeff->_v[j]));
                  alv.push_back(sc_al->_v[j]);
                }
                d += cd*sc_d;
              } else {
                coeffv.push_back(c_coeff[i]);
                alv.push_back(al->_v[i]);
              }
            }
            simplify_lin(coeffv,alv,d);
            if (coeffv.size()==0) {
              ret.b = conj(env,b,Ctx(),args_ee);
              ret.r = bind(env,ctx,r,IntLit::a(Location(),d));
              break;
            } else if (coeffv.size()==1 && coeffv[0]==1 && d==0) {
              ret.b = conj(env,b,Ctx(),args_ee);
              ret.r = bind(env,ctx,r,alv[0]);
              break;
            }
            std::vector<Expression*> coeff_ev(coeffv.size());
            for (unsigned int i=coeff_ev.size(); i--;)
              coeff_ev[i] = IntLit::a(Location(),coeffv[i]);
            ArrayLit* ncoeff = ArrayLit::a(Location(),coeff_ev);
            ncoeff->_type = Type::parint(1);
            args.push_back(ncoeff);
            ArrayLit* nal = ArrayLit::a(al->_loc,alv);
            nal->_type = al->_type;
            args.push_back(nal);
            IntLit* il = IntLit::a(Location(),d);
            il->_type = Type::parint();
            args.push_back(il);
          } else {
            for (unsigned int i=0; i<args_ee.size(); i++)
              args.push_back(args_ee[i].r);
          }
          Call* cr = Call::a(Location(),cid,args);
          cr->_type = c->_type;
          decl = env.orig->matchFn(cr);
          assert(decl);
          cr->_decl = decl;
          Env::Map::iterator cit = env.map.find(cr);
          if (cit != env.map.end()) {
            ret.b = bind(env,Ctx(),b,cit->second.b);
            ret.r = bind(env,ctx,r,cit->second.r);
          } else {
            if (decl->_e==NULL) {
              /// For now assume that all builtins are total
              if (cit != env.map.end()) {
                ret.b = bind(env,Ctx(),b,cit->second.b);
                ret.r = bind(env,ctx,r,cit->second.r);
              } else {
                if (decl->_builtins.e) {
                  Expression* callres = 
                    decl->_builtins.e(cr->_args);
                  EE res = flat_exp(env,ctx,callres,r,b);
                  args_ee.push_back(res);
                  ret.b = conj(env,b,Ctx(),args_ee);
                  ret.r = bind(env,ctx,r,res.r);
                  env.map.insert(cr,ret);
                } else {
                  ret.b = conj(env,b,Ctx(),args_ee);
                  ret.r = bind(env,ctx,r,cr);
                  env.map.insert(cr,ret);
                }
              }
            } else {
              std::vector<std::pair<Id*,Expression*> > idmap;
              // Save mapping from Ids to VarDecls and set to parameters
              /// TODO: save vd->_e as well (if we want to support recursive functions)
              for (unsigned int i=decl->_params.size(); i--;) {
                VarDecl* vd = decl->_params[i];
                Id* id = Id::a(Location(),vd->_id,NULL);
                id->_type = vd->_type;
                Env::Map::iterator idit = env.map.find(id);
                if (idit==env.map.end()) {
                  EE ee(vd,NULL);
                  Expression* nullexp = NULL;
                  idmap.push_back(std::pair<Id*,Expression*>(id,nullexp));
                  env.map.insert(id,ee);
                } else {
                  idmap.push_back(
                    std::pair<Id*,Expression*>(id,idit->second.r));
                  idit->second.r = vd;
                }
                vd->_e = args[i];
              }
              if (isTotal(decl)) {
                EE ee = flat_exp(env,Ctx(),decl->_e,r,constants().t);
                ret.r = bind(env,ctx,r,ee.r);
                ret.b = conj(env,b,Ctx(),args_ee);
                env.map.insert(cr,ret);
              } else {
                ret = flat_exp(env,ctx,decl->_e,r,NULL);
                args_ee.push_back(ret);
                ret.b = conj(env,b,Ctx(),args_ee);
                env.map.insert(cr,ret);
              }
              // Restore previous mapping
              for (unsigned int i=0; i<idmap.size(); i++) {
                std::pair<Id*,Expression*>& idvd = idmap[i];
                Env::Map::iterator idit = env.map.find(idvd.first);
                assert(idit != env.map.end());
                if (idvd.second==NULL) {
                  env.map.remove(idvd.first);
                } else {
                  idit->second.r = idvd.second;
                }
              }
              for (unsigned int i=decl->_params.size(); i--;) {
                decl->_params[i]->_e = NULL;
              }
            }
          }
        }
      }
      break;
    case Expression::E_VARDECL:
      {
        if (ctx.b != C_ROOT)
          throw FlatteningError(e->_loc, "not in root context");
        VarDecl* v = e->cast<VarDecl>();
        Id* id = Id::a(Location(),v->_id,NULL); /// TODO: avoid allocation
        id->_type = v->_type;
        Env::Map::iterator it = env.map.find(id);
        if (it==env.map.end()) {
          VarDecl* vd = VarDecl::a(Location(),
                                   eval_typeinst(env,v->_ti),
                                   v->_id.str());
          vd->introduced(v->introduced());
          if (v->_ann) {
            vd->annotate(
              static_cast<Annotation*>(
              flat_exp(env,Ctx(),v->_ann,NULL,constants().t).r)
            );
          }
          VarDeclI* nv = VarDeclI::a(Location(),vd);
          Ctx nctx;
          if (v->_e && v->_e->_type.isbool())
            nctx.b = C_MIX;
          if (v->_e) {
            (void) flat_exp(env,nctx,v->_e,vd,constants().t);
            if (v->_e->_type._bt==Type::BT_INT && v->_e->_type._dim==0) {
              IntBounds ib = compute_int_bounds(vd->_e);
              if (ib.valid) {
                IntSetVal* ibv = IntSetVal::a(ib.l,ib.u);
                if (vd->_ti->_domain) {
                  IntSetVal* domain = eval_intset(vd->_ti->_domain);
                  IntSetRanges dr(domain);
                  IntSetRanges ibr(ibv);
                  Ranges::Inter<IntSetRanges,IntSetRanges> i(dr,ibr);
                  ibv = IntSetVal::ai(i);
                }
                vd->_ti->_domain = SetLit::a(Location(),ibv);
              }
            }
          }
          env.m->addItem(nv);
          
          EE ee(vd,NULL);
          env.map.insert(id,ee);
          ret.r = bind(env,Ctx(),r,vd);
        } else {
          ret.r = bind(env,Ctx(),r,it->second.r);
        }
        ret.b = bind(env,Ctx(),b,constants().lt);
      }
      break;
    case Expression::E_LET:
      {
        Let* let = e->cast<Let>();
        GC::mark();
        std::vector<EE> cs;
        std::vector<std::pair<Id*,Expression*> > idmap;
        let->pushbindings();
        for (unsigned int i=0; i<let->_let.size(); i++) {
          Expression* le = let->_let[i];
          if (VarDecl* vd = le->dyn_cast<VarDecl>()) {
            EE ee;
            TypeInst* ti = eval_typeinst(env,vd->_ti);
            VarDecl* nvd = VarDecl::a(Location(),ti,
                                      env.genId("FromLet_"+vd->_id.str()));
            nvd->toplevel(true);
            nvd->introduced(true);
            nvd->_type = vd->_type;
            VarDeclI* nv = VarDeclI::a(Location(),nvd);
            env.m->addItem(nv);
            if (vd->_e) {
              ee = flat_exp(env,ctx,vd->_e,nvd,NULL);
              cs.push_back(ee);
            } else {
              if (ctx.b==C_NEG || ctx.b==C_MIX)
                throw FlatteningError(vd->_loc,
                  "free variable in non-positive context");
            }
            Id* nid = Id::a(Location(),nvd->_id,nvd);
            nid->_type = vd->_type;
            ee = EE(nvd,NULL);
            env.map.insert(nid,ee);
            vd->_e = nid;
            (void) flat_exp(env,Ctx(),nid,NULL,constants().t);
            Id* id = Id::a(Location(),vd->_id,NULL);
            id->_type = vd->_type;
            Env::Map::iterator it = env.map.find(id);
            if (it==env.map.end()) {
              Expression* nullexp = NULL;
              idmap.push_back(std::pair<Id*,Expression*>(id,nullexp));
              env.map.insert(id,ee);
            } else {
              idmap.push_back(std::pair<Id*,Expression*>(id,it->second.r));
              it->second.r = vd;
            }
          } else {
            if (ctx.b==C_ROOT) {
              (void) flat_exp(env,Ctx(),le,constants().t,constants().t);
            } else {
              EE ee = flat_exp(env,ctx,le,NULL,constants().t);
              ee.b = ee.r;
              cs.push_back(ee);
            }
          }
        }
        if (r==constants().t && ctx.b==C_ROOT && !ctx.neg) {
          ret.b = bind(env,Ctx(),b,constants().lt);
          (void) flat_exp(env,ctx,let->_in,r,b);
          ret.r = conj(env,r,Ctx(),cs);
        } else {
          EE ee = flat_exp(env,ctx,let->_in,NULL,NULL);
          if (let->_type.isbool()) {
            ee.b = ee.r;
            cs.push_back(ee);
            ret.r = conj(env,r,ctx,cs);
            ret.b = bind(env,Ctx(),b,constants().lt);
          } else {
            cs.push_back(ee);
            ret.r = bind(env,Ctx(),r,ee.r);
            ret.b = conj(env,b,Ctx(),cs);
          }
          let->popbindings();
          // Restore previous mapping
          for (unsigned int i=0; i<idmap.size(); i++) {
            std::pair<Id*,Expression*>& idvd = idmap[i];
            Env::Map::iterator idit = env.map.find(idvd.first);
            assert(idit != env.map.end());
            if (idvd.second==NULL) {
              env.map.remove(idvd.first);
            } else {
              idit->second.r = idvd.second;
            }
          }
        }
      }
      break;
    case Expression::E_ANN:
      {
        Annotation* ann = e->cast<Annotation>();
        EE ee = flat_exp(env,Ctx(),ann->_e,NULL,constants().t);
        EE ea = flat_exp(env,Ctx(),ann->_a,NULL,constants().t);
        ret.r = Annotation::a(Location(),ee.r,
                              static_cast<Annotation*>(ea.r));
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


  Model* flatten(Model* m) {
    Model* flat = new Model;
    Env env(m,flat);
    
    class FV : public ItemVisitor {
    public:
      Env& env;
      FV(Env& env0) : env(env0) {}
      void vVarDeclI(VarDeclI* v) {
        if (v->_e->_type.isvar()) {
          GCLock lock;
          (void) flat_exp(env,Ctx(),v->_e,NULL,constants().t);
        }
      }
      void vConstraintI(ConstraintI* ci) {
        GCLock lock;
        (void) flat_exp(env,Ctx(),ci->_e,constants().t,constants().t);
      }
      void vSolveI(SolveI* si) {
        GCLock lock;
        Annotation* ann = static_cast<Annotation*>(
          flat_exp(env,Ctx(),si->_ann,NULL,constants().t).r);
        switch (si->st()) {
        case SolveI::ST_SAT:
          env.m->addItem(SolveI::sat(Location(),ann));
          break;
        case SolveI::ST_MIN:
          env.m->addItem(SolveI::min(Location(),
            flat_exp(env,Ctx(),si->_e,NULL,constants().t).r,
            ann));
          break;
        case SolveI::ST_MAX:
          env.m->addItem(SolveI::max(Location(),
            flat_exp(env,Ctx(),si->_e,NULL,constants().t).r,
            ann));
          break;
        }
      }
    } _fv(env);
    iterItems<FV>(_fv,m);
    
    return flat;
  }

  void oldflatzinc(Model* m) {
    struct {
    public:
      bool operator() (const Item* i) {
        return i->isa<VarDeclI>() &&
          (i->cast<VarDeclI>()->_e->_type._ot == Type::OT_OPTIONAL ||
           i->cast<VarDeclI>()->_e->_type._bt == Type::BT_ANN);
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
        VarDecl* vd = v->_e;
        if (vd->_type.isvar() && vd->_type.isbool()) {
          if (Expression::equal(vd->_ti->_domain,constants().lt)) {
            Expression* ve = vd->_e;
            vd->_e = constants().lt;
            vd->_ti->_domain = NULL;
            if (ve != NULL) {
              if (Call* vc = ve->dyn_cast<Call>()) {
                if (vc->_id == "exists") {
                  vc->_id = ASTString("array_bool_or");
                  std::vector<Expression*> args(2);
                  args[0] = vc->_args[0];
                  args[1] = constants().lt;
                  ASTExprVec<Expression> argsv(args);
                  vc->_args = argsv;
                } else if (vc->_id == "forall") {
                  vc->_id = ASTString("array_bool_and");
                  std::vector<Expression*> args(2);
                  args[0] = vc->_args[0];
                  args[1] = constants().lt;
                  ASTExprVec<Expression> argsv(args);
                  vc->_args = argsv;
                }
              }
              tmp._items.push_back(ConstraintI::a(Location(),ve));
            }
          } else {
            if (vd->_e != NULL) {
              if (vd->_e->eid()==Expression::E_CALL) {
                Call* c = vd->_e->cast<Call>();
                vd->_e = NULL;
                if (c->_id == "exists") {
                  c->_id = ASTString("array_bool_or");
                } else if (c->_id == "forall") {
                  c->_id = ASTString("array_bool_and");
                } else {
                  c->_id = ASTString(c->_id.str()+"_reif");
                }
                std::vector<Expression*> args(c->_args.size());
                std::copy(c->_args.begin(),c->_args.end(),args.begin());
                args.push_back(Id::a(Location(),vd->_id,vd));
                c->_args = ASTExprVec<Expression>(args);
                tmp._items.push_back(ConstraintI::a(Location(),c));
              } else {
                assert(vd->_e->eid() == Expression::E_ID ||
                       vd->_e->eid() == Expression::E_BOOLLIT);
              }
            }
          }
        } else if (vd->_type.isvar() && vd->_type._dim==0) {
          if (vd->_e != NULL) {
            if (vd->_e->eid()==Expression::E_CALL) {
              Call* c = vd->_e->cast<Call>();
              vd->_e = NULL;
              std::vector<Expression*> args(c->_args.size());
              if (c->_id == "lin_exp") {
                c->_id = ASTString("int_lin_eq");
                ArrayLit* le_c = c->_args[0]->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->_v.size());
                std::copy(le_c->_v.begin(),le_c->_v.end(),nc.begin());
                nc.push_back(IntLit::a(Location(),-1));
                c->_args[0] = ArrayLit::a(Location(),nc);
                ArrayLit* le_x = c->_args[1]->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->_v.size());
                std::copy(le_x->_v.begin(),le_x->_v.end(),nx.begin());
                nx.push_back(Id::a(Location(),vd->_id,vd));
                c->_args[1] = ArrayLit::a(Location(),nx);
                IntVal d = c->_args[2]->cast<IntLit>()->_v;
                c->_args[2] = IntLit::a(Location(),-d);
              } else {
                args.push_back(Id::a(Location(),vd->_id,vd));
              }
              std::copy(c->_args.begin(),c->_args.end(),args.begin());
              c->_args = ASTExprVec<Expression>(args);
              tmp._items.push_back(ConstraintI::a(Location(),c));
            } else {
              assert(vd->_e->eid() == Expression::E_ID ||
                     vd->_e->eid() == Expression::E_INTLIT ||
                     vd->_e->eid() == Expression::E_BOOLLIT ||
                     vd->_e->eid() == Expression::E_SETLIT);
              
            }
          }
        } else if (vd->_type._dim==1 &&
                   vd->_ti->_ranges.size()==1) {
          if (vd->_ti->_ranges[0]->_domain==NULL) {
            assert(vd->_e != NULL);
            ArrayLit* al = NULL;
            Expression* e = vd->_e;
            while (al==NULL) {
              switch (e->eid()) {
              case Expression::E_ARRAYLIT:
                al = e->cast<ArrayLit>();
                break;
              case Expression::E_ID:
                e = e->cast<Id>()->_decl->_e;
                break;
              default:
                assert(false);
              }
            }
            IntSetVal* isv = IntSetVal::a(al->min(0),al->max(0));
            vd->_ti->_ranges[0]->_domain = SetLit::a(Location(),isv);
          } else {
            IntSetVal* isv = eval_intset(vd->_ti->_ranges[0]->_domain);
            if (isv->min(0)!=1) {
              vd->_ti->_ranges[0]->_domain = SetLit::a(Location(),
                IntSetVal::a(1,isv->max(0)-isv->min(0)+1));
              if (vd->_e != NULL && vd->_e->eid()==Expression::E_ARRAYLIT) {
                std::vector<int> dims(2);
                dims[0] = 1;
                dims[1] = isv->max(0)-isv->min(0)+1;
                ArrayLit* al = vd->_e->cast<ArrayLit>();
                al->_dims = ASTIntVec(dims);
              }
            }
          }
        }
      }
      void vConstraintI(ConstraintI* ci) {
        if (Call* vc = ci->_e->dyn_cast<Call>()) {
          if (vc->_id == "exists") {
            GCLock lock;
            vc->_id = ASTString("array_bool_or");
            std::vector<Expression*> args(2);
            args[0] = vc->_args[0];
            args[1] = constants().lt;
            ASTExprVec<Expression> argsv(args);
            vc->_args = argsv;
          } else if (vc->_id == "forall") {
            GCLock lock;
            vc->_id = ASTString("array_bool_and");
            std::vector<Expression*> args(2);
            args[0] = vc->_args[0];
            args[1] = constants().lt;
            ASTExprVec<Expression> argsv(args);
            vc->_args = argsv;
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
          if (i->cast<VarDeclI>()->_e->_type._dim == 0 &&
              j->cast<VarDeclI>()->_e->_type._dim != 0)
            return true;
          if (i->cast<VarDeclI>()->_e->_e==NULL &&
              j->cast<VarDeclI>()->_e->_e != NULL)
            return true;
        }
        if (j->iid()==Item::II_VD) {
          if (j->iid() != i->iid())
            return false;
          if (j->cast<VarDeclI>()->_e->_type._dim == 0 &&
              i->cast<VarDeclI>()->_e->_type._dim != 0)
            return false;
          if (j->cast<VarDeclI>()->_e->_e==NULL &&
              i->cast<VarDeclI>()->_e->_e != NULL)
            return false;
        }
        return i<j;
      }
    } _cmp;
    std::stable_sort(m->_items.begin(),m->_items.end(),_cmp);

  }
  
}
