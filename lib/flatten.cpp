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

#include <minizinc/stl_map_set.hh>

#include <minizinc/flatten_internal.hh>

// temporary
#include <minizinc/prettyprinter.hh>

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
      std::cerr << *ee[i].b();
  }
  void dumpEEr(const std::vector<EE>& ee) {
    for (unsigned int i=0; i<ee.size(); i++)
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

  bool isDefinesVarAnn(Expression* e) {
    return e->isa<Call>() && e->cast<Call>()->id()==constants().ann.defines_var;
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

#define MZN_FILL_REIFY_MAP(T,ID) reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.T.ID,constants().ids.T ## reif.ID));

  EnvI::EnvI(Model* orig0) : orig(orig0), output(new Model), ignorePartial(false), _flat(new Model), ids(0) {
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
          if (idx >= 0 && (*_flat)[idx]->removed())
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
      static std::string d(const WW& ee) {
        std::ostringstream oss;
        oss << ee.r() << " " << ee.b();
        return oss.str();
      }
    };
    map.dump<EED>();
  }
  
  void EnvI::flat_addItem(Item* i) {
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
          addWarning("model inconsistency detected");
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
      case Item::II_OUT:
      {
        OutputI* si = i->cast<OutputI>();
        CollectOccurrencesE ce(vo,si);
        topDown(ce,si->e());
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
          if (i==callStack.size()-1 || !isDefinesVarAnn(ee_ann.r()))
            toAnnotate->addAnnotation(ee_ann.r());
        }
      }
    }
  }
  void EnvI::vo_add_exp(VarDecl* vd) {
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
    CollectOccurrencesE ce(vo,(*_flat)[idx]);
    topDown(ce, vd->e());
  }
  Model* EnvI::flat(void) {
    return _flat;
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
    std::ostringstream oss;
    dumpStack(oss, false);
    warnings.push_back(msg+"\n"+oss.str());
  }
  
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
  
  FlatteningError::FlatteningError(EnvI& env, const Location& loc, const std::string& msg)
  : LocationException(loc,msg) {
    env.errorStack.clear();
  }

  
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
    return e->dumpStack(os, true);
  }
  std::ostream&
  EnvI::dumpStack(std::ostream& os, bool errStack) {
    int lastError = 0;
    
    std::vector<const Expression*>& stack = errStack ? errorStack : callStack;
    
    for (; lastError < stack.size(); lastError++) {
      if (stack[lastError]->isa<Id>()) {
        break;
      }
    }

    ASTString curloc_f;
    int curloc_l = -1;
    
    for (int i=lastError-1; i>=0; i--) {
      ASTString newloc_f = stack[i]->loc().filename;
      int newloc_l = stack[i]->loc().first_line;
      if (newloc_f != curloc_f || newloc_l != curloc_l) {
        os << "  " << newloc_f << ":" << newloc_l << ":" << std::endl;
        curloc_f = newloc_f;
        curloc_l = newloc_l;
      }
      os << "  in ";
      switch (stack[i]->eid()) {
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
          os << "identifier" << std::endl;
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
          const Comprehension* cmp = stack[i]->cast<Comprehension>();
          if (cmp->set())
            os << "set ";
          else
            os << "array ";
          os << "comprehension expression with" << std::endl;
          for (unsigned int i=0; i<cmp->n_generators(); i++) {
            for (unsigned int j=0; j<cmp->n_decls(i); j++) {
              os << "    " << cmp->decl(i, j)->id()->str() << " = " << eval_int(cmp->decl(i, j)->e()) << std::endl;
            }
          }
        }
          break;
        case Expression::E_ITE:
          os << "if-then-else expression" << std::endl;
          break;
        case Expression::E_BINOP:
          os << "binary '" << stack[i]->cast<BinOp>()->opToString() << "' operator expression" << std::endl;
          break;
        case Expression::E_UNOP:
          os << "unary '" << stack[i]->cast<UnOp>()->opToString() << "' operator expression" << std::endl;
          break;
        case Expression::E_CALL:
          os << "call '" << stack[i]->cast<Call>()->id() << "'" << std::endl;
          break;
        case Expression::E_VARDECL:
        {
          GCLock lock;
          os << "variable declaration for '" << stack[i]->cast<VarDecl>()->id()->str() << "'" << std::endl;
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

  const std::vector<std::string>& Env::warnings(void) {
    return envi().warnings;
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
      if (e->eid()==Expression::E_ID && e != constants().absent) {
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
      if (e==constants().absent)
        return e;
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
      assert(e->type().bt() == Type::BT_BOOL);
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
            VarDecl* vd = new VarDecl(e->loc(),ti,env.genId(),e);
            vd->introduced(true);
            vd->flat(vd);

            if (vd->e()->type().bt()==Type::BT_INT && vd->e()->type().dim()==0) {
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
                  if (id->type().st()==Type::ST_PLAIN && ibv->size()==0) {
                    env.addWarning("model inconsistency detected");
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
                cid = constants().ids.int_.eq;
              } else if (e->type().isbool()) {
                cid = constants().ids.bool_eq;
              } else if (e->type().isset()) {
                cid = constants().ids.set_eq;
              } else if (e->type().isfloat()) {
                cid = constants().ids.float_.eq;
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
              ret = vd->id();
            }
            if (vd->e() && vd->e()->type().bt()==Type::BT_INT && vd->e()->type().dim()==0) {
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
          if (vd == e)
            return vd->id();
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
              } else if (e->type().isset()) {
                cid = constants().ids.set_eq;
              } else if (e->type().isfloat()) {
                cid = constants().ids.float_.eq;
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
                c->id(constants().ids.int_.lin_eq);
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
                  c->id(constants().ids.array_bool_or);
                } else if (c->id() == constants().ids.forall) {
                  c->id(constants().ids.array_bool_and);
                } else if (vd->type().isbool()) {
                  c->id(env.reifyId(c->id()));
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
      Type t = vd->e() ? vd->e()->type() : vd->ti()->type();
      return new TypeInst(vd->ti()->loc(), t, dims, eval_par(vd->ti()->domain()));
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
    } else if (op->rhs()->type().isset()) {
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
      if (x[i]()->isa<Id>() && x[j]()->isa<Id>() &&
          x[i]()->cast<Id>()->idn() != -1 &&
          x[j]()->cast<Id>()->idn() != -1)
        return x[i]()->cast<Id>()->idn() < x[j]()->cast<Id>()->idn();
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
          callid = constants().ids.int_.lin_le;
          coeff_sign = 1;
          d += 1;
          break;
        case BOT_LQ:
          callid = constants().ids.int_.lin_le;
          coeff_sign = 1;
          break;
        case BOT_GR:
          callid = constants().ids.int_.lin_le;
          coeff_sign = -1;
          d = -d+1;
          break;
        case BOT_GQ:
          callid = constants().ids.int_.lin_le;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_EQ:
          callid = constants().ids.int_.lin_eq;
          coeff_sign = 1;
          break;
        case BOT_NQ:
          callid = constants().ids.int_.lin_ne;
          coeff_sign = 1;
          break;
        default: assert(false); break;
      }
    }
    static ASTString id_eq(void) { return constants().ids.int_.eq; }
    typedef IntBounds Bounds;
    static bool finite(const IntBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
    static Bounds compute_bounds(Expression* e) { return compute_int_bounds(e); }
    typedef IntSetVal* Domain;
    static Domain eval_domain(Expression* e) { return eval_intset(e); }
    static Expression* new_domain(Val v) { return new SetLit(Location(),IntSetVal::a(v,v)); }
    static Expression* new_domain(Val v0, Val v1) { return new SetLit(Location(),IntSetVal::a(v0,v1)); }
    static Expression* new_domain(Domain d) { return new SetLit(Location(),d); }
    static bool domain_contains(Domain dom, Val v) { return dom->contains(v); }
    static bool domain_equals(Domain dom, Val v) { return dom->size()==1 && dom->min(0)==v && dom->max(0)==v; }
    static bool domain_equals(Domain dom1, Domain dom2) {
      IntSetRanges d1(dom1);
      IntSetRanges d2(dom2);
      return Ranges::equal(d1,d2);
    }
    static bool domain_intersects(Domain dom, Val v0, Val v1) {
      return dom->min(0) <= v1 && v0 <= dom->max(dom->size()-1);
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
        default: assert(false); return NULL;
      }
      return ndomain;
    }
    static Domain intersect_domain(Domain dom, Val v0, Val v1) {
      IntSetRanges dr(dom);
      Ranges::Const c(v0,v1);
      Ranges::Inter<IntSetRanges,Ranges::Const> inter(dr,c);
      return IntSetVal::ai(inter);
    }
    static Val floor_div(Val v0, Val v1) {
      return static_cast<long long int>(floor(static_cast<FloatVal>(v0.toInt()) / static_cast<FloatVal>(v1.toInt())));
    }
    static Val ceil_div(Val v0, Val v1) { return static_cast<long long int>(ceil(static_cast<FloatVal>(v0.toInt()) / v1.toInt())); }
  };
  template<>
  class LinearTraits<FloatLit> {
  public:
    typedef FloatVal Val;
    static Val eval(Expression* e) { return eval_float(e); }
    static void constructLinBuiltin(BinOpType bot, ASTString& callid, int& coeff_sign, Val& d) {
      switch (bot) {
        case BOT_LE:
          callid = constants().ids.float_.lin_lt;
          coeff_sign = 1;
          break;
        case BOT_LQ:
          callid = constants().ids.float_.lin_le;
          coeff_sign = 1;
          break;
        case BOT_GR:
          callid = constants().ids.float_.lin_lt;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_GQ:
          callid = constants().ids.float_.lin_le;
          coeff_sign = -1;
          d = -d;
          break;
        case BOT_EQ:
          callid = constants().ids.float_.lin_eq;
          coeff_sign = 1;
          break;
        case BOT_NQ:
          callid = constants().ids.float_.lin_ne;
          coeff_sign = 1;
          break;
        default: assert(false); break;
      }
    }
    static ASTString id_eq(void) { return constants().ids.float_.eq; }
    typedef FloatBounds Bounds;
    static bool finite(const FloatBounds& ib) { return true; }
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
    static Expression* new_domain(Val v0, Val v1) {
      BinOp* ret = new BinOp(Location(),new FloatLit(Location(),v0),BOT_DOTDOT,new FloatLit(Location(),v1));
      ret->type(Type::parsetfloat());
      return ret;
    }
    static Expression* new_domain(Domain d) { return d; }
    static bool domain_contains(Domain dom, Val v) {
      return dom==NULL || (dom->lhs()->cast<FloatLit>()->v() <= v && dom->rhs()->cast<FloatLit>()->v() >= v);
    }
    static bool domain_intersects(Domain dom, Val v0, Val v1) {
      return dom==NULL || (dom->lhs()->cast<FloatLit>()->v() <= v1 && dom->rhs()->cast<FloatLit>()->v() >= v0);
    }
    static bool domain_equals(Domain dom, Val v) {
      return dom != NULL && dom->lhs()->cast<FloatLit>()->v() == v && dom->rhs()->cast<FloatLit>()->v() == v;
    }
    static bool domain_equals(Domain dom1, Domain dom2) {
      if (dom1==dom2) return true;
      if (dom1==NULL || dom2==NULL) return false;
      return
        dom1->lhs()->cast<FloatLit>()->v() == dom2->lhs()->cast<FloatLit>()->v() &&
        dom1->rhs()->cast<FloatLit>()->v() == dom2->rhs()->cast<FloatLit>()->v();
    }
    static bool domain_empty(Domain dom) {
      return dom != NULL && dom->lhs()->cast<FloatLit>()->v() > dom->rhs()->cast<FloatLit>()->v();
    }
    static Domain intersect_domain(Domain dom, Val v0, Val v1) {
      if (dom) {
        Val lb = dom->lhs()->cast<FloatLit>()->v();
        Val ub = dom->rhs()->cast<FloatLit>()->v();
        lb = std::max(lb,v0);
        ub = std::min(ub,v1);
        Domain d = new BinOp(Location(), new FloatLit(Location(),lb), BOT_DOTDOT, new FloatLit(Location(),ub));
        d->type(Type::parsetfloat());
        return d;
      } else {
        Domain d = new BinOp(Location(), new FloatLit(Location(),v0), BOT_DOTDOT, new FloatLit(Location(),v1));
        d->type(Type::parsetfloat());
        return d;
      }
    }
    static Domain limit_domain(BinOpType bot, Domain dom, Val v) {
      if (dom) {
        Val lb = dom->lhs()->cast<FloatLit>()->v();
        Val ub = dom->rhs()->cast<FloatLit>()->v();
        Domain ndomain;
        switch (bot) {
          case BOT_LE:
            return NULL;
          case BOT_LQ:
            if (v < ub) {
              Domain d = new BinOp(dom->loc(),dom->lhs(),BOT_DOTDOT,new FloatLit(Location(),v));
              d->type(Type::parsetfloat());
              return d;
            } else {
              return dom;
            }
          case BOT_GR:
            return NULL;
          case BOT_GQ:
            if (v > lb) {
              Domain d = new BinOp(dom->loc(),new FloatLit(Location(),v),BOT_DOTDOT,dom->rhs());
              d->type(Type::parsetfloat());
              return d;
            } else {
              return dom;
            }
          case BOT_NQ:
            return NULL;
          default: assert(false); return NULL;
        }
        return ndomain;
      }
      return NULL;
    }
    static Val floor_div(Val v0, Val v1) { return floor(v0 / v1); }
    static Val ceil_div(Val v0, Val v1) { return ceil(v0 / v1); }
  };

  template<class Lit>
  void collectLinExps(typename LinearTraits<Lit>::Val c, Expression* exp,
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
        constval += c * LinearTraits<Lit>::eval(e);
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
              stack.push_back(StackItem(bo->rhs(),c*LinearTraits<Lit>::eval(bo->lhs())));
            } else if (bo->rhs()->type().ispar()) {
              stack.push_back(StackItem(bo->lhs(),c*LinearTraits<Lit>::eval(bo->rhs())));
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
  void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c,
                    std::vector<KeepAlive>& x,
                    typename LinearTraits<Lit>::Val& d) {
    std::vector<int> idx(c.size());
    for (unsigned int i=idx.size(); i--;) {
      idx[i]=i;
    }
    std::sort(idx.begin(),idx.end(),CmpExpIdx(x));
    unsigned int ci = 0;
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

  template<class Lit>
  KeepAlive mklinexp(EnvI& env, typename LinearTraits<Lit>::Val c0, typename LinearTraits<Lit>::Val c1,
                     Expression* e0, Expression* e1) {
    typedef typename LinearTraits<Lit>::Val Val;
    GCLock lock;
    
    std::vector<Val> coeffs;
    std::vector<KeepAlive> vars;
    Val constval = 0;
    collectLinExps<Lit>(c0, e0, coeffs, vars, constval);
    collectLinExps<Lit>(c1, e1, coeffs, vars, constval);
    simplify_lin<Lit>(coeffs, vars, constval);
    KeepAlive ka;
    if (coeffs.size()==0) {
      ka = new Lit(e0->loc(),constval);
    } else if (coeffs.size()==1 && coeffs[0]==1 && constval==0) {
      ka = vars[0];
    } else {
      std::vector<Expression*> coeffs_e(coeffs.size());
      for (unsigned int i=coeffs.size(); i--;)
        coeffs_e[i] = new Lit(e0->loc(),coeffs[i]);
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
      args[2] = new Lit(e0->loc(),constval);
      Call* c = new Call(e0->loc(),constants().ids.lin_exp,args);
      tt = args[1]->type();
      tt.dim(0);
      c->decl(env.orig->matchFn(c));
      if (c->decl()==NULL) {
        throw FlatteningError(env,c->loc(), "cannot find matching declaration");
      }
      c->type(c->decl()->rtype(args));
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

  
  Expression* createImplication(EnvI& env, const std::vector<Expression*>& elseconds, Expression* ifcond, Expression* then) {
    std::vector<Expression*> clauseArgs(2);
    std::vector<Expression*> pos = elseconds;
    pos.push_back(then);
    std::vector<Expression*> neg;
    if (ifcond)
      neg.push_back(ifcond);
    clauseArgs[0] = new ArrayLit(Location(), pos);
    clauseArgs[0]->type(Type::varbool(1));
    clauseArgs[1] = new ArrayLit(Location(), neg);
    clauseArgs[1]->type(Type::varbool(1));
    Call* clause = new Call(Location(), constants().ids.clause, clauseArgs);
    clause->decl(env.orig->matchFn(clause));
    clause->type(clause->decl()->rtype(clauseArgs));
    return clause;
  }

  /// TODO: check if all expressions are total
  /// If yes, use element encoding
  /// If not, use implication encoding
  EE flat_ite(EnvI& env,Ctx ctx, ITE* ite, VarDecl* r, VarDecl* b) {
    
    std::vector<Expression*> clauses;
    std::vector<Expression*> elseconds;
    
    GC::lock();
    
    IntBounds r_bounds(IntVal::infinity,-IntVal::infinity,true);
    if (r && r->type().isint()) {
      r_bounds = compute_int_bounds(r);
    }
    
    VarDecl* nr = r;
    
    for (int i=0; i<ite->size(); i++) {
      if (ite->e_if(i)->type()==Type::parbool()) {
        bool cond = eval_bool(ite->e_if(i));
        if (cond) {
          if (nr==NULL || elseconds.size()==0) {
            GC::unlock();
            return flat_exp(env,ctx,ite->e_then(i),r,b);
          }
          Expression* eq_then;
          if (nr == constants().var_true) {
            eq_then = ite->e_then(i);
          } else {
            eq_then = new BinOp(Location(),nr->id(),BOT_EQ,ite->e_then(i));
            eq_then->type(Type::varbool());
          }
          clauses.push_back(createImplication(env, elseconds, NULL, eq_then));
          break;
        }
      } else {
        if (nr==NULL) {
          TypeInst* ti = new TypeInst(Location(),ite->type(),NULL);
          nr = new VarDecl(ite->loc(),ti,env.genId());
          nr->introduced(true);
          nr->flat(nr);
          nr->addAnnotation(constants().ann.promise_total);
          clauses.push_back(nr);
        }
        Expression* eq_then;
        if (nr == constants().var_true) {
          eq_then = ite->e_then(i);
        } else {
          eq_then = new BinOp(Location(),nr->id(),BOT_EQ,ite->e_then(i));
          eq_then->type(Type::varbool());
        }
        clauses.push_back(createImplication(env, elseconds, ite->e_if(i), eq_then));
        elseconds.push_back(ite->e_if(i));
      }

      if (r_bounds.valid && ite->e_then(i)->type().isint()) {
        IntBounds ib_then = compute_int_bounds(ite->e_then(i));
        if (ib_then.valid) {
          IntVal lb = std::min(r_bounds.l, ib_then.l);
          IntVal ub = std::max(r_bounds.u, ib_then.u);
          r_bounds = IntBounds(lb,ub,true);
        } else {
          r_bounds = IntBounds(0,0,false);
        }
      }
    
    }
    if (nr==NULL || elseconds.size()==0) {
      GC::unlock();
      return flat_exp(env,ctx,ite->e_else(),r,b);
    }

    if (r_bounds.valid && ite->e_else()->type().isint()) {
      IntBounds ib_else = compute_int_bounds(ite->e_else());
      if (ib_else.valid) {
        IntVal lb = std::min(r_bounds.l, ib_else.l);
        IntVal ub = std::max(r_bounds.u, ib_else.u);
        SetLit* r_dom = new SetLit(Location(), IntSetVal::a(lb,ub));
        nr->ti()->domain(r_dom);
      }
    }

    Expression* eq_else;
    if (nr == constants().var_true) {
      eq_else = ite->e_else();
    } else {
      eq_else = new BinOp(Location(),nr->id(),BOT_EQ,ite->e_else());
      eq_else->type(Type::varbool());
    }
    clauses.push_back(createImplication(env, elseconds, NULL, eq_else));
    
    Let* let = new Let(Location(),clauses,nr->id());
    let->type(nr->id()->type());
    KeepAlive ka = let;
    GC::unlock();
    return flat_exp(env, ctx, ka(), r, b);
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
        if (domain && ndomain) {
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
      GCLock lock;
      if (assignTo != NULL) {
        Val resultCoeff;
        typename LinearTraits<Lit>::Bounds bounds(d,d,true);
        for (unsigned int i=coeffv.size(); i--;) {
          if (alv[i]()==assignTo) {
            resultCoeff = coeffv[i];
            continue;
          }
          typename LinearTraits<Lit>::Bounds b = LinearTraits<Lit>::compute_bounds(alv[i]());

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
        if (bounds.valid) {
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
            typename LinearTraits<Lit>::Domain domain = LinearTraits<Lit>::eval_domain(vd->ti()->domain());
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
        coeff_ev[i] = new Lit(Location(),coeff_sign*coeffv[i]);
      ArrayLit* ncoeff = new ArrayLit(Location(),coeff_ev);
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
    t.dim(1);
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
        t.dim(1);
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
    if (e->type().ispar() && !e->isa<Let>() && !e->isa<VarDecl>() && e->type().bt()!=Type::BT_ANN) {
      ret.b = bind(env,Ctx(),b,constants().lit_true);
      if (e->type().dim() > 0) {
        EnvI::Map::iterator it;
        Id* id = e->dyn_cast<Id>();
        if (id && (id->decl()->flat()==NULL || id->decl()->toplevel())) {
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
          VarDecl* vd = new VarDecl(e->loc(),ti,env.genId(),al);
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
    case Expression::E_STRINGLIT:
      {
        GCLock lock;
        ret.b = bind(env,Ctx(),b,constants().lit_true);
        ret.r = bind(env,Ctx(),r,e);
        return ret;
      }
    case Expression::E_SETLIT:
      {
        SetLit* sl = e->cast<SetLit>();
        assert(sl->isv()==NULL);
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
          Expression* ee = eval_par(e);
          ret.r = bind(env,Ctx(),r,ee);
        } else {
          GCLock lock;
          ArrayLit* al = new ArrayLit(sl->loc(),elems);
          al->type(Type::varint(1));
          std::vector<Expression*> args(1);
          args[0] = al;
          Call* cc = new Call(sl->loc(), "array2set", args);
          cc->type(Type::varsetint());
          FunctionI* fi = env.orig->matchFn(cc->id(),args);
          if (fi==NULL) {
            throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
          }
          assert(fi);
          assert(fi->rtype(args).isSubtypeOf(cc->type()));
          cc->decl(fi);
          EE ee = flat_exp(env, Ctx(), cc, NULL, constants().var_true);
          ret.r = bind(env,Ctx(),r,ee.r());
        }
      }
      break;
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
            throw FlatteningError(env,e->loc(), "undefined identifier");
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
                                    new IntLit(id->loc(),0));
            
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
                throw FlatteningError(env,ti->loc(),"array dimensions unknown");
              IntSetVal* isv = eval_intset(ti->domain());
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
            TypeInst* vti = new TypeInst(Location(),tt,vd->ti()->domain());
            
            std::vector<Expression*> elems(static_cast<int>(asize.toInt()));
            for (int i=0; i<static_cast<int>(asize.toInt()); i++) {
              VarDecl* nvd = new VarDecl(vd->loc(),vti,env.genId());
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
                  new VarDecl(vd->loc(),eval_typeinst(env,vd),env.genId(),NULL);
                  nvd->introduced(true);
                  nvd->flat(nvd);
                  for (ExpressionSetIter it = vd->ann().begin(); it != vd->ann().end(); ++it) {
                    EE ee_ann = flat_exp(env, Ctx(), *it, NULL, constants().var_true);
                    nvd->addAnnotation(ee_ann.r());
                  }
                  
                  VarDeclI* ni = new VarDeclI(Location(),nvd);
                  env.flat_addItem(ni);
                  if (vd->e()) {
                    (void) flat_exp(env, Ctx(), vd->e(), nvd, constants().var_true);
                  }
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
              if (id->type().bt() == Type::BT_ANN && vd->e()) {
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
                                  env.genId());
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
        ret.r = bind(env,Ctx(),r,ka());
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
          if (!success && ctx.b==C_ROOT && b==constants().var_true) {
            throw FlatteningError(env,e->loc(),"array access out of bounds");
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
            Call* cc = new Call(e->loc(),constants().ids.element,args);
            cc->type(aa->type());
            FunctionI* fi = env.orig->matchFn(cc->id(),args);
            if (fi==NULL) {
              throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
            }
            assert(fi);
            assert(fi->rtype(args).isSubtypeOf(cc->type()));
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
        Comprehension* c = e->cast<Comprehension>();
        KeepAlive c_ka(c);
        
        if (c->set()) {
          throw InternalError("not supported yet");
        }
        
        if (c->type().isopt()) {
          std::vector<Expression*> in(c->n_generators());
          std::vector<Expression*> where;
          GCLock lock;
          for (int i=0; i<c->n_generators(); i++) {
            if (c->in(i)->type().isvar()) {
              std::vector<Expression*> args(1);
              args[0] = c->in(i);
              Call* ub = new Call(Location(),"ub",args);
              ub->type(Type::parsetint());
              ub->decl(env.orig->matchFn(ub));
              in[i] = ub;
              for (int j=0; j<c->n_decls(i); j++) {
                BinOp* bo = new BinOp(Location(),c->decl(i,j)->id(), BOT_IN, c->in(i));
                bo->type(Type::varbool());
                where.push_back(bo);
              }
            } else {
              in[i] = c->in(i);
            }
          }
          if (where.size() > 0 || (c->where() && c->where()->type().isvar())) {
            Generators gs;
            if (c->where()==NULL || c->where()->type().ispar())
              gs._w = c->where();
            else
              where.push_back(c->where());
            for (int i=0; i<c->n_generators(); i++) {
              std::vector<VarDecl*> vds(c->n_decls(i));
              for (int j=0; j<c->n_decls(i); j++)
                vds[j] = c->decl(i, j);
              gs._g.push_back(Generator(vds,in[i]));
            }
            Expression* cond;
            if (where.size() > 1) {
              ArrayLit* al = new ArrayLit(Location(), where);
              al->type(Type::varbool(1));
              std::vector<Expression*> args(1);
              args[0] = al;
              Call* forall = new Call(Location(), constants().ids.forall, args);
              forall->type(Type::varbool());
              forall->decl(env.orig->matchFn(forall));
              cond = forall;
            } else {
              cond = where[0];
            }
            

            Expression* r_bounds = NULL;
            if (c->e()->type().bt()==Type::BT_INT && c->e()->type().dim() == 0) {
              std::vector<Expression*> ubargs(1);
              ubargs[0] = c->e();
              if (c->e()->type().st()==Type::ST_SET) {
                Call* bc = new Call(Location(),"ub",ubargs);
                bc->type(Type::parsetint());
                bc->decl(env.orig->matchFn(bc));
                r_bounds = bc;
              } else {
                Call* lbc = new Call(Location(),"lb",ubargs);
                lbc->type(Type::parint());
                lbc->decl(env.orig->matchFn(lbc));
                Call* ubc = new Call(Location(),"ub",ubargs);
                ubc->type(Type::parint());
                ubc->decl(env.orig->matchFn(ubc));
                r_bounds = new BinOp(Location(),lbc,BOT_DOTDOT,ubc);
                r_bounds->type(Type::parsetint());
              }
            }
            Type tt;
            tt = c->e()->type();
            tt.ti(Type::TI_VAR);
            tt.ot(Type::OT_OPTIONAL);
            
            TypeInst* ti = new TypeInst(Location(),tt,r_bounds);
            VarDecl* r = new VarDecl(c->loc(),ti,env.genId());
            r->addAnnotation(constants().ann.promise_total);
            r->introduced(true);
            r->flat(r);

            std::vector<Expression*> let_exprs(3);
            let_exprs[0] = r;
            BinOp* r_eq_e = new BinOp(Location(),r->id(),BOT_EQ,c->e());
            r_eq_e->type(Type::varbool());
            let_exprs[1] = new BinOp(Location(),cond,BOT_IMPL,r_eq_e);
            let_exprs[1]->type(Type::varbool());
            std::vector<Expression*> absent_r_args(1);
            absent_r_args[0] = r->id();
            Call* absent_r = new Call(Location(), "absent", absent_r_args);
            absent_r->type(Type::varbool());
            absent_r->decl(env.orig->matchFn(absent_r));
            let_exprs[2] = new BinOp(Location(),cond,BOT_OR,absent_r);
            let_exprs[2]->type(Type::varbool());
            Let* let = new Let(Location(), let_exprs, r->id());
            let->type(r->type());
            Comprehension* nc = new Comprehension(c->loc(),let,gs,c->set());
            nc->type(c->type());
            c = nc;
            c_ka = c;
          }
        }
        
        class EvalF {
        public:
          EnvI& env;
          Ctx ctx;
          EvalF(EnvI& env0, Ctx ctx0) : env(env0), ctx(ctx0) {}
          typedef EE ArrayVal;
          EE e(Expression* e) {
            VarDecl* b = ctx.b==C_ROOT ? constants().var_true : NULL;
            VarDecl* r = (ctx.b == C_ROOT && e->type().isbool() && !e->type().isopt()) ? constants().var_true : NULL;
            return flat_exp(env,ctx,e,r,b);
          }
        } _evalf(env,ctx);
        std::vector<EE> elems_ee = eval_comp<EvalF>(_evalf,c);
        std::vector<Expression*> elems(elems_ee.size());
        bool allPar = true;
        for (unsigned int i=elems.size(); i--;) {
          elems[i] = elems_ee[i].r();
          if (!elems[i]->type().ispar())
            allPar = false;
        }
        KeepAlive ka;
        {
          GCLock lock;
          ArrayLit* alr = new ArrayLit(Location(),elems);
          Type alt = c->type();
          if (allPar)
            alt.ti(Type::TI_PAR);
          alr->type(alt);
          ka = alr;
        }
        ret.b = conj(env,b,Ctx(),elems_ee);
        ret.r = bind(env,Ctx(),r,ka());
      }
      break;
    case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        ret = flat_ite(env,ctx,ite,r,b);
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
          ret = flat_exp(env, Ctx(), keepAlive, constants().var_true, constants().var_true);
          
          env.reverseMappers.insert(ee.r()->cast<Id>(),revMap);
          
          break;
        }
        if ( (bo->op()==BOT_EQ ||  bo->op()==BOT_EQUIV) && (bo->lhs()==constants().absent || bo->rhs()==constants().absent) ) {
          GCLock lock;
          std::vector<Expression*> args(1);
          args[0] = bo->lhs()==constants().absent ? bo->rhs() : bo->lhs();
          if (args[0] != constants().absent) {
            Call* cr = new Call(bo->loc(),"absent",args);
            cr->decl(env.orig->matchFn(cr));
            cr->type(cr->decl()->rtype(args));
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
              Expression* res = eval_par(parbo);
              assert(!res->type().isunknown());
              ret.r = bind(env,ctx,r,res);
              std::vector<EE> ees(2);
              ees[0].b = e0.b; ees[1].b = e1.b;
              ret.b = conj(env,b,Ctx(),ees);
              break;
            }
            
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
            Call* cc;
            if (bo->decl()) {
              cc = new Call(bo->loc(),bo->opToString(),args);
            } else {
              cc = new Call(bo->loc(),opToBuiltin(bo,bot),args);
            }
            cc->type(bo->type());

            EnvI::Map::iterator cit;
            if ( (cit = env.map_find(cc)) != env.map_end()) {
              ret.b = bind(env,Ctx(),b,env.ignorePartial ? constants().lit_true : cit->second.b());
              ret.r = bind(env,ctx,r,cit->second.r());
            } else {
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
              (void) flat_exp(env,nctx,boe0,constants().var_true,constants().var_true);
              (void) flat_exp(env,nctx,boe1,constants().var_true,constants().var_true);
              ret.r = bind(env,ctx,r,constants().lit_true);
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
            if (ctx.b==C_ROOT && r==constants().var_true && boe0->type().ispar()) {
              if (eval_bool(boe0)) {
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
              if (eval_bool(boe1)) {
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
                  if (id->type().st()==Type::ST_PLAIN && newdom->size()==0) {
                    env.addWarning("model inconsistency detected");
                    env.flat_addItem(new ConstraintI(Location(),constants().lit_false));
                  } else if (changeDom) {
                    id->decl()->ti()->setComputedDomain(false);
                    id->decl()->ti()->domain(new SetLit(Location(),newdom));
                  }
                  id = id->decl()->e() ? id->decl()->e()->dyn_cast<Id>() : NULL;
                }
              }
              ret.r = bind(env,ctx,r,constants().lit_true);
              break;
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
              if (boe1->type().isint() && !boe0->type().isopt()) {
                le1 = get_linexp<IntLit>(e1.r());
              } else if (boe1->type().isfloat() && !boe0->type().isopt()) {
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
                if (bo->decl()) {
                  callid = bo->decl()->id();
                } else {
                  callid = opToBuiltin(bo,bot);
                }
              }
              
              std::vector<Expression*> args_e(args.size());
              for (unsigned int i=args.size(); i--;)
                args_e[i] = args[i]();
              Call* cc = new Call(e->loc(),callid,args_e);
              cc->decl(env.orig->matchFn(cc->id(),args_e));
              if (cc->decl()==NULL) {
                throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
              }
              cc->type(cc->decl()->rtype(args_e));

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
                  ees[2].b = new UnOp(Location(),UOT_NOT,ees[2].b());
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
            Expression* zero;
            if (uo->e()->type().bt()==Type::BT_INT)
              zero = new IntLit(Location(),0);
            else
              zero = new FloatLit(Location(),0.0);
            BinOp* bo = new BinOp(Location(),zero,BOT_MINUS,uo->e());
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
            KeepAlive callres = decl->_builtins.e(c->args());
            ret = flat_exp(env,ctx,callres(),r,b);
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
          ret.r = bind(env,ctx,r,constants().lit_true);
        } else {
          std::vector<EE> args_ee(c->args().size());
          bool mixContext = decl->e()!=NULL ||
            (cid != constants().ids.forall && cid != constants().ids.exists && cid != constants().ids.bool2int &&
             cid != constants().ids.sum && cid != constants().ids.lin_exp && cid != "assert");
          for (unsigned int i=c->args().size(); i--;) {
            Ctx argctx = nctx;
            if (mixContext) {
              if (c->args()[i]->type().bt()==Type::BT_BOOL) {
                argctx.b = C_MIX;
              } else if (c->args()[i]->type().bt()==Type::BT_INT) {
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
              bool subsumed = remove_dups(pos_alv,false);
              subsumed = subsumed || remove_dups(neg_alv,true);
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
            if (decl==NULL)
              throw FlatteningError(env,cr_c->loc(), "cannot find matching declaration");
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
                  if (args[i]()->type().bt() == Type::BT_INT) {
                    GCLock lock;
                    IntSetVal* isv = eval_intset(dom);
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
                  } else if (args[i]()->type().bt() == Type::BT_BOT) {
                    // Nothing to be done for empty arrays/sets
                  } else {
                    throw EvalError(decl->params()[i]->loc(),"domain restrictions other than int not supported yet");
                  }
                }
              }
            }
            if (cr()->type().isbool() && !cr()->type().isopt() && (ctx.b != C_ROOT || r != constants().var_true)) {
              GCLock lock;
              VarDecl* reif_b = r;
              if (reif_b == NULL) {
                VarDecl* nvd = new VarDecl(Location(), new TypeInst(Location(),Type::varbool()), env.genId());
                nvd->type(Type::varbool());
                nvd->introduced(true);
                nvd->flat(nvd);
                VarDeclI* nv = new VarDeclI(Location(),nvd);
                env.flat_addItem(nv);
                reif_b = nvd;
              }
              args.push_back(reif_b->id());
              Call* cr_real = new Call(Location(),env.reifyId(cid),toExpVec(args));
              cr_real->type(Type::varbool());
              FunctionI* decl_real = env.orig->matchFn(cr_real);
              if (decl_real && decl_real->e()) {
                cr_real->decl(decl_real);
                bool ignorePartial = env.ignorePartial;
                env.ignorePartial = true;
                reif_b->addAnnotation(constants().ann.is_defined_var);
                cr_real->addAnnotation(definesVarAnn(reif_b->id()));
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
                if (callt.ispar() && callt.bt()!=Type::BT_ANN) {
                  GCLock lock;
                  ret.b = conj(env,b,Ctx(),args_ee);
                  ret.r = bind(env,ctx,r,eval_par(cr_c));
                  // Do not insert into map, since par results will quickly become
                  // garbage anyway and then disappear from the map
                } else if (decl->_builtins.e) {
                  KeepAlive callres = decl->_builtins.e(cr_c->args());
                  EE res = flat_exp(env,ctx,callres(),r,b);
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
          throw FlatteningError(env,e->loc(), "not in root context");
        VarDecl* v = e->cast<VarDecl>();
        VarDecl* it = v->flat();
        if (it==NULL) {
          VarDecl* vd = new VarDecl(v->loc(), eval_typeinst(env,v), v->id());
          vd->introduced(v->introduced());
          vd->flat(vd);
          v->flat(vd);
          for (ExpressionSetIter it = v->ann().begin(); it != v->ann().end(); ++it) {
            vd->addAnnotation(flat_exp(env,Ctx(),*it,NULL,constants().var_true).r());
          }
          VarDeclI* nv = new VarDeclI(Location(),vd);
          env.flat_addItem(nv);
          Ctx nctx;
          if (v->e() && v->e()->type().bt() == Type::BT_BOOL)
            nctx.b = C_MIX;
          if (v->e()) {
            (void) flat_exp(env,nctx,v->e(),vd,constants().var_true);
            if (v->e()->type().bt()==Type::BT_INT && v->e()->type().dim()==0) {
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
              if (vd->e()->type().bt()==Type::BT_BOOL)
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
              if ((ctx.b==C_NEG || ctx.b==C_MIX) && !vd->ann().contains(constants().ann.promise_total)) {
                CallStackItem csi_vd(env, vd);
                throw FlatteningError(env,vd->loc(),
                                      "free variable in non-positive context");
              }
              GCLock lock;
              TypeInst* ti = eval_typeinst(env,vd);
              VarDecl* nvd = new VarDecl(vd->loc(),ti,env.genId());
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
  
  bool isBuiltin(FunctionI* decl) {
    return (decl->loc().filename == "builtins.mzn" ||
            decl->loc().filename.endsWith("/builtins.mzn") ||
            decl->loc().filename == "stdlib.mzn" ||
            decl->loc().filename.endsWith("/stdlib.mzn"));
  }
  
  void outputVarDecls(EnvI& env, Item* ci, Expression* e);

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
          tv[i].ti(Type::TI_PAR);
        }
        FunctionI* decl = env.output->matchFn(c.id(), tv);
        Type t;
        if (decl==NULL) {
          FunctionI* origdecl = env.orig->matchFn(c.id(), tv);
          if (origdecl == NULL) {
            throw FlatteningError(env,c.loc(),"function is used in output, par version needed");
          }

          if (origdecl->e() && cannotUseRHSForOutput(env, origdecl->e())) {
            success = false;
          } else {
            if (!isBuiltin(origdecl)) {
              decl = copy(env.cmap,origdecl)->cast<FunctionI>();
              CollectOccurrencesE ce(env.output_vo,decl);
              topDown(ce, decl->e());
              topDown(ce, decl->ti());
              for (unsigned int i = decl->params().size(); i--;)
                topDown(ce, decl->params()[i]);
              env.output->registerFn(decl);
              env.output->addItem(decl);
              outputVarDecls(env,origdecl,decl->e());
            } else {
              decl = origdecl;
            }
            c.decl(decl);
          }
        }
        if (success) {
          t = decl->rtype(tv);
          if (!t.ispar())
            success = false;
        }
      }
      void vId(const Id& id) {}
      /// Visit let
      void vLet(const Let&) { success = false; }
      /// Visit variable declaration
      void vVarDecl(const VarDecl& vd) {}
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
  
  void outputVarDecls(EnvI& env, Item* ci, Expression* e) {
    class O : public EVisitor {
    public:
      EnvI& env;
      Item* ci;
      O(EnvI& env0, Item* ci0) : env(env0), ci(ci0) {}
      void vId(Id& id) {
        if (&id==constants().absent)
          return;
        if (!id.decl()->toplevel())
          return;
        VarDecl* vd = id.decl();
        VarDecl* reallyFlat = vd->flat();
        while (reallyFlat != NULL && reallyFlat != reallyFlat->flat())
          reallyFlat = reallyFlat->flat();
        IdMap<int>::iterator idx = env.output_vo.idx.find(reallyFlat->id());
        IdMap<int>::iterator idx2 = env.output_vo.idx.find(vd->id());
        if (idx==env.output_vo.idx.end() && idx2==env.output_vo.idx.end()) {
          VarDeclI* nvi = new VarDeclI(Location(), copy(env.cmap,vd)->cast<VarDecl>());
          Type t = nvi->e()->ti()->type();
          if (t.ti() != Type::TI_PAR) {
            t.ti(Type::TI_PAR);
          }
          nvi->e()->type(t);
          nvi->e()->ti()->type(t);
          nvi->e()->ti()->domain(NULL);
          nvi->e()->flat(vd->flat());
          nvi->e()->ann().clear();
          nvi->e()->introduced(false);
          if (reallyFlat)
            env.output_vo.add(reallyFlat, env.output->size());
          env.output_vo.add(nvi, env.output->size());
          env.output_vo.add(nvi->e(), ci);
          env.output->addItem(nvi);
          
          IdMap<KeepAlive>::iterator it;
          if ( (it = env.reverseMappers.find(nvi->e()->id())) != env.reverseMappers.end()) {
            Call* rhs = copy(env.cmap,it->second())->cast<Call>();
            {
              std::vector<Type> tv(rhs->args().size());
              for (unsigned int i=rhs->args().size(); i--;) {
                tv[i] = rhs->args()[i]->type();
                tv[i].ti(Type::TI_PAR);
              }
              FunctionI* decl = env.output->matchFn(rhs->id(), tv);
              Type t;
              if (decl==NULL) {
                FunctionI* origdecl = env.orig->matchFn(rhs->id(), tv);
                if (origdecl == NULL) {
                  throw FlatteningError(env,rhs->loc(),"function is used in output, par version needed");
                }
                if (!isBuiltin(origdecl)) {
                  decl = copy(env.cmap,origdecl)->cast<FunctionI>();
                  CollectOccurrencesE ce(env.output_vo,decl);
                  topDown(ce, decl->e());
                  topDown(ce, decl->ti());
                  for (unsigned int i = decl->params().size(); i--;)
                    topDown(ce, decl->params()[i]);
                  env.output->registerFn(decl);
                  env.output->addItem(decl);
                } else {
                  decl = origdecl;
                }
                rhs->decl(decl);
              }
            }
            outputVarDecls(env,nvi,it->second());
            nvi->e()->e(rhs);
          } else if (reallyFlat && cannotUseRHSForOutput(env, reallyFlat->e())) {
            assert(nvi->e()->flat());
            nvi->e()->e(NULL);
            if (nvi->e()->type().dim() == 0) {
              reallyFlat->addAnnotation(constants().ann.output_var);
            } else {
              std::vector<Expression*> args(reallyFlat->e()->type().dim());
              for (unsigned int i=0; i<args.size(); i++) {
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
            outputVarDecls(env, nvi, nvi->e()->e());
          }
          CollectOccurrencesE ce(env.output_vo,nvi);
          topDown(ce, nvi->e());
        }
      }
    } _o(env,ci);
    topDown(_o, e);
  }

  
  void copyOutput(EnvI& e) {
    struct CopyOutput : public EVisitor {
      EnvI& env;
      CopyOutput(EnvI& env0) : env(env0) {}
      void vId(Id& _id) {
        _id.decl(_id.decl()->flat());
      }
      void vCall(Call& c) {
        std::vector<Type> tv(c.args().size());
        for (unsigned int i=c.args().size(); i--;) {
          tv[i] = c.args()[i]->type();
          tv[i].ti(Type::TI_PAR);
        }
        FunctionI* decl = c.decl();
        if (!isBuiltin(decl)) {
          env.flat_addItem(decl);
        }
      }
    };
    for (unsigned int i=e.orig->size(); i--;) {
      if (OutputI* oi = (*e.orig)[i]->dyn_cast<OutputI>()) {
        GCLock lock;
        OutputI* noi = copy(oi)->cast<OutputI>();
        CopyOutput co(e);
        topDown(co, noi->e());
        e.flat_addItem(noi);
        break;
      }
    }
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
            IdMap<KeepAlive>::iterator it;
            GCLock lock;
            VarDecl* reallyFlat = vd->flat();
            while (reallyFlat && reallyFlat!=reallyFlat->flat())
              reallyFlat=reallyFlat->flat();
            if (vd->e()==NULL) {
              if (vd->flat()->e() && vd->flat()->e()->type().ispar()) {
                VarDecl* reallyFlat = vd->flat();
                while (reallyFlat!=reallyFlat->flat())
                  reallyFlat=reallyFlat->flat();
                removeIsOutput(reallyFlat);
                Expression* flate = copy(e.cmap,follow_id(reallyFlat->id()));
                outputVarDecls(e,item,flate);
                vd->e(flate);
              } else if ( (it = e.reverseMappers.find(vd->id())) != e.reverseMappers.end()) {
                Call* rhs = copy(e.cmap,it->second())->cast<Call>();
                std::vector<Type> tv(rhs->args().size());
                for (unsigned int i=rhs->args().size(); i--;) {
                  tv[i] = rhs->args()[i]->type();
                  tv[i].ti(Type::TI_PAR);
                }
                FunctionI* decl = e.output->matchFn(rhs->id(), tv);
                if (decl==NULL) {
                  FunctionI* origdecl = e.orig->matchFn(rhs->id(), tv);
                  if (origdecl == NULL) {
                    throw FlatteningError(e,rhs->loc(),"function is used in output, par version needed");
                  }
                  if (!isBuiltin(origdecl)) {
                    decl = copy(e.cmap,origdecl)->cast<FunctionI>();
                    CollectOccurrencesE ce(e.output_vo,decl);
                    topDown(ce, decl->e());
                    topDown(ce, decl->ti());
                    for (unsigned int i = decl->params().size(); i--;)
                      topDown(ce, decl->params()[i]);
                    e.output->registerFn(decl);
                    e.output->addItem(decl);
                  } else {
                    decl = origdecl;
                  }
                  rhs->decl(decl);
                }
                removeIsOutput(reallyFlat);
                
                outputVarDecls(e,item,rhs);
                vd->e(rhs);
              } else {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                assert(vd->flat());

                bool needOutputAnn = true;
                if (reallyFlat->e() && reallyFlat->e()->isa<ArrayLit>()) {
                  ArrayLit* al = reallyFlat->e()->cast<ArrayLit>();
                  for (unsigned int i=0; i<al->v().size(); i++) {
                    if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                      if (e.reverseMappers.find(id) != e.reverseMappers.end()) {
                        needOutputAnn = false;
                        break;
                      }
                    }
                  }
                  if (!needOutputAnn) {
                    removeIsOutput(vd);
                    outputVarDecls(e, item, al);
                    vd->e(copy(e.cmap,al));
                  }
                }
                if (needOutputAnn) {
                  if (!isOutput(vd->flat())) {
                    GCLock lock;
                    if (vd->type().dim() == 0) {
                      vd->flat()->addAnnotation(constants().ann.output_var);
                    } else {
                      std::vector<Expression*> args(vd->type().dim());
                      for (unsigned int i=0; i<args.size(); i++) {
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
            topDown(ce, item->cast<FunctionI>()->ti());
            for (unsigned int i = item->cast<FunctionI>()->params().size(); i--;)
              topDown(ce, item->cast<FunctionI>()->params()[i]);
          }
            break;
          default:
            throw FlatteningError(e,item->loc(), "invalid item in output model");
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
        // Create output item for all variables defined at toplevel in the MiniZinc source
        GCLock lock;
        std::vector<Expression*> outputVars;
        for (unsigned int i=0; i<e.orig->size(); i++) {
          if (VarDeclI* vdi = (*e.orig)[i]->dyn_cast<VarDeclI>()) {
            VarDecl* vd = vdi->e();
            if (vd->type().isvar() && vd->e()==NULL) {
              std::ostringstream s;
              s << vd->id()->str().str() << " = ";
              if (vd->type().dim() > 0) {
                s << "array" << vd->type().dim() << "d(";
                for (unsigned int i=0; i<vd->type().dim(); i++) {
                  IntSetVal* idxset = eval_intset(vd->ti()->ranges()[i]->domain());
                  s << *idxset << ",";
                }
              }
              StringLit* sl = new StringLit(Location(),s.str());
              outputVars.push_back(sl);
              
              std::vector<Expression*> showArgs(1);
              showArgs[0] = vd->id();
              Call* show = new Call(Location(),constants().ids.show,showArgs);
              show->type(Type::parstring());
              FunctionI* fi = e.orig->matchFn(show);
              assert(fi);
              show->decl(fi);
              outputVars.push_back(show);
              std::string ends = vd->type().dim() > 0 ? ")" : "";
              ends += ";\n";
              StringLit* eol = new StringLit(Location(),ends);
              outputVars.push_back(eol);
            }
          }
        }
        OutputI* newOutputItem = new OutputI(Location(),new ArrayLit(Location(),outputVars));
        e.orig->addItem(newOutputItem);
        outputItem = copy(e.cmap, newOutputItem)->cast<OutputI>();
        e.output->addItem(outputItem);
      }
      
      class CollectFunctions : public EVisitor {
      public:
        EnvI& env;
        CollectFunctions(EnvI& env0) : env(env0) {}
        bool enter(Expression* e) {
          if (e->type().isvar()) {
            Type t = e->type();
            t.ti(Type::TI_PAR);
            e->type(t);
          }
          return true;
        }
        void vCall(Call& c) {
          std::vector<Type> tv(c.args().size());
          for (unsigned int i=c.args().size(); i--;) {
            tv[i] = c.args()[i]->type();
            tv[i].ti(Type::TI_PAR);
          }
          FunctionI* decl = env.output->matchFn(c.id(), tv);
          Type t;
          if (decl==NULL) {
            FunctionI* origdecl = env.orig->matchFn(c.id(), tv);
            if (origdecl == NULL || !origdecl->rtype(tv).ispar()) {
              throw FlatteningError(env,c.loc(),"function is used in output, par version needed");
            }
            if (!isBuiltin(origdecl)) {
              GCLock lock;
              decl = copy(env.cmap,origdecl)->cast<FunctionI>();
              CollectOccurrencesE ce(env.output_vo,decl);
              topDown(ce, decl->e());
              topDown(ce, decl->ti());
              for (unsigned int i = decl->params().size(); i--;)
                topDown(ce, decl->params()[i]);
              env.output->registerFn(decl);
              env.output->addItem(decl);
            } else {
              decl = origdecl;
            }
            c.decl(decl);
          }
        }
      } _cf(e);
      topDown(_cf, outputItem->e());
      
      class OV2 : public ItemVisitor {
      public:
        EnvI& env;
        OV2(EnvI& env0) : env(env0) {}
        void vVarDeclI(VarDeclI* vdi) {
          if (Expression* vd_e = env.cmap.find(vdi->e())) {
            VarDecl* vd = vd_e->cast<VarDecl>();
            GCLock lock;
            VarDeclI* vdi_copy = copy(env.cmap,vdi)->cast<VarDeclI>();
            Type t = vdi_copy->e()->ti()->type();
            t.ti(Type::TI_PAR);
            vdi_copy->e()->type(t);
            vdi_copy->e()->ti()->type(t);
            vdi_copy->e()->ti()->domain(NULL);
            vdi_copy->e()->flat(vdi->e()->flat());
            vdi_copy->e()->ann().clear();
            vdi_copy->e()->introduced(false);
            IdMap<KeepAlive>::iterator it;
            if (!vdi->e()->type().ispar()) {
              VarDecl* reallyFlat = vd->flat();
              while (reallyFlat!=reallyFlat->flat())
                reallyFlat=reallyFlat->flat();
              if (vd->flat()->e() && vd->flat()->e()->type().ispar()) {
                Expression* flate = copy(env.cmap,follow_id(reallyFlat->id()));
                outputVarDecls(env,vdi_copy,flate);
                vd->e(flate);
              } else if ( (it = env.reverseMappers.find(vd->id())) != env.reverseMappers.end()) {
                Call* rhs = copy(env.cmap,it->second())->cast<Call>();
                {
                  std::vector<Type> tv(rhs->args().size());
                  for (unsigned int i=rhs->args().size(); i--;) {
                    tv[i] = rhs->args()[i]->type();
                    tv[i].ti(Type::TI_PAR);
                  }
                  FunctionI* decl = env.output->matchFn(rhs->id(), tv);
                  if (decl==NULL) {
                    FunctionI* origdecl = env.orig->matchFn(rhs->id(), tv);
                    if (origdecl == NULL) {
                      throw FlatteningError(env,rhs->loc(),"function is used in output, par version needed");
                    }
                    if (!isBuiltin(origdecl)) {
                      decl = copy(env.cmap,origdecl)->cast<FunctionI>();
                      CollectOccurrencesE ce(env.output_vo,decl);
                      topDown(ce, decl->e());
                      topDown(ce, decl->ti());
                      for (unsigned int i = decl->params().size(); i--;)
                        topDown(ce, decl->params()[i]);
                      env.output->registerFn(decl);
                      env.output->addItem(decl);
                    } else {
                      decl = origdecl;
                    }
                    rhs->decl(decl);
                  }
                }
                outputVarDecls(env,vdi_copy,rhs);
                vd->e(rhs);
              } else if (cannotUseRHSForOutput(env,vd->e())) {
                // If the VarDecl does not have a usable right hand side, it needs to be
                // marked as output in the FlatZinc
                vd->e(NULL);
                assert(vdi->e()->flat());
                if (vdi->e()->type().dim() == 0) {
                  vdi->e()->flat()->addAnnotation(constants().ann.output_var);
                } else {
                  bool needOutputAnn = true;
                  if (reallyFlat->e() && reallyFlat->e()->isa<ArrayLit>()) {
                    ArrayLit* al = reallyFlat->e()->cast<ArrayLit>();
                    for (unsigned int i=0; i<al->v().size(); i++) {
                      if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                        if (env.reverseMappers.find(id) != env.reverseMappers.end()) {
                          needOutputAnn = false;
                          break;
                        }
                      }
                    }
                    if (!needOutputAnn) {
                      outputVarDecls(env, vdi_copy, al);
                      vd->e(copy(env.cmap,al));
                    }
                  }
                  if (needOutputAnn) {
                    std::vector<Expression*> args(vdi->e()->type().dim());
                    for (unsigned int i=0; i<args.size(); i++) {
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
              env.output_vo.add(reallyFlat, env.output->size());
            }
            env.output_vo.add(vdi_copy, env.output->size());
            CollectOccurrencesE ce(env.output_vo,vdi_copy);
            topDown(ce, vdi_copy->e());
            env.output->addItem(vdi_copy);
          }
        }
      } _ov2(e);
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
        IdMap<int>::iterator cur_idx = e.output_vo.idx.find(cur->id());
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
  
  bool checkParDomain(Expression* e, Expression* domain) {
    if (e->type()==Type::parint()) {
      IntSetVal* isv = eval_intset(domain);
      if (!isv->contains(eval_int(e)))
        return false;
    } else if (e->type()==Type::parfloat()) {
      BinOp* bo = domain->cast<BinOp>();
      assert(bo->op()==BOT_DOTDOT);
      FloatVal d_min = eval_float(bo->lhs());
      FloatVal d_max = eval_float(bo->rhs());
      FloatVal de = eval_float(e);
      if (de < d_min || de > d_max)
        return false;
    } else if (e->type()==Type::parsetint()) {
      IntSetVal* isv = eval_intset(domain);
      IntSetRanges ir(isv);
      IntSetVal* rsv = eval_intset(e);
      IntSetRanges rr(rsv);
      if (!Ranges::subset(rr, ir))
        return false;
    }
    return true;
  }
  
  void flatten(Env& e, FlatteningOptions opt) {
    EnvI& env = e.envi();
    
    bool hadSolveItem = false;
    // Flatten main model
    class FV : public ItemVisitor {
    public:
      EnvI& env;
      bool& hadSolveItem;
      FV(EnvI& env0, bool& hadSolveItem0) : env(env0), hadSolveItem(hadSolveItem0) {}
      void vVarDeclI(VarDeclI* v) {
        if (v->e()->type().isvar() || v->e()->type().isann()) {
          (void) flat_exp(env,Ctx(),v->e()->id(),NULL,constants().var_true);
        } else {
          if (v->e()->e()==NULL) {
            if (!v->e()->type().isann())
              throw EvalError(v->e()->loc(), "Undefined parameter", v->e()->id()->v());
          } else {
            CallStackItem csi(env,v->e());
            GCLock lock;
            Location v_loc = v->e()->e()->loc();
            v->e()->e(eval_par(v->e()->e()));
            if (v->e()->type().dim() > 0) {
              checkIndexSets(v->e(), v->e()->e());
              if (v->e()->ti()->domain() != NULL) {
                ArrayLit* al = eval_array_lit(v->e()->e());
                for (unsigned int i=0; i<al->v().size(); i++) {
                  if (!checkParDomain(al->v()[i], v->e()->ti()->domain())) {
                    throw EvalError(v_loc, "parameter value out of range");
                  }
                }
              }
            } else {
              if (v->e()->ti()->domain() != NULL) {
                if (!checkParDomain(v->e()->e(), v->e()->ti()->domain())) {
                  throw EvalError(v_loc, "parameter value out of range");
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
    } _fv(env,hadSolveItem);
    iterItems<FV>(_fv,e.model());

    if (!hadSolveItem) {
      e.envi().errorStack.clear();
      Location modelLoc;
      modelLoc.filename = e.model()->filepath();
      throw FlatteningError(e.envi(),modelLoc, "Model does not have a solve item");
    }
    
    // Create output model
    if (opt.keepOutputInFzn) {
      copyOutput(env);
    } else {
      createOutput(env);
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
      FunctionI* fi = env.orig->matchFn(constants().ids.int_.lin_eq, int_lin_eq_t);
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
      for (int i=startItem; i<=endItem; i++) {
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
                CollectDecls cd(env.vo,deletedVarDecls,vdi);
                topDown(cd,vdi->e()->e());
                vdi->remove();
                keptVariable = false;
              } else {
                vdi->e()->e(NULL);
              }
              env.flat_addItem(ci);
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
        if (vdi && keptVariable &&
            vdi->e()->type().isint() && vdi->e()->type().isvar() &&
            vdi->e()->ti()->domain() != NULL) {

          GCLock lock;
          IntSetVal* dom = eval_intset(vdi->e()->ti()->domain());

          bool needRangeDomain = opt.onlyRangeDomains;
          if (!needRangeDomain && dom->size() > 0) {
            if (dom->min(0).isMinusInfinity() || dom->max(dom->size()-1).isPlusInfinity())
              needRangeDomain = true;
          }
          
          if (needRangeDomain) {
            if (dom->min(0).isMinusInfinity() || dom->max(dom->size()-1).isPlusInfinity()) {
              TypeInst* nti = copy(vdi->e()->ti())->cast<TypeInst>();
              nti->domain(NULL);
              vdi->e()->ti(nti);
              if (dom->min(0).isFinite()) {
                std::vector<Expression*> args(2);
                args[0] = new IntLit(Location(),dom->min(0));
                args[1] = vdi->e()->id();
                Call* call = new Call(Location(),constants().ids.int_.le,args);
                call->type(Type::varbool());
                call->decl(env.orig->matchFn(call));
                (void) flat_exp(env, Ctx(), call, constants().var_true, constants().var_true);
              } else if (dom->max(dom->size()-1).isFinite()) {
                std::vector<Expression*> args(2);
                args[0] = vdi->e()->id();
                args[1] = new IntLit(Location(),dom->max(dom->size()-1));
                Call* call = new Call(Location(),constants().ids.int_.le,args);
                call->type(Type::varbool());
                call->decl(env.orig->matchFn(call));
                (void) flat_exp(env, Ctx(), call, constants().var_true, constants().var_true);
              }
            } else if (dom->size() > 1) {
              SetLit* newDom = new SetLit(Location(),IntSetVal::a(dom->min(0),dom->max(dom->size()-1)));
              TypeInst* nti = copy(vdi->e()->ti())->cast<TypeInst>();
              nti->domain(newDom);
              vdi->e()->ti(nti);
            }
            if (dom->size() > 1) {
              IntVal firstHole = dom->max(0)+1;
              IntSetRanges domr(dom);
              ++domr;
              for (; domr(); ++domr) {
                for (IntVal i=firstHole; i<domr.min(); i++) {
                  std::vector<Expression*> args(2);
                  args[0] = vdi->e()->id();
                  args[1] = new IntLit(Location(),i);
                  Call* call = new Call(Location(),constants().ids.int_.ne,args);
                  call->type(Type::varbool());
                  call->decl(env.orig->matchFn(call));
                  (void) flat_exp(env, Ctx(), call, constants().var_true, constants().var_true);
                  firstHole = domr.max().plus(1);
                }
              }
            }
          }
        }
      }
      while (!deletedVarDecls.empty()) {
        VarDecl* cur = deletedVarDecls.back(); deletedVarDecls.pop_back();
        if (env.vo.occurrences(cur) == 0 && !isOutput(cur)) {
          IdMap<int>::iterator cur_idx = env.vo.idx.find(cur->id());
          if (cur_idx != env.vo.idx.end() && !m[cur_idx->second]->removed()) {
            CollectDecls cd(env.vo,deletedVarDecls,m[cur_idx->second]->cast<VarDeclI>());
            topDown(cd,cur->e());
            m[cur_idx->second]->remove();
          }
        }
      }
      
      // rewrite some constraints if there are redefinitions
      for (int i=startItem; i<=endItem; i++) {
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
                      cid = env.reifyId(c->id());
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
                vd->addAnnotation(constants().ann.is_defined_var);
                nc->addAnnotation(definesVarAnn(vd->id()));
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
          IdMap<int>::iterator cur_idx = env.vo.idx.find(cur->id());
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
        IdMap<KeepAlive>::iterator it;
        GCLock lock;
        if (!vdi->e()->type().ispar() &&
            vdi->e()->e()==NULL &&
            (it = env.reverseMappers.find(vdi->e()->id())) != env.reverseMappers.end()) {
          GCLock lock;
          Call* rhs = copy(env.cmap,it->second())->cast<Call>();
          std::vector<Type> tv(rhs->args().size());
          for (unsigned int i=rhs->args().size(); i--;) {
            tv[i] = rhs->args()[i]->type();
            tv[i].ti(Type::TI_PAR);
          }
          FunctionI* decl = env.output->matchFn(rhs->id(), tv);
          Type t;
          if (decl==NULL) {
            FunctionI* origdecl = env.orig->matchFn(rhs->id(), tv);
            if (origdecl == NULL) {
              throw FlatteningError(env,rhs->loc(),"function is used in output, par version needed");
            }
            if (!isBuiltin(origdecl)) {
              decl = copy(env.cmap,origdecl)->cast<FunctionI>();
              CollectOccurrencesE ce(env.output_vo,decl);
              topDown(ce, decl->e());
              topDown(ce, decl->ti());
              for (unsigned int i = decl->params().size(); i--;)
                topDown(ce, decl->params()[i]);
              env.output->registerFn(decl);
              env.output->addItem(decl);
            } else {
              decl = origdecl;
            }
            rhs->decl(decl);
          }
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
            ci->remove();
          }
        }
      }
    }
    
    if (!opt.keepOutputInFzn) {
      createOutput(env);
    }

  }
  
  void oldflatzinc(Env& e) {
    Model* m = e.flat();
    for (unsigned int i=0; i<m->size(); i++) {
      Item* item = (*m)[i];
      if (item->isa<VarDeclI>() &&
          (item->cast<VarDeclI>()->e()->type().ot() == Type::OT_OPTIONAL ||
           item->cast<VarDeclI>()->e()->type().bt() == Type::BT_ANN) ) {
            item->remove();
          }
    }

    m->compact();
    
    int msize = m->size();
    UNORDERED_NAMESPACE::unordered_set<Item*> globals;
    std::vector<int> declsWithIds;
    for (int i=0; i<msize; i++) {
      if ((*m)[i]->removed())
        continue;
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
        vd->ann().remove(constants().ann.promise_total);
        
        if (vd->e() && vd->e()->isa<Id>()) {
          declsWithIds.push_back(i);
          vdi->e()->payload(-static_cast<int>(i)-1);
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
                ASTString cid;
                std::vector<Expression*> args;
                if (vcc->id() == constants().ids.exists) {
                  cid = constants().ids.array_bool_or;
                  args.push_back(vcc->args()[0]);
                  args.push_back(constants().lit_true);
                } else if (vcc->id() == constants().ids.forall) {
                  cid = constants().ids.array_bool_and;
                  args.push_back(vcc->args()[0]);
                  args.push_back(constants().lit_true);
                } else if (vcc->id() == constants().ids.clause) {
                  cid = constants().ids.bool_clause;
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
                if (id->decl()->ti()->domain() != constants().lit_true) {
                  std::vector<Expression*> args(2);
                  args[0] = id;
                  args[1] = constants().lit_true;
                  GCLock lock;
                  ve = new Call(Location(),constants().ids.bool_eq,args);
                } else {
                  ve = constants().lit_true;
                }
              }
              if (ve != constants().lit_true) {
                e.envi().flat_addItem(new ConstraintI(Location(),ve));
              }
            }
          } else {
            if (vd->e() != NULL) {
              if (vd->e()->eid()==Expression::E_CALL) {
                const Call* c = vd->e()->cast<Call>();
                GCLock lock;
                vd->e(NULL);
                vd->addAnnotation(constants().ann.is_defined_var);
                ASTString cid;
                if (c->id() == constants().ids.exists) {
                  cid = constants().ids.array_bool_or;
                } else if (c->id() == constants().ids.forall) {
                  cid = constants().ids.array_bool_and;
                } else if (c->id() == constants().ids.clause) {
                  cid = constants().ids.bool_clause_reif;
                } else {
                  cid = e.envi().reifyId(c->id());
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
        } else if (vd->type().isvar() && vd->type().dim()==0) {
          if (vd->e() != NULL) {
            if (const Call* cc = vd->e()->dyn_cast<Call>()) {
              vd->e(NULL);
              vd->addAnnotation(constants().ann.is_defined_var);
              std::vector<Expression*> args(cc->args().size());
              ASTString cid;
              if (cc->id() == constants().ids.lin_exp) {
                ArrayLit* le_c = follow_id(cc->args()[0])->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),nc.begin());
                if (le_c->type().bt()==Type::BT_INT) {
                  cid = constants().ids.int_.lin_eq;
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
                  cid = constants().ids.float_.lin_eq;
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
                  cid = constants().ids.set_card;
                } else {
                  cid = cc->id();
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
                     vd->e()->eid() == Expression::E_FLOATLIT ||
                     vd->e()->eid() == Expression::E_BOOLLIT ||
                     vd->e()->eid() == Expression::E_SETLIT);
              
            }
          }
        } else if (vd->type().dim() > 0) {
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
          } else if (vc->id() == constants().ids.bool_xor && vc->args().size()==2) {
            GCLock lock;
            std::vector<Expression*> args(3);
            args[0] = vc->args()[0];
            args[1] = vc->args()[1];
            args[2] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
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
          VarDecl* constantobj = new VarDecl(Location(),ti,e.envi().genId(),si->e());
          si->e(constantobj->id());
          e.envi().flat_addItem(new VarDeclI(Location(),constantobj));
        }
      }
    }
    
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
    
    m->compact();
    
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
    std::stable_sort(m->begin(),m->end(),_cmp);

  }

}
