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

  VarDecl* newVarDecl(EnvI& env, Ctx ctx, TypeInst* ti, Id* id, VarDecl* origVd, Expression* rhs) {
    VarDecl* vd;
    if (id == NULL)
      vd = new VarDecl(rhs ? rhs->loc().introduce() : Location().introduce(), ti, env.genId());
    else
      vd = new VarDecl(rhs ? rhs->loc().introduce() : Location().introduce(), ti, id);
    if (vd->e()) {
      bind(env, ctx, vd, rhs);
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

    if (origVd) {
      for (ExpressionSetIter it = origVd->ann().begin(); it != origVd->ann().end(); ++it) {
        EE ee_ann = flat_exp(env, Ctx(), *it, NULL, constants().var_true);
        vd->addAnnotation(ee_ann.r());
      }
    }
    
    VarDeclI* ni = new VarDeclI(Location().introduce(),vd);
    env.flat_addItem(ni);
    EE ee(vd,NULL);
    env.map_insert(vd->id(),ee);
    
    return vd;
  }

#define MZN_FILL_REIFY_MAP(T,ID) reifyMap.insert(std::pair<ASTString,ASTString>(constants().ids.T.ID,constants().ids.T ## reif.ID));

  EnvI::EnvI(Model* orig0) : orig(orig0), output(new Model), ignorePartial(false), maxCallStack(0), collect_vardecls(false), in_redundant_constraint(0), in_maybe_partial(0), collect_symmetry_vars(true), num_symmetry_predicates(0), flatten_symmetry_predicates(true), _flat(new Model), _failed(false), ids(0) {
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
      static std::string d(const WW& ee) {
        std::ostringstream oss;
        oss << ee.r() << " " << ee.b();
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
        toAnnotate = vd->e()->e();
        vo.add(vd, _flat->size()-1);
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
  
  void EnvI::flat_removeItem(MiniZinc::Item* i) {
    i->remove();
  }
  void EnvI::flat_removeItem(int i) {
    (*_flat)[i]->remove();
  }
  
  void EnvI::fail(void) {
    if (!_failed) {
      addWarning("model inconsistency detected");
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
  
  void EnvI::collectVarDecls(bool b) {
    collect_vardecls = b;
  }

  void EnvI::collectSymmetryVars(bool b) {
    collect_symmetry_vars = b;
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
  ASTString EnvI::symOrdId(const ASTString& id) {
    return id.str()+"_ord";
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
  
  Env::Env(Model* m) : e(new EnvI(m)) {}
  Env::~Env(void) {
    delete e;
  }
  
  Model*
  Env::model(void) { return e->orig; }
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
      if (e->loc().is_introduced)
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
      ASTString newloc_f = e->loc().filename;
      if (e->loc().is_introduced)
        continue;
      int newloc_l = e->loc().first_line;
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
              dims = c->args()[0]->cast<ArrayLit>();
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
            s << "array" << dims->v().size() << "d(";
            for (unsigned int i=0; i<dims->v().size(); i++) {
              IntSetVal* idxset = eval_intset(envi,dims->v()[i]);
              s << *idxset << ",";
            }
          }
          StringLit* sl = new StringLit(Location().introduce(),s.str());
          outputVars.push_back(sl);

          std::vector<Expression*> showArgs(1);
          showArgs[0] = vd->id();
          Call* show = new Call(Location().introduce(),constants().ids.show,showArgs);
          show->type(Type::parstring());
          FunctionI* fi = _flat->matchFn(envi, show);
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
    for (int i=0; i<al->v().size(); i++) {
      std::string s = eval_string(*this, al->v()[i]);
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

  bool requiresGlobalOrder(FunctionI* fi) {
    return fi->ann().contains(constants().ann.symmetry);
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
      Expression* e0 = c->args()[0];
      Expression* e1 = c->args()[1];
      if (e0->type().ispar() && e1->isa<Id>()) {
        // greater than
        Id* id = e1->cast<Id>();
        IntVal lb = eval_int(env,e0);
        if (id->decl()->ti()->domain()) {
          IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
          if (domain->min() >= lb)
            return false;
          IntSetRanges dr(domain);
          Ranges::Const cr(lb,IntVal::infinity());
          Ranges::Inter<IntSetRanges,Ranges::Const> i(dr,cr);
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
          IntSetRanges dr(domain);
          Ranges::Const cr(-IntVal::infinity(), ub);
          Ranges::Inter<IntSetRanges,Ranges::Const> i(dr,cr);
          IntSetVal* newibv = IntSetVal::ai(i);
          id->decl()->ti()->domain(new SetLit(Location().introduce(), newibv));
          id->decl()->ti()->setComputedDomain(false);
        } else {
          id->decl()->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(-IntVal::infinity(), ub)));
        }
      }
    } else if (c->id()==constants().ids.int_.lin_le) {
      ArrayLit* al_c = follow_id(c->args()[0])->cast<ArrayLit>();
      if (al_c->v().size()==1) {
        ArrayLit* al_x = follow_id(c->args()[1])->cast<ArrayLit>();
        IntVal coeff = eval_int(env,al_c->v()[0]);
        IntVal y = eval_int(env,c->args()[2]);
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
        if (Id* id = al_x->v()[0]->dyn_cast<Id>()) {
          if (id->decl()->ti()->domain()) {
            IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
            if (domain->max() <= ub && domain->min() >= lb)
              return false;
            IntSetRanges dr(domain);
            Ranges::Const cr(lb, ub);
            Ranges::Inter<IntSetRanges,Ranges::Const> i(dr,cr);
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
                c->decl(env.orig->matchFn(env,c));
                c->type(c->decl()->rtype(env,args));
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
                c->decl(env.orig->matchFn(env,c));
                c->type(c->decl()->rtype(env,args));
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
            if (al->type().bt()==Type::BT_ANN || al->v().size() <= 10)
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
            if (al->v().size() > 0 && al->v()[0]->type().isint()) {
              IntVal min = IntVal::infinity();
              IntVal max = -IntVal::infinity();
              for (unsigned int i=0; i<al->v().size(); i++) {
                IntBounds ib = compute_int_bounds(env,al->v()[i]);
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
              BinOp* ibv = LinearTraits<FloatLit>::intersect_domain(NULL, fb.l, fb.u);
              if (fb.valid) {
                Id* id = vd->id();
                while (id != NULL) {
                  if (id->decl()->ti()->domain()) {
                    BinOp* domain = Expression::cast<BinOp>(id->decl()->ti()->domain());
                    BinOp* ndomain = LinearTraits<FloatLit>::intersect_domain(domain, fb.l, fb.u);
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
                    id->decl()->ti()->domain(ibv);
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
            if (e==NULL || eval_bool(env,e)) {
              vd->e(constants().lit_true);
            } else {
              vd->e(constants().lit_false);
            }
            if (vd->ti()->domain()) {
              if (vd->ti()->domain() != vd->e()) {
                env.fail();
                return vd->id();
              }
            } else {
              vd->ti()->domain(vd->e());
              vd->ti()->setComputedDomain(true);
            }
            std::vector<Expression*> args(2);
            args[0] = vd->id();
            args[1] = vd->e();
            Call* c = new Call(Location().introduce(),constants().ids.bool_eq,args);
            c->decl(env.orig->matchFn(env,c));
            c->type(c->decl()->rtype(env,args));
            if (c->decl()->e()) {
              flat_exp(env, Ctx(), c, constants().var_true, constants().var_true);
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
                  for (unsigned int i=0; i<al->v().size(); i++) {
                    if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                      VarDecl* vdi = id->decl();
                      if (vdi->ti()->domain()==NULL) {
                        vdi->ti()->domain(vd->ti()->domain());
                      } else {
                        IntSetVal* vdi_dom = eval_intset(env, vdi->ti()->domain());
                        IntSetRanges isvr(isv);
                        IntSetRanges vdi_domr(vdi_dom);
                        Ranges::Inter<IntSetRanges, IntSetRanges> inter(isvr,vdi_domr);
                        IntSetVal* newdom = IntSetVal::ai(inter);
                        if (newdom->size()==0) {
                          env.fail();
                        } else {
                          vdi->ti()->domain(new SetLit(Location().introduce(),newdom));
                        }
                      }
                    }
                  }
                } else if (e->type().bt()==Type::BT_FLOAT) {
                  Expression* floatDomain = follow_id_to_value(vd->ti()->domain());
                  FloatVal f_min = eval_float(env, floatDomain->cast<BinOp>()->lhs());
                  FloatVal f_max = eval_float(env, floatDomain->cast<BinOp>()->rhs());
                  for (unsigned int i=0; i<al->v().size(); i++) {
                    if (Id* id = al->v()[i]->dyn_cast<Id>()) {
                      VarDecl* vdi = id->decl();
                      if (vdi->ti()->domain()==NULL) {
                        vdi->ti()->domain(vd->ti()->domain());
                      } else {
                        BinOp* vdidomain = follow_id_to_value(vdi->ti()->domain())->cast<BinOp>();
                        BinOp* ndomain = LinearTraits<FloatLit>::intersect_domain(vdidomain, f_min, f_max);
                        if (ndomain != vdi->ti()->domain()) {
                          vdi->ti()->domain(ndomain);
                        }
                      }
                    }
                  }
                }
              }
            } else if (Id* e_id = e->dyn_cast<Id>()) {
              if (e_id == vd->id()) {
                ret = vd->id();
              } else {
                ASTString cid;
                if (e->type().isint()) {
                  cid = constants().ids.int_.eq;
                } else if (e->type().isbool()) {
                  cid = constants().ids.bool_eq;
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
                  c->decl(env.orig->matchFn(env,c));
                  c->type(c->decl()->rtype(env,args));
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
                    ArrayLit* al = eval_array_lit(env, call->args()[1]);
                    if (al->v().size()==1) {
                      IntBounds check_zeroone = compute_int_bounds(env, al->v()[0]);
                      if (check_zeroone.l==0 && check_zeroone.u==1) {
                        ArrayLit* coeffs = eval_array_lit(env, call->args()[0]);
                        std::vector<IntVal> newdom(2);
                        newdom[0] = 0;
                        newdom[1] = eval_int(env, coeffs->v()[0])+eval_int(env, call->args()[2]);
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
                vd->ti()->domain(new SetLit(Location().introduce(),ibv));
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
                  c->decl(env.orig->matchFn(env,c));
                  c->type(c->decl()->rtype(env,args));
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
              Call* c = new Call(Location().introduce(),cid,args);
              c->decl(env.orig->matchFn(env,c));
              c->type(c->decl()->rtype(env,args));
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
                ArrayLit* le_c = follow_id(c->args()[0])->cast<ArrayLit>();
                std::vector<Expression*> ncoeff(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),ncoeff.begin());
                ncoeff.push_back(IntLit::a(-1));
                args.push_back(new ArrayLit(Location().introduce(),ncoeff));
                args[0]->type(le_c->type());
                ArrayLit* le_x = follow_id(c->args()[1])->cast<ArrayLit>();
                std::vector<Expression*> nx(le_x->v().size());
                std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                nx.push_back(vd->id());
                args.push_back(new ArrayLit(Location().introduce(),nx));
                args[1]->type(le_x->type());
                args.push_back(c->args()[2]);
                nc = new Call(c->loc().introduce(), constants().ids.lin_exp, args);
                nc->decl(env.orig->matchFn(env,nc));
                if (nc->decl() == NULL) {
                  throw InternalError("undeclared function or predicate "
                                      +nc->id().str());
                }
                nc->type(nc->decl()->rtype(env,args));
                BinOp* bop = new BinOp(nc->loc(), nc, BOT_EQ, IntLit::a(0));
                bop->type(Type::varbool());
                flat_exp(env, Ctx(), bop, constants().var_true, constants().var_true);
                return vd->id();
              } else {
                args.resize(c->args().size());
                std::copy(c->args().begin(),c->args().end(),args.begin());
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
              nc->decl(env.orig->matchFn(env,nc));
              if (nc->decl() == NULL) {
                throw InternalError("undeclared function or predicate "
                                    +nc->id().str());
              }
              nc->type(nc->decl()->rtype(env,args));
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
          Call* ret = new Call(Location().introduce(),constants().ids.forall,args);
          ret->decl(env.orig->matchFn(env,ret));
          ret->type(ret->decl()->rtype(env,args));
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
          Call* ret = new Call(Location().introduce().introduce(),constants().ids.exists,args);
          ret->decl(env.orig->matchFn(env, ret));
          ret->type(ret->decl()->rtype(env, args));
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
    return (decl->loc().filename == "builtins.mzn" ||
            decl->loc().filename.endsWith("/builtins.mzn") ||
            decl->loc().filename == "stdlib.mzn" ||
            decl->loc().filename.endsWith("/stdlib.mzn") ||
            decl->loc().filename == "flatzinc_builtins.mzn" ||
            decl->loc().filename.endsWith("/flatzinc_builtins.mzn"));
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
      tt = args[1]->type();
      tt.dim(0);
      c->decl(env.orig->matchFn(env, c));
      if (c->decl()==NULL) {
        throw FlatteningError(env,c->loc(), "cannot find matching declaration");
      }
      c->type(c->decl()->rtype(env, args));
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
              clause->decl(env.orig->matchFn(env, clause));
              clause->type(clause->decl()->rtype(env, clauseArgs));
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
            clause->decl(env.orig->matchFn(env, clause));
            clause->type(clause->decl()->rtype(env, clauseArgs));
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
          Ranges::Union<IntSetRanges, IntSetRanges> u(i0,i1);
          isv = IntSetVal::ai(u);
        }
        if (r) {
          IntSetVal* orig_r_bounds = compute_intset_bounds(env,r->id());
          if (orig_r_bounds) {
            IntSetRanges i0(isv);
            IntSetRanges i1(orig_r_bounds);
            Ranges::Inter<IntSetRanges, IntSetRanges> inter(i0,i1);
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
        clause->decl(env.orig->matchFn(env, clause));
        clause->type(clause->decl()->rtype(env, clauseArgs));
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
          clause->decl(env.orig->matchFn(env, clause));
          clause->type(clause->decl()->rtype(env, clauseArgs));
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
          clause->decl(env.orig->matchFn(env, clause));
          clause->type(clause->decl()->rtype(env, clauseArgs));
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
        d += sign*l->v();
      } else if (le[i]->isa<Id>()) {
        coeffv.push_back(sign);
        alv.push_back(le[i]);
      } else if (Call* sc = le[i]->dyn_cast<Call>()) {
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(env,sc->args()[0]);
        ArrayLit* sc_al = eval_array_lit(env,sc->args()[1]);
        d += sign*LinearTraits<Lit>::eval(env,sc->args()[2]);
        for (unsigned int j=0; j<sc_coeff->v().size(); j++) {
          coeffv.push_back(sign*LinearTraits<Lit>::eval(env,sc_coeff->v()[j]));
          alv.push_back(sc_al->v()[j]);
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
                c->decl(env.orig->matchFn(env, c));
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
        Val resultCoeff;
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
      al = new ArrayLit(al->loc(),al->v());
      al->type(alt);
      al_ka = al;
    }
    Val d = (cid == constants().ids.sum ? Val(0) : LinearTraits<Lit>::eval(env,args_ee[2].r()));
    
    std::vector<Val> c_coeff(al->v().size());
    if (cid==constants().ids.sum) {
      for (unsigned int i=al->v().size(); i--;)
        c_coeff[i] = 1;
    } else {
      EE flat_coeff = flat_exp(env,nctx,args_ee[0].r(),NULL,NULL);
      ArrayLit* coeff = follow_id(flat_coeff.r())->template cast<ArrayLit>();
      for (unsigned int i=coeff->v().size(); i--;)
        c_coeff[i] = LinearTraits<Lit>::eval(env,coeff->v()[i]);
    }
    cid = constants().ids.lin_exp;
    std::vector<Val> coeffv;
    std::vector<KeepAlive> alv;
    for (unsigned int i=0; i<al->v().size(); i++) {
      if (Call* sc = same_call(al->v()[i],cid)) {
        if (VarDecl* alvi_decl = follow_id_to_decl(al->v()[i])->dyn_cast<VarDecl>()) {
          if (alvi_decl->ti()->domain()) {
            typename LinearTraits<Lit>::Domain sc_dom = LinearTraits<Lit>::eval_domain(env,alvi_decl->ti()->domain());
            typename LinearTraits<Lit>::Bounds sc_bounds = LinearTraits<Lit>::compute_bounds(env,sc);
            if (LinearTraits<Lit>::domain_tighter(sc_dom, sc_bounds)) {
              coeffv.push_back(c_coeff[i]);
              alv.push_back(al->v()[i]);
              continue;
            }
          }
        }
        
        Val cd = c_coeff[i];
        GCLock lock;
        ArrayLit* sc_coeff = eval_array_lit(env,sc->args()[0]);
        ArrayLit* sc_al = eval_array_lit(env,sc->args()[1]);
        Val sc_d = LinearTraits<Lit>::eval(env,sc->args()[2]);
        assert(sc_coeff->v().size() == sc_al->v().size());
        for (unsigned int j=0; j<sc_coeff->v().size(); j++) {
          coeffv.push_back(cd*LinearTraits<Lit>::eval(env,sc_coeff->v()[j]));
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
        ArrayLit* al = eval_array_lit(env,rd);
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
      ArrayLit* al = new ArrayLit(Location().introduce(), output_pos);
      Type al_t = bo->type();
      al_t.dim(1);
      al->type(al_t);
      c_args[0] = al;
      c = new Call(bo->loc().introduce(), bot==BOT_AND ? constants().ids.forall : constants().ids.exists, c_args);
    } else {
      ArrayLit* al_pos = new ArrayLit(Location().introduce(), output_pos);
      Type al_t = bo->type();
      al_t.dim(1);
      al_pos->type(al_t);
      c_args[0] = al_pos;
      if (output_neg.size() > 0) {
        ArrayLit* al_neg = new ArrayLit(Location().introduce(), output_neg);
        al_neg->type(al_t);
        c_args.push_back(al_neg);
      }
      c = new Call(bo->loc().introduce(), output_neg.empty() ? constants().ids.exists : constants().ids.clause, c_args);
    }
    c->decl(env.orig->matchFn(env, c));
    assert(c->decl());
    Type t = c->decl()->rtype(env, c_args);
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
        if (sl->isv())
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
        std::vector<Expression*> es(al->v().size());
        GCLock lock;
        for (unsigned int i=0; i<al->v().size(); i++) {
          es[i] = flat_cv_exp(env, ctx, al->v()[i])();
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
        std::vector<Expression*> args(c->args().size());
        GCLock lock;
        for (unsigned int i=0; i<c->args().size(); i++) {
          args[i] = flat_cv_exp(env, ctx, c->args()[i])();
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
  
  EE flat_exp(EnvI& env, Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    if (e==NULL) return EE();
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
            if (al->v().size()==0) {
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
          if (al->v().size()==0 || (r && r->e()==NULL)) {
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
          FunctionI* fi = env.orig->matchFn(env, cc->id(),args);
          if (fi==NULL) {
            throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
          }
          assert(fi);
          assert(fi->rtype(env, args).isSubtypeOf(cc->type()));
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
            idxset->decl(env.orig->matchFn(env, idxset));
            idxset->type(idxset->decl()->rtype(env, idxsetargs));
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
              TypeInst* vti = new TypeInst(Location().introduce(),tt,vd->ti()->domain());
              VarDecl* nvd = newVarDecl(env, Ctx(), vti, NULL, vd, NULL);
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
                if (vdea && vdea->v().size()==0) {
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
                if (vda && vda->v().size()==0) {
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
        VarDecl* vd = new VarDecl(Location().introduce(), new TypeInst(Location().introduce(), av->type()),
                                  env.genId());
        ret = flat_exp(env,Ctx(),vd,NULL,constants().var_true);
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
              al = follow_id(id)->cast<ArrayLit>();
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
            std::vector<Expression*> composed_e(al_inner->v().size());
            for (unsigned int i=0; i<al_inner->v().size(); i++) {
              GCLock lock;
              IntVal inner_idx = eval_int(env, al_inner->v()[i]);
              if (inner_idx < al->min(0) || inner_idx > al->max(0))
                goto flatten_arrayaccess;
              composed_e[i] = al->v()[inner_idx.toInt()-al->min(0)];
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
            FunctionI* fi = env.orig->matchFn(env,cc->id(),args);
            if (fi==NULL) {
              throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
            }
            assert(fi);
            assert(fi->rtype(env,args).isSubtypeOf(cc->type()));
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
          std::vector<Expression*> where;
          GCLock lock;
          for (int i=0; i<c->n_generators(); i++) {
            if (c->in(i)->type().isvar() && c->in(i)->type().dim()==0) {
              std::vector<Expression*> args(1);
              args[0] = c->in(i);
              Call* ub = new Call(Location().introduce(),"ub",args);
              ub->type(Type::parsetint());
              ub->decl(env.orig->matchFn(env, ub));
              in[i] = ub;
              for (int j=0; j<c->n_decls(i); j++) {
                BinOp* bo = new BinOp(Location().introduce(),c->decl(i,j)->id(), BOT_IN, c->in(i));
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
              ArrayLit* al = new ArrayLit(Location().introduce(), where);
              al->type(Type::varbool(1));
              std::vector<Expression*> args(1);
              args[0] = al;
              Call* forall = new Call(Location().introduce(), constants().ids.forall, args);
              forall->type(Type::varbool());
              forall->decl(env.orig->matchFn(env, forall));
              cond = forall;
            } else {
              cond = where[0];
            }
            

            Expression* r_bounds = NULL;
            if (c->e()->type().bt()==Type::BT_INT && c->e()->type().dim() == 0) {
              std::vector<Expression*> ubargs(1);
              ubargs[0] = c->e();
              if (c->e()->type().st()==Type::ST_SET) {
                Call* bc = new Call(Location().introduce(),"ub",ubargs);
                bc->type(Type::parsetint());
                bc->decl(env.orig->matchFn(env, bc));
                r_bounds = bc;
              } else {
                Call* lbc = new Call(Location().introduce(),"lb",ubargs);
                lbc->type(Type::parint());
                lbc->decl(env.orig->matchFn(env, lbc));
                Call* ubc = new Call(Location().introduce(),"ub",ubargs);
                ubc->type(Type::parint());
                ubc->decl(env.orig->matchFn(env, ubc));
                r_bounds = new BinOp(Location().introduce(),lbc,BOT_DOTDOT,ubc);
                r_bounds->type(Type::parsetint());
              }
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
            absent_r->decl(env.orig->matchFn(env, absent_r));
            let_exprs[2] = new BinOp(Location().introduce(),cond,BOT_OR,absent_r);
            let_exprs[2]->type(Type::varbool());
            let_exprs[2]->addAnnotation(constants().ann.promise_total);
            Let* let = new Let(Location().introduce(), let_exprs, r->id());
            let->type(r->type());
            Comprehension* nc = new Comprehension(c->loc(),let,gs,c->set());
            nc->type(c->type());
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

          std::vector<Expression*> args(c->args().size());
          for (unsigned int i=0; i<c->args().size(); i++) {
            Id* idi = c->args()[i]->dyn_cast<Id>();
            if (idi==NULL)
              throw EvalError(env, c->args()[i]->loc(), "Reverse mapper calls require identifiers as arguments");
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
            cr->decl(env.orig->matchFn(env, cr));
            cr->type(cr->decl()->rtype(env, args));
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
              Expression* res = eval_par(env,parbo);
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
              if (FunctionI* fi = env.orig->matchFn(env,cc->id(),args)) {
                assert(cc->type() == fi->rtype(env,args));
                cc->decl(fi);
                cc->type(cc->decl()->rtype(env,args));
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
              std::vector<Expression*> todo;
              todo.push_back(boe0);
              todo.push_back(boe1);
              while (!todo.empty()) {
                Expression* e_todo = todo.back();
                todo.pop_back();
                BinOp* e_bo = e_todo->dyn_cast<BinOp>();
                if (e_bo && e_bo->op()==BOT_AND) {
                  todo.push_back(e_bo->lhs());
                  todo.push_back(e_bo->rhs());
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
            c->decl(env.orig->matchFn(env,c));
            if (c->decl()==NULL) {
              throw FlatteningError(env,c->loc(), "cannot find matching declaration");
            }
            c->type(c->decl()->rtype(env,args));
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
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
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
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = -ctx0.b;
                ctx1.b = +ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
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
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
                ctx0.i = +ctx0.i;
                ctx1.i = -ctx1.i;
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
                ctx0.i = -ctx0.i;
                ctx1.i = +ctx1.i;
              }
            } else {
              if (boe0->type().bt()==Type::BT_BOOL) {
                ctx0.b = +ctx0.b;
                ctx1.b = -ctx1.b;
              } else if (boe0->type().bt()==Type::BT_INT) {
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
            
            if (ctx.b==C_ROOT && r==constants().var_true && e1.r()->type().ispar() &&
                e0.r()->isa<Id>() && (bot==BOT_IN || bot==BOT_SUBSET) ) {
              VarDecl* vd = e0.r()->cast<Id>()->decl();
              if (vd->ti()->domain()==NULL) {
                vd->ti()->domain(e1.r());
              } else {
                GCLock lock;
                IntSetVal* newdom = eval_intset(env,e1.r());
                Id* id = vd->id();
                while (id != NULL) {
                  bool changeDom = false;
                  if (id->decl()->ti()->domain()) {
                    IntSetVal* domain = eval_intset(env,id->decl()->ti()->domain());
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
                    env.fail();
                  } else if (changeDom) {
                    id->decl()->ti()->setComputedDomain(false);
                    id->decl()->ti()->domain(new SetLit(Location().introduce(),newdom));
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
              if (boe1->type().isint() && !boe1->type().isopt()) {
                le1 = get_linexp<IntLit>(e1.r());
              } else if (boe1->type().isfloat() && !boe1->type().isopt()) {
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
              Call* cc = new Call(e->loc().introduce(),callid,args_e);
              cc->decl(env.orig->matchFn(env,cc->id(),args_e));
              if (cc->decl()==NULL) {
                throw FlatteningError(env,cc->loc(), "cannot find matching declaration");
              }
              cc->type(cc->decl()->rtype(env,args_e));

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
        FunctionI* decl = env.orig->matchFn(env,c);
        if (decl == NULL) {
          throw InternalError("undeclared function or predicate "
                              +c->id().str());
        }

        if (decl->params().size()==1) {
          if (Call* call_body = Expression::dyn_cast<Call>(decl->e())) {
            if (call_body->args().size()==1 && Expression::equal(call_body->args()[0],decl->params()[0]->id())) {
              // Symmetry predicates are handled later on
              if (!c->type().isbool() || !requiresGlobalOrder(decl) || (!env.collect_symmetry_vars && !(env.num_symmetry_predicates > 1 && env.flatten_symmetry_predicates))) {
                c->id(call_body->id());
                c->decl(call_body->decl());
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
            if (cid == constants().ids.assert && c->args().size()==2) {
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
          for (unsigned int i=0; i<c->args().size(); i++) {
            if (c->args()[i]->type().bt()!=Type::BT_BOOL) {
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
          EE flat_al = flat_exp(env,Ctx(),c->args()[0],constants().var_ignore,constants().var_true);
          ArrayLit* al = follow_id(flat_al.r())->cast<ArrayLit>();
          nctx.b = C_ROOT;
          for (unsigned int i=0; i<al->v().size(); i++)
            (void) flat_exp(env,nctx,al->v()[i],r,b);
          ret.r = bind(env,ctx,r,constants().lit_true);
        } else {
          
          if (decl->e() && decl->params().size()==1 && decl->e()->isa<Id>() &&
              decl->params()[0]->ti()->domain()==NULL &&
              decl->e()->cast<Id>()->decl() == decl->params()[0]) {
            ret = flat_exp(env, ctx, c->args()[0], r, b);
            break;
          }
          
          std::vector<EE> args_ee(c->args().size());
          bool mixContext = decl->e()!=NULL ||
            (cid != constants().ids.forall && cid != constants().ids.exists && cid != constants().ids.bool2int &&
             cid != constants().ids.sum && cid != constants().ids.lin_exp && cid != "assert");
          bool isPartial = false;
          for (unsigned int i=c->args().size(); i--;) {
            Ctx argctx = nctx;
            if (mixContext) {
              if (cid==constants().ids.clause) {
                argctx.b = (i==0 ? +nctx.b : -nctx.b);
              } else if (c->args()[i]->type().bt()==Type::BT_BOOL) {
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
            isPartial |= isfalse(env, args_ee[i].b());
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
              for (unsigned int i=0; i<al->v().size(); i++) {
                if (Call* sc = same_call(al->v()[i],cid)) {
                  if (sc->id()==constants().ids.clause) {
                    alv.push_back(sc);
                  } else {
                    GCLock lock;
                    ArrayLit* sc_c = eval_array_lit(env,sc->args()[0]);
                    for (unsigned int j=0; j<sc_c->v().size(); j++) {
                      alv.push_back(sc_c->v()[j]);
                    }
                  }
                } else {
                  alv.push_back(al->v()[i]);
                }
              }

              for (unsigned int j=0; j<alv.size(); j++) {
                Call* neg_call = same_call(alv[j](),constants().ids.bool_eq);
                if (neg_call &&
                    Expression::equal(neg_call->args()[1],constants().lit_false)) {
                  local_neg.push_back(neg_call->args()[0]);
                } else {
                  Call* clause = same_call(alv[j](),constants().ids.clause);
                  if (clause) {
                    ArrayLit* clause_pos = eval_array_lit(env,clause->args()[0]);
                    for (unsigned int k=0; k<clause_pos->v().size(); k++) {
                      local_pos.push_back(clause_pos->v()[k]);
                    }
                    ArrayLit* clause_neg = eval_array_lit(env,clause->args()[1]);
                    for (unsigned int k=0; k<clause_neg->v().size(); k++) {
                      local_neg.push_back(clause_neg->v()[k]);
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
            for (unsigned int i=0; i<al->v().size(); i++) {
              if (Call* sc = same_call(al->v()[i],cid)) {
                GCLock lock;
                ArrayLit* sc_c = eval_array_lit(env,sc->args()[0]);
                for (unsigned int j=0; j<sc_c->v().size(); j++) {
                  alv.push_back(sc_c->v()[j]);
                }
              } else {
                alv.push_back(al->v()[i]);
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
            decl = env.orig->matchFn(env,cr_c);
            if (decl==NULL)
              throw FlatteningError(env,cr_c->loc(), "cannot find matching declaration");
            cr_c->type(decl->rtype(env,e_args));
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
                        c->decl(env.orig->matchFn(env,c));
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
                    BinOp* bo = follow_id_to_value(dom)->cast<BinOp>();
                    FloatVal dom_min = eval_float(env,bo->lhs());
                    FloatVal dom_max = eval_float(env,bo->rhs());
                    bool needToConstrain;
                    if (args[i]()->type().dim() > 0) {
                      needToConstrain = true;
                    } else {
                      FloatBounds fb = compute_float_bounds(env,args[i]());
                      needToConstrain = !fb.valid || dom_min > dom_max || fb.l < dom_min || fb.u > dom_max;
                    }
                    if (needToConstrain) {
                      GCLock lock;
                      Expression* domconstraint;
                      std::vector<Expression*> domargs(3);
                      domargs[0] = args[i]();
                      domargs[1] = bo->lhs();
                      domargs[2] = bo->rhs();
                      Call* c = new Call(Location().introduce(),"var_dom",domargs);
                      c->type(Type::varbool());
                      c->decl(env.orig->matchFn(env,c));
                      domconstraint = c;
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
              FunctionI* reif_decl = env.orig->matchFn(env, r_cid, argtypes);
              if (reif_decl && reif_decl->e()) {
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
                reif_b->e(cr());
                if (r != NULL && r->e() != NULL) {
                  bind(env,Ctx(),r,reif_b->id());
                }
                env.vo_add_exp(reif_b);
                ret.b = bind(env,Ctx(),b,constants().lit_true);
                args_ee.push_back(EE(NULL,reif_b->id()));
                ret.r = conj(env,NULL,ctx,args_ee);
                return ret;
              }
            }
          call_nonreif:
            // Handle symmetry predicates
            if (cr()->type().isbool() && requiresGlobalOrder(decl)) {
              if (env.collect_symmetry_vars) {
                // Leave symmetry predicates unflattened initially, they are recorded and flattened later
                GCLock lock;
                Call* cr_c = cr()->cast<Call>();
                ret.b = conj(env,b,Ctx(),args_ee);
                ret.r = bind(env,ctx,r,cr_c);
                env.num_symmetry_predicates++;

                // Record the variables that may need to be ordered
                ArrayLit* al = eval_array_lit(env,c->args()[0]);
                for (unsigned int i=0; i < al->v().size(); i++) {
                  Expression* eid = flat_exp(env, Ctx(), al->v()[i], NULL, constants().var_true).r();
                  if (eid->isa<Id>()) {
                    Id* id = eid->cast<Id>();
                    if (env.symmetryVars.find(id) == env.symmetryVars.end()) {
                      env.orderedSymmetryVars.push_back(id);
                      env.symmetryVars.insert(id);
                    }
                  } else {
                    throw FlatteningError(env, eid->loc(), "the first argument of a symmetry predicate must be a variable array");
                  }
                }
                return ret;
              } else if (env.num_symmetry_predicates > 1 && env.flatten_symmetry_predicates) {
                // Flatten symmetry predicates when ordering is required
                Call* cr_c = cr()->cast<Call>();
                int nargs = cr_c->args().size();
                std::vector<Expression*> args(nargs + 1);
                for (unsigned int j = 0; j < nargs; j++) {
                  args[j] = c->args()[j];
                }

                Type order_type = Type::vartop(1);
                order_type.bt(args[0]->type().bt());
                std::vector<Expression*> orderedVars;
                for (std::vector<Id*>::iterator id = env.orderedSymmetryVars.begin(); id != env.orderedSymmetryVars.end(); id++) {
                  if (Type::bt_subtype((*id)->type().bt(), order_type.bt())) {
                    orderedVars.push_back(*id);
                  }
                }
                ArrayLit* order = new ArrayLit(Location().introduce(), orderedVars);
                order->type(order_type);
                args[nargs] = order;

                {
                  GCLock lock;
                  cid = env.symOrdId(c->id());
                  c = new Call(Location().introduce(), cid, args);
                  decl = env.orig->matchFn(env, c);
                  if (decl==NULL)
                    throw FlatteningError(env,cr_c->loc(), "predicate '" + cr_c->id().str() + "' annotated with symmetry must have _ord version");
                  c->type(decl->rtype(env, args));
                  assert(decl);
                  c->decl(decl);
                }
                // We don't currently handle ordering symmetry predicates within symmetry predicates
                env.flatten_symmetry_predicates = false;
                ret = flat_exp(env, ctx, c, r, b);
                env.flatten_symmetry_predicates = true;
                return ret;
              }
            }

            if ( (cr()->type().ispar() && !cr()->type().isann()) || decl->e()==NULL) {
              Call* cr_c = cr()->cast<Call>();
              /// All builtins are total
              std::vector<Type> argt(cr_c->args().size());
              for (unsigned int i=argt.size(); i--;)
                argt[i] = cr_c->args()[i]->type();
              Type callt = decl->rtype(env,argt);
              if (callt.ispar() && callt.bt()!=Type::BT_ANN) {
                GCLock lock;
                try {
                  ret.r = bind(env,ctx,r,eval_par(env,cr_c));
                  ret.b = conj(env,b,Ctx(),args_ee);
                } catch (ResultUndefinedError&) {
                  ret.b = bind(env,Ctx(),b,constants().lit_false);
                  return ret;
                }
                // Do not insert into map, since par results will quickly become
                // garbage anyway and then disappear from the map
              } else if (decl->_builtins.e) {
                KeepAlive callres = decl->_builtins.e(env,cr_c);
                EE res = flat_exp(env,ctx,callres(),r,b);
                args_ee.push_back(res);
                ret.b = conj(env,b,Ctx(),args_ee);
                ret.r = bind(env,ctx,r,res.r());
                if (!ctx.neg && !cr_c->type().isann())
                  env.map_insert(cr_c,ret);
              } else {
                ret.b = conj(env,b,Ctx(),args_ee);
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
                      c->decl(env.orig->matchFn(env,c));
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
          VarDecl* vd = newVarDecl(env, ctx, ti, v->id()->idn()==-1 && !v->toplevel() ? NULL : v->id(), v, NULL);
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
                  BinOp* bo_dom = follow_id_to_value(vd->ti()->domain())->cast<BinOp>();
                  domargs[1] = bo_dom->lhs();
                  domargs.push_back(bo_dom->rhs());
                } else {
                  domargs[1] = vd->ti()->domain();
                }
                Call* c = new Call(vd->ti()->loc().introduce(),"var_dom",domargs);
                c->type(Type::varbool());
                c->decl(env.orig->matchFn(env,c));
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
              GCLock lock;
              TypeInst* ti = eval_typeinst(env,vd);
              VarDecl* nvd = newVarDecl(env, ctx, ti, NULL, vd, NULL);
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
  
  void outputVarDecls(EnvI& env, Item* ci, Expression* e, bool fCopy=true);

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
        FunctionI* decl = env.output->matchFn(env,c.id(), tv);
        Type t;
        if (decl==NULL) {
          FunctionI* origdecl = env.orig->matchFn(env, c.id(), tv);
          if (origdecl == NULL) {
            throw FlatteningError(env,c.loc(),"function is used in output, par version needed");
          }

          if (origdecl->e() && cannotUseRHSForOutput(env, origdecl->e())) {
            success = false;
          } else {
            if (!origdecl->from_stdlib()) {
              decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
              CollectOccurrencesE ce(env.output_vo,decl);
              topDown(ce, decl->e());
              topDown(ce, decl->ti());
              for (unsigned int i = decl->params().size(); i--;)
                topDown(ce, decl->params()[i]);
              env.output->registerFn(env, decl);
              env.output->addItem(decl);
              outputVarDecls(env,origdecl,decl->e());
              outputVarDecls(env,origdecl,decl->ti());
            } else {
              decl = origdecl;
            }
            c.decl(decl);
          }
        }
        if (success) {
          t = decl->rtype(env, tv);
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
  
  void makePar(EnvI& env, Expression* e) {
    class Par : public EVisitor {
    public:
      /// Visit variable declaration
      void vVarDecl(VarDecl& vd) {
        vd.ti()->type(vd.type());
      }
      /// Determine whether to enter node
      bool enter(Expression* e) {
        Type t = e->type();
        t.ti(Type::TI_PAR);
        e->type(t);
        return true;
      }
    } _par;
    topDown(_par, e);
    class Decls : public EVisitor {
    public:
      EnvI& env;
      Decls(EnvI& env0) : env(env0) {}
      void vCall(Call& c) {
        c.decl(env.orig->matchFn(env,&c));
      }
    } _decls(env);
    topDown(_decls, e);
  }
  
  void outputVarDecls(EnvI& env, Item* ci, Expression* e, bool fCopy) {
    class O : public EVisitor {
    public:
      EnvI& env;
      Item* ci;
      const bool fCopy;  // whether to copy the vd before putting to output
      O(EnvI& env0, Item* ci0, bool fC=1) : env(env0), ci(ci0), fCopy(fC) {}
      void vId(Id& id) {
        if (&id==constants().absent)
          return;
        if (!id.decl()->toplevel())
          return;
        VarDecl* vd = id.decl();
        VarDecl* reallyFlat = vd->flat();
        while (reallyFlat != NULL && reallyFlat != reallyFlat->flat())
          reallyFlat = reallyFlat->flat();
        IdMap<int>::iterator idx = reallyFlat ? env.output_vo.idx.find(reallyFlat->id()) : env.output_vo.idx.end();
        IdMap<int>::iterator idx2 = env.output_vo.idx.find(vd->id());
        if (idx==env.output_vo.idx.end() && idx2==env.output_vo.idx.end()) {
          VarDeclI* nvi = new VarDeclI(Location().introduce(), fCopy ?
                                       copy(env,env.cmap,vd)->cast<VarDecl>() : vd);
          Type t = nvi->e()->ti()->type();
          if (t.ti() != Type::TI_PAR) {
            t.ti(Type::TI_PAR);
          }
          makePar(env,nvi->e());
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
            Call* rhs = copy(env,env.cmap,it->second())->cast<Call>();
            {
              std::vector<Type> tv(rhs->args().size());
              for (unsigned int i=rhs->args().size(); i--;) {
                tv[i] = rhs->args()[i]->type();
                tv[i].ti(Type::TI_PAR);
              }
              FunctionI* decl = env.output->matchFn(env, rhs->id(), tv);
              Type t;
              if (decl==NULL) {
                FunctionI* origdecl = env.orig->matchFn(env, rhs->id(), tv);
                if (origdecl == NULL) {
                  throw FlatteningError(env,rhs->loc(),"function is used in output, par version needed");
                }
                if (!origdecl->from_stdlib()) {
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
                  args[i] = new SetLit(Location().introduce(), eval_intset(env,reallyFlat->ti()->ranges()[i]->domain()));
                } else {
                  args[i] = new SetLit(Location().introduce(), eval_intset(env,nvi->e()->ti()->ranges()[i]->domain()));
                }
              }
              ArrayLit* al = new ArrayLit(Location().introduce(), args);
              args.resize(1);
              args[0] = al;
              reallyFlat->addAnnotation(new Call(Location().introduce(),constants().ann.output_array,args,NULL));
            }
          } else {
            outputVarDecls(env, nvi, nvi->e()->ti());
            outputVarDecls(env, nvi, nvi->e()->e());
          }
          CollectOccurrencesE ce(env.output_vo,nvi);
          topDown(ce, nvi->e());
        }
      }
    } _o(env,ci,fCopy);
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
        if (!decl->from_stdlib()) {
          env.flat_addItem(decl);
        }
      }
    };
    for (unsigned int i=e.orig->size(); i--;) {
      if (OutputI* oi = (*e.orig)[i]->dyn_cast<OutputI>()) {
        GCLock lock;
        OutputI* noi = copy(e,oi)->cast<OutputI>();
        CopyOutput co(e);
        topDown(co, noi->e());
        e.flat_addItem(noi);
        break;
      }
    }
  }
  
  void createOutput(EnvI& e, std::vector<VarDecl*>& deletedFlatVarDecls) {
    if (e.output->size() > 0) {
      // Adapt existing output model
      // (generated by repeated flattening)
      e.output_vo.clear();
      for (unsigned int i=0; i<e.output->size(); i++) {
        Item* item = (*e.output)[i];
        if (item->removed())
          continue;
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
                Expression* flate = copy(e,e.cmap,follow_id(reallyFlat->id()));
                outputVarDecls(e,item,flate);
                vd->e(flate);
              } else if ( (it = e.reverseMappers.find(vd->id())) != e.reverseMappers.end()) {
                Call* rhs = copy(e,e.cmap,it->second())->cast<Call>();
                std::vector<Type> tv(rhs->args().size());
                for (unsigned int i=rhs->args().size(); i--;) {
                  tv[i] = rhs->args()[i]->type();
                  tv[i].ti(Type::TI_PAR);
                }
                FunctionI* decl = e.output->matchFn(e, rhs->id(), tv);
                if (decl==NULL) {
                  FunctionI* origdecl = e.orig->matchFn(e, rhs->id(), tv);
                  if (origdecl == NULL) {
                    throw FlatteningError(e,rhs->loc(),"function is used in output, par version needed");
                  }
                  if (!origdecl->from_stdlib()) {
                    decl = copy(e,e.cmap,origdecl)->cast<FunctionI>();
                    CollectOccurrencesE ce(e.output_vo,decl);
                    topDown(ce, decl->e());
                    topDown(ce, decl->ti());
                    for (unsigned int i = decl->params().size(); i--;)
                      topDown(ce, decl->params()[i]);
                    e.output->registerFn(e, decl);
                    e.output->addItem(decl);
                  } else {
                    decl = origdecl;
                  }
                }
                rhs->decl(decl);
                removeIsOutput(reallyFlat);
                
                if (e.vo.occurrences(reallyFlat)==0 && reallyFlat->e()==NULL) {
                  deletedFlatVarDecls.push_back(reallyFlat);
                }
                
                outputVarDecls(e,item,it->second()->cast<Call>());
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
                    removeIsOutput(reallyFlat);
                    if (e.vo.occurrences(reallyFlat)==0) {
                      deletedFlatVarDecls.push_back(reallyFlat);
                    }

                    outputVarDecls(e, item, al);
                    vd->e(copy(e,e.cmap,al));
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
                          args[i] = new SetLit(Location().introduce(), eval_intset(e,vd->flat()->ti()->ranges()[i]->domain()));
                        } else {
                          args[i] = new SetLit(Location().introduce(), eval_intset(e,vd->ti()->ranges()[i]->domain()));
                        }
                      }
                      ArrayLit* al = new ArrayLit(Location().introduce(), args);
                      args.resize(1);
                      args[0] = al;
                      vd->flat()->addAnnotation(new Call(Location().introduce(),constants().ann.output_array,args,NULL));
                    }
                  }
                }
              }
              vd->flat(NULL);
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
      GCLock lock;

      class OV1 : public ItemVisitor {
      public:
        EnvI& env;
        VarOccurrences& vo;
        OutputI*& outputItem;
        OV1(EnvI& env0, VarOccurrences& vo0, OutputI*& outputItem0)
        : env(env0), vo(vo0), outputItem(outputItem0) {}
        void vOutputI(OutputI* oi) {
          outputItem = copy(env,env.cmap, oi)->cast<OutputI>();
          makePar(env,outputItem->e());
          env.output->addItem(outputItem);
        }
      } _ov1(e,e.output_vo,outputItem);
      iterItems(_ov1,e.orig);
      
      if (outputItem==NULL) {
        // Create output item for all variables defined at toplevel in the MiniZinc source
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
                  IntSetVal* idxset = eval_intset(e,vd->ti()->ranges()[i]->domain());
                  s << *idxset << ",";
                }
              }
              StringLit* sl = new StringLit(Location().introduce(),s.str());
              outputVars.push_back(sl);
              
              std::vector<Expression*> showArgs(1);
              showArgs[0] = vd->id();
              Call* show = new Call(Location().introduce(),constants().ids.show,showArgs);
              show->type(Type::parstring());
              FunctionI* fi = e.orig->matchFn(e, show);
              assert(fi);
              show->decl(fi);
              outputVars.push_back(show);
              std::string ends = vd->type().dim() > 0 ? ")" : "";
              ends += ";\n";
              StringLit* eol = new StringLit(Location().introduce(),ends);
              outputVars.push_back(eol);
            }
          }
        }
        OutputI* newOutputItem = new OutputI(Location().introduce(),new ArrayLit(Location().introduce(),outputVars));
        e.orig->addItem(newOutputItem);
        outputItem = copy(e,e.cmap, newOutputItem)->cast<OutputI>();
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
          FunctionI* decl = env.output->matchFn(env, c.id(), tv);
          Type t;
          if (decl==NULL) {
            FunctionI* origdecl = env.orig->matchFn(env, c.id(), tv);
            if (origdecl == NULL || !origdecl->rtype(env, tv).ispar()) {
              throw FlatteningError(env,c.loc(),"function is used in output, par version needed");
            }
            if (!origdecl->from_stdlib()) {
              decl = copy(env,env.cmap,origdecl)->cast<FunctionI>();
              CollectOccurrencesE ce(env.output_vo,decl);
              topDown(ce, decl->e());
              topDown(ce, decl->ti());
              for (unsigned int i = decl->params().size(); i--;)
                topDown(ce, decl->params()[i]);
              env.output->registerFn(env, decl);
              env.output->addItem(decl);
              if (decl->e())
                topDown(*this, decl->e());
            } else {
              decl = origdecl;
            }
          }
          c.decl(decl);
        }
      } _cf(e);
      topDown(_cf, outputItem->e());
      
      class OV2 : public ItemVisitor {
      public:
        EnvI& env;
        OV2(EnvI& env0) : env(env0) {}
        void vVarDeclI(VarDeclI* vdi) {
          IdMap<int>::iterator idx = env.output_vo.idx.find(vdi->e()->id());
          if (idx!=env.output_vo.idx.end())
            return;
          if (Expression* vd_e = env.cmap.find(vdi->e())) {
            VarDecl* vd = vd_e->cast<VarDecl>();
            VarDeclI* vdi_copy = copy(env,env.cmap,vdi)->cast<VarDeclI>();
            Type t = vdi_copy->e()->ti()->type();
            t.ti(Type::TI_PAR);
            vdi_copy->e()->ti()->domain(NULL);
            vdi_copy->e()->flat(vdi->e()->flat());
            vdi_copy->e()->ann().clear();
            vdi_copy->e()->introduced(false);
            IdMap<KeepAlive>::iterator it;
            if (!vdi->e()->type().ispar()) {
              if (vd->flat() == NULL && vdi->e()->e()!=NULL && vdi->e()->e()->type().ispar()) {
                Expression* flate = eval_par(env, vdi->e()->e());
                outputVarDecls(env,vdi_copy,flate);
                vd->e(flate);
              } else {
                vd = follow_id_to_decl(vd->id())->cast<VarDecl>();
                VarDecl* reallyFlat = vd->flat();
              
                while (reallyFlat!=reallyFlat->flat())
                  reallyFlat=reallyFlat->flat();
                if (vd->flat()->e() && vd->flat()->e()->type().ispar()) {
                  Expression* flate = copy(env,env.cmap,follow_id(reallyFlat->id()));
                  outputVarDecls(env,vdi_copy,flate);
                  vd->e(flate);
                } else if ( (it = env.reverseMappers.find(vd->id())) != env.reverseMappers.end()) {
                  Call* rhs = copy(env,env.cmap,it->second())->cast<Call>();
                  {
                    std::vector<Type> tv(rhs->args().size());
                    for (unsigned int i=rhs->args().size(); i--;) {
                      tv[i] = rhs->args()[i]->type();
                      tv[i].ti(Type::TI_PAR);
                    }
                    FunctionI* decl = env.output->matchFn(env, rhs->id(), tv);
                    if (decl==NULL) {
                      FunctionI* origdecl = env.orig->matchFn(env, rhs->id(), tv);
                      if (origdecl == NULL) {
                        throw FlatteningError(env,rhs->loc(),"function is used in output, par version needed");
                      }
                      if (!origdecl->from_stdlib()) {
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
                  }
                  outputVarDecls(env,vdi_copy,rhs,0);
                  vd->e(rhs);
                } else if (cannotUseRHSForOutput(env,vd->e())) {
                  // If the VarDecl does not have a usable right hand side, it needs to be
                  // marked as output in the FlatZinc
                  vd->e(NULL);
                  assert(vd->flat());
                  if (vd->type().dim() == 0) {
                    vd->flat()->addAnnotation(constants().ann.output_var);
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
                        vd->e(copy(env,env.cmap,al));
                      }
                    }
                    if (needOutputAnn) {
                      std::vector<Expression*> args(vdi->e()->type().dim());
                      for (unsigned int i=0; i<args.size(); i++) {
                        if (vdi->e()->ti()->ranges()[i]->domain() == NULL) {
                          args[i] = new SetLit(Location().introduce(), eval_intset(env,vd->flat()->ti()->ranges()[i]->domain()));
                        } else {
                          args[i] = new SetLit(Location().introduce(), eval_intset(env,vd->ti()->ranges()[i]->domain()));
                        }
                      }
                      ArrayLit* al = new ArrayLit(Location().introduce(), args);
                      args.resize(1);
                      args[0] = al;
                      vd->flat()->addAnnotation(new Call(Location().introduce(),constants().ann.output_array,args,NULL));
                    }
                  }
                }
                if (env.output_vo.find(reallyFlat) == -1)
                  env.output_vo.add(reallyFlat, env.output->size());
              }
            }
            makePar(env,vdi_copy->e());
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

      e.orig->mergeStdLib(e, e.output);
    }
    
    std::vector<VarDecl*> deletedVarDecls;
    for (unsigned int i=0; i<e.output->size(); i++) {
      if (VarDeclI* vdi = (*e.output)[i]->dyn_cast<VarDeclI>()) {
        if (!vdi->removed() && e.output_vo.occurrences(vdi->e())==0) {
          CollectDecls cd(e.output_vo,deletedVarDecls,vdi);
          topDown(cd, vdi->e()->e());
          removeIsOutput(vdi->e()->flat());
          if (e.output_vo.find(vdi->e())!=-1)
            e.output_vo.remove(vdi->e());
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
            if (e.output_vo.find(vdi->e())!=-1)
              e.output_vo.remove(vdi->e());
            vdi->remove();
          }
        }
      }
    }

    for (IdMap<VarOccurrences::Items>::iterator it = e.output_vo._m.begin();
         it != e.output_vo._m.end(); ++it) {
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
  }
  
  void cleanupOutput(EnvI& env) {
    for (unsigned int i=0; i<env.output->size(); i++) {
      if (VarDeclI* vdi = (*env.output)[i]->dyn_cast<VarDeclI>()) {
        vdi->e()->flat(NULL);
      }
    }
  }

  bool checkParDomain(EnvI& env, Expression* e, Expression* domain) {
    if (e->type()==Type::parint()) {
      IntSetVal* isv = eval_intset(env,domain);
      if (!isv->contains(eval_int(env,e)))
        return false;
    } else if (e->type()==Type::parfloat()) {
      BinOp* bo = follow_id_to_value(domain)->cast<BinOp>();
      assert(bo->op()==BOT_DOTDOT);
      FloatVal d_min = eval_float(env,bo->lhs());
      FloatVal d_max = eval_float(env,bo->rhs());
      FloatVal de = eval_float(env,e);
      if (de < d_min || de > d_max)
        return false;
    } else if (e->type()==Type::parsetint()) {
      IntSetVal* isv = eval_intset(env,domain);
      IntSetRanges ir(isv);
      IntSetVal* rsv = eval_intset(env,e);
      IntSetRanges rr(rsv);
      if (!Ranges::subset(rr, ir))
        return false;
    }
    return true;
  }
  
  void flatten(Env& e, FlatteningOptions opt) {
    
    try {

      EnvI& env = e.envi();
      
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
        check_only_range->decl(env.orig->matchFn(e.envi(), check_only_range));
        onlyRangeDomains = eval_bool(e.envi(), check_only_range);
      }
      
      class ExpandArrayDecls : public ItemVisitor {
      public:
        EnvI& env;
        ExpandArrayDecls(EnvI& env0) : env(env0) {}
        void vVarDeclI(VarDeclI* v) {
          if (v->e()->type().isvar() && v->e()->type().dim() > 0 && v->e()->e() == NULL) {
            (void) flat_exp(env,Ctx(),v->e()->id(),NULL,constants().var_true);
          }
        }
      } _ead(env);
      iterItems<ExpandArrayDecls>(_ead,e.model());;
      
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
                  for (unsigned int i=0; i<al->v().size(); i++) {
                    if (!checkParDomain(env,al->v()[i], v->e()->ti()->domain())) {
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
            nsi = SolveI::min(Location().introduce(),flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r());
            break;
          case SolveI::ST_MAX:
            nsi = SolveI::max(Location().introduce(),flat_exp(env,Ctx(),si->e(),NULL,constants().var_true).r());
            break;
          }
          bool hadGlobalOrder = false;
          bool hadSearchOrder = false;
          std::vector<Id*> globalOrder;
          std::vector<Id*> searchOrder;
          for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
            if ((*it)->isa<Call>()) {
              Call* c = (*it)->cast<Call>();
              // Extract ordering from global_order
              if (c->id() == constants().ann.global_order) {
                if (hadGlobalOrder)
                  throw FlatteningError(env,si->loc(), "Only one global_order annotation is allowed");
                hadGlobalOrder = true;

                ArrayLit* al = eval_array_lit(env,c->args()[0]);
                for (unsigned int i=0; i < al->v().size(); i++) {
                  Expression* eid = flat_exp(env, Ctx(), al->v()[i], NULL, constants().var_true).r();
                  if (eid->isa<Id>()) {
                    globalOrder.push_back(eid->cast<Id>());
                  }
                }
                continue;
              } else if ((c->id() == constants().ann.seq_search  || c->id() == constants().ann.int_search   ||
                          c->id() == constants().ann.bool_search || c->id() == constants().ann.float_search ||
                          c->id() == constants().ann.set_search) && !hadSearchOrder) {
                hadSearchOrder = true;

                // Extract ordering from search strategy
                Expression* flatSearch = flat_exp(env,Ctx(),c,NULL,constants().var_true).r();
                if (flatSearch->isa<Call>()) {
                  std::vector<Call*> searches;
                  searches.push_back(flatSearch->cast<Call>());
                  while (!searches.empty()) {
                    Call* search = searches.back();
                    searches.pop_back();
                    if (search->id() == constants().ann.seq_search) {
                      ArrayLit* al = eval_array_lit(env,search->args()[0]);
                      for (unsigned int i=al->v().size(); i--;) {
                        if (al->v()[i]->isa<Call>()) {
                          Call* cs = al->v()[i]->cast<Call>();
                          if (cs->id() == constants().ann.seq_search  || cs->id() == constants().ann.int_search   ||
                              cs->id() == constants().ann.bool_search || cs->id() == constants().ann.float_search ||
                              cs->id() == constants().ann.set_search) {
                            searches.push_back(cs);
                          }
                        }
                      }
                    } else if (search->id() == constants().ann.int_search   || search->id() == constants().ann.bool_search ||
                               search->id() == constants().ann.float_search || search->id() == constants().ann.set_search) {
                      ArrayLit* al = eval_array_lit(env,search->args()[0]);
                      for (unsigned int i=0; i < al->v().size(); i++) {
                        Expression* eid = flat_exp(env, Ctx(), al->v()[i], NULL, constants().var_true).r();
                        if (eid->isa<Id>()) {
                          searchOrder.push_back(eid->cast<Id>());
                        }
                      }
                    }
                  }
                }
                nsi->ann().add(flatSearch);
                continue;
              }
            }
            nsi->ann().add(flat_exp(env,Ctx(),*it,NULL,constants().var_true).r());
          }
          env.flat_addItem(nsi);

          // Recreate global order to respect global_order, then search strategy, then any remaining symmetry predicate variables
          if (globalOrder.size() > 0 || searchOrder.size() > 0) {
            std::vector<Id*> predicateVars(env.orderedSymmetryVars);
            env.orderedSymmetryVars.clear();
            env.symmetryVars.clear();
            if (globalOrder.size() > 0) {
              for (unsigned int i = 0; i < globalOrder.size(); i++) {
                if (env.symmetryVars.find(globalOrder[i]) == env.symmetryVars.end()) {
                  env.orderedSymmetryVars.push_back(globalOrder[i]);
                  env.symmetryVars.insert(globalOrder[i]);
                }
              }
            }
            if (searchOrder.size() > 0) {
              for (unsigned int i = 0; i < searchOrder.size(); i++) {
                if (env.symmetryVars.find(searchOrder[i]) == env.symmetryVars.end()) {
                  env.orderedSymmetryVars.push_back(searchOrder[i]);
                  env.symmetryVars.insert(searchOrder[i]);
                }
              }
            }
            for (unsigned int i = 0; i < predicateVars.size(); i++) {
              if (env.symmetryVars.find(predicateVars[i]) == env.symmetryVars.end()) {
                env.orderedSymmetryVars.push_back(predicateVars[i]);
                env.symmetryVars.insert(predicateVars[i]);
              }
            }
          }
        }
      } _fv(env,hadSolveItem);
      iterItems<FV>(_fv,e.model());
      
      if (!hadSolveItem) {
        e.envi().errorStack.clear();
        Location modelLoc;
        modelLoc.filename = e.model()->filepath();
        throw FlatteningError(e.envi(),modelLoc, "Model does not have a solve item");
      }

      std::vector<VarDecl*> deletedVarDecls;

      // Create output model
      if (opt.keepOutputInFzn) {
        copyOutput(env);
      } else {
        createOutput(env, deletedVarDecls);
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
        FunctionI* fi = env.orig->matchFn(env, constants().ids.int_.lin_eq, int_lin_eq_t);
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
        FunctionI* fi = env.orig->matchFn(env, ASTString("array_bool_and"), array_bool_andor_t);
        array_bool_and = (fi && fi->e()) ? fi : NULL;
        fi = env.orig->matchFn(env, ASTString("array_bool_or"), array_bool_andor_t);
        array_bool_or = (fi && fi->e()) ? fi : NULL;

        array_bool_andor_t[1] = Type::varbool(1);
        fi = env.orig->matchFn(env, ASTString("bool_clause"), array_bool_andor_t);
        array_bool_clause = (fi && fi->e()) ? fi : NULL;

        array_bool_andor_t.push_back(Type::varbool());
        fi = env.orig->matchFn(env, ASTString("bool_clause_reif"), array_bool_andor_t);
        array_bool_clause_reif = (fi && fi->e()) ? fi : NULL;
        
        std::vector<Type> bool_xor_t(3);
        bool_xor_t[0] = Type::varbool();
        bool_xor_t[1] = Type::varbool();
        bool_xor_t[2] = Type::varbool();
        fi = env.orig->matchFn(env, constants().ids.bool_xor, bool_xor_t);
        bool_xor = (fi && fi->e()) ? fi : NULL;
      }
      
      std::vector<VarDeclI*> removedItems;
      env.collectVarDecls(true);
      env.collectSymmetryVars(false);

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
                  vdi->remove();
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
              vdi->remove();
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
                  call->decl(env.orig->matchFn(env, call));
                  env.flat_addItem(new ConstraintI(Location().introduce(), call));
                } else if (dom->max(dom->size()-1).isFinite()) {
                  std::vector<Expression*> args(2);
                  args[0] = vdi->e()->id();
                  args[1] = IntLit::a(dom->max(dom->size()-1));
                  Call* call = new Call(Location().introduce(),constants().ids.int_.le,args);
                  call->type(Type::varbool());
                  call->decl(env.orig->matchFn(env, call));
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
                    Call* call = new Call(Location().introduce(),constants().ids.int_.ne,args);
                    call->type(Type::varbool());
                    call->decl(env.orig->matchFn(env, call));
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
            BinOp* bo = follow_id_to_value(vdi->e()->ti()->domain())->cast<BinOp>();
            FloatVal vmin = eval_float(env, bo->lhs());
            FloatVal vmax = eval_float(env, bo->rhs());
            if (vmin == -std::numeric_limits<FloatVal>::infinity() && vmax == std::numeric_limits<FloatVal>::infinity()) {
              vdi->e()->ti()->domain(NULL);
            } else if (vmin == -std::numeric_limits<FloatVal>::infinity()) {
              vdi->e()->ti()->domain(NULL);
              std::vector<Expression*> args(2);
              args[0] = vdi->e()->id();
              args[1] = FloatLit::a(vmax);
              Call* call = new Call(Location().introduce(),constants().ids.float_.le,args);
              call->type(Type::varbool());
              call->decl(env.orig->matchFn(env, call));
              env.flat_addItem(new ConstraintI(Location().introduce(), call));
            } else if (vmax == std::numeric_limits<FloatVal>::infinity()) {
              vdi->e()->ti()->domain(NULL);
              std::vector<Expression*> args(2);
              args[0] = FloatLit::a(vmin);
              args[1] = vdi->e()->id();
              Call* call = new Call(Location().introduce(),constants().ids.float_.le,args);
              call->type(Type::varbool());
              call->decl(env.orig->matchFn(env, call));
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
                    std::vector<Expression*> args(c->args().size());
                    ArrayLit* le_c = follow_id(c->args()[0])->cast<ArrayLit>();
                    std::vector<Expression*> nc_c(le_c->v().size());
                    std::copy(le_c->v().begin(),le_c->v().end(),nc_c.begin());
                    nc_c.push_back(IntLit::a(-1));
                    args[0] = new ArrayLit(Location().introduce(),nc_c);
                    args[0]->type(Type::parint(1));
                    ArrayLit* le_x = follow_id(c->args()[1])->cast<ArrayLit>();
                    std::vector<Expression*> nx(le_x->v().size());
                    std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                    nx.push_back(vd->id());
                    args[1] = new ArrayLit(Location().introduce(),nx);
                    args[1]->type(Type::varint(1));
                    IntVal d = c->args()[2]->cast<IntLit>()->v();
                    args[2] = IntLit::a(-d);
                    args[2]->type(Type::parint(0));
                    nc = new Call(c->loc().introduce(),ASTString("int_lin_eq"),args);
                    nc->type(Type::varbool());
                    nc->decl(int_lin_eq);
                  }
                } else if (c->id() == constants().ids.exists) {
                  if (array_bool_or) {
                    std::vector<Expression*> args(2);
                    args[0] = c->args()[0];
                    args[1] = vd->id();
                    nc = new Call(c->loc().introduce(),array_bool_or->id(),args);
                    nc->type(Type::varbool());
                    nc->decl(array_bool_or);
                  }
                } else if (!isTrueVar && c->id() == constants().ids.forall) {
                  if (array_bool_and) {
                    std::vector<Expression*> args(2);
                    args[0] = c->args()[0];
                    args[1] = vd->id();
                    nc = new Call(c->loc().introduce(),array_bool_and->id(),args);
                    nc->type(Type::varbool());
                    nc->decl(array_bool_and);
                  }
                } else if (c->id() == constants().ids.clause && array_bool_clause_reif) {
                  std::vector<Expression*> args(3);
                  args[0] = c->args()[0];
                  args[1] = c->args()[1];
                  args[2] = vd->id();
                  nc = new Call(c->loc().introduce(),array_bool_clause_reif->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause_reif);
                } else {
                  if (isTrueVar) {
                    FunctionI* decl = env.orig->matchFn(env,c);
                    env.map_remove(c);
                    if (decl->e() || c->id() == constants().ids.forall) {
                      c->decl(decl);
                      nc = c;
                    }
                  } else {
                    std::vector<Expression*> args(c->args().size());
                    std::copy(c->args().begin(),c->args().end(),args.begin());
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
                      FunctionI* decl = env.orig->matchFn(env,cid,args);
                      if (decl && (decl->e() || (c->type().isbool() && requiresGlobalOrder(decl)))) {
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
                  if (nc != c) {
                    vd->addAnnotation(constants().ann.is_defined_var);
                    nc->addAnnotation(definesVarAnn(vd->id()));
                  }
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
                  nc = new Call(c->loc().introduce(),array_bool_or->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_or);
                }
              } else if (c->id() == constants().ids.forall) {
                if (array_bool_and) {
                  std::vector<Expression*> args(2);
                  args[0] = c->args()[0];
                  args[1] = constants().lit_true;
                  nc = new Call(c->loc().introduce(),array_bool_and->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_and);
                }
              } else if (c->id() == constants().ids.clause) {
                if (array_bool_clause) {
                  std::vector<Expression*> args(2);
                  args[0] = c->args()[0];
                  args[1] = c->args()[1];
                  nc = new Call(c->loc().introduce(),array_bool_clause->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(array_bool_clause);
                }
              } else if (c->id() == constants().ids.bool_xor) {
                if (bool_xor) {
                  std::vector<Expression*> args(3);
                  args[0] = c->args()[0];
                  args[1] = c->args()[1];
                  args[2] = c->args().size()==2 ? constants().lit_true : c->args()[2];
                  nc = new Call(c->loc().introduce(),bool_xor->id(),args);
                  nc->type(Type::varbool());
                  nc->decl(bool_xor);
                }
              } else {
                FunctionI* decl = env.orig->matchFn(env,c);
                if (decl && (decl->e() || (c->type().isbool() && requiresGlobalOrder(decl)))) {
                  nc = c;
                  nc->decl(decl);
                }
              }
              if (nc != NULL) {
                CollectDecls cd(env.vo,deletedVarDecls,ci);
                topDown(cd,c);
                ci->e(constants().lit_true);
                env.flat_removeItem(i);
                (void) flat_exp(env, Ctx(), nc, constants().var_true, constants().var_true);
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
          if (!vdi->e()->type().ispar() &&
              vdi->e()->e()==NULL &&
              (it = env.reverseMappers.find(vdi->e()->id())) != env.reverseMappers.end()) {
            GCLock lock;
            Call* rhs = copy(env,env.cmap,it->second())->cast<Call>();
            std::vector<Type> tv(rhs->args().size());
            for (unsigned int i=rhs->args().size(); i--;) {
              tv[i] = rhs->args()[i]->type();
              tv[i].ti(Type::TI_PAR);
            }
            FunctionI* decl = env.output->matchFn(env, rhs->id(), tv);
            Type t;
            if (decl==NULL) {
              FunctionI* origdecl = env.orig->matchFn(env, rhs->id(), tv);
              if (origdecl == NULL) {
                throw FlatteningError(env,rhs->loc(),"function is used in output, par version needed");
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
        createOutput(env, deletedVarDecls);
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
  
  void oldflatzinc(Env& e) {
    Model* m = e.flat();

    // Mark annotations and optional variables for removal
    for (unsigned int i=0; i<m->size(); i++) {
      Item* item = (*m)[i];
      if (item->isa<VarDeclI>() &&
          (item->cast<VarDeclI>()->e()->type().ot() == Type::OT_OPTIONAL ||
           item->cast<VarDeclI>()->e()->type().bt() == Type::BT_ANN) ) {
            e.envi().flat_removeItem(i);
          }
    }

    // Remove items marked for removal
    m->compact();
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

        // Record whether this VarDecl is equal to an Id (aliasing)
        if (vd->e() && vd->e()->isa<Id>()) {
          declsWithIds.push_back(i);
          vdi->e()->payload(-static_cast<int>(i)-1);
        } else {
          vdi->e()->payload(i);
        }

        // In FlatZinc the RHS of a VarDecl must be a literal, Id or empty
        // Example:
        //   var 1..5: x = function(y)
        // becomes:
        //   var 1..5: x;
        //   relation(x, y);
        if (vd->type().isvar() && vd->type().isbool()) {
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
                e.envi().flat_addItem(new ConstraintI(Location().introduce(),ve));
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
                Call * nc = new Call(c->loc().introduce(),cid,args);
                nc->type(c->type());
                nc->decl(env.orig->matchFn(env, nc));
                if (nc->decl()==NULL) {
                  throw FlatteningError(env,c->loc(),"'"+c->id().str()+"' is used in a reified context but no reified version is available");
                }
                nc->addAnnotation(definesVarAnn(vd->id()));
                nc->ann().merge(c->ann());
                e.envi().flat_addItem(new ConstraintI(Location().introduce(),nc));
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
          // Int or Float var
          if (vd->e() != NULL) {
            if (const Call* cc = vd->e()->dyn_cast<Call>()) {
              // Remove RHS from vd
              vd->e(NULL);
              vd->addAnnotation(constants().ann.is_defined_var);

              std::vector<Expression*> args(cc->args().size());
              ASTString cid;
              if (cc->id() == constants().ids.lin_exp) {
                // a = lin_exp([1],[b],5) => int_lin_eq([1,-1],[b,a],-5):: defines_var(a)
                ArrayLit* le_c = follow_id(cc->args()[0])->cast<ArrayLit>();
                std::vector<Expression*> nc(le_c->v().size());
                std::copy(le_c->v().begin(),le_c->v().end(),nc.begin());
                if (le_c->type().bt()==Type::BT_INT) {
                  cid = constants().ids.int_.lin_eq;
                  nc.push_back(IntLit::a(-1));
                  args[0] = new ArrayLit(Location().introduce(),nc);
                  args[0]->type(Type::parint(1));
                  ArrayLit* le_x = follow_id(cc->args()[1])->cast<ArrayLit>();
                  std::vector<Expression*> nx(le_x->v().size());
                  std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location().introduce(),nx);
                  args[1]->type(le_x->type());
                  IntVal d = cc->args()[2]->cast<IntLit>()->v();
                  args[2] = IntLit::a(-d);
                } else {
                  // float
                  cid = constants().ids.float_.lin_eq;
                  nc.push_back(FloatLit::a(-1.0));
                  args[0] = new ArrayLit(Location().introduce(),nc);
                  args[0]->type(Type::parfloat(1));
                  ArrayLit* le_x = follow_id(cc->args()[1])->cast<ArrayLit>();
                  std::vector<Expression*> nx(le_x->v().size());
                  std::copy(le_x->v().begin(),le_x->v().end(),nx.begin());
                  nx.push_back(vd->id());
                  args[1] = new ArrayLit(Location().introduce(),nx);
                  args[1]->type(le_x->type());
                  FloatVal d = cc->args()[2]->cast<FloatLit>()->v();
                  args[2] = FloatLit::a(-d);
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
              Call* nc = new Call(cc->loc().introduce(),cid,args);
              nc->type(cc->type());
              nc->addAnnotation(definesVarAnn(vd->id()));
              nc->ann().merge(cc->ann());
              e.envi().flat_addItem(new ConstraintI(Location().introduce(),nc));
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
              continue;
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
          std::vector<int> dims(2);
          dims[0] = 1;
          dims[1] = al->length();
          al->setDims(ASTIntVec(dims));
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
      } else if (ConstraintI* ci = (*m)[i]->dyn_cast<ConstraintI>()) {

        // Remove defines_var(x) annotation where x is par
        std::vector<Expression*> removeAnns;
        for (ExpressionSetIter anns = ci->e()->ann().begin(); anns != ci->e()->ann().end(); ++anns) {
          if (Call* c = (*anns)->dyn_cast<Call>()) {
            if (c->id() == constants().ann.defines_var && c->args()[0]->type().ispar()) {
              removeAnns.push_back(c);
            }
          }
        }
        for (unsigned int i=0; i<removeAnns.size(); i++) {
          ci->e()->ann().remove(removeAnns[i]);
        }

        if (Call* vc = ci->e()->dyn_cast<Call>()) {
          for (unsigned int i=0; i<vc->args().size(); i++) {
            // Change array indicies to be 1 indexed
            if (ArrayLit* al = vc->args()[i]->dyn_cast<ArrayLit>()) {
              if (al->dims()>1 || al->min(0)!= 1) {
                std::vector<int> dims(2);
                dims[0] = 1;
                dims[1] = al->length();
                GCLock lock;
                al->setDims(ASTIntVec(dims));
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
            vc->id(ASTString("array_bool_or"));
            std::vector<Expression*> args(2);
            args[0] = vc->args()[0];
            args[1] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
            vc->decl(e.envi().orig->matchFn(env, vc));
          } else if (vc->id() == constants().ids.forall) {
            GCLock lock;
            vc->id(ASTString("array_bool_and"));
            std::vector<Expression*> args(2);
            args[0] = vc->args()[0];
            args[1] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
            vc->decl(e.envi().orig->matchFn(env, vc));
          } else if (vc->id() == constants().ids.clause) {
            GCLock lock;
            vc->id(ASTString("bool_clause"));
            vc->decl(e.envi().orig->matchFn(env, vc));
          } else if (vc->id() == constants().ids.bool_xor && vc->args().size()==2) {
            GCLock lock;
            std::vector<Expression*> args(3);
            args[0] = vc->args()[0];
            args[1] = vc->args()[1];
            args[2] = constants().lit_true;
            ASTExprVec<Expression> argsv(args);
            vc->args(argsv);
            vc->decl(e.envi().orig->matchFn(env, vc));
          }

          // If vc->decl() is a solver builtin and has not been added to the
          // FlatZinc, add it
          if (vc->decl() && vc->decl() != constants().var_redef &&
              !vc->decl()->from_stdlib() &&
              globals.find(vc->decl())==globals.end()) {
            e.envi().flat_addItem(vc->decl());
            globals.insert(vc->decl());
          }
        } else if (Id* id = ci->e()->dyn_cast<Id>()) {
          // Ex: constraint b; => constraint bool_eq(b, true);
          std::vector<Expression*> args(2);
          args[0] = id;
          args[1] = constants().lit_true;
          GCLock lock;
          ci->e(new Call(Location().introduce(),constants().ids.bool_eq,args));
        } else if (BoolLit* bl = ci->e()->dyn_cast<BoolLit>()) {
          // Ex: true => delete; false => bool_eq(false, true);
          if (!bl->v()) {
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = constants().lit_false;
            args[1] = constants().lit_true;
            Call* neq = new Call(Location().introduce(),constants().ids.bool_eq,args);
            ci->e(neq);
          } else {
            ci->remove();
          }
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
            if (call->args().size() > 0) {
              Type all_t;
              for (unsigned int i=0; i<call->args().size(); i++) {
                Type t = call->args()[i]->type();
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
