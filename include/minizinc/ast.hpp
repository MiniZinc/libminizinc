/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace MiniZinc {

inline bool Expression::equal(const Expression* e0, const Expression* e1) {
  if (e0 == e1) {
    return true;
  }
  if (e0 == nullptr || e1 == nullptr) {
    return false;
  }
  if (isUnboxedInt(e0) || isUnboxedInt(e1)) {
    return false;
  }
  if (isUnboxedFloatVal(e0) || isUnboxedFloatVal(e1)) {
    if (isUnboxedFloatVal(e0) && isUnboxedFloatVal(e1)) {
      return unboxedFloatToFloatVal(e0) == unboxedFloatToFloatVal(e1);
    }
    return false;
  }
  if (static_cast<ExpressionId>(e0->_id) != static_cast<ExpressionId>(e1->_id)) {
    return false;
  }
  if (Expression::type(e0) != Expression::type(e1)) {
    return false;
  }
  if (Expression::hash(e0) != Expression::hash(e1)) {
    return false;
  }
  return equalInternal(e0, e1);
}

inline void Expression::type(Expression* e, const Type& t) {
  if (isUnboxedVal(e)) {
    assert(!isUnboxedInt(e) || t == Type::parint());
    assert(!isUnboxedFloatVal(e) || t == Type::parfloat());
    return;
  }
  if (Expression::eid(e) == E_VARDECL) {
    Expression::cast<VarDecl>(e)->id()->_type = t;
  } else if (Expression::eid(e) == E_ID && (Expression::cast<Id>(e)->decl() != nullptr)) {
    assert(e->_type.bt() == Type::BT_UNKNOWN || e->_type.dim() == t.dim() || t.dim() != -1);
    Expression::cast<Id>(e)->decl()->_type = t;
  }
  e->_type = t;
}

inline IntLit::IntLit(const Location& loc, IntVal v)
    : Expression(loc, E_INTLIT, Type::parint()), _v(v) {
  rehash();
}

inline IntLit* IntLit::a(MiniZinc::IntVal v) {
  if (v.isFinite()) {
    IntLit* ret = intToUnboxedInt(v.toInt());
    if (ret != nullptr) {
      return ret;
    }
  }

  auto it = Constants::constants().integerMap.find(v);
  if (it == Constants::constants().integerMap.end() || it->second() == nullptr) {
    auto* il = new IntLit(Location().introduce(), v);
    if (it == Constants::constants().integerMap.end()) {
      Constants::constants().integerMap.insert(std::make_pair(v, il));
    } else {
      it->second = il;
    }
    return il;
  }
  return Expression::cast<IntLit>(it->second());
}

inline IntLit* IntLit::aEnum(IntVal v, unsigned int enumId) {
  if (enumId == 0) {
    return a(v);
  }
  auto* il = new IntLit(Location().introduce(), v);
  Type tt(Expression::type(il));
  tt.typeId(enumId);
  Expression::type(il, tt);
  return il;
}

inline ASTString Location::LocVec::filename() const {
  return static_cast<ASTStringData*>(_data[0]);
}
inline unsigned int Location::LocVec::firstLine() const {
  if (_size == 2) {
    static const unsigned int pointerBits = sizeof(void*) * 8;
    auto* il = static_cast<IntLit*>(_data[1]);
    long long unsigned int mask = pointerBits <= 32 ? 0xFF : 0xFFFFF;
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = IntLit::v(il).toInt();
    return static_cast<unsigned int>(ui.u & mask);
  }
  auto* il = static_cast<IntLit*>(_data[1]);
  return static_cast<unsigned int>(IntLit::v(il).toInt());
}
inline unsigned int Location::LocVec::lastLine() const {
  if (_size == 2) {
    static const unsigned int pointerBits = sizeof(void*) * 8;
    auto* il = static_cast<IntLit*>(_data[1]);
    long long unsigned int first_line_size = pointerBits <= 32 ? 8 : 20;
    long long unsigned int mask = pointerBits <= 32 ? 0xFF : 0xFFFFF;
    long long unsigned int offsetmask = pointerBits <= 32 ? 0x7F : 0xFFFFF;
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = IntLit::v(il).toInt();
    // return first line (8 bit) + offset (7 bit)
    return static_cast<unsigned int>((ui.u & mask) + ((ui.u >> first_line_size) & offsetmask));
  }
  auto* il = static_cast<IntLit*>(_data[2]);
  return static_cast<unsigned int>(IntLit::v(il).toInt());
}
inline unsigned int Location::LocVec::firstColumn() const {
  if (_size == 2) {
    static const unsigned int pointerBits = sizeof(void*) * 8;
    auto* il = static_cast<IntLit*>(_data[1]);
    long long unsigned int first_col_offset = pointerBits <= 32 ? 8 + 7 : 20 + 20;
    long long unsigned int mask = pointerBits <= 32 ? 0x3F : 0x3FF;
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = IntLit::v(il).toInt();
    // return first line (8 bit) + offset (7 bit)
    return static_cast<unsigned int>((ui.u >> first_col_offset) & mask);
  }
  auto* il = static_cast<IntLit*>(_data[3]);
  return static_cast<unsigned int>(IntLit::v(il).toInt());
}
inline unsigned int Location::LocVec::lastColumn() const {
  if (_size == 2) {
    static const unsigned int pointerBits = sizeof(void*) * 8;
    auto* il = static_cast<IntLit*>(_data[1]);
    long long unsigned int last_col_offset = pointerBits <= 32 ? 8 + 7 + 6 : 20 + 20 + 10;
    long long unsigned int mask = pointerBits <= 32 ? 0x7F : 0x3FF;
    union {
      long long int i;
      unsigned long long int u;
    } ui;
    ui.i = IntLit::v(il).toInt();
    // return first line (8 bit) + offset (7 bit)
    return static_cast<unsigned int>((ui.u >> last_col_offset) & mask);
  }
  auto* il = static_cast<IntLit*>(_data[4]);
  return static_cast<unsigned int>(IntLit::v(il).toInt());
}

inline FloatLit::FloatLit(const Location& loc, FloatVal v)
    : Expression(loc, E_FLOATLIT, Type::parfloat()), _v(v) {
  rehash();
}

inline FloatLit* FloatLit::a(MiniZinc::FloatVal v) {
  if (sizeof(double) <= sizeof(void*) && v.isFinite()) {
    FloatLit* ret = Expression::doubleToUnboxedFloatVal(v.toDouble());
    if (ret != nullptr) {
      return ret;
    }
  }

  auto it = Constants::constants().floatMap.find(v);
  if (it == Constants::constants().floatMap.end() || it->second() == nullptr) {
    auto* fl = new FloatLit(Location().introduce(), v);
    if (it == Constants::constants().floatMap.end()) {
      Constants::constants().floatMap.insert(std::make_pair(v, fl));
    } else {
      it->second = fl;
    }
    return fl;
  }
  return Expression::cast<FloatLit>(it->second());
}

inline SetLit::SetLit(const Location& loc, const std::vector<Expression*>& v)
    : BoxedExpression(loc, E_SETLIT, Type()), _v(ASTExprVec<Expression>(v)) {
  _u.isv = nullptr;
  rehash();
}

inline SetLit::SetLit(const Location& loc, const ASTExprVec<Expression>& v)
    : BoxedExpression(loc, E_SETLIT, Type()), _v(v) {
  _u.isv = nullptr;
  rehash();
}

inline SetLit::SetLit(const Location& loc, IntSetVal* isv)
    : BoxedExpression(loc, E_SETLIT, Type()) {
  _type = Type::parsetint();
  _u.isv = isv;
  rehash();
}

inline SetLit::SetLit(const Location& loc, FloatSetVal* fsv)
    : BoxedExpression(loc, E_SETLIT, Type()) {
  _type = Type::parsetfloat();
  _u.fsv = fsv;
  rehash();
}

inline BoolLit::BoolLit(const Location& loc, bool v)
    : BoxedExpression(loc, E_BOOLLIT, Type::parbool()), _v(v) {
  rehash();
}

inline StringLit::StringLit(const Location& loc, const std::string& v)
    : BoxedExpression(loc, E_STRINGLIT, Type::parstring()), _v(ASTString(v)) {
  rehash();
}

inline StringLit::StringLit(const Location& loc, const ASTString& v)
    : BoxedExpression(loc, E_STRINGLIT, Type::parstring()), _v(v) {
  rehash();
}

inline Id::Id(const Location& loc, const std::string& v0, VarDecl* decl)
    : BoxedExpression(loc, E_ID, Type()), _decl(decl) {
  v(ASTString(v0));
  rehash();
}

inline Id::Id(const Location& loc, const ASTString& v0, VarDecl* decl)
    : BoxedExpression(loc, E_ID, Type()), _decl(decl) {
  v(v0);
  rehash();
}

inline Id::Id(const Location& loc, long long int idn0, VarDecl* decl)
    : BoxedExpression(loc, E_ID, Type()), _decl(decl) {
  idn(idn0);
  rehash();
}

inline void Id::decl(VarDecl* d) { _decl = d; }

inline ASTString Id::v() const {
  if ((_decl != nullptr) && Expression::isa<Id>(_decl)) {
    Expression* d = _decl;
    while ((d != nullptr) && Expression::isa<Id>(d)) {
      d = Expression::cast<Id>(d)->_decl;
    }
    return Expression::cast<VarDecl>(d)->id()->v();
  }
  assert(hasStr());
  return _vOrIdn.val;
}

inline long long int Id::idn() const {
  if ((_decl != nullptr) && Expression::isa<Id>(_decl)) {
    Expression* d = _decl;
    while ((d != nullptr) && Expression::isa<Id>(d)) {
      d = Expression::cast<Id>(d)->_decl;
    }
    return Expression::cast<VarDecl>(d)->id()->idn();
  }
  if (hasStr()) {
    return -1;
  }
  auto i_u = reinterpret_cast<mzn_uintptr_t>(_vOrIdn.idn) & ~static_cast<mzn_uintptr_t>(1);
  if (static_cast<mzn_intptr_t>(i_u) < 0) {
    auto ret = static_cast<mzn_intptr_t>(~(~i_u >> 1));
    return static_cast<long long int>(ret);
  }
  return static_cast<long long int>(i_u >> 1);
}

inline TIId::TIId(const Location& loc, const std::string& v)
    : BoxedExpression(loc, E_TIID, Type()), _v(ASTString(v)) {
  rehash();
}

inline TIId::TIId(const Location& loc, const ASTString& v)
    : BoxedExpression(loc, E_TIID, Type()), _v(v) {
  rehash();
}

inline AnonVar::AnonVar(const Location& loc) : BoxedExpression(loc, E_ANON, Type()) { rehash(); }

inline ArrayLit::ArrayLit(const Location& loc, ArrayLit* v,
                          const std::vector<std::pair<int, int> >& dims)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = v->_flag2;
  _secondaryId = AL_ARRAY;
  if (_flag2) {
    _u.al = v->_u.al;
    std::vector<int> d(static_cast<int>(dims.size()) * 2 + v->_dims.size() - v->dims() * 2);
    for (auto i = dims.size(); (i--) != 0U;) {
      d[i * 2] = dims[i].first;
      d[i * 2 + 1] = dims[i].second;
    }
    int sliceOffset = static_cast<int>(dims.size()) * 2;
    unsigned int origSliceOffset = v->dims() * 2;
    for (unsigned int i = 0; i < _u.al->dims() * 2; i++) {
      d[sliceOffset + i] = v->_dims[origSliceOffset + i];
    }
    _dims = ASTIntVec(d);
  } else {
    std::vector<int> d(dims.size() * 2);
    for (auto i = dims.size(); (i--) != 0U;) {
      d[i * 2] = dims[i].first;
      d[i * 2 + 1] = dims[i].second;
    }
    if (v->_u.v->flag() || d.size() != 2 || d[0] != 1) {
      // only allocate dims vector if it is not a 1d array indexed from 1
      _dims = ASTIntVec(d);
    }
    _u.v = v->_u.v;
  }
  rehash();
}

inline ArrayLit::ArrayLit(const Location& loc, ArrayLit* v)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = v->_flag2;
  _secondaryId = AL_ARRAY;
  if (_flag2) {
    _u.al = v->_u.al;
    std::vector<int> d(2 + v->_dims.size() - v->dims() * 2);
    d[0] = 1;
    d[1] = static_cast<int>(v->size());
    int sliceOffset = 2;
    unsigned int origSliceOffset = v->dims() * 2;
    for (unsigned int i = 0; i < _u.al->dims() * 2; i++) {
      d[sliceOffset + i] = v->_dims[origSliceOffset + i];
    }
    _dims = ASTIntVec(d);
  } else {
    _u.v = v->_u.v;
    if (_u.v->flag()) {
      std::vector<int> d(2);
      d[0] = 1;
      d[1] = static_cast<int>(v->length());
      _dims = ASTIntVec(d);
    } else {
      // don't allocate dims vector since this is a 1d array indexed from 1
    }
  }
  rehash();
}

inline ArrayLit::ArrayLit(const Location& loc, const std::vector<Expression*>& v)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = false;
  _secondaryId = AL_ARRAY;
  std::vector<int> d(2);
  d[0] = 1;
  d[1] = static_cast<int>(v.size());
  compress(v, d);
  rehash();
}

inline ArrayLit::ArrayLit(const Location& loc, const std::vector<KeepAlive>& v)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = false;
  _secondaryId = AL_ARRAY;
  std::vector<int> d(2);
  d[0] = 1;
  d[1] = static_cast<int>(v.size());
  std::vector<Expression*> vv(v.size());
  for (unsigned int i = 0; i < v.size(); i++) {
    vv[i] = v[i]();
  }
  compress(vv, d);
  rehash();
}

inline ArrayLit::ArrayLit(const Location& loc, const std::vector<std::vector<Expression*> >& v)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = false;
  _secondaryId = AL_ARRAY;
  std::vector<int> dims(4);
  dims[0] = 1;
  dims[1] = static_cast<int>(v.size());
  dims[2] = 1;
  dims[3] = !v.empty() ? static_cast<int>(v[0].size()) : 0;
  std::vector<Expression*> vv;
  for (const auto& i : v) {
    for (auto* j : i) {
      vv.push_back(j);
    }
  }
  compress(vv, dims);
  rehash();
}

inline ArrayLit* ArrayLit::constructTuple(const Location& loc, const std::vector<Expression*>& v) {
  auto* t = new ArrayLit(loc, v);
  t->_secondaryId = AL_TUPLE;
  return t;
}

inline ArrayLit* ArrayLit::constructTuple(const Location& loc, ArrayLit* v) {
  auto* t = new ArrayLit(loc, v);
  t->_secondaryId = AL_TUPLE;
  return t;
}

inline ArrayAccess::ArrayAccess(const Location& loc, Expression* v,
                                const std::vector<Expression*>& idx)
    : BoxedExpression(loc, E_ARRAYACCESS, Type()) {
  _v = v;
  _idx = ASTExprVec<Expression>(idx);
  rehash();
}

inline ArrayAccess::ArrayAccess(const Location& loc, Expression* v,
                                const ASTExprVec<Expression>& idx)
    : BoxedExpression(loc, E_ARRAYACCESS, Type()) {
  _v = v;
  _idx = idx;
  rehash();
}

inline FieldAccess::FieldAccess(const Location& loc, Expression* v, Expression* field)
    : BoxedExpression(loc, E_FIELDACCESS, Type()) {
  _v = v;
  _field = field;
  rehash();
}

inline void Comprehension::init(Expression* e, Generators& g) {
  _e = e;
  std::vector<Expression*> es;
  std::vector<int> idx;
  for (auto& i : g.g) {
    idx.push_back(static_cast<int>(es.size()));
    es.push_back(i._in);
    es.push_back(i._where);
    for (auto& j : i._v) {
      es.push_back(j);
    }
  }
  idx.push_back(static_cast<int>(es.size()));
  _g = ASTExprVec<Expression>(es);
  _gIndex = ASTIntVec(idx);
  rehash();
}
inline Comprehension::Comprehension(const Location& loc, Expression* e, Generators& g, bool set)
    : BoxedExpression(loc, E_COMP, Type()) {
  _flag1 = set;
  init(e, g);
}
inline void ITE::init(const std::vector<Expression*>& e_if_then, Expression* e_else) {
  _eIfThen = ASTExprVec<Expression>(e_if_then);
  _eElse = e_else;
  rehash();
}
inline ITE::ITE(const Location& loc, const std::vector<Expression*>& e_if_then, Expression* e_else)
    : BoxedExpression(loc, E_ITE, Type()) {
  init(e_if_then, e_else);
}

inline BinOp::BinOp(const Location& loc, Expression* e0, BinOpType op, Expression* e1)
    : BoxedExpression(loc, E_BINOP, Type()), _e0(e0), _e1(e1), _decl(nullptr) {
  _secondaryId = op;
  rehash();
}

inline UnOp::UnOp(const Location& loc, UnOpType op, Expression* e)
    : BoxedExpression(loc, E_UNOP, Type()), _e0(e), _decl(nullptr) {
  _secondaryId = op;
  rehash();
}

inline bool Call::hasId() const {
  return (reinterpret_cast<mzn_uintptr_t>(_uId.decl) & static_cast<mzn_uintptr_t>(1)) == 0;
}

inline ASTString Call::id() const { return hasId() ? _uId.id : decl()->id(); }

inline void Call::id(const ASTString& i) {
  _uId.id = i;
  assert(hasId());
  assert(decl() == nullptr);
}

inline FunctionI* Call::decl() const {
  return hasId() ? nullptr
                 : reinterpret_cast<FunctionI*>(reinterpret_cast<mzn_uintptr_t>(_uId.decl) &
                                                ~static_cast<mzn_uintptr_t>(1));
}

inline void Call::decl(FunctionI* f) {
  assert(f != nullptr);
  _uId.decl = reinterpret_cast<FunctionI*>(reinterpret_cast<mzn_uintptr_t>(f) |
                                           static_cast<mzn_uintptr_t>(1));
}

inline unsigned int Call::argCount() const {
  return static_cast<CallKind>(_secondaryId) >= CK_NARY
             ? static_cast<const CallNary*>(this)->_args->size()
             : static_cast<unsigned int>(_secondaryId);
}
inline Expression* Call::arg(unsigned int i) const {
  assert(i < argCount());
  return static_cast<CallKind>(_secondaryId) >= CK_NARY
             ? (*static_cast<const CallNary*>(this)->_args)[i]
             : static_cast<const Call4*>(this)->_data[i];
}
inline void Call::arg(unsigned int i, Expression* e) {
  assert(i < argCount());
  if (static_cast<CallKind>(_secondaryId) >= CK_NARY) {
    (*static_cast<CallNary*>(this)->_args)[i] = e;
  } else {
    static_cast<Call4*>(this)->_data[i] = e;
  }
}

inline Call::CallArgs::CallArgs(const Call* c) {
  if (static_cast<CallKind>(c->_secondaryId) >= CK_NARY) {
    _begin = &(*static_cast<const CallNary*>(c)->_args)[0];
  } else {
    _begin = &(static_cast<const Call4*>(c)->_data[0]);
  }
  _end = _begin + c->argCount();
}

inline Call::Call(const Location& loc, const ASTString& id0, const std::vector<Expression*>& args)
    : BoxedExpression(loc, E_CALL, Type()) {
  _flag1 = false;
  id(ASTString(id0));
  if (args.size() >= CK_NARY) {
    _secondaryId = CK_NARY;
    static_cast<CallNary*>(this)->_args = ASTExprVec<Expression>(args).vec();
  } else {
    _secondaryId = static_cast<CallKind>(args.size());
    unsigned int i = 0;
    for (auto* e : args) {
      static_cast<Call4*>(this)->_data[i] = e;
      i++;
    }
  }
  rehash();
  assert(hasId());
  assert(decl() == nullptr);
}

inline Call* Call::a(const Location& loc, const ASTString& id0,
                     const std::vector<Expression*>& args) {
  switch (args.size()) {
    case 0:
      return new Call1(loc, id0, {});
    case 1:
      return new Call1(loc, id0, args);
    case 2:
      return new Call2(loc, id0, args);
    case 3:
      return new Call3(loc, id0, args);
    case 4:
      return new Call4(loc, id0, args);
    default:
      return new CallNary(loc, id0, args);
  }
}

inline Call* Call::a(const Location& loc, const std::string& id0,
                     const std::vector<Expression*>& args) {
  return Call::a(loc, ASTString(id0), args);
}

inline VarDecl::VarDecl(const Location& loc, TypeInst* ti, const ASTString& id, Expression* e)
    : BoxedExpression(loc, E_VARDECL, ti != nullptr ? ti->type() : Type()),
      _id(nullptr),
      _flat(nullptr) {
  _id = new Id(loc, id, this);
  _flag1 = false;
  _secondaryId = 1;
  _ti = ti;
  _e = e;
  Expression::type(_id, type());
  _payload = 0;
  rehash();
}

inline VarDecl::VarDecl(const Location& loc, TypeInst* ti, long long int idn, Expression* e)
    : BoxedExpression(loc, E_VARDECL, ti != nullptr ? ti->type() : Type()),
      _id(nullptr),
      _flat(nullptr) {
  _id = new Id(loc, idn, this);
  _flag1 = false;
  _secondaryId = 1;
  _ti = ti;
  _e = e;
  Expression::type(_id, type());
  _payload = 0;
  rehash();
}

inline VarDecl::VarDecl(const Location& loc, TypeInst* ti, const std::string& id, Expression* e)
    : BoxedExpression(loc, E_VARDECL, ti != nullptr ? ti->type() : Type()),
      _id(nullptr),
      _flat(nullptr) {
  _id = new Id(loc, ASTString(id), this);
  _flag1 = false;
  _secondaryId = 1;
  _ti = ti;
  _e = e;
  Expression::type(_id, type());
  _payload = 0;
  rehash();
}

inline VarDecl::VarDecl(const Location& loc, TypeInst* ti, Id* id, Expression* e)
    : BoxedExpression(loc, E_VARDECL, ti->type()), _id(nullptr), _flat(nullptr) {
  if (id->decl() == nullptr) {
    _id = id;
    _id->decl(this);
  } else if (id->idn() == -1) {
    _id = new Id(Expression::loc(id), id->v(), this);
  } else {
    _id = new Id(Expression::loc(id), id->idn(), this);
  }
  _flag1 = false;
  _secondaryId = 1;
  _ti = ti;
  _e = e;
  Expression::type(_id, type());
  _payload = 0;
  rehash();
}

inline Expression* VarDecl::e() const {
  return (_e == nullptr || isUnboxedVal(_e)) ? _e : untag(_e);
}

inline void VarDecl::e(Expression* rhs) {
  assert(rhs == nullptr || !Expression::isa<Id>(rhs) || Expression::cast<Id>(rhs) != _id);
  _e = rhs;
}

inline bool VarDecl::toplevel() const { return (_secondaryId & 1U) == 1U; }
inline void VarDecl::toplevel(bool t) {
  if (t) {
    _secondaryId |= 1;
  } else {
    _secondaryId &= ~1;
  }
}
inline bool VarDecl::introduced() const { return (_secondaryId & 2U) == 2U; }
inline void VarDecl::introduced(bool t) {
  if (t) {
    _secondaryId |= 2;
  } else {
    _secondaryId &= ~2;
  }
}
inline bool VarDecl::evaluated() const {
  return Expression::isUnboxedVal(_e) || Expression::isTagged(_e);
}
inline void VarDecl::evaluated(bool t) {
  if (!isUnboxedVal(_e)) {
    if (t) {
      _e = Expression::tag(_e);
    } else {
      _e = Expression::untag(_e);
    }
  }
}
inline void VarDecl::flat(VarDecl* vd) { _flat = vd; }

inline TypeInst::TypeInst(const Location& loc, const Type& type, const ASTExprVec<TypeInst>& ranges,
                          Expression* domain)
    : BoxedExpression(loc, E_TI, type), _ranges(ranges), _domain(domain) {
  _flag1 = false;
  _flag2 = false;
  rehash();
}

inline TypeInst::TypeInst(const Location& loc, const Type& type, Expression* domain)
    : BoxedExpression(loc, E_TI, type), _domain(domain) {
  _flag1 = false;
  _flag2 = false;
  rehash();
}

inline IncludeI::IncludeI(const Location& loc, const ASTString& f)
    : Item(loc, II_INC), _f(f), _m(nullptr) {}

inline VarDeclI* VarDeclI::a(const Location& loc, VarDecl* e) {
  return reinterpret_cast<VarDeclI*>(e);
}

inline AssignI::AssignI(const Location& loc, const std::string& id, Expression* e)
    : Item(loc, II_ASN), _id(ASTString(id)), _e(e), _decl(nullptr) {}

inline AssignI::AssignI(const Location& loc, const ASTString& id, Expression* e)
    : Item(loc, II_ASN), _id(id), _e(e), _decl(nullptr) {}

inline ConstraintI::ConstraintI(const Location& loc, Expression* e) : Item(loc, II_CON), _e(e) {}

inline SolveI::SolveI(const Location& loc, Expression* e) : Item(loc, II_SOL), _e(e) {}
inline SolveI* SolveI::sat(const Location& loc) {
  auto* si = new SolveI(loc, nullptr);
  si->_secondaryId = ST_SAT;
  return si;
}
inline SolveI* SolveI::min(const Location& loc, Expression* e) {
  auto* si = new SolveI(loc, e);
  si->_secondaryId = ST_MIN;
  return si;
}
inline SolveI* SolveI::max(const Location& loc, Expression* e) {
  auto* si = new SolveI(loc, e);
  si->_secondaryId = ST_MAX;
  return si;
}
inline SolveI::SolveType SolveI::st() const { return static_cast<SolveType>(_secondaryId); }
inline void SolveI::st(SolveI::SolveType s) { _secondaryId = s; }

inline OutputI::OutputI(const Location& loc, Expression* e) : Item(loc, II_OUT), _e(e) {}

inline FunctionI::FunctionI(const Location& loc, const ASTString& id, TypeInst* ti,
                            const std::vector<VarDecl*>& params, Expression* e, bool from_stdlib,
                            bool capture_annotations)
    : Item(loc, II_FUN),
      _id(id),
      _ti(ti),
      _params(ASTExprVec<VarDecl>(params)),
      _e(e),
      _fromStdLib(from_stdlib),
      _captureAnnotations(capture_annotations) {
  builtins.e = nullptr;
  builtins.b = nullptr;
  builtins.f = nullptr;
  builtins.i = nullptr;
  builtins.s = nullptr;
  builtins.fs = nullptr;
  builtins.str = nullptr;
  _flag2 = false;
}

inline void FunctionI::init(const std::vector<VarDecl*>& params) {
  _params = ASTExprVec<VarDecl>(params);
}

inline void FunctionI::markParams() {
  _params.mark();
  for (auto* p : _params) {
    Expression::mark(p);
  }
}

}  // namespace MiniZinc
