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

  /// Evaluation context
  enum BCtx { C_ROOT, C_POS, C_NEG, C_MIX };

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

  Annotation* ctx_ann(BCtx& c) {
    std::string ctx;
    switch (c) {
    case C_ROOT: ctx = "ctx_root"; break;
    case C_POS: ctx = "ctx_pos"; break;
    case C_NEG: ctx = "ctx_neg"; break;
    case C_MIX: ctx = "ctx_mix"; break;
    default: assert(false); break;
    }
    return Annotation::a(Location(),Id::a(Location(),ctx,NULL));
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

  class Constants {
  public:
    Model* m;
    BoolLit* lt;
    VarDecl* t;
    BoolLit* lf;
    VarDecl* f;
    Constants(void) {
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
  } constants;

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
      if (it->first->eid() == Expression::E_ID)
        r.push_back(it->first);
      // if (x.second.r)
      //   r.push_back(x.second.r);
      // if (x.second.b)
      //   r.push_back(x.second.b);
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

  Expression* bind(Env& env, VarDecl* vd, Expression* e) {
    if (vd==constants.t) {
      if (!istrue(e)) {
        if (Id* id = e->dyn_cast<Id>()) {
          assert(id->_decl != NULL);
          id->_decl->_ti->_domain = constants.lt;
        } else {
          env.m->addItem(ConstraintI::a(Location(),e));
        }
      }
      return constants.lt;
    } else if (vd==constants.f) {
      if (!isfalse(e)) {
        throw InternalError("not supported yet");
      }
      return constants.lt;
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
          vd->_e = constants.lt;
        } else if (e->_type.ispar() && e->_type.isbool()) {
          if (eval_bool(e)) {
            vd->_e = constants.lt;
          } else {
            vd->_e = constants.lf;
          }
        } else {
          vd->_e = e;
        }
        return e;
      } else if (vd->_e != e) {
        throw InternalError("not supported yet");
      } else {
        return e;
      }
    }
  }

  EE flat_exp(Env& env, BCtx bctx, Expression* e, VarDecl* r, VarDecl* b);

  Expression* conj(Env& env,VarDecl* b, const std::vector<EE>& e) {
    std::vector<Expression*> nontrue;
    for (unsigned int i=0; i<e.size(); i++) {
      if (istrue(e[i].b))
        continue;
      if (isfalse(e[i].b)) {
        return bind(env,b,constants.lf);
      }
      nontrue.push_back(e[i].b);
    }
    if (nontrue.empty()) {
      return bind(env,b,constants.lt);
    } else if (nontrue.size()==1) {
      return bind(env,b,nontrue[0]);
    } else {
      if (b==constants.t) {
        for (unsigned int i=0; i<nontrue.size(); i++)
          bind(env,b,nontrue[i]);
        return constants.lt;
      } else {
        BinOp* ret = BinOp::a(Location(),
                              nontrue[0],BOT_AND,nontrue[1]);
        ret->_type = Type::varbool();
        for (unsigned int i=2; i<nontrue.size(); i++) {
          ret = BinOp::a(Location(),ret,BOT_AND,nontrue[i]);
          ret->_type = Type::varbool();
        }
        EE rete = flat_exp(env,C_ROOT,ret,NULL,constants.t);
        return bind(env,b,rete.r);
      }
    }
  }

  TypeInst* eval_typeinst(Env& env, TypeInst* ti) {
    /// TODO: evaluate all par components in the domain. This probably
    ///       needs the VarDecl to compute the actual dimensions of
    ///       array[int] expressions
    return eval_par(ti)->cast<TypeInst>();
  }

  std::string opToBuiltin(BinOp* op) {
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
    switch (op->op()) {
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

  EE flat_exp(Env& env, BCtx bctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
    EE ret;
    if (e->_type.ispar() && e->_type._bt!=Type::BT_ANN) {
      ret.b = bind(env,b,constants.lt);
      ret.r = bind(env,r,eval_par(e));
      return ret;
    }
    switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_SETLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
      ret.b = bind(env,b,constants.lt);
      ret.r = bind(env,r,e);
      return ret;
    case Expression::E_ID:
      {
        Id* id = e->cast<Id>();
        if (id->_decl==NULL)
          throw FlatteningError(e->_loc, "undefined identifier");
        Env::Map::iterator it = env.map.find(id);
        VarDecl* vd = NULL;
        Expression* rete = NULL;
        if (it==env.map.end()) {
          // New top-level id, need to copy into env.m
          vd = flat_exp(env,C_ROOT,id->_decl,NULL,constants.t).r
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
        ret.b = bind(env,b,constants.lt);
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
            (void) flat_exp(env,C_ROOT,nvd,NULL,constants.t);
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
        ret.r = bind(env,r,rete);
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
          elems_ee[i] = flat_exp(env,bctx,al->_v[i],NULL,NULL);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r;
        std::vector<std::pair<int,int> > dims(al->dims());
        for (unsigned int i=al->dims(); i--;)
          dims[i] = std::pair<int,int>(al->min(i), al->max(i));
        ArrayLit* alr = ArrayLit::a(Location(),elems,dims);
        alr->_type = al->_type;
        ret.b = conj(env,b,elems_ee);
        ret.r = bind(env,r,alr);
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
          EE eev = flat_exp(env,+bctx,aa->_v,NULL,NULL);
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
          ret.b = bind(env,b,constants.lt);
          ret.r = bind(env,r,val);
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
          ret = flat_exp(env,bctx,cc,r,b);
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
          BCtx bctx;
          EvalF(Env& env0, BCtx bctx0) : env(env0), bctx(bctx0) {}
          typedef EE ArrayVal;
          EE e(Expression* e) {
            return flat_exp(env,bctx,e,NULL,NULL);
          }
        } _evalf(env,bctx);
        std::vector<EE> elems_ee = eval_comp<EvalF>(_evalf,c);
        std::vector<Expression*> elems(elems_ee.size());
        for (unsigned int i=elems.size(); i--;)
          elems[i] = elems_ee[i].r;
        ArrayLit* alr = ArrayLit::a(Location(),elems);
        alr->_type = c->_type;
        ret.b = conj(env,b,elems_ee);
        ret.r = bind(env,r,alr);
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        bool done = false;
        for (int i=0; i<ite->_e_if_then.size(); i+=2) {
          if (eval_bool(ite->_e_if_then[i])) {
            ret = flat_exp(env,bctx,ite->_e_if_then[i+1],r,b);
            done = true;
            break;
          }
        }
        if (!done) {
          ret = flat_exp(env,bctx,ite->_e_else,r,b);
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
          ret = flat_exp(env,bctx,cr,r,b);
        } else {
          BCtx bctx0 = bctx;
          BCtx bctx1 = bctx;
          switch (bo->op()) {
          case BOT_PLUS:
          case BOT_MINUS:
          case BOT_MULT:
          case BOT_IDIV:
          case BOT_MOD:
          case BOT_DIV:
          case BOT_UNION:
          case BOT_DIFF:
          case BOT_SYMDIFF:
          case BOT_INTERSECT:
            {
              EE e0 = flat_exp(env,bctx0,bo->_e0,NULL,NULL);
              EE e1 = flat_exp(env,bctx1,bo->_e1,NULL,NULL);

              std::vector<Expression*> args(2);
              args[0] = e0.r; args[1] = e1.r;
              Call* cc = Call::a(Location(),opToBuiltin(bo),args);
              cc->_type = bo->_type;

              if (FunctionI* fi = env.orig->matchFn(cc->_id,args)) {
                assert(cc->_type == fi->rtype(args));
                cc->_decl = fi;
                ret = flat_exp(env,bctx,cc,r,b);
              } else {
                ret.r = bind(env,r,cc);
                std::vector<EE> ees(2);
                ees[0].b = e0.b; ees[1].b = e1.b;
                ret.b = conj(env,b,ees);
              }
            }
            break;

          case BOT_AND:
            {
              if (r==constants.t) {
                (void) flat_exp(env,C_ROOT,bo->_e0,constants.t,constants.t);
                (void) flat_exp(env,C_ROOT,bo->_e1,constants.t,constants.t);
                break;
              }
              // else fall through
            }
          case BOT_EQUIV:
          case BOT_IMPL:
          case BOT_RIMPL:
          case BOT_OR:
          case BOT_XOR:
          case BOT_LE:
          case BOT_LQ:
          case BOT_GR:
          case BOT_GQ:
          case BOT_EQ:
          case BOT_NQ:
          case BOT_IN:
          case BOT_SUBSET:
          case BOT_SUPERSET:
            {
              switch (bo->op()) {
              case BOT_XOR:
              case BOT_EQUIV:
                bctx0 = bctx1 = C_MIX;
                break;
              case BOT_IMPL:
                bctx0 = -bctx0;
                bctx1 = +bctx1;
                break;
              case BOT_RIMPL:
                bctx0 = +bctx0;
                bctx1 = -bctx1;
                break;
              case BOT_OR:
                bctx0 = +bctx0;
                bctx1 = +bctx1;
                break;
              default:
                break;
              }
              EE e0 = flat_exp(env,bctx0,bo->_e0,NULL,NULL);
              EE e1 = flat_exp(env,bctx1,bo->_e1,NULL,NULL);
              ret.b = bind(env,b,constants.lt);

              std::vector<Expression*> args(2);
              args[0] = e0.r; args[1] = e1.r;
              Call* cc = Call::a(Location(),opToBuiltin(bo),args);
              cc->_type = bo->_type;

              std::vector<EE> ees(3);
              ees[0].b = e0.b; ees[1].b = e1.b;
              Env::Map::iterator cit = env.map.find(cc);
              if (cit != env.map.end()) {
                ees[2].b = cit->second.r;
                if (Id* id = ees[2].b->dyn_cast<Id>()) {
                  if (id->_decl)
                    id->_decl->annotate(ctx_ann(bctx));
                }
                ret.r = conj(env,r,ees);
              } else {
                cc->_decl = env.orig->matchFn(cc->_id.str(),args);
                ees[2].b = flat_exp(env,C_ROOT,cc,NULL,NULL).r;
                if (Id* id = ees[2].b->dyn_cast<Id>()) {
                  if (id->_decl)
                    id->_decl->annotate(ctx_ann(bctx));
                }
                ret.r = conj(env,r,ees);
                env.map.insert(cc,ret);
              }
            }
            break;

          case BOT_PLUSPLUS:
            {
              std::vector<EE> ee(2);
              EE eev = flat_exp(env,bctx,bo->_e0,NULL,NULL);
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
              eev = flat_exp(env,bctx,bo->_e1,NULL,NULL);
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
              ret.b = conj(env,b,ee);
              ret.r = bind(env,r,alret);
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
            EE ee = flat_exp(env,-bctx,uo->_e0,NULL,NULL);
            BinOp* bo = BinOp::a(Location(),ee.r,BOT_EQUIV,constants.lf);
            bo->_type = uo->_type;
            return flat_exp(env,bctx,bo,r,b);
          }
        case UOT_PLUS:
          return flat_exp(env,bctx,uo->_e0,r,b);
        case UOT_MINUS:
          {
            BinOp* bo = BinOp::a(Location(),IntLit::a(Location(),0),
                                 BOT_MINUS,uo->_e0);
            bo->_type = uo->_type;
            return flat_exp(env,bctx,bo,r,b);
          }
        default: break;
        }
      }
      break;
    case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        if (c->_decl == NULL) {
          ret.r = bind(env,r,e);
          ret.b = bind(env,b,constants.lt);
          break;
        }
        FunctionI* decl = env.orig->matchFn(c);
        if (c->_id == "forall" && decl==NULL)
          throw InternalError("forall not defined");

        if (decl->_e==NULL && c->_id == "forall" && r==constants.t) {
          /// TODO: need generic array evaluation function
          ret.b = bind(env,b,constants.lt);
          Expression* ca = c->_args[0];
          switch (ca->eid()) {
          case Expression::E_ARRAYLIT:
            {
              ArrayLit* al = ca->cast<ArrayLit>();
              for (unsigned int i=0; i<al->_v.size(); i++)
                (void) flat_exp(env,C_ROOT,al->_v[i],r,b);
            }
            break;
          case Expression::E_COMP:
            {
              class EvalFlat {
              public:
                Env& env;
                EvalFlat(Env& env0) : env(env0) {}
                typedef bool ArrayVal;
                bool e(Expression* e) {
                  (void) flat_exp(env,C_ROOT,e,constants.t,constants.t);
                  return true;
                }
              } _evalf(env);
              (void) eval_comp<EvalFlat>(_evalf,
                                         ca->cast<Comprehension>());
            }
            break;
          default:
            throw InternalError("not supported yet");
            break;
          }
        } else {

          std::vector<EE> args_ee(c->_args.size());
          for (unsigned int i=c->_args.size(); i--;)
            args_ee[i] = flat_exp(env,bctx,c->_args[i],NULL,NULL);
          std::vector<Expression*> args(args_ee.size());
          for (unsigned int i=args_ee.size(); i--;)
            args[i] = args_ee[i].r;
          Call* cr = Call::a(Location(),c->_id.str(),args);
          cr->_type = c->_type;
          Env::Map::iterator cit = env.map.find(cr);
          if (cit != env.map.end()) {
            ret.b = bind(env,b,cit->second.b);
            ret.r = bind(env,r,cit->second.r);
          } else {
            if (decl->_e==NULL) {
              /// For now assume that all builtins are total
              if (cit != env.map.end()) {
                ret.b = bind(env,b,cit->second.b);
                ret.r = bind(env,r,cit->second.r);
              } else {
                if (decl->_builtins.e) {
                  Expression* callres = 
                    decl->_builtins.e(cr->_args);
                  EE res = flat_exp(env,bctx,callres,r,b);
                  args_ee.push_back(res);
                  ret.b = conj(env,b,args_ee);
                  ret.r = bind(env,r,res.r);
                  env.map.insert(cr,ret);
                } else {
                  ret.b = conj(env,b,args_ee);
                  ret.r = bind(env,r,cr);
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
                EE ee = flat_exp(env,C_ROOT,decl->_e,r,constants.t);
                ret.r = bind(env,r,ee.r);
                ret.b = conj(env,b,args_ee);
                env.map.insert(cr,ret);
              } else {
                ret = flat_exp(env,bctx,decl->_e,r,NULL);
                args_ee.push_back(ret);
                ret.b = conj(env,b,args_ee);
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
        if (bctx != C_ROOT)
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
              flat_exp(env,C_ROOT,v->_ann,NULL,constants.t).r)
            );
          }
          VarDeclI* nv = VarDeclI::a(Location(),vd);
          if (v->_e) {
            (void) flat_exp(env,
              v->_e->_type.isbool() ? C_MIX : C_ROOT,v->_e,vd,constants.t);
          }
          env.m->addItem(nv);
          
          EE ee(vd,NULL);
          env.map.insert(id,ee);
          ret.r = bind(env,r,vd);
        } else {
          ret.r = bind(env,r,it->second.r);
        }
        ret.b = bind(env,b,constants.lt);
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
            if (!vd->_e) {
              if (bctx==C_NEG || bctx==C_MIX)
                throw FlatteningError(vd->_loc,
                  "free variable in non-positive context");
              TypeInst* ti = eval_typeinst(env,vd->_ti);
              VarDecl* nvd = VarDecl::a(Location(),ti,env.genId("FromLet_"+vd->_id.str()));
              nvd->toplevel(true);
              nvd->introduced(true);
              nvd->_type = vd->_type;
              VarDeclI* nv = VarDeclI::a(Location(),nvd);
              env.m->addItem(nv);
              Id* id = Id::a(Location(),nvd->_id,nvd);
              id->_type = vd->_type;
              ee = EE(nvd,NULL);
              env.map.insert(id,ee);
              vd->_e = id;
              (void) flat_exp(env,C_ROOT,id,NULL,constants.t);
            } else {
              ee = flat_exp(env,bctx,vd->_e,NULL,NULL);
              cs.push_back(ee);
              vd->_e = ee.r;
            }
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
            if (bctx==C_ROOT) {
              (void) flat_exp(env,C_ROOT,le,constants.t,constants.t);
            } else {
              EE ee = flat_exp(env,bctx,le,NULL,constants.t);
              ee.b = ee.r;
              cs.push_back(ee);
            }
          }
        }
        EE ee = flat_exp(env,bctx,let->_in,NULL,NULL);
        if (let->_type.isbool()) {
          ee.b = ee.r;
          cs.push_back(ee);
          ret.r = conj(env,r,cs);
          ret.b = bind(env,b,constants.lt);
        } else {
          cs.push_back(ee);
          ret.r = bind(env,r,ee.r);
          ret.b = conj(env,b,cs);
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
      break;
    case Expression::E_ANN:
      {
        Annotation* ann = e->cast<Annotation>();
        EE ee = flat_exp(env,C_ROOT,ann->_e,NULL,constants.t);
        EE ea = flat_exp(env,C_ROOT,ann->_a,NULL,constants.t);
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
          (void) flat_exp(env,C_ROOT,v->_e,NULL,constants.t);
        }
      }
      void vConstraintI(ConstraintI* ci) {
        GCLock lock;
        // std::cerr << "+++++++++++++++++++++++++++++++++++\n";
        // debugprint(ci);
        (void) flat_exp(env,C_ROOT,ci->_e,constants.t,constants.t);
        // std::cerr << "+++++++++++++++++++++++++++++++++++\n";
      }
      void vSolveI(SolveI* si) {
        GCLock lock;
        Annotation* ann = static_cast<Annotation*>(
          flat_exp(env,C_ROOT,si->_ann,NULL,constants.t).r);
        switch (si->st()) {
        case SolveI::ST_SAT:
          env.m->addItem(SolveI::sat(Location(),ann));
          break;
        case SolveI::ST_MIN:
          env.m->addItem(SolveI::min(Location(),
            flat_exp(env,C_ROOT,si->_e,NULL,constants.t).r,
            ann));
          break;
        case SolveI::ST_MAX:
          env.m->addItem(SolveI::max(Location(),
            flat_exp(env,C_ROOT,si->_e,NULL,constants.t).r,
            ann));
          break;
        }
      }
    } _fv(env);
    iterItems<FV>(_fv,m);
    
    return flat;
  }

  void oldflatzinc(Model* m) {
    GCLock lock;

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
        
    std::vector<ConstraintI*> cs;
    class FV : public ItemVisitor {
    public:
      std::vector<ConstraintI*>& cs;
      FV(std::vector<ConstraintI*>& cs0)
        : cs(cs0) {}
      void vVarDeclI(VarDeclI* v) {
        VarDecl* vd = v->_e;
        if (vd->_type.isvar() && vd->_type.isbool()) {
          if (Expression::equal(vd->_ti->_domain,constants.lt)) {
            Expression* ve = vd->_e;
            vd->_e = constants.lt;
            vd->_ti->_domain = NULL;
            if (ve != NULL) {
              if (Call* vc = ve->dyn_cast<Call>()) {
                if ( (vc->_id == "bool_and" || vc->_id == "bool_or")
                     && vc->_args.size()==2) {
                  std::vector<Expression*> args(3);
                  args[0] = vc->_args[0];
                  args[1] = vc->_args[1];
                  args[2] = constants.lt;
                  ASTExprVec<Expression> argsv(args);
                  vc->_args = argsv;
                }
              }
              cs.push_back(ConstraintI::a(Location(),ve));
            }
          } else {
            if (vd->_e != NULL) {
              if (vd->_e->eid()==Expression::E_CALL) {
                Call* c = vd->_e->cast<Call>();
                vd->_e = NULL;
                if (c->_id=="bool_and" || c->_id=="bool_or") {
                  
                } else {
                  c->_id = ASTString(c->_id.str()+"_reif");
                }
                std::vector<Expression*> args(c->_args.size());
                std::copy(c->_args.begin(),c->_args.end(),args.begin());
                args.push_back(Id::a(Location(),vd->_id,vd));
                c->_args = ASTExprVec<Expression>(args);
                cs.push_back(ConstraintI::a(Location(),c));
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
              std::copy(c->_args.begin(),c->_args.end(),args.begin());
              args.push_back(Id::a(Location(),vd->_id,vd));
              c->_args = ASTExprVec<Expression>(args);
              cs.push_back(ConstraintI::a(Location(),c));
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
      }
    } _fv(cs);
    iterItems<FV>(_fv,m);
    for (unsigned int i=0; i<cs.size(); i++)
      m->addItem(cs[i]);
    
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
