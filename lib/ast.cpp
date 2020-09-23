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
  static const unsigned int pointerBits = sizeof(IntLit*) * 8;
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
      combined |= (static_cast<unsigned long long int>(last_line - first_line)) << 20;
      combined |= (static_cast<unsigned long long int>(first_column)) << (20 + 20);
      combined |= (static_cast<unsigned long long int>(last_column)) << (20 + 20 + 10);
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

Location::LocVec::LocVec(const ASTString& filename, unsigned int fl, unsigned int first_column,
                         unsigned int last_line, unsigned int last_column)
    : ASTVec(5) {
  *(_data + 0) = filename.aststr();
  *(_data + 1) = IntLit::a(fl);
  *(_data + 2) = IntLit::a(last_line);
  *(_data + 3) = IntLit::a(first_column);
  *(_data + 4) = IntLit::a(last_column);
}

Location Location::nonalloc;

Type Type::unboxedint = Type::parint();
Type Type::unboxedfloat = Type::parfloat();

Annotation Annotation::empty;

std::string Location::toString(void) const {
  std::ostringstream oss;
  oss << filename() << ":" << first_line() << "." << first_column();
  return oss.str();
}

void Location::mark(void) const {
  if (lv()) lv()->mark();
}

Location Location::introduce() const {
  Location l = *this;
  if (l._loc_info.lv) {
    l._loc_info.t |= 1;
  }
  return l;
}

void Expression::addAnnotation(Expression* ann) {
  if (!isUnboxedVal()) _ann.add(ann);
}
void Expression::addAnnotations(std::vector<Expression*> ann) {
  if (!isUnboxedVal())
    for (unsigned int i = 0; i < ann.size(); i++)
      if (ann[i]) _ann.add(ann[i]);
}

#define pushstack(e)      \
  do {                    \
    if (e != NULL) {      \
      stack.push_back(e); \
    }                     \
  } while (0)
#define pushall(v)                              \
  do {                                          \
    v.mark();                                   \
    for (unsigned int i = 0; i < v.size(); i++) \
      if (v[i] != NULL) {                       \
        stack.push_back(v[i]);                  \
      }                                         \
  } while (0)
#define pushann(a)                                                \
  do {                                                            \
    for (ExpressionSetIter it = a.begin(); it != a.end(); ++it) { \
      pushstack(*it);                                             \
    }                                                             \
  } while (0)
void Expression::mark(Expression* e) {
  if (e == nullptr || e->isUnboxedVal()) return;
  std::vector<const Expression*> stack;
  stack.reserve(1000);
  stack.push_back(e);
  while (!stack.empty()) {
    const Expression* cur = stack.back();
    stack.pop_back();
    if (!cur->isUnboxedVal() && cur->_gc_mark == 0) {
      cur->_gc_mark = 1;
      cur->loc().mark();
      pushann(cur->ann());
      switch (cur->eid()) {
        case Expression::E_INTLIT:
        case Expression::E_FLOATLIT:
        case Expression::E_BOOLLIT:
        case Expression::E_ANON:
          break;
        case Expression::E_SETLIT:
          if (cur->cast<SetLit>()->isv()) {
            cur->cast<SetLit>()->isv()->mark();
          } else if (cur->cast<SetLit>()->fsv()) {
            cur->cast<SetLit>()->fsv()->mark();
          } else {
            pushall(cur->cast<SetLit>()->v());
          }
          break;
        case Expression::E_STRINGLIT:
          cur->cast<StringLit>()->v().mark();
          break;
        case Expression::E_ID:
          if (cur->cast<Id>()->idn() == -1) cur->cast<Id>()->v().mark();
          pushstack(cur->cast<Id>()->decl());
          break;
        case Expression::E_ARRAYLIT:
          if (cur->_flag_2) {
            pushstack(cur->cast<ArrayLit>()->_u._al);
          } else {
            pushall(ASTExprVec<Expression>(cur->cast<ArrayLit>()->_u._v));
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
          cur->cast<Comprehension>()->_g_idx.mark();
          break;
        case Expression::E_ITE:
          pushstack(cur->cast<ITE>()->e_else());
          pushall(cur->cast<ITE>()->_e_if_then);
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
          for (unsigned int i = cur->cast<Call>()->n_args(); i--;)
            pushstack(cur->cast<Call>()->arg(i));
          if (!cur->cast<Call>()->_u._oneArg->isUnboxedVal() &&
              !cur->cast<Call>()->_u._oneArg->isTagged())
            cur->cast<Call>()->_u._args->mark();
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
          pushall(cur->cast<Let>()->_let_orig);
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

void IntLit::rehash(void) {
  init_hash();
  std::hash<IntVal> h;
  cmb_hash(h(_v));
}

void FloatLit::rehash(void) {
  init_hash();
  std::hash<FloatVal> h;
  cmb_hash(h(_v));
}

void SetLit::rehash(void) {
  init_hash();
  if (isv()) {
    std::hash<IntVal> h;
    for (IntSetRanges r0(isv()); r0(); ++r0) {
      cmb_hash(h(r0.min()));
      cmb_hash(h(r0.max()));
    }
  } else if (fsv()) {
    std::hash<FloatVal> h;
    for (FloatSetRanges r0(fsv()); r0(); ++r0) {
      cmb_hash(h(r0.min()));
      cmb_hash(h(r0.max()));
    }
  } else {
    for (unsigned int i = v().size(); i--;) cmb_hash(Expression::hash(_v[i]));
  }
}

void BoolLit::rehash(void) {
  init_hash();
  std::hash<bool> h;
  cmb_hash(h(_v));
}

void StringLit::rehash(void) {
  init_hash();
  cmb_hash(_v.hash());
}

void Id::rehash(void) {
  init_hash();
  std::hash<long long int> h;
  if (idn() == -1)
    cmb_hash(v().hash());
  else
    cmb_hash(h(idn()));
}

int Id::levenshteinDistance(Id* other) const {
  if (idn() != -1 || other->idn() != -1) {
    return std::numeric_limits<int>::max();
  } else {
    return v().levenshteinDistance(other->v());
  }
}

ASTString Id::str() const {
  if (idn() == -1) return v();
  std::ostringstream oss;
  oss << "X_INTRODUCED_" << idn() << "_";
  return oss.str();
}

void TIId::rehash(void) {
  init_hash();
  cmb_hash(_v.hash());
}

void AnonVar::rehash(void) { init_hash(); }

int ArrayLit::dims(void) const {
  return _flag_2 ? ((_dims.size() - 2 * _u._al->dims()) / 2)
                 : (_dims.size() == 0 ? 1 : _dims.size() / 2);
}
int ArrayLit::min(int i) const {
  if (_dims.size() == 0) {
    assert(i == 0);
    return 1;
  }
  return _dims[2 * i];
}
int ArrayLit::max(int i) const {
  if (_dims.size() == 0) {
    assert(i == 0);
    return _u._v->size();
  }
  return _dims[2 * i + 1];
}
int ArrayLit::length(void) const {
  if (dims() == 0) return 0;
  int l = max(0) - min(0) + 1;
  for (int i = 1; i < dims(); i++) l *= (max(i) - min(i) + 1);
  return l;
}
void ArrayLit::make1d(void) {
  if (_dims.size() != 0) {
    GCLock lock;
    if (_flag_2) {
      std::vector<int> d(2 + _u._al->dims() * 2);
      int dimOffset = dims() * 2;
      d[0] = 1;
      d[1] = length();
      for (unsigned int i = 2; i < d.size(); i++) {
        d[i] = _dims[dimOffset + i];
      }
      _dims = ASTIntVec(d);
    } else {
      std::vector<int> d(2);
      d[0] = 1;
      d[1] = length();
      _dims = ASTIntVec(d);
    }
  }
}

int ArrayLit::origIdx(int i) const {
  assert(_flag_2);
  int curIdx = i;
  int multiplyer = 1;
  int oIdx = 0;
  int sliceOffset = dims() * 2;
  for (int curDim = _u._al->dims() - 1; curDim >= 0; curDim--) {
    oIdx +=
        multiplyer *
        ((curIdx % (_dims[sliceOffset + curDim * 2 + 1] - _dims[sliceOffset + curDim * 2] + 1)) +
         (_dims[sliceOffset + curDim * 2] - _u._al->min(curDim)));
    curIdx = curIdx / (_dims[sliceOffset + curDim * 2 + 1] - _dims[sliceOffset + curDim * 2] + 1);
    multiplyer *= (_u._al->max(curDim) - _u._al->min(curDim) + 1);
  }
  return oIdx;
}

Expression* ArrayLit::slice_get(int i) const {
  if (!_flag_2) {
    assert(_u._v->flag());
    int off = length() - _u._v->size();
    return i <= off ? (*_u._v)[0] : (*_u._v)[i - off];
  } else {
    assert(_flag_2);
    return (*_u._al)[origIdx(i)];
  }
}

void ArrayLit::slice_set(int i, Expression* e) {
  if (!_flag_2) {
    assert(_u._v->flag());
    int off = length() - _u._v->size();
    if (i <= off) {
      (*_u._v)[0] = e;
    } else {
      (*_u._v)[i - off] = e;
    }
  } else {
    assert(_flag_2);
    _u._al->set(origIdx(i), e);
  }
}

ArrayLit::ArrayLit(const Location& loc, ArrayLit* v, const std::vector<std::pair<int, int> >& dims,
                   const std::vector<std::pair<int, int> >& slice)
    : Expression(loc, E_ARRAYLIT, Type()) {
  _flag_1 = false;
  _flag_2 = true;
  _u._al = v;
  assert(slice.size() == v->dims());
  std::vector<int> d(dims.size() * 2 + 2 * slice.size());
  for (auto i = static_cast<unsigned int>(dims.size()); i--;) {
    d[i * 2] = dims[i].first;
    d[i * 2 + 1] = dims[i].second;
  }
  int sliceOffset = static_cast<int>(2 * dims.size());
  for (auto i = static_cast<unsigned int>(slice.size()); i--;) {
    d[sliceOffset + i * 2] = slice[i].first;
    d[sliceOffset + i * 2 + 1] = slice[i].second;
  }
  _dims = ASTIntVec(d);
}

void ArrayLit::compress(const std::vector<Expression*>& v, const std::vector<int>& dims) {
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
    _u._v = ASTExprVec<Expression>(compress).vec();
    _u._v->flag(true);
    _dims = ASTIntVec(dims);
  } else {
    _u._v = ASTExprVec<Expression>(v).vec();
    if (dims.size() != 2 || dims[0] != 1) {
      // only allocate dims vector if it is not a 1d array indexed from 1
      _dims = ASTIntVec(dims);
    }
  }
}

ArrayLit::ArrayLit(const Location& loc, const std::vector<Expression*>& v,
                   const std::vector<std::pair<int, int> >& dims)
    : Expression(loc, E_ARRAYLIT, Type()) {
  _flag_1 = false;
  _flag_2 = false;
  std::vector<int> d(dims.size() * 2);
  for (auto i = static_cast<unsigned int>(dims.size()); i--;) {
    d[i * 2] = dims[i].first;
    d[i * 2 + 1] = dims[i].second;
  }
  compress(v, d);
  rehash();
}

void ArrayLit::rehash(void) {
  init_hash();
  std::hash<int> h;
  for (unsigned int i = 0; i < _dims.size(); i++) {
    cmb_hash(h(_dims[i]));
  }
  if (_flag_2) {
    cmb_hash(Expression::hash(_u._al));
  } else {
    for (unsigned int i = _u._v->size(); i--;) {
      cmb_hash(h(i));
      cmb_hash(Expression::hash((*_u._v)[i]));
    }
  }
}

void ArrayAccess::rehash(void) {
  init_hash();
  cmb_hash(Expression::hash(_v));
  std::hash<unsigned int> h;
  cmb_hash(h(_idx.size()));
  for (unsigned int i = _idx.size(); i--;) cmb_hash(Expression::hash(_idx[i]));
}

Generator::Generator(const std::vector<ASTString>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  Location loc = in == nullptr ? where->loc() : in->loc();
  for (unsigned int i = 0; i < v.size(); i++) {
    auto* nvd = new VarDecl(loc, new TypeInst(loc, Type::parint()), v[i]);
    nvd->toplevel(false);
    vd.push_back(nvd);
  }
  _v = vd;
  _in = in;
  _where = where;
}
Generator::Generator(const std::vector<Id*>& v, Expression* in, Expression* where) {
  std::vector<VarDecl*> vd;
  for (unsigned int i = 0; i < v.size(); i++) {
    auto* nvd = new VarDecl(v[i]->loc(), new TypeInst(v[i]->loc(), Type::parint()), v[i]->v());
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
  for (unsigned int i = 0; i < v.size(); i++) {
    auto* nvd = new VarDecl(loc, new TypeInst(loc, Type::parint()), ASTString(v[i]));
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

bool Comprehension::set(void) const { return _flag_1; }
void Comprehension::rehash(void) {
  init_hash();
  std::hash<unsigned int> h;
  cmb_hash(h(set()));
  cmb_hash(Expression::hash(_e));
  cmb_hash(h(_g_idx.size()));
  for (unsigned int i = _g_idx.size(); i--;) {
    cmb_hash(h(_g_idx[i]));
  }
  cmb_hash(h(_g.size()));
  for (unsigned int i = _g.size(); i--;) {
    cmb_hash(Expression::hash(_g[i]));
  }
}

int Comprehension::n_generators(void) const { return _g_idx.size() - 1; }
Expression* Comprehension::in(int i) { return _g[_g_idx[i]]; }
const Expression* Comprehension::in(int i) const { return _g[_g_idx[i]]; }
const Expression* Comprehension::where(int i) const { return _g[_g_idx[i] + 1]; }
Expression* Comprehension::where(int i) { return _g[_g_idx[i] + 1]; }

int Comprehension::n_decls(int i) const { return _g_idx[i + 1] - _g_idx[i] - 2; }
VarDecl* Comprehension::decl(int gen, int i) { return _g[_g_idx[gen] + 2 + i]->cast<VarDecl>(); }
const VarDecl* Comprehension::decl(int gen, int i) const {
  return _g[_g_idx[gen] + 2 + i]->cast<VarDecl>();
}

bool Comprehension::containsBoundVariable(Expression* e) {
  std::unordered_set<VarDecl*> decls;
  for (unsigned int i = 0; i < n_generators(); i++) {
    for (unsigned int j = 0; j < n_decls(i); j++) {
      decls.insert(decl(i, j));
    }
  }
  class FindVar : public EVisitor {
    std::unordered_set<VarDecl*>& _decls;
    bool _found;

  public:
    FindVar(std::unordered_set<VarDecl*>& decls) : _decls(decls), _found(false) {}
    bool enter(Expression*) { return !_found; }
    void vId(Id& ident) {
      if (_decls.find(ident.decl()) != _decls.end()) {
        _found = true;
      }
    }
    bool found(void) const { return _found; }
  } _fv(decls);
  topDown(_fv, e);
  return _fv.found();
}

void ITE::rehash(void) {
  init_hash();
  std::hash<unsigned int> h;
  cmb_hash(h(_e_if_then.size()));
  for (unsigned int i = _e_if_then.size(); i--;) {
    cmb_hash(Expression::hash(_e_if_then[i]));
  }
  cmb_hash(Expression::hash(e_else()));
}

BinOpType BinOp::op(void) const { return static_cast<BinOpType>(_sec_id); }
void BinOp::rehash(void) {
  init_hash();
  std::hash<int> h;
  cmb_hash(h(static_cast<int>(op())));
  cmb_hash(Expression::hash(_e0));
  cmb_hash(Expression::hash(_e1));
}

Call* BinOp::morph(const ASTString& ident, const std::vector<Expression*>& args) {
  _id = Call::eid;
  _flag_1 = true;
  Call* c = cast<Call>();
  c->id(ident);
  c->args(args);
  return c;
}

namespace {

class OpToString : public GCMarker {
public:
  Id* sBOT_PLUS;
  Id* sBOT_MINUS;
  Id* sBOT_MULT;
  Id* sBOT_DIV;
  Id* sBOT_IDIV;
  Id* sBOT_MOD;
  Id* sBOT_POW;
  Id* sBOT_LE;
  Id* sBOT_LQ;
  Id* sBOT_GR;
  Id* sBOT_GQ;
  Id* sBOT_EQ;
  Id* sBOT_NQ;
  Id* sBOT_IN;
  Id* sBOT_SUBSET;
  Id* sBOT_SUPERSET;
  Id* sBOT_UNION;
  Id* sBOT_DIFF;
  Id* sBOT_SYMDIFF;
  Id* sBOT_INTERSECT;
  Id* sBOT_PLUSPLUS;
  Id* sBOT_EQUIV;
  Id* sBOT_IMPL;
  Id* sBOT_RIMPL;
  Id* sBOT_OR;
  Id* sBOT_AND;
  Id* sBOT_XOR;
  Id* sBOT_DOTDOT;
  Id* sBOT_NOT;

  OpToString(void) {
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

  static OpToString& o(void) {
    static OpToString _o;
    return _o;
  }

  void mark(MINIZINC_GC_STAT_ARGS) override {
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

ASTString BinOp::opToString(void) const {
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

UnOpType UnOp::op(void) const { return static_cast<UnOpType>(_sec_id); }
void UnOp::rehash(void) {
  init_hash();
  std::hash<int> h;
  cmb_hash(h(static_cast<int>(_sec_id)));
  cmb_hash(Expression::hash(_e0));
}

ASTString UnOp::opToString(void) const {
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

void Call::rehash(void) {
  init_hash();
  cmb_hash(id().hash());
  std::hash<FunctionI*> hf;
  cmb_hash(hf(decl()));
  std::hash<unsigned int> hu;
  cmb_hash(hu(n_args()));
  for (unsigned int i = 0; i < n_args(); i++) cmb_hash(Expression::hash(arg(i)));
}

void VarDecl::trail(void) {
  GC::trail(&_e, e());
  if (_ti->ranges().size() > 0) {
    GC::trail(reinterpret_cast<Expression**>(&_ti), _ti);
  }
}

void VarDecl::rehash(void) {
  init_hash();
  cmb_hash(Expression::hash(_ti));
  cmb_hash(_id->hash());
  cmb_hash(Expression::hash(_e));
}

void Let::rehash(void) {
  init_hash();
  cmb_hash(Expression::hash(_in));
  std::hash<unsigned int> h;
  cmb_hash(h(_let.size()));
  for (unsigned int i = _let.size(); i--;) cmb_hash(Expression::hash(_let[i]));
}

Let::Let(const Location& loc, const std::vector<Expression*>& let, Expression* in)
    : Expression(loc, E_LET, Type()) {
  _let = ASTExprVec<Expression>(let);
  std::vector<Expression*> vde;
  for (unsigned int i = 0; i < let.size(); i++) {
    if (auto* vd = Expression::dyn_cast<VarDecl>(let[i])) {
      vde.push_back(vd->e());
      for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
        vde.push_back(vd->ti()->ranges()[i]->domain());
      }
    }
  }
  _let_orig = ASTExprVec<Expression>(vde);
  _in = in;
  rehash();
}

void Let::pushbindings(void) {
  GC::mark();
  for (unsigned int i = 0, j = 0; i < _let.size(); i++) {
    if (auto* vd = _let[i]->dyn_cast<VarDecl>()) {
      vd->trail();
      vd->e(_let_orig[j++]);
      for (unsigned int k = 0; k < vd->ti()->ranges().size(); k++) {
        vd->ti()->ranges()[k]->domain(_let_orig[j++]);
      }
    }
  }
}
void Let::popbindings(void) {
  for (unsigned int i = 0; i < _let.size(); i++) {
    if (auto* vd = _let[i]->dyn_cast<VarDecl>()) {
      GC::untrail();
      break;
    }
  }
}

void TypeInst::rehash(void) {
  init_hash();
  std::hash<unsigned int> h;
  unsigned int rsize = _ranges.size();
  cmb_hash(h(rsize));
  for (unsigned int i = rsize; i--;) cmb_hash(Expression::hash(_ranges[i]));
  cmb_hash(Expression::hash(domain()));
}

void TypeInst::setRanges(const std::vector<TypeInst*>& ranges) {
  _ranges = ASTExprVec<TypeInst>(ranges);
  if (ranges.size() == 1 && ranges[0] && ranges[0]->isa<TypeInst>() &&
      ranges[0]->cast<TypeInst>()->domain() && ranges[0]->cast<TypeInst>()->domain()->isa<TIId>() &&
      !ranges[0]->cast<TypeInst>()->domain()->cast<TIId>()->v().beginsWith("$"))
    _type.dim(-1);
  else
    _type.dim(static_cast<int>(ranges.size()));
  rehash();
}

bool TypeInst::hasTiVariable(void) const {
  if (domain() && domain()->isa<TIId>()) return true;
  for (unsigned int i = _ranges.size(); i--;)
    if (_ranges[i]->isa<TIId>()) return true;
  return false;
}

namespace {
Type getType(Expression* e) { return e->type(); }
Type getType(const Type& t) { return t; }
const Location& getLoc(Expression* e, FunctionI*) { return e->loc(); }
const Location& getLoc(const Type&, FunctionI* fi) { return fi->loc(); }

bool isaTIId(Expression* e) {
  if (TIId* t = Expression::dyn_cast<TIId>(e)) {
    return !t->v().beginsWith("$");
  }
  return false;
}
bool isaEnumTIId(Expression* e) {
  if (TIId* t = Expression::dyn_cast<TIId>(e)) {
    return t->v().beginsWith("$");
  }
  return false;
}

template <class T>
Type return_type(EnvI& env, FunctionI* fi, const std::vector<T>& ta, bool strictEnum) {
  if (fi->id() == constants().var_redef->id()) return Type::varbool();
  Type ret = fi->ti()->type();
  ASTString dh;
  if (fi->ti()->domain() && fi->ti()->domain()->isa<TIId>())
    dh = fi->ti()->domain()->cast<TIId>()->v();
  ASTString rh;
  if (fi->ti()->ranges().size() == 1 && isaTIId(fi->ti()->ranges()[0]->domain()))
    rh = fi->ti()->ranges()[0]->domain()->cast<TIId>()->v();

  ASTStringMap<Type> tmap;
  for (unsigned int i = 0; i < ta.size(); i++) {
    TypeInst* tii = fi->params()[i]->ti();
    if (tii->domain() && tii->domain()->isa<TIId>()) {
      ASTString tiid = tii->domain()->cast<TIId>()->v();
      Type tiit = getType(ta[i]);
      if (tiit.enumId() != 0 && tiit.dim() > 0) {
        const std::vector<unsigned int>& enumIds = env.getArrayEnum(tiit.enumId());
        tiit.enumId(enumIds[enumIds.size() - 1]);
      }
      tiit.dim(0);
      if (tii->type().st() == Type::ST_SET) {
        tiit.st(Type::ST_PLAIN);
      }
      if (isaEnumTIId(tii->domain())) {
        tiit.st(Type::ST_SET);
      }
      auto it = tmap.find(tiid);
      if (it == tmap.end()) {
        tmap.insert(std::pair<ASTString, Type>(tiid, tiit));
      } else {
        if (it->second.dim() > 0) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " used in both array and non-array position";
          throw TypeError(env, getLoc(ta[i], fi), ss.str());
        } else {
          Type tiit_par = tiit;
          tiit_par.ti(Type::TI_PAR);
          tiit_par.ot(Type::OT_PRESENT);
          Type its_par = it->second;
          its_par.ti(Type::TI_PAR);
          its_par.ot(Type::OT_PRESENT);
          if (tiit_par.bt() == Type::BT_TOP || tiit_par.bt() == Type::BT_BOT) {
            tiit_par.bt(its_par.bt());
          }
          if (its_par.bt() == Type::BT_TOP || its_par.bt() == Type::BT_BOT) {
            its_par.bt(tiit_par.bt());
          }
          if (env.isSubtype(tiit_par, its_par, strictEnum)) {
            if (it->second.bt() == Type::BT_TOP) it->second.bt(tiit.bt());
          } else if (env.isSubtype(its_par, tiit_par, strictEnum)) {
            it->second = tiit_par;
          } else {
            std::ostringstream ss;
            ss << "type-inst variable $" << tiid << " instantiated with different types ("
               << tiit.toString(env) << " vs " << it->second.toString(env) << ")";
            throw TypeError(env, getLoc(ta[i], fi), ss.str());
          }
        }
      }
    }
    if (tii->ranges().size() == 1 && isaTIId(tii->ranges()[0]->domain())) {
      ASTString tiid = tii->ranges()[0]->domain()->cast<TIId>()->v();
      Type orig_tiit = getType(ta[i]);
      if (orig_tiit.dim() == 0) {
        std::ostringstream ss;
        ss << "type-inst variable $" << tiid << " must be an array index";
        throw TypeError(env, getLoc(ta[i], fi), ss.str());
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
        tmap.insert(std::pair<ASTString, Type>(tiid, tiit));
      } else {
        if (it->second.dim() == 0) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " used in both array and non-array position";
          throw TypeError(env, getLoc(ta[i], fi), ss.str());
        } else if (it->second != tiit) {
          std::ostringstream ss;
          ss << "type-inst variable $" << tiid << " instantiated with different types ("
             << tiit.toString(env) + " vs " << it->second.toString(env) << ")";
          throw TypeError(env, getLoc(ta[i], fi), ss.str());
        }
      }
    } else if (tii->ranges().size() > 0) {
      for (unsigned int j = 0; j < tii->ranges().size(); j++) {
        if (isaEnumTIId(tii->ranges()[j]->domain())) {
          ASTString enumTIId = tii->ranges()[j]->domain()->cast<TIId>()->v();
          Type tiit = getType(ta[i]);
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
            tmap.insert(std::pair<ASTString, Type>(enumTIId, enumIdT));
          } else {
            if (it->second.enumId() != enumIdT.enumId()) {
              std::ostringstream ss;
              ss << "type-inst variable $" << enumTIId << " used for different enum types";
              throw TypeError(env, getLoc(ta[i], fi), ss.str());
            }
          }
        }
      }
    }
  }
  if (dh.size() != 0) {
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
      ret.bt(it->second.bt());
      if (ret.st() == Type::ST_PLAIN) ret.st(it->second.st());
    }
    if (fi->ti()->ranges().size() > 0 && it->second.enumId() != 0) {
      std::vector<unsigned int> enumIds(fi->ti()->ranges().size() + 1);
      for (unsigned int i = 0; i < fi->ti()->ranges().size(); i++) {
        enumIds[i] = 0;
      }
      enumIds[enumIds.size() - 1] = it->second.enumId();
      ret.enumId(env.registerArrayEnum(enumIds));
    } else {
      ret.enumId(it->second.enumId());
    }
  }
  if (rh.size() != 0) {
    auto it = tmap.find(rh);
    if (it == tmap.end()) {
      std::ostringstream ss;
      ss << "type-inst variable $" << rh << " used but not defined";
      throw TypeError(env, fi->loc(), ss.str());
    }
    ret.dim(it->second.dim());
    if (it->second.enumId() != 0) {
      std::vector<unsigned int> enumIds(it->second.dim() + 1);
      const std::vector<unsigned int>& orig_enumIds = env.getArrayEnum(it->second.enumId());
      for (unsigned int i = 0; i < enumIds.size() - 1; i++) {
        enumIds[i] = orig_enumIds[i];
      }
      enumIds[enumIds.size() - 1] =
          ret.enumId() == 0 ? 0 : env.getArrayEnum(ret.enumId())[enumIds.size() - 1];
      ret.enumId(env.registerArrayEnum(enumIds));
    }

  } else if (fi->ti()->ranges().size() > 0) {
    std::vector<unsigned int> enumIds(fi->ti()->ranges().size() + 1);
    bool hadRealEnum = false;
    if (ret.enumId() == 0) {
      enumIds[enumIds.size() - 1] = 0;
    } else {
      enumIds[enumIds.size() - 1] = env.getArrayEnum(ret.enumId())[enumIds.size() - 1];
      hadRealEnum = true;
    }

    for (unsigned int i = 0; i < fi->ti()->ranges().size(); i++) {
      if (isaEnumTIId(fi->ti()->ranges()[i]->domain())) {
        ASTString enumTIId = fi->ti()->ranges()[i]->domain()->cast<TIId>()->v();
        auto it = tmap.find(enumTIId);
        if (it == tmap.end()) {
          std::ostringstream ss;
          ss << "type-inst variable $" << enumTIId << " used but not defined";
          throw TypeError(env, fi->loc(), ss.str());
        }
        enumIds[i] = it->second.enumId();
        hadRealEnum |= (enumIds[i] != 0);
      } else {
        enumIds[i] = 0;
      }
    }
    if (hadRealEnum) ret.enumId(env.registerArrayEnum(enumIds));
  }
  return ret;
}
}  // namespace

#if defined(MINIZINC_GC_STATS)
void Item::mark(Item* item, MINIZINC_GC_STAT_ARGS) {
#else
void Item::mark(Item* item) {
#endif
  if (item->has_mark()) {
    return;
  }
  item->_gc_mark = 1;
  item->loc().mark();
  switch (item->iid()) {
    case Item::II_INC:
      item->cast<IncludeI>()->f().mark();
      break;
    case Item::II_VD:
      Expression::mark(item->cast<VarDeclI>()->e());
#if defined(MINIZINC_GC_STATS)
      gc_stats[item->cast<VarDeclI>()->e()->Expression::eid()].inmodel++;
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
      gc_stats[item->cast<ConstraintI>()->e()->Expression::eid()].inmodel++;
#endif
      break;
    case Item::II_SOL: {
      auto* si = item->cast<SolveI>();
      for (ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
        Expression::mark(*it);
      }
    }
      Expression::mark(item->cast<SolveI>()->e());
      break;
    case Item::II_OUT:
      Expression::mark(item->cast<OutputI>()->e());
      break;
    case Item::II_FUN: {
      auto* fi = item->cast<FunctionI>();
      fi->id().mark();
      Expression::mark(fi->ti());
      for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
        Expression::mark(*it);
      }
      Expression::mark(fi->e());
      fi->params().mark();
      for (unsigned int k = 0; k < fi->params().size(); k++) {
        Expression::mark(fi->params()[k]);
      }
    } break;
  }
}

Type FunctionI::rtype(EnvI& env, const std::vector<Expression*>& ta, bool strictEnums) {
  return return_type(env, this, ta, strictEnums);
}

Type FunctionI::rtype(EnvI& env, const std::vector<Type>& ta, bool strictEnums) {
  return return_type(env, this, ta, strictEnums);
}

Type FunctionI::argtype(EnvI& env, const std::vector<Expression*>& ta, int n) {
  TypeInst* tii = params()[n]->ti();
  if (tii->domain() && tii->domain()->isa<TIId>()) {
    Type ty = ta[n]->type();
    ty.st(tii->type().st());
    ty.dim(tii->type().dim());
    ASTString tv = tii->domain()->cast<TIId>()->v();
    for (unsigned int i = 0; i < params().size(); i++) {
      if (params()[i]->ti()->domain() && params()[i]->ti()->domain()->isa<TIId>() &&
          params()[i]->ti()->domain()->cast<TIId>()->v() == tv) {
        Type toCheck = ta[i]->type();
        toCheck.st(tii->type().st());
        toCheck.dim(tii->type().dim());
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
  } else {
    return tii->type();
  }
}

bool Expression::equal_internal(const Expression* e0, const Expression* e1) {
  switch (e0->eid()) {
    case Expression::E_INTLIT:
      return e0->cast<IntLit>()->v() == e1->cast<IntLit>()->v();
    case Expression::E_FLOATLIT:
      return e0->cast<FloatLit>()->v() == e1->cast<FloatLit>()->v();
    case Expression::E_SETLIT: {
      const auto* s0 = e0->cast<SetLit>();
      const auto* s1 = e1->cast<SetLit>();
      if (s0->isv()) {
        if (s1->isv()) {
          IntSetRanges r0(s0->isv());
          IntSetRanges r1(s1->isv());
          return Ranges::equal(r0, r1);
        } else {
          return false;
        }
      } else if (s0->fsv()) {
        if (s1->fsv()) {
          FloatSetRanges r0(s0->fsv());
          FloatSetRanges r1(s1->fsv());
          return Ranges::equal(r0, r1);
        } else {
          return false;
        }
      } else {
        if (s1->isv() || s1->fsv()) return false;
        if (s0->v().size() != s1->v().size()) return false;
        for (unsigned int i = 0; i < s0->v().size(); i++)
          if (!Expression::equal(s0->v()[i], s1->v()[i])) return false;
        return true;
      }
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
      if (a0->size() != a1->size()) return false;
      if (a0->_dims.size() != a1->_dims.size()) return false;
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
      if (!Expression::equal(a0->v(), a1->v())) return false;
      if (a0->idx().size() != a1->idx().size()) return false;
      for (unsigned int i = 0; i < a0->idx().size(); i++)
        if (!Expression::equal(a0->idx()[i], a1->idx()[i])) return false;
      return true;
    }
    case Expression::E_COMP: {
      const auto* c0 = e0->cast<Comprehension>();
      const auto* c1 = e1->cast<Comprehension>();
      if (c0->set() != c1->set()) return false;
      if (!Expression::equal(c0->_e, c1->_e)) return false;
      if (c0->_g.size() != c1->_g.size()) return false;
      for (unsigned int i = 0; i < c0->_g.size(); i++) {
        if (!Expression::equal(c0->_g[i], c1->_g[i])) return false;
      }
      for (unsigned int i = 0; i < c0->_g_idx.size(); i++) {
        if (c0->_g_idx[i] != c1->_g_idx[i]) return false;
      }
      return true;
    }
    case Expression::E_ITE: {
      const ITE* i0 = e0->cast<ITE>();
      const ITE* i1 = e1->cast<ITE>();
      if (i0->_e_if_then.size() != i1->_e_if_then.size()) return false;
      for (unsigned int i = i0->_e_if_then.size(); i--;) {
        if (!Expression::equal(i0->_e_if_then[i], i1->_e_if_then[i])) return false;
      }
      if (!Expression::equal(i0->e_else(), i1->e_else())) return false;
      return true;
    }
    case Expression::E_BINOP: {
      const auto* b0 = e0->cast<BinOp>();
      const auto* b1 = e1->cast<BinOp>();
      if (b0->op() != b1->op()) return false;
      if (!Expression::equal(b0->lhs(), b1->lhs())) return false;
      if (!Expression::equal(b0->rhs(), b1->rhs())) return false;
      return true;
    }
    case Expression::E_UNOP: {
      const UnOp* b0 = e0->cast<UnOp>();
      const UnOp* b1 = e1->cast<UnOp>();
      if (b0->op() != b1->op()) return false;
      if (!Expression::equal(b0->e(), b1->e())) return false;
      return true;
    }
    case Expression::E_CALL: {
      const Call* c0 = e0->cast<Call>();
      const Call* c1 = e1->cast<Call>();
      if (c0->id() != c1->id()) return false;
      if (c0->decl() != c1->decl()) return false;
      if (c0->n_args() != c1->n_args()) return false;
      for (unsigned int i = 0; i < c0->n_args(); i++)
        if (!Expression::equal(c0->arg(i), c1->arg(i))) return false;
      return true;
    }
    case Expression::E_VARDECL: {
      const auto* v0 = e0->cast<VarDecl>();
      const auto* v1 = e1->cast<VarDecl>();
      if (!Expression::equal(v0->ti(), v1->ti())) return false;
      if (!Expression::equal(v0->id(), v1->id())) return false;
      if (!Expression::equal(v0->e(), v1->e())) return false;
      return true;
    }
    case Expression::E_LET: {
      const Let* l0 = e0->cast<Let>();
      const Let* l1 = e1->cast<Let>();
      if (!Expression::equal(l0->in(), l1->in())) return false;
      if (l0->let().size() != l1->let().size()) return false;
      for (unsigned int i = l0->let().size(); i--;)
        if (!Expression::equal(l0->let()[i], l1->let()[i])) return false;
      return true;
    }
    case Expression::E_TI: {
      const auto* t0 = e0->cast<TypeInst>();
      const auto* t1 = e1->cast<TypeInst>();
      if (t0->ranges().size() != t1->ranges().size()) return false;
      for (unsigned int i = t0->ranges().size(); i--;)
        if (!Expression::equal(t0->ranges()[i], t1->ranges()[i])) return false;
      if (!Expression::equal(t0->domain(), t1->domain())) return false;
      return true;
    }
    case Expression::E_TIID:
      return false;
    default:
      assert(false);
      return false;
  }
}

Constants::Constants(void) {
  GCLock lock;
  auto* ti = new TypeInst(Location(), Type::parbool());
  lit_true = new BoolLit(Location(), true);
  var_true = new VarDecl(Location(), ti, "_bool_true", lit_true);
  lit_false = new BoolLit(Location(), false);
  var_false = new VarDecl(Location(), ti, "_bool_false", lit_false);
  var_ignore = new VarDecl(Location(), ti, "_bool_ignore");
  absent = new Id(Location(), "_absent", nullptr);
  var_redef = new FunctionI(Location(), "__internal_var_redef",
                            new TypeInst(Location(), Type::varbool()), std::vector<VarDecl*>());
  Type absent_t;
  absent_t.bt(Type::BT_BOT);
  absent_t.dim(0);
  absent_t.st(Type::ST_PLAIN);
  absent_t.ot(Type::OT_OPTIONAL);
  absent->type(absent_t);

  IntSetVal* isv_infty = IntSetVal::a(-IntVal::infinity(), IntVal::infinity());
  infinity = new SetLit(Location(), isv_infty);

  ids.forall = ASTString("forall");
  ids.forall_reif = ASTString("forall_reif");
  ids.exists = ASTString("exists");
  ids.clause = ASTString("clause");
  ids.bool2int = ASTString("bool2int");
  ids.int2float = ASTString("int2float");
  ids.bool2float = ASTString("bool2float");
  ids.assert = ASTString("assert");
  ids.mzn_deprecate = ASTString("mzn_deprecate");
  ids.trace = ASTString("trace");

  ids.sum = ASTString("sum");
  ids.lin_exp = ASTString("lin_exp");
  ids.element = ASTString("element");

  ids.show = ASTString("show");
  ids.output = ASTString("output");
  ids.fix = ASTString("fix");

  ids.int_.lin_eq = ASTString("int_lin_eq");
  ids.int_.lin_le = ASTString("int_lin_le");
  ids.int_.lin_ne = ASTString("int_lin_ne");
  ids.int_.plus = ASTString("int_plus");
  ids.int_.minus = ASTString("int_minus");
  ids.int_.times = ASTString("int_times");
  ids.int_.div = ASTString("int_div");
  ids.int_.mod = ASTString("int_mod");
  ids.int_.lt = ASTString("int_lt");
  ids.int_.le = ASTString("int_le");
  ids.int_.gt = ASTString("int_gt");
  ids.int_.ge = ASTString("int_ge");
  ids.int_.eq = ASTString("int_eq");
  ids.int_.ne = ASTString("int_ne");

  ids.int_reif.lin_eq = ASTString("int_lin_eq_reif");
  ids.int_reif.lin_le = ASTString("int_lin_le_reif");
  ids.int_reif.lin_ne = ASTString("int_lin_ne_reif");
  ids.int_reif.plus = ASTString("int_plus_reif");
  ids.int_reif.minus = ASTString("int_minus_reif");
  ids.int_reif.times = ASTString("int_times_reif");
  ids.int_reif.div = ASTString("int_div_reif");
  ids.int_reif.mod = ASTString("int_mod_reif");
  ids.int_reif.lt = ASTString("int_lt_reif");
  ids.int_reif.le = ASTString("int_le_reif");
  ids.int_reif.gt = ASTString("int_gt_reif");
  ids.int_reif.ge = ASTString("int_ge_reif");
  ids.int_reif.eq = ASTString("int_eq_reif");
  ids.int_reif.ne = ASTString("int_ne_reif");

  ids.float_.lin_eq = ASTString("float_lin_eq");
  ids.float_.lin_le = ASTString("float_lin_le");
  ids.float_.lin_lt = ASTString("float_lin_lt");
  ids.float_.lin_ne = ASTString("float_lin_ne");
  ids.float_.plus = ASTString("float_plus");
  ids.float_.minus = ASTString("float_minus");
  ids.float_.times = ASTString("float_times");
  ids.float_.div = ASTString("float_div");
  ids.float_.mod = ASTString("float_mod");
  ids.float_.lt = ASTString("float_lt");
  ids.float_.le = ASTString("float_le");
  ids.float_.gt = ASTString("float_gt");
  ids.float_.ge = ASTString("float_ge");
  ids.float_.eq = ASTString("float_eq");
  ids.float_.ne = ASTString("float_ne");
  ids.float_.in = ASTString("float_in");
  ids.float_.dom = ASTString("float_dom");

  ids.float_reif.lin_eq = ASTString("float_lin_eq_reif");
  ids.float_reif.lin_le = ASTString("float_lin_le_reif");
  ids.float_reif.lin_lt = ASTString("float_lin_lt_reif");
  ids.float_reif.lin_ne = ASTString("float_lin_ne_reif");
  ids.float_reif.plus = ASTString("float_plus_reif");
  ids.float_reif.minus = ASTString("float_minus_reif");
  ids.float_reif.times = ASTString("float_times_reif");
  ids.float_reif.div = ASTString("float_div_reif");
  ids.float_reif.mod = ASTString("float_mod_reif");
  ids.float_reif.lt = ASTString("float_lt_reif");
  ids.float_reif.le = ASTString("float_le_reif");
  ids.float_reif.gt = ASTString("float_gt_reif");
  ids.float_reif.ge = ASTString("float_ge_reif");
  ids.float_reif.eq = ASTString("float_eq_reif");
  ids.float_reif.ne = ASTString("float_ne_reif");
  ids.float_reif.in = ASTString("float_in_reif");

  ids.bool_eq = ASTString("bool_eq");
  ids.bool_eq_reif = ASTString("bool_eq_reif");
  ids.bool_not = ASTString("bool_not");
  ids.bool_clause = ASTString("bool_clause");
  ids.bool_clause_reif = ASTString("bool_clause_reif");
  ids.bool_xor = ASTString("bool_xor");
  ids.array_bool_or = ASTString("array_bool_or");
  ids.array_bool_and = ASTString("array_bool_and");
  ids.set_eq = ASTString("set_eq");
  ids.set_in = ASTString("set_in");
  ids.set_subset = ASTString("set_subset");
  ids.set_card = ASTString("set_card");
  ids.pow = ASTString("pow");

  ids.introduced_var = ASTString("__INTRODUCED");
  ids.anonEnumFromStrings = ASTString("anon_enum");

  ctx.root = new Id(Location(), ASTString("ctx_root"), nullptr);
  ctx.root->type(Type::ann());
  ctx.pos = new Id(Location(), ASTString("ctx_pos"), nullptr);
  ctx.pos->type(Type::ann());
  ctx.neg = new Id(Location(), ASTString("ctx_neg"), nullptr);
  ctx.neg->type(Type::ann());
  ctx.mix = new Id(Location(), ASTString("ctx_mix"), nullptr);
  ctx.mix->type(Type::ann());

  ann.output_var = new Id(Location(), ASTString("output_var"), nullptr);
  ann.output_var->type(Type::ann());
  ann.output_only = new Id(Location(), ASTString("output_only"), nullptr);
  ann.output_only->type(Type::ann());
  ann.output_array = ASTString("output_array");
  ann.add_to_output = new Id(Location(), ASTString("add_to_output"), nullptr);
  ann.add_to_output->type(Type::ann());
  ann.mzn_check_var = new Id(Location(), ASTString("mzn_check_var"), nullptr);
  ann.mzn_check_var->type(Type::ann());
  ann.mzn_check_enum_var = ASTString("mzn_check_enum_var");
  ann.is_defined_var = new Id(Location(), ASTString("is_defined_var"), nullptr);
  ann.is_defined_var->type(Type::ann());
  ann.defines_var = ASTString("defines_var");
  ann.is_reverse_map = new Id(Location(), ASTString("is_reverse_map"), nullptr);
  ann.is_reverse_map->type(Type::ann());
  ann.promise_total = new Id(Location(), ASTString("promise_total"), nullptr);
  ann.promise_total->type(Type::ann());
  ann.maybe_partial = new Id(Location(), ASTString("maybe_partial"), nullptr);
  ann.maybe_partial->type(Type::ann());
  ann.doc_comment = ASTString("doc_comment");
  ann.mzn_path = ASTString("mzn_path");
  ann.is_introduced = ASTString("is_introduced");
  ann.user_cut = new Id(Location(), ASTString("user_cut"), nullptr);
  ann.user_cut->type(Type::ann());
  ann.lazy_constraint = new Id(Location(), ASTString("lazy_constraint"), nullptr);
  ann.lazy_constraint->type(Type::ann());
#ifndef NDEBUG
  ann.mzn_break_here = new Id(Location(), ASTString("mzn_break_here"), nullptr);
  ann.mzn_break_here->type(Type::ann());
#endif
  ann.rhs_from_assignment = new Id(Location(), ASTString("mzn_rhs_from_assignment"), nullptr);
  ann.rhs_from_assignment->type(Type::ann());
  ann.domain_change_constraint = new Id(Location(), ASTString("domain_change_constraint"), nullptr);
  ann.domain_change_constraint->type(Type::ann());
  ann.mzn_deprecated = ASTString("mzn_deprecated");
  ann.mzn_was_undefined = new Id(Location(), ASTString("mzn_was_undefined"), nullptr);
  ann.mzn_was_undefined->type(Type::ann());
  ann.array_check_form = new Id(Location(), ASTString("array_check_form"), nullptr);
  ann.array_check_form->type(Type::ann());

  cli.cmdlineData_short_str = ASTString("-D");
  cli.cmdlineData_str = ASTString("--cmdline-data");
  cli.datafile_str = ASTString("--data");
  cli.datafile_short_str = ASTString("-d");
  cli.globalsDir_str = ASTString("--globals-dir");
  cli.globalsDir_alt_str = ASTString("--mzn-globals-dir");
  cli.globalsDir_short_str = ASTString("-G");
  cli.help_str = ASTString("--help");
  cli.help_short_str = ASTString("-h");
  cli.ignoreStdlib_str = ASTString("--ignore-stdlib");
  cli.include_str = ASTString("-I");
  cli.inputFromStdin_str = ASTString("--input-from-stdin");
  cli.instanceCheckOnly_str = ASTString("--instance-check-only");
  cli.newfzn_str = ASTString("--newfzn");
  cli.no_optimize_str = ASTString("--no-optimize");
  cli.no_optimize_alt_str = ASTString("--no-optimise");
  cli.no_outputOzn_str = ASTString("--no-output-ozn");
  cli.no_outputOzn_short_str = ASTString("-O-");
  cli.no_typecheck_str = ASTString("--no-typecheck");
  cli.outputBase_str = ASTString("--output-base");
  cli.outputFznToStdout_str = ASTString("--output-to-stdout");
  cli.outputFznToStdout_alt_str = ASTString("--output-fzn-to-stdout");
  cli.outputOznToFile_str = ASTString("--output-ozn-to-file");
  cli.outputOznToStdout_str = ASTString("--output-ozn-to-stdout");
  cli.outputFznToFile_alt_str = ASTString("--output-fzn-to-file");
  cli.outputFznToFile_short_str = ASTString("-o");
  cli.outputFznToFile_str = ASTString("--output-to-file");
  cli.rangeDomainsOnly_str = ASTString("--only-range-domains");
  cli.statistics_str = ASTString("--statistics");
  cli.statistics_short_str = ASTString("-s");
  cli.stdlib_str = ASTString("--stdlib-dir");
  cli.verbose_str = ASTString("--verbose");
  cli.verbose_short_str = ASTString("-v");
  cli.version_str = ASTString("--version");
  cli.werror_str = ASTString("-Werror");

  cli.solver.all_sols_str = ASTString("-a");
  cli.solver.fzn_solver_str = ASTString("--solver");

  opts.cmdlineData = ASTString("cmdlineData");
  opts.datafile = ASTString("datafile");
  opts.datafiles = ASTString("datafiles");
  opts.fznToFile = ASTString("fznToFile");
  opts.fznToStdout = ASTString("fznToStdout");
  opts.globalsDir = ASTString("globalsDir");
  opts.ignoreStdlib = ASTString("ignoreStdlib");
  opts.includeDir = ASTString("includeDir");
  opts.includePaths = ASTString("includePaths");
  opts.inputFromStdin = ASTString("inputStdin");
  opts.instanceCheckOnly = ASTString("instanceCheckOnly");
  opts.model = ASTString("model");
  opts.newfzn = ASTString("newfzn");
  opts.noOznOutput = ASTString("noOznOutput");
  opts.optimize = ASTString("optimize");
  opts.outputBase = ASTString("outputBase");
  opts.oznToFile = ASTString("oznToFile");
  opts.oznToStdout = ASTString("oznToStdout");
  opts.rangeDomainsOnly = ASTString("rangeDomainsOnly");
  opts.statistics = ASTString("statistics");
  opts.stdlib = ASTString("stdlib");
  opts.typecheck = ASTString("typecheck");
  opts.verbose = ASTString("verbose");
  opts.werror = ASTString("werror");

  opts.solver.allSols = ASTString("allSols");
  opts.solver.numSols = ASTString("numSols");
  opts.solver.threads = ASTString("threads");
  opts.solver.fzn_solver = ASTString("fznsolver");
  opts.solver.fzn_flags = ASTString("fzn_flags");
  opts.solver.fzn_flag = ASTString("fzn_flag");
  opts.solver.fzn_time_limit_ms = ASTString("fzn_time_limit_ms");
  opts.solver.fzn_sigint = ASTString("fzn_sigint");

  cli_cat.general = ASTString("General Options");
  cli_cat.io = ASTString("Input/Output Options");
  cli_cat.solver = ASTString("Solver Options");
  cli_cat.translation = ASTString("Translation Options");
};

void Constants::mark(MINIZINC_GC_STAT_ARGS) {
  Expression::mark(lit_true);
  Expression::mark(var_true);
  Expression::mark(lit_false);
  Expression::mark(var_false);
  Expression::mark(var_ignore);
#if defined(MINIZINC_GC_STATS)
  Item::mark(var_redef, gc_stats);
#else
  Item::mark(var_redef);
#endif
  Expression::mark(absent);
  Expression::mark(infinity);

  ids.forall.mark();
  ids.exists.mark();
  ids.clause.mark();
  ids.bool2int.mark();
  ids.int2float.mark();
  ids.bool2float.mark();
  ids.sum.mark();
  ids.lin_exp.mark();
  ids.element.mark();
  ids.show.mark();
  ids.output.mark();
  ids.fix.mark();

  ids.int_.lin_eq.mark();
  ids.int_.lin_le.mark();
  ids.int_.lin_ne.mark();
  ids.int_.plus.mark();
  ids.int_.minus.mark();
  ids.int_.times.mark();
  ids.int_.div.mark();
  ids.int_.mod.mark();
  ids.int_.lt.mark();
  ids.int_.le.mark();
  ids.int_.gt.mark();
  ids.int_.ge.mark();
  ids.int_.eq.mark();
  ids.int_.ne.mark();

  ids.int_reif.lin_eq.mark();
  ids.int_reif.lin_le.mark();
  ids.int_reif.lin_ne.mark();
  ids.int_reif.plus.mark();
  ids.int_reif.minus.mark();
  ids.int_reif.times.mark();
  ids.int_reif.div.mark();
  ids.int_reif.mod.mark();
  ids.int_reif.lt.mark();
  ids.int_reif.le.mark();
  ids.int_reif.gt.mark();
  ids.int_reif.ge.mark();
  ids.int_reif.eq.mark();
  ids.int_reif.ne.mark();

  ids.float_.lin_eq.mark();
  ids.float_.lin_le.mark();
  ids.float_.lin_lt.mark();
  ids.float_.lin_ne.mark();
  ids.float_.plus.mark();
  ids.float_.minus.mark();
  ids.float_.times.mark();
  ids.float_.div.mark();
  ids.float_.mod.mark();
  ids.float_.lt.mark();
  ids.float_.le.mark();
  ids.float_.gt.mark();
  ids.float_.ge.mark();
  ids.float_.eq.mark();
  ids.float_.ne.mark();
  ids.float_.in.mark();
  ids.float_.dom.mark();

  ids.float_reif.lin_eq.mark();
  ids.float_reif.lin_le.mark();
  ids.float_reif.lin_lt.mark();
  ids.float_reif.lin_ne.mark();
  ids.float_reif.plus.mark();
  ids.float_reif.minus.mark();
  ids.float_reif.times.mark();
  ids.float_reif.div.mark();
  ids.float_reif.mod.mark();
  ids.float_reif.lt.mark();
  ids.float_reif.le.mark();
  ids.float_reif.gt.mark();
  ids.float_reif.ge.mark();
  ids.float_reif.eq.mark();
  ids.float_reif.ne.mark();
  ids.float_reif.in.mark();

  ids.bool_eq.mark();
  ids.bool_eq_reif.mark();
  ids.bool_not.mark();
  ids.bool_clause.mark();
  ids.bool_clause_reif.mark();
  ids.bool_xor.mark();
  ids.array_bool_or.mark();
  ids.array_bool_and.mark();
  ids.set_eq.mark();
  ids.set_in.mark();
  ids.set_subset.mark();
  ids.set_card.mark();
  ids.pow.mark();

  ids.assert.mark();
  ids.mzn_deprecate.mark();
  ids.trace.mark();
  ids.introduced_var.mark();
  ids.anonEnumFromStrings.mark();
  Expression::mark(ctx.root);
  Expression::mark(ctx.pos);
  Expression::mark(ctx.neg);
  Expression::mark(ctx.mix);
  Expression::mark(ann.output_var);
  Expression::mark(ann.output_only);
  Expression::mark(ann.add_to_output);
  Expression::mark(ann.mzn_check_var);
  ann.mzn_check_enum_var.mark();
  ann.output_array.mark();
  Expression::mark(ann.is_defined_var);
  ann.defines_var.mark();
  Expression::mark(ann.is_reverse_map);
  Expression::mark(ann.promise_total);
  Expression::mark(ann.maybe_partial);
  ann.doc_comment.mark();
  ann.mzn_path.mark();
  ann.is_introduced.mark();
  Expression::mark(ann.user_cut);
  Expression::mark(ann.lazy_constraint);
#ifndef NDEBUG
  Expression::mark(ann.mzn_break_here);
#endif
  Expression::mark(ann.rhs_from_assignment);
  Expression::mark(ann.domain_change_constraint);
  ann.mzn_deprecated.mark();
  Expression::mark(ann.mzn_was_undefined);
  Expression::mark(ann.array_check_form);

  cli.cmdlineData_short_str.mark();
  cli.cmdlineData_str.mark();
  cli.datafile_short_str.mark();
  cli.datafile_str.mark();
  cli.globalsDir_alt_str.mark();
  cli.globalsDir_short_str.mark();
  cli.globalsDir_str.mark();
  cli.help_short_str.mark();
  cli.help_str.mark();
  cli.ignoreStdlib_str.mark();
  cli.include_str.mark();
  cli.inputFromStdin_str.mark();
  cli.instanceCheckOnly_str.mark();
  cli.newfzn_str.mark();
  cli.no_optimize_alt_str.mark();
  cli.no_optimize_str.mark();
  cli.no_outputOzn_short_str.mark();
  cli.no_outputOzn_str.mark();
  cli.no_typecheck_str.mark();
  cli.outputBase_str.mark();
  cli.outputFznToStdout_alt_str.mark();
  cli.outputFznToStdout_str.mark();
  cli.outputOznToFile_str.mark();
  cli.outputOznToStdout_str.mark();
  cli.outputFznToFile_alt_str.mark();
  cli.outputFznToFile_short_str.mark();
  cli.outputFznToFile_str.mark();
  cli.rangeDomainsOnly_str.mark();
  cli.statistics_short_str.mark();
  cli.statistics_str.mark();
  cli.stdlib_str.mark();
  cli.verbose_short_str.mark();
  cli.verbose_str.mark();
  cli.version_str.mark();
  cli.werror_str.mark();

  cli.solver.all_sols_str.mark();
  cli.solver.fzn_solver_str.mark();

  opts.cmdlineData.mark();
  opts.datafile.mark();
  opts.datafiles.mark();
  opts.fznToFile.mark();
  opts.fznToStdout.mark();
  opts.globalsDir.mark();
  opts.ignoreStdlib.mark();
  opts.includePaths.mark();
  opts.includeDir.mark();
  opts.inputFromStdin.mark();
  opts.instanceCheckOnly.mark();
  opts.model.mark();
  opts.newfzn.mark();
  opts.noOznOutput.mark();
  opts.optimize.mark();
  opts.outputBase.mark();
  opts.oznToFile.mark();
  opts.oznToStdout.mark();
  opts.rangeDomainsOnly.mark();
  opts.statistics.mark();
  opts.stdlib.mark();
  opts.typecheck.mark();
  opts.verbose.mark();
  opts.werror.mark();

  opts.solver.allSols.mark();
  opts.solver.numSols.mark();
  opts.solver.threads.mark();
  opts.solver.fzn_solver.mark();
  opts.solver.fzn_flags.mark();
  opts.solver.fzn_flag.mark();
  opts.solver.fzn_time_limit_ms.mark();
  opts.solver.fzn_sigint.mark();

  cli_cat.general.mark();
  cli_cat.io.mark();
  cli_cat.solver.mark();
  cli_cat.translation.mark();
}

const int Constants::max_array_size;

Constants& constants(void) {
  static Constants _c;
  return _c;
}

Annotation::~Annotation(void) { delete _s; }

bool Annotation::contains(Expression* e) const { return _s && _s->contains(e); }

bool Annotation::isEmpty(void) const { return _s == nullptr || _s->isEmpty(); }

ExpressionSetIter Annotation::begin(void) const {
  return _s == nullptr ? ExpressionSetIter(true) : _s->begin();
}

ExpressionSetIter Annotation::end(void) const {
  return _s == nullptr ? ExpressionSetIter(true) : _s->end();
}

void Annotation::add(Expression* e) {
  if (_s == nullptr) _s = new ExpressionSet;
  if (e) _s->insert(e);
}

void Annotation::add(std::vector<Expression*> e) {
  if (_s == nullptr) _s = new ExpressionSet;
  for (auto i = static_cast<unsigned int>(e.size()); i--;)
    if (e[i]) _s->insert(e[i]);
}

void Annotation::remove(Expression* e) {
  if (_s && e) {
    _s->remove(e);
  }
}

void Annotation::removeCall(const ASTString& id) {
  if (_s == nullptr) return;
  std::vector<Expression*> toRemove;
  for (ExpressionSetIter it = _s->begin(); it != _s->end(); ++it) {
    if (Call* c = (*it)->dyn_cast<Call>()) {
      if (c->id() == id) toRemove.push_back(*it);
    }
  }
  for (auto i = static_cast<unsigned int>(toRemove.size()); i--;) _s->remove(toRemove[i]);
}

Call* Annotation::getCall(const ASTString& id) const {
  if (_s == nullptr) return nullptr;
  for (ExpressionSetIter it = _s->begin(); it != _s->end(); ++it) {
    if (Call* c = (*it)->dyn_cast<Call>()) {
      if (c->id() == id) return c;
    }
  }
  return nullptr;
}

bool Annotation::containsCall(const MiniZinc::ASTString& id) const {
  if (_s == nullptr) return false;
  for (ExpressionSetIter it = _s->begin(); it != _s->end(); ++it) {
    if (Call* c = (*it)->dyn_cast<Call>()) {
      if (c->id() == id) return true;
    }
  }
  return false;
}

void Annotation::clear(void) {
  if (_s) {
    _s->clear();
  }
}

void Annotation::merge(const Annotation& ann) {
  if (ann._s == nullptr) return;
  if (_s == nullptr) {
    _s = new ExpressionSet;
  }
  for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
    _s->insert(*it);
  }
}

Expression* getAnnotation(const Annotation& ann, std::string str) {
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if ((e->isa<Id>() && e->cast<Id>()->str() == str) ||
        (e->isa<Call>() && e->cast<Call>()->id() == str)) {
      return e;
    }
  }
  return nullptr;
}
Expression* getAnnotation(const Annotation& ann, const ASTString& str) {
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if ((e->isa<Id>() && e->cast<Id>()->str() == str) ||
        (e->isa<Call>() && e->cast<Call>()->id() == str))
      return e;
  }
  return nullptr;
}
}  // namespace MiniZinc
