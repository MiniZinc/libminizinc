/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/aststring.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/warning.hh>

#include <atomic>
#include <cassert>
#include <cmath>
#include <random>
#include <utility>

// TODO: Should this be a command line option? It doesn't seem too expensive
// #define OUTPUT_CALLTREE

namespace MiniZinc {

/// Result of evaluation
class EE {
public:
  /// The result value
  KeepAlive r;
  /// Boolean expression representing whether result is defined
  KeepAlive b;
  /// Constructor
  explicit EE(Expression* r0 = nullptr, Expression* b0 = nullptr) : r(r0), b(b0) {}
};

/// Boolean evaluation context
enum BCtx { C_ROOT, C_POS, C_NEG, C_MIX };

/// Evaluation context
struct Ctx {
  /// Boolean context
  BCtx b;
  /// Integer context
  BCtx i;
  /// Boolen negation flag
  bool neg;
  /// Default constructor (root context)
  Ctx() : b(C_ROOT), i(C_MIX), neg(false) {}
  /// Copy constructor
  Ctx(const Ctx& ctx) : b(ctx.b), i(ctx.i), neg(ctx.neg) {}
  /// Assignment operator
  Ctx& operator=(const Ctx& ctx) {
    if (this != &ctx) {
      b = ctx.b;
      i = ctx.i;
      neg = ctx.neg;
    }
    return *this;
  }
  /// Return true variable if in root context, nullptr otherwise
  VarDecl* partialityVar(EnvI& env) const;
};

/// Turn \a c into positive context
BCtx operator+(const BCtx& c);
/// Negate context \a c
BCtx operator-(const BCtx& c);

struct MultiPassInfo {
  // The current pass number (used for unifying and disabling path construction in final pass)
  unsigned int currentPassNumber;
  // Used for disabling path construction in final pass
  unsigned int finalPassNumber;

  MultiPassInfo();
};

struct VarPathStore {
  // Used for disabling path construction past the maxPathDepth of previous passes
  unsigned int maxPathDepth;

  struct PathVar {
    KeepAlive decl;
    unsigned int passNumber;
  };
  // Store mapping from path string to (VarDecl, pass_no) tuples
  typedef std::unordered_map<std::string, PathVar> PathMap;
  // Mapping from arbitrary Expressions to paths
  typedef KeepAliveMap<std::string> ReversePathMap;

  PathMap pathMap;
  ReversePathMap reversePathMap;
  ASTStringSet filenameSet;

  VarPathStore();
  PathMap& getPathMap() { return pathMap; }
  ReversePathMap& getReversePathMap() { return reversePathMap; }
  ASTStringSet& getFilenameSet() { return filenameSet; }
};

class ErrStreamWrapper {
private:
  std::ostream& _stream;
  bool _traceModified;
  std::string _prevTraceLoc;

public:
  explicit ErrStreamWrapper(std::ostream& stream) : _stream(stream), _traceModified(true) {}
  template <typename T>
  ErrStreamWrapper& operator<<(const T& t) {
    _traceModified = true;
    _stream << t;
    return *this;
  }
  std::ostream& stream() { return _stream; }
  void resetTraceModified(const std::string& loc) {
    _traceModified = false;
    _prevTraceLoc = loc;
  }
  bool traceModified() const { return _traceModified; }
  std::string prevTraceLoc() const { return _prevTraceLoc; }
};

class OutputSectionStore : public GCMarker {
private:
  typedef std::vector<std::pair<ASTString, Expression*>> OutputSections;

public:
  typedef OutputSections::iterator iterator;
  typedef OutputSections::const_iterator const_iterator;

  void add(ASTString section, Expression* e);
  bool empty() const { return _sections.empty(); };
  bool contains(ASTString section) const { return _idx.count(section) > 0; }

  iterator begin() { return _sections.begin(); }
  const_iterator begin() const { return _sections.begin(); }
  iterator end() { return _sections.end(); }
  const_iterator end() const { return _sections.end(); }

private:
  OutputSections _sections;
  std::unordered_map<ASTString, OutputSections::size_type> _idx;

protected:
  void mark() override {
    for (auto& it : *this) {
      it.first.mark();
      Expression::mark(it.second);
    }
  }
};

class StructType {
public:
  virtual size_t size() const = 0;
  virtual Type operator[](size_t i) const = 0;
  bool containsArray(const EnvI& env) const;
};

class TupleType : public StructType {
protected:
  size_t _size;
  Type _fields[1];  // Resized by TupleType::a
  TupleType(const std::vector<Type>& fields);

public:
  static TupleType* a(const std::vector<Type>& fields);
  static void free(TupleType* rt) { ::free(rt); }
  ~TupleType() = delete;

  size_t size() const override { return _size; }
  Type operator[](size_t i) const override {
    assert(i < size());
    return _fields[i];
  }
  size_t hash() const {
    std::size_t seed = _size;
    for (size_t i = 0; i < _size; ++i) {
      seed ^= _fields[i].toInt() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  bool operator==(const TupleType& rhs) const {
    if (_size != rhs._size) {
      return false;
    }
    for (int i = 0; i < _size; ++i) {
      if (_fields[i].cmp(rhs._fields[i]) != 0) {
        return false;
      }
    }
    return true;
  }

  bool isSubtypeOf(const EnvI& env, const TupleType& other, bool strictEnum) const {
    if (other.size() != size()) {
      return false;
    }
    for (size_t i = 0; i < other.size(); ++i) {
      if (!operator[](i).isSubtypeOf(env, other[i], strictEnum)) {
        return false;
      }
    }
    return true;
  }
  bool matchesBT(const EnvI& env, const TupleType& other) const;

  struct Hash {
    size_t operator()(const TupleType* tt) const { return tt->hash(); }
  };
  struct Equals {
    bool operator()(const TupleType* lhs, const TupleType* rhs) const { return *lhs == *rhs; }
  };
};

class RecordType : public StructType {
protected:
  // name offset + type
  using FieldTup = std::pair<size_t, Type>;
  size_t _size;
  std::string _fieldNames;
  FieldTup _fields[1];  // Resized by TupleType::a
  RecordType(const std::vector<std::pair<ASTString, Type>>& fields);
  RecordType(const RecordType& orig);

public:
  static RecordType* a(const std::vector<std::pair<ASTString, Type>>& fields);
  static RecordType* a(const RecordType* orig, const std::vector<Type>& types);
  static void free(RecordType* tt) { ::free(tt); }

  size_t size() const override { return _size; }
  Type operator[](size_t i) const override {
    assert(i < size());
    return _fields[i].second;
  }
  std::string fieldName(size_t i) const {
    assert(i < size());
    if (i + 1 < size()) {
      return _fieldNames.substr(_fields[i].first, _fields[i + 1].first - _fields[i].first);
    }
    return _fieldNames.substr(_fields[i].first);
  }
  std::pair<bool, size_t> findField(const ASTString& name) const {
    for (size_t i = 0; i < size(); ++i) {
      if (fieldName(i) == name) {
        return {true, i};
      }
    }
    return {false, 0};
  };
  size_t hash() const {
    std::size_t seed = _size;
    std::hash<std::string> h;
    for (size_t i = 0; i < _size; ++i) {
      seed ^= h(fieldName(i)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= _fields[i].second.toInt() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  bool operator==(const RecordType& rhs) const {
    if (_size != rhs._size || _fieldNames != rhs._fieldNames) {
      return false;
    }
    for (int i = 0; i < _size; ++i) {
      if (_fields[i].first != rhs._fields[i].first ||
          _fields[i].second.cmp(rhs._fields[i].second) != 0) {
        return false;
      }
    }
    return true;
  }

  bool isSubtypeOf(const EnvI& env, const RecordType& other, bool strictEnum) const {
    if (other.size() != size()) {
      return false;
    }
    for (size_t i = 0; i < other.size(); ++i) {
      // TODO: Should we allow subtyping based on name?
      if (fieldName(i) != other.fieldName(i)) {
        return false;
      }
      if (!operator[](i).isSubtypeOf(env, other[i], strictEnum)) {
        return false;
      }
    }
    return true;
  }
  bool matchesBT(const EnvI& env, const RecordType& other) const;

  struct Hash {
    size_t operator()(const RecordType* tt) const { return tt->hash(); }
  };
  struct Equals {
    bool operator()(const RecordType* lhs, const RecordType* rhs) const { return *lhs == *rhs; }
  };
};

struct TypeList : public StructType {
  const std::vector<Type>& tt;
  TypeList(const std::vector<Type>& ts) : tt(ts){};
  size_t size() const override { return tt.size(); }
  Type operator[](size_t i) const override { return tt[i]; };
};

class EnvI {
  friend class Type;
  template <bool ignoreVarDecl>
  friend class Typer;
  friend KeepAlive add_coercion(EnvI& env, Model* m, Expression* e, const Type& funarg_t);
  friend Type type_from_tmap(EnvI& env, TypeInst* ti,
                             const ASTStringMap<std::pair<Type, bool>>& tmap);

public:
  Model* model;
  Model* originalModel;
  Model* output;
  VarOccurrences varOccurrences;
  VarOccurrences outputVarOccurrences;

  const Constants& constants;

  std::ostream& outstream;
  ErrStreamWrapper errstream;
  std::stringstream logstream;
  std::stringstream checkerOutput;

#ifdef OUTPUT_CALLTREE
  // Call stack depth
  int callDepth = 0;
#endif

  VarOccurrences outputFlatVarOccurrences;
  CopyMap cmap;
  IdMap<KeepAlive> reverseMappers;
  struct WW {
    Expression* r;
    Expression* b;
    WW(Expression* r0, Expression* b0) : r(r0), b(b0) {}
  };
  class CSEMap : public KeepAliveMap<WW> {
  public:
    void fixWeakRefs() override {
      std::vector<Expression*> toRemove;
      for (auto& it : _m) {
        if (!Expression::hasMark(it.second.r) || !Expression::hasMark(it.second.b)) {
          toRemove.push_back(it.first);
        }
      }
      for (auto* e : toRemove) {
        _m.erase(e);
      }
    }
  };
  bool ignorePartial;
  bool ignoreUnknownIds;
  struct CallStackEntry {
    Expression* e;
    bool tag;
    bool replaced;
    CallStackEntry(Expression* e0 = nullptr, bool tag0 = false)
        : e(e0), tag(tag0), replaced(false) {}
  };
  std::vector<CallStackEntry> callStack;
  std::vector<int> idStack;
  unsigned int maxCallStack;
  std::vector<std::unique_ptr<Warning>> warnings;
  std::vector<int> modifiedVarDecls;
  std::unordered_set<std::string> deprecationWarnings;
  int inRedundantConstraint;
  int inSymmetryBreakingConstraint;
  int inMaybePartial;
  bool inTraceExp;
  struct {
    int reifConstraints;
    int impConstraints;
    int impDel;
    int linDel;
  } counters;
  bool inReverseMapVar;
  FlatteningOptions fopts;
  ASTStringMap<Item*> reverseEnum;
  std::vector<KeepAlive> checkVars;
  std::vector<std::pair<ASTString, KeepAlive>> outputVars;
  OutputSectionStore outputSections;

  // General multipass information
  MultiPassInfo multiPassInfo;

  // Storage for mznpaths
  VarPathStore varPathStore;

protected:
  CSEMap _cseMap;
  Model* _flat;
  bool _failed;
  unsigned int _ids;
  ASTStringMap<ASTString> _reifyMap;
  typedef std::unordered_map<VarDeclI*, unsigned int> EnumMap;
  EnumMap _enumMap;
  std::vector<VarDeclI*> _enumVarDecls;
  typedef std::unordered_map<std::string, unsigned int> ArrayEnumMap;
  ArrayEnumMap _arrayEnumMap;
  std::vector<std::vector<unsigned int>> _arrayEnumDecls;
  typedef std::unordered_map<TupleType*, unsigned int, TupleType::Hash, TupleType::Equals>
      TupleTypeMap;
  TupleTypeMap _tupleTypeMap;
  std::vector<TupleType*> _tupleTypes;
  typedef std::unordered_map<RecordType*, unsigned int, RecordType::Hash, RecordType::Equals>
      RecordTypeMap;
  RecordTypeMap _recordTypeMap;
  std::vector<RecordType*> _recordTypes;
  bool _collectVardecls;
  std::default_random_engine _g;
  std::atomic<bool> _cancel = {false};

  /// Register tuple type directly from a list of fields
  /// WARNING: This method is unsafe unless the tuple is explicitly made canonical and the types
  /// of the TypeInst objects are actively maintained. Use method on TypeInst objects whenever
  /// possible.
  unsigned int registerTupleType(const std::vector<Type>& fields);
  /// Register record type directly from a list of fields
  /// WARNING: This method is unsafe unless the tuple is explicitly made canonical and the types
  /// of the TypeInst objects are actively maintained. Use method on TypeInst objects whenever
  /// possible.
  unsigned int registerRecordType(const std::vector<std::pair<ASTString, Type>>& fields);
  /// Variant of above function which reused the list of names of previous record Type
  unsigned int registerRecordType(const RecordType* orig, const std::vector<Type>& field_type);

  /// Get the tuple type from the register using a direct key (typeId in Type).
  /// WARNING: This method is unsafe unless the ArrayTypes have been resolved. Use method on Type
  /// whenever possible.
  TupleType* getTupleType(unsigned int i) const {
    assert(i > 0 && i <= _tupleTypes.size());
    return _tupleTypes[i - 1];
  }
  /// Get the tuple type from the register using a direct key (typeId in Type).
  /// WARNING: This method is unsafe unless the ArrayTypes have been resolved. Use method on Type
  /// whenever possible.
  RecordType* getRecordType(unsigned int i) const {
    assert(i > 0 && i <= _recordTypes.size());
    return _recordTypes[i - 1];
  }
  /// Get the struct type from the register using a direct key (typeId in Type).
  /// WARNING: This method is unsafe unless the ArrayTypes have been resolved. Use method on Type
  /// whenever possible.
  StructType* getStructType(unsigned int typeId, Type::BaseType bt) const {
    if (bt == Type::BT_TUPLE) {
      return getTupleType(typeId);
    }
    return getRecordType(typeId);
  }

public:
  EnvI(Model* model0, std::ostream& outstream0 = std::cout, std::ostream& errstream0 = std::cerr);
  ~EnvI();
  long long int genId();
  /// Set minimum new temporary id to \a i+1
  void minId(unsigned int i) { _ids = std::max(_ids, i + 1); }
  void cseMapInsert(Expression* e, const EE& ee);
  CSEMap::iterator cseMapFind(Expression* e);
  void cseMapRemove(Expression* e);
  CSEMap::iterator cseMapEnd();
  void dump();

  unsigned int registerEnum(VarDeclI* vdi);
  VarDeclI* getEnum(unsigned int i) const;
  unsigned int registerArrayEnum(const std::vector<unsigned int>& arrayEnum);
  const std::vector<unsigned int>& getArrayEnum(unsigned int i) const;
  // Register a new tuple type from a TypeInst.
  // NOTE: this method updates the types of the TypeInst and its domain to become cononical tuple
  // types.
  unsigned int registerTupleType(TypeInst* ti);
  // Register a new tuple type from an tuple literal.
  // NOTE: this method updates the type of the ArrayLit object
  unsigned int registerTupleType(ArrayLit* tup);
  // Get the TupleType for Type with tuple BaseType (safe)
  TupleType* getTupleType(Type t) const {
    assert(t.bt() == Type::BT_TUPLE);
    unsigned int typeId = t.typeId();
    assert(typeId != 0);
    if (t.dim() > 0) {
      const std::vector<unsigned int>& arrayEnumIds = getArrayEnum(typeId);
      typeId = arrayEnumIds[arrayEnumIds.size() - 1];
    }
    return getTupleType(typeId);
  }
  // Register a new record type from a TypeInst.
  // NOTE: this method updates the types of the TypeInst and its domain to become cononical tuple
  // types.
  unsigned int registerRecordType(TypeInst* ti);
  // Register a new tuple type from an tuple literal.
  // NOTE: this method expects an array with VarDecl objects. It will update the type and content
  // of the ArrayLit to only contain the expressions.
  unsigned int registerRecordType(ArrayLit* rec);
  // Get the TupleType for Type with tuple BaseType (safe)
  RecordType* getRecordType(Type t) const {
    assert(t.bt() == Type::BT_RECORD);
    unsigned int typeId = t.typeId();
    assert(typeId != 0);
    if (t.dim() > 0) {
      const std::vector<unsigned int>& arrayEnumIds = getArrayEnum(typeId);
      typeId = arrayEnumIds[arrayEnumIds.size() - 1];
    }
    return getRecordType(typeId);
  }
  StructType* getStructType(Type t) const {
    assert(t.structBT());
    unsigned int typeId = t.typeId();
    assert(typeId != 0);
    if (t.dim() > 0) {
      const std::vector<unsigned int>& arrayEnumIds = getArrayEnum(typeId);
      typeId = arrayEnumIds[arrayEnumIds.size() - 1];
    }
    return getStructType(typeId, t.bt());
  }
  /// Returns the type of a common tuple type or bot if no such tuple type exists
  Type commonTuple(Type tuple1, Type tuple2, bool ignoreTuple1Dim = false);
  /// Returns the type of a common record type or 0b if no such tuple type exists
  Type commonRecord(Type record1, Type record2, bool ignoreRecord1Dim = false);
  /// Returns a record type that merges the fields to two record types
  /// WARNING: This method throws an error when two fields have the same name.
  Type mergeRecord(Type record1, Type record2, Location loc);
  /// Returns a tuple type of `tuple1 ++ tuple2'
  Type concatTuple(Type tuple1, Type tuple2);
  /// Check if tuple can be evaluated (instead of flattened).
  /// (i.e., true if the tuple contains to variable or annotation types)
  bool tupleIsPar(const Type& tuple);
  std::string enumToString(unsigned int enumId, int i);
  /// Check if \a t1 is a subtype of \a t2 (including enumerated types if \a strictEnum is true)
  bool isSubtype(const Type& t1, const Type& t2, bool strictEnum) const;
  bool hasReverseMapper(Id* ident) { return reverseMappers.find(ident) != reverseMappers.end(); }

  void flatAddItem(Item* i);
  void flatRemoveItem(ConstraintI* i);
  void flatRemoveItem(VarDeclI* i);
  void flatRemoveExpr(Expression* e, Item* i);

  std::tuple<BCtx, bool> annToCtx(VarDecl* vd) const;
  Id* ctxToAnn(BCtx c) const;
  void addCtxAnn(VarDecl* vd, const BCtx& c) const;

  void voAddExp(VarDecl* vd);
  void annotateFromCallStack(Expression* e);
  ArrayLit* createAnnotationArray(const BCtx& ctx);
  void fail(const std::string& msg = std::string(), const Location& loc = Location());
  bool failed() const;
  Model* flat();
  void swap();
  void swapOutput() { std::swap(model, output); }
  ASTString reifyId(const ASTString& id);
  static ASTString halfReifyId(const ASTString& id);
  bool dumpPath(std::ostream& os, bool force = false);
  int addWarning(const std::string& msg);
  int addWarning(const Location& loc, const std::string& msg, bool dumpStack = true);
  void collectVarDecls(bool b);

  void copyPathMapsAndState(EnvI& env);
  /// deprecated, use Solns2Out
  std::ostream& evalOutput(std::ostream& os, std::ostream& log);
  Call* surroundingCall() const;

  void cleanupExceptOutput();
  std::default_random_engine& rndGenerator() { return _g; }
  void setRandomSeed(long unsigned int r) { _g.seed(r); }
  void cancel() { _cancel = true; }
  void checkCancel() {
    if (_cancel) {  // TODO: Should this be annotated "unlikely"?
      throw Timeout();
    }
  }

  bool outputSectionEnabled(ASTString section) const;

  std::string show(Expression* e);
  std::string show(const IntVal& iv, unsigned int enumId);
  std::string show(IntSetVal* isv, unsigned int enumId);
};

inline VarDecl* Ctx::partialityVar(EnvI& env) const {
  return b == C_ROOT ? env.constants.varTrue : nullptr;
}

void set_computed_domain(EnvI& envi, VarDecl* vd, Expression* domain, bool is_computed);
EE flat_exp(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b,
              bool doNotFollowChains);

std::vector<Expression*> field_slices(EnvI& env, Expression* arrExpr);

class CmpExpIdx {
public:
  std::vector<KeepAlive>& x;
  CmpExpIdx(std::vector<KeepAlive>& x0) : x(x0) {}
  bool operator()(int i, int j) const {
    if (Expression::equal(x[i](), x[j]())) {
      return false;
    }
    if (Expression::isa<Id>(x[i]()) && Expression::isa<Id>(x[j]()) &&
        Expression::cast<Id>(x[i]())->idn() != -1 && Expression::cast<Id>(x[j]())->idn() != -1) {
      return Expression::cast<Id>(x[i]())->idn() < Expression::cast<Id>(x[j]())->idn();
    }
    return x[i]() < x[j]();
  }
};

struct RecordFieldSort {
  bool operator()(const VarDecl* a, const VarDecl* b) const {
    return operator()(a->id()->str(), b->id()->str());
  }
  bool operator()(const std::pair<ASTString, Type>& a, const std::pair<ASTString, Type>& b) const {
    return operator()(a.first, b.first);
  }
  bool operator()(ASTString a, ASTString b) const { return std::strcmp(a.c_str(), b.c_str()) < 0; }
};

template <class Lit>
class LinearTraits {};
template <>
class LinearTraits<IntLit> {
public:
  typedef IntVal Val;
  static Val eval(EnvI& env, Expression* e) { return eval_int(env, e); }
  static void constructLinBuiltin(EnvI& env, BinOpType bot, ASTString& callid, int& coeff_sign,
                                  Val& d) {
    switch (bot) {
      case BOT_LE:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = 1;
        d += 1;
        break;
      case BOT_LQ:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = 1;
        break;
      case BOT_GR:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = -1;
        d = -d + 1;
        break;
      case BOT_GQ:
        callid = env.constants.ids.int_.lin_le;
        coeff_sign = -1;
        d = -d;
        break;
      case BOT_EQ:
        callid = env.constants.ids.int_.lin_eq;
        coeff_sign = 1;
        break;
      case BOT_NQ:
        callid = env.constants.ids.int_.lin_ne;
        coeff_sign = 1;
        break;
      default:
        assert(false);
        break;
    }
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_eq() { return Constants::constants().ids.int_.eq; }
  typedef IntBounds Bounds;
  static bool finite(const IntBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
  static bool finite(const IntVal& v) { return v.isFinite(); }
  static Bounds computeBounds(EnvI& env, Expression* e) { return compute_int_bounds(env, e); }
  typedef IntSetVal* Domain;
  static Domain evalDomain(EnvI& env, Expression* e) { return eval_intset(env, e); }
  static Expression* newDomain(Val v) {
    return new SetLit(Location().introduce(), IntSetVal::a(v, v));
  }
  static Expression* newDomain(Val v0, Val v1) {
    return new SetLit(Location().introduce(), IntSetVal::a(v0, v1));
  }
  static Expression* newDomain(Domain d) { return new SetLit(Location().introduce(), d); }
  static bool domainContains(Domain dom, Val v) { return dom->contains(v); }
  static bool domainEquals(Domain dom, Val v) {
    return dom->size() == 1 && dom->min(0) == v && dom->max(0) == v;
  }
  static bool domainEquals(Domain dom1, Domain dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::equal(d1, d2);
  }
  static bool domainSubset(Domain dom1, Domain dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::subset(d1, d2);
  }
  static bool domainDisjoint(Domain dom1, Domain dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::disjoint(d1, d2);
  }
  static bool domainTighter(Domain dom, Bounds b) {
    return !b.valid || dom->min() > b.l || dom->max() < b.u;
  }
  static bool domainIntersects(Domain dom, Val v0, Val v1) {
    return (v0 > v1) || (!dom->empty() && dom->min(0) <= v1 && v0 <= dom->max(dom->size() - 1));
  }
  static bool domainEmpty(Domain dom) { return dom->empty(); }
  static Domain limitDomain(BinOpType bot, Domain dom, Val v) {
    IntSetRanges dr(dom);
    IntSetVal* ndomain;
    switch (bot) {
      case BOT_LE:
        v -= 1;
        // fall through
      case BOT_LQ: {
        Ranges::Bounded<IntVal, IntSetRanges> b =
            Ranges::Bounded<IntVal, IntSetRanges>::maxiter(dr, v);
        ndomain = IntSetVal::ai(b);
      } break;
      case BOT_GR:
        v += 1;
        // fall through
      case BOT_GQ: {
        Ranges::Bounded<IntVal, IntSetRanges> b =
            Ranges::Bounded<IntVal, IntSetRanges>::miniter(dr, v);
        ndomain = IntSetVal::ai(b);
      } break;
      case BOT_NQ: {
        Ranges::Const<IntVal> c(v, v);
        Ranges::Diff<IntVal, IntSetRanges, Ranges::Const<IntVal>> d(dr, c);
        ndomain = IntSetVal::ai(d);
      } break;
      default:
        assert(false);
        return nullptr;
    }
    return ndomain;
  }
  static Domain intersectDomain(Domain dom, Val v0, Val v1) {
    IntSetRanges dr(dom);
    Ranges::Const<IntVal> c(v0, v1);
    Ranges::Inter<IntVal, IntSetRanges, Ranges::Const<IntVal>> inter(dr, c);
    return IntSetVal::ai(inter);
  }
  static Domain intersectDomain(Domain dom0, Domain dom1) {
    IntSetRanges dr0(dom0);
    IntSetRanges dr1(dom1);
    Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(dr0, dr1);
    return IntSetVal::ai(inter);
  }
  static Val floorDiv(Val v0, Val v1) {
    return static_cast<long long int>(
        floor(static_cast<double>(v0.toInt()) / static_cast<double>(v1.toInt())));
  }
  static Val ceilDiv(Val v0, Val v1) {
    return static_cast<long long int>(
        ceil(static_cast<double>(v0.toInt()) / static_cast<double>(v1.toInt())));
  }
  static IntLit* newLit(Val v) { return IntLit::a(v); }
  static IntVal v(const IntLit* il) { return IntLit::v(il); }
};
template <>
class LinearTraits<FloatLit> {
public:
  typedef FloatVal Val;
  static Val eval(EnvI& env, Expression* e) { return eval_float(env, e); }
  static void constructLinBuiltin(EnvI& env, BinOpType bot, ASTString& callid, int& coeff_sign,
                                  Val& d) {
    switch (bot) {
      case BOT_LE:
        callid = env.constants.ids.float_.lin_lt;
        coeff_sign = 1;
        break;
      case BOT_LQ:
        callid = env.constants.ids.float_.lin_le;
        coeff_sign = 1;
        break;
      case BOT_GR:
        callid = env.constants.ids.float_.lin_lt;
        coeff_sign = -1;
        d = -d;
        break;
      case BOT_GQ:
        callid = env.constants.ids.float_.lin_le;
        coeff_sign = -1;
        d = -d;
        break;
      case BOT_EQ:
        callid = env.constants.ids.float_.lin_eq;
        coeff_sign = 1;
        break;
      case BOT_NQ:
        callid = env.constants.ids.float_.lin_ne;
        coeff_sign = 1;
        break;
      default:
        assert(false);
        break;
    }
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  static ASTString id_eq() { return Constants::constants().ids.float_.eq; }
  typedef FloatBounds Bounds;
  static bool finite(const FloatBounds& ib) { return ib.l.isFinite() && ib.u.isFinite(); }
  static bool finite(const FloatVal& v) { return v.isFinite(); }
  static Bounds computeBounds(EnvI& env, Expression* e) { return compute_float_bounds(env, e); }
  typedef FloatSetVal* Domain;
  static Domain evalDomain(EnvI& env, Expression* e) { return eval_floatset(env, e); }

  static Expression* newDomain(Val v) {
    return new SetLit(Location().introduce(), FloatSetVal::a(v, v));
  }
  static Expression* newDomain(Val v0, Val v1) {
    return new SetLit(Location().introduce(), FloatSetVal::a(v0, v1));
  }
  static Expression* newDomain(Domain d) { return new SetLit(Location().introduce(), d); }
  static bool domainContains(Domain dom, Val v) { return dom->contains(v); }
  static bool domainEquals(Domain dom, Val v) {
    return dom->size() == 1 && dom->min(0) == v && dom->max(0) == v;
  }

  static bool domainTighter(Domain dom, Bounds b) {
    return !b.valid || dom->min() > b.l || dom->max() < b.u;
  }
  static bool domainIntersects(Domain dom, Val v0, Val v1) {
    return (v0 > v1) || (!dom->empty() && dom->min(0) <= v1 && v0 <= dom->max(dom->size() - 1));
  }
  static bool domainEmpty(Domain dom) { return dom->empty(); }

  static bool domainEquals(Domain dom1, Domain dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::equal(d1, d2);
  }
  static bool domainSubset(Domain dom1, Domain dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::subset(d1, d2);
  }
  static bool domainDisjoint(Domain dom1, Domain dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::disjoint(d1, d2);
  }
  static Domain intersectDomain(Domain dom, Val v0, Val v1) {
    if (dom != nullptr) {
      FloatSetRanges dr(dom);
      Ranges::Const<FloatVal> c(v0, v1);
      Ranges::Inter<FloatVal, FloatSetRanges, Ranges::Const<FloatVal>> inter(dr, c);
      return FloatSetVal::ai(inter);
    }
    Domain d = FloatSetVal::a(v0, v1);
    return d;
  }
  static Domain intersectDomain(Domain dom0, Domain dom1) {
    if (dom0 == nullptr) {
      return dom1;
    }
    if (dom1 == nullptr) {
      return dom0;
    }
    FloatSetRanges dr0(dom0);
    FloatSetRanges dr1(dom1);
    Ranges::Inter<FloatVal, FloatSetRanges, FloatSetRanges> inter(dr0, dr1);
    return FloatSetVal::ai(inter);
  }

  static Domain limitDomain(BinOpType bot, Domain dom, Val v) {
    FloatSetRanges dr(dom);
    FloatSetVal* ndomain;
    switch (bot) {
      case BOT_LE:
        return nullptr;
      case BOT_LQ: {
        Ranges::Bounded<FloatVal, FloatSetRanges> b =
            Ranges::Bounded<FloatVal, FloatSetRanges>::maxiter(dr, v);
        ndomain = FloatSetVal::ai(b);
      } break;
      case BOT_GR:
        return nullptr;
      case BOT_GQ: {
        Ranges::Bounded<FloatVal, FloatSetRanges> b =
            Ranges::Bounded<FloatVal, FloatSetRanges>::miniter(dr, v);
        ndomain = FloatSetVal::ai(b);
      } break;
      case BOT_NQ: {
        Ranges::Const<FloatVal> c(v, v);
        Ranges::Diff<FloatVal, FloatSetRanges, Ranges::Const<FloatVal>> d(dr, c);
        ndomain = FloatSetVal::ai(d);
      } break;
      default:
        assert(false);
        return nullptr;
    }
    return ndomain;
  }
  static Val floorDiv(Val v0, Val v1) { return v0 / v1; }
  static Val ceilDiv(Val v0, Val v1) { return v0 / v1; }
  static FloatLit* newLit(Val v) { return FloatLit::a(v); }
  static FloatVal v(const FloatLit* fl) { return FloatLit::v(fl); }
};

template <class Lit>
void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c, std::vector<KeepAlive>& x,
                  typename LinearTraits<Lit>::Val& d) {
  std::vector<int> idx(c.size());
  for (auto i = static_cast<int>(idx.size()); i--;) {
    idx[i] = i;
    Expression* e = follow_id_to_decl(x[i]());
    if (auto* vd = Expression::dynamicCast<VarDecl>(e)) {
      if (vd->e() && Expression::isa<Lit>(vd->e())) {
        x[i] = vd->e();
      } else {
        x[i] = Expression::cast<VarDecl>(e)->id();
      }
    } else {
      x[i] = e;
    }
  }
  std::sort(idx.begin(), idx.end(), CmpExpIdx(x));
  unsigned int ci = 0;
  for (; ci < x.size(); ci++) {
    if (Lit* il = Expression::dynamicCast<Lit>(x[idx[ci]]())) {
      d += c[idx[ci]] * LinearTraits<Lit>::v(il);
      c[idx[ci]] = 0;
    } else {
      break;
    }
  }
  for (unsigned int i = ci + 1; i < x.size(); i++) {
    if (Expression::equal(x[idx[i]](), x[idx[ci]]())) {
      c[idx[ci]] += c[idx[i]];
      c[idx[i]] = 0;
    } else if (Lit* il = Expression::dynamicCast<Lit>(x[idx[i]]())) {
      d += c[idx[i]] * LinearTraits<Lit>::v(il);
      c[idx[i]] = 0;
    } else {
      ci = i;
    }
  }
  ci = 0;
  for (unsigned int i = 0; i < c.size(); i++) {
    if (c[i] != 0) {
      c[ci] = c[i];
      x[ci] = x[i];
      ci++;
    }
  }
  c.resize(ci);
  x.resize(ci);
}

}  // namespace MiniZinc
