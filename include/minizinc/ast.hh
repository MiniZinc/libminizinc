/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/aststring.hh>
#include <minizinc/astvec.hh>
#include <minizinc/gc.hh>
#include <minizinc/type.hh>
#include <minizinc/values.hh>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

namespace MiniZinc {

namespace find_pointer_type {

// Functions to implement static selection of suitable integer type
// that can hold pointer values.

template <bool IsSuitable, typename T, typename... Rest>
struct find_pointer_type;

template <typename T, typename... Rest>
struct find_pointer_type<true, T, Rest...> {
  using type = T;
};

template <typename T, typename Next, typename... Rest>
struct find_pointer_type<false, T, Next, Rest...> {
  using type = typename find_pointer_type<(sizeof(Next) >= sizeof(void*)), Next, Rest...>::type;
};

template <typename T>
struct find_pointer_type<false, T> {};
}  // namespace find_pointer_type

#ifdef UINTPTR_MAX
// Use uintptr_t if available
typedef uintptr_t mzn_uintptr_t;
#else
// Otherwise, try size_t, unsigned long, unsigned long long (in that order)
typedef mzn_uintptr_t =
    typename find_pointer_type::find_pointer_type<(sizeof(size_t) >= sizeof(void*)), size_t,
                                                  unsigned long, unsigned long long>::type;
#endif

#ifdef INTPTR_MAX
// Use intptr_t if available
typedef intptr_t mzn_intptr_t;
#else
// Otherwise, try ptrdiff_t, long, long long (in that order)
typedef mzn_intptr_t =
    typename find_pointer_type::find_pointer_type<(sizeof(ptrdiff_t) >= sizeof(void*)), ptrdiff_t,
                                                  long, long long>::type;
#endif

static_assert(sizeof(mzn_uintptr_t) >= sizeof(void*),
              "No suitable unsigned integer type found to hold pointer values");

static_assert(sizeof(mzn_intptr_t) >= sizeof(void*),
              "No suitable signed integer type found to hold pointer values");

class IntLit;
class FloatLit;
class SetLit;
class BoolLit;
class StringLit;
class Id;
class AnonVar;
class ArrayLit;
class ArrayAccess;
class Comprehension;
class ITE;
class BinOp;
class UnOp;
class Call;
class VarDecl;
class Let;
class TypeInst;

class Item;
class FunctionI;

class ExpressionSet;
class ExpressionSetIter;

/// %Location of an expression used during parsing
class ParserLocation {
protected:
  /// Source code file name
  ASTString _filename;
  /// Line where expression starts
  unsigned int _firstLine;
  /// Line where expression ends
  unsigned int _lastLine;
  /// Column where expression starts
  unsigned int _firstColumn;
  /// Column where expression ends
  unsigned int _lastColumn;

public:
  /// Construct empty location
  ParserLocation() : _firstLine(1), _lastLine(1), _firstColumn(0), _lastColumn(0) {}

  /// Construct location
  ParserLocation(const ASTString& filename, unsigned int first_line, unsigned int first_column,
                 unsigned int last_line, unsigned int last_column)
      : _filename(filename),
        _firstLine(first_line),
        _lastLine(last_line),
        _firstColumn(first_column),
        _lastColumn(last_column) {}

  ASTString filename() const { return _filename; }
  void filename(const ASTString& f) { _filename = f; }

  unsigned int firstLine() const { return _firstLine; }
  void firstLine(unsigned int l) { _firstLine = l; }

  unsigned int lastLine() const { return _lastLine; }
  void lastLine(unsigned int l) { _lastLine = l; }

  unsigned int firstColumn() const { return _firstColumn; }
  void firstColumn(unsigned int c) { _firstColumn = c; }

  unsigned int lastColumn() const { return _lastColumn; }
  void lastColumn(unsigned int c) { _lastColumn = c; }

  std::string toString() const {
    std::ostringstream oss;
    oss << _filename << ":" << _firstLine << "." << _firstColumn;
    if (_firstLine != _lastLine) {
      oss << "-" << _lastLine << "." << _lastColumn;
    } else if (_firstColumn != _lastColumn) {
      oss << "-" << _lastColumn;
    }
    return oss.str();
  }
};

/// %Location of an expression in the source code
class Location {
protected:
  /// Internal representation of a Location
  /// Layout depends on sizeof pointer and arguments
  /// 32 bit pointers:
  ///   Layout 1: filename (32 bit), first line (8 bit), last line-first line (7 bit), first column
  ///   (6 bit), last column (7 bit) Layout 2: filename (32 bit), 4 IntLit for the lines/columns
  /// 64 bit pointers:
  ///   Layout 1: filename (64 bit), first line (20 bit), last line-first line (20 bit), first
  ///   column (10 bit), last column (10 bit) Layout 2: filename (64 bit), 4 IntLit for the
  ///   lines/columns
  class LocVec : public ASTVec {
  protected:
    LocVec(const ASTString& filename, unsigned int first_line, unsigned int first_column,
           unsigned int last_line, unsigned int last_column);
    LocVec(const ASTString& filename, IntVal combined);

  public:
    static LocVec* a(const ASTString& filename, unsigned int first_line, unsigned int first_column,
                     unsigned int last_line, unsigned int last_column);
    void mark() {
      _gcMark = 1;
      if (_data[0] != nullptr) {
        static_cast<ASTStringData*>(_data[0])->mark();
      }
    }

    ASTString filename() const;
    unsigned int firstLine() const;
    unsigned int lastLine() const;
    unsigned int firstColumn() const;
    unsigned int lastColumn() const;
  };

  union LI {
    LocVec* lv;
    mzn_uintptr_t t;
  } _locInfo;

  LocVec* lv() const {
    LI li = _locInfo;
    li.t &= ~static_cast<mzn_uintptr_t>(1);
    return li.lv;
  }

public:
  /// Construct empty location
  Location() { _locInfo.lv = nullptr; }

  /// Construct location
  Location(const ASTString& filename, unsigned int first_line, unsigned int first_column,
           unsigned int last_line, unsigned int last_column) {
    if (last_line < first_line) {
      throw InternalError("invalid location");
    }
    _locInfo.lv = LocVec::a(filename, first_line, first_column, last_line, last_column);
  }

  Location(const ParserLocation& loc) {
    _locInfo.lv = LocVec::a(loc.filename(), loc.firstLine(), loc.firstColumn(), loc.lastLine(),
                            loc.lastColumn());
  }

  /// Return string representation
  std::string toString() const;

  /// Return JSON representation
  std::string toJSON() const;

  /// Return filename
  ASTString filename() const { return lv() != nullptr ? lv()->filename() : ASTString(); }

  /// Return first line number
  unsigned int firstLine() const { return lv() != nullptr ? lv()->firstLine() : 0; }

  /// Return last line number
  unsigned int lastLine() const { return lv() != nullptr ? lv()->lastLine() : 0; }

  /// Return first column number
  unsigned int firstColumn() const { return lv() != nullptr ? lv()->firstColumn() : 0; }

  /// Return last column number
  unsigned int lastColumn() const { return lv() != nullptr ? lv()->lastColumn() : 0; }

  /// Return whether location is introduced by the compiler
  bool isIntroduced() const { return _locInfo.lv == nullptr || ((_locInfo.t & 1) != 0); }

  /// Mark as alive for garbage collection
  void mark() const;

  /// Return location with introduced flag set
  Location introduce() const;

  /// Location used for un-allocated expressions
  static Location nonalloc;

  /// Return true if this is a location for an unallocated expression
  bool isNonAlloc() const { return this == &Location::nonalloc; }

  ParserLocation parserLocation() const {
    return ParserLocation(filename(), firstLine(), firstColumn(), lastLine(), lastColumn());
  }
};

/// Output operator for locations
template <class Char, class Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os,
                                             const Location& loc) {
  std::basic_ostringstream<Char, Traits> s;
  s.copyfmt(os);
  s.width(0);
  if (loc.filename().empty()) {
    s << "unknown file";
  } else {
    s << loc.filename();
  }
  s << ":" << loc.firstLine() << "." << loc.firstColumn();
  if (loc.firstLine() != loc.lastLine()) {
    s << "-" << loc.lastLine() << "." << loc.lastColumn();
  } else if (loc.firstColumn() != loc.lastColumn()) {
    s << "-" << loc.lastColumn();
  }

  return os << s.str();
}

/**
 * \brief Annotations
 */
class Annotation {
private:
  ExpressionSet* _s;

  /// Delete
  Annotation(const Annotation&);
  /// Delete
  Annotation& operator=(const Annotation&);

public:
  Annotation() : _s(nullptr) {}
  ~Annotation();
  bool contains(Expression* e) const;
  bool containsCall(const ASTString& id) const;
  bool isEmpty() const;
  unsigned int size() const;
  ExpressionSetIter begin() const;
  ExpressionSetIter end() const;
  void add(Expression* e);
  void add(std::vector<Expression*> e);
  void remove(Expression* e);
  void removeCall(const ASTString& id);
  void clear();
  void merge(const Annotation& ann);
  Call* getCall(const ASTString& id) const;

  static Annotation empty;
};

/// returns the Annotation specified by the string; returns NULL if not exists
Expression* get_annotation(const Annotation& ann, const std::string& str);

/// returns the Annotation specified by the string; returns NULL if not exists
Expression* get_annotation(const Annotation& ann, const ASTString& str);

/**
 * \brief Base class for expressions
 */
class Expression : public ASTNode {
protected:
  /// The %MiniZinc type of the expression
  Type _type;
  /// The location of the expression
  Location _loc;
  /// The annotations
  Annotation _ann;
  /// The hash value of the expression
  size_t _hash;

public:
  /// Identifier of the concrete expression type
  enum ExpressionId {
    E_INTLIT = ASTNode::NID_END + 1,
    E_FLOATLIT,
    E_SETLIT,
    E_BOOLLIT,
    E_STRINGLIT,
    E_ID,
    E_ANON,
    E_ARRAYLIT,
    E_ARRAYACCESS,
    E_FIELDACCESS,
    E_COMP,
    E_ITE,
    E_BINOP,
    E_UNOP,
    E_CALL,
    E_VARDECL,
    E_LET,
    E_TI,
    E_TIID,
    EID_END = E_TIID
  };

  static bool isUnboxedVal(const Expression* e);
  static bool isUnboxedInt(const Expression* e);
  static bool isUnboxedFloatVal(const Expression* e);

  static ExpressionId eid(const Expression* e);

  static const Location& loc(const Expression* e);
  static const Location& locDefault(const Expression* e0, const Expression* e1);
  static const Location& locDefault(const Expression* e0, const Location& ldef);
  static void loc(Expression* e, const Location& l) {
    if (!Expression::isUnboxedVal(e)) {
      e->_loc = l;
    }
  }
  static const Type& type(const Expression* e);
  static void type(Expression* e, const Type& t);

protected:
  /// Combination function for hash values
  void combineHash(size_t h) { _hash ^= h + 0x9e3779b9 + (_hash << 6) + (_hash >> 2); }
  /// Combination function for hash values
  static size_t combineHash(size_t seed, size_t h) {
    seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }

  /// Compute base hash value
  void initHash() { _hash = combineHash(0, _id); }

  /// Check if \a e0 and \a e1 are equal
  static bool equalInternal(const Expression* e0, const Expression* e1);

#ifdef __EMSCRIPTEN__
  __attribute__((noinline))
#endif
  static mzn_uintptr_t
  asPtrDiff(const Expression* e) {
    return reinterpret_cast<mzn_uintptr_t>(e);
  }

  /// Constructor
  Expression(const Location& loc, const ExpressionId& eid, const Type& t)
      : ASTNode(eid), _type(t), _loc(loc) {}

public:
  static IntVal unboxedIntToIntVal(const Expression* e) {
    assert(Expression::isUnboxedInt(e));
    if (sizeof(double) <= sizeof(void*)) {
      unsigned long long int i = Expression::asPtrDiff(e) & ~static_cast<mzn_uintptr_t>(7);
      bool pos = ((Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(4)) == 0);
      if (pos) {
        return static_cast<long long int>(i >> 3);
      }
      return -(static_cast<long long int>(i >> 3));
    }
    unsigned long long int i = Expression::asPtrDiff(e) & ~static_cast<mzn_uintptr_t>(3);
    bool pos = ((Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(2)) == 0);
    if (pos) {
      return static_cast<long long int>(i >> 2);
    }
    return -(static_cast<long long int>(i >> 2));
  }
  static IntLit* intToUnboxedInt(long long int i) {
    static const unsigned int pointerBits = sizeof(void*) * 8;
    if (sizeof(double) <= sizeof(void*)) {
      static const long long int maxUnboxedVal =
          (static_cast<long long int>(1) << (pointerBits - 3)) - static_cast<long long int>(1);
      if (i < -maxUnboxedVal || i > maxUnboxedVal) {
        return nullptr;
      }
      long long int j = i < 0 ? -i : i;
      mzn_uintptr_t ubi_p = (static_cast<mzn_uintptr_t>(j) << 3) | static_cast<mzn_uintptr_t>(2);
      if (i < 0) {
        ubi_p = ubi_p | static_cast<mzn_uintptr_t>(4);
      }
      return reinterpret_cast<IntLit*>(ubi_p);
    }
    static const long long int maxUnboxedVal =
        (static_cast<long long int>(1) << (pointerBits - 2)) - static_cast<long long int>(1);
    if (i < -maxUnboxedVal || i > maxUnboxedVal) {
      return nullptr;
    }
    long long int j = i < 0 ? -i : i;
    mzn_uintptr_t ubi_p = (static_cast<mzn_uintptr_t>(j) << 2) | static_cast<mzn_uintptr_t>(1);
    if (i < 0) {
      ubi_p = ubi_p | static_cast<mzn_uintptr_t>(2);
    }
    return reinterpret_cast<IntLit*>(ubi_p);
  }
  static FloatVal unboxedFloatToFloatVal(const Expression* e) {
    assert(Expression::isUnboxedFloatVal(e));
    union {
      double d;
      uint64_t bits;
      const Expression* p;
    } _u;
    _u.p = e;
    _u.bits = _u.bits >> 1;
    uint64_t exponent = (_u.bits & (static_cast<uint64_t>(0x3FF) << 52)) >> 52;
    if (exponent != 0) {
      exponent += 512;  // reconstruct original bias of 1023
    }
    uint64_t sign = ((_u.bits & (static_cast<uint64_t>(1) << 62)) != 0U ? 1 : 0);
    _u.bits = (sign << 63) | (exponent << 52) | (_u.bits & static_cast<uint64_t>(0xFFFFFFFFFFFFF));
    return _u.d;
  }
  static FloatLit* doubleToUnboxedFloatVal(double d) {
    if (sizeof(double) > sizeof(void*)) {
      return nullptr;
    }
    union {
      double d;
      uint64_t bits;
      FloatLit* p;
    } _u;
    _u.d = d;

    uint64_t exponent = (_u.bits & (static_cast<uint64_t>(0x7FF) << 52)) >> 52;
    if (exponent != 0) {
      if (exponent < 513 || exponent > 1534) {
        return nullptr;  // exponent doesn't fit in 10 bits
      }
      exponent -= 512;  // make exponent fit in 10 bits, with bias 511
    }
    bool sign = (_u.bits & (static_cast<uint64_t>(1) << 63)) != 0;

    _u.bits = _u.bits &
              ~(static_cast<uint64_t>(0x7FF) << 52);  // mask out top 11 bits (previously exponent)
    _u.bits = (_u.bits << 1) | 1U;                    // shift by one bit and add tag for double
    _u.bits = _u.bits | (static_cast<uint64_t>(sign) << 63) | (exponent << 53);
    return _u.p;
  }

  static bool isTagged(Expression* e) {
    // only bit 2 is set
    if (Expression::isUnboxedVal(e)) {
      return false;
    }
    if (sizeof(double) <= sizeof(void*)) {
      return (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(7)) == 4;
    }
    return (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(3)) == 2;
  }

  static Expression* tag(Expression* e) {
    assert(!Expression::isUnboxedVal(e));
    if (sizeof(double) <= sizeof(void*)) {
      return reinterpret_cast<Expression*>(Expression::asPtrDiff(e) |
                                           static_cast<mzn_uintptr_t>(4));
    }
    return reinterpret_cast<Expression*>(Expression::asPtrDiff(e) | static_cast<mzn_uintptr_t>(2));
  }
  static Expression* untag(Expression* e) {
    if (Expression::isUnboxedVal(e)) {
      return e;
    }
    if (sizeof(double) <= sizeof(void*)) {
      return reinterpret_cast<Expression*>(Expression::asPtrDiff(e) &
                                           ~static_cast<mzn_uintptr_t>(4));
    }
    return reinterpret_cast<Expression*>(Expression::asPtrDiff(e) & ~static_cast<mzn_uintptr_t>(2));
  }

  /// Test if expression is of type \a T
  template <class T>
  static bool isa(const Expression* e);

  /// Cast expression to type \a T*
  template <class T>
  static T* cast(Expression* e);
  /// Cast expression to type \a const T*
  template <class T>
  static const T* cast(const Expression* e);
  /// Cast expression to type \a T* or NULL if types do not match
  template <class T>
  static T* dynamicCast(Expression* e);
  /// Cast expression to type \a const T* or NULL if types do not match
  template <class T>
  static const T* dynamicCast(const Expression* e);

  /// Add annotation \a ann to the expression
  static void addAnnotation(Expression* e, Expression* ann);

  /// Add annotation \a ann to the expression
  static void addAnnotations(Expression* e, const std::vector<Expression*>& ann);

  static const Annotation& ann(const Expression* e) {
    return Expression::isUnboxedVal(e) ? Annotation::empty : e->_ann;
  }
  static Annotation& ann(Expression* e) {
    return Expression::isUnboxedVal(e) ? Annotation::empty : e->_ann;
  }

  /// Return hash value of \a e
  static size_t hash(const Expression* e);

  /// Check if \a e0 and \a e1 are equal
  static bool equal(const Expression* e0, const Expression* e1);

  /// Mark \a e as alive for garbage collection
  static void mark(Expression* e);
  /// Check if \e is marked as alive for garbage collection
  static bool hasMark(Expression* e);
};

inline bool Expression::isUnboxedVal(const Expression* e) {
  if (sizeof(double) <= sizeof(void*)) {
    // bit 1 or bit 0 is set
    return (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(3)) != 0;
  }  // bit 0 is set
  return (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(1)) != 0;
}
inline bool Expression::isUnboxedInt(const Expression* e) {
  if (sizeof(double) <= sizeof(void*)) {
    // bit 1 is set, bit 0 is not set
    return (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(3)) == 2;
  }  // bit 0 is set
  return (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(1)) == 1;
}
inline bool Expression::isUnboxedFloatVal(const Expression* e) {
  // bit 0 is set (and doubles fit inside pointers)
  return (sizeof(double) <= sizeof(void*)) &&
         (Expression::asPtrDiff(e) & static_cast<mzn_uintptr_t>(1)) == 1;
}
inline Expression::ExpressionId Expression::eid(const Expression* e) {
  return Expression::isUnboxedInt(e)        ? E_INTLIT
         : Expression::isUnboxedFloatVal(e) ? E_FLOATLIT
                                            : static_cast<ExpressionId>(e->_id);
}
inline const Location& Expression::loc(const Expression* e) {
  return Expression::isUnboxedVal(e) ? Location::nonalloc : e->_loc;
}
inline const Location& Expression::locDefault(const Expression* e0, const Expression* e1) {
  return Expression::isUnboxedVal(e0) ? loc(e1) : e0->_loc;
}
inline const Location& Expression::locDefault(const Expression* e0, const Location& ldef) {
  return Expression::isUnboxedVal(e0) ? ldef : e0->_loc;
}
inline const Type& Expression::type(const Expression* e) {
  return Expression::isUnboxedInt(e)        ? Type::unboxedint
         : Expression::isUnboxedFloatVal(e) ? Type::unboxedfloat
                                            : e->_type;
}
inline size_t Expression::hash(const Expression* e) {
  return e == nullptr
             ? 0
             : (Expression::isUnboxedInt(e)        ? Expression::unboxedIntToIntVal(e).hash()
                : Expression::isUnboxedFloatVal(e) ? Expression::unboxedFloatToFloatVal(e).hash()
                                                   : e->_hash);
}

template <class T>
inline bool Expression::isa(const Expression* e) {
  if (nullptr == e) {
    throw InternalError("isa: nullptr");
  }
  return Expression::isUnboxedInt(e)        ? T::eid == E_INTLIT
         : Expression::isUnboxedFloatVal(e) ? T::eid == E_FLOATLIT
                                            : e->_id == T::eid;
}

template <class T>
inline T* Expression::cast(Expression* e) {
  if (e == nullptr) {
    return nullptr;
  }
  assert(isa<T>(e));
  return static_cast<T*>(e);
}
template <class T>
inline const T* Expression::cast(const Expression* e) {
  if (e == nullptr) {
    return nullptr;
  }
  assert(isa<T>(e));
  return static_cast<const T*>(e);
}
template <class T>
inline T* Expression::dynamicCast(Expression* e) {
  return e == nullptr || !isa<T>(e) ? nullptr : static_cast<T*>(e);
}
template <class T>
inline const T* Expression::dynamicCast(const Expression* e) {
  return e == nullptr || !isa<T>(e) ? nullptr : static_cast<const T*>(e);
}

class BoxedExpression : public Expression {
public:
  BoxedExpression(const Location& loc, const ExpressionId& eid, const Type& t)
      : Expression(loc, eid, t) {}

  const Type& type() const { return Expression::type(this); }
  void type(const Type& t) { Expression::type(this, t); }
};

/// \brief Integer literal expression
class IntLit : public Expression {
protected:
  /// The value of this expression
  IntVal _v;
  /// Constructor
  IntLit(const Location& loc, IntVal v);

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_INTLIT;
  /// Access value
  static IntVal v(const IntLit* il) {
    return Expression::isUnboxedInt(il) ? Expression::unboxedIntToIntVal(il) : il->_v;
  }
  /// Recompute hash value
  void rehash();
  /// Allocate literal
  static IntLit* a(IntVal v);
  /// Allocate literal for enumerated type (only used internally for generators)
  static IntLit* aEnum(IntVal v, unsigned int enumId);
};
/// \brief Float literal expression
class FloatLit : public Expression {
protected:
  /// The value of this expression
  FloatVal _v;
  /// Constructor
  FloatLit(const Location& loc, FloatVal v);

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_FLOATLIT;
  /// Access value
  static FloatVal v(const FloatLit* fl) {
    return Expression::isUnboxedFloatVal(fl) ? Expression::unboxedFloatToFloatVal(fl) : fl->_v;
  }
  /// Recompute hash value
  void rehash();
  /// Allocate literal
  static FloatLit* a(FloatVal v);
};
/// \brief Set literal expression
class SetLit : public BoxedExpression {
protected:
  /// The value of this expression
  ASTExprVec<Expression> _v;
  union {
    /// A range-list based representation for an integer set, or NULL
    IntSetVal* isv;
    /// A range-list based representation for an float set, or NULL
    FloatSetVal* fsv;
  } _u;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_SETLIT;
  /// Construct set \$f\{v1,\dots,vn\}\$f
  SetLit(const Location& loc, const std::vector<Expression*>& v);
  /// Construct set \$f\{v1,\dots,vn\}\$f
  SetLit(const Location& loc, const ASTExprVec<Expression>& v);
  /// Construct set
  SetLit(const Location& loc, IntSetVal* isv);
  /// Construct set
  SetLit(const Location& loc, FloatSetVal* fsv);
  /// Access value
  ASTExprVec<Expression> v() const { return _v; }
  /// Set value
  void v(const ASTExprVec<Expression>& val) { _v = val; }
  /// Access integer set value if present
  IntSetVal* isv() const {
    return (type().bt() == Type::BT_INT || type().bt() == Type::BT_BOOL) ? _u.isv : nullptr;
  }
  /// Set integer set value
  void isv(IntSetVal* val) { _u.isv = val; }
  /// Access float set value if present
  FloatSetVal* fsv() const { return type().bt() == Type::BT_FLOAT ? _u.fsv : nullptr; }
  /// Check if int set value or float set value is present
  bool evaluated() const { return _u.isv != nullptr; }
  /// Set integer set value
  void fsv(FloatSetVal* val) { _u.fsv = val; }
  /// Recompute hash value
  void rehash();
};
/// \brief Boolean literal expression
class BoolLit : public BoxedExpression {
protected:
  /// The value of this expression
  bool _v;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_BOOLLIT;
  /// Constructor
  BoolLit(const Location& loc, bool v);
  /// Access value
  bool v() const { return _v; }
  /// Recompute hash value
  void rehash();
};
/// \brief String literal expression
class StringLit : public BoxedExpression {
protected:
  /// The value of this expression
  ASTString _v;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_STRINGLIT;
  /// Constructor
  StringLit(const Location& loc, const std::string& v);
  /// Constructor
  StringLit(const Location& loc, const ASTString& v);
  /// Access value
  ASTString v() const { return _v; }
  /// Set value
  void v(const ASTString& val) { _v = val; }
  /// Recompute hash value
  void rehash();
};
/// \brief Identifier expression
class Id : public BoxedExpression {
protected:
  /// The string identifier
  union {
    /// Identifier of called predicate or function
    ASTString val;
    /// The predicate or function declaration (or NULL)
    void* idn;
  } _vOrIdn = {nullptr};
  /// The declaration corresponding to this identifier (may be NULL)
  Expression* _decl;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_ID;
  /// Constructor (\a decl may be NULL)
  Id(const Location& loc, const std::string& v, VarDecl* decl);
  /// Constructor (\a decl may be NULL)
  Id(const Location& loc, const ASTString& v, VarDecl* decl);
  /// Constructor (\a decl may be NULL)
  Id(const Location& loc, long long int idn, VarDecl* decl);
  /// Access identifier
  ASTString v() const;
  bool hasStr() const {
    return (reinterpret_cast<mzn_uintptr_t>(_vOrIdn.idn) & static_cast<mzn_uintptr_t>(1)) == 0;
  }
  /// Set identifier
  void v(const ASTString& val) { _vOrIdn.val = val; }
  /// Access identifier number
  long long int idn() const;
  /// Set identifier number
  void idn(long long int n) {
    auto n1 = static_cast<mzn_uintptr_t>(n) << 1;
    _vOrIdn.idn = reinterpret_cast<void*>(n1 | static_cast<mzn_uintptr_t>(1));
    rehash();
  }
  /// Return identifier or X_INTRODUCED plus identifier number
  ASTString str() const;
  /// Access declaration
  VarDecl* decl() const {
    Expression* d = _decl;
    while ((d != nullptr) && Expression::isa<Id>(d)) {
      d = Expression::cast<Id>(d)->_decl;
    }
    return Expression::cast<VarDecl>(d);
  }
  /// Set declaration
  void decl(VarDecl* d);
  /// Redirect to another Id \a id
  void redirect(Id* id) {
    assert(_decl == nullptr || Expression::isa<VarDecl>(_decl));
    _decl = id;
    rehash();
  }
  /// Get the identifier or declaration this identifier directly points to
  Expression* destination() const { return _decl; }
  /// Recompute hash value
  void rehash();
  /// Levenshtein distance to \a other identifier
  int levenshteinDistance(Id* other) const;
};
/// \brief Type-inst identifier expression
class TIId : public BoxedExpression {
protected:
  /// The string identifier
  ASTString _v;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_TIID;
  /// Constructor
  TIId(const Location& loc, const std::string& v);
  /// Constructor
  TIId(const Location& loc, const ASTString& v);
  /// Access identifier
  ASTString v() const { return _v; }
  /// Set identifier
  void v(const ASTString& val) { _v = val; }
  /// Check whether it is an enum identifier (starting with two $ signs)
  bool isEnum() const { return _v.c_str()[0] == '$'; }
  /// Recompute hash value
  void rehash();
};
/// \brief Anonymous variable expression
class AnonVar : public BoxedExpression {
public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_ANON;
  /// Constructor
  AnonVar(const Location& loc);
  /// Recompute hash value
  void rehash();
};
/// \brief Array literal expression
class ArrayLit : public BoxedExpression {
  friend class Expression;

protected:
  /// The array
  union {
    /// An expression vector (if _flag2==false)
    ASTExprVecO<Expression*>* v;
    /// Another array literal (if _flag2==true)
    ArrayLit* al;
  } _u;
  /// The declared array dimensions
  // If _flag2 is true, then this is an array view. In that case,
  // the _dims array holds the sliced dimensions
  ASTIntVec _dims;
  /// Set compressed vector (initial repetitions are removed)
  void compress(const std::vector<Expression*>& v, const std::vector<int>& dims);
  /// Whether this is an array or a tuple
  enum ArrayLitType { AL_ARRAY, AL_TUPLE };

public:
  /// Index conversion from slice to original
  unsigned int origIdx(unsigned int i) const;
  /// Get element \a i of a sliced array
  Expression* getSlice(unsigned int i) const;
  /// Set element \a i of a sliced array
  void setSlice(unsigned int i, Expression* e);

  /// The identifier of this expression type
  static const ExpressionId eid = E_ARRAYLIT;
  /// Constructor
  ArrayLit(const Location& loc, const std::vector<Expression*>& v,
           const std::vector<std::pair<int, int> >& dims);
  /// Constructor (existing content)
  ArrayLit(const Location& loc, ArrayLit* v, const std::vector<std::pair<int, int> >& dims);
  /// Constructor (one-dimensional, existing content)
  ArrayLit(const Location& loc, ArrayLit* v);
  /// Constructor (one-dimensional)
  ArrayLit(const Location& loc, const std::vector<Expression*>& v);
  /// Constructor (two-dimensional)
  ArrayLit(const Location& loc, const std::vector<std::vector<Expression*> >& v);
  /// Constructor for slices
  ArrayLit(const Location& loc, ArrayLit* v, const std::vector<std::pair<int, int> >& dims,
           const std::vector<std::pair<int, int> >& slice);
  /// Constructor (one-dimensional)
  ArrayLit(const Location& loc, const std::vector<KeepAlive>& v);
  /// Construct tuple
  static ArrayLit* constructTuple(const Location& loc, const std::vector<Expression*>& v);
  /// Construct tuple (existing content)
  static ArrayLit* constructTuple(const Location& loc, ArrayLit* v);
  /// Recompute hash value
  void rehash();

  // The following methods are only used for copying

  /// Access value
  ASTExprVec<Expression> getVec() const {
    assert(!_flag2);
    return _u.v;
  }
  /// Set value
  void setVec(const ASTExprVec<Expression>& val) {
    assert(!_flag2);
    _u.v = val.vec();
  }
  /// Get underlying array (if this is an array slice) or NULL
  ArrayLit* getSliceLiteral() const { return _flag2 ? _u.al : nullptr; }
  /// Get underlying _dims vector
  ASTIntVec dimsInternal() const { return _dims; }

  /// Return number of dimensions
  unsigned int dims() const;
  /// Return minimum index of dimension \a i
  int min(unsigned int i) const;
  /// Return maximum index of dimension \a i
  int max(unsigned int i) const;
  /// Return the length of the array
  unsigned int length() const;
  /// Turn into 1d array (only used at the end of flattening)
  void make1d();
  /// Check if this array was produced by flattening
  bool flat() const { return _flag1; }
  /// Set whether this array was produced by flattening
  void flat(bool b) { _flag1 = b; }
  /// Return size of underlying array
  unsigned int size() const { return (_flag2 || _u.v->flag()) ? length() : _u.v->size(); }
  /// Return whether array is empty
  bool empty() const { return size() == 0; }
  /// Access element \a i
  Expression* operator[](unsigned int i) const;
  /// Set element \a i
  void set(unsigned int i, Expression* e) {
    if (_flag2 || _u.v->flag()) {
      setSlice(i, e);
    } else {
      (*_u.v)[i] = e;
    }
  }
  bool isTuple() const { return static_cast<ArrayLitType>(_secondaryId) == AL_TUPLE; }
};
/// Access element \a i
inline Expression* ArrayLit::operator[](unsigned int i) const {
  return (_flag2 || _u.v->flag()) ? getSlice(i) : (*_u.v)[i];
}

/// \brief Array access expression
class ArrayAccess : public BoxedExpression {
protected:
  /// The array to access
  Expression* _v;
  /// The indexes (for all array dimensions)
  ASTExprVec<Expression> _idx;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_ARRAYACCESS;
  /// Constructor
  ArrayAccess(const Location& loc, Expression* v, const std::vector<Expression*>& idx);
  /// Constructor
  ArrayAccess(const Location& loc, Expression* v, const ASTExprVec<Expression>& idx);
  /// Access value
  Expression* v() const { return _v; }
  /// Set value
  void v(Expression* val) { _v = val; }
  /// Access index sets
  ASTExprVec<Expression> idx() const { return _idx; }
  /// Set index sets
  void idx(const ASTExprVec<Expression>& idx) { _idx = idx; }
  /// Recompute hash value
  void rehash();
};

/// \brief Field access expression
class FieldAccess : public BoxedExpression {
protected:
  /// The structured type to access
  Expression* _v;
  /// The field being accessed (must be IntLit for tuples)
  Expression* _field;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_FIELDACCESS;
  /// Constructor
  FieldAccess(const Location& loc, Expression* v, Expression* field);
  /// Access value
  Expression* v() const { return _v; }
  /// Set value
  void v(Expression* val) { _v = val; }
  /// Field to be accessed
  Expression* field() const { return _field; }
  /// Set field to be accessed
  void field(Expression* field) { _field = field; }
  /// Recompute hash value
  void rehash();
};

/**
 * \brief Generators for comprehensions
 *
 * A generator consists of a list of variable declarations, one for
 * each generated variable, and the expression to generate. E.g.,
 * the Zinc expression [ x[i,j,k] | i,j in 1..10, k in 1..5] contains
 * two generators. The first one has variable declarations for i and j
 * and the expression 1..10, and the second one has a variable declaration
 * for k and the expression 1..5.
 *
 */
class Generator {
  friend class Comprehension;

protected:
  /// Variable declarations
  std::vector<VarDecl*> _v;
  /// in-expression
  Expression* _in;
  /// where-expression
  Expression* _where;

public:
  /// Allocate
  Generator(const std::vector<std::string>& v, Expression* in, Expression* where);
  /// Allocate
  Generator(const std::vector<ASTString>& v, Expression* in, Expression* where);
  /// Allocate
  Generator(const std::vector<Id*>& v, Expression* in, Expression* where);
  /// Allocate
  Generator(const std::vector<VarDecl*>& v, Expression* in, Expression* where);
  /// Allocate single where clause (without generator) at position \a pos
  Generator(int pos, Expression* where);
};
/// \brief A list of generators with one where-expression
struct Generators {
  /// %Generators
  std::vector<Generator> g;
  /// Constructor
  Generators() {}
};
/// \brief An expression representing an array- or set-comprehension
class Comprehension : public BoxedExpression {
  friend class Expression;

protected:
  /// The expression to generate
  Expression* _e;
  /// A list of generator expressions
  ASTExprVec<Expression> _g;
  /// A list of indices where generators start
  ASTIntVec _gIndex;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_COMP;
  /// Constructor
  Comprehension(const Location& loc, Expression* e, Generators& g, bool set);
  /// Recompute hash value
  void rehash();
  /// Whether comprehension is a set
  bool set() const;

  /// Return number of generators
  unsigned int numberOfGenerators() const;
  /// Return "in" expression for generator \a i
  Expression* in(unsigned int i);
  /// Return "in" expression for generator \a i
  const Expression* in(unsigned int i) const;
  /// Return number of declarations for generator \a i
  unsigned int numberOfDecls(unsigned int i) const;
  /// Return declaration \a i for generator \a gen
  VarDecl* decl(unsigned int gen, unsigned int i);
  /// Return declaration \a i for generator \a gen
  const VarDecl* decl(unsigned int gen, unsigned int i) const;
  /// Return where clause for generator \a i
  Expression* where(unsigned int i);
  /// Return where clause for generator \a i
  const Expression* where(unsigned int i) const;
  /// Return generator body
  Expression* e() const { return _e; }
  /// Set generator body
  void e(Expression* e0) { _e = e0; }
  /// Re-construct (used for copying)
  void init(Expression* e, Generators& g);
  /// Check if \a e contains one of the variables bound by this comprehension
  bool containsBoundVariable(Expression* e);
};
/// \brief If-then-else expression
class ITE : public BoxedExpression {
  friend class Expression;

protected:
  /// List of if-then-pairs
  ASTExprVec<Expression> _eIfThen;
  /// Else-expression
  Expression* _eElse;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_ITE;
  /// Constructor
  ITE(const Location& loc, const std::vector<Expression*>& e_if_then, Expression* e_else);
  unsigned int size() const { return static_cast<int>(_eIfThen.size() / 2); }
  Expression* ifExpr(unsigned int i) { return _eIfThen[2 * i]; }
  Expression* thenExpr(unsigned int i) { return _eIfThen[2 * i + 1]; }
  Expression* elseExpr() { return _eElse; }
  const Expression* ifExpr(unsigned int i) const { return _eIfThen[2 * i]; }
  const Expression* thenExpr(unsigned int i) const { return _eIfThen[2 * i + 1]; }
  const Expression* elseExpr() const { return _eElse; }
  void thenExpr(unsigned int i, Expression* e) { _eIfThen[2 * i + 1] = e; }
  void elseExpr(Expression* e) { _eElse = e; }
  /// Recompute hash value
  void rehash();
  /// Re-construct (used for copying)
  void init(const std::vector<Expression*>& e_if_then, Expression* e_else);
};

/// Type of binary operators
enum BinOpType {
  BOT_PLUS,
  BOT_MINUS,
  BOT_MULT,
  BOT_DIV,
  BOT_IDIV,
  BOT_MOD,
  BOT_POW,
  BOT_LE,
  BOT_LQ,
  BOT_GR,
  BOT_GQ,
  BOT_EQ,
  BOT_NQ,
  BOT_IN,
  BOT_SUBSET,
  BOT_SUPERSET,
  BOT_UNION,
  BOT_DIFF,
  BOT_SYMDIFF,
  BOT_INTERSECT,
  BOT_PLUSPLUS,
  BOT_EQUIV,
  BOT_IMPL,
  BOT_RIMPL,
  BOT_OR,
  BOT_AND,
  BOT_XOR,
  BOT_DOTDOT
};
/// \brief Binary-operator expression
class BinOp : public BoxedExpression {
protected:
  /// Left hand side expression
  Expression* _e0;
  /// Right hand side expression
  Expression* _e1;
  /// The predicate or function declaration (or NULL)
  FunctionI* _decl;

private:
  /// Required to make BinOp same size as a ternary call
  void* _placeholder;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_BINOP;
  /// Constructor
  BinOp(const Location& loc, Expression* e0, BinOpType op, Expression* e1);
  /// Access left hand side
  Expression* lhs() const { return _e0; }
  /// Set left hand side
  void lhs(Expression* e) { _e0 = e; }
  /// Access right hand side
  Expression* rhs() const { return _e1; }
  /// Set right hand side
  void rhs(Expression* e) { _e1 = e; }
  /// Access argument \a i
  Expression* arg(int i) {
    assert(i == 0 || i == 1);
    return i == 0 ? _e0 : _e1;
  }
  /// Return number of arguments
  static unsigned int argCount() { return 2; }
  /// Access declaration
  FunctionI* decl() const { return _decl; }
  /// Set declaration
  void decl(FunctionI* f) { _decl = f; }
  /// Return string representation of the operator
  ASTString opToString() const;
  /// Recompute hash value
  void rehash();
  /// Return operator type
  BinOpType op() const;
  /// Morph into a call
  Call* morph(const ASTString& ident, const std::vector<Expression*>& args);
};

/// Type of unary operators
enum UnOpType { UOT_NOT, UOT_PLUS, UOT_MINUS };
/// \brief Unary-operator expressions
class UnOp : public BoxedExpression {
protected:
  /// %Expression
  Expression* _e0;
  /// The predicate or function declaration (or NULL)
  FunctionI* _decl;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_UNOP;
  /// Constructor
  UnOp(const Location& loc, UnOpType op, Expression* e);
  /// Access expression
  Expression* e() const { return _e0; }
  /// Set expression
  void e(Expression* e0) { _e0 = e0; }
  /// Access argument \a i
  Expression* arg(int i) {
    assert(i == 0);
    return _e0;
  }
  /// Return number of arguments
  static unsigned int argCount() { return 1; }
  /// Access declaration
  FunctionI* decl() const { return _decl; }
  /// Set declaration
  void decl(FunctionI* f) { _decl = f; }
  ASTString opToString() const;
  /// Recompute hash value
  void rehash();
  /// Return operator type
  UnOpType op() const;
};

class ExpressionContainerIterator {
private:
  Expression* const* _e;

public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = Expression*;
  using difference_type = std::ptrdiff_t;
  using pointer = Expression* const*;
  using reference = Expression* const&;

  ExpressionContainerIterator(Expression* const* e) : _e(e) {}

  reference operator*() const { return *_e; }
  pointer operator->() const { return &*_e; }
  ExpressionContainerIterator& operator++() {
    ++_e;
    return *this;
  }
  ExpressionContainerIterator& operator--() {
    --_e;
    return *this;
  }
  ExpressionContainerIterator operator++(int) { return ExpressionContainerIterator(_e++); }
  ExpressionContainerIterator operator--(int) { return ExpressionContainerIterator(_e--); }
  ExpressionContainerIterator operator+(const difference_type& n) const {
    return ExpressionContainerIterator(_e + n);
  }
  ExpressionContainerIterator& operator+=(const difference_type& n) {
    _e += n;
    return *this;
  }
  ExpressionContainerIterator operator-(const difference_type& n) const {
    return ExpressionContainerIterator(_e - n);
  }
  ExpressionContainerIterator& operator-=(const difference_type& n) {
    _e -= n;
    return *this;
  }
  reference operator[](const difference_type& n) const { return *(_e + n); }
  bool operator==(const ExpressionContainerIterator& it) const { return _e == it._e; }
  bool operator!=(const ExpressionContainerIterator& it) const { return !(*this == it); }
  bool operator<(const ExpressionContainerIterator& it) const { return _e < it._e; }
  bool operator>(const ExpressionContainerIterator& it) const { return _e > it._e; }
  bool operator<=(const ExpressionContainerIterator& it) const { return _e <= it._e; }
  bool operator>=(const ExpressionContainerIterator& it) const { return _e >= it._e; }
  difference_type operator-(const ExpressionContainerIterator& it) const { return _e - it._e; }
};

/// \brief A predicate or function call expression
class Call : public BoxedExpression {
  friend class Expression;
  friend class BinOp;

public:
  enum CallKind {
    CK_NULLARY = 0,
    CK_UNARY = 1,
    CK_BINARY = 2,
    CK_TERNARY = 3,
    CK_QUATERNARY = 4,
    CK_NARY = 5,
    CK_NARY_2,  // BINARY morphed into NARY
    CK_NARY_3,  // TERNARY morphed into NARY
    CK_NARY_4   // QUATERNARY morphed into NARY
  };

  class CallArgs {
  private:
    Expression* const* _begin;
    Expression* const* _end;

  public:
    typedef ExpressionContainerIterator iterator;
    typedef ExpressionContainerIterator const_iterator;

    CallArgs(const Call* c);

    iterator begin() { return ExpressionContainerIterator(_begin); }
    const_iterator begin() const { return ExpressionContainerIterator(_begin); }
    const_iterator cbegin() const { return ExpressionContainerIterator(_begin); }
    iterator end() { return ExpressionContainerIterator(_end); }
    const_iterator end() const { return ExpressionContainerIterator(_end); }
    const_iterator cend() const { return ExpressionContainerIterator(_end); }
  };

protected:
  union {
    /// Identifier of called predicate or function
    ASTString id;
    /// The predicate or function declaration (or NULL)
    FunctionI* decl;
  } _uId = {nullptr};
  /// Check if _uId contains an id or a decl
  bool hasId() const;
  /// Constructor
  Call(const Location& loc, const ASTString& id, const std::vector<Expression*>& args);

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_CALL;
  /// Constructor
  static Call* a(const Location& loc, const std::string& id, const std::vector<Expression*>& args);
  /// Constructor
  static Call* a(const Location& loc, const ASTString& id, const std::vector<Expression*>& args);
  /// Constructor to create commutative sorted call
  static Call* commutativeNormalized(EnvI& env, const Call* orig);
  /// Access identifier
  ASTString id() const;
  /// Set identifier (overwrites decl)
  void id(const ASTString& i);
  /// Number of arguments
  unsigned int argCount() const;
  /// Access argument \a i
  Expression* arg(unsigned int i) const;
  /// Set argument \a i
  void arg(unsigned int i, Expression* e);
  /// Set arguments
  void args(const std::vector<Expression*>& args);
  /// Access declaration
  FunctionI* decl() const;
  /// Set declaration (overwrites id)
  void decl(FunctionI* f);
  /// Recompute hash value
  void rehash();

  CallArgs args() const { return CallArgs(this); }
};

class Call1 : public Call {
  friend class Call;

protected:
  Expression* _data[1];
  Call1(const Location& loc, const ASTString& id, const std::vector<Expression*>& args)
      : Call(loc, id, args) {}
};
class Call2 : public Call {
  friend class Call;

protected:
  Expression* _data[2];
  Call2(const Location& loc, const ASTString& id, const std::vector<Expression*>& args)
      : Call(loc, id, args) {}
};
class Call3 : public Call {
  friend class Call;

protected:
  Expression* _data[3];
  Call3(const Location& loc, const ASTString& id, const std::vector<Expression*>& args)
      : Call(loc, id, args) {}
};
class Call4 : public Call {
  friend class Call;

protected:
  Expression* _data[4];
  Call4(const Location& loc, const ASTString& id, const std::vector<Expression*>& args)
      : Call(loc, id, args) {}
};
class CallNary : public Call {
  friend class Call;
  friend class Expression;

protected:
  ASTExprVecO<Expression*>* _args;
  CallNary(const Location& loc, const ASTString& id, const std::vector<Expression*>& args)
      : Call(loc, id, args) {}
};

class VarDeclI;

/// \brief A variable declaration expression
class VarDecl : public BoxedExpression {
protected:
  /// Type-inst of the declared variable
  TypeInst* _ti;
  /// Identifier
  Id* _id;
  /// Initialisation expression (can be NULL)
  Expression* _e;
  /// Flattened version of the VarDecl
  VarDecl* _flat;
  /// Integer payload
  int _payload;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_VARDECL;
  /// Constructor
  VarDecl(const Location& loc, TypeInst* ti, const std::string& id, Expression* e = nullptr);
  /// Constructor
  VarDecl(const Location& loc, TypeInst* ti, const ASTString& id, Expression* e = nullptr);
  /// Constructor
  VarDecl(const Location& loc, TypeInst* ti, long long int idn, Expression* e = nullptr);
  /// Constructor
  VarDecl(const Location& loc, TypeInst* ti, Id* id, Expression* e = nullptr);

  /// Access TypeInst
  TypeInst* ti() const { return _ti; }
  /// Set TypeInst
  void ti(TypeInst* t) { _ti = t; }
  /// Access identifier
  Id* id() const { return _id; }
  /// Access initialisation expression
  Expression* e() const;
  /// Set initialisation expression
  void e(Expression* rhs);
  /// Access flattened version
  VarDecl* flat() { return _flat; }
  /// Set flattened version
  void flat(VarDecl* vd);
  /// Access item
  VarDeclI* item() { return reinterpret_cast<VarDeclI*>(this); }
  /// Access item
  const VarDeclI* item() const { return reinterpret_cast<const VarDeclI*>(this); }

  /// Recompute hash value
  void rehash();
  /// Whether variable is toplevel
  bool toplevel() const;
  /// Whether variable is toplevel
  void toplevel(bool t);
  /// Whether variable is introduced
  bool introduced() const;
  /// Whether variable is introduced
  void introduced(bool t);
  /// Whether variable has been evaluated
  bool evaluated() const;
  /// Whether variable has been evaluated
  void evaluated(bool t);
  /// Access payload
  int payload() const { return _payload; }
  /// Set payload
  void payload(int i) { _payload = i; }
  /// Put current value on trail
  void trail();
  /// Whether VarDecl object is a type alias
  bool isTypeAlias() const {
    return _ti == nullptr && _e != nullptr && Expression::isa<TypeInst>(_e);
  }
};

class EnvI;
class CopyMap;
struct TIIDInfo;

/// \brief %Let expression
class Let : public BoxedExpression {
  friend Expression* copy(EnvI& env, CopyMap& m, Expression* e, bool followIds, bool copyFundecls,
                          bool isFlatModel);
  friend class Expression;

protected:
  /// List of local declarations
  ASTExprVec<Expression> _let;
  /// Copy of original local declarations
  ASTExprVec<Expression> _letOrig;
  /// Body of the let
  Expression* _in;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_LET;
  /// Constructor
  Let(const Location& loc, const std::vector<Expression*>& let, Expression* in);
  /// Recompute hash value
  void rehash();

  /// Access local declarations
  ASTExprVec<Expression> let() const { return _let; }
  /// Access local declarations
  ASTExprVec<Expression> letOrig() const { return _letOrig; }
  /// Set local declarations
  void setLetOrig(const ASTExprVec<Expression>& letOrig) { _letOrig = letOrig; }
  /// Access body
  Expression* in() const { return _in; }
  /// Set body
  void in(Expression* e) { _in = e; }

  /// Remember current let bindings
  void pushbindings();
  /// Restore previous let bindings
  void popbindings();
};

/// Convenience class for pushing/popping let bindings
class LetPushBindings {
private:
  Let* _let;

public:
  LetPushBindings(Let* l) : _let(l) { l->pushbindings(); }
  ~LetPushBindings() { _let->popbindings(); }
};

/// \brief Type-inst expression
class TypeInst : public BoxedExpression {
protected:
  /// Ranges of an array expression
  ASTExprVec<TypeInst> _ranges;
  /// Declared domain (or NULL)
  Expression* _domain;

public:
  /// The identifier of this expression type
  static const ExpressionId eid = E_TI;
  /// Constructor
  TypeInst(const Location& loc, const Type& t, const ASTExprVec<TypeInst>& ranges,
           Expression* domain = nullptr);
  /// Constructor
  TypeInst(const Location& loc, const Type& t, Expression* domain = nullptr);

  /// Access ranges
  ASTExprVec<TypeInst> ranges() const { return _ranges; }
  /// Access domain
  Expression* domain() const { return _domain; }
  //// Set domain
  void domain(Expression* d) { _domain = d; }
  /// Erase domain, preserving tuple types stored in domain field
  void eraseDomain() {
    if (_domain == nullptr || !Expression::isa<ArrayLit>(_domain)) {
      _domain = nullptr;
      return;
    }
    auto* al = Expression::cast<ArrayLit>(_domain);
    for (unsigned int i = 0; i < al->size(); ++i) {
      auto* field_ti = Expression::cast<TypeInst>((*al)[i]);
      field_ti->eraseDomain();
    }
  }
  void canonicaliseStruct(EnvI& env);
  void mkVar(const EnvI& env);
  /// Make this type-inst par, maintaining the domain.
  ///
  /// Assumes TypeInst has already gone through the typechecker,
  /// (i.e. struct domains are ArrayLits containing TypeInsts)
  void mkPar(EnvI& env);
  // Set domain of a tuple based on a tuple type object
  void setStructDomain(EnvI& env, const Type& struct_type, bool setTypeAny = false,
                       bool setTIRanges = true);
  /// Resolve type aliases
  bool resolveAlias(EnvI& env);
  /// Resolve ++ binop expressions in the domain (if present)
  /// Returns whether any expression had to be resolved
  bool concatDomain(EnvI& env);

  /// Set ranges to \a ranges
  void setRanges(const std::vector<TypeInst*>& ranges);
  bool isarray() const { return !_ranges.empty(); }
  bool hasTiVariable() const;
  /// Recompute hash value
  void rehash();
  /// Check if domain is computed from right hand side of variable
  bool computedDomain() const { return _flag1; }
  /// Set if domain is computed from right hand side of variable
  void setComputedDomain(bool b) { _flag1 = b; }
  /// Check if this TypeInst represents an enum
  bool isEnum() const { return _flag2; }
  /// Set if this TypeInst represents an enum
  void setIsEnum(bool b) { _flag2 = b; }

  // Collect type ids for monomorphasation (assumes tuple TypeInst)
  void collectTypeIds(std::unordered_map<ASTString, size_t>& seen_tiids,
                      std::vector<TIIDInfo>& type_ids) const;
};

/**
 * \brief Base-class for items
 */
class Item : public ASTNode {
private:
  Type _tUnused;  // required to enforce same layout as Expression
protected:
  /// Location of the item
  Location _loc;

public:
  /// Identifier of the concrete item type
  enum ItemId {
    II_VD = Expression::E_VARDECL,
    II_INC = Expression::EID_END + 1,
    II_ASN,
    II_CON,
    II_SOL,
    II_OUT,
    II_FUN,
    II_END = II_FUN
  };
  ItemId iid() const { return static_cast<ItemId>(_id); }

  const Location& loc() const { return _loc; }

protected:
  /// Constructor
  Item(const Location& loc, const ItemId& iid) : ASTNode(iid), _loc(loc) { _flag1 = false; }

public:
  /// Test if item is of type \a T
  template <class T>
  bool isa() const {
    return _id == T::iid;
  }
  /// Cast item to type \a T*
  template <class T>
  T* cast() {
    assert(isa<T>());
    return static_cast<T*>(this);
  }
  /// Cast expression to type \a const T*
  template <class T>
  const T* cast() const {
    assert(isa<T>());
    return static_cast<const T*>(this);
  }
  /// Cast item to type \a T* or NULL if types do not match
  template <class T>
  T* dynamicCast() {
    return isa<T>() ? static_cast<T*>(this) : nullptr;
  }
  /// Cast item to type \a const T* or NULL if types do not match
  template <class T>
  const T* dynamicCast() const {
    return isa<T>() ? static_cast<const T*>(this) : NULL;
  }

  /// Cast item to type \a T*
  template <class T>
  static T* cast(Item* i) {
    return i == nullptr ? nullptr : i->cast<T>();
  }
  /// Cast item to type \a const T*
  template <class T>
  static const T* cast(const Item* i) {
    return i == nullptr ? NULL : i->cast<T>();
  }
  /// Cast item to type \a T* or NULL if types do not match
  template <class T>
  static T* dynamicCast(Item* i) {
    return i == nullptr ? nullptr : i->dynamicCast<T>();
  }
  /// Cast item to type \a const T* or NULL if types do not match
  template <class T>
  static const T* dynamicCast(const Item* i) {
    return i == nullptr ? NULL : i->dynamicCast<T>();
  }

  /// Check if item should be removed
  bool removed() const { return _flag1; }
  /// Set flag to remove item
  void remove() { _flag1 = true; }
  /// Unset remove item flag (only possible if not already removed by compact())
  void unremove() { _flag1 = false; }

  /// Mark alive for garbage collection
  static void mark(Item* item);

  bool hasMark() { return _gcMark != 0U; }
};

class Model;

/// \brief Include item
class IncludeI : public Item {
protected:
  /// Filename to include
  ASTString _f;
  /// Model for that file
  Model* _m;

public:
  /// The identifier of this item type
  static const ItemId iid = II_INC;
  /// Constructor
  IncludeI(const Location& loc, const ASTString& f);
  /// Access filename
  ASTString f() const { return _f; }
  /// Set filename
  void f(const ASTString& nf) { _f = nf; }
  /// Access model
  Model* m() const { return _m; }
  /// Set the model
  void m(Model* m0, bool own = true) {
    assert(_m == nullptr || m0 == nullptr);
    _m = m0;
    _flag2 = own;
  }
  bool own() const { return _flag2; }
};

/// \brief Variable declaration item
class VarDeclI : public Item {
protected:
  VarDeclI() = delete;
  VarDeclI(const VarDeclI&) = delete;
  VarDeclI& operator=(const VarDeclI&) = delete;

public:
  /// The identifier of this item type
  static const ItemId iid = II_VD;
  /// Constructor
  static VarDeclI* a(const Location& loc, VarDecl* e);
  /// Access expression
  VarDecl* e() { return reinterpret_cast<VarDecl*>(this); }
  /// Access expression
  const VarDecl* e() const { return reinterpret_cast<const VarDecl*>(this); }
  /// Flag used during compilation
  bool flag() const { return _flag2; }
  /// Set flag used during compilation
  void flag(bool b) { _flag2 = b; }
};

/// \brief Assign item
class AssignI : public Item {
protected:
  /// Identifier of variable to assign to
  ASTString _id;
  /// Expression to assign to the variable
  Expression* _e;
  /// Declaration of the variable to assign to
  VarDecl* _decl;

public:
  /// The identifier of this item type
  static const ItemId iid = II_ASN;
  /// Constructor
  AssignI(const Location& loc, const std::string& id, Expression* e);
  /// Constructor
  AssignI(const Location& loc, const ASTString& id, Expression* e);
  /// Access identifier
  ASTString id() const { return _id; }
  /// Access expression
  Expression* e() const { return _e; }
  /// Set expression
  void e(Expression* e0) { _e = e0; }
  /// Access declaration
  VarDecl* decl() const { return _decl; }
  /// Set declaration
  void decl(VarDecl* d) { _decl = d; }
};

/// \brief Constraint item
class ConstraintI : public Item {
protected:
  /// Constraint expression
  Expression* _e;

public:
  /// The identifier of this item type
  static const ItemId iid = II_CON;
  /// Constructor
  ConstraintI(const Location& loc, Expression* e);
  /// Access expression
  Expression* e() const { return _e; }
  /// Set expression
  void e(Expression* e0) { _e = e0; }
  /// Flag used during compilation
  bool flag() const { return _flag2; }
  /// Set flag used during compilation
  void flag(bool b) { _flag2 = b; }
};

/// \brief Solve item
class SolveI : public Item {
protected:
  /// Solve item annotation
  Annotation _ann;
  /// Expression for minimisation/maximisation (or NULL)
  Expression* _e;
  /// Constructor
  SolveI(const Location& loc, Expression* e);

public:
  /// The identifier of this item type
  static const ItemId iid = II_SOL;
  /// Type of solving
  enum SolveType { ST_SAT, ST_MIN, ST_MAX };
  /// Allocate solve satisfy item
  static SolveI* sat(const Location& loc);
  /// Allocate solve minimize item
  static SolveI* min(const Location& loc, Expression* e);
  /// Allocate solve maximize item
  static SolveI* max(const Location& loc, Expression* e);
  /// Access solve annotation
  const Annotation& ann() const { return _ann; }
  /// Access solve annotation
  Annotation& ann() { return _ann; }
  /// Access expression for optimisation
  Expression* e() const { return _e; }
  /// Set expression for optimisation
  void e(Expression* e0) { _e = e0; }
  /// Return type of solving
  SolveType st() const;
  /// Set type of solving
  void st(SolveType s);
};

/// \brief Output item
class OutputI : public Item {
protected:
  /// Expression to output
  Expression* _e;
  /// Annotation
  Annotation _ann;

public:
  /// The identifier of this item type
  static const ItemId iid = II_OUT;
  /// Constructor
  OutputI(const Location& loc, Expression* e);
  /// Access expression
  Expression* e() const { return _e; }
  /// Update expression
  void e(Expression* e) { _e = e; }
  /// Access annotation
  const Annotation& ann() const { return _ann; }
  /// Access annotation
  Annotation& ann() { return _ann; }
};

class EnvI;

/// \brief Function declaration item
class FunctionI : public Item {
protected:
  /// Identifier of this function
  ASTString _id;
  /// Type-inst of the return value
  TypeInst* _ti;
  /// List of parameter declarations
  ASTExprVec<VarDecl> _params;
  /// Annotation
  Annotation _ann;
  /// Function body (or NULL)
  Expression* _e;
  /// Whether function is defined in the standard library
  bool _fromStdLib;
  /// Whether function captures annotations
  bool _captureAnnotations;

public:
  /// The identifier of this item type
  static const ItemId iid = II_FUN;

  /// Type of builtin expression-valued functions
  typedef Expression* (*builtin_e)(EnvI&, Call*);
  /// Type of builtin int-valued functions
  typedef IntVal (*builtin_i)(EnvI&, Call*);
  /// Type of builtin bool-valued functions
  typedef bool (*builtin_b)(EnvI&, Call*);
  /// Type of builtin float-valued functions
  typedef FloatVal (*builtin_f)(EnvI&, Call*);
  /// Type of builtin int set-valued functions
  typedef IntSetVal* (*builtin_s)(EnvI&, Call*);
  /// Type of builtin float set-valued functions
  typedef FloatSetVal* (*builtin_fs)(EnvI&, Call*);
  /// Type of builtin string-valued functions
  typedef std::string (*builtin_str)(EnvI&, Call*);

  /// Builtin functions (or NULL)
  struct {
    builtin_e e;
    builtin_i i;
    builtin_f f;
    builtin_b b;
    builtin_s s;
    builtin_fs fs;
    builtin_str str;
  } builtins;

  /// Constructor
  FunctionI(const Location& loc, const ASTString& id, TypeInst* ti,
            const std::vector<VarDecl*>& params, Expression* e = nullptr, bool from_stdlib = false,
            bool capture_annotations = false);

  /// Set parameters (only needed for copying)
  void init(const std::vector<VarDecl*>& params);

  /// Access identifier
  ASTString id() const { return _id; }
  /// Set identifier
  void id(const ASTString& newId) { _id = newId; }
  /// Access TypeInst
  TypeInst* ti() const { return _ti; }
  /// Set TypeInst
  void ti(TypeInst* newTi) { _ti = newTi; }

  /// Get number of parameters
  unsigned int paramCount() const {
    return _params.size() - static_cast<unsigned int>(_captureAnnotations);
  }
  /// Get parameter \a i
  VarDecl* param(unsigned int i) const { return _params[i]; }
  /// Mark param array for garbage collection
  void markParams();

  /// Access annotation
  const Annotation& ann() const { return _ann; }
  /// Access annotation
  Annotation& ann() { return _ann; }
  /// Access body
  Expression* e() const { return _e; }
  /// Set body
  void e(Expression* b) { _e = b; }

  /** \brief Compute return type given argument types \a ta
   */
  Type rtype(EnvI& env, const std::vector<Expression*>& ta, Expression* call, bool strictEnums);
  /** \brief Compute return type given argument types \a ta
   */
  Type rtype(EnvI& env, const std::vector<Type>& ta, Expression* call, bool strictEnums);
  /** \brief Compute expected type of argument \a n given argument types \a ta
   */
  Type argtype(EnvI& env, const std::vector<Expression*>& ta, unsigned int n) const;

  /// Return whether function is defined in the standard library
  bool fromStdLib() const { return _fromStdLib; };

  /// Return whether function is a monomorphised version of another function
  bool isMonomorphised() const { return _flag2; }

  /// Set whether function is a monomorphised version of another function
  void isMonomorphised(bool b) { _flag2 = b; }

  /// Whether this function is polymorphic
  bool isPolymorphic() const {
    bool isPoly = ti()->hasTiVariable();
    if (!isPoly) {
      for (unsigned int i = 0; i < paramCount(); i++) {
        if (param(i)->ti()->hasTiVariable()) {
          return true;
        }
      }
      return false;
    }
    return true;
  }

  /// Return variable that captures annotations (or null)
  VarDecl* capturedAnnotationsVar() const {
    return _captureAnnotations ? _params[_params.size() - 1] : nullptr;
  }

  /// Creates a tuple TypeInst literal for the TIs of the parameters
  TypeInst* paramTypes() const {
    assert(GC::locked());
    // Create a parameter TypeInst in the format of a tuple TypeInst
    std::vector<Expression*> tis(paramCount());
    for (unsigned int i = 0; i < paramCount(); ++i) {
      tis[i] = param(i)->ti();
    }
    return new TypeInst(Location().introduce(), Type::tuple(),
                        ArrayLit::constructTuple(Location().introduce(), tis));
  }
};

/**
 * \brief Visitor for expressions
 *
 * This class implements no-ops for all expression types.
 * Override the methods to implement custom behaviour.
 */
class EVisitor {
public:
  /// Visit integer literal
  void vIntLit(const IntLit* /*il*/) {}
  /// Visit floating point literal
  void vFloatLit(const FloatLit* /*fl*/) {}
  /// Visit Boolean literal
  void vBoolLit(const BoolLit* /*bl*/) {}
  /// Visit set literal
  void vSetLit(const SetLit* /*sl*/) {}
  /// Visit string literal
  void vStringLit(const StringLit* /*sl*/) {}
  /// Visit identifier
  void vId(const Id* /*ident*/) {}
  /// Visit anonymous variable
  void vAnonVar(const AnonVar* /*x*/) {}
  /// Visit array literal
  void vArrayLit(const ArrayLit* /*al*/) {}
  /// Visit array access
  void vArrayAccess(const ArrayAccess* /*aa*/) {}
  /// Visit field access
  void vFieldAccess(const FieldAccess* /*fa*/) {}
  /// Visit array comprehension
  void vComprehension(const Comprehension* /*c*/) {}
  /// Visit array comprehension (only generator \a gen_i)
  void vComprehensionGenerator(const Comprehension* /*c*/, int /*gen_i*/) {}
  /// Visit if-then-else
  void vITE(const ITE* /*ite*/) {}
  /// Visit binary operator
  void vBinOp(const BinOp* /*bo*/) {}
  /// Visit unary operator
  void vUnOp(const UnOp* /*uo*/) {}
  /// Visit call
  void vCall(const Call* /*c*/) {}
  /// Visit let
  void vLet(const Let* /*let*/) {}
  /// Visit variable declaration
  void vVarDecl(const VarDecl* /*vd*/) {}
  /// Visit type inst
  void vTypeInst(const TypeInst* /*ti*/) {}
  /// Visit TIId
  void vTIId(const TIId* /*tiid*/) {}
  /// Determine whether to enter node
  static bool enter(Expression* /*e*/) { return true; }
  /// Exit node after processing has finished
  void exit(Expression* /*e*/) {}
};

/// Statically allocated constants
class Constants : public GCMarker {
protected:
  /// All the IDs (used for garbage collection)
  std::vector<Id*> _ids;
  /// All the strings (used for garbage collection)
  std::vector<ASTString> _strings;

  /// Register a new string
  ASTString addString(const std::string& s);
  /// Register a new identifier
  Id* addId(const std::string& s);

public:
  /// Literal true
  BoolLit* literalTrue;
  /// Variable bound to true
  VarDecl* varTrue;
  /// Literal false
  BoolLit* literalFalse;
  /// Variable bound to false
  VarDecl* varFalse;
  /// Special variable to signal compiler to ignore result
  VarDecl* varIgnore;
  /// Infinite (integer) set
  SetLit* infinityInt;
  /// Infinite (float) set
  SetLit* infinityFloat;
  /// Function item used to keep track of redefined variables
  FunctionI* varRedef;
  /// Literal absent value
  Expression* absent;
  /// Empty array of literals
  ArrayLit* emptyBoolArray;
  /// Identifiers for builtins
  struct {
    ASTString forall;
    ASTString forallReif;
    ASTString exists;
    ASTString clause;
    ASTString bool2int;
    ASTString int2float;
    ASTString bool2float;
    ASTString enum2int;
    ASTString index2int;
    ASTString to_enum_internal;  // NOLINT(readability-identifier-naming)
    ASTString set2iter;
    ASTString assert;
    ASTString assert_dbg;  // NOLINT(readability-identifier-naming)
    ASTString deopt;
    ASTString absent;
    ASTString occurs;
    ASTString card;
    ASTString abs;
    ASTString mzn_alias_eq;  // NOLINT(readability-identifier-naming)

    ASTString symmetry_breaking_constraint;      // NOLINT(readability-identifier-naming)
    ASTString redundant_constraint;              // NOLINT(readability-identifier-naming)
    ASTString implied_constraint;                // NOLINT(readability-identifier-naming)
    ASTString mzn_deprecate;                     // NOLINT(readability-identifier-naming)
    ASTString mzn_symmetry_breaking_constraint;  // NOLINT(readability-identifier-naming)
    ASTString mzn_redundant_constraint;          // NOLINT(readability-identifier-naming)
    ASTString mzn_reverse_map_var;               // NOLINT(readability-identifier-naming)
    ASTString mzn_default;                       // NOLINT(readability-identifier-naming)
    ASTString mzn_in_root_context;               // NOLINT(readability-identifier-naming)
    ASTString mzn_output_section;                // NOLINT(readability-identifier-naming)
    ASTString output_to_section;                 // NOLINT(readability-identifier-naming)
    ASTString output_to_json_section;            // NOLINT(readability-identifier-naming)
    ASTString trace;
    ASTString trace_dbg;             // NOLINT(readability-identifier-naming)
    ASTString mzn_trace_to_section;  // NOLINT(readability-identifier-naming)

    ASTString array1d;
    ASTString array2d;
    ASTString array3d;
    ASTString array4d;
    ASTString array5d;
    ASTString array6d;
    ASTString arrayXd;

    ASTString arrayNd(int n) const {
      switch (n) {
        case 1:
          return array1d;
        case 2:
          return array2d;
        case 3:
          return array3d;
        case 4:
          return array4d;
        case 5:
          return array5d;
        case 6:
          return array6d;
        default:
          throw InternalError("arrayXd only defined for between 1 and 6 dimensions");
      }
    }

    ASTString sum;
    ASTString lex_less;    // NOLINT(readability-identifier-naming)
    ASTString lex_lesseq;  // NOLINT(readability-identifier-naming)
    ASTString lin_exp;     // NOLINT(readability-identifier-naming)
    ASTString count;
    ASTString element;
    ASTString table;
    ASTString anon_enum;      // NOLINT(readability-identifier-naming)
    ASTString anon_enum_set;  // NOLINT(readability-identifier-naming)
    ASTString enumFromConstructors;
    ASTString enumOf;
    ASTString enumOfInternal;

    ASTString concat;
    ASTString join;
    ASTString show;
    ASTString format;
    ASTString format_justify_string;  // NOLINT(readability-identifier-naming)
    ASTString showDzn;
    ASTString showJSON;
    ASTString fix;
    ASTString output;
    ASTString outputJSON;

    struct {
      ASTString lin_eq;  // NOLINT(readability-identifier-naming)
      ASTString lin_le;  // NOLINT(readability-identifier-naming)
      ASTString lin_ne;  // NOLINT(readability-identifier-naming)
      ASTString plus;
      ASTString minus;
      ASTString times;
      ASTString div;
      ASTString mod;
      ASTString lt;
      ASTString le;
      ASTString gt;
      ASTString ge;
      ASTString eq;
      ASTString ne;
    } int_;  // NOLINT(readability-identifier-naming)

    struct {
      ASTString lin_eq;  // NOLINT(readability-identifier-naming)
      ASTString lin_le;  // NOLINT(readability-identifier-naming)
      ASTString lin_ne;  // NOLINT(readability-identifier-naming)
      ASTString plus;
      ASTString minus;
      ASTString times;
      ASTString div;
      ASTString mod;
      ASTString lt;
      ASTString le;
      ASTString gt;
      ASTString ge;
      ASTString eq;
      ASTString ne;
    } int_reif;  // NOLINT(readability-identifier-naming)

    struct {
      ASTString lin_eq;  // NOLINT(readability-identifier-naming)
      ASTString lin_le;  // NOLINT(readability-identifier-naming)
      ASTString lin_lt;  // NOLINT(readability-identifier-naming)
      ASTString lin_ne;  // NOLINT(readability-identifier-naming)
      ASTString plus;
      ASTString minus;
      ASTString times;
      ASTString div;
      ASTString mod;
      ASTString lt;
      ASTString le;
      ASTString gt;
      ASTString ge;
      ASTString eq;
      ASTString ne;
      ASTString in;
      ASTString dom;
    } float_;  // NOLINT(readability-identifier-naming)

    struct {
      ASTString lin_eq;  // NOLINT(readability-identifier-naming)
      ASTString lin_le;  // NOLINT(readability-identifier-naming)
      ASTString lin_lt;  // NOLINT(readability-identifier-naming)
      ASTString lin_ne;  // NOLINT(readability-identifier-naming)
      ASTString plus;
      ASTString minus;
      ASTString times;
      ASTString div;
      ASTString mod;
      ASTString lt;
      ASTString le;
      ASTString gt;
      ASTString ge;
      ASTString eq;
      ASTString ne;
      ASTString in;
    } float_reif;  // NOLINT(readability-identifier-naming)

    struct {
      ASTString and_;  // NOLINT(readability-identifier-naming)
      ASTString clause;
      ASTString eq;
      ASTString ge;
      ASTString gt;
      ASTString le;
      ASTString lt;
      ASTString ne;
      ASTString not_;  // NOLINT(readability-identifier-naming)
      ASTString or_;   // NOLINT(readability-identifier-naming)
    } bool_;           // NOLINT(readability-identifier-naming)

    struct {
      ASTString clause;
      ASTString eq;
      ASTString array_and;  // NOLINT(readability-identifier-naming)
    } bool_reif;            // NOLINT(readability-identifier-naming)

    ASTString array_bool_and_imp;  // NOLINT(readability-identifier-naming)

    struct {
      ASTString card;
      ASTString diff;
      ASTString eq;
      ASTString ge;
      ASTString gt;
      ASTString in;
      ASTString intersect;
      ASTString le;
      ASTString lt;
      ASTString ne;
      ASTString subset;
      ASTString superset;
      ASTString symdiff;
      ASTString union_;  // NOLINT(readability-identifier-naming)
    } set_;              // NOLINT(readability-identifier-naming)

    // Identifiers for experimental on_restart feature
    struct {
      ASTString sol;
      ASTString last_val;            // NOLINT(readability-identifier-naming)
      ASTString on_restart;          // NOLINT(readability-identifier-naming)
      ASTString uniform_on_restart;  // NOLINT(readability-identifier-naming)
    } on_restart;                    // NOLINT(readability-identifier-naming)

    ASTString pow;

    ASTString mzn_set_in_internal;  // NOLINT(readability-identifier-naming)

    ASTString introduced_var;  // NOLINT(readability-identifier-naming)
    ASTString anonEnumFromStrings;

    ASTString unnamedArgument;
  } ids;

  bool isCallByReferenceId(const ASTString& cid) const;

  /// Identifiers for Boolean contexts
  struct {
    Id* root;
    Id* pos;
    Id* neg;
    Id* mix;
    Id* promise_monotone;  // NOLINT(readability-identifier-naming)
    Id* promise_antitone;  // NOLINT(readability-identifier-naming)
  } ctx;
  /// Common annotations
  struct {
    Id* empty_annotation;                    // NOLINT(readability-identifier-naming)
    Id* output_var;                          // NOLINT(readability-identifier-naming)
    ASTString output_array;                  // NOLINT(readability-identifier-naming)
    Id* add_to_output;                       // NOLINT(readability-identifier-naming)
    Id* output;                              // NOLINT(readability-identifier-naming)
    Id* no_output;                           // NOLINT(readability-identifier-naming)
    Id* output_only;                         // NOLINT(readability-identifier-naming)
    Id* mzn_check_var;                       // NOLINT(readability-identifier-naming)
    ASTString mzn_check_enum_var;            // NOLINT(readability-identifier-naming)
    Id* is_defined_var;                      // NOLINT(readability-identifier-naming)
    ASTString defines_var;                   // NOLINT(readability-identifier-naming)
    Id* is_reverse_map;                      // NOLINT(readability-identifier-naming)
    Id* promise_total;                       // NOLINT(readability-identifier-naming)
    Id* maybe_partial;                       // NOLINT(readability-identifier-naming)
    ASTString doc_comment;                   // NOLINT(readability-identifier-naming)
    ASTString mzn_path;                      // NOLINT(readability-identifier-naming)
    ASTString is_introduced;                 // NOLINT(readability-identifier-naming)
    Id* mzn_break_here;                      // NOLINT(readability-identifier-naming)
    Id* rhs_from_assignment;                 // NOLINT(readability-identifier-naming)
    Id* domain_change_constraint;            // NOLINT(readability-identifier-naming)
    ASTString mzn_deprecated;                // NOLINT(readability-identifier-naming)
    Id* mzn_was_undefined;                   // NOLINT(readability-identifier-naming)
    Id* array_check_form;                    // NOLINT(readability-identifier-naming)
    Id* annotated_expression;                // NOLINT(readability-identifier-naming)
    ASTString mzn_add_annotated_expression;  // NOLINT(readability-identifier-naming)
    ASTString expression_name_dbg;           // NOLINT(readability-identifier-naming)
    Id* cache_result;                        // NOLINT(readability-identifier-naming)
    Id* no_cse;                              // NOLINT(readability-identifier-naming)
    Id* mzn_internal_representation;         // NOLINT(readability-identifier-naming)
    Id* flatzinc_builtin;                    // NOLINT(readability-identifier-naming)
    Id* mzn_evaluate_once;                   // NOLINT(readability-identifier-naming)
    Id* promise_commutative;                 // NOLINT(readability-identifier-naming)
    ASTString seq_search;                    // NOLINT(readability-identifier-naming)
    ASTString seq_search_internal;           // NOLINT(readability-identifier-naming)
    ASTString int_search;                    // NOLINT(readability-identifier-naming)
    ASTString int_search_internal;           // NOLINT(readability-identifier-naming)
    ASTString bool_search;                   // NOLINT(readability-identifier-naming)
    ASTString bool_search_internal;          // NOLINT(readability-identifier-naming)
    ASTString float_search;                  // NOLINT(readability-identifier-naming)
    ASTString float_search_internal;         // NOLINT(readability-identifier-naming)
    ASTString set_search;                    // NOLINT(readability-identifier-naming)
    ASTString set_search_internal;           // NOLINT(readability-identifier-naming)
    ASTString warm_start;                    // NOLINT(readability-identifier-naming)
    ASTString warm_start_internal;           // NOLINT(readability-identifier-naming)
    ASTString warm_start_array;              // NOLINT(readability-identifier-naming)
    ASTString warm_start_array_internal;     // NOLINT(readability-identifier-naming)
    Id* computed_domain;                     // NOLINT(readability-identifier-naming)
  } ann;

  /// Command line options
  struct {                                /// basic MiniZinc command line options
    ASTString cmdlineData_str;            // NOLINT(readability-identifier-naming)
    ASTString cmdlineData_short_str;      // NOLINT(readability-identifier-naming)
    ASTString datafile_str;               // NOLINT(readability-identifier-naming)
    ASTString datafile_short_str;         // NOLINT(readability-identifier-naming)
    ASTString globalsDir_str;             // NOLINT(readability-identifier-naming)
    ASTString globalsDir_alt_str;         // NOLINT(readability-identifier-naming)
    ASTString globalsDir_short_str;       // NOLINT(readability-identifier-naming)
    ASTString help_str;                   // NOLINT(readability-identifier-naming)
    ASTString help_short_str;             // NOLINT(readability-identifier-naming)
    ASTString ignoreStdlib_str;           // NOLINT(readability-identifier-naming)
    ASTString include_str;                // NOLINT(readability-identifier-naming)
    ASTString inputFromStdin_str;         // NOLINT(readability-identifier-naming)
    ASTString instanceCheckOnly_str;      // NOLINT(readability-identifier-naming)
    ASTString no_optimize_str;            // NOLINT(readability-identifier-naming)
    ASTString no_optimize_alt_str;        // NOLINT(readability-identifier-naming)
    ASTString no_outputOzn_str;           // NOLINT(readability-identifier-naming)
    ASTString no_outputOzn_short_str;     // NOLINT(readability-identifier-naming)
    ASTString no_typecheck_str;           // NOLINT(readability-identifier-naming)
    ASTString newfzn_str;                 // NOLINT(readability-identifier-naming)
    ASTString outputBase_str;             // NOLINT(readability-identifier-naming)
    ASTString outputFznToStdout_str;      // NOLINT(readability-identifier-naming)
    ASTString outputFznToStdout_alt_str;  // NOLINT(readability-identifier-naming)
    ASTString outputOznToFile_str;        // NOLINT(readability-identifier-naming)
    ASTString outputOznToStdout_str;      // NOLINT(readability-identifier-naming)
    ASTString outputFznToFile_str;        // NOLINT(readability-identifier-naming)
    ASTString outputFznToFile_alt_str;    // NOLINT(readability-identifier-naming)
    ASTString outputFznToFile_short_str;  // NOLINT(readability-identifier-naming)
    ASTString rangeDomainsOnly_str;       // NOLINT(readability-identifier-naming)
    ASTString statistics_str;             // NOLINT(readability-identifier-naming)
    ASTString statistics_short_str;       // NOLINT(readability-identifier-naming)
    ASTString stdlib_str;                 // NOLINT(readability-identifier-naming)
    ASTString verbose_str;                // NOLINT(readability-identifier-naming)
    ASTString verbose_short_str;          // NOLINT(readability-identifier-naming)
    ASTString version_str;                // NOLINT(readability-identifier-naming)
    ASTString werror_str;                 // NOLINT(readability-identifier-naming)

    struct {
      ASTString all_sols_str;    // NOLINT(readability-identifier-naming)
      ASTString fzn_solver_str;  // NOLINT(readability-identifier-naming)
    } solver;

  } cli;

  /// options strings to find setting in Options map
  struct {
    ASTString cmdlineData;
    ASTString datafile;
    ASTString datafiles;
    ASTString fznToStdout;
    ASTString fznToFile;
    ASTString globalsDir;
    ASTString ignoreStdlib;
    ASTString includeDir;
    ASTString includePaths;
    ASTString instanceCheckOnly;
    ASTString inputFromStdin;
    ASTString model;
    ASTString newfzn;
    ASTString noOznOutput;
    ASTString optimize;
    ASTString outputBase;
    ASTString oznToFile;
    ASTString oznToStdout;
    ASTString rangeDomainsOnly;
    ASTString statistics;
    ASTString stdlib;
    ASTString typecheck;
    ASTString verbose;
    ASTString werror;

    struct {
      ASTString allSols;
      ASTString numSols;
      ASTString threads;
      ASTString fzn_solver;         // NOLINT(readability-identifier-naming)
      ASTString fzn_flags;          // NOLINT(readability-identifier-naming)
      ASTString fzn_flag;           // NOLINT(readability-identifier-naming)
      ASTString fzn_time_limit_ms;  // NOLINT(readability-identifier-naming)
      ASTString fzn_sigint;         // NOLINT(readability-identifier-naming)
    } solver;

  } opts;

  /// categories of the command line interface options
  struct {
    ASTString general;
    ASTString io;
    ASTString solver;
    ASTString translation;
  } cli_cat;  // NOLINT(readability-identifier-naming)

  /// Keep track of allocated integer literals
  std::unordered_map<IntVal, WeakRef> integerMap;
  /// Keep track of allocated float literals
  std::unordered_map<FloatVal, WeakRef> floatMap;
  /// Constructor
  Constants();
  /// Return shared BoolLit
  BoolLit* boollit(bool b) const { return b ? literalTrue : literalFalse; }
  static const int max_array_size = std::numeric_limits<int>::max() / 2;

  /// Internal annotations
  std::array<Id*, 11> internalAnn() const {
    return {
        ctx.mix,
        ctx.pos,
        ctx.neg,
        ctx.root,
        ann.promise_total,
        ann.add_to_output,
        ann.output,
        ann.no_output,
        ann.mzn_check_var,
        ann.rhs_from_assignment,
        ann.mzn_was_undefined,
    };
  };

  void mark() override;

  /// Return static instance
  static Constants& constants();
};

}  // namespace MiniZinc

#include <minizinc/ast.hpp>
