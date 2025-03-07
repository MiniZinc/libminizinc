/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/iter.hh>
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/values.hh>

#include <algorithm>
#include <limits>

namespace MiniZinc {

/// Helper function that can copy the domain of a TI.
///
/// TypeInst objects cannot be shared between VarDecl's, so domains of records/tuples require copies
/// of their domain's TypeInst objects, but normal domains can be shared by multiple VarDecl's
Expression* domain_shallow_copy(EnvI& env, Expression* orig, Type type) {
  assert(GC::locked());
  if (orig == nullptr) {
    return nullptr;
  }
  auto* al = Expression::dynamicCast<ArrayLit>(orig);
  if (al == nullptr) {
    return orig;
  }
  StructType* st = env.getStructType(type);
  std::vector<Expression*> clone(al->size());
  for (unsigned int i = 0; i < al->size(); i++) {
    Type nt = (*st)[i];
    auto* ti = Expression::cast<TypeInst>((*al)[i]);
    clone[i] = new TypeInst(Expression::loc(ti), nt, ti->ranges(),
                            domain_shallow_copy(env, ti->domain(), nt));
  }
  ArrayLit* tup = ArrayLit::constructTuple(Expression::loc(orig), clone);
  tup->type(type.elemType(env));
  return tup;
}

Location::LocVec* Location::LocVec::a(const ASTString& filename, unsigned int first_line,
                                      unsigned int first_column, unsigned int last_line,
                                      unsigned int last_column) {
  static const unsigned int pointerBits = sizeof(void*) * 8;
  if (pointerBits <= 32) {
    if (first_line < (1 << 8) && last_line - first_line < (1 << 7) && first_column < (1 << 6) &&
        last_column < (1 << 7)) {
      long long int combined = first_line;
      combined |= (last_line - first_line) << 8;
      combined |= (first_column) << (8 + 7);
      combined |= (last_column) << (8 + 7 + 6);
      auto* v = static_cast<LocVec*>(alloc(2));
      new (v) LocVec(filename, combined);
      return v;
    }
  } else if (pointerBits >= 64) {
    if (first_line < (1 << 20) && last_line - first_line < (1 << 20) && first_column < (1 << 10) &&
        last_column < (1 << 10)) {
      long long int combined = first_line;
      combined |= (static_cast<long long int>(last_line - first_line)) << 20;
      combined |= (static_cast<long long int>(first_column)) << (20 + 20);
      combined |= (static_cast<long long int>(last_column)) << (20 + 20 + 10);
      auto* v = static_cast<LocVec*>(alloc(2));
      new (v) LocVec(filename, combined);
      return v;
    }
  }

  auto* v = static_cast<LocVec*>(alloc(5));
  new (v) LocVec(filename, first_line, first_column, last_line, last_column);
  return v;
}

Location::LocVec::LocVec(const ASTString& filename, IntVal combined) : ASTVec(2) {
  *(_data + 0) = filename.aststr();
  *(_data + 1) = IntLit::a(combined);
}

Location::LocVec::LocVec(const ASTString& filename, unsigned int first_line,
                         unsigned int first_column, unsigned int last_line,
                         unsigned int last_column)
    : ASTVec(5) {
  *(_data + 0) = filename.aststr();
  *(_data + 1) = IntLit::a(first_line);
  *(_data + 2) = IntLit::a(last_line);
  *(_data + 3) = IntLit::a(first_column);
  *(_data + 4) = IntLit::a(last_column);
}

Location Location::nonalloc;

Type Type::unboxedint = Type::parint();
Type Type::unboxedfloat = Type::parfloat();

Annotation Annotation::empty;

std::string Location::toString() const {
  std::ostringstream os;
  if (filename().empty()) {
    os << "unknown file";
  } else {
    os << filename();
  }
  os << ":" << firstLine() << "." << firstColumn();
  if (firstLine() != lastLine()) {
    os << "-" << lastLine() << "." << lastColumn();
  } else if (firstColumn() != lastColumn()) {
    os << "-" << lastColumn();
  }
  return os.str();
}

std::string Location::toJSON() const {
  std::ostringstream os;
  os << "{\"filename\": ";
  if (filename().empty()) {
    os << "null";
  } else {
    os << "\"" << Printer::escapeStringLit(filename()) << "\"";
  }
  os << ", \"firstLine\": " << firstLine() << ", \"firstColumn\": " << firstColumn()
     << ", \"lastLine\": " << lastLine() << ", \"lastColumn\": " << lastColumn() << "}";
  return os.str();
}

void Location::mark() const {
  if (lv() != nullptr) {
    lv()->mark();
  }
}

Location Location::introduce() const {
  Location l = *this;
  if (l._locInfo.lv != nullptr) {
    l._locInfo.t |= 1;
  }
  return l;
}

void Expression::addAnnotation(Expression* e, Expression* ann) {
  if (!isUnboxedVal(e) && e != Constants::constants().literalTrue &&
      e != Constants::constants().literalFalse &&
      !Expression::equal(ann, Constants::constants().ann.empty_annotation)) {
    e->_ann.add(ann);
  }
}
void Expression::addAnnotations(Expression* e, const std::vector<Expression*>& ann) {
  if (!isUnboxedVal(e) && e != Constants::constants().literalTrue &&
      e != Constants::constants().literalFalse) {
    for (auto* a : ann) {
      if (a != nullptr && !Expression::equal(a, Constants::constants().ann.empty_annotation)) {
        e->_ann.add(a);
      }
    }
  }
}

#define pushstack(e)      \
  do {                    \
    if ((e) != nullptr) { \
      stack.push_back(e); \
    }                     \
  } while (0)
#define pushall(v)                                \
  do {                                            \
    (v).mark();                                   \
    for (unsigned int i = 0; i < (v).size(); i++) \
      if ((v)[i] != nullptr) {                    \
        stack.push_back((v)[i]);                  \
      }                                           \
  } while (0)
#define pushann(a)                                                    \
  do {                                                                \
    for (ExpressionSetIter it = (a).begin(); it != (a).end(); ++it) { \
      pushstack(*it);                                                 \
    }                                                                 \
  } while (0)
void Expression::mark(Expression* e) {
  if (e == nullptr || isUnboxedVal(e)) {
    return;
  }
  std::vector<const Expression*> stack;
  stack.reserve(1000);
  stack.push_back(e);
  while (!stack.empty()) {
    const Expression* cur = stack.back();
    stack.pop_back();
    if (!isUnboxedVal(cur) && cur->_gcMark == 0U) {
      cur->_gcMark = 1U;
      Expression::loc(cur).mark();
      pushann(Expression::ann(cur));
      switch (Expression::eid(cur)) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_ANON:
          break;
        case Expression::E_SETLIT:
          if (Expression::cast<SetLit>(cur)->isv() != nullptr) {
            Expression::cast<SetLit>(cur)->isv()->mark();
          } else if (Expression::cast<SetLit>(cur)->fsv() != nullptr) {
            Expression::cast<SetLit>(cur)->fsv()->mark();
          } else {
            pushall(Expression::cast<SetLit>(cur)->v());
          }
          break;
        case Expression::E_STRINGLIT:
          Expression::cast<StringLit>(cur)->v().mark();
          break;
        case Expression::E_ID:
          if (Expression::cast<Id>(cur)->idn() == -1) {
            Expression::cast<Id>(cur)->v().mark();
          }
          pushstack(Expression::cast<Id>(cur)->destination());
          break;
        case Expression::E_ARRAYLIT:
          if (cur->_flag2) {
            pushstack(Expression::cast<ArrayLit>(cur)->_u.al);
          } else {
            pushall(ASTExprVec<Expression>(Expression::cast<ArrayLit>(cur)->_u.v));
          }
          Expression::cast<ArrayLit>(cur)->_dims.mark();
          break;
        case Expression::E_ARRAYACCESS:
          pushstack(Expression::cast<ArrayAccess>(cur)->v());
          pushall(Expression::cast<ArrayAccess>(cur)->idx());
          break;
        case Expression::E_FIELDACCESS:
          pushstack(Expression::cast<FieldAccess>(cur)->v());
          pushstack(Expression::cast<FieldAccess>(cur)->field());
          break;
        case Expression::E_COMP:
          pushstack(Expression::cast<Comprehension>(cur)->_e);
          pushall(Expression::cast<Comprehension>(cur)->_g);
          Expression::cast<Comprehension>(cur)->_gIndex.mark();
          break;
        case Expression::E_ITE:
          pushstack(Expression::cast<ITE>(cur)->elseExpr());
          pushall(Expression::cast<ITE>(cur)->_eIfThen);
          break;
        case Expression::E_BINOP:
          pushstack(Expression::cast<BinOp>(cur)->lhs());
          pushstack(Expression::cast<BinOp>(cur)->rhs());
          break;
        case Expression::E_UNOP:
          pushstack(Expression::cast<UnOp>(cur)->e());
          break;
        case Expression::E_CALL:
          Expression::cast<Call>(cur)->id().mark();
          for (unsigned int i = Expression::cast<Call>(cur)->argCount(); (i--) != 0U;) {
            pushstack(Expression::cast<Call>(cur)->arg(i));
          }
          if (static_cast<Call::CallKind>(cur->_secondaryId) >= Call::CK_NARY) {
            Expression::cast<CallNary>(cur)->_args->mark();
          }
          if (FunctionI* fi = Expression::cast<Call>(cur)->decl()) {
            Item::mark(fi);
          }
          break;
        case Expression::E_VARDECL:
          cur->_vdGcMark = 1U;
          pushstack(Expression::cast<VarDecl>(cur)->ti());
          pushstack(Expression::cast<VarDecl>(cur)->e());
          pushstack(Expression::cast<VarDecl>(cur)->id());
          break;
        case Expression::E_LET:
          pushall(Expression::cast<Let>(cur)->let());
          pushall(Expression::cast<Let>(cur)->_letOrig);
          pushstack(Expression::cast<Let>(cur)->in());
          break;
        case Expression::E_TI:
          pushstack(Expression::cast<TypeInst>(cur)->domain());
          pushall(Expression::cast<TypeInst>(cur)->ranges());
          break;
        case Expression::E_TIID:
          Expression::cast<TIId>(cur)->v().mark();
          break;
      }
    }
  }
}
#undef pushstack
#undef pushall

bool Expression::hasMark(Expression* e) {
  return e != nullptr && !isUnboxedVal(e) && e->_gcMark != 0U;
}

void IntLit::rehash() {
  initHash();
  std::hash<IntVal> h;
  combineHash(h(_v));
}

void FloatLit::rehash() {
  initHash();
  std::hash<FloatVal> h;
  combineHash(h(_v));
}

void SetLit::rehash() {
  initHash();
  if (isv() != nullptr) {
    std::hash<IntVal> h;
    for (IntSetRanges r0(isv()); r0(); ++r0) {
      combineHash(h(r0.min()));
      combineHash(h(r0.max()));
    }
  } else if (fsv() != nullptr) {
    std::hash<FloatVal> h;
    for (FloatSetRanges r0(fsv()); r0(); ++r0) {
      combineHash(h(r0.min()));
      combineHash(h(r0.max()));
    }
  } else {
    for (unsigned int i = v().size(); (i--) != 0U;) {
      combineHash(Expression::hash(_v[i]));
    }
  }
}

void BoolLit::rehash() {
  initHash();
  std::hash<bool> h;
  combineHash(h(_v));
}

void StringLit::rehash() {
  initHash();
  combineHash(_v.hash());
}

void Id::rehash() {
  initHash();
  std::hash<long long int> h;
  if (idn() == -1) {
    combineHash(v().hash());
  } else {
    combineHash(h(idn()));
  }
}

int Id::levenshteinDistance(Id* other) const {
  if (idn() != -1 || other->idn() != -1) {
    return std::numeric_limits<int>::max();
  }
  return v().levenshteinDistance(other->v());
}

ASTString Id::str() const {
  if (idn() == -1) {
    return v();
  }
  if (idn() < -1) {
    return ASTString("_");
  }
  std::ostringstream oss;
  oss << "X_INTRODUCED_" << idn() << "_";
  return ASTString(oss.str());
}

void TIId::rehash() {
  initHash();
  combineHash(_v.hash());
}

void AnonVar::rehash() { initHash(); }

unsigned int ArrayLit::dims() const {
  return _flag2 ? ((_dims.size() - 2 * _u.al->dims()) / 2) : (_dims.empty() ? 1 : _dims.size() / 2);
}
int ArrayLit::min(unsigned int i) const {
  if (_dims.empty()) {
    assert(i == 0);
    return 1;
  }
  return _dims[2 * i];
}
int ArrayLit::max(unsigned int i) const {
  if (_dims.empty()) {
    assert(i == 0);
    return static_cast<int>(_u.v->size());
  }
  return _dims[2 * i + 1];
}
unsigned int ArrayLit::length() const {
  if (dims() == 0) {
    return 0;
  }
  unsigned int l = max(0) - min(0) + 1;
  for (unsigned int i = 1; i < dims(); i++) {
    l *= (max(i) - min(i) + 1);
  }
  return l;
}
void ArrayLit::make1d() {
  if (!_dims.empty()) {
    GCLock lock;
    if (_flag2) {
      std::vector<int> d(2 + _u.al->dims() * 2);
      unsigned int dimOffset = dims() * 2;
      d[0] = 1;
      d[1] = static_cast<int>(length());
      for (unsigned int i = 2; i < d.size(); i++) {
        d[i] = _dims[dimOffset + i];
      }
      _dims = ASTIntVec(d);
    } else {
      std::vector<int> d(2);
      d[0] = 1;
      d[1] = static_cast<int>(length());
      _dims = ASTIntVec(d);
    }
  }
}

unsigned int ArrayLit::origIdx(unsigned int i) const {
  assert(_flag2);
  unsigned int curIdx = i;
  int multiplyer = 1;
  unsigned int oIdx = 0;
  unsigned int sliceOffset = dims() * 2;
  for (int curDim = static_cast<int>(_u.al->dims()) - 1; curDim >= 0; curDim--) {
    oIdx +=
        multiplyer *
        ((curIdx % (_dims[sliceOffset + curDim * 2 + 1] - _dims[sliceOffset + curDim * 2] + 1)) +
         (_dims[sliceOffset + curDim * 2] - _u.al->min(curDim)));
    curIdx = curIdx / (_dims[sliceOffset + curDim * 2 + 1] - _dims[sliceOffset + curDim * 2] + 1);
    multiplyer *= (_u.al->max(curDim) - _u.al->min(curDim) + 1);
  }
  return oIdx;
}

Expression* ArrayLit::getSlice(unsigned int i) const {
  if (!_flag2) {
    assert(_u.v->flag());
    int off = static_cast<int>(length()) - static_cast<int>(_u.v->size());
    return static_cast<int>(i) <= off ? (*_u.v)[0] : (*_u.v)[i - off];
  }
  assert(_flag2);
  return (*_u.al)[origIdx(i)];
}

void ArrayLit::setSlice(unsigned int i, Expression* e) {
  if (!_flag2) {
    assert(_u.v->flag());
    int off = static_cast<int>(length()) - static_cast<int>(_u.v->size());
    if (static_cast<int>(i) <= off) {
      (*_u.v)[0] = e;
    } else {
      (*_u.v)[i - off] = e;
    }
  } else {
    assert(_flag2);
    _u.al->set(origIdx(i), e);
  }
}

ArrayLit::ArrayLit(const Location& loc, ArrayLit* v, const std::vector<std::pair<int, int>>& dims,
                   const std::vector<std::pair<int, int>>& slice)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = true;
  _secondaryId = v->_secondaryId;
  _u.al = v;
  assert(slice.size() == v->dims());
  std::vector<int> d(dims.size() * 2 + 2 * slice.size());
  for (size_t i = dims.size(); (i--) != 0U;) {
    d[i * 2] = dims[i].first;
    d[i * 2 + 1] = dims[i].second;
  }
  int sliceOffset = static_cast<int>(2 * dims.size());
  for (size_t i = slice.size(); (i--) != 0U;) {
    d[sliceOffset + i * 2] = slice[i].first;
    d[sliceOffset + i * 2 + 1] = slice[i].second;
  }
  _dims = ASTIntVec(d);
}

void ArrayLit::compress(const std::vector<Expression*>& v, const std::vector<int>& dims) {
  bool allFlat = true;
  for (auto* e : v) {
    if (!Expression::isa<IntLit>(e) && !Expression::isa<FloatLit>(e) &&
        !Expression::isa<BoolLit>(e) &&
        !(Expression::isa<SetLit>(e) && Expression::cast<SetLit>(e)->evaluated()) &&
        !(Expression::isa<Id>(e) && Expression::cast<Id>(e)->decl() != nullptr &&
          Expression::cast<Id>(e)->decl()->flat() == Expression::cast<Id>(e)->decl())) {
      allFlat = false;
      break;
    }
  }
  if (allFlat) {
    flat(true);
  }
  if (v.size() >= 4 && Expression::equal(v[0], v[1]) && Expression::equal(v[1], v[2]) &&
      Expression::equal(v[2], v[3])) {
    std::vector<Expression*> compress(v.size());
    compress[0] = v[0];
    int k = 4;
    while (k < v.size() && Expression::equal(v[k], v[0])) {
      k++;
    }
    int i = 1;
    for (; k < v.size(); k++) {
      compress[i++] = v[k];
    }
    compress.resize(i);
    _u.v = ASTExprVec<Expression>(compress).vec();
    _u.v->flag(true);
    _dims = ASTIntVec(dims);
  } else {
    _u.v = ASTExprVec<Expression>(v).vec();
    if (dims.size() != 2 || dims[0] != 1) {
      // only allocate dims vector if it is not a 1d array indexed from 1
      _dims = ASTIntVec(dims);
    }
  }
}

ArrayLit::ArrayLit(const Location& loc, const std::vector<Expression*>& v,
                   const std::vector<std::pair<int, int>>& dims)
    : BoxedExpression(loc, E_ARRAYLIT, Type()) {
  _flag1 = false;
  _flag2 = false;
  _secondaryId = AL_ARRAY;
  std::vector<int> d(dims.size() * 2);
  for (size_t i = dims.size(); (i--) != 0U;) {
    d[i * 2] = dims[i].first;
    d[i * 2 + 1] = dims[i].second;
  }
  compress(v, d);
  rehash();
}

void ArrayLit::rehash() {
  initHash();
  std::hash<int> h;
  for (int _dim : _dims) {
    combineHash(h(_dim));
  }
  if (_flag2) {
    combineHash(Expression::hash(_u.al));
  } else {
    for (unsigned int i = _u.v->size(); (i--) != 0U;) {
      combineHash(h(static_cast<int>(i)));
      combineHash(Expression::hash((*_u.v)[i]));
    }
  }
}

void ArrayAccess::rehash() {
  initHash();
  combineHash(Expression::hash(_v));
  std::hash<unsigned int> h;
  combineHash(h(_idx.size()));
  for (unsigned int i = _idx.size(); (i--) != 0U;) {
    combineHash(Expression::hash(_idx[i]));
  }
}

void FieldAccess::rehash() {
  initHash();
  combineHash(Expression::hash(_v));
  combineHash(Expression::hash(_field));
}

Generator::Generator(const std::vector<ASTString>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  Location loc = in == nullptr ? Expression::loc(where) : Expression::loc(in);
  for (auto i : v) {
    auto* nvd = new VarDecl(loc, new TypeInst(loc, Type::parint()), i);
    nvd->toplevel(false);
    vd.push_back(nvd);
  }
  _v = vd;
  _in = in;
  _where = where;
}
Generator::Generator(const std::vector<Id*>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  for (auto* i : v) {
    auto* nvd =
        new VarDecl(Expression::loc(i), new TypeInst(Expression::loc(i), Type::parint()), i->v());
    nvd->toplevel(false);
    vd.push_back(nvd);
  }
  _v = vd;
  _in = in;
  _where = where;
}
Generator::Generator(const std::vector<std::string>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  Location loc = in == nullptr ? Expression::loc(where) : Expression::loc(in);
  int anon_count = -2;
  for (const auto& i : v) {
    VarDecl* nvd;
    if (i.empty()) {
      nvd = new VarDecl(loc, new TypeInst(loc, Type::parint()), anon_count--);
    } else {
      nvd = new VarDecl(loc, new TypeInst(loc, Type::parint()), ASTString(i));
    }
    nvd->toplevel(false);
    vd.push_back(nvd);
  }
  _v = vd;
  _in = in;
  _where = where;
}
Generator::Generator(const std::vector<VarDecl*>& v, Expression* in, Expression* where) {
  _v = v;
  _in = in;
  _where = where;
}
Generator::Generator(int pos, Expression* where) {
  std::vector<VarDecl*> vd;
  std::ostringstream oss;
  oss << "__dummy" << pos;
  auto* nvd =
      new VarDecl(Location().introduce(), new TypeInst(Location().introduce(), Type::parint()),
                  ASTString(oss.str()));
  nvd->toplevel(false);
  vd.push_back(nvd);
  _v = vd;
  _in = new ArrayLit(Location().introduce(), std::vector<Expression*>({IntLit::a(0)}));
  _where = where;
}

bool Comprehension::set() const { return _flag1; }
void Comprehension::rehash() {
  initHash();
  std::hash<unsigned int> h;
  combineHash(h(static_cast<unsigned int>(set())));
  combineHash(Expression::hash(_e));
  combineHash(h(_gIndex.size()));
  for (unsigned int i = _gIndex.size(); (i--) != 0U;) {
    combineHash(h(_gIndex[i]));
  }
  combineHash(h(_g.size()));
  for (unsigned int i = _g.size(); (i--) != 0U;) {
    combineHash(Expression::hash(_g[i]));
  }
}

unsigned int Comprehension::numberOfGenerators() const { return _gIndex.size() - 1; }
Expression* Comprehension::in(unsigned int i) { return _g[_gIndex[i]]; }
const Expression* Comprehension::in(unsigned int i) const { return _g[_gIndex[i]]; }
const Expression* Comprehension::where(unsigned int i) const { return _g[_gIndex[i] + 1]; }
Expression* Comprehension::where(unsigned int i) { return _g[_gIndex[i] + 1]; }

unsigned int Comprehension::numberOfDecls(unsigned int i) const {
  return _gIndex[i + 1] - _gIndex[i] - 2;
}
VarDecl* Comprehension::decl(unsigned int gen, unsigned int i) {
  return Expression::cast<VarDecl>(_g[_gIndex[gen] + 2 + i]);
}
const VarDecl* Comprehension::decl(unsigned int gen, unsigned int i) const {
  return Expression::cast<VarDecl>(_g[_gIndex[gen] + 2 + i]);
}

bool Comprehension::containsBoundVariable(Expression* e) {
  std::unordered_set<VarDecl*> decls;
  for (unsigned int i = 0; i < numberOfGenerators(); i++) {
    for (unsigned int j = 0; j < numberOfDecls(i); j++) {
      decls.insert(decl(i, j));
    }
  }
  class FindVar : public EVisitor {
    std::unordered_set<VarDecl*>& _decls;
    bool _found;

  public:
    FindVar(std::unordered_set<VarDecl*>& decls) : _decls(decls), _found(false) {}
    bool enter(Expression* /*e*/) const { return !_found; }
    void vId(Id* ident) {
      if (_decls.find(ident->decl()) != _decls.end()) {
        _found = true;
      }
    }
    bool found() const { return _found; }
  } _fv(decls);
  top_down(_fv, e);
  return _fv.found();
}

void ITE::rehash() {
  initHash();
  std::hash<unsigned int> h;
  combineHash(h(_eIfThen.size()));
  for (unsigned int i = _eIfThen.size(); (i--) != 0U;) {
    combineHash(Expression::hash(_eIfThen[i]));
  }
  combineHash(Expression::hash(elseExpr()));
}

BinOpType BinOp::op() const { return static_cast<BinOpType>(_secondaryId); }
void BinOp::rehash() {
  initHash();
  std::hash<int> h;
  combineHash(h(static_cast<int>(op())));
  combineHash(Expression::hash(_e0));
  combineHash(Expression::hash(_e1));
}

Call* BinOp::morph(const ASTString& ident, const std::vector<Expression*>& args) {
  assert(sizeof(BinOp) == sizeof(Call3));
  _id = Call::eid;
  _secondaryId = Call::CK_TERNARY;
  _flag1 = true;
  Call* c = Expression::cast<Call>(this);
  c->id(ident);
  c->args(args);
  return c;
}

namespace {

class OpToString : public GCMarker {
public:
  Id* sBOT_PLUS;       // NOLINT(readability-identifier-naming)
  Id* sBOT_MINUS;      // NOLINT(readability-identifier-naming)
  Id* sBOT_MULT;       // NOLINT(readability-identifier-naming)
  Id* sBOT_DIV;        // NOLINT(readability-identifier-naming)
  Id* sBOT_IDIV;       // NOLINT(readability-identifier-naming)
  Id* sBOT_MOD;        // NOLINT(readability-identifier-naming)
  Id* sBOT_POW;        // NOLINT(readability-identifier-naming)
  Id* sBOT_LE;         // NOLINT(readability-identifier-naming)
  Id* sBOT_LQ;         // NOLINT(readability-identifier-naming)
  Id* sBOT_GR;         // NOLINT(readability-identifier-naming)
  Id* sBOT_GQ;         // NOLINT(readability-identifier-naming)
  Id* sBOT_EQ;         // NOLINT(readability-identifier-naming)
  Id* sBOT_NQ;         // NOLINT(readability-identifier-naming)
  Id* sBOT_IN;         // NOLINT(readability-identifier-naming)
  Id* sBOT_SUBSET;     // NOLINT(readability-identifier-naming)
  Id* sBOT_SUPERSET;   // NOLINT(readability-identifier-naming)
  Id* sBOT_UNION;      // NOLINT(readability-identifier-naming)
  Id* sBOT_DIFF;       // NOLINT(readability-identifier-naming)
  Id* sBOT_SYMDIFF;    // NOLINT(readability-identifier-naming)
  Id* sBOT_INTERSECT;  // NOLINT(readability-identifier-naming)
  Id* sBOT_PLUSPLUS;   // NOLINT(readability-identifier-naming)
  Id* sBOT_EQUIV;      // NOLINT(readability-identifier-naming)
  Id* sBOT_IMPL;       // NOLINT(readability-identifier-naming)
  Id* sBOT_RIMPL;      // NOLINT(readability-identifier-naming)
  Id* sBOT_OR;         // NOLINT(readability-identifier-naming)
  Id* sBOT_AND;        // NOLINT(readability-identifier-naming)
  Id* sBOT_XOR;        // NOLINT(readability-identifier-naming)
  Id* sBOT_DOTDOT;     // NOLINT(readability-identifier-naming)
  Id* sBOT_NOT;        // NOLINT(readability-identifier-naming)

  OpToString() {
    GCLock lock;

    sBOT_PLUS = new Id(Location(), "'+'", nullptr);
    sBOT_MINUS = new Id(Location(), "'-'", nullptr);
    sBOT_MULT = new Id(Location(), "'*'", nullptr);
    sBOT_DIV = new Id(Location(), "'/'", nullptr);
    sBOT_IDIV = new Id(Location(), "'div'", nullptr);
    sBOT_MOD = new Id(Location(), "'mod'", nullptr);
    sBOT_POW = new Id(Location(), "'^'", nullptr);
    sBOT_LE = new Id(Location(), "'<'", nullptr);
    sBOT_LQ = new Id(Location(), "'<='", nullptr);
    sBOT_GR = new Id(Location(), "'>'", nullptr);
    sBOT_GQ = new Id(Location(), "'>='", nullptr);
    sBOT_EQ = new Id(Location(), "'='", nullptr);
    sBOT_NQ = new Id(Location(), "'!='", nullptr);
    sBOT_IN = new Id(Location(), "'in'", nullptr);
    sBOT_SUBSET = new Id(Location(), "'subset'", nullptr);
    sBOT_SUPERSET = new Id(Location(), "'superset'", nullptr);
    sBOT_UNION = new Id(Location(), "'union'", nullptr);
    sBOT_DIFF = new Id(Location(), "'diff'", nullptr);
    sBOT_SYMDIFF = new Id(Location(), "'symdiff'", nullptr);
    sBOT_INTERSECT = new Id(Location(), "'intersect'", nullptr);
    sBOT_PLUSPLUS = new Id(Location(), "'++'", nullptr);
    sBOT_EQUIV = new Id(Location(), "'<->'", nullptr);
    sBOT_IMPL = new Id(Location(), "'->'", nullptr);
    sBOT_RIMPL = new Id(Location(), "'<-'", nullptr);
    sBOT_OR = new Id(Location(), "'\\/'", nullptr);
    sBOT_AND = new Id(Location(), "'/\\'", nullptr);
    sBOT_XOR = new Id(Location(), "'xor'", nullptr);
    sBOT_DOTDOT = new Id(Location(), "'..'", nullptr);
    sBOT_NOT = new Id(Location(), "'not'", nullptr);
  }

  static OpToString& o() {
    static OpToString _o;
    return _o;
  }

  void mark() override {
    Expression::mark(sBOT_PLUS);
    Expression::mark(sBOT_MINUS);
    Expression::mark(sBOT_MULT);
    Expression::mark(sBOT_DIV);
    Expression::mark(sBOT_IDIV);
    Expression::mark(sBOT_MOD);
    Expression::mark(sBOT_POW);
    Expression::mark(sBOT_LE);
    Expression::mark(sBOT_LQ);
    Expression::mark(sBOT_GR);
    Expression::mark(sBOT_GQ);
    Expression::mark(sBOT_EQ);
    Expression::mark(sBOT_NQ);
    Expression::mark(sBOT_IN);
    Expression::mark(sBOT_SUBSET);
    Expression::mark(sBOT_SUPERSET);
    Expression::mark(sBOT_UNION);
    Expression::mark(sBOT_DIFF);
    Expression::mark(sBOT_SYMDIFF);
    Expression::mark(sBOT_INTERSECT);
    Expression::mark(sBOT_PLUSPLUS);
    Expression::mark(sBOT_EQUIV);
    Expression::mark(sBOT_IMPL);
    Expression::mark(sBOT_RIMPL);
    Expression::mark(sBOT_OR);
    Expression::mark(sBOT_AND);
    Expression::mark(sBOT_XOR);
    Expression::mark(sBOT_DOTDOT);
    Expression::mark(sBOT_NOT);
  }
};
}  // namespace

ASTString BinOp::opToString() const {
  switch (op()) {
    case BOT_PLUS:
      return OpToString::o().sBOT_PLUS->v();
    case BOT_MINUS:
      return OpToString::o().sBOT_MINUS->v();
    case BOT_MULT:
      return OpToString::o().sBOT_MULT->v();
    case BOT_DIV:
      return OpToString::o().sBOT_DIV->v();
    case BOT_IDIV:
      return OpToString::o().sBOT_IDIV->v();
    case BOT_MOD:
      return OpToString::o().sBOT_MOD->v();
    case BOT_POW:
      return OpToString::o().sBOT_POW->v();
    case BOT_LE:
      return OpToString::o().sBOT_LE->v();
    case BOT_LQ:
      return OpToString::o().sBOT_LQ->v();
    case BOT_GR:
      return OpToString::o().sBOT_GR->v();
    case BOT_GQ:
      return OpToString::o().sBOT_GQ->v();
    case BOT_EQ:
      return OpToString::o().sBOT_EQ->v();
    case BOT_NQ:
      return OpToString::o().sBOT_NQ->v();
    case BOT_IN:
      return OpToString::o().sBOT_IN->v();
    case BOT_SUBSET:
      return OpToString::o().sBOT_SUBSET->v();
    case BOT_SUPERSET:
      return OpToString::o().sBOT_SUPERSET->v();
    case BOT_UNION:
      return OpToString::o().sBOT_UNION->v();
    case BOT_DIFF:
      return OpToString::o().sBOT_DIFF->v();
    case BOT_SYMDIFF:
      return OpToString::o().sBOT_SYMDIFF->v();
    case BOT_INTERSECT:
      return OpToString::o().sBOT_INTERSECT->v();
    case BOT_PLUSPLUS:
      return OpToString::o().sBOT_PLUSPLUS->v();
    case BOT_EQUIV:
      return OpToString::o().sBOT_EQUIV->v();
    case BOT_IMPL:
      return OpToString::o().sBOT_IMPL->v();
    case BOT_RIMPL:
      return OpToString::o().sBOT_RIMPL->v();
    case BOT_OR:
      return OpToString::o().sBOT_OR->v();
    case BOT_AND:
      return OpToString::o().sBOT_AND->v();
    case BOT_XOR:
      return OpToString::o().sBOT_XOR->v();
    case BOT_DOTDOT:
      return OpToString::o().sBOT_DOTDOT->v();
    default:
      assert(false);
      return ASTString("");
  }
}

UnOpType UnOp::op() const { return static_cast<UnOpType>(_secondaryId); }
void UnOp::rehash() {
  initHash();
  std::hash<int> h;
  combineHash(h(static_cast<int>(_secondaryId)));
  combineHash(Expression::hash(_e0));
}

ASTString UnOp::opToString() const {
  switch (op()) {
    case UOT_PLUS:
      return OpToString::o().sBOT_PLUS->v();
    case UOT_MINUS:
      return OpToString::o().sBOT_MINUS->v();
    case UOT_NOT:
      return OpToString::o().sBOT_NOT->v();
    default:
      assert(false);
      return ASTString("");
  }
}

void Call::rehash() {
  initHash();
  combineHash(id().hash());
  std::hash<FunctionI*> hf;
  combineHash(hf(decl()));
  std::hash<unsigned int> hu;
  combineHash(hu(argCount()));
  for (unsigned int i = 0; i < argCount(); i++) {
    combineHash(Expression::hash(arg(i)));
  }
}

void Call::args(const std::vector<Expression*>& args) {
  if (argCount() == args.size()) {
    for (unsigned int i = 0; i < argCount(); i++) {
      arg(i, args[i]);
    }
  } else if (argCount() == 0 && args.size() == 1) {
    _secondaryId = CK_UNARY;
    arg(0, args[0]);
  } else {
    switch (static_cast<CallKind>(_secondaryId)) {
      case CK_BINARY:
        _secondaryId = CK_NARY_2;
        break;
      case CK_TERNARY:
        _secondaryId = CK_NARY_3;
        break;
      case CK_QUATERNARY:
        _secondaryId = CK_NARY_4;
        break;
      default:
        _secondaryId = CK_NARY;
        break;
    }
    static_cast<CallNary*>(this)->_args = ASTExprVec<Expression>(args).vec();
  }
}

/// Constructor to create commutative sorted call
Call* Call::commutativeNormalized(EnvI& env, const Call* orig) {
  auto com_sort = [](Expression* x, Expression* y) {
    if (Expression::eid(x) != Expression::eid(y)) {
      return Expression::eid(x) < Expression::eid(y);
    }
    switch (Expression::eid(x)) {
      case Expression::E_ID: {
        return Expression::cast<Id>(x)->str() < Expression::cast<Id>(y)->str();
      }
      case Expression::E_BOOLLIT: {
        return static_cast<int>(Expression::cast<BoolLit>(x)->v()) <
               static_cast<int>(Expression::cast<BoolLit>(y)->v());
      }
      case Expression::E_INTLIT: {
        return IntLit::v(Expression::cast<IntLit>(x)) < IntLit::v(Expression::cast<IntLit>(y));
      }
      case Expression::E_FLOATLIT: {
        return FloatLit::v(Expression::cast<FloatLit>(x)) <
               FloatLit::v(Expression::cast<FloatLit>(y));
      }
      case Expression::E_STRINGLIT: {
        return Expression::cast<StringLit>(x)->v() < Expression::cast<StringLit>(y)->v();
      }
      case Expression::E_SETLIT: {
        if (Expression::type(x).bt() == Type::BT_INT) {
          IntSetVal* xs = Expression::cast<SetLit>(x)->isv();
          IntSetVal* ys = Expression::cast<SetLit>(y)->isv();
          if (xs->size() != ys->size()) {
            return xs->size() < ys->size();
          }
          for (unsigned int i = 0; i < xs->size(); ++i) {
            if (xs->min(i) != ys->min(i)) {
              return xs->min(i) < ys->min(i);
            }
            if (xs->max(i) != ys->max(i)) {
              return xs->max(i) < ys->max(i);
            }
          }
          return true;  // equal
        }
        FloatSetVal* xs = Expression::cast<SetLit>(x)->fsv();
        FloatSetVal* ys = Expression::cast<SetLit>(y)->fsv();
        if (xs->size() != ys->size()) {
          return xs->size() < ys->size();
        }
        for (unsigned int i = 0; i < xs->size(); ++i) {
          if (xs->min(i) != ys->min(i)) {
            return xs->min(i) < ys->min(i);
          }
          if (xs->max(i) != ys->max(i)) {
            return xs->max(i) < ys->max(i);
          }
        }
        return true;  // equal
      }
      default: {
        return x < y;
      }
    }
  };

  assert(orig->argCount() > 0);
  Call* c = nullptr;
  if (orig->argCount() == 1) {
    assert(Expression::type(orig->arg(0)).dim() != 0);
    ArrayLit* al = eval_array_lit(env, orig->arg(0));
    std::vector<Expression*> elem(al->size());
    for (unsigned int i = 0; i < al->size(); ++i) {
      elem[i] = (*al)[i];
    }
    std::sort(elem.begin(), elem.end(), com_sort);
    auto* arg = new ArrayLit(Expression::loc(al), elem);
    arg->type(Expression::type(al));
    c = Call::a(Expression::loc(orig), orig->id(), {arg});
  } else {
    std::vector<Expression*> args(orig->argCount());
    for (unsigned int i = 0; i < orig->argCount(); ++i) {
      args[i] = orig->arg(i);
    }
    std::sort(args.begin(), args.end(), com_sort);
    c = Call::a(Expression::loc(orig), orig->id(), args);
  }
  c->decl(orig->decl());
  return c;
}

void VarDecl::trail() {
  GC::trail(&_e, e());
  if (!_ti->ranges().empty()) {
    GC::trail(reinterpret_cast<Expression**>(&_ti), _ti);
  }
}

void VarDecl::rehash() {
  initHash();
  combineHash(Expression::hash(_ti));
  combineHash(Expression::hash(_id));
  combineHash(Expression::hash(_e));
}

void Let::rehash() {
  initHash();
  combineHash(Expression::hash(_in));
  std::hash<unsigned int> h;
  combineHash(h(_let.size()));
  for (unsigned int i = _let.size(); (i--) != 0U;) {
    combineHash(Expression::hash(_let[i]));
  }
}

Let::Let(const Location& loc, const std::vector<Expression*>& let, Expression* in)
    : BoxedExpression(loc, E_LET, Type()) {
  _let = ASTExprVec<Expression>(let);
  std::vector<Expression*> vde;
  for (auto* i : let) {
    if (auto* vd = Expression::dynamicCast<VarDecl>(i)) {
      vde.push_back(vd->e());
      for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
        vde.push_back(vd->ti()->ranges()[i]->domain());
      }
    }
  }
  _letOrig = ASTExprVec<Expression>(vde);
  _in = in;
  rehash();
}

void Let::pushbindings() {
  GC::mark();
  for (unsigned int i = 0, j = 0; i < _let.size(); i++) {
    if (auto* vd = Expression::dynamicCast<VarDecl>(_let[i])) {
      vd->trail();
      vd->e(_letOrig[j++]);
      for (unsigned int k = 0; k < vd->ti()->ranges().size(); k++) {
        vd->ti()->ranges()[k]->domain(_letOrig[j++]);
      }
    }
  }
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Let::popbindings() { GC::untrail(); }

void TypeInst::rehash() {
  initHash();
  std::hash<unsigned int> h;
  unsigned int rsize = _ranges.size();
  combineHash(h(rsize));
  for (unsigned int i = rsize; (i--) != 0U;) {
    combineHash(Expression::hash(_ranges[i]));
  }
  combineHash(Expression::hash(domain()));
}

void TypeInst::setRanges(const std::vector<TypeInst*>& ranges) {
  _ranges = ASTExprVec<TypeInst>(ranges);
  if (ranges.size() == 1 && (ranges[0] != nullptr) && Expression::isa<TypeInst>(ranges[0]) &&
      (Expression::cast<TypeInst>(ranges[0])->domain() != nullptr) &&
      Expression::isa<TIId>(Expression::cast<TypeInst>(ranges[0])->domain()) &&
      !Expression::cast<TIId>(Expression::cast<TypeInst>(ranges[0])->domain())
           ->v()
           .beginsWith("$")) {
    _type.dim(-1);
  } else {
    _type.dim(static_cast<int>(ranges.size()));
  }
  rehash();
}

void TypeInst::canonicaliseStruct(EnvI& env) {
  bool isArrayOfArray = domain() != nullptr && Expression::isa<TypeInst>(domain());
  if (isArrayOfArray) {
    GCLock lock;
    auto* inner = Expression::cast<TypeInst>(domain());
    assert(inner->isarray());
    auto tid = env.registerTupleType({Expression::type(inner), Type()});
    auto nt = Type::tuple(tid);
    ArrayLit* al = ArrayLit::constructTuple(Expression::loc(inner), {inner});
    al->type(nt);
    domain(al);
  }

  if (type().bt() == Type::BT_TUPLE) {
    // Warning: Do not check TypeInst twice! A canonical tuple does not abide by the rules
    // that a user definition abides by (e.g., a tuple might be marked var (because all
    // members are var) and contain an array (with var members)).
    if (type().isvar() && (isArrayOfArray || type().typeId() == 0)) {
      auto* dom = Expression::cast<ArrayLit>(domain());
      // Check if "var" tuple is allowed
      for (unsigned int i = 0; i < dom->size(); i++) {
        auto* tii = Expression::cast<TypeInst>((*dom)[i]);
        Type field = tii->type();
        if (field.st() == Type::ST_SET && field.bt() != Type::BT_INT &&
            field.bt() != Type::BT_TOP) {
          throw TypeError(env, Expression::loc(this),
                          "var tuples with set element types other than `int' are not allowed");
        }
        if (field.bt() == Type::BT_ANN || field.bt() == Type::BT_STRING) {
          throw TypeError(env, Expression::loc(this),
                          "var tuples with " + field.toString(env) + " types are not allowed");
        }
        if (!isArrayOfArray && field.dim() != 0) {
          throw TypeError(env, Expression::loc(this),
                          "var tuples with array types are not allowed");
        }
      }
      // spread var keyword in field types:
      // var tuple (X, Y, ...) -> var tuple(var X, var Y, ...)
      mkVar(env);
    }
    env.registerTupleType(this);
  } else if (type().bt() == Type::BT_RECORD) {
    // Warning: Do not check TypeInst twice! A canonical record does not abide by the rules
    // that a user definition abides by (e.g., a record might be marked var (because all
    // members are var) and contain an array (with var members)).
    if (type().isvar() && type().typeId() == 0) {
      auto* dom = Expression::cast<ArrayLit>(domain());
      // Check if "var" record is allowed
      for (unsigned int i = 0; i < dom->size(); i++) {
        auto* tii = Expression::cast<VarDecl>((*dom)[i]);
        Type field = tii->type();
        if (field.st() == Type::ST_SET && field.bt() != Type::BT_INT &&
            field.bt() != Type::BT_TOP) {
          throw TypeError(env, Expression::loc(this),
                          "var record with set element types other than `int' are not allowed");
        }
        if (field.bt() == Type::BT_ANN || field.bt() == Type::BT_STRING) {
          throw TypeError(env, Expression::loc(this),
                          "var record with " + field.toString(env) + " types are not allowed");
        }
        if (field.dim() != 0) {
          throw TypeError(env, Expression::loc(this),
                          "var record with array types are not allowed");
        }
      }
      // spread var keyword in field types:
      // var record (X: a, Y: b, ...) -> var tuple(var X: a, var Y: b, ...)
      mkVar(env);
    }
    env.registerRecordType(this);
  }
}

void TypeInst::mkVar(const EnvI& env) {
  if (_domain == nullptr || !Expression::isa<ArrayLit>(_domain)) {
    assert(!type().structBT());
    Type tt = type();
    tt.ti(Type::TI_VAR);
    type(tt);
    return;
  }
  auto* al = Expression::cast<ArrayLit>(_domain);
  if (type().bt() == Type::BT_TUPLE) {
    for (unsigned int i = 0; i < al->size(); ++i) {
      Expression::cast<TypeInst>((*al)[i])->mkVar(env);
    }
  } else {
    if (type().typeId() != 0) {
      GCLock lock;
      RecordType* rt = env.getRecordType(type());
      for (unsigned int i = 0; i < al->size(); ++i) {
        auto* field_ti = Expression::cast<TypeInst>((*al)[i]);
        field_ti->mkVar(env);
        auto* field_vd = new VarDecl(Expression::loc(field_ti), field_ti, rt->fieldName(i));
        al->set(i, field_vd);
      }
    } else {
      for (unsigned int i = 0; i < al->size(); ++i) {
        auto* field_vd = Expression::cast<VarDecl>((*al)[i]);
        field_vd->ti()->mkVar(env);
        field_vd->type(field_vd->ti()->type());
      }
    }
  }
  // TypeId would now be invalid. Tuple type must be re-registered after mkVar call
  Type tt = type();
  tt.typeId(0);
  tt.ti(Type::TI_VAR);
  type(tt);
}

void TypeInst::mkPar(EnvI& env) {
  Type tt = type();
  tt.mkPar(env);
  std::vector<std::pair<TypeInst*, Type>> todo({{this, tt}});
  while (!todo.empty()) {
    auto it = todo.back();
    todo.pop_back();
    it.first->type(it.second);
    if (it.second.structBT()) {
      auto* al = Expression::cast<ArrayLit>(it.first->domain());
      al->type(it.second);
      auto* st = env.getStructType(it.second);
      assert(st->size() == al->size() ||
             al->size() == 1 && st->size() == 2 && (*st)[1].isunknown());
      for (unsigned int i = 0; i < al->size(); i++) {
        todo.emplace_back(Expression::cast<TypeInst>((*al)[i]), (*st)[i]);
      }
    }
  }
}

void TypeInst::setStructDomain(EnvI& env, const Type& struct_type, bool setTypeAny,
                               bool setTIRanges) {
  GCLock lock;
  StructType* st = env.getStructType(struct_type);
  std::vector<Expression*> field_ti(st->size());
  for (unsigned int i = 0; i < st->size(); ++i) {
    Type tti = (*st)[i];
    if (setTypeAny) {
      tti.any(true);
    }
    field_ti[i] = new TypeInst(Expression::loc(this).introduce(), tti);
    if (tti.structBT()) {
      Expression::cast<TypeInst>(field_ti[i])->setStructDomain(env, tti);
    } else if (tti.dim() != 0) {
      std::vector<TypeInst*> newRanges(tti.dim());
      for (int k = 0; k < tti.dim(); k++) {
        newRanges[k] = new TypeInst(Location().introduce(), Type::parint());
      }
      Expression::cast<TypeInst>(field_ti[i])->setRanges(newRanges);
    }
  }
  if (setTIRanges && ranges().size() != type().dim()) {
    assert(ranges().empty() || ranges().size() == 1 &&
                                   Expression::isa<TIId>(ranges()[0]->domain()) &&
                                   !Expression::cast<TIId>(ranges()[0]->domain())->isEnum());
    std::vector<TypeInst*> newRanges(type().dim());
    for (int k = 0; k < type().dim(); k++) {
      newRanges[k] = new TypeInst(Location().introduce(), Type::parint());
    }
    setRanges(newRanges);
  }
  auto* al = ArrayLit::constructTuple(Expression::loc(this).introduce(), field_ti);
  al->type(struct_type.elemType(env));
  domain(al);
  if (struct_type.dim() == 0 && !ranges().empty()) {
    std::vector<unsigned int> enumIds(ranges().size() + 1);
    for (unsigned int k = 0; k < ranges().size(); k++) {
      enumIds[k] = ranges()[k]->type().typeId();
    }
    enumIds[ranges().size()] = struct_type.typeId();
    Type t = struct_type;
    t.typeId(0);
    t.dim(static_cast<int>(ranges().size()));
    t.typeId(env.registerArrayEnum(enumIds));
    type(t);
  } else {
    type(struct_type);
  }
}

bool TypeInst::resolveAlias(EnvI& env) {
  auto is_aliased = [&]() {
    return domain() != nullptr && Expression::isa<Id>(domain()) &&
           Expression::cast<Id>(domain())->decl() != nullptr &&
           Expression::cast<Id>(domain())->decl()->isTypeAlias();
  };
  if (!is_aliased()) {
    return false;
  }
  GCLock lock;
  auto* alias = Expression::cast<TypeInst>(Expression::cast<Id>(domain())->decl()->e());
  Type ntype = alias->type();
  bool isArrayOfArray = false;
  if (type().dim() != 0 && ntype.dim() != 0) {
    // Array of array will get turned into a tuple
    ntype = Type::tuple(env.registerTupleType({ntype, Type()}));
    isArrayOfArray = true;
  }

  if (type().tiExplicit() && ntype.ti() != type().ti()) {
    if (type().ti() == Type::TI_VAR) {
      ntype.mkVar(env);
    } else {
      ntype.mkPar(env);
    }
  }
  if (type().otExplicit() && ntype.ot() != type().ot()) {
    if (type().ot() == Type::OT_OPTIONAL) {
      if (ntype.bt() == Type::BT_TUPLE) {
        throw TypeError(env, Expression::loc(this), "opt tuples are not allowed");
      }
      if (ntype.bt() == Type::BT_RECORD) {
        throw TypeError(env, Expression::loc(this), "opt records are not allowed");
      }
      ntype.mkOpt(env);
    } else {
      ntype.mkPresent(env);
    }
  }
  if (type().st() == Type::ST_SET) {
    if (ntype.st() == Type::ST_SET) {
      std::stringstream ss;
      ss << "Unable to create a `set of' the type aliased by `" << *domain()
         << "', which has been resolved to `" << alias->type().toString(env)
         << "' and is already a set type";
      throw TypeError(env, Expression::loc(this), ss.str());
    }
    if (ntype.dim() != 0) {
      std::stringstream ss;
      ss << "Unable to create a `set of' the type aliased by `" << *domain()
         << "', which has been resolved to `" << alias->type().toString(env)
         << "' and is an array type";
      throw TypeError(env, Expression::loc(this), ss.str());
    }
    ntype.st(Type::ST_SET);
  }
  assert(type().dim() == -1 || type().dim() == ranges().size() &&
                                   (isArrayOfArray || ntype.dim() == alias->ranges().size()));
  if (type().dim() != 0) {
    const int dim = type().dim() == -1 ? 1 : type().dim();
    const unsigned int curTypeId = type().typeId();
    const unsigned int newTypeId = ntype.typeId();
    if (curTypeId != 0 || newTypeId != 0) {
      // Type needs an Array Enum type
      std::vector<unsigned int> arrayEnumIds;
      if (curTypeId != 0) {
        arrayEnumIds = env.getArrayEnum(type().typeId());
        // This should not have been set yet.
        assert(arrayEnumIds[dim] == 0);
      } else {
        arrayEnumIds = std::vector<unsigned int>(dim + 1, 0);
      }
      if (newTypeId != 0) {
        arrayEnumIds[dim] = newTypeId;
      }
      ntype.typeId(0);
      ntype.dim(type().dim());
      ntype.typeId(env.registerArrayEnum(arrayEnumIds));
    } else {
      ntype.dim(type().dim());
    }
  } else if (ntype.dim() != 0) {
    std::vector<TypeInst*> ranges(alias->ranges().size());
    for (unsigned int i = 0; i < alias->ranges().size(); ++i) {
      ranges[i] = alias->ranges()[i];
    }
    setRanges(ranges);
  }
  type(ntype);
  if (isArrayOfArray) {
    domain(domain_shallow_copy(env, alias, ntype));
  } else {
    domain(domain_shallow_copy(env, alias->domain(), ntype));
  }
  assert(!is_aliased());  // Resolving aliases should be done in order
  return true;
}

bool TypeInst::concatDomain(EnvI& env) {
  if (domain() == nullptr || !Expression::isa<BinOp>(domain())) {
    return false;
  }
  auto* bop = Expression::cast<BinOp>(domain());
  if (bop->op() != BOT_PLUSPLUS) {
    return false;
  }
  auto* lhs = Expression::cast<TypeInst>(bop->lhs());
  auto* rhs = Expression::cast<TypeInst>(bop->rhs());

  assert(lhs->type().typeId() != 0);
  assert(rhs->type().typeId() != 0);

  ArrayLit* dom;
  Type ty;
  if (lhs->type().isrecord()) {
    assert(rhs->type().isrecord());
    GCLock lock;
    // Merge domains
    dom = eval_record_merge(env, Expression::cast<ArrayLit>(lhs->domain()),
                            Expression::cast<ArrayLit>(rhs->domain()));
    // Merge types
    ty = env.mergeRecord(lhs->type(), rhs->type(), Expression::loc(this));
    dom->type(ty);
  } else {
    assert(lhs->type().istuple());
    assert(rhs->type().istuple());
    GCLock lock;
    // Concat types
    ty = env.concatTuple(lhs->type(), rhs->type());
    // Concat domains
    auto* nbo = new BinOp(Expression::loc(bop), lhs->domain(), bop->op(), rhs->domain());
    nbo->type(ty);
    dom = ArrayLit::constructTuple(Expression::loc(bop).introduce(), eval_array_lit(env, nbo));
    dom->type(ty);
  }
  // Update TI
  domain(dom);

  unsigned int tId = ty.typeId();
  ty.typeId(0);
  ty.dim(type().dim());
  ty.typeId(tId);
  type(ty);
  return true;
}

bool TypeInst::hasTiVariable() const {
  if (domain() != nullptr) {
    if (Expression::isa<TIId>(domain())) {
      return true;
    }
    if (auto* al = Expression::dynamicCast<ArrayLit>(domain())) {
      for (unsigned int i = 0; i < al->size(); ++i) {
        auto* ti = Expression::cast<TypeInst>((*al)[i]);
        if (ti->hasTiVariable()) {
          return true;
        }
      }
    }
  }
  for (unsigned int i = 0; i < _ranges.size(); ++i) {
    if (_ranges[i]->domain() != nullptr && Expression::isa<TIId>(_ranges[i]->domain())) {
      return true;
    }
  }
  return false;
}

namespace {
Type get_type(Expression* e) { return Expression::type(e); }
Type get_type(const Type& t) { return t; }
const Location& get_loc(Expression* e, Expression* call, FunctionI* /*fi*/) {
  return Expression::loc(e).isNonAlloc() ? Expression::loc(call) : Expression::loc(e);
}
const Location& get_loc(const Type& /*t*/, Expression* call, FunctionI* fi) {
  if (call == nullptr || Expression::loc(call).isNonAlloc()) {
    return fi == nullptr ? Location::nonalloc : fi->loc();
  }
  return Expression::loc(call);
}

bool isa_tiid(Expression* e) {
  if (TIId* t = Expression::dynamicCast<TIId>(e)) {
    return !t->v().beginsWith("$");
  }
  return false;
}
bool isa_enum_tiid(Expression* e) {
  if (TIId* t = Expression::dynamicCast<TIId>(e)) {
    return t->v().beginsWith("$");
  }
  return false;
}

// Compute return type of function \a fi given argument types \ta
template <class T>
Type return_type(EnvI& env, FunctionI* fi, const std::vector<T>& ta, Expression* call,
                 bool strictEnum) {
  if (fi->id() == env.constants.varRedef->id()) {
    return Type::varbool();
  }
  std::vector<std::pair<TypeInst*, Type>> stack(ta.size());
  for (unsigned int i = 0; i < ta.size(); i++) {
    stack[i] = std::make_pair(fi->param(i)->ti(), get_type(ta[i]));
  }

  ASTStringMap<std::pair<Type, bool>> tmap;
  while (!stack.empty()) {
    std::pair<TypeInst*, Type> cur(stack.back());
    stack.pop_back();
    TypeInst* tii = cur.first;
    if (tii->domain()) {
      if (Expression::isa<TIId>(tii->domain())) {
        ASTString tiid = Expression::cast<TIId>(tii->domain())->v();
        Type tiit = cur.second;
        bool isEnumTIID = isa_enum_tiid(tii->domain());
        if (tii->type().any()) {
          tiit.any(true);
        }
        tiit = tiit.elemType(env);
        auto it = tmap.find(tiid);
        if (it == tmap.end()) {
          tmap.insert(std::pair<ASTString, std::pair<Type, bool>>(tiid, {tiit, isEnumTIID}));
        } else {
          // We've seen this identifier before, unify the types
          if (it->second.first.dim() > 0) {
            std::ostringstream ss;
            ss << "type-inst variable $" << tiid << " used in both array and non-array position";
            throw TypeError(env, get_loc(tiit, call, fi), ss.str());
          }
          Type tiit_par = tiit;
          tiit_par.any(false);
          tiit_par.mkPar(env);
          tiit_par.mkPresent(env);
          if (isEnumTIID) {
            tiit_par.st(Type::ST_SET);
          }
          Type its_par = it->second.first;
          its_par.any(false);
          its_par.mkPar(env);
          its_par.mkPresent(env);
          if (it->second.second) {
            its_par.st(Type::ST_SET);
          }
          if (tiit_par.bt() == Type::BT_TOP || tiit_par.bt() == Type::BT_BOT) {
            tiit_par.bt(its_par.bt());
            tiit_par.typeId(its_par.typeId());
          }
          if (its_par.bt() == Type::BT_TOP || its_par.bt() == Type::BT_BOT) {
            its_par.bt(tiit_par.bt());
            its_par.typeId(tiit_par.typeId());
          }
          if (env.isSubtype(tiit_par, its_par, strictEnum)) {
            if (it->second.first.bt() == Type::BT_TOP) {
              it->second.first.bt(tiit.bt());
              it->second.first.typeId(tiit.typeId());
            }
          } else if (env.isSubtype(its_par, tiit_par, strictEnum)) {
            it->second.first.bt(tiit_par.bt());
            it->second.first.typeId(tiit_par.typeId());
          } else {
            std::ostringstream ss;
            ss << "type-inst variable $" << tiid << " instantiated with different types ("
               << tiit.toString(env) << " vs " << it->second.first.toString(env) << ")";
            throw TypeError(env, get_loc(tiit, call, fi), ss.str());
          }
          if (tiit.isvar()) {
            it->second.first.mkVar(env);
          }
          if (tiit.isOpt() && it->second.first.st() == Type::ST_PLAIN) {
            it->second.first.mkOpt(env);
          }
        }
      } else if (cur.second.structBT()) {
        auto* al = Expression::cast<ArrayLit>(tii->domain());
        StructType* tiit_st = env.getStructType(cur.second);
        for (unsigned int i = 0; i < al->size(); ++i) {
          stack.emplace_back(Expression::cast<TypeInst>((*al)[i]), (*tiit_st)[i]);
        }
      }
    }
    if (tii->ranges().size() == 1 && isa_tiid(tii->ranges()[0]->domain())) {
      ASTString tiid = Expression::cast<TIId>(tii->ranges()[0]->domain())->v();
      Type orig_tiit = get_type(cur.second);
      if (orig_tiit.dim() == 0) {
        std::ostringstream ss;
        ss << "type-inst variable $" << tiid << " must be an array index";
        throw TypeError(env, get_loc(cur.second, call, fi), ss.str());
      }
      Type tiit = Type::top(orig_tiit.dim());
      if (orig_tiit.typeId() != 0) {
        std::vector<unsigned int> enumIds = env.getArrayEnum(orig_tiit.typeId());
        enumIds[enumIds.size() - 1] = 0;
        tiit.typeId(env.registerArrayEnum(enumIds));
      }
      auto it = tmap.find(tiid);
      if (it == tmap.end()) {
        tmap.insert(std::pair<ASTString, std::pair<Type, bool>>(tiid, {tiit, false}));
      } else {
        if (it->second.first.dim() == 0) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " used in both array and non-array position";
          throw TypeError(env, get_loc(cur.second, call, fi), ss.str());
        }
        if (it->second.first != tiit) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " instantiated with different types ("
             << tiit.toString(env) + " vs " << it->second.first.toString(env) << ")";
          throw TypeError(env, get_loc(cur.second, call, fi), ss.str());
        }
      }
    } else if (!tii->ranges().empty()) {
      for (unsigned int j = 0; j < tii->ranges().size(); j++) {
        if (isa_enum_tiid(tii->ranges()[j]->domain())) {
          ASTString enumTIId = Expression::cast<TIId>(tii->ranges()[j]->domain())->v();
          Type tiit = cur.second;
          Type enumIdT;
          if (tiit.typeId() != 0) {
            unsigned int enumId = env.getArrayEnum(tiit.typeId())[j];
            enumIdT = Type::parsetenum(enumId);
          } else {
            enumIdT = Type::parsetint();
          }
          auto it = tmap.find(enumTIId);
          // TODO: this may clash if the same enum TIId is used for different types
          // but the same enum
          if (it == tmap.end()) {
            tmap.insert(std::pair<ASTString, std::pair<Type, bool>>(enumTIId, {enumIdT, true}));
          } else if (strictEnum && it->second.first.typeId() != enumIdT.typeId()) {
            std::ostringstream ss;
            ss << "type-inst variable $" << enumTIId << " used for different enum types";
            throw TypeError(env, get_loc(cur.second, call, fi), ss.str());
          }
        }
      }
    }
  }
  return type_from_tmap(env, fi->ti(), tmap);
}
}  // namespace

Type type_from_tmap(EnvI& env, TypeInst* ti, const ASTStringMap<std::pair<Type, bool>>& tmap) {
  Type ret = ti->type();
  if (ret.structBT()) {
    auto* al = Expression::cast<ArrayLit>(ti->domain());
    auto isArrayOfArray = false;
    if (ret.bt() == Type::BT_TUPLE && ret.typeId() != 0) {
      auto* tt = env.getTupleType(ret);
      if (tt->size() == 2 && (*tt)[1].isunknown()) {
        isArrayOfArray = true;
      }
    }
    std::vector<Type> fields(al->size() + (isArrayOfArray ? 1U : 0U));
    for (unsigned int i = 0; i < al->size(); i++) {
      fields[i] = type_from_tmap(env, Expression::cast<TypeInst>((*al)[i]), tmap);
      ret.cv(ret.cv() || fields[i].cv());
    }
    if (isArrayOfArray) {
      fields[al->size()] = Type();
    }
    unsigned int typeId = 0;
    if (ret.bt() == Type::BT_TUPLE) {
      typeId = env.registerTupleType(fields);
    } else {
      auto* rt = env.getRecordType(ret);
      typeId = env.registerRecordType(rt, fields);
    }
    if (!ti->ranges().empty()) {
      std::vector<unsigned int> enumIds(ti->ranges().size() + 1);
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        enumIds[i] = 0;
      }
      enumIds[enumIds.size() - 1] = typeId;
      ret.typeId(env.registerArrayEnum(enumIds));
    } else {
      ret.typeId(typeId);
    }
  }
  ASTString dh;
  if (ti->domain() != nullptr && Expression::isa<TIId>(ti->domain())) {
    dh = Expression::cast<TIId>(ti->domain())->v();
  }
  ASTString rh;
  if (ti->ranges().size() == 1 && isa_tiid(ti->ranges()[0]->domain())) {
    rh = Expression::cast<TIId>(ti->ranges()[0]->domain())->v();
  }
  if (!dh.empty()) {
    auto it = tmap.find(dh);
    if (it == tmap.end()) {
      std::ostringstream ss;
      ss << "type-inst variable $" << dh << " used but not defined";
      throw TypeError(env, Expression::loc(ti), ss.str());
    }
    if (dh.beginsWith("$")) {
      // this is an enum
      ret.bt(Type::BT_INT);
    } else {
      ret.bt(it->second.first.bt());
      if (ret.st() == Type::ST_PLAIN) {
        ret.st(it->second.first.st());
      }
      if (ret.any()) {
        ret.ot(it->second.first.ot());
        ret.ti(it->second.first.ti());
        ret.cv(it->second.first.cv());
        ret.any(false);
      }
    }
    if (!ti->ranges().empty() && it->second.first.typeId() != 0) {
      std::vector<unsigned int> enumIds(ti->ranges().size() + 1);
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        enumIds[i] = 0;
      }
      enumIds[enumIds.size() - 1] = it->second.first.typeId();
      ret.typeId(env.registerArrayEnum(enumIds));
    } else {
      ret.typeId(it->second.first.typeId());
      ret.cv(ret.cv() || it->second.first.cv());
    }
  }
  if (!rh.empty()) {
    auto it = tmap.find(rh);
    if (it == tmap.end()) {
      std::ostringstream ss;
      ss << "type-inst variable $" << rh << " used but not defined";
      throw TypeError(env, Expression::loc(ti), ss.str());
    }
    unsigned int curTypeId = ret.typeId();
    ret.typeId(0);
    ret.dim(it->second.first.dim());
    ret.typeId(curTypeId);
    if (it->second.first.typeId() != 0) {
      std::vector<unsigned int> enumIds(it->second.first.dim() + 1);
      const std::vector<unsigned int>& orig_enumIds = env.getArrayEnum(it->second.first.typeId());
      for (unsigned int i = 0; i < enumIds.size() - 1; i++) {
        enumIds[i] = orig_enumIds[i];
      }
      if (curTypeId != 0 && ret.dim() != 0) {
        const auto& curIds = env.getArrayEnum(curTypeId);
        curTypeId = curIds[curIds.size() - 1];
      }
      enumIds[enumIds.size() - 1] = curTypeId;
      ret.typeId(env.registerArrayEnum(enumIds));
    }
  } else if (!ti->ranges().empty()) {
    std::vector<unsigned int> enumIds(ti->ranges().size() + 1);
    bool hadRealEnum = false;
    if (ret.typeId() == 0) {
      enumIds[enumIds.size() - 1] = 0;
    } else {
      enumIds = env.getArrayEnum(ret.typeId());
      hadRealEnum = true;
    }

    for (unsigned int i = 0; i < ti->ranges().size(); i++) {
      if (isa_enum_tiid(ti->ranges()[i]->domain())) {
        ASTString enumTIId = Expression::cast<TIId>(ti->ranges()[i]->domain())->v();
        auto it = tmap.find(enumTIId);
        if (it == tmap.end()) {
          std::ostringstream ss;
          ss << "type-inst variable $" << enumTIId << " used but not defined";
          throw TypeError(env, Expression::loc(ti), ss.str());
        }
        enumIds[i] = it->second.first.typeId();
        hadRealEnum |= (enumIds[i] != 0);
      }
    }
    if (hadRealEnum) {
      ret.typeId(env.registerArrayEnum(enumIds));
    }
  }
  if (ti->type().isPar()) {
    ret.mkPar(env);
  }
  return ret;
}

void Item::mark(Item* item) {
  if (item->hasMark()) {
    return;
  }
  item->_gcMark = 1;
  item->loc().mark();
  switch (item->iid()) {
    case Item::II_INC:
      item->cast<IncludeI>()->f().mark();
      break;
    case Item::II_VD:
      item->_gcMark = 0;  // need to reset so that Expression::mark works
      Expression::mark(item->cast<VarDeclI>()->e());
#if defined(MINIZINC_GC_STATS)
      GC::stats()[item->cast<VarDeclI>()->e()->Expression::eid()].inmodel++;
#endif
      break;
    case Item::II_ASN:
      item->cast<AssignI>()->id().mark();
      Expression::mark(item->cast<AssignI>()->e());
      Expression::mark(item->cast<AssignI>()->decl());
      break;
    case Item::II_CON:
      Expression::mark(item->cast<ConstraintI>()->e());
#if defined(MINIZINC_GC_STATS)
      GC::stats()[item->cast<ConstraintI>()->e()->Expression::eid()].inmodel++;
#endif
      break;
    case Item::II_SOL: {
      auto* si = item->cast<SolveI>();
      for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
        Expression::mark(*it);
      }
      Expression::mark(item->cast<SolveI>()->e());
    } break;
    case Item::II_OUT: {
      auto* oi = item->cast<OutputI>();
      Expression::mark(oi->e());
      for (ExpressionSetIter it = oi->ann().begin(); it != oi->ann().end(); ++it) {
        Expression::mark(*it);
      }
    } break;
    case Item::II_FUN: {
      auto* fi = item->cast<FunctionI>();
      fi->id().mark();
      Expression::mark(fi->ti());
      for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
        Expression::mark(*it);
      }
      Expression::mark(fi->e());
      fi->markParams();
    } break;
  }
}

Type FunctionI::rtype(EnvI& env, const std::vector<Expression*>& ta, Expression* call,
                      bool strictEnums) {
  return return_type(env, this, ta, call, strictEnums);
}

Type FunctionI::rtype(EnvI& env, const std::vector<Type>& ta, Expression* call, bool strictEnums) {
  return return_type(env, this, ta, call, strictEnums);
}

Type FunctionI::argtype(EnvI& env, const std::vector<Expression*>& ta, unsigned int n) const {
  if (this == env.constants.varRedef) {
    return Type::top();
  }
  // Given the concrete types for all function arguments ta, compute the
  // least common supertype that fits function parameter n.
  TypeInst* tii = param(n)->ti();
  Type curTiiT = tii->type();
  Type dimTy = curTiiT;
  if (curTiiT.dim() == -1) {
    if (env.getTransparentType(ta[n]).dim() == 0) {
      dimTy = Type::partop(1);
    } else {
      dimTy = env.getTransparentType(ta[n]);
      if (dimTy.dim() == -1) {
        dimTy = Type::partop(1);
      }
    }
  }
  if ((tii->domain() != nullptr) && Expression::isa<TIId>(tii->domain())) {
    // We need to determine both the base type and whether this tiid
    // can stand for a set. It can only stand for a set if none
    // of the uses of tiid is opt. The base type has to be int
    // if any of the uses are var set.

    Type ty = env.getTransparentType(ta[n]);
    if (!ty.structBT()) {
      ty.st(curTiiT.st());
    }
    if (dimTy.dim() != ty.dim()) {
      if (dimTy.dim() == 0) {
        ty = ty.elemType(env);
      } else {
        ty = Type::arrType(env, dimTy, ty);
      }
    }
    ASTString tv = Expression::cast<TIId>(tii->domain())->v();
    for (unsigned int i = 0; i < paramCount(); i++) {
      if ((param(i)->ti()->domain() != nullptr) &&
          Expression::isa<TIId>(param(i)->ti()->domain()) &&
          Expression::cast<TIId>(param(i)->ti()->domain())->v() == tv) {
        Type toCheck = env.getTransparentType(ta[i]);
        if (!toCheck.structBT()) {
          toCheck.ot(curTiiT.ot());
          toCheck.st(curTiiT.st());
        }
        if (dimTy.dim() != toCheck.dim()) {
          if (dimTy.dim() == 0) {
            toCheck = toCheck.elemType(env);
          } else {
            toCheck = Type::arrType(env, dimTy, toCheck);
          }
        }
        if (toCheck != ty) {
          if (env.isSubtype(ty, toCheck, true)) {
            ty = toCheck;
          } else {
            Type ty_par = ty;
            ty_par.mkPar(env);
            Type toCheck_par = toCheck;
            toCheck_par.mkPar(env);
            if (env.isSubtype(ty_par, toCheck_par, true)) {
              ty.bt(toCheck.bt());
            }
          }
        }
      }
    }
    return ty;
  }
  return curTiiT;
}

bool Expression::equalInternal(const Expression* e0, const Expression* e1) {
  switch (Expression::eid(e0)) {
    case Expression::E_INTLIT:
      return IntLit::v(Expression::cast<IntLit>(e0)) == IntLit::v(Expression::cast<IntLit>(e1));
    case Expression::E_FLOATLIT:
      return FloatLit::v(Expression::cast<FloatLit>(e0)) ==
             FloatLit::v(Expression::cast<FloatLit>(e1));
    case Expression::E_SETLIT: {
      const auto* s0 = Expression::cast<SetLit>(e0);
      const auto* s1 = Expression::cast<SetLit>(e1);
      if (s0->isv() != nullptr) {
        if (s1->isv() != nullptr) {
          IntSetRanges r0(s0->isv());
          IntSetRanges r1(s1->isv());
          return Ranges::equal(r0, r1);
        }
        return false;
      }
      if (s0->fsv() != nullptr) {
        if (s1->fsv() != nullptr) {
          FloatSetRanges r0(s0->fsv());
          FloatSetRanges r1(s1->fsv());
          return Ranges::equal(r0, r1);
        }
        return false;
      }
      if ((s1->isv() != nullptr) || (s1->fsv() != nullptr)) {
        return false;
      }
      if (s0->v().size() != s1->v().size()) {
        return false;
      }
      for (unsigned int i = 0; i < s0->v().size(); i++) {
        if (!Expression::equal(s0->v()[i], s1->v()[i])) {
          return false;
        }
      }
      return true;
    }
    case Expression::E_BOOLLIT:
      return Expression::cast<BoolLit>(e0)->v() == Expression::cast<BoolLit>(e1)->v();
    case Expression::E_STRINGLIT:
      return Expression::cast<StringLit>(e0)->v() == Expression::cast<StringLit>(e1)->v();
    case Expression::E_ID: {
      const Id* id0 = Expression::cast<Id>(e0);
      const Id* id1 = Expression::cast<Id>(e1);
      if (id0->decl() == nullptr || id1->decl() == nullptr) {
        return id0->v() == id1->v() && id0->idn() == id1->idn();
      }
      return id0->decl() == id1->decl() ||
             (id0->decl()->flat() != nullptr && id0->decl()->flat() == id1->decl()->flat());
    }
    case Expression::E_ANON:
      return false;
    case Expression::E_ARRAYLIT: {
      const auto* a0 = Expression::cast<ArrayLit>(e0);
      const auto* a1 = Expression::cast<ArrayLit>(e1);
      if (a0->size() != a1->size()) {
        return false;
      }
      if (a0->_dims.size() != a1->_dims.size()) {
        return false;
      }
      for (unsigned int i = 0; i < a0->_dims.size(); i++) {
        if (a0->_dims[i] != a1->_dims[i]) {
          return false;
        }
      }
      for (unsigned int i = 0; i < a0->size(); i++) {
        if (!Expression::equal((*a0)[i], (*a1)[i])) {
          return false;
        }
      }
      return true;
    }
    case Expression::E_ARRAYACCESS: {
      const auto* a0 = Expression::cast<ArrayAccess>(e0);
      const auto* a1 = Expression::cast<ArrayAccess>(e1);
      if (!Expression::equal(a0->v(), a1->v())) {
        return false;
      }
      if (a0->idx().size() != a1->idx().size()) {
        return false;
      }
      for (unsigned int i = 0; i < a0->idx().size(); i++) {
        if (!Expression::equal(a0->idx()[i], a1->idx()[i])) {
          return false;
        }
      }
      return true;
    }
    case Expression::E_FIELDACCESS: {
      const auto* f0 = Expression::cast<FieldAccess>(e0);
      const auto* f1 = Expression::cast<FieldAccess>(e1);
      if (!Expression::equal(f0->field(), f1->field())) {
        return false;
      }
      return Expression::equal(f0->v(), f1->v());
    }
    case Expression::E_COMP: {
      const auto* c0 = Expression::cast<Comprehension>(e0);
      const auto* c1 = Expression::cast<Comprehension>(e1);
      if (c0->set() != c1->set()) {
        return false;
      }
      if (!Expression::equal(c0->_e, c1->_e)) {
        return false;
      }
      if (c0->_g.size() != c1->_g.size()) {
        return false;
      }
      for (unsigned int i = 0; i < c0->_g.size(); i++) {
        if (!Expression::equal(c0->_g[i], c1->_g[i])) {
          return false;
        }
      }
      for (unsigned int i = 0; i < c0->_gIndex.size(); i++) {
        if (c0->_gIndex[i] != c1->_gIndex[i]) {
          return false;
        }
      }
      return true;
    }
    case Expression::E_ITE: {
      const ITE* i0 = Expression::cast<ITE>(e0);
      const ITE* i1 = Expression::cast<ITE>(e1);
      if (i0->_eIfThen.size() != i1->_eIfThen.size()) {
        return false;
      }
      for (unsigned int i = i0->_eIfThen.size(); (i--) != 0U;) {
        if (!Expression::equal(i0->_eIfThen[i], i1->_eIfThen[i])) {
          return false;
        }
      }
      return Expression::equal(i0->elseExpr(), i1->elseExpr());
    }
    case Expression::E_BINOP: {
      const auto* b0 = Expression::cast<BinOp>(e0);
      const auto* b1 = Expression::cast<BinOp>(e1);
      if (b0->op() != b1->op()) {
        return false;
      }
      if (!Expression::equal(b0->lhs(), b1->lhs())) {
        return false;
      }
      if (!Expression::equal(b0->rhs(), b1->rhs())) {
        return false;
      }
      return true;
    }
    case Expression::E_UNOP: {
      const UnOp* b0 = Expression::cast<UnOp>(e0);
      const UnOp* b1 = Expression::cast<UnOp>(e1);
      if (b0->op() != b1->op()) {
        return false;
      }
      if (!Expression::equal(b0->e(), b1->e())) {
        return false;
      }
      return true;
    }
    case Expression::E_CALL: {
      const Call* c0 = Expression::cast<Call>(e0);
      const Call* c1 = Expression::cast<Call>(e1);
      if (c0->id() != c1->id()) {
        return false;
      }
      if (c0->decl() != c1->decl()) {
        return false;
      }
      if (c0->argCount() != c1->argCount()) {
        return false;
      }
      for (unsigned int i = 0; i < c0->argCount(); i++) {
        if (!Expression::equal(c0->arg(i), c1->arg(i))) {
          return false;
        }
      }
      return true;
    }
    case Expression::E_VARDECL: {
      const auto* v0 = Expression::cast<VarDecl>(e0);
      const auto* v1 = Expression::cast<VarDecl>(e1);
      if (!Expression::equal(v0->ti(), v1->ti())) {
        return false;
      }
      if (!Expression::equal(v0->id(), v1->id())) {
        return false;
      }
      if (!Expression::equal(v0->e(), v1->e())) {
        return false;
      }
      return true;
    }
    case Expression::E_LET: {
      const Let* l0 = Expression::cast<Let>(e0);
      const Let* l1 = Expression::cast<Let>(e1);
      if (!Expression::equal(l0->in(), l1->in())) {
        return false;
      }
      if (l0->let().size() != l1->let().size()) {
        return false;
      }
      for (unsigned int i = l0->let().size(); (i--) != 0U;) {
        if (!Expression::equal(l0->let()[i], l1->let()[i])) {
          return false;
        }
      }
      return true;
    }
    case Expression::E_TI: {
      const auto* t0 = Expression::cast<TypeInst>(e0);
      const auto* t1 = Expression::cast<TypeInst>(e1);
      if (t0->ranges().size() != t1->ranges().size()) {
        return false;
      }
      for (unsigned int i = t0->ranges().size(); (i--) != 0U;) {
        if (!Expression::equal(t0->ranges()[i], t1->ranges()[i])) {
          return false;
        }
      }
      return Expression::equal(t0->domain(), t1->domain());
    }
    case Expression::E_TIID:
      return false;
    default:
      assert(false);
      return false;
  }
}

Constants::Constants() {
  GCLock lock;
  auto* ti = new TypeInst(Location(), Type::parbool());
  emptyBoolArray = new ArrayLit(Location(), std::vector<Expression*>{});
  emptyBoolArray->type(Type::parbool(1));
  literalTrue = new BoolLit(Location(), true);
  varTrue = new VarDecl(Location(), ti, "_bool_true", literalTrue);
  literalFalse = new BoolLit(Location(), false);
  varFalse = new VarDecl(Location(), ti, "_bool_false", literalFalse);
  varIgnore = new VarDecl(Location(), ti, "_bool_ignore");
  absent = new Id(Location(), "_absent", nullptr);
  varRedef = new FunctionI(Location(), ASTString("__internal_varRedef"),
                           new TypeInst(Location(), Type::varbool()), std::vector<VarDecl*>());
  Type absent_t;
  absent_t.bt(Type::BT_BOT);
  absent_t.dim(0);
  absent_t.st(Type::ST_PLAIN);
  absent_t.ot(Type::OT_OPTIONAL);
  Expression::type(absent, absent_t);

  IntSetVal* isv_infty = IntSetVal::a(-IntVal::infinity(), IntVal::infinity());
  infinityInt = new SetLit(Location(), isv_infty);
  FloatSetVal* fsv_infty = FloatSetVal::a(-FloatVal::infinity(), FloatVal::infinity());
  infinityFloat = new SetLit(Location(), fsv_infty);

  ids.forall = addString("forall");
  ids.forallReif = addString("forallReif");
  ids.exists = addString("exists");
  ids.clause = addString("clause");
  ids.bool2int = addString("bool2int");
  ids.int2float = addString("int2float");
  ids.bool2float = addString("bool2float");
  ids.enum2int = addString("enum2int");
  ids.index2int = addString("index2int");
  ids.to_enum_internal = addString("to_enum_internal");
  ids.set2iter = addString("set2iter");
  ids.assert = addString("assert");
  ids.assert_dbg = addString("assert_dbg");

  ids.deopt = addString("deopt");
  ids.absent = addString("absent");
  ids.occurs = addString("occurs");
  ids.card = addString("card");
  ids.abs = addString("abs");

  ids.mzn_alias_eq = addString("mzn_alias_eq");

  ids.symmetry_breaking_constraint = addString("symmetry_breaking_constraint");
  ids.redundant_constraint = addString("redundant_constraint");
  ids.implied_constraint = addString("implied_constraint");
  ids.mzn_deprecate = addString("mzn_deprecate");
  ids.mzn_symmetry_breaking_constraint = addString("mzn_symmetry_breaking_constraint");
  ids.mzn_redundant_constraint = addString("mzn_redundant_constraint");
  ids.mzn_reverse_map_var = addString("mzn_reverse_map_var");
  ids.mzn_in_root_context = addString("mzn_in_root_context");
  ids.mzn_output_section = addString("mzn_output_section");
  ids.output_to_section = addString("output_to_section");
  ids.output_to_json_section = addString("output_to_json_section");
  ids.mzn_default = addString("default");
  ids.trace = addString("trace");
  ids.trace_dbg = addString("trace_dbg");
  ids.mzn_trace_to_section = addString("mzn_trace_to_section");

  ids.array1d = addString("array1d");
  ids.array2d = addString("array2d");
  ids.array3d = addString("array3d");
  ids.array4d = addString("array4d");
  ids.array5d = addString("array5d");
  ids.array6d = addString("array6d");
  ids.arrayXd = addString("arrayXd");

  ids.sum = addString("sum");
  ids.lex_less = addString("lex_less");
  ids.lex_lesseq = addString("lex_lesseq");
  ids.lin_exp = addString("lin_exp");
  ids.count = addString("count");
  ids.element = addString("element");
  ids.table = addString("table");
  ids.anon_enum = addString("anon_enum");
  ids.anon_enum_set = addString("anon_enum_set");
  ids.enumFromConstructors = addString("enumFromConstructors");
  ids.enumOf = addString("enum_of");
  ids.enumOfInternal = addString("enum_of_internal");

  ids.concat = addString("concat");
  ids.join = addString("join");
  ids.show = addString("show");
  ids.format = addString("format");
  ids.format_justify_string = addString("format_justify_string");
  ids.showDzn = addString("showDzn");
  ids.showJSON = addString("showJSON");
  ids.output = addString("output");
  ids.outputJSON = addString("outputJSON");
  ids.fix = addString("fix");

  ids.int_.lin_eq = addString("int_lin_eq");
  ids.int_.lin_le = addString("int_lin_le");
  ids.int_.lin_ne = addString("int_lin_ne");
  ids.int_.plus = addString("int_plus");
  ids.int_.minus = addString("int_minus");
  ids.int_.times = addString("int_times");
  ids.int_.div = addString("int_div");
  ids.int_.mod = addString("int_mod");
  ids.int_.lt = addString("int_lt");
  ids.int_.le = addString("int_le");
  ids.int_.gt = addString("int_gt");
  ids.int_.ge = addString("int_ge");
  ids.int_.eq = addString("int_eq");
  ids.int_.ne = addString("int_ne");

  ids.int_reif.lin_eq = addString("int_lin_eq_reif");
  ids.int_reif.lin_le = addString("int_lin_le_reif");
  ids.int_reif.lin_ne = addString("int_lin_ne_reif");
  ids.int_reif.plus = addString("int_plus_reif");
  ids.int_reif.minus = addString("int_minus_reif");
  ids.int_reif.times = addString("int_times_reif");
  ids.int_reif.div = addString("int_div_reif");
  ids.int_reif.mod = addString("int_mod_reif");
  ids.int_reif.lt = addString("int_lt_reif");
  ids.int_reif.le = addString("int_le_reif");
  ids.int_reif.gt = addString("int_gt_reif");
  ids.int_reif.ge = addString("int_ge_reif");
  ids.int_reif.eq = addString("int_eq_reif");
  ids.int_reif.ne = addString("int_ne_reif");

  ids.float_.lin_eq = addString("float_lin_eq");
  ids.float_.lin_le = addString("float_lin_le");
  ids.float_.lin_lt = addString("float_lin_lt");
  ids.float_.lin_ne = addString("float_lin_ne");
  ids.float_.plus = addString("float_plus");
  ids.float_.minus = addString("float_minus");
  ids.float_.times = addString("float_times");
  ids.float_.div = addString("float_div");
  ids.float_.mod = addString("float_mod");
  ids.float_.lt = addString("float_lt");
  ids.float_.le = addString("float_le");
  ids.float_.gt = addString("float_gt");
  ids.float_.ge = addString("float_ge");
  ids.float_.eq = addString("float_eq");
  ids.float_.ne = addString("float_ne");
  ids.float_.in = addString("float_in");
  ids.float_.dom = addString("float_dom");

  ids.float_reif.lin_eq = addString("float_lin_eq_reif");
  ids.float_reif.lin_le = addString("float_lin_le_reif");
  ids.float_reif.lin_lt = addString("float_lin_lt_reif");
  ids.float_reif.lin_ne = addString("float_lin_ne_reif");
  ids.float_reif.plus = addString("float_plus_reif");
  ids.float_reif.minus = addString("float_minus_reif");
  ids.float_reif.times = addString("float_times_reif");
  ids.float_reif.div = addString("float_div_reif");
  ids.float_reif.mod = addString("float_mod_reif");
  ids.float_reif.lt = addString("float_lt_reif");
  ids.float_reif.le = addString("float_le_reif");
  ids.float_reif.gt = addString("float_gt_reif");
  ids.float_reif.ge = addString("float_ge_reif");
  ids.float_reif.eq = addString("float_eq_reif");
  ids.float_reif.ne = addString("float_ne_reif");
  ids.float_reif.in = addString("float_in_reif");

  ids.bool_.and_ = addString("bool_and");
  ids.bool_.clause = addString("bool_clause");
  ids.bool_.eq = addString("bool_eq");
  ids.bool_.ge = addString("bool_ge");
  ids.bool_.gt = addString("bool_gt");
  ids.bool_.le = addString("bool_le");
  ids.bool_.lt = addString("bool_lt");
  ids.bool_.ne = addString("bool_xor");
  ids.bool_.not_ = addString("bool_not");
  ids.bool_.or_ = addString("bool_or");

  ids.bool_reif.clause = addString("bool_clause_reif");
  ids.bool_reif.eq = addString("bool_eq_reif");
  ids.bool_reif.array_and = addString("array_bool_and");

  ids.array_bool_and_imp = addString("array_bool_and_imp");

  ids.set_.card = addString("set_card");
  ids.set_.diff = addString("set_diff");
  ids.set_.eq = addString("set_eq");
  ids.set_.ge = addString("set_ge");
  ids.set_.gt = addString("set_gt");
  ids.set_.le = addString("set_le");
  ids.set_.lt = addString("set_lt");
  ids.set_.in = addString("set_in");
  ids.set_.intersect = addString("set_intersect");
  ids.set_.ne = addString("set_ne");
  ids.set_.subset = addString("set_subset");
  ids.set_.superset = addString("set_superset");
  ids.set_.symdiff = addString("set_symdiff");
  ids.set_.union_ = addString("set_union");

  ids.on_restart.sol = addString("sol");
  ids.on_restart.last_val = addString("last_val");
  ids.on_restart.on_restart = addString("on_restart");
  ids.on_restart.uniform_on_restart = addString("uniform_on_restart");

  ids.pow = addString("pow");
  ids.mzn_set_in_internal = addString("mzn_set_in_internal");
  ids.introduced_var = addString("__INTRODUCED");
  ids.anonEnumFromStrings = addString("anon_enum");
  ids.unnamedArgument = addString("<unnamed argument>");

  ctx.root = addId("ctx_root");
  ctx.root->type(Type::ann());
  ctx.pos = addId("ctx_pos");
  ctx.pos->type(Type::ann());
  ctx.neg = addId("ctx_neg");
  ctx.neg->type(Type::ann());
  ctx.mix = addId("ctx_mix");
  ctx.mix->type(Type::ann());

  ann.empty_annotation = addId("empty_annotation");
  ann.empty_annotation->type(Type::ann());
  ann.output_var = addId("output_var");

  ctx.promise_monotone = addId("promise_ctx_monotone");
  ctx.promise_monotone->type(Type::ann());
  ctx.promise_antitone = addId("promise_ctx_antitone");
  ctx.promise_antitone->type(Type::ann());

  ann.output_var->type(Type::ann());
  ann.output_only = addId("output_only");
  ann.output_only->type(Type::ann());
  ann.output_array = addString("output_array");
  ann.add_to_output = addId("add_to_output");
  ann.add_to_output->type(Type::ann());
  ann.output = addId("output");
  ann.output->type(Type::ann());
  ann.no_output = addId("no_output");
  ann.no_output->type(Type::ann());
  ann.mzn_check_var = addId("mzn_check_var");
  ann.mzn_check_var->type(Type::ann());
  ann.mzn_check_enum_var = addString("mzn_check_enum_var");
  ann.is_defined_var = addId("is_defined_var");
  ann.is_defined_var->type(Type::ann());
  ann.defines_var = addString("defines_var");
  ann.is_reverse_map = addId("is_reverse_map");
  ann.is_reverse_map->type(Type::ann());
  ann.promise_total = addId("promise_total");
  ann.promise_total->type(Type::ann());
  ann.maybe_partial = addId("maybe_partial");
  ann.maybe_partial->type(Type::ann());
  ann.doc_comment = addString("doc_comment");
  ann.mzn_path = addString("mzn_path");
  ann.is_introduced = addString("is_introduced");
#ifndef NDEBUG
  ann.mzn_break_here = addId("mzn_break_here");
  ann.mzn_break_here->type(Type::ann());
#endif
  ann.rhs_from_assignment = addId("mzn_rhs_from_assignment");
  ann.rhs_from_assignment->type(Type::ann());
  ann.domain_change_constraint = addId("domain_change_constraint");
  ann.domain_change_constraint->type(Type::ann());
  ann.mzn_deprecated = addString("mzn_deprecated");
  ann.mzn_was_undefined = addId("mzn_was_undefined");
  ann.mzn_was_undefined->type(Type::ann());
  ann.array_check_form = addId("array_check_form");
  ann.array_check_form->type(Type::ann());
  ann.annotated_expression = addId("annotated_expression");
  ann.annotated_expression->type(Type::ann());
  ann.mzn_add_annotated_expression = addString("mzn_add_annotated_expression");
  ann.expression_name_dbg = addString("expression_name_dbg");
  ann.cache_result = addId("cache_result");
  ann.cache_result->type(Type::ann());
  ann.no_cse = addId("no_cse");
  ann.no_cse->type(Type::ann());
  ann.mzn_internal_representation = addId("mzn_internal_representation");
  ann.mzn_internal_representation->type(Type::ann());
  ann.flatzinc_builtin = addId("flatzinc_builtin");
  ann.flatzinc_builtin->type(Type::ann());
  ann.mzn_evaluate_once = addId("mzn_evaluate_once");
  ann.mzn_evaluate_once->type(Type::ann());
  ann.promise_commutative = addId("promise_commutative");
  ann.promise_commutative->type(Type::ann());
  ann.seq_search = addString("seq_search");
  ann.seq_search_internal = addString("mzn_internal_seq_search");
  ann.int_search = addString("int_search");
  ann.int_search_internal = addString("mzn_internal_int_search");
  ann.bool_search = addString("bool_search");
  ann.bool_search_internal = addString("mzn_internal_bool_search");
  ann.float_search = addString("float_search");
  ann.float_search_internal = addString("mzn_internal_float_search");
  ann.set_search = addString("set_search");
  ann.set_search_internal = addString("mzn_internal_set_search");
  ann.warm_start = addString("warm_start");
  ann.warm_start_internal = addString("mzn_internal_warm_start");
  ann.warm_start_array = addString("warm_start_array");
  ann.warm_start_array_internal = addString("mzn_internal_warm_start_array");
  ann.computed_domain = addId("computed_domain");
  ann.computed_domain->type(Type::ann());

  cli.cmdlineData_short_str = addString("-D");
  cli.cmdlineData_str = addString("--cmdline-data");
  cli.datafile_str = addString("--data");
  cli.datafile_short_str = addString("-d");
  cli.globalsDir_str = addString("--globals-dir");
  cli.globalsDir_alt_str = addString("--mzn-globals-dir");
  cli.globalsDir_short_str = addString("-G");
  cli.help_str = addString("--help");
  cli.help_short_str = addString("-h");
  cli.ignoreStdlib_str = addString("--ignore-stdlib");
  cli.include_str = addString("-I");
  cli.inputFromStdin_str = addString("--input-from-stdin");
  cli.instanceCheckOnly_str = addString("--instance-check-only");
  cli.newfzn_str = addString("--newfzn");
  cli.no_optimize_str = addString("--no-optimize");
  cli.no_optimize_alt_str = addString("--no-optimise");
  cli.no_outputOzn_str = addString("--no-output-ozn");
  cli.no_outputOzn_short_str = addString("-O-");
  cli.no_typecheck_str = addString("--no-typecheck");
  cli.outputBase_str = addString("--output-base");
  cli.outputFznToStdout_str = addString("--output-to-stdout");
  cli.outputFznToStdout_alt_str = addString("--output-fzn-to-stdout");
  cli.outputOznToFile_str = addString("--output-ozn-to-file");
  cli.outputOznToStdout_str = addString("--output-ozn-to-stdout");
  cli.outputFznToFile_alt_str = addString("--output-fzn-to-file");
  cli.outputFznToFile_short_str = addString("-o");
  cli.outputFznToFile_str = addString("--output-to-file");
  cli.rangeDomainsOnly_str = addString("--only-range-domains");
  cli.statistics_str = addString("--statistics");
  cli.statistics_short_str = addString("-s");
  cli.stdlib_str = addString("--stdlib-dir");
  cli.verbose_str = addString("--verbose");
  cli.verbose_short_str = addString("-v");
  cli.version_str = addString("--version");
  cli.werror_str = addString("-Werror");

  cli.solver.all_sols_str = addString("-a");
  cli.solver.fzn_solver_str = addString("--solver");

  opts.cmdlineData = addString("cmdlineData");
  opts.datafile = addString("datafile");
  opts.datafiles = addString("datafiles");
  opts.fznToFile = addString("fznToFile");
  opts.fznToStdout = addString("fznToStdout");
  opts.globalsDir = addString("globalsDir");
  opts.ignoreStdlib = addString("ignoreStdlib");
  opts.includeDir = addString("includeDir");
  opts.includePaths = addString("includePaths");
  opts.inputFromStdin = addString("inputStdin");
  opts.instanceCheckOnly = addString("instanceCheckOnly");
  opts.model = addString("model");
  opts.newfzn = addString("newfzn");
  opts.noOznOutput = addString("noOznOutput");
  opts.optimize = addString("optimize");
  opts.outputBase = addString("outputBase");
  opts.oznToFile = addString("oznToFile");
  opts.oznToStdout = addString("oznToStdout");
  opts.rangeDomainsOnly = addString("rangeDomainsOnly");
  opts.statistics = addString("statistics");
  opts.stdlib = addString("stdlib");
  opts.typecheck = addString("typecheck");
  opts.verbose = addString("verbose");
  opts.werror = addString("werror");

  opts.solver.allSols = addString("allSols");
  opts.solver.numSols = addString("numSols");
  opts.solver.threads = addString("threads");
  opts.solver.fzn_solver = addString("fznsolver");
  opts.solver.fzn_flags = addString("fzn_flags");
  opts.solver.fzn_flag = addString("fzn_flag");
  opts.solver.fzn_time_limit_ms = addString("fzn_time_limit_ms");
  opts.solver.fzn_sigint = addString("fzn_sigint");

  cli_cat.general = addString("General Options");
  cli_cat.io = addString("Input/Output Options");
  cli_cat.solver = addString("Solver Options");
  cli_cat.translation = addString("Translation Options");
};

bool Constants::isCallByReferenceId(const ASTString& cid) const {
  return (cid == ids.assert || cid == ids.assert_dbg || cid == ids.trace || cid == ids.trace_dbg ||
          cid == "trace_exp" || cid == ids.mzn_symmetry_breaking_constraint ||
          cid == ids.mzn_redundant_constraint || cid == ids.mzn_default ||
          cid == ids.mzn_deprecate || cid == ids.output_to_section ||
          cid == ids.output_to_json_section || cid == ids.output);
}

void Constants::mark() {
  Expression::mark(emptyBoolArray);
  Expression::mark(literalTrue);
  Expression::mark(varTrue);
  Expression::mark(literalFalse);
  Expression::mark(varFalse);
  Expression::mark(varIgnore);
  Item::mark(varRedef);
  Expression::mark(absent);
  Expression::mark(infinityInt);
  Expression::mark(infinityFloat);

  for (auto* ident : _ids) {
    Expression::mark(ident);
  }
  for (auto& s : _strings) {
    s.mark();
  }
}

ASTString Constants::addString(const std::string& s) {
  ASTString as(s);
  _strings.push_back(as);
  return as;
}

Id* Constants::addId(const std::string& s) {
  Id* ident = new Id(Location(), ASTString(s), nullptr);
  _ids.push_back(ident);
  return ident;
}

const int Constants::max_array_size;

Constants& Constants::constants() {
  static Constants _c;
  return _c;
}

Annotation::~Annotation() { delete _s; }

bool Annotation::contains(Expression* e) const { return (_s != nullptr) && _s->contains(e); }

bool Annotation::isEmpty() const { return _s == nullptr || _s->isEmpty(); }

unsigned int Annotation::size() const { return _s == nullptr ? 0 : _s->size(); }

ExpressionSetIter Annotation::begin() const {
  return _s == nullptr ? ExpressionSetIter(true) : _s->begin();
}

ExpressionSetIter Annotation::end() const {
  return _s == nullptr ? ExpressionSetIter(true) : _s->end();
}

void Annotation::add(Expression* e) {
  if (_s == nullptr) {
    _s = new ExpressionSet;
  }
  if (e != nullptr && !Expression::equal(e, Constants::constants().ann.empty_annotation)) {
    _s->insert(e);
  }
}

void Annotation::add(std::vector<Expression*> e) {
  if (_s == nullptr) {
    _s = new ExpressionSet;
  }
  for (auto i = static_cast<unsigned int>(e.size()); (i--) != 0U;) {
    if (e[i] != nullptr && !Expression::equal(e[i], Constants::constants().ann.empty_annotation)) {
      _s->insert(e[i]);
    }
  }
}

void Annotation::remove(Expression* e) {
  if ((_s != nullptr) && (e != nullptr)) {
    _s->remove(e);
  }
}

void Annotation::removeCall(const ASTString& id) {
  if (_s == nullptr) {
    return;
  }
  std::vector<Expression*> toRemove;
  for (ExpressionSetIter it = _s->begin(); it != _s->end(); ++it) {
    if (Call* c = Expression::dynamicCast<Call>(*it)) {
      if (c->id() == id) {
        toRemove.push_back(*it);
      }
    }
  }
  for (auto i = static_cast<unsigned int>(toRemove.size()); (i--) != 0U;) {
    _s->remove(toRemove[i]);
  }
}

Call* Annotation::getCall(const ASTString& id) const {
  if (_s == nullptr) {
    return nullptr;
  }
  for (ExpressionSetIter it = _s->begin(); it != _s->end(); ++it) {
    if (Call* c = Expression::dynamicCast<Call>(*it)) {
      if (c->id() == id) {
        return c;
      }
    }
  }
  return nullptr;
}

bool Annotation::containsCall(const MiniZinc::ASTString& id) const {
  if (_s == nullptr) {
    return false;
  }
  for (ExpressionSetIter it = _s->begin(); it != _s->end(); ++it) {
    if (Call* c = Expression::dynamicCast<Call>(*it)) {
      if (c->id() == id) {
        return true;
      }
    }
  }
  return false;
}

void Annotation::clear() {
  if (_s != nullptr) {
    _s->clear();
  }
}

void Annotation::merge(const Annotation& ann) {
  if (ann._s == nullptr) {
    return;
  }
  if (_s == nullptr) {
    _s = new ExpressionSet;
  }
  for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
    _s->insert(*it);
  }
}

Expression* get_annotation(const Annotation& ann, const std::string& str) {
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if ((Expression::isa<Id>(e) && Expression::cast<Id>(e)->str() == str) ||
        (Expression::isa<Call>(e) && Expression::cast<Call>(e)->id() == str)) {
      return e;
    }
  }
  return nullptr;
}
Expression* get_annotation(const Annotation& ann, const ASTString& str) {
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if ((Expression::isa<Id>(e) && Expression::cast<Id>(e)->str() == str) ||
        (Expression::isa<Call>(e) && Expression::cast<Call>(e)->id() == str)) {
      return e;
    }
  }
  return nullptr;
}
}  // namespace MiniZinc
