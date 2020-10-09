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
#include <sstream>
#include <string>

namespace MiniZinc {

class EnvI;

/// Type of a MiniZinc expression
class Type {
public:
  /// Type-inst
  enum TypeInst { TI_PAR, TI_VAR };
  /// Basic type
  enum BaseType { BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN, BT_TOP, BT_BOT, BT_UNKNOWN };
  /// Whether the expression is plain or set
  enum SetType { ST_PLAIN, ST_SET };
  /// Whether the expression is normal or optional
  enum OptType { OT_PRESENT, OT_OPTIONAL };
  /// Whether the par expression contains a var argument
  enum ContainsVarType { CV_NO, CV_YES };

private:
  unsigned int _ti : 1;
  unsigned int _bt : 4;
  unsigned int _st : 1;
  unsigned int _ot : 1;
  unsigned int _cv : 1;
  /** \brief Enumerated type identifier
   * This is an index into a table in the Env. It is currently limited to
   * 4095 different enumerated type identifiers.
   * For a non-array type, this maps directly to the identity of the enum.
   * For an array type, it maps to a tuple of enum identities.
   */
  unsigned int _enumId : 12;
  /// Number of array dimensions
  signed int _dim : 7;

public:
  /// Default constructor
  Type()
      : _ti(TI_PAR),
        _bt(BT_UNKNOWN),
        _st(ST_PLAIN),
        _ot(OT_PRESENT),
        _cv(CV_NO),
        _enumId(0),
        _dim(0) {}

  /// Access type-inst
  TypeInst ti() const { return static_cast<TypeInst>(_ti); }
  /// Set type-inst
  void ti(const TypeInst& t) {
    _ti = t;
    if (t == TI_VAR) {
      _cv = CV_YES;
    }
  }

  /// Access basic type
  BaseType bt() const { return static_cast<BaseType>(_bt); }
  /// Set basic type
  void bt(const BaseType& b) { _bt = b; }

  /// Access set type
  SetType st() const { return static_cast<SetType>(_st); }
  /// Set set type
  void st(const SetType& s) { _st = s; }

  /// Access opt type
  OptType ot() const { return static_cast<OptType>(_ot); }
  /// Set opt type
  void ot(const OptType& o) { _ot = o; }

  /// Access var-in-par type
  bool cv() const { return static_cast<ContainsVarType>(_cv) == CV_YES; }
  /// Set var-in-par type
  void cv(bool b) { _cv = b ? CV_YES : CV_NO; }

  /// Access enum identifier
  unsigned int enumId() const { return _enumId; }
  /// Set enum identifier
  void enumId(unsigned int eid) { _enumId = eid; }

  /// Access dimensions
  int dim() const { return _dim; }
  /// Set dimensions
  void dim(int d) {
    _dim = d;
    assert(_dim == d);
  }

protected:
  /// Constructor
  Type(const TypeInst& ti, const BaseType& bt, const SetType& st, unsigned int enumId, int dim)
      : _ti(ti),
        _bt(bt),
        _st(st),
        _ot(OT_PRESENT),
        _cv(ti == TI_VAR ? CV_YES : CV_NO),
        _enumId(enumId),
        _dim(dim) {}

public:
  static Type parint(int dim = 0) { return Type(TI_PAR, BT_INT, ST_PLAIN, 0, dim); }
  static Type parenum(unsigned int enumId, int dim = 0) {
    return Type(TI_PAR, BT_INT, ST_PLAIN, enumId, dim);
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

  static Type unboxedint;
  static Type unboxedfloat;

  bool isunknown() const { return bt() == BT_UNKNOWN; }
  bool isplain() const { return _dim == 0 && st() == ST_PLAIN && ot() == OT_PRESENT; }
  bool isint() const { return _dim == 0 && st() == ST_PLAIN && bt() == BT_INT; }
  bool isbot() const { return bt() == BT_BOT; }
  bool isfloat() const { return _dim == 0 && st() == ST_PLAIN && bt() == BT_FLOAT; }
  bool isbool() const { return _dim == 0 && st() == ST_PLAIN && bt() == BT_BOOL; }
  bool isstring() const { return isplain() && bt() == BT_STRING; }
  bool isvar() const { return ti() != TI_PAR; }
  bool isvarbool() const {
    return ti() == TI_VAR && _dim == 0 && st() == ST_PLAIN && bt() == BT_BOOL && ot() == OT_PRESENT;
  }
  bool isvarfloat() const {
    return ti() == TI_VAR && _dim == 0 && st() == ST_PLAIN && bt() == BT_FLOAT &&
           ot() == OT_PRESENT;
  }
  bool isvarint() const {
    return ti() == TI_VAR && _dim == 0 && st() == ST_PLAIN && bt() == BT_INT && ot() == OT_PRESENT;
  }
  bool isPar() const { return ti() == TI_PAR; }
  bool isOpt() const { return ot() == OT_OPTIONAL; }
  bool isPresent() const { return ot() == OT_PRESENT; }
  bool isSet() const { return _dim == 0 && st() == ST_SET; }
  bool isIntSet() const { return isSet() && (bt() == BT_INT || bt() == BT_BOT); }
  bool isBoolSet() const { return isSet() && (bt() == BT_BOOL || bt() == BT_BOT); }
  bool isFloatSet() const { return isSet() && (bt() == BT_FLOAT || bt() == BT_BOT); }
  bool isAnn() const { return isplain() && bt() == BT_ANN; }
  bool isIntArray() const {
    return _dim == 1 && st() == ST_PLAIN && ot() == OT_PRESENT && bt() == BT_INT;
  }
  bool isBoolArray() const {
    return _dim == 1 && st() == ST_PLAIN && ot() == OT_PRESENT && bt() == BT_BOOL;
  }
  bool isIntSetArray() const { return _dim == 1 && st() == ST_SET && bt() == BT_INT; }

  bool operator==(const Type& t) const {
    return ti() == t.ti() && bt() == t.bt() && st() == t.st() && ot() == t.ot() && _dim == t._dim;
  }
  bool operator!=(const Type& t) const { return !this->operator==(t); }
  // protected:

  int toInt() const {
    return +((1 - static_cast<int>(_st)) << 28) + (static_cast<int>(_bt) << 24) +
           (static_cast<int>(_ti) << 21) + (static_cast<int>(_ot) << 20) +
           (static_cast<int>(_enumId) << 8) + (_dim == -1 ? 1 : (_dim == 0 ? 0 : _dim + 1));
  }
  static Type fromInt(int i) {
    Type t;
    t._st = 1 - static_cast<SetType>((i >> 28) & 0x1);
    t._bt = static_cast<BaseType>((i >> 24) & 0xF);
    t._ti = static_cast<TypeInst>((i >> 21) & 0x7);
    t._ot = static_cast<OptType>((i >> 20) & 0x1);
    t._enumId = static_cast<unsigned int>((i >> 8) & 0xFFF);
    int dim = (i & 0x7F);
    t._dim = (dim == 0 ? 0 : (dim == 1 ? -1 : dim - 1));
    return t;
  }
  std::string toString(EnvI& env) const;
  std::string nonEnumToString() const;

  /// Check if \a bt0 is a subtype of \a bt1
  static bool btSubtype(const Type& t0, const Type& t1, bool strictEnums) {
    if (t0.bt() == t1.bt() &&
        (!strictEnums || t0.dim() != 0 || (t0.enumId() == t1.enumId() || t1.enumId() == 0))) {
      return true;
    }
    switch (t0.bt()) {
      case BT_BOOL:
        return (t1.bt() == BT_INT || t1.bt() == BT_FLOAT);
      case BT_INT:
        return t1.bt() == BT_FLOAT;
      default:
        return false;
    }
  }

  /// Check if this type is a subtype of \a t
  bool isSubtypeOf(const Type& t, bool strictEnums) const {
    if (_dim == 0 && t._dim != 0 && st() == ST_SET && t.st() == ST_PLAIN && bt() != BT_FLOAT &&
        (bt() == BT_BOT || btSubtype(*this, t, false) || t.bt() == BT_TOP) && ti() == TI_PAR &&
        (ot() == OT_PRESENT || ot() == t.ot())) {
      return true;
    }
    // either same dimension or t has variable dimension
    if (_dim != t._dim && (_dim == 0 || t._dim != -1)) {
      return false;
    }
    // same type, this is present or both optional
    if (ti() == t.ti() && btSubtype(*this, t, strictEnums) && st() == t.st()) {
      return ot() == OT_PRESENT || ot() == t.ot();
    }
    // this is par other than that same type as t
    if (ti() == TI_PAR && btSubtype(*this, t, strictEnums) && st() == t.st()) {
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
