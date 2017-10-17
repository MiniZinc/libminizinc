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
#include <cassert>

namespace MiniZinc {

  class EnvI;
  
  /// Type of a MiniZinc expression
  class Type {
  public:
    /// Type-inst
    enum TypeInst { TI_PAR, TI_VAR };
    /// Basic type
    enum BaseType { BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN,
                    BT_TOP, BT_BOT, BT_UNKNOWN };
    /// Whether the expression is plain or set
    enum SetType { ST_PLAIN, ST_SET };
    /// Whether the expression is normal or optional
    enum OptType { OT_PRESENT, OT_OPTIONAL };
    /// Whether the par expression contains a var argument
    enum ContainsVarType { CV_NO, CV_YES };
  private:
    unsigned int _ti : 1;
    unsigned int _bt : 4;
    unsigned int _st  : 1;
    unsigned int _ot  : 1;
    unsigned int _cv  : 1;
    /** \brief Enumerated type identifier
     * This is an index into a table in the Env. It is currently limited to
     * 4095 different enumerated type identifiers.
     * For a non-array type, this maps directly to the identity of the enum.
     * For an array type, it maps to a tuple of enum identities.
     */
    unsigned int _enumId : 12;
    /// Number of array dimensions
    int _dim : 7;
  public:
    /// Default constructor
    Type(void) : _ti(TI_PAR), _bt(BT_UNKNOWN), _st(ST_PLAIN),
                 _ot(OT_PRESENT), _cv(CV_NO), _enumId(0), _dim(0) {}
    
    /// Access type-inst
    TypeInst ti(void) const { return static_cast<TypeInst>(_ti); }
    /// Set type-inst
    void ti(const TypeInst& t) {
      _ti = t;
      if (t==TI_VAR)
        _cv=CV_YES;
    }

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
    
    /// Access var-in-par type
    bool cv(void) const { return static_cast<ContainsVarType>(_cv) == CV_YES; }
    /// Set var-in-par type
    void cv(bool b) { _cv = b ? CV_YES : CV_NO; }
    
    /// Access enum identifier
    unsigned int enumId(void) const { return _enumId; }
    /// Set enum identifier
    void enumId(unsigned int eid) { _enumId = eid; }
    
    /// Access dimensions
    int dim(void) const { return _dim; }
    /// Set dimensions
    void dim(int d) { _dim = d; assert(_dim==d); }

  protected:
    /// Constructor
    Type(const TypeInst& ti, const BaseType& bt, const SetType& st,
         unsigned int enumId, int dim)
    : _ti(ti), _bt(bt), _st(st), _ot(OT_PRESENT), _cv(ti==TI_VAR ? CV_YES : CV_NO)
    , _enumId(enumId), _dim(dim) {}
  public:
    static Type parint(int dim=0) {
      return Type(TI_PAR,BT_INT,ST_PLAIN,0,dim);
    }
    static Type parenum(unsigned int enumId, int dim=0) {
      return Type(TI_PAR,BT_INT,ST_PLAIN,enumId,dim);
    }
    static Type parbool(int dim=0) {
      return Type(TI_PAR,BT_BOOL,ST_PLAIN,0,dim);
    }
    static Type parfloat(int dim=0) {
      return Type(TI_PAR,BT_FLOAT,ST_PLAIN,0,dim);
    }
    static Type parstring(int dim=0) {
      return Type(TI_PAR,BT_STRING,ST_PLAIN,0,dim);
    }
    static Type ann(int dim=0) {
      return Type(TI_PAR,BT_ANN,ST_PLAIN,0,dim);
    }
    static Type parsetint(int dim=0) {
      return Type(TI_PAR,BT_INT,ST_SET,0,dim);
    }
    static Type parsetenum(unsigned int enumId, int dim=0) {
      return Type(TI_PAR,BT_INT,ST_SET,enumId,dim);
    }
    static Type parsetbool(int dim=0) {
      return Type(TI_PAR,BT_BOOL,ST_SET,0,dim);
    }
    static Type parsetfloat(int dim=0) {
      return Type(TI_PAR,BT_FLOAT,ST_SET,0,dim);
    }
    static Type parsetstring(int dim=0) {
      return Type(TI_PAR,BT_STRING,ST_SET,0,dim);
    }
    static Type varint(int dim=0) {
      return Type(TI_VAR,BT_INT,ST_PLAIN,0,dim);
    }
    static Type varenumint(unsigned int enumId, int dim=0) {
      return Type(TI_VAR,BT_INT,ST_PLAIN,enumId,dim);
    }
    static Type varbool(int dim=0) {
      return Type(TI_VAR,BT_BOOL,ST_PLAIN,0,dim);
    }
    static Type varfloat(int dim=0) {
      return Type(TI_VAR,BT_FLOAT,ST_PLAIN,0,dim);
    }
    static Type varsetint(int dim=0) {
      return Type(TI_VAR,BT_INT,ST_SET,0,dim);
    }
    static Type varbot(int dim=0) {
      return Type(TI_VAR,BT_BOT,ST_PLAIN,0,dim);
    }
    static Type bot(int dim=0) {
      return Type(TI_PAR,BT_BOT,ST_PLAIN,0,dim);
    }
    static Type top(int dim=0) {
      return Type(TI_PAR,BT_TOP,ST_PLAIN,0,dim);
    }
    static Type vartop(int dim=0) {
      return Type(TI_VAR,BT_TOP,ST_PLAIN,0,dim);
    }
    static Type optvartop(int dim=0) {
      Type t(TI_VAR,BT_TOP,ST_PLAIN,0,dim);
      t._ot = OT_OPTIONAL;
      return t;
    }

    static Type unboxedint;
    static Type unboxedfloat;
    
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
    bool isvarbool(void) const { return _ti==TI_VAR && _dim==0 && _st==ST_PLAIN && _bt==BT_BOOL && _ot==OT_PRESENT; }
    bool isvarfloat(void) const { return _ti==TI_VAR && _dim==0 && _st==ST_PLAIN && _bt==BT_FLOAT && _ot==OT_PRESENT; }
    bool isvarint(void) const { return _ti==TI_VAR && _dim==0 && _st==ST_PLAIN && _bt==BT_INT && _ot==OT_PRESENT; }
    bool ispar(void) const { return _ti==TI_PAR; }
    bool isopt(void) const { return _ot==OT_OPTIONAL; }
    bool ispresent(void) const { return _ot==OT_PRESENT; }
    bool is_set(void) const { return _dim==0 && _st==ST_SET; }
    bool isintset(void) const {
      return is_set() && (_bt==BT_INT || _bt==BT_BOT);
    }
    bool isboolset(void) const {
      return is_set() && (_bt==BT_BOOL || _bt==BT_BOT);
    }
    bool isfloatset(void) const {
      return is_set() && (_bt==BT_FLOAT || _bt==BT_BOT);
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

    int toInt(void) const {
      return
      + ((1-static_cast<int>(_st))<<28)
      + (static_cast<int>(_bt)<<24)
      + (static_cast<int>(_ti)<<21)
      + (static_cast<int>(_ot)<<20)
      + (static_cast<int>(_enumId)<<8)
      + (_dim == -1 ? 1 : (_dim == 0 ? 0 : _dim+1));
    }
    static Type fromInt(int i) {
      Type t;
      t._st = 1-static_cast<SetType>((i >> 28) & 0x1);
      t._bt = static_cast<BaseType>((i >> 24) & 0xF);
      t._ti = static_cast<TypeInst>((i >> 21) & 0x7);
      t._ot = static_cast<OptType>((i >> 20) & 0x1);
      t._enumId = static_cast<unsigned int>((i >> 8) & 0xFFF);
      int dim = (i & 0x7F);
      t._dim =  (dim == 0 ? 0 : (dim==1 ? -1 : dim-1));
      return t;
    }
    std::string toString(EnvI& env) const;
    std::string nonEnumToString(void) const;
  public:
    /// Check if \a bt0 is a subtype of \a bt1
    static bool bt_subtype(const Type& t0, const Type& t1, bool strictEnums) {
      if (t0.bt() == t1.bt() && (!strictEnums || t0.dim() != 0 || (t0.enumId() == t1.enumId() || t1.enumId()==0)))
        return true;
      switch (t0.bt()) {
        case BT_BOOL: return (t1.bt()==BT_INT || t1.bt()==BT_FLOAT);
        case BT_INT: return t1.bt()==BT_FLOAT;
        default: return false;
      }
    }

    /// Check if this type is a subtype of \a t
    bool isSubtypeOf(const Type& t, bool strictEnums) const {
      if (_dim==0 && t._dim!=0 && _st==ST_SET && t._st==ST_PLAIN &&
          ( bt()==BT_BOT || bt_subtype(*this, t, false) || t.bt()==BT_TOP) && _ti==TI_PAR &&
          (_ot==OT_PRESENT || _ot==t._ot) )
        return true;
      // either same dimension or t has variable dimension
      if (_dim!=t._dim && (_dim==0 || t._dim!=-1))
        return false;
      // same type, this is present or both optional
      if (_ti==t._ti && bt_subtype(*this,t,strictEnums) && _st==t._st)
        return _ot==OT_PRESENT || _ot==t._ot;
      // this is par other than that same type as t
      if (_ti==TI_PAR && bt_subtype(*this,t,strictEnums) && _st==t._st)
        return _ot==OT_PRESENT || _ot==t._ot;
      if ( _ti==TI_PAR && t._bt==BT_BOT)
        return true;
      if ((_ti==t._ti || _ti==TI_PAR) && _bt==BT_BOT && (_st==t._st || _st==ST_PLAIN))
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
