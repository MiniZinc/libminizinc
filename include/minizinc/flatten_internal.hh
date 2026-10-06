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

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <memory>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// TODO: Should this be a command line option? It doesn't seem too expensive
// #define OUTPUT_CALLTREE

namespace MiniZinc {

/// Result of evaluation
class EE {
public:
  /// The result value
  Ref<Expression> r;
  /// Boolean expression representing whether result is defined
  Ref<Expression> b;
  /// Constructor
  explicit EE(Expression* r0 = nullptr, Expression* b0 = nullptr) : r(r0), b(b0) {}
};

struct MultiPassInfo {
  // The current pass number (used for unifying and disabling path construction in final pass)
  unsigned int currentPassNumber;
  // Used for disabling path construction in final pass
  unsigned int finalPassNumber;

  MultiPassInfo();
};

/// Interned store for the paths identifying variables across compilation passes.
///
/// A path is the flattening call stack, one frame per entry. Written out per
/// variable that is kilobytes, since every frame repeats its file name; stored
/// as a tree of shared cons cells it is one pointer. Text is built on demand.
class PathStore {
public:
  struct Node;
  /// A path, or nullptr for the empty one. Owned by, and only comparable within,
  /// the store that handed it out, hence the deleted copy below.
  using Path = const Node*;
  struct Node {
    Path parent;
    /// Rendered frame text, interned. Null when the frame stands in for `spliced`.
    const std::string* frame;
    /// An earlier path spliced in here, for compilation resumed away from where
    /// the expression came from.
    Path spliced;
  };

  PathStore() = default;
  PathStore(const PathStore&) = delete;
  PathStore& operator=(const PathStore&) = delete;

  /// Return \a parent extended by a frame rendering as \a frame.
  Path extend(Path parent, const std::string& frame);
  /// Return \a parent extended by a frame splicing in \a spliced.
  Path extendSpliced(Path parent, Path spliced);
  /// Write the text of \a p.
  static void print(std::ostream& os, Path p);
  /// Text of \a p.
  static std::string toString(Path p);
  /// Leftmost frame of \a p, which by construction reads "<filename>|<line>|...",
  /// or nullptr if \a p is empty.
  static const std::string* leadingFrame(Path p);
  /// Number of frames in \a p, spliced ones included. Walks the path; only used
  /// to pick between the few paths one expression accumulated.
  static unsigned int depth(Path p);
  /// Index identifying \a p, for an annotation to carry instead of its text.
  unsigned int markerIndex(Path p);
  /// The path index \a i identifies, or nullptr if it identifies none.
  Path fromMarkerIndex(IntVal i) const;

private:
  struct NodeHash {
    size_t operator()(const Node& n) const {
      // Cells are aligned, so a pointer's low bits carry nothing: mix, do not shift.
      size_t h = 0;
      for (const void* p : {static_cast<const void*>(n.parent), static_cast<const void*>(n.frame),
                            static_cast<const void*>(n.spliced)}) {
        h ^= std::hash<const void*>()(p) + 0x9e3779b9 + (h << 6) + (h >> 2);
      }
      return h;
    }
  };
  struct NodeEq {
    bool operator()(const Node& a, const Node& b) const {
      return a.parent == b.parent && a.frame == b.frame && a.spliced == b.spliced;
    }
  };
  /// Frame texts, interned. Node-based, so the addresses stay put.
  std::unordered_set<std::string> _frames;
  /// Paths that have been handed a marker index, and the index of each.
  std::vector<Path> _markerPaths;
  std::unordered_map<Path, unsigned int> _markers;
  /// Cons cells, interned so equal paths are the same pointer. The element is
  /// the whole key, so equality cannot miss a field.
  std::unordered_set<Node, NodeHash, NodeEq> _nodes;
};

struct VarPathStore {
  // Used for disabling path construction past the maxPathDepth of previous passes
  unsigned int maxPathDepth;

  struct PathVar {
    Ref<Expression> decl;
    unsigned int passNumber;
  };
  // Store mapping from path to (VarDecl, pass_no) tuples
  using PathMap = std::unordered_map<PathStore::Path, PathVar>;
  // Mapping from arbitrary Expressions to paths
  using ReversePathMap = KeepAliveMap<PathStore::Path>;

  PathMap pathMap;
  ReversePathMap reversePathMap;
  std::unordered_set<ASTString> filenameSet;
  /// Shared with the other passes' environments: paths compare by pointer, so
  /// every pass must intern into the same store.
  std::shared_ptr<PathStore> paths;

  VarPathStore();
  PathMap& getPathMap() { return pathMap; }
  ReversePathMap& getReversePathMap() { return reversePathMap; }
  std::unordered_set<ASTString>& getFilenameSet() { return filenameSet; }
  PathStore& getPaths() { return *paths; }
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

class OutputSectionStore {
public:
  struct OutputSection {
    ASTString section;
    Ref<Expression> e;
    bool json;
    OutputSection(ASTString section0, Expression* e0, bool json0 = false)
        : section(std::move(section0)), e(e0), json(json0) {}
  };

private:
  typedef std::vector<OutputSection> OutputSections;

public:
  typedef OutputSections::iterator iterator;
  typedef OutputSections::const_iterator const_iterator;

  void add(EnvI& env, const ASTString& section, Expression* e, bool json);
  bool empty() const { return _sections.empty(); };
  bool contains(const ASTString& section) const { return _idx.count(section) > 0; }
  bool noUserDefined() const { return _blank; };

  iterator begin() { return _sections.begin(); }
  const_iterator begin() const { return _sections.begin(); }
  iterator end() { return _sections.end(); }
  const_iterator end() const { return _sections.end(); }

private:
  OutputSections _sections;
  std::unordered_map<ASTString, OutputSections::size_type> _idx;
  bool _blank = true;

protected:
};

class StructType {
public:
  virtual unsigned int size() const = 0;
  virtual Type operator[](unsigned int i) const = 0;
  bool containsArray(const EnvI& env) const;
};

class TupleType : public StructType {
protected:
  unsigned int _size;
  Type _fields[1];  // Resized by TupleType::a
  TupleType(const std::vector<Type>& fields);

public:
  static TupleType* a(const std::vector<Type>& fields);
  static void free(TupleType* rt) { ::free(rt); }
  ~TupleType() = delete;

  unsigned int size() const override { return _size; }
  Type operator[](unsigned int i) const override {
    assert(i < size());
    return _fields[i];
  }
  size_t hash() const {
    std::size_t seed = _size;
    for (unsigned int i = 0; i < _size; ++i) {
      seed ^= _fields[i].toInt() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  bool operator==(const TupleType& rhs) const {
    if (_size != rhs._size) {
      return false;
    }
    for (unsigned int i = 0; i < _size; ++i) {
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
    for (unsigned int i = 0; i < other.size(); ++i) {
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
  unsigned int _size;
  std::string _fieldNames;
  FieldTup _fields[1];  // Resized by TupleType::a
  RecordType(const std::vector<std::pair<ASTString, Type>>& fields);
  RecordType(const RecordType& orig);

public:
  static RecordType* a(const std::vector<std::pair<ASTString, Type>>& fields);
  static RecordType* a(const RecordType* orig, const std::vector<Type>& types);
  static void free(RecordType* tt) { ::free(tt); }

  unsigned int size() const override { return _size; }
  Type operator[](unsigned int i) const override {
    assert(i < size());
    return _fields[i].second;
  }
  std::string fieldName(unsigned int i) const {
    assert(i < size());
    if (i + 1 < size()) {
      return _fieldNames.substr(_fields[i].first, _fields[i + 1].first - _fields[i].first);
    }
    return _fieldNames.substr(_fields[i].first);
  }
  std::pair<bool, unsigned int> findField(const ASTString& name) const {
    for (unsigned int i = 0; i < size(); ++i) {
      if (fieldName(i) == name) {
        return {true, i};
      }
    }
    return {false, 0};
  };
  size_t hash() const {
    std::size_t seed = _size;
    std::hash<std::string> h;
    for (unsigned int i = 0; i < _size; ++i) {
      seed ^= h(fieldName(i)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= _fields[i].second.toInt() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  bool operator==(const RecordType& rhs) const {
    if (_size != rhs._size || _fieldNames != rhs._fieldNames) {
      return false;
    }
    for (unsigned int i = 0; i < _size; ++i) {
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
    for (unsigned int i = 0; i < other.size(); ++i) {
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
  TypeList(const std::vector<Type>& ts) : tt(ts) {};
  unsigned int size() const override { return static_cast<unsigned int>(tt.size()); }
  Type operator[](unsigned int i) const override { return tt[i]; };
};

class EnvI {
  friend class Type;
  friend class TypeInst;
  template <bool ignoreVarDecl>
  friend class Typer;
  friend Ref<Expression> add_coercion(EnvI& env, Model* m, Expression* e,
                                      const Location& loc_default, const Type& funarg_t);
  friend Type type_from_tmap(EnvI& env, TypeInst* ti,
                             const std::unordered_map<ASTString, std::pair<Type, bool>>& tmap);

public:
  Model* model;
  Model* originalModel;
  /// The functions and variables of models that are already deleted that can hold themselves
  /// (see Env::~Env)
  std::vector<Weak<ASTNode>> cycleCandidates;
  /// Add the candidates of \a models (null entries are skipped) to cycleCandidates
  void gatherCycleCandidates(std::initializer_list<Model*> models);
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
  IdMap<Ref<Expression>> reverseMappers;
  /// CSE result: weak, so an entry does not keep a removed result alive (see cseMapFind)
  struct WW {
    Weak<Expression> r;
    Weak<Expression> b;
    WW(Expression* r0, Expression* b0) : r(canonical(r0)), b(canonical(b0)) {}
    /// A variable's own Id lives as long as the variable; a copy of the Id may not
    static Expression* canonical(Expression* e) {
      if (e != nullptr && Expression::isa<Id>(e) && Expression::cast<Id>(e)->decl() != nullptr) {
        return Expression::cast<Id>(e)->decl()->id();
      }
      return e;
    }
  };
  using CSEMap = KeepAliveMap<WW>;
  bool ignorePartial;
  bool ignoreUnknownIds;
  struct CallStackEntry {
    Expression* e;
    Ctx ctx;
    bool tag;
    bool replaced;
    /// Path up to this entry, filled on first request rather than at push time:
    /// a comprehension iterator is assigned its value after being pushed.
    PathStore::Path path;
    /// An earlier path this entry stands for, set where compilation resumes away
    /// from where the expression came from. Contributed instead of `e`'s frame.
    PathStore::Path pathSplice;
    CallStackEntry()
        : e(nullptr), tag(false), replaced(false), path(nullptr), pathSplice(nullptr) {}
    CallStackEntry(Expression* e0, bool tag0, const Ctx& ctx0)
        : e(e0), ctx(ctx0), tag(tag0), replaced(false), path(nullptr), pathSplice(nullptr) {}
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
  bool warnImplicitEnum2Int;
  /// Opt-in: run checkAuthoritativeParameterNames, which warns when a solver
  /// library's parameter names diverge from the body-less builtin anchors.
  /// Off by default (builtins are positional-only, so users never see it);
  /// enabled via the --warn-non-authoritative-names command-line option so
  /// solver implementers can audit their libraries.
  bool warnNonAuthoritativeNames;
  FlatteningOptions fopts;
  std::unordered_map<ASTString, Item*> reverseEnum;
  /// Calls a data file makes that are not data-reshaping builtins. Checked once
  /// `reverseEnum` is complete, because a data file may use an enum constructor
  /// before the assignment that introduces it. Owned here, because type checking may replace them.
  std::vector<Ref<Call>> dataFileCalls;
  std::vector<Ref<Expression>> checkVars;
  std::vector<Ref<Expression>> outputVars;
  OutputSectionStore outputSections;
  std::unordered_map<std::string, int> keyCounters;
  // Maps each FlatZinc variable name originating from an `assume` argument (or the objective)
  // to the expression it should be reported as in an unsatisfiable core (`%%%mzn-core:`): the
  // original assumption expression, a synthesised indexed access (e.g. `asmp(x)[1]`) when the
  // elements are not individually recoverable, or the objective expression. Rendered into the
  // output model only when `assumptionsUsed` is set (i.e. the model contains an `assume`).
  std::unordered_map<ASTString, Ref<Expression>> assumptionExprs;
  bool assumptionsUsed = false;

  // General multipass information
  MultiPassInfo multiPassInfo;

  // Storage for mznpaths
  VarPathStore varPathStore;

  /// The inverse of a par array of strings or integers, used by the builtin versions of
  /// arg_val, arg_val_weak and first_duplicate. Maps each value to the position of its first
  /// occurrence.
  struct ArgValIndex {
    std::unordered_map<ASTString, unsigned int> strings;
    std::unordered_map<IntVal, unsigned int> ints;
    /// The first position whose value occurs at an earlier position, or -1 if there is none
    long long int duplicate = -1;
  };
  /// Map from arrays to their indexes that does not keep the arrays alive. The entry of a
  /// destroyed array is removed when a lookup finds it, and all such entries are removed when
  /// the map has doubled in size.
  class ArgValIndexMap {
  private:
    struct Entry {
      Weak<ArrayLit> al;
      std::shared_ptr<const ArgValIndex> index;
    };
    /// Entries by the hash of their array (which cannot be computed for a destroyed array)
    std::unordered_multimap<size_t, Entry> _m;
    size_t _purgeSize = 16;

  public:
    /// The index of an array that equals \a al, or nullptr
    std::shared_ptr<const ArgValIndex> find(ArrayLit* al) {
      auto range = _m.equal_range(Expression::hash(al));
      for (auto it = range.first; it != range.second;) {
        ArrayLit* key = it->second.al.get();
        if (key == nullptr) {
          it = _m.erase(it);
        } else if (Expression::equal(key, al)) {
          return it->second.index;
        } else {
          ++it;
        }
      }
      return nullptr;
    }
    /// Add \a index for \a al, which must not be in the map
    void insert(ArrayLit* al, std::shared_ptr<const ArgValIndex> index) {
      if (_m.size() >= _purgeSize) {
        for (auto it = _m.begin(); it != _m.end();) {
          it = it->second.al.get() == nullptr ? _m.erase(it) : std::next(it);
        }
        _purgeSize = std::max<size_t>(16, 2 * _m.size());
      }
      _m.emplace(Expression::hash(al), Entry{al, std::move(index)});
    }
  };
  /// The indexes of the arrays seen so far. Arrays are compared structurally, so a copy of an
  /// array reuses its index. Only arrays of literals are cached, since an array with other
  /// elements can evaluate differently each time.
  ArgValIndexMap argValIndexes;
  /// The number of indexes built (cached or not), for testing the cache
  long long int argValIndexBuilds = 0;

protected:
  CSEMap _cseMap;
  Model* _flat;
  bool _failed;
  long long int _ids;
  std::unordered_map<ASTString, ASTString> _reifyMap;
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
  void minId(long long int i) { _ids = std::max(_ids, i + 1); }
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
    if (t.dim() != 0) {
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
    if (t.dim() != 0) {
      const std::vector<unsigned int>& arrayEnumIds = getArrayEnum(typeId);
      typeId = arrayEnumIds[arrayEnumIds.size() - 1];
    }
    return getRecordType(typeId);
  }
  StructType* getStructType(Type t) const {
    assert(t.structBT());
    unsigned int typeId = t.typeId();
    assert(typeId != 0);
    if (t.dim() != 0) {
      const std::vector<unsigned int>& arrayEnumIds = getArrayEnum(typeId);
      typeId = arrayEnumIds[arrayEnumIds.size() - 1];
    }
    return getStructType(typeId, t.bt());
  }
  /// Returns the type of a common tuple type or bot if no such tuple type exists
  Type commonTuple(Type tuple1, Type tuple2, bool ignoreTuple1Dim = false,
                   bool strictEnums = false);
  /// Returns the type of a common record type or 0b if no such tuple type exists
  Type commonRecord(Type record1, Type record2, bool ignoreRecord1Dim = false,
                    bool strictEnums = false);
  /// Returns a record type that merges the fields to two record types
  /// WARNING: This method throws an error when two fields have the same name.
  Type mergeRecord(Type record1, Type record2, const Location& loc);
  /// Returns a tuple type of `tuple1 ++ tuple2'
  Type concatTuple(Type tuple1, Type tuple2);
  /// Get the type t, but remove surrounding transparent tuple if necessary
  Type getTransparentType(Type t) const;
  /// Get the type of e, but remove surrounding transparent tuple if necessary
  Type getTransparentType(const Expression* e) const;
  std::string enumToString(unsigned int enumId, int i);
  /// Check if \a t1 is a subtype of \a t2 (including enumerated types if \a strictEnum is true)
  bool isSubtype(const Type& t1, const Type& t2, bool strictEnum) const;
  bool hasReverseMapper(Id* ident) { return reverseMappers.find(ident) != reverseMappers.end(); }

  void flatAddItem(Arg<Item> i);
  void flatRemoveItem(ConstraintI* i);
  void flatRemoveItem(VarDeclI* i);
  void flatRemoveExpr(Expression* e, Item* i);

  std::tuple<BCtx, bool> annToCtx(VarDecl* vd) const;
  Id* ctxToAnn(BCtx c) const;
  void addCtxAnn(VarDecl* vd, const BCtx& c) const;

  void voAddExp(VarDecl* vd);
  void annotateFromCallStack(Expression* e);
  Ref<ArrayLit> createAnnotationArray(const BCtx& ctx);
  void fail(const std::string& msg = std::string(), const Location& loc = Location());
  bool failed() const;
  Model* flat();
  void swap();
  void swapOutput() { std::swap(model, output); }
  ASTString reifyId(const ASTString& id);
  static ASTString halfReifyId(const ASTString& id);
  bool dumpPath(std::ostream& os, bool force = false);
  /// Path of the current call stack, or nullptr if untracked at this depth.
  PathStore::Path currentPath(bool force = false);
  /// Have the innermost call-stack entry contribute \a p to the path instead of
  /// its own frame, so what is flattened under it continues from there.
  void setPathSplice(PathStore::Path p) { callStack.back().pathSplice = p; }
  int addWarning(const std::string& msg);
  int addWarning(const Location& loc, const std::string& msg, bool dumpStack = true);
  void collectVarDecls(bool b);

  void copyPathMapsAndState(EnvI& env);
  /// Drop state no later pass reads, so that it is freed while they run.
  void releasePassState();
  /// deprecated, use Solns2Out
  std::ostream& evalOutput(std::ostream& os, std::ostream& log);
  Call* surroundingCall() const;

  void cleanupExceptOutput();
  std::default_random_engine& rndGenerator() { return _g; }
  void setRandomSeed(long unsigned int r) {
    _g.seed(static_cast<std::default_random_engine::result_type>(r));
  }
  void cancel() { _cancel = true; }
  void checkCancel() {
    if (_cancel) {  // TODO: Should this be annotated "unlikely"?
      throw Timeout();
    }
  }

  bool outputSectionEnabled(const ASTString& section) const;

  std::string show(Expression* e);
  std::string show(const IntVal& iv, unsigned int enumId);
  std::string show(IntSetVal* isv, unsigned int enumId);
};

inline VarDecl* Ctx::partialityVar(EnvI& env) const {
  return b == C_ROOT ? env.constants.varTrue : nullptr;
}

/// True if \a isv is the index set `1..infinity`, which marks a `list`: only the lower bound is
/// fixed, the length is arbitrary. Actual index sets are only required to be 1-based.
inline bool is_list_index_set(IntSetVal* isv) {
  return isv->size() == 1 && isv->min(0) == 1 && isv->max(0).isPlusInfinity();
}

void set_computed_domain(EnvI& envi, VarDecl* vd, Expression* domain, bool is_computed);
EE flat_exp(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b);
EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b,
              bool doNotFollowChains);

Ref<ArrayLit> field_slice(EnvI& env, StructType* st, ArrayLit* al,
                          const std::vector<std::pair<int, int>>& dims, unsigned int field);
std::vector<Ref<Expression>> field_slices(EnvI& env, Expression* arrExpr);

class CmpExpIdx {
public:
  std::vector<Ref<Expression>>& x;
  CmpExpIdx(std::vector<Ref<Expression>>& x0) : x(x0) {}
  bool operator()(int i, int j) const {
    const long long int i_idn = Expression::isa<Id>(x[i]) ? Expression::cast<Id>(x[i])->idn() : -1;
    const long long int j_idn = Expression::isa<Id>(x[j]) ? Expression::cast<Id>(x[j])->idn() : -1;
    const bool i_is_valid_id = i_idn != -1;
    const bool j_is_valid_id = j_idn != -1;
    if (i_is_valid_id != j_is_valid_id) {
      return i_is_valid_id;
    }
    if (i_is_valid_id && j_is_valid_id) {
      return i_idn < j_idn;
    }
    return Expression::compare(x[i], x[j]) < 0;
  }
};

struct RecordFieldSort {
  bool operator()(const VarDecl* a, const VarDecl* b) const {
    return operator()(a->id()->str(), b->id()->str());
  }
  bool operator()(const std::pair<ASTString, Type>& a, const std::pair<ASTString, Type>& b) const {
    return operator()(a.first, b.first);
  }
  bool operator()(const ASTString& a, const ASTString& b) const {
    return std::strcmp(a.c_str(), b.c_str()) < 0;
  }
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
  typedef Ref<IntSetVal> Domain;
  static Domain evalDomain(EnvI& env, Expression* e) { return eval_intset(env, e); }
  static Ref<Expression> newDomain(Val v) {
    return make<SetLit>(Location().introduce(), IntSetVal::a(v, v));
  }
  static Ref<Expression> newDomain(Val v0, Val v1) {
    return make<SetLit>(Location().introduce(), IntSetVal::a(v0, v1));
  }
  static Ref<Expression> newDomain(Domain d) { return make<SetLit>(Location().introduce(), d); }
  static bool domainContains(const Domain& dom, Val v) { return dom->contains(v); }
  static bool domainEquals(const Domain& dom, Val v) {
    return dom->size() == 1 && dom->min(0) == v && dom->max(0) == v;
  }
  static bool domainEquals(const Domain& dom1, const Domain& dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::equal(d1, d2);
  }
  static bool domainSubset(const Domain& dom1, const Domain& dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::subset(d1, d2);
  }
  static bool domainDisjoint(const Domain& dom1, const Domain& dom2) {
    IntSetRanges d1(dom1);
    IntSetRanges d2(dom2);
    return Ranges::disjoint(d1, d2);
  }
  static bool domainTighter(const Domain& dom, Bounds b) {
    return !b.valid || dom->min() > b.l || dom->max() < b.u;
  }
  static bool domainIntersects(const Domain& dom, Val v0, Val v1) {
    return (v0 > v1) || (!dom->empty() && dom->min(0) <= v1 && v0 <= dom->max(dom->size() - 1));
  }
  static bool domainEmpty(const Domain& dom) { return dom->empty(); }
  static Domain limitDomain(BinOpType bot, const Domain& dom, Val v) {
    IntSetRanges dr(dom);
    Domain ndomain;
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
  static Domain intersectDomain(const Domain& dom, Val v0, Val v1) {
    IntSetRanges dr(dom);
    Ranges::Const<IntVal> c(v0, v1);
    Ranges::Inter<IntVal, IntSetRanges, Ranges::Const<IntVal>> inter(dr, c);
    return IntSetVal::ai(inter);
  }
  static Domain intersectDomain(const Domain& dom0, const Domain& dom1) {
    IntSetRanges dr0(dom0);
    IntSetRanges dr1(dom1);
    Ranges::Inter<IntVal, IntSetRanges, IntSetRanges> inter(dr0, dr1);
    return IntSetVal::ai(inter);
  }
  static Val floorDiv(Val v0, Val v1) {
    return static_cast<long long int>(
        std::floor(static_cast<double>(v0.toInt()) / static_cast<double>(v1.toInt())));
  }
  static Val ceilDiv(Val v0, Val v1) {
    return static_cast<long long int>(
        std::ceil(static_cast<double>(v0.toInt()) / static_cast<double>(v1.toInt())));
  }
  static Ref<IntLit> newLit(Val v) { return IntLit::a(v); }
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
  typedef Ref<FloatSetVal> Domain;
  static Domain evalDomain(EnvI& env, Expression* e) { return eval_floatset(env, e); }

  static Ref<Expression> newDomain(Val v) {
    return make<SetLit>(Location().introduce(), FloatSetVal::a(v, v));
  }
  static Ref<Expression> newDomain(Val v0, Val v1) {
    return make<SetLit>(Location().introduce(), FloatSetVal::a(v0, v1));
  }
  static Ref<Expression> newDomain(Domain d) { return make<SetLit>(Location().introduce(), d); }
  static bool domainContains(const Domain& dom, Val v) { return dom->contains(v); }
  static bool domainEquals(const Domain& dom, Val v) {
    return dom->size() == 1 && dom->min(0) == v && dom->max(0) == v;
  }

  static bool domainTighter(const Domain& dom, Bounds b) {
    return !b.valid || dom->min() > b.l || dom->max() < b.u;
  }
  static bool domainIntersects(const Domain& dom, Val v0, Val v1) {
    return (v0 > v1) || (!dom->empty() && dom->min(0) <= v1 && v0 <= dom->max(dom->size() - 1));
  }
  static bool domainEmpty(const Domain& dom) { return dom->empty(); }

  static bool domainEquals(const Domain& dom1, const Domain& dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::equal(d1, d2);
  }
  static bool domainSubset(const Domain& dom1, const Domain& dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::subset(d1, d2);
  }
  static bool domainDisjoint(const Domain& dom1, const Domain& dom2) {
    FloatSetRanges d1(dom1);
    FloatSetRanges d2(dom2);
    return Ranges::disjoint(d1, d2);
  }
  static Domain intersectDomain(const Domain& dom, Val v0, Val v1) {
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

  static Domain limitDomain(BinOpType bot, const Domain& dom, Val v) {
    FloatSetRanges dr(dom);
    Domain ndomain;
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
  static Ref<FloatLit> newLit(Val v) { return FloatLit::a(v); }
  static FloatVal v(const FloatLit* fl) { return FloatLit::v(fl); }
};

template <class Lit>
void simplify_lin(std::vector<typename LinearTraits<Lit>::Val>& c, std::vector<Ref<Expression>>& x,
                  typename LinearTraits<Lit>::Val& d) {
  std::vector<int> idx(c.size());
  for (auto i = static_cast<int>(idx.size()); i--;) {
    idx[i] = i;
    Expression* e = follow_id_to_decl(x[i]);
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
    if (Lit* il = Expression::dynamicCast<Lit>(x[idx[ci]])) {
      d += c[idx[ci]] * LinearTraits<Lit>::v(il);
      c[idx[ci]] = 0;
    } else {
      break;
    }
  }
  for (unsigned int i = ci + 1; i < x.size(); i++) {
    if (Expression::equal(x[idx[i]], x[idx[ci]])) {
      c[idx[ci]] += c[idx[i]];
      c[idx[i]] = 0;
    } else if (Lit* il = Expression::dynamicCast<Lit>(x[idx[i]])) {
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

/// Helper function used to recursively change the context for (to be reified) functionally defined
/// vardecls
void cse_result_change_ctx(EnvI& env, Expression* cseRes, BCtx newCtx);

}  // namespace MiniZinc
