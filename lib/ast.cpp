/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/exception.hh>
#include <minizinc/iter.hh>

namespace MiniZinc {

  Location
  Location::a(void) {
    Location l;
    l.first_line = 0;
    l.first_column = 0;
    l.last_line = 0;
    l.last_column = 0;
    return l;
  }

  std::string
  Location::toString(void) const {
    std::ostringstream oss;
    oss << filename.str() << ":" << first_line << "." << first_column;
    return oss.str();
  }

  void
  Location::mark(void) {
    filename.mark();
  }

  void
  Annotation::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_e));
    cmb_hash(Expression::hash(_a));
  }
  Annotation*
  Annotation::a(const Location& loc, Expression* e) {
    Annotation* ann = new Annotation(loc,e);
    ann->rehash();
    return ann;
  }
  Annotation*
  Annotation::a(const Location& loc, Expression* e, Annotation* a) {
    Annotation* ann = new Annotation(loc,e);
    ann->_a = a;
    ann->rehash();
    return ann;
  }
  void
  Annotation::merge(Annotation* a) {
    Annotation* r = this;
    while (r->_a != NULL) r=r->_a;
    r->_a = a;
    r->rehash();
  }

  void
  Expression::annotate(Annotation* ann) {
    if (_ann) _ann->merge(ann); else _ann=ann;
  }

#define pushstack(e) do { if (e!=NULL) { stack.push_back(e); }} while(0)
#define pushall(v) do { v.mark(); for (Expression* e : v) if (e!=NULL) { stack.push_back(e); }} while(0)
  void
  Expression::mark(Expression* e) {
    if (e==NULL) return;
    std::vector<Expression*> stack;
    stack.push_back(e);
    while (!stack.empty()) {
      Expression* cur = stack.back(); stack.pop_back();
      if (cur->_gc_mark==0) {
        cur->_gc_mark = 1;
        cur->_loc.mark();
        pushstack(cur->_ann);
        switch (cur->eid()) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_ANON:
          break;
        case Expression::E_SETLIT:
          if (cur->cast<SetLit>()->_isv)
            cur->cast<SetLit>()->_isv->mark();
          else
            pushall(cur->cast<SetLit>()->_v);
          break;
        case Expression::E_STRINGLIT:
          cur->cast<StringLit>()->_v.mark();
          break;
        case Expression::E_ID:
          cur->cast<Id>()->_v.mark();
          pushstack(cur->cast<Id>()->_decl);
          break;
        case Expression::E_ARRAYLIT:
          pushall(cur->cast<ArrayLit>()->_v);
          cur->cast<ArrayLit>()->_dims.mark();
          break;
        case Expression::E_ARRAYACCESS:
          pushstack(cur->cast<ArrayAccess>()->_v);
          pushall(cur->cast<ArrayAccess>()->_idx);
          break;
        case Expression::E_COMP:
          pushstack(cur->cast<Comprehension>()->_e);
          pushstack(cur->cast<Comprehension>()->_where);
          pushall(cur->cast<Comprehension>()->_g);
          cur->cast<Comprehension>()->_g_idx.mark();
          break;
        case Expression::E_ITE:
          pushstack(cur->cast<ITE>()->_e_else);
          pushall(cur->cast<ITE>()->_e_if_then);
          break;
        case Expression::E_BINOP:
          pushstack(cur->cast<BinOp>()->_e0);
          pushstack(cur->cast<BinOp>()->_e1);
          break;
        case Expression::E_UNOP:
          pushstack(cur->cast<UnOp>()->_e0);
          break;
        case Expression::E_CALL:
          cur->cast<Call>()->_id.mark();
          pushall(cur->cast<Call>()->_args);
          if (FunctionI* fi = cur->cast<Call>()->_decl) {
            fi->_id.mark();
            pushstack(fi->_ti);
            pushstack(fi->_ann);
            pushstack(fi->_e);
            pushall(fi->_params);
          }
          break;
        case Expression::E_VARDECL:
          pushstack(cur->cast<VarDecl>()->_ti);
          pushstack(cur->cast<VarDecl>()->_e);
          cur->cast<VarDecl>()->_id.mark();
          break;
        case Expression::E_LET:
          pushall(cur->cast<Let>()->_let);
          pushstack(cur->cast<Let>()->_in);
          break;
        case Expression::E_ANN:
          pushstack(cur->cast<Annotation>()->_e);
          pushstack(cur->cast<Annotation>()->_a);
          break;
        case Expression::E_TI:
          pushstack(cur->cast<TypeInst>()->_domain);
          pushall(cur->cast<TypeInst>()->_ranges);
          break;
        case Expression::E_TIID:
          cur->cast<TIId>()->_v.mark();
          break;
        }
      }
    }
  }
#undef pushstack
#undef pushall

  void
  IntLit::rehash(void) {
    init_hash();
    std::hash<IntVal> h;
    cmb_hash(h(_v));
  }
  IntLit*
  IntLit::a(const Location& loc, IntVal v) {
    IntLit* il = new IntLit(loc,v);
    il->rehash();
    return il;
  }

  void
  FloatLit::rehash(void) {
    init_hash();
    std::hash<FloatVal> h;
    cmb_hash(h(_v));
  }
  FloatLit*
  FloatLit::a(const Location& loc, FloatVal v) {
    FloatLit* fl = new FloatLit(loc,v);
    fl->rehash();
    return fl;
  }

  void
  SetLit::rehash(void) {
    init_hash();
    if (_isv) {
      std::hash<IntVal> h;
      for (IntSetRanges r0(_isv); r0(); ++r0) {
        cmb_hash(h(r0.min()));
        cmb_hash(h(r0.max()));
      }
    } else {
      for (unsigned int i=_v.size(); i--;)
        cmb_hash(Expression::hash(_v[i]));
    }
  }
  SetLit*
  SetLit::a(const Location& loc,
            const std::vector<Expression*>& v) {
    SetLit* sl = new SetLit(loc);
    sl->_v = ASTExprVec<Expression>(v);
    sl->_isv = NULL;
    sl->rehash();
    return sl;
  }
  SetLit*
  SetLit::a(const Location& loc,
            ASTExprVec<Expression> v) {
    SetLit* sl = new SetLit(loc);
    sl->_v = v;
    sl->_isv = NULL;
    sl->rehash();
    return sl;
  }
  SetLit*
  SetLit::a(const Location& loc,
            IntSetVal* isv) {
    SetLit* sl = new SetLit(loc);
    sl->_isv = isv;
    sl->_type = Type::parsetint();
    sl->rehash();
    return sl;
  }

  void
  BoolLit::rehash(void) {
    init_hash();
    std::hash<bool> h;
    cmb_hash(h(_v));
  }
  BoolLit*
  BoolLit::a(const Location& loc, bool v) {
    BoolLit* bl = new BoolLit(loc,v);
    bl->rehash();
    return bl;
  }

  void
  StringLit::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }
  StringLit*
  StringLit::a(const Location& loc, const std::string& v) {
    StringLit* sl = new StringLit(loc,ASTString(v));
    sl->rehash();
    return sl;
  }
  StringLit*
  StringLit::a(const Location& loc, const ASTString& v) {
    StringLit* sl = new StringLit(loc,v);
    sl->rehash();
    return sl;
  }

  void
  Id::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }
  Id*
  Id::a(const Location& loc, const std::string& v, VarDecl* decl) {
    Id* id = new Id(loc,ASTString(v),decl);
    id->rehash();
    return id;
  }
  Id*
  Id::a(const Location& loc, const ASTString& v, VarDecl* decl) {
    Id* id = new Id(loc,v,decl);
    id->rehash();
    return id;
  }

  void
  TIId::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }
  TIId*
  TIId::a(const Location& loc, const std::string& v) {
    TIId* t = new TIId(loc,ASTString(v));
    t->rehash();
    return t;
  }

  void
  AnonVar::rehash(void) {
    init_hash();
  }
  AnonVar*
  AnonVar::a(const Location& loc) {
    AnonVar* av = new AnonVar(loc);
    av->init_hash();
    return av;
  }

  void
  ArrayLit::rehash(void) {
    init_hash();
    std::hash<int> h;
    for (unsigned int i=0; i<_dims.size(); i+=2) {
      cmb_hash(h(_dims[i]));
      cmb_hash(h(_dims[i+1]));
    }
    for (unsigned int i=_v.size(); i--;)
      cmb_hash(Expression::hash(_v[i]));
  }
  ArrayLit*
  ArrayLit::a(const Location& loc,
              const std::vector<Expression*>& v,
              const std::vector<pair<int,int> >& dims) {
    ArrayLit* al = new ArrayLit(loc);
    std::vector<int> _dims(dims.size()*2);
    for (unsigned int i=dims.size(); i--;) {
      _dims[i*2] = dims[i].first;
      _dims[i*2+1] = dims[i].second;
    }
    al->_v = ASTExprVec<Expression>(v);
    al->_dims = ASTIntVec(_dims);
    al->rehash();
    return al;
  }
  ArrayLit*
  ArrayLit::a(const Location& loc,
              ASTExprVec<Expression> v,
              const std::vector<pair<int,int> >& dims) {
    ArrayLit* al = new ArrayLit(loc);
    std::vector<int> _dims(dims.size()*2);
    for (unsigned int i=dims.size(); i--;) {
      _dims[i*2] = dims[i].first;
      _dims[i*2+1] = dims[i].second;
    }
    al->_v = v;
    al->_dims = ASTIntVec(_dims);
    al->rehash();
    return al;
  }
  ArrayLit*
  ArrayLit::a(const Location& loc,
              const std::vector<Expression*>& v) {
    std::vector<pair<int,int> > dims;
    dims.push_back(pair<int,int>(1,v.size()));
    return a(loc,v,dims);
  }
  ArrayLit*
  ArrayLit::a(const Location& loc,
              const std::vector<std::vector<Expression*> >& v) {
    std::vector<pair<int,int> > dims;
    dims.push_back(pair<int,int>(1,v.size()));
    dims.push_back(pair<int,int>(1,v[0].size()));
    std::vector<Expression*> vv;
    for (const std::vector<Expression*>& evi : v)
      for (Expression* ei : evi)
        vv.push_back(ei);
    return a(loc,vv,dims);
  }
  int
  ArrayLit::dims(void) const {
    return _dims.size()/2;
  }
  int
  ArrayLit::min(int i) const {
    return _dims[2*i];
  }
  int
  ArrayLit::max(int i) const {
    return _dims[2*i+1];
  }
  int
  ArrayLit::length(void) const {
    if(dims() == 0) return 0;
    int l = max(0) - min(0) + 1;
    for(unsigned int i=1; i<dims(); i++)
      l *= (max(i) - min(i) + 1);
    return l;
  }

  void
  ArrayAccess::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_v));
    std::hash<unsigned int> h;
    cmb_hash(h(_idx.size()));
    for (unsigned int i=_idx.size(); i--;)
      cmb_hash(Expression::hash(_idx[i]));
  }
  ArrayAccess*
  ArrayAccess::a(const Location& loc,
                 Expression* v,
                 const std::vector<Expression*>& idx) {
    ArrayAccess* aa = new ArrayAccess(loc);
    aa->_v = v;
    aa->_idx = ASTExprVec<Expression>(idx);
    aa->rehash();
    return aa;
  }
  ArrayAccess*
  ArrayAccess::a(const Location& loc,
                 Expression* v,
                 ASTExprVec<Expression> idx) {
    ArrayAccess* aa = new ArrayAccess(loc);
    aa->_v = v;
    aa->_idx = idx;
    aa->rehash();
    return aa;
  }

  Generator::Generator(const std::vector<ASTString>& v,
                       Expression* in) {
    std::vector<VarDecl*> vd;
    for (const ASTString& vdi : v)
      vd.push_back(VarDecl::a(in->_loc,
        TypeInst::a(in->_loc,Type::parint()),vdi,
        IntLit::a(in->_loc,0)));
    _v = vd;
    _in = in;
  }
  Generator::Generator(const std::vector<std::string>& v,
                       Expression* in) {
    std::vector<VarDecl*> vd;
    for (const std::string& vdi : v)
      vd.push_back(VarDecl::a(in->_loc,
        TypeInst::a(in->_loc,Type::parint()),ASTString(vdi),
        IntLit::a(in->_loc,0)));
    _v = vd;
    _in = in;
  }
  Generator::Generator(const std::vector<VarDecl*>& v,
                       Expression* in) {
    _v = v;
    _in = in;
  }

  bool
  Comprehension::set(void) const {
    return _flag_1;
  }
  void
  Comprehension::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    cmb_hash(h(set()));
    cmb_hash(Expression::hash(_e));
    cmb_hash(Expression::hash(_where));
    cmb_hash(h(_g_idx.size()));
    for (unsigned int i=_g_idx.size(); i--;) {
      cmb_hash(h(_g_idx[i]));
    }
    cmb_hash(h(_g.size()));
    for (unsigned int i=_g.size(); i--;) {
      cmb_hash(Expression::hash(_g[i]));
    }
  }
  Comprehension*
  Comprehension::a(const Location& loc,
                   Expression* e,
                   Generators& g,
                   bool set) {
    Comprehension* c = new Comprehension(loc);
    c->_e = e;
    std::vector<Expression*> es;
    std::vector<int> idx;
    for (unsigned int i=g._g.size(); i--;) {
      idx.push_back(es.size());
      es.push_back(g._g[i]._in);
      for (unsigned int j=g._g[i]._v.size(); j--;) {
        es.push_back(g._g[i]._v[j]);
      }
    }
    idx.push_back(es.size());
    c->_g = ASTExprVec<Expression>(es);
    c->_g_idx = ASTIntVec(idx);
    c->_where = g._w;
    c->_flag_1 = set;
    c->rehash();
    return c;
  }

  void
  ITE::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    cmb_hash(h(_e_if_then.size()));
    for (unsigned int i=_e_if_then.size(); i--; ) {
      cmb_hash(Expression::hash(_e_if_then[i]));
    }
    cmb_hash(Expression::hash(_e_else));
  }
  ITE*
  ITE::a(const Location& loc,
         const std::vector<Expression*>& e_if_then, Expression* e_else) {
    ITE* ite = new ITE(loc);
    ite->_e_if_then = ASTExprVec<Expression>(e_if_then);
    ite->_e_else = e_else;
    ite->rehash();
    return ite;
  }

  BinOpType
  BinOp::op(void) const {
    return static_cast<BinOpType>(_sec_id);
  }
  void
  BinOp::rehash(void) {
    init_hash();
    std::hash<int> h;
    cmb_hash(h(static_cast<int>(op())));
    cmb_hash(Expression::hash(_e0));
    cmb_hash(Expression::hash(_e1));
  }
  BinOp*
  BinOp::a(const Location& loc,
           Expression* e0, BinOpType op, Expression* e1) {
    BinOp* bo = new BinOp(loc,e0,op,e1);
    bo->rehash();
    return bo;
  }

  namespace {
    class OpToString {
    public:
      ASTString sBOT_PLUS;
      ASTString sBOT_MINUS;
      ASTString sBOT_MULT;
      ASTString sBOT_DIV;
      ASTString sBOT_IDIV;
      ASTString sBOT_MOD;
      ASTString sBOT_LE;
      ASTString sBOT_LQ;
      ASTString sBOT_GR;
      ASTString sBOT_GQ;
      ASTString sBOT_EQ;
      ASTString sBOT_NQ;
      ASTString sBOT_IN;
      ASTString sBOT_SUBSET;
      ASTString sBOT_SUPERSET;
      ASTString sBOT_UNION;
      ASTString sBOT_DIFF;
      ASTString sBOT_SYMDIFF;
      ASTString sBOT_INTERSECT;
      ASTString sBOT_PLUSPLUS;
      ASTString sBOT_EQUIV;
      ASTString sBOT_IMPL;
      ASTString sBOT_RIMPL;
      ASTString sBOT_OR;
      ASTString sBOT_AND;
      ASTString sBOT_XOR;
      ASTString sBOT_DOTDOT;
      ASTString sBOT_NOT;
      
      OpToString(void) {
        GCLock lock;
        sBOT_PLUS = ASTString("@+");
        sBOT_MINUS = ASTString("@-");
        sBOT_MULT = ASTString("@*");
        sBOT_DIV = ASTString("@/");
        sBOT_IDIV = ASTString("@div");
        sBOT_MOD = ASTString("@mod");
        sBOT_LE = ASTString("@<");
        sBOT_LQ = ASTString("@<=");
        sBOT_GR = ASTString("@>");
        sBOT_GQ = ASTString("@>=");
        sBOT_EQ = ASTString("@=");
        sBOT_NQ = ASTString("@!=");
        sBOT_IN = ASTString("@in");
        sBOT_SUBSET = ASTString("@subset");
        sBOT_SUPERSET = ASTString("@superset");
        sBOT_UNION = ASTString("@union");
        sBOT_DIFF = ASTString("@diff");
        sBOT_SYMDIFF = ASTString("@symdiff");
        sBOT_INTERSECT = ASTString("@intersect");
        sBOT_PLUSPLUS = ASTString("@++");
        sBOT_EQUIV = ASTString("@<->");
        sBOT_IMPL = ASTString("@->");
        sBOT_RIMPL = ASTString("@<-");
        sBOT_OR = ASTString("@\\/");
        sBOT_AND = ASTString("@/\\");
        sBOT_XOR = ASTString("@xor");
        sBOT_DOTDOT = ASTString("@..");
        sBOT_NOT = ASTString("@not");
      }
      
      static OpToString& o(void) {
        static OpToString _o;
        return _o;
      }
    };
  }

  ASTString
  BinOp::opToString(void) const {
    switch (op()) {
    case BOT_PLUS: return OpToString::o().sBOT_PLUS;
    case BOT_MINUS: return OpToString::o().sBOT_MINUS;
    case BOT_MULT: return OpToString::o().sBOT_MULT;
    case BOT_DIV: return OpToString::o().sBOT_DIV;
    case BOT_IDIV: return OpToString::o().sBOT_IDIV;
    case BOT_MOD: return OpToString::o().sBOT_MOD;
    case BOT_LE: return OpToString::o().sBOT_LE;
    case BOT_LQ: return OpToString::o().sBOT_LQ;
    case BOT_GR: return OpToString::o().sBOT_GR;
    case BOT_GQ: return OpToString::o().sBOT_GQ;
    case BOT_EQ: return OpToString::o().sBOT_EQ;
    case BOT_NQ: return OpToString::o().sBOT_NQ;
    case BOT_IN: return OpToString::o().sBOT_IN;
    case BOT_SUBSET: return OpToString::o().sBOT_SUBSET;
    case BOT_SUPERSET: return OpToString::o().sBOT_SUPERSET;
    case BOT_UNION: return OpToString::o().sBOT_UNION;
    case BOT_DIFF: return OpToString::o().sBOT_DIFF;
    case BOT_SYMDIFF: return OpToString::o().sBOT_SYMDIFF;
    case BOT_INTERSECT: return OpToString::o().sBOT_INTERSECT;
    case BOT_PLUSPLUS: return OpToString::o().sBOT_PLUSPLUS;
    case BOT_EQUIV: return OpToString::o().sBOT_EQUIV;
    case BOT_IMPL: return OpToString::o().sBOT_IMPL;
    case BOT_RIMPL: return OpToString::o().sBOT_RIMPL;
    case BOT_OR: return OpToString::o().sBOT_OR;
    case BOT_AND: return OpToString::o().sBOT_AND;
    case BOT_XOR: return OpToString::o().sBOT_XOR;
    case BOT_DOTDOT: return OpToString::o().sBOT_DOTDOT;
    default: assert(false);
    }
  }

  UnOpType
  UnOp::op(void) const {
    return static_cast<UnOpType>(_sec_id);
  }
  void
  UnOp::rehash(void) {
    init_hash();
    std::hash<int> h;
    cmb_hash(h(static_cast<int>(_sec_id)));
    cmb_hash(Expression::hash(_e0));
  }
  UnOp*
  UnOp::a(const Location& loc, UnOpType op, Expression* e) {
    UnOp* uo = new UnOp(loc,op,e);
    uo->rehash();
    return uo;
  }

  ASTString
  UnOp::opToString(void) const {
    switch (op()) {
    case UOT_PLUS: return OpToString::o().sBOT_PLUS;
    case UOT_MINUS: return OpToString::o().sBOT_MINUS;
    case UOT_NOT: return OpToString::o().sBOT_NOT;
    default: assert(false);
    }
  }

  void
  Call::rehash(void) {
    init_hash();
    cmb_hash(_id.hash());
    std::hash<FunctionI*> hf;
    cmb_hash(hf(_decl));
    std::hash<unsigned int> hu;
    cmb_hash(hu(_args.size()));
    for (unsigned int i=_args.size(); i--;)
      cmb_hash(Expression::hash(_args[i]));
  }
  Call*
  Call::a(const Location& loc,
          const std::string& id,
          const std::vector<Expression*>& args,
          FunctionI* decl) {
    Call* c = new Call(loc);
    c->_id = ASTString(id);
    c->_args = ASTExprVec<Expression>(args);
    c->_decl = decl;
    c->rehash();
    return c;
  }
  Call*
  Call::a(const Location& loc,
          const ASTString& id,
          const std::vector<Expression*>& args,
          FunctionI* decl) {
    Call* c = new Call(loc);
    c->_id = id;
    c->_args = ASTExprVec<Expression>(args);
    c->_decl = decl;
    c->rehash();
    return c;
  }

  void
  VarDecl::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_ti));
    cmb_hash(_id.hash());
    cmb_hash(Expression::hash(_e));
  }
  VarDecl*
  VarDecl::a(const Location& loc,
             TypeInst* ti, const ASTString& id, Expression* e) {
    VarDecl* v = new VarDecl(loc,ti->_type);
    v->_ti = ti;
    v->_id = id;
    v->_e = e;
    v->rehash();
    return v;
  }
  VarDecl*
  VarDecl::a(const Location& loc,
             TypeInst* ti, const std::string& id, Expression* e) {
    return a(loc,ti,ASTString(id));
  }
  bool
  VarDecl::toplevel(void) const {
    return _flag_1;
  }
  void
  VarDecl::toplevel(bool t) {
    _flag_1 = t;
  }
  bool
  VarDecl::introduced(void) const {
    return _flag_2;
  }
  void
  VarDecl::introduced(bool t) {
    _flag_2 = t;
  }

  void
  Let::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_in));
    std::hash<unsigned int> h;
    cmb_hash(h(_let.size()));
    for (unsigned int i=_let.size(); i--;)
      cmb_hash(Expression::hash(_let[i]));
  }
  Let*
  Let::a(const Location& loc,
         const std::vector<Expression*>& let, Expression* in) {
    Let* l = new Let(loc);
    l->_let = ASTExprVec<Expression>(let);
    l->_in = in;
    l->rehash();
    return l;
  }
  void
  Let::pushbindings(void) {
    GC::mark();
    for (unsigned int i=_let.size(); i--;) {
      if (_let[i]->isa<VarDecl>()) {
        GC::trail(reinterpret_cast<void**>(&_let[i]),_let[i]);
      }
    }
  }
  void
  Let::popbindings(void) {
    GC::untrail();
  }

  void
  TypeInst::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    unsigned int rsize = _ranges.size();
    cmb_hash(h(rsize));
    for (unsigned int i=rsize; i--;)
      cmb_hash(Expression::hash(_ranges[i]));
    cmb_hash(Expression::hash(_domain));
  }
  TypeInst*
  TypeInst::a(const Location& loc,
              const Type& type, 
              ASTExprVec<TypeInst> ranges,
              Expression* domain) {
    TypeInst* t = new TypeInst(loc,type,ranges,domain);
    t->rehash();
    return t;
  }
  TypeInst*
  TypeInst::a(const Location& loc,
              const Type& type, 
              Expression* domain) {
    TypeInst* t = new TypeInst(loc,type,domain);
    t->rehash();
    return t;
  }

  void
  TypeInst::addRanges(const std::vector<TypeInst*>& ranges) {
    assert(_ranges.size() == 0);
    _ranges = ASTExprVec<TypeInst>(ranges);
    if (ranges.size()==1 && ranges[0] && ranges[0]->isa<TypeInst>() &&
        ranges[0]->cast<TypeInst>()->_domain &&
        ranges[0]->cast<TypeInst>()->_domain->isa<TIId>())
      _type._dim=-1;
    else
      _type._dim=ranges.size();
    rehash();
  }

  bool
  TypeInst::hasTiVariable(void) const {
    if (_domain && _domain->isa<TIId>())
      return true;
    if (_ranges.size()==1 &&
        _ranges[0]->isa<TIId>())
      return true;
    return false;
  }

  IncludeI*
  IncludeI::a(const Location& loc, const ASTString& f) {
    IncludeI* i = new IncludeI(loc);
    i->_f = f;
    i->_m = NULL;
    return i;
  }

  VarDeclI*
  VarDeclI::a(const Location& loc, VarDecl* e) {
    VarDeclI* vi = new VarDeclI(loc);
    vi->_e = e;
    return vi;
  }

  AssignI*
  AssignI::a(const Location& loc, const std::string& id, Expression* e) {
    AssignI* ai = new AssignI(loc);
    ai->_id = ASTString(id);
    ai->_e = e;
    ai->_decl = NULL;
    return ai;
  }

  ConstraintI*
  ConstraintI::a(const Location& loc, Expression* e) {
    ConstraintI* ci = new ConstraintI(loc);
    ci->_e = e;
    return ci;
  }

  SolveI*
  SolveI::sat(const Location& loc, Annotation* ann) {
    SolveI* si = new SolveI(loc);
    si->_ann = ann;
    si->_e = NULL;
    si->_sec_id = ST_SAT;
    return si;
  }
  SolveI*
  SolveI::min(const Location& loc, Expression* e, Annotation* ann) {
    SolveI* si = new SolveI(loc);
    si->_ann = ann;
    si->_e = e;
    si->_sec_id = ST_MIN;
    return si;
  }
  SolveI*
  SolveI::max(const Location& loc, Expression* e, Annotation* ann) {
    SolveI* si = new SolveI(loc);
    si->_ann = ann;
    si->_e = e;
    si->_sec_id = ST_MAX;
    return si;
  }
  SolveI::SolveType
  SolveI::st(void) const {
    return static_cast<SolveType>(_sec_id);
  }
  void
  SolveI::st(SolveI::SolveType s) {
    _sec_id = s;
  }

  OutputI*
  OutputI::a(const Location& loc, Expression* e) {
    OutputI* oi = new OutputI(loc);
    oi->_e = e;
    return oi;
  }

  FunctionI*
  FunctionI::a(const Location& loc,
               const std::string& id, TypeInst* ti,
               const std::vector<VarDecl*>& params,
               Expression* e, Annotation* ann) {
    FunctionI* fi = new FunctionI(loc);
    fi->_id = ASTString(id);
    fi->_ti = ti;
    fi->_params = ASTExprVec<VarDecl>(params);
    fi->_ann = ann;
    fi->_e = e;
    // fi->_builtins.e = NULL;
    // fi->_builtins.b = NULL;
    // fi->_builtins.f = NULL;
    // fi->_builtins.i = NULL;
    return fi;
  }

  Type
  FunctionI::rtype(const std::vector<Expression*>& ta) {
    Type ret = _ti->_type;
    ASTString dh;
    if (_ti->_domain && _ti->_domain->isa<TIId>())
      dh = _ti->_domain->cast<TIId>()->_v;
    ASTString rh;
    if (_ti->_ranges.size()==1 &&
        _ti->_ranges[0] && _ti->_ranges[0]->isa<TIId>())
      rh = _ti->_ranges[0]->cast<TIId>()->_v;

    ASTStringMap<Type>::t tmap;
    for (unsigned int i=0; i<ta.size(); i++) {
      TypeInst* tii = _params[i]->_ti;
      if (tii->_domain && tii->_domain->isa<TIId>()) {
        ASTString tiid = tii->_domain->cast<TIId>()->_v;
        Type tiit = ta[i]->_type;
        tiit._dim=0;
        ASTStringMap<Type>::t::iterator it = tmap.find(tiid);
        if (it==tmap.end()) {
          tmap.insert(std::pair<ASTString,Type>(tiid,tiit));
        } else {
          if (it->second._dim > 0) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" used in both array and non-array position");
          } else {
            Type tiit_par = tiit; tiit_par._ti = Type::TI_PAR;
            Type its_par = it->second; its_par._ti = Type::TI_PAR;
            if (tiit_par._bt==Type::BT_BOT) {
              tiit_par._bt = its_par._bt;
            }
            if (its_par._bt==Type::BT_BOT) {
              its_par._bt = tiit_par._bt;
            }
            if (tiit_par != its_par) {
              throw TypeError(ta[i]->_loc,"type-inst variable $"+
                tiid.str()+" instantiated with different types ("+
                tiit.toString()+" vs "+
                it->second.toString()+")");
            }
            if (tiit.isvar())
              it->second._ti = Type::TI_VAR;
            if (it->second._bt == Type::BT_BOT)
              it->second._bt = tiit._bt;
          }
        }
      }
      if (tii->_ranges.size()==1 &&
          tii->_ranges[0]->_domain && 
          tii->_ranges[0]->_domain->isa<TIId>()) {
        ASTString tiid = tii->_ranges[0]->_domain->cast<TIId>()->_v;
        if (ta[i]->_type._dim<=0) {
          assert(false);
          throw TypeError(ta[i]->_loc,"type-inst variable $"+tiid.str()+
            " must be an array index");
        }
        Type tiit = Type::any(ta[i]->_type._dim);
        ASTStringMap<Type>::t::iterator it = tmap.find(tiid);
        if (it==tmap.end()) {
          tmap.insert(std::pair<ASTString,Type>(tiid,tiit));
        } else {
          if (it->second._dim == 0) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" used in both array and non-array position");
          } else if (it->second!=tiit) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" instantiated with different types ("+
              tiit.toString()+" vs "+
              it->second.toString()+")");
          }
        }
      }
    }
    if (dh.size() != 0) {
      ASTStringMap<Type>::t::iterator it = tmap.find(dh);
      if (it==tmap.end())
        throw TypeError(_loc,"type-inst variable $"+dh.str()+" used but not defined");
      ret._bt = it->second._bt;
      if (ret._ti==Type::TI_ANY)
        ret._ti = it->second._ti;
      if (ret._st==Type::ST_PLAIN)
        ret._st = it->second._st;
    } 
    if (rh.size() != 0) {
      ASTStringMap<Type>::t::iterator it = tmap.find(rh);
      if (it==tmap.end())
        throw TypeError(_loc,"type-inst variable $"+rh.str()+" used but not defined");
      ret._dim = it->second._dim;
    }
    return ret;
  }

  bool
  Expression::equal(const Expression* e0, const Expression* e1) {
    if (e0==e1) return true;
    if (e0 == NULL || e1 == NULL) return false;
    if (e0->_id != e1->_id) return false;
    if (e0->_type != e1->_type) return false;
    switch (e0->_id) {
    case Expression::E_INTLIT:
      return e0->cast<IntLit>()->_v == e1->cast<IntLit>()->_v;
    case Expression::E_FLOATLIT:
      return e0->cast<FloatLit>()->_v == e1->cast<FloatLit>()->_v;
    case Expression::E_SETLIT:
      {
        const SetLit* s0 = e0->cast<SetLit>();
        const SetLit* s1 = e1->cast<SetLit>();
        if (s0->_isv) {
          if (s1->_isv) {
            IntSetRanges r0(s0->_isv);
            IntSetRanges r1(s1->_isv);
            return Ranges::equal(r0,r1);
          } else {
            return false;
          }
        } else {
          if (s1->_isv) return false;
          if (s0->_v.size() != s1->_v.size()) return false;
          for (unsigned int i=0; i<s0->_v.size(); i++)
            if (!Expression::equal( s0->_v[i], s1->_v[i] ))
              return false;
          return true;
        }
      }
    case Expression::E_BOOLLIT:
      return e0->cast<BoolLit>()->_v == e1->cast<BoolLit>()->_v;
    case Expression::E_STRINGLIT:
      return e0->cast<StringLit>()->_v == e1->cast<StringLit>()->_v;
    case Expression::E_ID:
      // assert(e0->cast<Id>()->_decl != NULL);
      return e0->cast<Id>()->_v == e1->cast<Id>()->_v;
    case Expression::E_ANON:
      return false;
    case Expression::E_ARRAYLIT:
      {
        const ArrayLit* a0 = e0->cast<ArrayLit>();
        const ArrayLit* a1 = e1->cast<ArrayLit>();
        if (a0->_v.size() != a1->_v.size()) return false;
        if (a0->_dims.size() != a1->_dims.size()) return false;
        for (unsigned int i=0; i<a0->_dims.size(); i++)
          if ( a0->_dims[i] != a1->_dims[i] ) return false;
        for (unsigned int i=0; i<a0->_v.size(); i++)
          if (!Expression::equal( a0->_v[i], a1->_v[i] ))
            return false;
        return true;
      }
    case Expression::E_ARRAYACCESS:
      {
        const ArrayAccess* a0 = e0->cast<ArrayAccess>();
        const ArrayAccess* a1 = e1->cast<ArrayAccess>();
        if (!Expression::equal( a0->_v, a1->_v )) return false;
        if (a0->_idx.size() != a1->_idx.size()) return false;
        for (unsigned int i=0; i<a0->_idx.size(); i++)
          if (!Expression::equal( a0->_idx[i], a1->_idx[i] ))
            return false;
        return true;
      }
    case Expression::E_COMP:
      {
        const Comprehension* c0 = e0->cast<Comprehension>();
        const Comprehension* c1 = e1->cast<Comprehension>();
        if (c0->set() != c1->set()) return false;
        if (!Expression::equal ( c0->_e, c1->_e )) return false;
        if (!Expression::equal ( c0->_where, c1->_where )) return false;
        if (c0->_g.size() != c1->_g.size()) return false;
        for (unsigned int i=0; i<c0->_g.size(); i++) {
          if (!Expression::equal( c0->_g[i], c1->_g[i] ))
            return false;
        }
        for (unsigned int i=0; i<c0->_g_idx.size(); i++) {
          if (c0->_g_idx[i] != c1->_g_idx[i])
            return false;
        }
        return true;
      }
    case Expression::E_ITE:
      {
        const ITE* i0 = e0->cast<ITE>();
        const ITE* i1 = e1->cast<ITE>();
        if (i0->_e_if_then.size() != i1->_e_if_then.size()) return false;
        for (unsigned int i=i0->_e_if_then.size(); i--; ) {
          if (!Expression::equal ( i0->_e_if_then[i],
                                   i1->_e_if_then[i]))
            return false;
        }
        if (!Expression::equal (i0->_e_else, i1->_e_else)) return false;
        return true;
      }
    case Expression::E_BINOP:
      {
        const BinOp* b0 = e0->cast<BinOp>();
        const BinOp* b1 = e1->cast<BinOp>();
        if (b0->op() != b1->op()) return false;
        if (!Expression::equal (b0->_e0, b1->_e0)) return false;
        if (!Expression::equal (b0->_e1, b1->_e1)) return false;
        return true;
      }
    case Expression::E_UNOP:
      {
        const UnOp* b0 = e0->cast<UnOp>();
        const UnOp* b1 = e1->cast<UnOp>();
        if (b0->op() != b1->op()) return false;
        if (!Expression::equal (b0->_e0, b1->_e0)) return false;
        return true;
      }
    case Expression::E_CALL:
      {
        const Call* c0 = e0->cast<Call>();
        const Call* c1 = e1->cast<Call>();
        if (c0->_id != c1->_id) return false;
        if (c0->_decl != c1->_decl) return false;
        if (c0->_args.size() != c1->_args.size()) return false;
        for (unsigned int i=0; i<c0->_args.size(); i++)
          if (!Expression::equal ( c0->_args[i], c1->_args[i] ))
            return false;
        return true;
      }
    case Expression::E_VARDECL:
      {
        const VarDecl* v0 = e0->cast<VarDecl>();
        const VarDecl* v1 = e1->cast<VarDecl>();
        if (!Expression::equal ( v0->_ti, v1->_ti )) return false;
        if (v0->_id != v1->_id) return false;
        if (!Expression::equal ( v0->_e, v1->_e )) return false;
        return true;
      }
    case Expression::E_LET:
      {
        const Let* l0 = e0->cast<Let>();
        const Let* l1 = e1->cast<Let>();
        if (!Expression::equal ( l0->_in, l1->_in )) return false;
        if (l0->_let.size() != l1->_let.size()) return false;
        for (unsigned int i=l0->_let.size(); i--;)
          if (!Expression::equal ( l0->_let[i], l1->_let[i]))
            return false;
        return true;
      }
    case Expression::E_ANN:
      {
        const Annotation* a0 = e0->cast<Annotation>();
        const Annotation* a1 = e1->cast<Annotation>();
        if (!Expression::equal ( a0->_e, a1->_e )) return false;
        if (!Expression::equal ( a0->_a, a1->_a )) return false;
        return true;
      }
    case Expression::E_TI:
      {
        const TypeInst* t0 = e0->cast<TypeInst>();
        const TypeInst* t1 = e1->cast<TypeInst>();
        if (t0->_ranges.size() != t1->_ranges.size()) return false;
        for (unsigned int i=t0->_ranges.size(); i--;)
          if (!Expression::equal ( t0->_ranges[i], t1->_ranges[i]))
            return false;
        if (!Expression::equal (t0->_domain, t1->_domain)) return false;
        return true;
      }
    case Expression::E_TIID:
      return false;
    default:
      assert(false);
      return false;
    }
  }    

}
