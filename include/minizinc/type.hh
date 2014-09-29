/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_TYPE_HH__
#define __MINIZINC_TYPE_HH__

#include <string>
#include <sstream>

namespace MiniZinc {

  /// Type of a MiniZinc expression
  class Type {
  public:
    /// Type-inst
    enum TypeInst { TI_PAR, TI_VAR, TI_SVAR };
    /// Basic type
    enum BaseType { BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN,
                    BT_BOT, BT_TOP, BT_UNKNOWN };
    /// Whether the expression is plain or set
    enum SetType { ST_PLAIN, ST_SET };
    /// Whether the expression is normal or optional
    enum OptType { OT_PRESENT, OT_OPTIONAL };
  private:
    unsigned int _ti : 3;
    unsigned int _bt : 4;
    unsigned int _st  : 1;
    unsigned int _ot  : 1;
    /// Number of array dimensions
    int _dim : 20;
  public:
    /// Default constructor
    Type(void) : _ti(TI_PAR), _bt(BT_UNKNOWN), _st(ST_PLAIN),
                 _ot(OT_PRESENT), _dim(0) {}
    
    /// Access type-inst
    TypeInst ti(void) const { return static_cast<TypeInst>(_ti); }
    /// Set type-inst
    void ti(const TypeInst& t) { _ti = t; }

    /// Access basic type
    BaseType bt(void) const { return static_cast<BaseType>(_bt); }
    /// Set basic type
    void bt(const BaseType& b) { _bt = b; }
    
    /// Access set type
    SetType st(void) const { return static_cast<SetType>(_st); }
    /// Set set type
    void st(const SetType& s) { _st = s; }
    
    /// Access opt type
    OptType ot(void) const { return static_cast<OptType>(_ot); }
    /// Set opt type
    void ot(const OptType& o) { _ot = o; }
    
    /// Access dimensions
    int dim(void) const { return _dim; }
    /// Set dimensions
    void dim(int d) { _dim = d; }

  protected:
    /// Constructor
    Type(const TypeInst& ti, const BaseType& bt, const SetType& st,
         int dim)
      : _ti(ti), _bt(bt), _st(st), _ot(OT_PRESENT), _dim(dim) {}
  public:
    static Type parint(int dim=0) {
      return Type(TI_PAR,BT_INT,ST_PLAIN,dim);
    }
    static Type parbool(int dim=0) {
      return Type(TI_PAR,BT_BOOL,ST_PLAIN,dim);
    }
    static Type parfloat(int dim=0) {
      return Type(TI_PAR,BT_FLOAT,ST_PLAIN,dim);
    }
    static Type parstring(int dim=0) {
      return Type(TI_PAR,BT_STRING,ST_PLAIN,dim);
    }
    static Type ann(int dim=0) {
      return Type(TI_PAR,BT_ANN,ST_PLAIN,dim);
    }
    static Type parsetint(int dim=0) {
      return Type(TI_PAR,BT_INT,ST_SET,dim);
    }
    static Type parsetbool(int dim=0) {
      return Type(TI_PAR,BT_BOOL,ST_SET,dim);
    }
    static Type parsetfloat(int dim=0) {
      return Type(TI_PAR,BT_FLOAT,ST_SET,dim);
    }
    static Type parsetstring(int dim=0) {
      return Type(TI_PAR,BT_STRING,ST_SET,dim);
    }
    static Type varint(int dim=0) {
      return Type(TI_VAR,BT_INT,ST_PLAIN,dim);
    }
    static Type varbool(int dim=0) {
      return Type(TI_VAR,BT_BOOL,ST_PLAIN,dim);
    }
    static Type varfloat(int dim=0) {
      return Type(TI_VAR,BT_FLOAT,ST_PLAIN,dim);
    }
    static Type varsetint(int dim=0) {
      return Type(TI_VAR,BT_INT,ST_SET,dim);
    }
    static Type varbot(int dim=0) {
      return Type(TI_VAR,BT_BOT,ST_PLAIN,dim);
    }
    static Type bot(int dim=0) {
      return Type(TI_PAR,BT_BOT,ST_PLAIN,dim);
    }
    static Type top(int dim=0) {
      return Type(TI_PAR,BT_TOP,ST_PLAIN,dim);
    }
    static Type vartop(int dim=0) {
      return Type(TI_VAR,BT_TOP,ST_PLAIN,dim);
    }
    static Type optvartop(int dim=0) {
      Type t(TI_VAR,BT_TOP,ST_PLAIN,dim);
      t._ot = OT_OPTIONAL;
      return t;
    }

    bool isunknown(void) const { return _bt==BT_UNKNOWN; }
    bool isplain(void) const {
      return _dim==0 && _st==ST_PLAIN && _ot==OT_PRESENT;
    }
    bool isint(void) const { return _dim==0 && _st==ST_PLAIN && _bt==BT_INT; }
    bool isbot(void) const { return _bt==BT_BOT; }
    bool isfloat(void) const { return _dim==0 && _st==ST_PLAIN && _bt==BT_FLOAT; }
    bool isbool(void) const { return _dim==0 && _st==ST_PLAIN && _bt==BT_BOOL; }
    bool isstring(void) const { return isplain() && _bt==BT_STRING; }
    bool isvar(void) const { return _ti!=TI_PAR; }
    bool isvarint(void) const { return ti!=TI_PAR && _dim==0 && _st==ST_PLAIN && _bt==BT_INT; }
    bool issvar(void) const { return _ti==TI_SVAR; }
    bool ispar(void) const { return _ti==TI_PAR; }
    bool isopt(void) const { return _ot==OT_OPTIONAL; }
    bool ispresent(void) const { return _ot==OT_PRESENT; }
    bool isset(void) const { return _dim==0 && _st==ST_SET; }
    bool isintset(void) const {
      return isset() && (_bt==BT_INT || _bt==BT_BOT);
    }
    bool isann(void) const { return isplain() && _bt==BT_ANN; }
    bool isintarray(void) const {
      return _dim==1 && _st==ST_PLAIN && _ot==OT_PRESENT && _bt==BT_INT;
    }
    bool isboolarray(void) const {
      return _dim==1 && _st==ST_PLAIN && _ot==OT_PRESENT && _bt==BT_BOOL;
    }
    bool isintsetarray(void) const {
      return _dim==1 && _st==ST_SET && _bt==BT_INT;
    }

    bool operator== (const Type& t) const {
      return _ti==t._ti && _bt==t._bt && _st==t._st &&
             _ot==t._ot && _dim==t._dim;
    }
    bool operator!= (const Type& t) const {
      return !this->operator==(t);
    }
  // protected:

    /* We add 1 to _dim in toInt to ensure that it is non-negative
       (and subtract it again in fromInt). */
    int toInt(void) const {
      return
      + (static_cast<int>(_ot)<<28)
      + (static_cast<int>(_ti)<<25)
      + (static_cast<int>(_bt)<<21)
      + (static_cast<int>(_st)<<20)
      + (_dim + 1);
    }
    static Type fromInt(int i) {
      Type t;
      t._ot = static_cast<OptType>((i >> 28) & 0x1);
      t._ti = static_cast<TypeInst>((i >> 25) & 0x7);
      t._bt = static_cast<BaseType>((i >> 21) & 0xF);
      t._st = static_cast<SetType>((i >> 20) & 0x1);
      t._dim = (i & 0xFFFFF) - 1;
      return t;
    }
    std::string toString(void) const {
      std::ostringstream oss;
      if (_dim>0)
        oss<<"array["<<_dim<<"] of ";
      if (_dim<0)
        oss<<"array[$_] of ";
      if (_ot==OT_OPTIONAL) oss<<"opt ";
      switch (_ti) {
        case TI_PAR: oss<<"par "; break;
        case TI_VAR: oss<<"var "; break;
        case TI_SVAR: oss<<"svar "; break;
      }
      if (_st==ST_SET) oss<<"set of ";
      switch (_bt) {
        case BT_INT: oss<<"int"; break;
        case BT_BOOL: oss<<"bool"; break;
        case BT_FLOAT: oss<<"float"; break;
        case BT_STRING: oss<<"string"; break;
        case BT_ANN: oss<<"ann"; break;
        case BT_BOT: oss<<"bot"; break;
        case BT_TOP: oss<<"top"; break;
        case BT_UNKNOWN: oss<<"??? "; break;
      }
      return oss.str();
    }
  public:
    /// Check if this type is a subtype of \a t
    bool isSubtypeOf(const Type& t) const {
      // either same dimension or t has variable dimension
      if (_dim!=t._dim && (_dim==0 || t._dim!=-1))
        return false;
      // same type, this is present or both optional
      if (_ti==t._ti && _bt==t._bt && _st==t._st)
        return _ot==OT_PRESENT || _ot==t._ot;
      // this is par or svar, other than that same type as t
      if ((_ti==TI_PAR || _ti==TI_SVAR) && _bt==t._bt && _st==t._st)
        return _ot==OT_PRESENT || _ot==t._ot;
      // t is svar, other than that same type as this
      if (t._ti==TI_SVAR && _bt==t._bt && _st==t._st)
        return _ot==OT_PRESENT || _ot==t._ot;
      if ( (_ti==TI_PAR || _ti==TI_SVAR) && t._bt==BT_BOT)
        return true;
      if ( _bt==BT_BOT && _st==t._st)
        return _ot==OT_PRESENT || _ot==t._ot;
      if (t._bt==BT_TOP && (_ot==OT_PRESENT || _ot==t._ot) &&
          (t._st==ST_PLAIN || _st==t._st) &&
          (_ti==TI_PAR || t._ti==TI_VAR))
        return true;
      return false;
    }

    /// Compare types
    int cmp(const Type& t) const {
      return toInt()<t.toInt() ? -1 : (toInt()>t.toInt() ? 1 : 0);
    }

  };
  
};

#endif
