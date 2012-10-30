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
    l.filename = NULL;
    l.first_line = 0;
    l.first_column = 0;
    l.last_line = 0;
    l.last_column = 0;
    return l;
  }

  std::string
  Location::toString(void) const {
    std::ostringstream oss;
    oss << (filename ? filename->str() : "") << ":" << first_line << "." << first_column;
    return oss.str();
  }

  void
  Annotation::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_e));
    cmb_hash(Expression::hash(_a));
  }
  Annotation*
  Annotation::a(const ASTContext& ctx, const Location& loc,
                Expression* e) {
    Annotation* a = new (ctx) Annotation(loc,e);
    a->rehash();
    return a;
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

  void
  IntLit::rehash(void) {
    init_hash();
    std::hash<IntVal> h;
    cmb_hash(h(_v));
  }
  IntLit*
  IntLit::a(const ASTContext& ctx, const Location& loc,
            IntVal v) {
    IntLit* il = new (ctx) IntLit(loc,v);
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
  FloatLit::a(const ASTContext& ctx, const Location& loc,
              FloatVal v) {
    FloatLit* fl = new (ctx) FloatLit(loc,v);
    fl->rehash();
    return fl;
  }

  void
  SetLit::rehash(void) {
    init_hash();
    if (_v) {
      for (unsigned int i=_v->size(); i--;)
        cmb_hash(Expression::hash((*_v)[i]));
    } else {
      std::hash<IntVal> h;
      for (IntSetRanges r0(_isv); r0(); ++r0) {
        cmb_hash(h(r0.min()));
        cmb_hash(h(r0.max()));
      }
    }
  }
  SetLit*
  SetLit::a(const ASTContext& ctx,
            const Location& loc,
            const std::vector<Expression*>& v) {
    SetLit* sl = new (ctx) SetLit(loc);
    sl->_v = CtxVec<Expression*>::a(ctx,v);
    sl->_isv = NULL;
    sl->rehash();
    return sl;
  }
  SetLit*
  SetLit::a(const ASTContext& ctx,
            const Location& loc,
            IntSetVal* isv) {
    SetLit* sl = new (ctx) SetLit(loc);
    sl->_v = NULL;
    sl->_isv = isv;
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
  BoolLit::a(const ASTContext& ctx, const Location& loc,
             bool v) {
    BoolLit* bl = new (ctx) BoolLit(loc,v);
    bl->rehash();
    return bl;
  }

  void
  StringLit::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }
  StringLit*
  StringLit::a(const ASTContext& ctx, const Location& loc,
               const std::string& v) {
    StringLit* sl = new (ctx) StringLit(loc,CtxStringH(ctx,v));
    sl->rehash();
    return sl;
  }

  void
  Id::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }
  Id*
  Id::a(const ASTContext& ctx, const Location& loc,
        const std::string& v, VarDecl* decl) {
    Id* id = new (ctx) Id(loc,CtxStringH(ctx,v),decl);
    id->rehash();
    return id;
  }

  void
  TIId::rehash(void) {
    init_hash();
    cmb_hash(_v.hash());
  }
  TIId*
  TIId::a(const ASTContext& ctx, const Location& loc,
          const std::string& v) {
    TIId* t = new (ctx) TIId(loc,CtxStringH(ctx,v));
    t->rehash();
    return t;
  }

  void
  AnonVar::rehash(void) {
    init_hash();
  }
  AnonVar*
  AnonVar::a(const ASTContext& ctx, const Location& loc) {
    AnonVar* av = new (ctx) AnonVar(loc);
    av->init_hash();
    return av;
  }

  void
  ArrayLit::rehash(void) {
    init_hash();
    std::hash<int> h;
    for (unsigned int i=0; i<_dims->size(); i++) {
      cmb_hash(h((*_dims)[i].first));
      cmb_hash(h((*_dims)[i].second));
    }
    for (unsigned int i=_v->size(); i--;)
      cmb_hash(Expression::hash((*_v)[i]));
  }
  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              const std::vector<Expression*>& v,
              const std::vector<pair<int,int> >& dims) {
    ArrayLit* al = new (ctx) ArrayLit(loc);
    al->_v = CtxVec<Expression*>::a(ctx,v);
    al->_dims = CtxVec<pair<int,int> >::a(ctx,dims);
    al->rehash();
    return al;
  }
  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              CtxVec<Expression*>* v,
              const std::vector<pair<int,int> >& dims) {
    ArrayLit* al = new (ctx) ArrayLit(loc);
    al->_v = v;
    al->_dims = CtxVec<pair<int,int> >::a(ctx,dims);
    al->rehash();
    return al;
  }
  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              const std::vector<Expression*>& v) {
    std::vector<pair<int,int> > dims;
    dims.push_back(pair<int,int>(1,v.size()));
    return a(ctx,loc,v,dims);
  }
  ArrayLit*
  ArrayLit::a(const ASTContext& ctx,
              const Location& loc,
              const std::vector<std::vector<Expression*> >& v) {
    std::vector<pair<int,int> > dims;
    dims.push_back(pair<int,int>(1,v.size()));
    dims.push_back(pair<int,int>(1,v[0].size()));
    std::vector<Expression*> vv;
    for (const std::vector<Expression*>& evi : v)
      for (Expression* ei : evi)
        vv.push_back(ei);
    return a(ctx,loc,vv,dims);
  }

  void
  ArrayAccess::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_v));
    std::hash<unsigned int> h;
    cmb_hash(h(_idx->size()));
    for (unsigned int i=_idx->size(); i--;)
      cmb_hash(Expression::hash((*_idx)[i]));
  }
  ArrayAccess*
  ArrayAccess::a(const ASTContext& ctx,
                 const Location& loc,
                 Expression* v,
                 const std::vector<Expression*>& idx) {
    ArrayAccess* aa = new (ctx) ArrayAccess(loc);
    aa->_v = v;
    aa->_idx = CtxVec<Expression*>::a(ctx,idx);
    aa->rehash();
    return aa;
  }

  Generator*
  Generator::a(const ASTContext& ctx,
               const std::vector<CtxStringH>& v,
               Expression* in) {
    Generator* g = new (ctx) Generator();
    std::vector<VarDecl*> vd;
    for (const CtxStringH& vdi : v)
      vd.push_back(VarDecl::a(ctx,in->_loc,
        TypeInst::a(ctx,in->_loc,Type::parint()),vdi,
        IntLit::a(ctx,in->_loc,0)));
    g->_v = CtxVec<VarDecl*>::a(ctx,vd);
    g->_in = in;
    return g;
  }
  Generator*
  Generator::a(const ASTContext& ctx,
               const std::vector<std::string>& v,
               Expression* in) {
    std::vector<CtxStringH> vv;
    for (const std::string& si : v)
      vv.push_back(CtxStringH(ctx,si));
    return a(ctx,vv,in);
  }

  void
  Comprehension::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    cmb_hash(h(_set));
    cmb_hash(Expression::hash(_e));
    cmb_hash(Expression::hash(_where));
    cmb_hash(h(_g->size()));
    for (unsigned int i=_g->size(); i--;) {
      cmb_hash(Expression::hash((*_g)[i]->_in));
      cmb_hash(h((*_g)[i]->_v->size()));
      for (unsigned int j=(*_g)[i]->_v->size(); j--;) {
        cmb_hash(Expression::hash((*(*_g)[i]->_v)[j]));
      }
    }
  }
  Comprehension*
  Comprehension::a(const ASTContext& ctx,
                   const Location& loc,
                   Expression* e,
                   Generators& g,
                   bool set) {
    Comprehension* c = new (ctx) Comprehension(loc);
    c->_e = e;
    c->_g = CtxVec<Generator*>::a(ctx,g._g);
    c->_where = g._w;
    c->_set = set;
    c->rehash();
    return c;
  }

  void
  ITE::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    cmb_hash(h(_e_if->size()));
    for (unsigned int i=_e_if->size(); i--; ) {
      cmb_hash(Expression::hash((*_e_if)[i].first));
      cmb_hash(Expression::hash((*_e_if)[i].second));
    }
    cmb_hash(Expression::hash(_e_else));
  }
  ITE*
  ITE::a(const ASTContext& ctx, const Location& loc,
         const std::vector<IfThen>& e_if, Expression* e_else) {
    ITE* ite = new (ctx) ITE(loc);
    ite->_e_if = CtxVec<IfThen>::a(ctx,e_if);
    ite->_e_else = e_else;
    ite->rehash();
    return ite;
  }

  void
  BinOp::rehash(void) {
    init_hash();
    std::hash<int> h;
    cmb_hash(h(static_cast<int>(_op)));
    cmb_hash(Expression::hash(_e0));
    cmb_hash(Expression::hash(_e1));
  }
  BinOp*
  BinOp::a(const ASTContext& ctx, const Location& loc,
           Expression* e0, BinOpType op, Expression* e1) {
    BinOp* bo = new (ctx) BinOp(loc,e0,op,e1);
    bo->rehash();
    return bo;
  }

  namespace {
    class OpToString {
    public:
      ASTContext ctx;
      
      CtxStringH sBOT_PLUS;
      CtxStringH sBOT_MINUS;
      CtxStringH sBOT_MULT;
      CtxStringH sBOT_DIV;
      CtxStringH sBOT_IDIV;
      CtxStringH sBOT_MOD;
      CtxStringH sBOT_LE;
      CtxStringH sBOT_LQ;
      CtxStringH sBOT_GR;
      CtxStringH sBOT_GQ;
      CtxStringH sBOT_EQ;
      CtxStringH sBOT_NQ;
      CtxStringH sBOT_IN;
      CtxStringH sBOT_SUBSET;
      CtxStringH sBOT_SUPERSET;
      CtxStringH sBOT_UNION;
      CtxStringH sBOT_DIFF;
      CtxStringH sBOT_SYMDIFF;
      CtxStringH sBOT_INTERSECT;
      CtxStringH sBOT_PLUSPLUS;
      CtxStringH sBOT_EQUIV;
      CtxStringH sBOT_IMPL;
      CtxStringH sBOT_RIMPL;
      CtxStringH sBOT_OR;
      CtxStringH sBOT_AND;
      CtxStringH sBOT_XOR;
      CtxStringH sBOT_DOTDOT;
      CtxStringH sBOT_NOT;
      
      OpToString(void) {
        sBOT_PLUS = CtxStringH(ctx, "@+");
        sBOT_MINUS = CtxStringH(ctx, "@-");
        sBOT_MULT = CtxStringH(ctx, "@*");
        sBOT_DIV = CtxStringH(ctx, "@/");
        sBOT_IDIV = CtxStringH(ctx, "@div");
        sBOT_MOD = CtxStringH(ctx, "@mod");
        sBOT_LE = CtxStringH(ctx, "@<");
        sBOT_LQ = CtxStringH(ctx, "@<=");
        sBOT_GR = CtxStringH(ctx, "@>");
        sBOT_GQ = CtxStringH(ctx, "@>=");
        sBOT_EQ = CtxStringH(ctx, "@=");
        sBOT_NQ = CtxStringH(ctx, "@!=");
        sBOT_IN = CtxStringH(ctx, "@in");
        sBOT_SUBSET = CtxStringH(ctx, "@subset");
        sBOT_SUPERSET = CtxStringH(ctx, "@superset");
        sBOT_UNION = CtxStringH(ctx, "@union");
        sBOT_DIFF = CtxStringH(ctx, "@diff");
        sBOT_SYMDIFF = CtxStringH(ctx, "@symdiff");
        sBOT_INTERSECT = CtxStringH(ctx, "@intersect");
        sBOT_PLUSPLUS = CtxStringH(ctx, "@++");
        sBOT_EQUIV = CtxStringH(ctx, "@<->");
        sBOT_IMPL = CtxStringH(ctx, "@->");
        sBOT_RIMPL = CtxStringH(ctx, "@<-");
        sBOT_OR = CtxStringH(ctx, "@\\/");
        sBOT_AND = CtxStringH(ctx, "@/\\");
        sBOT_XOR = CtxStringH(ctx, "@xor");
        sBOT_DOTDOT = CtxStringH(ctx, "@..");
        sBOT_NOT = CtxStringH(ctx, "@not");
      }
    } _opToString;
  }

  CtxStringH
  BinOp::opToString(void) const {
    switch (_op) {
    case BOT_PLUS: return _opToString.sBOT_PLUS;
    case BOT_MINUS: return _opToString.sBOT_MINUS;
    case BOT_MULT: return _opToString.sBOT_MULT;
    case BOT_DIV: return _opToString.sBOT_DIV;
    case BOT_IDIV: return _opToString.sBOT_IDIV;
    case BOT_MOD: return _opToString.sBOT_MOD;
    case BOT_LE: return _opToString.sBOT_LE;
    case BOT_LQ: return _opToString.sBOT_LQ;
    case BOT_GR: return _opToString.sBOT_GR;
    case BOT_GQ: return _opToString.sBOT_GQ;
    case BOT_EQ: return _opToString.sBOT_EQ;
    case BOT_NQ: return _opToString.sBOT_NQ;
    case BOT_IN: return _opToString.sBOT_IN;
    case BOT_SUBSET: return _opToString.sBOT_SUBSET;
    case BOT_SUPERSET: return _opToString.sBOT_SUPERSET;
    case BOT_UNION: return _opToString.sBOT_UNION;
    case BOT_DIFF: return _opToString.sBOT_DIFF;
    case BOT_SYMDIFF: return _opToString.sBOT_SYMDIFF;
    case BOT_INTERSECT: return _opToString.sBOT_INTERSECT;
    case BOT_PLUSPLUS: return _opToString.sBOT_PLUSPLUS;
    case BOT_EQUIV: return _opToString.sBOT_EQUIV;
    case BOT_IMPL: return _opToString.sBOT_IMPL;
    case BOT_RIMPL: return _opToString.sBOT_RIMPL;
    case BOT_OR: return _opToString.sBOT_OR;
    case BOT_AND: return _opToString.sBOT_AND;
    case BOT_XOR: return _opToString.sBOT_XOR;
    case BOT_DOTDOT: return _opToString.sBOT_DOTDOT;
    default: assert(false);
    }
  }

  void
  UnOp::rehash(void) {
    init_hash();
    std::hash<int> h;
    cmb_hash(h(static_cast<int>(_op)));
    cmb_hash(Expression::hash(_e0));
  }
  UnOp*
  UnOp::a(const ASTContext& ctx, const Location& loc,
          UnOpType op, Expression* e) {
    UnOp* uo = new (ctx) UnOp(loc,op,e);
    uo->rehash();
    return uo;
  }

  CtxStringH
  UnOp::opToString(void) const {
    switch (_op) {
    case UOT_PLUS: return _opToString.sBOT_PLUS;
    case UOT_MINUS: return _opToString.sBOT_MINUS;
    case UOT_NOT: return _opToString.sBOT_NOT;
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
    cmb_hash(hu(_args->size()));
    for (unsigned int i=_args->size(); i--;)
      cmb_hash(Expression::hash((*_args)[i]));
  }
  Call*
  Call::a(const ASTContext& ctx, const Location& loc,
          const std::string& id,
          const std::vector<Expression*>& args,
          FunctionI* decl) {
    Call* c = new (ctx) Call(loc);
    c->_id = CtxStringH(ctx,id);
    c->_args = CtxVec<Expression*>::a(ctx,args);
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
  VarDecl::a(const ASTContext& ctx, const Location& loc,
             TypeInst* ti, const CtxStringH& id, Expression* e) {
    VarDecl* v = new (ctx) VarDecl(loc,ti->_type);
    v->_ti = ti;
    v->_id = id;
    v->_e = e;
    v->_allocator = 0;
    v->rehash();
    return v;
  }
  VarDecl*
  VarDecl::a(const ASTContext& ctx, const Location& loc,
             TypeInst* ti, const std::string& id, Expression* e) {
    return a(ctx,loc,ti,CtxStringH(ctx,id));
  }

  void
  Let::rehash(void) {
    init_hash();
    cmb_hash(Expression::hash(_in));
    std::hash<unsigned int> h;
    cmb_hash(h(_let->size()));
    for (unsigned int i=_let->size(); i--;)
      cmb_hash(Expression::hash((*_let)[i]));
  }
  Let*
  Let::a(const ASTContext& ctx, const Location& loc,
         const std::vector<Expression*>& let, Expression* in) {
    Let* l = new (ctx) Let(loc);
    l->_let = CtxVec<Expression*>::a(ctx,let);
    l->_in = in;
    l->rehash();
    return l;
  }

  void
  TypeInst::rehash(void) {
    init_hash();
    std::hash<unsigned int> h;
    unsigned int rsize = _ranges==NULL ? 0 : _ranges->size();
    cmb_hash(h(rsize));
    for (unsigned int i=rsize; i--;)
      cmb_hash(Expression::hash((*_ranges)[i]));
    cmb_hash(Expression::hash(_domain));
  }
  TypeInst*
  TypeInst::a(const ASTContext& ctx, const Location& loc,
              const Type& type, Expression* domain,
              CtxVec<TypeInst*>* ranges) {
    TypeInst* t = new (ctx) TypeInst(loc,type,domain,ranges);
    t->rehash();
    return t;
  }

  void
  TypeInst::addRanges(const ASTContext& ctx,
                      const std::vector<TypeInst*>& ranges) {
    assert(_ranges == NULL);
    _ranges = CtxVec<TypeInst*>::a(ctx,ranges);
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
    if (_ranges && _ranges->size()==1 &&
        (*_ranges)[0]->isa<TIId>())
      return true;
    return false;
  }

  IncludeI*
  IncludeI::a(const ASTContext& ctx, const Location& loc,
              const CtxStringH& f) {
    IncludeI* i = new (ctx) IncludeI(loc);
    i->_f = f;
    return i;
  }

  VarDeclI*
  VarDeclI::a(const ASTContext& ctx, const Location& loc,
              VarDecl* e) {
    VarDeclI* vi = new (ctx) VarDeclI(loc);
    vi->_e = e;
    return vi;
  }

  AssignI*
  AssignI::a(const ASTContext& ctx, const Location& loc,
             const std::string& id, Expression* e) {
    AssignI* ai = new (ctx) AssignI(loc);
    ai->_id = CtxStringH(ctx,id);
    ai->_e = e;
    ai->_decl = NULL;
    return ai;
  }

  ConstraintI*
  ConstraintI::a(const ASTContext& ctx, const Location& loc, 
                 Expression* e) {
    ConstraintI* ci = new (ctx) ConstraintI(loc);
    ci->_e = e;
    return ci;
  }

  SolveI*
  SolveI::sat(const ASTContext& ctx, const Location& loc, Annotation* ann) {
    SolveI* si = new (ctx) SolveI(loc);
    si->_ann = ann;
    si->_e = NULL;
    si->_st = ST_SAT;
    return si;
  }
  SolveI*
  SolveI::min(const ASTContext& ctx, const Location& loc,
              Expression* e, Annotation* ann) {
    SolveI* si = new (ctx) SolveI(loc);
    si->_ann = ann;
    si->_e = e;
    si->_st = ST_MIN;
    return si;
  }
  SolveI*
  SolveI::max(const ASTContext& ctx, const Location& loc,
              Expression* e, Annotation* ann) {
    SolveI* si = new (ctx) SolveI(loc);
    si->_ann = ann;
    si->_e = e;
    si->_st = ST_MAX;
    return si;
  }

  OutputI*
  OutputI::a(const ASTContext& ctx, const Location& loc,
             Expression* e) {
    OutputI* oi = new (ctx) OutputI(loc);
    oi->_e = e;
    return oi;
  }

  FunctionI*
  FunctionI::a(const ASTContext& ctx, const Location& loc,
               const std::string& id, TypeInst* ti,
               const std::vector<VarDecl*>& params,
               Expression* e, Annotation* ann) {
    FunctionI* fi = new (ctx) FunctionI(loc);
    fi->_id = CtxStringH(ctx,id);
    fi->_ti = ti;
    fi->_params = CtxVec<VarDecl*>::a(ctx,params);
    fi->_ann = ann;
    fi->_e = e;
    fi->_builtins.e = NULL;
    fi->_builtins.b = NULL;
    fi->_builtins.f = NULL;
    fi->_builtins.i = NULL;
    return fi;
  }

  Type
  FunctionI::rtype(const std::vector<Expression*>& ta) {
    Type ret = _ti->_type;
    CtxStringH dh;
    if (_ti->_domain && _ti->_domain->isa<TIId>())
      dh = _ti->_domain->cast<TIId>()->_v;
    CtxStringH rh;
    if (_ti->_ranges && _ti->_ranges->size()==1 &&
        (*_ti->_ranges)[0] && (*_ti->_ranges)[0]->isa<TIId>())
      rh = (*_ti->_ranges)[0]->cast<TIId>()->_v;

    CtxStringMap<Type>::t tmap;
    for (unsigned int i=0; i<ta.size(); i++) {
      TypeInst* tii = (*_params)[i]->_ti;
      if (tii->_domain && tii->_domain->isa<TIId>()) {
        CtxStringH tiid = tii->_domain->cast<TIId>()->_v;
        Type tiit = ta[i]->_type;
        tiit._dim=0;
        CtxStringMap<Type>::t::iterator it = tmap.find(tiid);
        if (it==tmap.end()) {
          tmap.insert(std::pair<CtxStringH,Type>(tiid,tiit));
        } else {
          if (it->second._dim > 0) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" used in both array and non-array position");
          } else if (it->second!=tiit) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" instantiated with different types");
          }
        }
      }
      if (tii->_ranges && tii->_ranges->size()==1 &&
          (*tii->_ranges)[0]->_domain && 
          (*tii->_ranges)[0]->_domain->isa<TIId>()) {
        CtxStringH tiid = (*tii->_ranges)[0]->_domain->cast<TIId>()->_v;
        if (ta[i]->_type._dim<=0) {
          throw TypeError(ta[i]->_loc,"type-inst variable $"+tiid.str()+
            " must be an array index");
        }
        Type tiit = Type::any(ta[i]->_type._dim);
        CtxStringMap<Type>::t::iterator it = tmap.find(tiid);
        if (it==tmap.end()) {
          tmap.insert(std::pair<CtxStringH,Type>(tiid,tiit));
        } else {
          if (it->second._dim == 0) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" used in both array and non-array position");
          } else if (it->second!=tiit) {
            throw TypeError(ta[i]->_loc,"type-inst variable $"+
              tiid.str()+" instantiated with different types");
          }
        }
      }
    }
    if (dh.size() != 0) {
      CtxStringMap<Type>::t::iterator it = tmap.find(dh);
      if (it==tmap.end())
        throw TypeError(_loc,"type-inst variable $"+dh.str()+" used but not defined");
      ret._bt = it->second._bt;
      if (ret._ti==Type::TI_ANY)
        ret._ti = it->second._ti;
      if (ret._st==Type::ST_PLAIN)
        ret._st = it->second._st;
    } 
    if (rh.size() != 0) {
      CtxStringMap<Type>::t::iterator it = tmap.find(rh);
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
    if (e0->_eid != e1->_eid) return false;
    if (e0->_type != e1->_type) return false;
    switch (e0->_eid) {
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
          if (s0->_v->size() != s1->_v->size()) return false;
          for (unsigned int i=0; i<s0->_v->size(); i++)
            if (!Expression::equal( (*s0->_v)[i], (*s1->_v)[i] ))
              return false;
          return true;
        }
      }
    case Expression::E_BOOLLIT:
      return e0->cast<BoolLit>()->_v == e1->cast<BoolLit>()->_v;
    case Expression::E_STRINGLIT:
      return e0->cast<StringLit>()->_v == e1->cast<StringLit>()->_v;
    case Expression::E_ID:
      assert(e0->cast<Id>()->_decl != NULL);
      return e0->cast<Id>()->_decl == e1->cast<Id>()->_decl;
    case Expression::E_ANON:
      return false;
    case Expression::E_ARRAYLIT:
      {
        const ArrayLit* a0 = e0->cast<ArrayLit>();
        const ArrayLit* a1 = e1->cast<ArrayLit>();
        if (a0->_v->size() != a1->_v->size()) return false;
        if (a0->_dims->size() != a1->_dims->size()) return false;
        for (unsigned int i=0; i<a0->_dims->size(); i++)
          if ( (*a0->_dims)[i] != (*a1->_dims)[i] ) return false;
        for (unsigned int i=0; i<a0->_v->size(); i++)
          if (!Expression::equal( (*a0->_v)[i], (*a1->_v)[i] ))
            return false;
        return true;
      }
    case Expression::E_ARRAYACCESS:
      {
        const ArrayAccess* a0 = e0->cast<ArrayAccess>();
        const ArrayAccess* a1 = e1->cast<ArrayAccess>();
        if (!Expression::equal( a0->_v, a1->_v )) return false;
        if (a0->_idx->size() != a1->_idx->size()) return false;
        for (unsigned int i=0; i<a0->_idx->size(); i++)
          if (!Expression::equal( (*a0->_idx)[i], (*a1->_idx)[i] ))
            return false;
        return true;
      }
    case Expression::E_COMP:
      {
        const Comprehension* c0 = e0->cast<Comprehension>();
        const Comprehension* c1 = e1->cast<Comprehension>();
        if (c0->_set != c1->_set) return false;
        if (!Expression::equal ( c0->_e, c1->_e )) return false;
        if (!Expression::equal ( c0->_where, c1->_where )) return false;
        if (c0->_g->size() != c1->_g->size()) return false;
        for (unsigned int i=0; i<c0->_g->size(); i++) {
          if (!Expression::equal( (*c0->_g)[i]->_in, (*c1->_g)[i]->_in ))
            return false;
          if ((*c0->_g)[i]->_v->size() != (*c1->_g)[i]->_v->size())
            return false;
          for (unsigned int j=0; j<(*c0->_g)[i]->_v->size(); j++) {
            if (!Expression::equal ( (*(*c0->_g)[i]->_v)[j], 
                                    (*(*c1->_g)[i]->_v)[j] ))
              return false;
          }
        }
        return true;
      }
    case Expression::E_ITE:
      {
        const ITE* i0 = e0->cast<ITE>();
        const ITE* i1 = e1->cast<ITE>();
        if (i0->_e_if->size() != i1->_e_if->size()) return false;
        for (unsigned int i=i0->_e_if->size(); i--; ) {
          if (!Expression::equal ( (*i0->_e_if)[i].first, 
                                  (*i1->_e_if)[i].first))
            return false;
          if (!Expression::equal ( (*i0->_e_if)[i].second, 
                                  (*i1->_e_if)[i].second))
            return false;
        }
        if (!Expression::equal (i0->_e_else, i1->_e_else)) return false;
        return true;
      }
    case Expression::E_BINOP:
      {
        const BinOp* b0 = e0->cast<BinOp>();
        const BinOp* b1 = e1->cast<BinOp>();
        if (b0->_op != b1->_op) return false;
        if (!Expression::equal (b0->_e0, b1->_e0)) return false;
        if (!Expression::equal (b0->_e1, b1->_e1)) return false;
        return true;
      }
    case Expression::E_UNOP:
      {
        const UnOp* b0 = e0->cast<UnOp>();
        const UnOp* b1 = e1->cast<UnOp>();
        if (b0->_op != b1->_op) return false;
        if (!Expression::equal (b0->_e0, b1->_e0)) return false;
        return true;
      }
    case Expression::E_CALL:
      {
        const Call* c0 = e0->cast<Call>();
        const Call* c1 = e1->cast<Call>();
        if (c0->_id != c1->_id) return false;
        if (c0->_decl != c1->_decl) return false;
        if (c0->_args->size() != c1->_args->size()) return false;
        for (unsigned int i=0; i<c0->_args->size(); i++)
          if (!Expression::equal ( (*c0->_args)[i], (*c1->_args)[i] ))
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
        if (l0->_let->size() != l1->_let->size()) return false;
        for (unsigned int i=l0->_let->size(); i--;)
          if (!Expression::equal ( (*l0->_let)[i], (*l1->_let)[i]))
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
        if (t0->_ranges->size() != t1->_ranges->size()) return false;
        for (unsigned int i=t0->_ranges->size(); i--;)
          if (!Expression::equal ( (*t0->_ranges)[i], (*t1->_ranges)[i]))
            return false;
        if (!Expression::equal (t0->_domain, t1->_domain)) return false;
        return true;
      }
    case Expression::E_TIID:
      return false;
    }
  }    

}
