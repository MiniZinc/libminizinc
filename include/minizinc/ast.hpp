/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace MiniZinc {
  
  inline bool
  Expression::equal(const Expression* e0, const Expression* e1) {
    if (e0==e1) return true;
    if (e0 == NULL || e1 == NULL) return false;
    if (e0->isUnboxedInt() || e1->isUnboxedInt()) return false;
    if (e0->isUnboxedFloatVal() || e1->isUnboxedFloatVal()) {
      if (e0->isUnboxedFloatVal() && e1->isUnboxedFloatVal()) {
        return e0->unboxedFloatToFloatVal()==e1->unboxedFloatToFloatVal();
      }
      return false;
    }
    if (e0->_id != e1->_id) return false;
    if (e0->type() != e1->type()) return false;
    if (e0->hash() != e1->hash()) return false;
    return equal_internal(e0, e1);
  }

  inline void
  Expression::type(const Type& t) {
    if (isUnboxedVal()) {
      assert(!isUnboxedInt() || t == Type::parint());
      assert(!isUnboxedFloatVal() || t == Type::parfloat());
      return;
    }
    if (eid()==E_VARDECL) {
      this->cast<VarDecl>()->id()->_type = t;
    } else if (eid()==E_ID && this->cast<Id>()->decl()) {
      assert(_type.bt() == Type::BT_UNKNOWN || _type.dim()==t.dim() || t.dim() != -1);
      this->cast<Id>()->decl()->_type = t;
    }
    _type = t;
  }

  inline
  IntLit::IntLit(const Location& loc, IntVal v)
  : Expression(loc,E_INTLIT,Type::parint()), _v(v) {
    rehash();
  }

  inline IntLit*
  IntLit::a(MiniZinc::IntVal v) {
    
    if (v.isFinite()) {
      IntLit* ret = intToUnboxedInt(v.toInt());
      if (ret) {
        return ret;
      }
    }
    
    UNORDERED_NAMESPACE::unordered_map<IntVal, WeakRef>::iterator it = constants().integerMap.find(v);
    if (it==constants().integerMap.end() || it->second()==NULL) {
      IntLit* il = new IntLit(Location().introduce(), v);
      if (it==constants().integerMap.end()) {
        constants().integerMap.insert(std::make_pair(v, il));
      } else {
        it->second = il;
      }
      return il;
    } else {
      return it->second()->cast<IntLit>();
    }
  }
  
  inline IntLit*
  IntLit::aEnum(IntVal v, unsigned int enumId) {
    if (enumId==0)
      return a(v);
    IntLit* il = new IntLit(Location().introduce(), v);
    Type tt(il->type());
    tt.enumId(enumId);
    il->type(tt);
    return il;
  }
  
  inline
  Location::LocVec::LocVec(const ASTString& filename, unsigned int fl,
                           unsigned int first_column, unsigned int last_line, unsigned int last_column) : ASTVec(3) {
    *(_data+0) = filename.aststr();
    long long unsigned int lines = fl;
    long long unsigned int ll = last_line;
    lines |= ll << 20;
    long long unsigned int columns = first_column;
    long long unsigned int lc = last_column;
    columns |= lc << 20;
    
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    
    ui.u = lines;
    *(_data+1) = IntLit::a(ui.i);
    
    ui.u = columns;
    *(_data+2) = IntLit::a(ui.i);
    
    assert(first_line()==fl);
  }

  inline ASTString
  Location::LocVec::filename(void) const {
    return static_cast<ASTStringO*>(_data[0]);
  }
  inline unsigned int
  Location::LocVec::first_line(void) const {
    IntLit* il = static_cast<IntLit*>(_data[1]);
    long long unsigned int mask = 0xFFFFF;
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = il->v().toInt();
    return ui.u & mask;
  }
  inline unsigned int
  Location::LocVec::last_line(void) const {
    IntLit* il = static_cast<IntLit*>(_data[1]);
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = il->v().toInt();
    return ui.u >> 20;
  }
  inline unsigned int
  Location::LocVec::first_column(void) const {
    IntLit* il = static_cast<IntLit*>(*(_data+2));
    long long unsigned int mask = 0xFFFFF;
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = il->v().toInt();
    return ui.u & mask;
  }
  inline unsigned int
  Location::LocVec::last_column(void) const {
    IntLit* il = static_cast<IntLit*>(*(_data+2));
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = il->v().toInt();
    return ui.u >> 20;
  }
  
  inline
  FloatLit::FloatLit(const Location& loc, FloatVal v)
  : Expression(loc,E_FLOATLIT,Type::parfloat()), _v(v) {
    rehash();
  }

  inline FloatLit*
  FloatLit::a(MiniZinc::FloatVal v) {
    if (sizeof(double) <= sizeof(FloatLit*) && v.isFinite()) {
      FloatLit* ret = Expression::doubleToUnboxedFloatVal(v.toDouble());
      if (ret) {
        return ret;
      }
    }
    
    UNORDERED_NAMESPACE::unordered_map<FloatVal, WeakRef>::iterator it = constants().floatMap.find(v);
    if (it==constants().floatMap.end() || it->second()==NULL) {
      FloatLit* fl = new FloatLit(Location().introduce(), v);
      if (it==constants().floatMap.end()) {
        constants().floatMap.insert(std::make_pair(v, fl));
      } else {
        it->second = fl;
      }
      return fl;
    } else {
      return it->second()->cast<FloatLit>();
    }
  }

  inline
  SetLit::SetLit(const Location& loc,
                 const std::vector<Expression*>& v)
  : Expression(loc,E_SETLIT,Type()), _v(ASTExprVec<Expression>(v)) {
    _u.isv = NULL;
    rehash();
  }
  
  inline
  SetLit::SetLit(const Location& loc, ASTExprVec<Expression> v)
  : Expression(loc,E_SETLIT,Type()), _v(v) {
    _u.isv = NULL;
    rehash();
  }

  inline
  SetLit::SetLit(const Location& loc, IntSetVal* isv)
  : Expression(loc,E_SETLIT,Type()) {
    _type = Type::parsetint();
    _u.isv = isv;
    rehash();
  }

  inline
  SetLit::SetLit(const Location& loc, FloatSetVal* fsv)
  : Expression(loc,E_SETLIT,Type()) {
    _type = Type::parsetfloat();
    _u.fsv = fsv;
    rehash();
  }

  inline
  BoolLit::BoolLit(const Location& loc, bool v)
  : Expression(loc,E_BOOLLIT,Type::parbool()), _v(v) {
    rehash();
  }

  inline
  StringLit::StringLit(const Location& loc, const std::string& v)
  : Expression(loc,E_STRINGLIT,Type::parstring()), _v(ASTString(v)) {
    rehash();
  }

  inline
  StringLit::StringLit(const Location& loc, const ASTString& v)
  : Expression(loc,E_STRINGLIT,Type::parstring()), _v(v) {
    rehash();
  }
  
  inline
  Id::Id(const Location& loc, const std::string& v0, VarDecl* decl)
  : Expression(loc,E_ID,Type()), _decl(decl) {
    v(v0);
    rehash();
  }

  inline
  Id::Id(const Location& loc, const ASTString& v0, VarDecl* decl)
  : Expression(loc,E_ID,Type()), _decl(decl) {
    v(v0);
    rehash();
  }

  inline
  Id::Id(const Location& loc, long long int idn0, VarDecl* decl)
  : Expression(loc,E_ID,Type()), _decl(decl) {
    idn(idn0);
    rehash();
  }

  inline void
  Id::decl(VarDecl* d) {
    _decl = d;
  }

  inline ASTString
  Id::v(void) const {
    if (_decl && _decl->isa<Id>()) {
      Expression* d = _decl;
      while (d && d->isa<Id>()) {
        d = d->cast<Id>()->_decl;
      }
      return d->cast<VarDecl>()->id()->v();
    } else {
      assert((reinterpret_cast<ptrdiff_t>(_v_or_idn) & static_cast<ptrdiff_t>(1)) == 0);
      return ASTString(reinterpret_cast<ASTStringO*>(_v_or_idn));
    }
  }

  inline long long int
  Id::idn(void) const {
    if (_decl && _decl->isa<Id>()) {
      Expression* d = _decl;
      while (d && d->isa<Id>()) {
        d = d->cast<Id>()->_decl;
      }
      return d->cast<VarDecl>()->id()->idn();
    } else {
      if ((reinterpret_cast<ptrdiff_t>(_v_or_idn) & static_cast<ptrdiff_t>(1)) == 0)
        return -1;
      long long int i = reinterpret_cast<ptrdiff_t>(_v_or_idn) & ~static_cast<ptrdiff_t>(1);
      return i >> 1;
    }
  }

  
  inline
  TIId::TIId(const Location& loc, const std::string& v)
  : Expression(loc,E_TIID,Type()), _v(ASTString(v)) {
    rehash();
  }

  inline
  AnonVar::AnonVar(const Location& loc)
  : Expression(loc,E_ANON,Type()) {
    rehash();
  }


  inline
  ArrayLit::ArrayLit(const Location& loc,
                     ArrayLit& v,
                     const std::vector<std::pair<int,int> >& dims)
  : Expression(loc,E_ARRAYLIT,Type()) {
    _flag_1 = false;
    _flag_2 = v._flag_2;
    if (_flag_2) {
      _u._al = v._u._al;
      std::vector<int> d(dims.size()*2+v._dims.size()-v.dims()*2);
      for (unsigned int i=dims.size(); i--;) {
        d[i*2] = dims[i].first;
        d[i*2+1] = dims[i].second;
      }
      int sliceOffset = dims.size()*2;
      int origSliceOffset = v.dims()*2;
      for (int i=0; i<_u._al->dims()*2; i++) {
        d[sliceOffset+i] = v._dims[origSliceOffset+i];
      }
      _dims = ASTIntVec(d);
    } else {
      std::vector<int> d(dims.size()*2);
      for (unsigned int i=dims.size(); i--;) {
        d[i*2] = dims[i].first;
        d[i*2+1] = dims[i].second;
      }
      if (v._u._v->flag() || d.size()!=2 || d[0]!=1) {
        // only allocate dims vector if it is not a 1d array indexed from 1
        _dims = ASTIntVec(d);
      }
      _u._v = v._u._v;
    }
    rehash();
  }

  inline
  ArrayLit::ArrayLit(const Location& loc,
                     ArrayLit& v)
  : Expression(loc,E_ARRAYLIT,Type()) {
    _flag_1 = false;
    _flag_2 = v._flag_2;
    if (_flag_2) {
      _u._al = v._u._al;
      std::vector<int> d(2+v._dims.size()-v.dims()*2);
      d[0] = 1;
      d[1] = v.size();
      int sliceOffset = 2;
      int origSliceOffset = v.dims()*2;
      for (int i=0; i<_u._al->dims()*2; i++) {
        d[sliceOffset+i] = v._dims[origSliceOffset+i];
      }
      _dims = ASTIntVec(d);
    } else {
      _u._v = v._u._v;
      if (_u._v->flag()) {
        std::vector<int> d(2);
        d[0] = 1;
        d[1] = v.length();
        _dims = ASTIntVec(d);
      } else {
        // don't allocate dims vector since this is a 1d array indexed from 1
      }
    }
    rehash();
  }

  inline
  ArrayLit::ArrayLit(const Location& loc,
                     const std::vector<Expression*>& v)
  : Expression(loc,E_ARRAYLIT,Type()) {
    _flag_1 = false;
    _flag_2 = false;
    std::vector<int> d(2);
    d[0] = 1;
    d[1] = v.size();
    compress(v, d);
    rehash();
  }

  inline
  ArrayLit::ArrayLit(const Location& loc,
                     const std::vector<std::vector<Expression*> >& v)
  : Expression(loc,E_ARRAYLIT,Type()) {
    _flag_1 = false;
    _flag_2 = false;
    std::vector<int> dims(4);
    dims[0]=1;
    dims[1]=v.size();
    dims[2]=1;
    dims[3]=v.size() > 0 ? v[0].size() : 0;
    std::vector<Expression*> vv;
    for (unsigned int i=0; i<v.size(); i++)
      for (unsigned int j=0; j<v[i].size(); j++)
        vv.push_back(v[i][j]);
    compress(vv, dims);
    rehash();
  }
  
  inline
  ArrayAccess::ArrayAccess(const Location& loc,
                           Expression* v,
                           const std::vector<Expression*>& idx)
  : Expression(loc,E_ARRAYACCESS,Type()) {
    _v = v;
    _idx = ASTExprVec<Expression>(idx);
    rehash();
  }

  inline
  ArrayAccess::ArrayAccess(const Location& loc,
                           Expression* v,
                           ASTExprVec<Expression> idx)
  : Expression(loc,E_ARRAYACCESS,Type()) {
    _v = v;
    _idx = idx;
    rehash();
  }

  inline void
  Comprehension::init(Expression *e, Generators &g) {
    _e = e;
    std::vector<Expression*> es;
    std::vector<int> idx;
    for (unsigned int i=0; i<g._g.size(); i++) {
      idx.push_back(es.size());
      es.push_back(g._g[i]._in);
      es.push_back(g._g[i]._where);
      for (unsigned int j=0; j<g._g[i]._v.size(); j++) {
        es.push_back(g._g[i]._v[j]);
      }
    }
    idx.push_back(es.size());
    _g = ASTExprVec<Expression>(es);
    _g_idx = ASTIntVec(idx);
    rehash();
  }
  inline
  Comprehension::Comprehension(const Location& loc,
                               Expression* e,
                               Generators& g,
                               bool set)
  : Expression(loc,E_COMP,Type()) {
    init(e,g);
    _flag_1 = set;
  }
  inline void
  ITE::init(const std::vector<Expression*>& e_if_then, Expression* e_else) {
    _e_if_then = ASTExprVec<Expression>(e_if_then);
    _e_else = e_else;
    rehash();
  }
  inline
  ITE::ITE(const Location& loc,
           const std::vector<Expression*>& e_if_then, Expression* e_else)
  : Expression(loc,E_ITE,Type()) {
    init(e_if_then,e_else);
  }

  inline
  BinOp::BinOp(const Location& loc,
               Expression* e0, BinOpType op, Expression* e1)
  : Expression(loc,E_BINOP,Type()), _e0(e0), _e1(e1), _decl(NULL) {
    _sec_id = op;
    rehash();
  }

  inline
  UnOp::UnOp(const Location& loc, UnOpType op, Expression* e)
  : Expression(loc,E_UNOP,Type()), _e0(e), _decl(NULL) {
    _sec_id = op;
    rehash();
  }

  inline bool
  Call::hasId(void) const {
    return (reinterpret_cast<ptrdiff_t>(_u_id._decl) & static_cast<ptrdiff_t>(1)) == 0;
  }
  
  inline ASTString
  Call::id(void) const {
    return hasId() ? _u_id._id : decl()->id();
  }
  
  inline void
  Call::id(const ASTString& i) {
    _u_id._id = i.aststr();
    assert(hasId());
    assert(decl()==NULL);
  }

  inline FunctionI*
  Call::decl(void) const {
    return hasId() ? NULL : reinterpret_cast<FunctionI*>(reinterpret_cast<ptrdiff_t>(_u_id._decl) & ~static_cast<ptrdiff_t>(1));
  }
  
  inline void
  Call::decl(FunctionI* f) {
    assert(f != NULL);
    _u_id._decl = reinterpret_cast<FunctionI*>(reinterpret_cast<ptrdiff_t>(f) | static_cast<ptrdiff_t>(1));
  }
  
  inline
  Call::Call(const Location& loc,
             const std::string& id0,
             const std::vector<Expression*>& args)
  : Expression(loc, E_CALL,Type()) {
    id(ASTString(id0));
    if (args.size()==1) {
      _u._oneArg = args[0]->isUnboxedVal() ? args[0] : args[0]->tag();
    } else {
      _u._args = ASTExprVec<Expression>(args).vec();
    }
    rehash();
    assert(hasId());
    assert(decl() == NULL);
  }

  inline
  Call::Call(const Location& loc,
             const ASTString& id0,
             const std::vector<Expression*>& args)
  : Expression(loc, E_CALL,Type()) {
    id(ASTString(id0));
    if (args.size()==1) {
      _u._oneArg = args[0]->isUnboxedVal() ? args[0] : args[0]->tag();
    } else {
      _u._args = ASTExprVec<Expression>(args).vec();
    }
    rehash();
    assert(hasId());
    assert(decl() == NULL);
  }

  inline
  VarDecl::VarDecl(const Location& loc,
                   TypeInst* ti, const ASTString& id, Expression* e)
  : Expression(loc,E_VARDECL,ti ? ti->type() : Type()),
    _id(NULL), _flat(NULL) {
    _id = new Id(loc,id,this);
    _flag_1 = true;
    _flag_2 = false;
    _ti = ti;
    _e = e;
    _id->type(type());
    _payload = 0;
    rehash();
  }

  inline
  VarDecl::VarDecl(const Location& loc,
                   TypeInst* ti, long long int idn, Expression* e)
  : Expression(loc,E_VARDECL,ti ? ti->type() : Type()),
  _id(NULL), _flat(NULL) {
    _id = new Id(loc,idn,this);
    _flag_1 = true;
    _flag_2 = false;
    _ti = ti;
    _e = e;
    _id->type(type());
    _payload = 0;
    rehash();
  }

  inline
  VarDecl::VarDecl(const Location& loc,
                   TypeInst* ti, const std::string& id, Expression* e)
  : Expression(loc,E_VARDECL,ti->type()),
    _id(NULL), _flat(NULL) {
    _id = new Id(loc,ASTString(id),this);
    _flag_1 = true;
    _flag_2 = false;
    _ti = ti;
    _e = e;
    _id->type(type());
    _payload = 0;
    rehash();
  }

  inline
  VarDecl::VarDecl(const Location& loc,
                   TypeInst* ti, Id* id, Expression* e)
  : Expression(loc,E_VARDECL,ti->type()),
  _id(NULL), _flat(NULL) {
    if (id->idn()==-1)
      _id = new Id(loc,id->v(),this);
    else
      _id = new Id(loc,id->idn(),this);
    _flag_1 = true;
    _flag_2 = false;
    _ti = ti;
    _e = e;
    _id->type(type());
    _payload = 0;
    rehash();
  }

  inline Expression*
  VarDecl::e(void) const {
    return _e->isUnboxedVal() ? _e : _e->untag();
  }

  inline void
  VarDecl::e(Expression* rhs) {
    _e = rhs;
  }
  
  inline bool
  VarDecl::toplevel(void) const {
    return _flag_1;
  }
  inline void
  VarDecl::toplevel(bool t) {
    _flag_1 = t;
  }
  inline bool
  VarDecl::introduced(void) const {
    return _flag_2;
  }
  inline void
  VarDecl::introduced(bool t) {
    _flag_2 = t;
  }
  inline bool
  VarDecl::evaluated(void) const {
    return _e->isUnboxedVal() || _e->isTagged();
  }
  inline void
  VarDecl::evaluated(bool t) {
    if (!_e->isUnboxedVal()) {
      if (t)
        _e = _e->tag();
      else
        _e = _e->untag();
    }
  }
  inline void
  VarDecl::flat(VarDecl* vd) {
    _flat = WeakRef(vd);
  }

  inline
  TypeInst::TypeInst(const Location& loc,
                     const Type& type,
                     ASTExprVec<TypeInst> ranges,
                     Expression* domain)
  : Expression(loc,E_TI,type), _ranges(ranges), _domain(domain) {
    _flag_1 = false;
    _flag_2 = false;
    rehash();
  }

  inline
  TypeInst::TypeInst(const Location& loc,
                     const Type& type,
                     Expression* domain)
  : Expression(loc,E_TI,type), _domain(domain) {
    _flag_1 = false;
    _flag_2 = false;
    rehash();
  }

  inline
  IncludeI::IncludeI(const Location& loc, const ASTString& f)
  : Item(loc, II_INC), _f(f), _m(NULL) {}
  
  inline
  VarDeclI::VarDeclI(const Location& loc, VarDecl* e)
  : Item(loc, II_VD), _e(e) {}

  inline
  AssignI::AssignI(const Location& loc, const std::string& id, Expression* e)
    : Item(loc, II_ASN), _id(ASTString(id)), _e(e), _decl(NULL) {}

  inline
  ConstraintI::ConstraintI(const Location& loc, Expression* e)
  : Item(loc, II_CON), _e(e) {}

  inline
  SolveI::SolveI(const Location& loc, Expression* e)
  : Item(loc, II_SOL), _e(e) {}
  inline SolveI*
  SolveI::sat(const Location& loc) {
    SolveI* si = new SolveI(loc,NULL);
    si->_sec_id = ST_SAT;
    return si;
  }
  inline SolveI*
  SolveI::min(const Location& loc, Expression* e) {
    SolveI* si = new SolveI(loc,e);
    si->_sec_id = ST_MIN;
    return si;
  }
  inline SolveI*
  SolveI::max(const Location& loc, Expression* e) {
    SolveI* si = new SolveI(loc,e);
    si->_sec_id = ST_MAX;
    return si;
  }
  inline SolveI::SolveType
  SolveI::st(void) const {
    return static_cast<SolveType>(_sec_id);
  }
  inline void
  SolveI::st(SolveI::SolveType s) {
    _sec_id = s;
  }

  inline
  OutputI::OutputI(const Location& loc, Expression* e)
  : Item(loc, II_OUT), _e(e) {}
  
  inline
  FunctionI::FunctionI(const Location& loc,
                       const std::string& id, TypeInst* ti,
                       const std::vector<VarDecl*>& params,
                       Expression* e)
  : Item(loc, II_FUN),
    _id(ASTString(id)),
    _ti(ti),
    _params(ASTExprVec<VarDecl>(params)),
    _e(e) {
    _builtins.e = NULL;
    _builtins.b = NULL;
    _builtins.f = NULL;
    _builtins.i = NULL;
    _builtins.s = NULL;
    _builtins.str = NULL;
    _from_stdlib = (loc.filename() == "builtins.mzn" ||
              loc.filename().endsWith("/builtins.mzn") ||
              loc.filename() == "stdlib.mzn" ||
              loc.filename().endsWith("/stdlib.mzn") ||
              loc.filename() == "flatzinc_builtins.mzn" ||
              loc.filename().endsWith("/flatzinc_builtins.mzn"));
  }

  inline
  FunctionI::FunctionI(const Location& loc,
                       const ASTString& id, TypeInst* ti,
                       const ASTExprVec<VarDecl>& params,
                       Expression* e)
  : Item(loc, II_FUN),
  _id(id),
  _ti(ti),
  _params(params),
  _e(e) {
    _builtins.e = NULL;
    _builtins.b = NULL;
    _builtins.f = NULL;
    _builtins.i = NULL;
    _builtins.s = NULL;
    _builtins.str = NULL;
    _from_stdlib = (loc.filename() == "builtins.mzn" ||
                    loc.filename().endsWith("/builtins.mzn") ||
                    loc.filename() == "stdlib.mzn" ||
                    loc.filename().endsWith("/stdlib.mzn") ||
                    loc.filename() == "flatzinc_builtins.mzn" ||
                    loc.filename().endsWith("/flatzinc_builtins.mzn"));
  }

}
