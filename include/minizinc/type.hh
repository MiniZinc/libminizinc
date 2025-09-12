/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <cassert>
#include <functional>
#include <sstream>
#include <string>

namespace MiniZinc {

class EnvI;
class TupleType;

/// Type of a MiniZinc expression
class Type {
public:
  /// Basic type
  enum BaseType {
    BT_BOOL,
    BT_INT,
    BT_FLOAT,
    BT_STRING,
    BT_ANN,
    BT_TUPLE,
    BT_RECORD,
    BT_TOP,
    BT_BOT,
    BT_UNKNOWN
  };
  /// Type-inst
  enum Inst { TI_PAR, TI_VAR };
  /// Whether a type element is explicitly set by user
  enum ExplicitType { EXPL_NO, EXPL_YES };
  /// Whether the expression is plain or set
  enum SetType { ST_PLAIN, ST_SET };
  /// Whether the expression is normal or optional
  enum OptType { OT_PRESENT, OT_OPTIONAL };
  /// Whether the par expression contains a var argument
  enum ContainsVarType { CV_NO, CV_YES };
  /// Whether the type represents an "any" type-inst variable
  enum AnyType { AT_NO, AT_YES };
  /// TypeId for tuples reserved for index tuples in comprehensions
  static const unsigned int COMP_INDEX = 0xFFFF;

private:
  unsigned int _bt : 4;
  unsigned int _ti : 1;
  unsigned int _tiExplicit : 1;
  unsigned int _st : 1;
  unsigned int _ot : 1;
  unsigned int _otExplicit : 1;
  unsigned int _cv : 1;
  unsigned int _at : 1;
  /** \brief Type identifier
   * This is an index into a table in the Env. It is currently limited to
   * 65,536 different type identifiers.
   * For a non-array integer type, this maps directly to the identity of the enum.
   * For a non-array tuple type, this maps directly to the definition of a tuple.
   * For an array type, it maps to a tuple of type identifiers.
   */
  unsigned int _typeId : 16;
  /// Number of array dimensions
  unsigned int _dim : 3;

public:
  /// Default constructor
  Type()
      : _bt(BT_UNKNOWN),
        _ti(TI_PAR),
        _tiExplicit(EXPL_NO),
        _st(ST_PLAIN),
        _ot(OT_PRESENT),
        _otExplicit(EXPL_NO),
        _cv(CV_NO),
        _at(AT_NO),
        _typeId(0),
        _dim(1) {}

  /// Access basic type
  BaseType bt() const { return static_cast<BaseType>(_bt); }
  /// Set basic type
  void bt(const BaseType& b) { _bt = b; }

  /// Access type-inst
  Inst ti() const { return static_cast<Inst>(_ti); }
  /// Set type-inst
  void ti(const Inst& t) {
    // TI of tuple should not be changed after typechecking
    assert(!structBT() || typeId() == 0);
    _ti = t;
    if (t == TI_VAR) {
      _cv = CV_YES;
    }
  }
  bool tiExplicit() const { return static_cast<ExplicitType>(_tiExplicit) == EXPL_YES; }
  void tiExplicit(bool b) { _tiExplicit = (b ? EXPL_YES : EXPL_NO); }

  /// Access set type
  SetType st() const { return static_cast<SetType>(_st); }
  /// Set set type
  void st(const SetType& s) {
    assert(s == ST_PLAIN || !structBT() ||
           typeId() == 0);  // Cannot create "set of tuple" after typechecking
    _st = s;
  }

  /// Access opt type
  OptType ot() const { return static_cast<OptType>(_ot); }
  /// Set opt type
  void ot(const OptType& o) {
    assert(o == OT_PRESENT || !structBT() ||
           typeId() == 0);  // Cannot create "opt tuple" after typechecking
    _ot = o;
  }
  bool otExplicit() const { return static_cast<ExplicitType>(_otExplicit) == EXPL_YES; }
  void otExplicit(bool b) { _otExplicit = (b ? EXPL_YES : EXPL_NO); }

  /// Access var-in-par type
  bool cv() const { return static_cast<ContainsVarType>(_cv) == CV_YES; }
  /// Set var-in-par type
  void cv(bool b) { _cv = (b ? CV_YES : CV_NO); }

  /// Access any type
  bool any() const { return static_cast<AnyType>(_at) == AT_YES; }
  /// Set any type
  void any(bool b) { _at = (b ? AT_YES : AT_NO); }

  /// Access enum identifier
  unsigned int typeId() const { return _typeId; }
  /// Set enum identifier
  void typeId(unsigned int eid) { _typeId = eid; }

  /// Access dimensions
  int dim() const { return static_cast<signed int>(_dim) - 1; }
  /// Set dimensions
  void dim(int d) {
    // Cannot change the dimension of a type that uses typeId, as the typeId would have to be
    // changed (and registered) as well. (see elemType and arrType)
    assert(typeId() == 0 || dim() == d);
    assert(d >= -1 && d < 7);
    _dim = static_cast<unsigned int>(d + 1);
  }

protected:
  /// Constructor
  Type(const Inst& ti, const BaseType& bt, const SetType& st, unsigned int typeId, int dim)
      : _bt(bt),
        _ti(ti),
        _tiExplicit(EXPL_NO),
        _st(st),
        _ot(OT_PRESENT),
        _otExplicit(EXPL_NO),
        _cv(ti == TI_VAR ? CV_YES : CV_NO),
        _at(AT_NO),
        _typeId(typeId),
        _dim(static_cast<unsigned int>(dim + 1)) {
    assert(dim >= -1 && dim < 7);
  }

public:
  static Type parint(int dim = 0) { return Type(TI_PAR, BT_INT, ST_PLAIN, 0, dim); }
  static Type parenum(unsigned int enumId, int dim = 0) {
    return Type(TI_PAR, BT_INT, ST_PLAIN, enumId, dim);
  }
  static Type tuple(unsigned int typeId = 0, int dim = 0) {
    return Type(TI_PAR, BT_TUPLE, ST_PLAIN, typeId, dim);
  }
  static Type record(unsigned int typeId = 0, int dim = 0) {
    return Type(TI_PAR, BT_RECORD, ST_PLAIN, typeId, dim);
  }
  static Type parbool(int dim = 0) { return Type(TI_PAR, BT_BOOL, ST_PLAIN, 0, dim); }
  static Type parfloat(int dim = 0) { return Type(TI_PAR, BT_FLOAT, ST_PLAIN, 0, dim); }
  static Type parstring(int dim = 0) { return Type(TI_PAR, BT_STRING, ST_PLAIN, 0, dim); }
  static Type partop(int dim = 0) { return Type(TI_PAR, BT_TOP, ST_PLAIN, 0, dim); }
  static Type ann(int dim = 0) { return Type(TI_PAR, BT_ANN, ST_PLAIN, 0, dim); }
  static Type parsetint(int dim = 0) { return Type(TI_PAR, BT_INT, ST_SET, 0, dim); }
  static Type parsetenum(unsigned int enumId, int dim = 0) {
    return Type(TI_PAR, BT_INT, ST_SET, enumId, dim);
  }
  static Type parsetbool(int dim = 0) { return Type(TI_PAR, BT_BOOL, ST_SET, 0, dim); }
  static Type parsetfloat(int dim = 0) { return Type(TI_PAR, BT_FLOAT, ST_SET, 0, dim); }
  static Type parsetstring(int dim = 0) { return Type(TI_PAR, BT_STRING, ST_SET, 0, dim); }
  static Type varint(int dim = 0) { return Type(TI_VAR, BT_INT, ST_PLAIN, 0, dim); }
  static Type varenumint(unsigned int enumId, int dim = 0) {
    return Type(TI_VAR, BT_INT, ST_PLAIN, enumId, dim);
  }
  static Type varbool(int dim = 0) { return Type(TI_VAR, BT_BOOL, ST_PLAIN, 0, dim); }
  static Type varfloat(int dim = 0) { return Type(TI_VAR, BT_FLOAT, ST_PLAIN, 0, dim); }
  static Type varsetint(int dim = 0) { return Type(TI_VAR, BT_INT, ST_SET, 0, dim); }
  static Type varbot(int dim = 0) { return Type(TI_VAR, BT_BOT, ST_PLAIN, 0, dim); }
  static Type bot(int dim = 0) { return Type(TI_PAR, BT_BOT, ST_PLAIN, 0, dim); }
  static Type top(int dim = 0) { return Type(TI_PAR, BT_TOP, ST_PLAIN, 0, dim); }
  static Type vartop(int dim = 0) { return Type(TI_VAR, BT_TOP, ST_PLAIN, 0, dim); }
  static Type optvartop(int dim = 0) {
    Type t(TI_VAR, BT_TOP, ST_PLAIN, 0, dim);
    t._ot = OT_OPTIONAL;
    return t;
  }
  static Type optpartop(int dim = 0) {
    Type t(TI_PAR, BT_TOP, ST_PLAIN, 0, dim);
    t._ot = OT_OPTIONAL;
    return t;
  }
  static Type mkAny(int dim = 0) {
    Type t(TI_VAR, BT_TOP, ST_PLAIN, 0, dim);
    t._ot = OT_OPTIONAL;
    t._at = AT_YES;
    return t;
  }

  static Type unboxedint;
  static Type unboxedfloat;

  bool isunknown() const { return bt() == BT_UNKNOWN; }
  bool isplain() const { return dim() == 0 && st() == ST_PLAIN && ot() == OT_PRESENT; }
  bool isint() const { return dim() == 0 && st() == ST_PLAIN && bt() == BT_INT; }
  bool isbot() const { return bt() == BT_BOT; }
  bool istop() const { return bt() == BT_TOP; }
  bool isfloat() const { return dim() == 0 && st() == ST_PLAIN && bt() == BT_FLOAT; }
  bool isbool() const { return dim() == 0 && st() == ST_PLAIN && bt() == BT_BOOL; }
  bool isstring() const { return isplain() && bt() == BT_STRING; }
  bool istuple() const { return isplain() && bt() == BT_TUPLE; }
  bool isrecord() const { return isplain() && bt() == BT_RECORD; }
  bool isvar() const { return ti() != TI_PAR; }
  bool isvarbool() const {
    return ti() == TI_VAR && dim() == 0 && st() == ST_PLAIN && bt() == BT_BOOL &&
           ot() == OT_PRESENT;
  }
  bool isvarfloat() const {
    return ti() == TI_VAR && dim() == 0 && st() == ST_PLAIN && bt() == BT_FLOAT &&
           ot() == OT_PRESENT;
  }
  bool isvarint() const {
    return ti() == TI_VAR && dim() == 0 && st() == ST_PLAIN && bt() == BT_INT && ot() == OT_PRESENT;
  }
  bool isPar() const { return ti() == TI_PAR; }
  bool isOpt() const { return ot() == OT_OPTIONAL; }
  bool isPresent() const { return ot() == OT_PRESENT; }
  bool isSet() const { return dim() == 0 && st() == ST_SET; }
  bool isIntSet() const { return isSet() && (bt() == BT_INT || bt() == BT_BOT); }
  bool isBoolSet() const { return isSet() && (bt() == BT_BOOL || bt() == BT_BOT); }
  bool isFloatSet() const { return isSet() && (bt() == BT_FLOAT || bt() == BT_BOT); }
  bool isAnn() const { return isplain() && bt() == BT_ANN; }
  bool isIntArray() const {
    return dim() == 1 && st() == ST_PLAIN && ot() == OT_PRESENT && bt() == BT_INT;
  }
  bool isBoolArray() const {
    return dim() == 1 && st() == ST_PLAIN && ot() == OT_PRESENT && bt() == BT_BOOL;
  }
  bool isIntSetArray() const { return dim() == 1 && st() == ST_SET && bt() == BT_INT; }
  bool isOptBot() const { return st() == ST_PLAIN && bt() == BT_BOT && ot() == OT_OPTIONAL; }
  bool structBT() const { return bt() == BT_TUPLE || bt() == BT_RECORD; }

  bool operator==(const Type& t) const {
    return ti() == t.ti() && bt() == t.bt() && st() == t.st() && ot() == t.ot() &&
           dim() == t.dim() && (!structBT() || typeId() == t.typeId());
  }
  bool operator!=(const Type& t) const { return !this->operator==(t); }
  // protected:

  int toInt() const {
    // WARNING: this method currently erases the flags that indicate whether type modifier are
    // explicitly added
    return +((1 - static_cast<int>(_st)) << 26) + (static_cast<int>(_bt) << 22) +
           (static_cast<int>(_ti) << 21) + (static_cast<int>(_ot) << 20) +
           (static_cast<int>(_typeId) << 4) + (dim() == -1 ? 1 : (dim() == 0 ? 0 : dim() + 1));
  }
  static Type fromInt(int i) {
    Type t;
    t._st = 1 - static_cast<SetType>((i >> 26) & 0x1);
    t._bt = static_cast<BaseType>((i >> 22) & 0xF);
    t._ti = static_cast<Inst>((i >> 21) & 0x1);
    t._ot = static_cast<OptType>((i >> 20) & 0x1);
    int dim = (i & 0x7);
    dim = (dim == 0 ? 0 : (dim == 1 ? -1 : dim - 1));
    t.dim(dim);
    t._typeId = static_cast<unsigned int>((i >> 4) & 0xFFF);
    return t;
  }

  /// Turn type or all fields of a tuple type into a parameter types
  void mkPar(EnvI& env);
  /// Turn type or all fields of a tuple type into a parameter types
  void mkVar(EnvI& env);
  /// Turn type or all fields of a tuple type into a optional types
  void mkOpt(EnvI& env);
  /// Turn type or all fields of a tuple type into a present types
  void mkPresent(EnvI& env);
  /// Go through the types (of tuple field type) in this order: var opt, var, par opt, par
  /// Returns whether the decrement operation was succesful (false when type is already par)
  bool decrement(EnvI& env);

  /// Return true if this type is varifiable
  bool isVarifiable(const EnvI& env) const;
  /// Return true if the predicate holds for this type or any nested type for structs
  bool contains(const EnvI& env, std::function<bool(const Type)> p) const;

  /// A helper function that returns the Type for a element of te current array Type
  /// NOTE: generally this is the same type with `_dim = 0`, but when typeId is set, the correct
  /// element typeId must be extracted.
  Type elemType(const EnvI& env) const;

  /// A helper function that merges the dimensions of dimTy into elemTy
  static Type arrType(EnvI& env, const Type& dimTy, const Type& elemTy);

  std::string toString(const EnvI& env) const;
  std::string simpleToString() const;

  /// Check if \a bt0 is a subtype of \a bt1
  static bool btSubtype(const EnvI& env, const Type& t0, const Type& t1, bool strictEnums);

  /// Get the most specific supertype of the given types, or unknown if there is none
  static Type commonType(EnvI& env, Type t1, Type t2);

  /// Check if this type is a subtype of \a t
  bool isSubtypeOf(const EnvI& env, const Type& t, bool strictEnums) const {
    if (dim() == 0 && t.dim() != 0 && st() == ST_SET && t.st() == ST_PLAIN && bt() != BT_FLOAT &&
        (bt() == BT_BOT || btSubtype(env, *this, t, false) || t.bt() == BT_TOP) && ti() == TI_PAR &&
        (ot() == OT_PRESENT || ot() == t.ot())) {
      return true;
    }
    // either same dimension or t has variable dimension
    if (dim() != t.dim() && (dim() == 0 || t.dim() != -1)) {
      return false;
    }
    if (any()) {
      return t.any();
    }
    if (t.any()) {
      return true;
    }
    // same type, this is present or both optional
    if (ti() == t.ti() && btSubtype(env, *this, t, strictEnums) && st() == t.st()) {
      return ot() == OT_PRESENT || ot() == t.ot();
    }
    // this is par other than that same type as t
    if (ti() == TI_PAR && btSubtype(env, *this, t, strictEnums) && st() == t.st()) {
      return ot() == OT_PRESENT || ot() == t.ot();
    }
    if (ti() == TI_PAR && t.bt() == BT_BOT) {
      return true;
    }
    if ((ti() == t.ti() || ti() == TI_PAR) && bt() == BT_BOT &&
        (st() == t.st() || st() == ST_PLAIN)) {
      return ot() == OT_PRESENT || ot() == t.ot();
    }
    if (t.bt() == BT_TOP && (ot() == OT_PRESENT || ot() == t.ot()) &&
        (t.st() == ST_PLAIN || st() == t.st()) && (ti() == TI_PAR || t.ti() == TI_VAR)) {
      return true;
    }
    return false;
  }

  /// Compare types
  int cmp(const Type& t) const { return toInt() < t.toInt() ? -1 : (toInt() > t.toInt() ? 1 : 0); }
};

};  // namespace MiniZinc
