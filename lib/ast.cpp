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

#include <limits>

namespace MiniZinc {

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

void Expression::addAnnotation(Expression* ann) {
  if (!isUnboxedVal() && this != Constants::constants().literalTrue &&
      this != Constants::constants().literalFalse &&
      !Expression::equal(ann, Constants::constants().ann.empty_annotation)) {
    _ann.add(ann);
  }
}
void Expression::addAnnotations(const std::vector<Expression*>& ann) {
  if (!isUnboxedVal() && this != Constants::constants().literalTrue &&
      this != Constants::constants().literalFalse) {
    for (auto* a : ann) {
      if (a != nullptr && !Expression::equal(a, Constants::constants().ann.empty_annotation)) {
        _ann.add(a);
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
  if (e == nullptr || e->isUnboxedVal()) {
    return;
  }
  std::vector<const Expression*> stack;
  stack.reserve(1000);
  stack.push_back(e);
  while (!stack.empty()) {
    const Expression* cur = stack.back();
    stack.pop_back();
    if (!cur->isUnboxedVal() && cur->_gcMark == 0U) {
      cur->_gcMark = 1U;
      cur->loc().mark();
      pushann(cur->ann());
      switch (cur->eid()) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_ANON:
          break;
        case Expression::E_SETLIT:
          if (cur->cast<SetLit>()->isv() != nullptr) {
            cur->cast<SetLit>()->isv()->mark();
          } else if (cur->cast<SetLit>()->fsv() != nullptr) {
            cur->cast<SetLit>()->fsv()->mark();
          } else {
            pushall(cur->cast<SetLit>()->v());
          }
          break;
        case Expression::E_STRINGLIT:
          cur->cast<StringLit>()->v().mark();
          break;
        case Expression::E_ID:
          if (cur->cast<Id>()->idn() == -1) {
            cur->cast<Id>()->v().mark();
          }
          pushstack(cur->cast<Id>()->decl());
          break;
        case Expression::E_ARRAYLIT:
          if (cur->_flag2) {
            pushstack(cur->cast<ArrayLit>()->_u.al);
          } else {
            pushall(ASTExprVec<Expression>(cur->cast<ArrayLit>()->_u.v));
          }
          cur->cast<ArrayLit>()->_dims.mark();
          break;
        case Expression::E_ARRAYACCESS:
          pushstack(cur->cast<ArrayAccess>()->v());
          pushall(cur->cast<ArrayAccess>()->idx());
          break;
        case Expression::E_COMP:
          pushstack(cur->cast<Comprehension>()->_e);
          pushall(cur->cast<Comprehension>()->_g);
          cur->cast<Comprehension>()->_gIndex.mark();
          break;
        case Expression::E_ITE:
          pushstack(cur->cast<ITE>()->elseExpr());
          pushall(cur->cast<ITE>()->_eIfThen);
          break;
        case Expression::E_BINOP:
          pushstack(cur->cast<BinOp>()->lhs());
          pushstack(cur->cast<BinOp>()->rhs());
          break;
        case Expression::E_UNOP:
          pushstack(cur->cast<UnOp>()->e());
          break;
        case Expression::E_CALL:
          cur->cast<Call>()->id().mark();
          for (unsigned int i = cur->cast<Call>()->argCount(); (i--) != 0U;) {
            pushstack(cur->cast<Call>()->arg(i));
          }
          if (!cur->cast<Call>()->_u.oneArg->isUnboxedVal() &&
              !cur->cast<Call>()->_u.oneArg->isTagged()) {
            cur->cast<Call>()->_u.args->mark();
          }
          if (FunctionI* fi = cur->cast<Call>()->decl()) {
            Item::mark(fi);
          }
          break;
        case Expression::E_VARDECL:
          pushstack(cur->cast<VarDecl>()->ti());
          pushstack(cur->cast<VarDecl>()->e());
          pushstack(cur->cast<VarDecl>()->id());
          break;
        case Expression::E_LET:
          pushall(cur->cast<Let>()->let());
          pushall(cur->cast<Let>()->_letOrig);
          pushstack(cur->cast<Let>()->in());
          break;
        case Expression::E_TI:
          pushstack(cur->cast<TypeInst>()->domain());
          pushall(cur->cast<TypeInst>()->ranges());
          break;
        case Expression::E_TIID:
          cur->cast<TIId>()->v().mark();
          break;
      }
    }
  }
}
#undef pushstack
#undef pushall

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
  return oss.str();
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
  for (int i = 1; i < dims(); i++) {
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
    return i <= off ? (*_u.v)[0] : (*_u.v)[i - off];
  }
  assert(_flag2);
  return (*_u.al)[origIdx(i)];
}

void ArrayLit::setSlice(unsigned int i, Expression* e) {
  if (!_flag2) {
    assert(_u.v->flag());
    int off = static_cast<int>(length()) - static_cast<int>(_u.v->size());
    if (i <= off) {
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
    : Expression(loc, E_ARRAYLIT, Type()) {
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
    if (!e->isa<IntLit>() && !e->isa<FloatLit>() && !e->isa<BoolLit>() &&
        !(e->isa<SetLit>() && e->cast<SetLit>()->evaluated()) &&
        !(e->isa<Id>() && e->cast<Id>()->decl() != nullptr &&
          e->cast<Id>()->decl()->flat() == e->cast<Id>()->decl())) {
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
    : Expression(loc, E_ARRAYLIT, Type()) {
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

Generator::Generator(const std::vector<ASTString>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  Location loc = in == nullptr ? where->loc() : in->loc();
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
    auto* nvd = new VarDecl(i->loc(), new TypeInst(i->loc(), Type::parint()), i->v());
    nvd->toplevel(false);
    vd.push_back(nvd);
  }
  _v = vd;
  _in = in;
  _where = where;
}
Generator::Generator(const std::vector<std::string>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  Location loc = in == nullptr ? where->loc() : in->loc();
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
  return _g[_gIndex[gen] + 2 + i]->cast<VarDecl>();
}
const VarDecl* Comprehension::decl(unsigned int gen, unsigned int i) const {
  return _g[_gIndex[gen] + 2 + i]->cast<VarDecl>();
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
  _id = Call::eid;
  _flag1 = true;
  Call* c = cast<Call>();
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

void VarDecl::trail() {
  GC::trail(&_e, e());
  if (!_ti->ranges().empty()) {
    GC::trail(reinterpret_cast<Expression**>(&_ti), _ti);
  }
}

void VarDecl::rehash() {
  initHash();
  combineHash(Expression::hash(_ti));
  combineHash(_id->hash());
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
    : Expression(loc, E_LET, Type()) {
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
    if (auto* vd = _let[i]->dynamicCast<VarDecl>()) {
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
  if (ranges.size() == 1 && (ranges[0] != nullptr) && ranges[0]->isa<TypeInst>() &&
      (ranges[0]->cast<TypeInst>()->domain() != nullptr) &&
      ranges[0]->cast<TypeInst>()->domain()->isa<TIId>() &&
      !ranges[0]->cast<TypeInst>()->domain()->cast<TIId>()->v().beginsWith("$")) {
    _type.dim(-1);
  } else {
    _type.dim(static_cast<int>(ranges.size()));
  }
  rehash();
}

bool TypeInst::hasTiVariable() const {
  if ((domain() != nullptr) && domain()->isa<TIId>()) {
    return true;
  }
  for (unsigned int i = _ranges.size(); (i--) != 0U;) {
    if (_ranges[i]->domain() != nullptr && _ranges[i]->domain()->isa<TIId>()) {
      return true;
    }
  }
  return false;
}

namespace {
Type get_type(Expression* e) { return e->type(); }
Type get_type(const Type& t) { return t; }
const Location& get_loc(Expression* e, FunctionI* /*fi*/) { return e->loc(); }
const Location& get_loc(const Type& /*t*/, FunctionI* fi) { return fi->loc(); }

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
Type return_type(EnvI& env, FunctionI* fi, const std::vector<T>& ta, bool strictEnum) {
  if (fi->id() == env.constants.varRedef->id()) {
    return Type::varbool();
  }
  Type ret = fi->ti()->type();
  ASTString dh;
  if (fi->ti()->domain() && fi->ti()->domain()->isa<TIId>()) {
    dh = fi->ti()->domain()->cast<TIId>()->v();
  }
  ASTString rh;
  if (fi->ti()->ranges().size() == 1 && isa_tiid(fi->ti()->ranges()[0]->domain())) {
    rh = fi->ti()->ranges()[0]->domain()->cast<TIId>()->v();
  }

  ASTStringMap<std::pair<Type, bool>> tmap;
  for (unsigned int i = 0; i < ta.size(); i++) {
    TypeInst* tii = fi->param(i)->ti();
    if (tii->domain() && tii->domain()->isa<TIId>()) {
      ASTString tiid = tii->domain()->cast<TIId>()->v();
      Type tiit = get_type(ta[i]);
      bool isEnumTIID = isa_enum_tiid(tii->domain());
      if (tii->type().any()) {
        tiit.any(true);
      }
      if (tiit.enumId() != 0 && tiit.dim() > 0) {
        const std::vector<unsigned int>& enumIds = env.getArrayEnum(tiit.enumId());
        tiit.enumId(enumIds[enumIds.size() - 1]);
      }
      tiit.dim(0);
      auto it = tmap.find(tiid);
      if (it == tmap.end()) {
        tmap.insert(std::pair<ASTString, std::pair<Type, bool>>(tiid, {tiit, isEnumTIID}));
      } else {
        // We've seen this identifier before, unify the types
        if (it->second.first.dim() > 0) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " used in both array and non-array position";
          throw TypeError(env, get_loc(ta[i], fi), ss.str());
        }
        Type tiit_par = tiit;
        tiit_par.any(false);
        tiit_par.ti(Type::TI_PAR);
        tiit_par.ot(Type::OT_PRESENT);
        if (isEnumTIID) {
          tiit_par.st(Type::ST_SET);
        }
        Type its_par = it->second.first;
        its_par.any(false);
        its_par.ti(Type::TI_PAR);
        its_par.ot(Type::OT_PRESENT);
        if (it->second.second) {
          its_par.st(Type::ST_SET);
        }
        if (tiit_par.bt() == Type::BT_TOP || tiit_par.bt() == Type::BT_BOT) {
          tiit_par.bt(its_par.bt());
        }
        if (its_par.bt() == Type::BT_TOP || its_par.bt() == Type::BT_BOT) {
          its_par.bt(tiit_par.bt());
        }
        if (env.isSubtype(tiit_par, its_par, strictEnum)) {
          if (it->second.first.bt() == Type::BT_TOP) {
            it->second.first.bt(tiit.bt());
          }
        } else if (env.isSubtype(its_par, tiit_par, strictEnum)) {
          it->second.first.bt(tiit_par.bt());
          it->second.first.enumId(tiit_par.enumId());
        } else {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " instantiated with different types ("
             << tiit.toString(env) << " vs " << it->second.first.toString(env) << ")";
          throw TypeError(env, get_loc(ta[i], fi), ss.str());
        }
        if (tiit.isvar()) {
          it->second.first.ti(Type::TI_VAR);
        }
        if (tiit.isOpt()) {
          it->second.first.ot(Type::OT_OPTIONAL);
        }
      }
    }
    if (tii->ranges().size() == 1 && isa_tiid(tii->ranges()[0]->domain())) {
      ASTString tiid = tii->ranges()[0]->domain()->cast<TIId>()->v();
      Type orig_tiit = get_type(ta[i]);
      if (orig_tiit.dim() == 0) {
        std::ostringstream ss;
        ss << "type-inst variable $" << tiid << " must be an array index";
        throw TypeError(env, get_loc(ta[i], fi), ss.str());
      }
      Type tiit = Type::top(orig_tiit.dim());
      if (orig_tiit.enumId() != 0) {
        std::vector<unsigned int> enumIds(tiit.dim() + 1);
        const std::vector<unsigned int>& orig_enumIds = env.getArrayEnum(orig_tiit.enumId());
        for (unsigned int i = 0; i < enumIds.size() - 1; i++) {
          enumIds[i] = orig_enumIds[i];
        }
        enumIds[enumIds.size() - 1] = 0;
        tiit.enumId(env.registerArrayEnum(enumIds));
      }
      auto it = tmap.find(tiid);
      if (it == tmap.end()) {
        tmap.insert(std::pair<ASTString, std::pair<Type, bool>>(tiid, {tiit, false}));
      } else {
        if (it->second.first.dim() == 0) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " used in both array and non-array position";
          throw TypeError(env, get_loc(ta[i], fi), ss.str());
        }
        if (it->second.first != tiit) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " instantiated with different types ("
             << tiit.toString(env) + " vs " << it->second.first.toString(env) << ")";
          throw TypeError(env, get_loc(ta[i], fi), ss.str());
        }
      }
    } else if (!tii->ranges().empty()) {
      for (unsigned int j = 0; j < tii->ranges().size(); j++) {
        if (isa_enum_tiid(tii->ranges()[j]->domain())) {
          ASTString enumTIId = tii->ranges()[j]->domain()->cast<TIId>()->v();
          Type tiit = get_type(ta[i]);
          Type enumIdT;
          if (tiit.enumId() != 0) {
            unsigned int enumId = env.getArrayEnum(tiit.enumId())[j];
            enumIdT = Type::parsetenum(enumId);
          } else {
            enumIdT = Type::parsetint();
          }
          auto it = tmap.find(enumTIId);
          // TODO: this may clash if the same enum TIId is used for different types
          // but the same enum
          if (it == tmap.end()) {
            tmap.insert(std::pair<ASTString, std::pair<Type, bool>>(enumTIId, {enumIdT, true}));
          } else if (strictEnum && it->second.first.enumId() != enumIdT.enumId()) {
            std::ostringstream ss;
            ss << "type-inst variable $" << enumTIId << " used for different enum types";
            throw TypeError(env, get_loc(ta[i], fi), ss.str());
          }
        }
      }
    }
  }
  if (!dh.empty()) {
    auto it = tmap.find(dh);
    if (it == tmap.end()) {
      std::ostringstream ss;
      ss << "type-inst variable $" << dh << " used but not defined";
      throw TypeError(env, fi->loc(), ss.str());
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
        ret.any(false);
      }
    }
    if (!fi->ti()->ranges().empty() && it->second.first.enumId() != 0) {
      std::vector<unsigned int> enumIds(fi->ti()->ranges().size() + 1);
      for (unsigned int i = 0; i < fi->ti()->ranges().size(); i++) {
        enumIds[i] = 0;
      }
      enumIds[enumIds.size() - 1] = it->second.first.enumId();
      ret.enumId(env.registerArrayEnum(enumIds));
    } else {
      ret.enumId(it->second.first.enumId());
    }
  }
  if (!rh.empty()) {
    auto it = tmap.find(rh);
    if (it == tmap.end()) {
      std::ostringstream ss;
      ss << "type-inst variable $" << rh << " used but not defined";
      throw TypeError(env, fi->loc(), ss.str());
    }
    ret.dim(it->second.first.dim());
    if (it->second.first.enumId() != 0) {
      std::vector<unsigned int> enumIds(it->second.first.dim() + 1);
      const std::vector<unsigned int>& orig_enumIds = env.getArrayEnum(it->second.first.enumId());
      for (unsigned int i = 0; i < enumIds.size() - 1; i++) {
        enumIds[i] = orig_enumIds[i];
      }
      unsigned int curEnumId = ret.enumId();
      if (curEnumId != 0 && ret.dim() > 0) {
        const auto& curIds = env.getArrayEnum(curEnumId);
        curEnumId = curIds[curIds.size() - 1];
      }
      enumIds[enumIds.size() - 1] = curEnumId;
      ret.enumId(env.registerArrayEnum(enumIds));
    }
  } else if (!fi->ti()->ranges().empty()) {
    std::vector<unsigned int> enumIds(fi->ti()->ranges().size() + 1);
    bool hadRealEnum = false;
    if (ret.enumId() == 0) {
      enumIds[enumIds.size() - 1] = 0;
    } else {
      enumIds = env.getArrayEnum(ret.enumId());
      hadRealEnum = true;
    }

    for (unsigned int i = 0; i < fi->ti()->ranges().size(); i++) {
      if (isa_enum_tiid(fi->ti()->ranges()[i]->domain())) {
        ASTString enumTIId = fi->ti()->ranges()[i]->domain()->cast<TIId>()->v();
        auto it = tmap.find(enumTIId);
        if (it == tmap.end()) {
          std::ostringstream ss;
          ss << "type-inst variable $" << enumTIId << " used but not defined";
          throw TypeError(env, fi->loc(), ss.str());
        }
        enumIds[i] = it->second.first.enumId();
        hadRealEnum |= (enumIds[i] != 0);
      }
    }
    if (hadRealEnum) {
      ret.enumId(env.registerArrayEnum(enumIds));
    }
  }
  return ret;
}
}  // namespace

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

Type FunctionI::rtype(EnvI& env, const std::vector<Expression*>& ta, bool strictEnums) {
  return return_type(env, this, ta, strictEnums);
}

Type FunctionI::rtype(EnvI& env, const std::vector<Type>& ta, bool strictEnums) {
  return return_type(env, this, ta, strictEnums);
}

Type FunctionI::argtype(EnvI& env, const std::vector<Expression*>& ta, unsigned int n) const {
  // Given the concrete types for all function arguments ta, compute the
  // least common supertype that fits function parameter n.
  TypeInst* tii = param(n)->ti();
  Type curTiiT = tii->type();
  if (curTiiT.dim() == -1) {
    if (ta[n]->type().dim() == 0) {
      curTiiT.dim(1);
    } else {
      curTiiT.dim(ta[n]->type().dim());
    }
  }
  if ((tii->domain() != nullptr) && tii->domain()->isa<TIId>()) {
    // We need to determine both the base type and whether this tiid
    // can stand for a set. It can only stand for a set if none
    // of the uses of tiid is opt. The base type has to be int
    // if any of the uses are var set.

    Type ty = ta[n]->type();
    ty.st(curTiiT.st());
    ty.dim(curTiiT.dim());
    ASTString tv = tii->domain()->cast<TIId>()->v();
    for (unsigned int i = 0; i < paramCount(); i++) {
      if ((param(i)->ti()->domain() != nullptr) && param(i)->ti()->domain()->isa<TIId>() &&
          param(i)->ti()->domain()->cast<TIId>()->v() == tv) {
        Type toCheck = ta[i]->type();
        toCheck.ot(curTiiT.ot());
        toCheck.st(curTiiT.st());
        toCheck.dim(curTiiT.dim());
        if (toCheck != ty) {
          if (env.isSubtype(ty, toCheck, true)) {
            ty = toCheck;
          } else {
            Type ty_par = ty;
            ty_par.ti(Type::TI_PAR);
            Type toCheck_par = toCheck;
            toCheck_par.ti(Type::TI_PAR);
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
  switch (e0->eid()) {
    case Expression::E_INTLIT:
      return e0->cast<IntLit>()->v() == e1->cast<IntLit>()->v();
    case Expression::E_FLOATLIT:
      return e0->cast<FloatLit>()->v() == e1->cast<FloatLit>()->v();
    case Expression::E_SETLIT: {
      const auto* s0 = e0->cast<SetLit>();
      const auto* s1 = e1->cast<SetLit>();
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
      return e0->cast<BoolLit>()->v() == e1->cast<BoolLit>()->v();
    case Expression::E_STRINGLIT:
      return e0->cast<StringLit>()->v() == e1->cast<StringLit>()->v();
    case Expression::E_ID: {
      const Id* id0 = e0->cast<Id>();
      const Id* id1 = e1->cast<Id>();
      if (id0->decl() == nullptr || id1->decl() == nullptr) {
        return id0->v() == id1->v() && id0->idn() == id1->idn();
      }
      return id0->decl() == id1->decl() ||
             (id0->decl()->flat() != nullptr && id0->decl()->flat() == id1->decl()->flat());
    }
    case Expression::E_ANON:
      return false;
    case Expression::E_ARRAYLIT: {
      const auto* a0 = e0->cast<ArrayLit>();
      const auto* a1 = e1->cast<ArrayLit>();
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
      const auto* a0 = e0->cast<ArrayAccess>();
      const auto* a1 = e1->cast<ArrayAccess>();
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
    case Expression::E_COMP: {
      const auto* c0 = e0->cast<Comprehension>();
      const auto* c1 = e1->cast<Comprehension>();
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
      const ITE* i0 = e0->cast<ITE>();
      const ITE* i1 = e1->cast<ITE>();
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
      const auto* b0 = e0->cast<BinOp>();
      const auto* b1 = e1->cast<BinOp>();
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
      const UnOp* b0 = e0->cast<UnOp>();
      const UnOp* b1 = e1->cast<UnOp>();
      if (b0->op() != b1->op()) {
        return false;
      }
      if (!Expression::equal(b0->e(), b1->e())) {
        return false;
      }
      return true;
    }
    case Expression::E_CALL: {
      const Call* c0 = e0->cast<Call>();
      const Call* c1 = e1->cast<Call>();
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
      const auto* v0 = e0->cast<VarDecl>();
      const auto* v1 = e1->cast<VarDecl>();
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
      const Let* l0 = e0->cast<Let>();
      const Let* l1 = e1->cast<Let>();
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
      const auto* t0 = e0->cast<TypeInst>();
      const auto* t1 = e1->cast<TypeInst>();
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
  absent->type(absent_t);

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
  ids.assert = addString("assert");
  ids.assert_dbg = addString("assert_dbg");

  ids.deopt = addString("deopt");
  ids.absent = addString("absent");
  ids.occurs = addString("occurs");
  ids.card = addString("card");
  ids.abs = addString("abs");

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
  ids.mzn_default = addString("default");
  ids.trace = addString("trace");
  ids.trace_dbg = addString("trace_dbg");
  ids.trace_to_section = addString("trace_to_section");

  ids.array2d = addString("array2d");
  ids.array3d = addString("array3d");
  ids.array4d = addString("array4d");
  ids.array5d = addString("array5d");
  ids.array6d = addString("array6d");
  ids.arrayXd = addString("arrayXd");

  ids.sum = addString("sum");
  ids.lin_exp = addString("lin_exp");
  ids.count = addString("count");
  ids.element = addString("element");
  ids.anon_enum = addString("anon_enum");
  ids.anon_enum_set = addString("anon_enum_set");
  ids.enumFromConstructors = addString("enumFromConstructors");
  ids.enumOf = addString("enum_of");
  ids.enumOfInternal = addString("enum_of_internal");

  ids.show = addString("show");
  ids.format = addString("format");
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

  ids.array_bool_or = addString("array_bool_or");
  ids.array_bool_and = addString("array_bool_and");

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

  ids.pow = addString("pow");
  ids.mzn_set_in_internal = addString("mzn_set_in_internal");
  ids.introduced_var = addString("__INTRODUCED");
  ids.anonEnumFromStrings = addString("anon_enum");

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
          cid == ids.mzn_deprecate || cid == ids.output_to_section || cid == ids.output);
}

void Constants::mark() {
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
    if (Call* c = (*it)->dynamicCast<Call>()) {
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
    if (Call* c = (*it)->dynamicCast<Call>()) {
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
    if (Call* c = (*it)->dynamicCast<Call>()) {
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
    if ((e->isa<Id>() && e->cast<Id>()->str() == str) ||
        (e->isa<Call>() && e->cast<Call>()->id() == str)) {
      return e;
    }
  }
  return nullptr;
}
Expression* get_annotation(const Annotation& ann, const ASTString& str) {
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if ((e->isa<Id>() && e->cast<Id>()->str() == str) ||
        (e->isa<Call>() && e->cast<Call>()->id() == str)) {
      return e;
    }
  }
  return nullptr;
}
}  // namespace MiniZinc
