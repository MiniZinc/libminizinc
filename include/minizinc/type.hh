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

  class Type {
  public:
    enum TypeInst { TI_PAR, TI_VAR, TI_SVAR, TI_ANY };
    enum BaseType { BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN,
                    BT_BOT, BT_UNKNOWN };
    enum SetType { ST_PLAIN, ST_SET };
    TypeInst _ti : 3;
    BaseType _bt : 3;
    SetType _st  : 1;
    int _dim : 20;
    Type(void) : _ti(TI_PAR), _bt(BT_UNKNOWN), _st(ST_PLAIN), _dim(0) {}
  protected:
    Type(const TypeInst& ti, const BaseType& bt, const SetType& st,
         int dim)
      : _ti(ti), _bt(bt), _st(st), _dim(dim) {}
  public:
    static Type any(int dim=0) {
      return Type(TI_ANY,BT_BOT,ST_PLAIN,dim);
    }
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
    static Type bot(int dim=0) {
      return Type(TI_PAR,BT_BOT,ST_PLAIN,dim);
    }

    bool isunknown(void) const { return _bt==BT_UNKNOWN; }
    bool isany(void) const { return _ti==TI_ANY; }
    bool isplain(void) const { return _dim==0 && _st==ST_PLAIN; }
    bool isint(void) const { return isplain() && _bt==BT_INT; }
    bool isfloat(void) const { return isplain() && _bt==BT_FLOAT; }
    bool isbool(void) const { return isplain() && _bt==BT_BOOL; }
    bool isstring(void) const { return isplain() && _bt==BT_STRING; }
    bool isvar(void) const { return _ti!=TI_PAR; }
    bool issvar(void) const { return _ti==TI_SVAR; }
    bool ispar(void) const { return _ti==TI_PAR; }
    bool isset(void) const { return _dim==0 && _st==ST_SET; }
    bool isintset(void) const { return isset() && _bt==BT_INT; }
    bool isann(void) const { return isplain() && _bt==BT_ANN; }
    bool isintarray(void) const {
      return _dim==1 && _st==ST_PLAIN && _bt==BT_INT;
    }
    bool isboolarray(void) const {
      return _dim==1 && _st==ST_PLAIN && _bt==BT_BOOL;
    }
    bool isintsetarray(void) const {
      return _dim==1 && _st==ST_SET && _bt==BT_INT;
    }

    bool operator== (const Type& t) const {
      return _ti==t._ti && _bt==t._bt && _st==t._st && _dim==t._dim;
    }
    bool operator!= (const Type& t) const {
      return !this->operator==(t);
    }

  // protected:
    int toInt(void) const {
      return
        (static_cast<int>(_ti)<<24)
      + (static_cast<int>(_bt)<<21)
      + (static_cast<int>(_st)<<20)
      + _dim;
    }
    static Type fromInt(int i) {
      Type t;
      t._ti = static_cast<TypeInst>((i >> 24) & 0x7);
      t._bt = static_cast<BaseType>((i >> 21) & 0x7);
      t._st = static_cast<SetType>((i >> 20) & 0x1);
      t._dim = i & 0xFFFFF;
      return t;
    }
    std::string toString(void) const {
      std::ostringstream oss;
      if (_dim>0)
        oss<<"array["<<_dim<<"] of ";
      if (_dim<0)
        oss<<"array[$_] of ";
      switch (_ti) {
        case TI_PAR: oss<<"par "; break;
        case TI_VAR: oss<<"var "; break;
        case TI_SVAR: oss<<"svar "; break;
        case TI_ANY: oss<<"any "; break;
      }
      if (_st==ST_SET) oss<<"set of ";
      switch (_bt) {
        case BT_INT: oss<<"int"; break;
        case BT_BOOL: oss<<"bool"; break;
        case BT_FLOAT: oss<<"float"; break;
        case BT_STRING: oss<<"string"; break;
        case BT_ANN: oss<<"ann"; break;
        case BT_BOT: oss<<"bot"; break;
        case BT_UNKNOWN: oss<<"??? "; break;
      }
      return oss.str();
    }
  public:
    bool isSubtypeOf(const Type& t) const {
      // either same dimension or t has variable dimension
      if (_dim!=t._dim && (_dim==0 || t._dim!=-1))
        return false;
      // same type
      if (_ti==t._ti && _bt==t._bt && _st==t._st)
        return true;
      // this is par or svar, other than that same type as t
      if ((_ti==TI_PAR || _ti==TI_SVAR) && _bt==t._bt && _st==t._st)
        return true;
      // t is svar, other than that same type as this
      if (t._ti==TI_SVAR && _bt==t._bt && _st==t._st)
        return true;
      if (t._ti==TI_ANY)
        return true;
      if ( (_ti==TI_PAR || _ti==TI_SVAR) && t._bt==BT_BOT)
        return true;
      if ( _bt==BT_BOT && _st==t._st)
        return true;
      return false;
    }

    int cmp(const Type& t) const {
      return toInt()<t.toInt() ? -1 : (toInt()>t.toInt() ? 1 : 0);
    }

  };
  
};

#endif
