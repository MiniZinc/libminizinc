/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_AST_HH__
#define __MINIZINC_AST_HH__

#include <minizinc/gc.hh>
#include <minizinc/aststring.hh>
#include <minizinc/astvec.hh>

#include <minizinc/values.hh>
#include <minizinc/type.hh>

#include <utility>
#include <vector>
#include <cstddef>
#include <cstdint>

#include <iostream>

namespace MiniZinc {

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
    unsigned int _first_line;
    /// Line where expression ends
    unsigned int _last_line;
    /// Column where expression starts
    unsigned int _first_column;
    /// Column where expression ends
    unsigned int _last_column;
  public:
    /// Construct empty location
    ParserLocation(void) : _first_line(0), _last_line(0), _first_column(0), _last_column(0) {}
    
    /// Construct location
    ParserLocation(const ASTString& filename, unsigned int first_line, unsigned int first_column, unsigned int last_line, unsigned int last_column)
    : _filename(filename), _first_line(first_line), _last_line(last_line), _first_column(first_column), _last_column(last_column) {}
    
    ASTString filename(void) const { return _filename; }
    void filename(const ASTString& f) { _filename = f; }
    
    unsigned int first_line(void) const { return _first_line; }
    void first_line(unsigned int l) { _first_line = l; }
    
    unsigned int last_line(void) const { return _last_line; }
    void last_line(unsigned int l) { _last_line = l; }
    
    unsigned int first_column(void) const { return _first_column; }
    void first_column(unsigned int c) { _first_column = c; }
    
    unsigned int last_column(void) const { return _last_column; }
    void last_column(unsigned int c) { _last_column = c; }
    
  };

  /// %Location of an expression in the source code
  class Location {
  protected:
    
    /// Internal representation of a Location
    class LocVec : public ASTVec {
    protected:
      LocVec(const ASTString& filename, unsigned int first_line, unsigned int first_column, unsigned int last_line, unsigned int last_column);
    public:
      static LocVec* a(const ASTString& filename, unsigned int first_line, unsigned int first_column, unsigned int last_line, unsigned int last_column) {
        LocVec* v = static_cast<LocVec*>(alloc(3));
        new (v) LocVec(filename,first_line,first_column,last_line,last_column);
        return v;
      }
      void mark(void) {
        _gc_mark = 1;
        if (_data[0])
          static_cast<ASTStringO*>(_data[0])->mark();
      }
      
      ASTString filename(void) const;
      unsigned int first_line(void) const;
      unsigned int last_line(void) const;
      unsigned int first_column(void) const;
      unsigned int last_column(void) const;
    };
    
    union LI {
      LocVec* lv;
      ptrdiff_t t;
    } _loc_info;
    
    LocVec* lv(void) const {
      LI li = _loc_info;
      li.t &= ~static_cast<ptrdiff_t>(1);
      return li.lv;
    }
    
  public:
    /// Construct empty location
    Location(void) { _loc_info.lv = NULL; }

    /// Construct location
    Location(const ASTString& filename, unsigned int first_line, unsigned int first_column, unsigned int last_line, unsigned int last_column) {
      _loc_info.lv = LocVec::a(filename,first_line,first_column,last_line,last_column);
    }

    Location(const ParserLocation& loc) {
      _loc_info.lv = LocVec::a(loc.filename(),loc.first_line(),loc.first_column(),loc.last_line(),loc.last_column());
    }
    
    /// Return string representation
    std::string toString(void) const;

    /// Return filename
    ASTString filename(void) const { return lv() ? lv()->filename() : ASTString(); }
    
    /// Return first line number
    unsigned int first_line(void) const { return lv() ? lv()->first_line() : 0; }
    
    /// Return last line number
    unsigned int last_line(void) const { return lv() ? lv()->last_line() : 0; }
    
    /// Return first column number
    unsigned int first_column(void) const { return lv() ? lv()->first_column() : 0; }
    
    /// Return last column number
    unsigned int last_column(void) const { return lv() ? lv()->last_column() : 0; }
    
    /// Return whether location is introduced by the compiler
    bool is_introduced(void) const {
      return _loc_info.lv==NULL || ( (_loc_info.t & 1) != 0);
    }
    
    /// Mark as alive for garbage collection
    void mark(void) const;
    
    /// Return location with introduced flag set
    Location introduce(void) const;
    
    /// Location used for un-allocated expressions
    static Location nonalloc;
    
    ParserLocation parserLocation(void) const {
      return ParserLocation(filename(), first_line(), first_column(), last_line(), last_column());
    }
  };

  /// Output operator for locations
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Location& loc) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    if (loc.filename()=="") {
      s << "unknown file";
    } else {
      s << loc.filename() << ":" << loc.first_line();
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
    Annotation& operator =(const Annotation&);
  public:
    Annotation(void) : _s(NULL) {}
    ~Annotation(void);
    bool contains(Expression* e) const;
    bool containsCall(const ASTString& id);
    bool isEmpty(void) const;
    ExpressionSetIter begin(void) const;
    ExpressionSetIter end(void) const;
    void add(Expression* e);
    void add(std::vector<Expression*> e);
    void remove(Expression* e);
    void removeCall(const ASTString& id);
    void clear(void);
    void merge(const Annotation& ann);
    
    static Annotation empty;
  };
  
  /// returns the Annotation specified by the string; returns NULL if not exists
  Expression* getAnnotation(const Annotation& ann, std::string str);

  /// returns the Annotation specified by the string; returns NULL if not exists
  Expression* getAnnotation(const Annotation& ann, const ASTString& str);

  /**
   * \brief Base class for expressions
   */
  class Expression : public ASTNode {
  protected:
    /// The annotations
    Annotation _ann;
    /// The location of the expression
    Location _loc;
    /// The %MiniZinc type of the expression
    Type _type;
    /// The hash value of the expression
    size_t _hash;
  public:
    /// Identifier of the concrete expression type
    enum ExpressionId {
      E_INTLIT = ASTNode::NID_END+1, E_FLOATLIT, E_SETLIT, E_BOOLLIT,
      E_STRINGLIT, E_ID, E_ANON, E_ARRAYLIT,
      E_ARRAYACCESS, E_COMP, E_ITE,
      E_BINOP, E_UNOP, E_CALL, E_VARDECL, E_LET,
      E_TI, E_TIID, EID_END = E_TIID
    };

    bool isUnboxedVal(void) const {
      if (sizeof(double) <= sizeof(FloatLit*)) {
        // bit 1 or bit 0 is set
        return (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(3)) != 0;
      } else {
        // bit 0 is set
        return (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(1)) != 0;
      }
    }
    bool isUnboxedInt(void) const {
      if (sizeof(double) <= sizeof(FloatLit*)) {
        // bit 1 is set, bit 0 is not set
        return (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(3)) == 2;
      } else {
        // bit 0 is set
        return (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(1)) == 1;
      }
    }
    bool isUnboxedFloatVal(void) const {
      // bit 0 is set (and doubles fit inside pointers)
      return (sizeof(double) <= sizeof(FloatLit*)) && (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(1)) == 1;
    }

    ExpressionId eid(void) const {
      return isUnboxedInt() ? E_INTLIT : isUnboxedFloatVal() ? E_FLOATLIT : static_cast<ExpressionId>(_id);
    }

    const Location& loc(void) const {
      return isUnboxedVal() ? Location::nonalloc : _loc;
    }
    void loc(const Location& l) {
      if (!isUnboxedVal())
        _loc = l;
    }
    const Type& type(void) const {
      return isUnboxedInt() ? Type::unboxedint : isUnboxedFloatVal() ? Type::unboxedfloat : _type;
    }
    void type(const Type& t);
    size_t hash(void) const {
      return isUnboxedInt() ? unboxedIntToIntVal().hash() : isUnboxedFloatVal() ? unboxedFloatToFloatVal().hash() : _hash;
    }
  protected:
    /// Combination function for hash values
    void cmb_hash(size_t h) {
      _hash ^= h + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
    }
    /// Combination function for hash values
    size_t cmb_hash(size_t seed, size_t h) {
      seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }

    /// Compute base hash value
    void init_hash(void) { _hash = cmb_hash(0,_id); }

    /// Check if \a e0 and \a e1 are equal
    static bool equal_internal(const Expression* e0, const Expression* e1);

    /// Constructor
    Expression(const Location& loc, const ExpressionId& eid, const Type& t)
      : ASTNode(eid), _loc(loc), _type(t) {}

  public:
    IntVal unboxedIntToIntVal(void) const {
      assert(isUnboxedInt());
      if (sizeof(double) <= sizeof(FloatVal*)) {
        unsigned long long int i = reinterpret_cast<ptrdiff_t>(this) & ~static_cast<ptrdiff_t>(7);
        bool pos = ((reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(4)) == 0);
        if (pos) {
          return i >> 3;
        } else {
          return -(static_cast<long long int>(i>>3));
        }
      } else {
        unsigned long long int i = reinterpret_cast<ptrdiff_t>(this) & ~static_cast<ptrdiff_t>(3);
        bool pos = ((reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(2)) == 0);
        if (pos) {
          return i >> 2;
        } else {
          return -(static_cast<long long int>(i>>2));
        }
      }
    }
    static IntLit* intToUnboxedInt(long long int i) {
      static const unsigned int pointerBits = sizeof(IntLit*)*8;
      if (sizeof(double) <= sizeof(FloatVal*)) {
        static const long long int maxUnboxedVal = (static_cast<long long int>(1) << (pointerBits - 3)) - static_cast<long long int>(1);
        if (i < -maxUnboxedVal || i > maxUnboxedVal)
          return NULL;
        long long int j = i < 0 ? -i : i;
        ptrdiff_t ubi_p = (static_cast<ptrdiff_t>(j) << 3) | static_cast<ptrdiff_t>(2);
        if (i < 0)
          ubi_p = ubi_p | static_cast<ptrdiff_t>(4);
        return reinterpret_cast<IntLit*>(ubi_p);
      } else {
        static const long long int maxUnboxedVal = (static_cast<long long int>(1) << (pointerBits - 2)) - static_cast<long long int>(1);
        if (i < -maxUnboxedVal || i > maxUnboxedVal)
          return NULL;
        long long int j = i < 0 ? -i : i;
        ptrdiff_t ubi_p = (static_cast<ptrdiff_t>(j) << 2) | static_cast<ptrdiff_t>(1);
        if (i < 0)
          ubi_p = ubi_p | static_cast<ptrdiff_t>(2);
        return reinterpret_cast<IntLit*>(ubi_p);
      }
    }
    FloatVal unboxedFloatToFloatVal(void) const {
      assert(isUnboxedFloatVal());
      union {
        double d;
        uint64_t bits;
        const Expression* p;
      } _u;
      _u.p = this;
      _u.bits = _u.bits >> 1;
      uint64_t exponent = (_u.bits & (static_cast<uint64_t>(0x3FF) << 52)) >> 52;
      if (exponent != 0) {
        exponent += 512; // reconstruct original bias of 1023
      }
      uint64_t sign = (_u.bits & (static_cast<uint64_t>(1)<<62) ? 1 : 0);
      _u.bits = (sign << 63) | (exponent << 52) | ( _u.bits & static_cast<uint64_t>(0xFFFFFFFFFFFFF));
      return _u.d;
    }
    static FloatLit* doubleToUnboxedFloatVal(double d) {
      if (sizeof(double) > sizeof(FloatLit*))
        return NULL;
      union {
        double d;
        uint64_t bits;
        FloatLit* p;
      } _u;
      _u.d = d;

      uint64_t exponent = (_u.bits & (static_cast<uint64_t>(0x7FF) << 52)) >> 52;
      if (exponent != 0) {
        if (exponent < 513 || exponent > 1534)
          return NULL; // exponent doesn't fit in 10 bits
        exponent -= 512; // make exponent fit in 10 bits, with bias 511
      }
      bool sign = _u.bits & (static_cast<uint64_t>(1) << 63);
      
      _u.bits = _u.bits & ~(static_cast<uint64_t>(0x7FF) << 52); // mask out top 11 bits (previously exponent)
      _u.bits = (_u.bits << 1) | 1u; // shift by one bit and add tag for double
      _u.bits = _u.bits | (static_cast<uint64_t>(sign) << 63) | (static_cast<uint64_t>(exponent) << 53);
      return _u.p;
    }
    
    bool isTagged(void) const {
      // only bit 2 is set
      if (isUnboxedVal())
        return false;
      if (sizeof(double) <= sizeof(FloatVal*))
        return (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(7)) == 4;
      else
        return (reinterpret_cast<ptrdiff_t>(this) & static_cast<ptrdiff_t>(3)) == 2;
    }
    
    Expression* tag(void) const {
      assert(!isUnboxedVal());
      if (sizeof(double) <= sizeof(FloatVal*))
        return reinterpret_cast<Expression*>(reinterpret_cast<ptrdiff_t>(this) |
                                             static_cast<ptrdiff_t>(4));
      else
        return reinterpret_cast<Expression*>(reinterpret_cast<ptrdiff_t>(this) |
                                             static_cast<ptrdiff_t>(2));
    }
    Expression* untag(void) {
      if (isUnboxedVal())
        return this;
      if (sizeof(double) <= sizeof(FloatVal*))
        return reinterpret_cast<Expression*>(reinterpret_cast<ptrdiff_t>(this) &
                                             ~static_cast<ptrdiff_t>(4));
      else
        return reinterpret_cast<Expression*>(reinterpret_cast<ptrdiff_t>(this) &
                                             ~static_cast<ptrdiff_t>(2));
    }

    /// Test if expression is of type \a T
    template<class T> bool isa(void) const {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
      if (nullptr==this)
        throw InternalError("isa: nullptr");
#pragma clang diagnostic pop
      return isUnboxedInt() ? T::eid==E_INTLIT : isUnboxedFloatVal() ? T::eid==E_FLOATLIT : _id==T::eid;
    }
    /// Cast expression to type \a T*
    template<class T> T* cast(void) {
      assert(isa<T>());
      return static_cast<T*>(this);
    }
    /// Cast expression to type \a const T*
    template<class T> const T* cast(void) const {
      assert(isa<T>());
      return static_cast<const T*>(this);
    }
    /// Cast expression to type \a T* or NULL if types do not match
    template<class T> T* dyn_cast(void) {
      return isa<T>() ? static_cast<T*>(this) : NULL;
    }
    /// Cast expression to type \a const T* or NULL if types do not match
    template<class T> const T* dyn_cast(void) const {
      return isa<T>() ? static_cast<const T*>(this) : NULL;
    }

    /// Cast expression to type \a T*
    template<class T> static T* cast(Expression* e) {
      return e==NULL ? NULL : e->cast<T>();
    }
    /// Cast expression to type \a const T*
    template<class T> static const T* cast(const Expression* e) {
      return e==NULL ? NULL : e->cast<T>();
    }
    /// Cast expression to type \a T* or NULL if types do not match
    template<class T> static T* dyn_cast(Expression* e) {
      return e==NULL ? NULL : e->dyn_cast<T>();
    }
    /// Cast expression to type \a const T* or NULL if types do not match
    template<class T> static const T* dyn_cast(const Expression* e) {
      return e==NULL ? NULL : e->dyn_cast<T>();
    }
    
    
    /// Add annotation \a ann to the expression
    void addAnnotation(Expression* ann);

    /// Add annotation \a ann to the expression
    void addAnnotations(std::vector<Expression*> ann);

    const Annotation& ann(void) const { return isUnboxedVal() ? Annotation::empty : _ann; }
    Annotation& ann(void) { return isUnboxedVal() ? Annotation::empty : _ann; }
    
    /// Return hash value of \a e
    static size_t hash(const Expression* e) {
      return e==NULL ? 0 : e->hash();
    }
    
    /// Check if \a e0 and \a e1 are equal
    static bool equal(const Expression* e0, const Expression* e1);
    
    /// Mark \a e as alive for garbage collection
    static void mark(Expression* e);
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
    IntVal v(void) const {
      return isUnboxedInt() ? unboxedIntToIntVal() : _v;
    }
    /// Recompute hash value
    void rehash(void);
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
    FloatVal v(void) const {
      return isUnboxedFloatVal() ? unboxedFloatToFloatVal() : _v;
    }
    /// Recompute hash value
    void rehash(void);
    /// Allocate literal
    static FloatLit* a(FloatVal v);
  };
  /// \brief Set literal expression
  class SetLit : public Expression {
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
    SetLit(const Location& loc, ASTExprVec<Expression> v);
    /// Construct set
    SetLit(const Location& loc, IntSetVal* isv);
    /// Construct set
    SetLit(const Location& loc, FloatSetVal* fsv);
    /// Access value
    ASTExprVec<Expression> v(void) const { return _v; }
    /// Set value
    void v(const ASTExprVec<Expression>& val) { _v = val; }
    /// Access integer set value if present
    IntSetVal* isv(void) const { return type().bt()==Type::BT_INT ? _u.isv : NULL; }
    /// Set integer set value
    void isv(IntSetVal* val) { _u.isv = val; }
    /// Access float set value if present
    FloatSetVal* fsv(void) const { return type().bt()==Type::BT_FLOAT ? _u.fsv : NULL; }
    /// Set integer set value
    void fsv(FloatSetVal* val) { _u.fsv = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Boolean literal expression
  class BoolLit : public Expression {
  protected:
    /// The value of this expression
    bool _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BOOLLIT;
    /// Constructor
    BoolLit(const Location& loc, bool v);
    /// Access value
    bool v(void) const { return _v; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief String literal expression
  class StringLit : public Expression {
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
    ASTString v(void) const { return _v; }
    /// Set value
    void v(const ASTString& val) { _v = val; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Identifier expression
  class Id : public Expression {
  protected:
    /// The string identifier
    void* _v_or_idn;
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
    ASTString v(void) const;
    /// Set identifier
    void v(const ASTString& val) {
      _v_or_idn = val.aststr();
    }
    /// Access identifier number
    long long int idn(void) const;
    /// Set identifier number
    void idn(long long int n) {
      _v_or_idn = reinterpret_cast<void*>((static_cast<ptrdiff_t>(n) << 1) | static_cast<ptrdiff_t>(1));
      rehash();
    }
    /// Return identifier or X_INTRODUCED plus identifier number
    ASTString str(void) const;
    /// Access declaration
    VarDecl* decl(void) const {
      Expression* d = _decl;
      while (d && d->isa<Id>())
        d = d->cast<Id>()->_decl;
      return Expression::cast<VarDecl>(d);
    }
    /// Set declaration
    void decl(VarDecl* d);
    /// Redirect to another Id \a id
    void redirect(Id* id) {
      assert(_decl==NULL || _decl->isa<VarDecl>());
      _decl = id;
    }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Type-inst identifier expression
  class TIId : public Expression {
  protected:
    /// The string identifier
    ASTString _v;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TIID;
    /// Constructor
    TIId(const Location& loc, const std::string& v);
    /// Access identifier
    ASTString v(void) const { return _v; }
    /// Set identifier
    void v(const ASTString& val) { _v = val; }
    /// Check whether it is an enum identifier (starting with two $ signs)
    bool isEnum(void) const { return _v.c_str()[0]=='$'; }
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Anonymous variable expression
  class AnonVar : public Expression {
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ANON;
    /// Constructor
    AnonVar(const Location& loc);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief Array literal expression
  class ArrayLit : public Expression {
    friend class Expression;
  protected:
    /// The array
    union {
      /// An expression vector (if _flag_2==false)
      ASTExprVecO<Expression*>* _v;
      /// Another array literal (if _flag_2==true)
      ArrayLit* _al;
    } _u;
    /// The declared array dimensions
    // If _flag_2 is true, then this is an array view. In that case,
    // the _dims array holds the sliced dimensions
    ASTIntVec _dims;
    /// Set compressed vector (initial repetitions are removed)
    void compress(const std::vector<Expression*>& v, const std::vector<int>& dims);
  public:
    /// Index conversion from slice to original
    int origIdx(int i) const;
    /// Get element \a i of a sliced array
    Expression* slice_get(int i) const;
    /// Set element \a i of a sliced array
    void slice_set(int i, Expression* e);
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYLIT;
    /// Constructor
    ArrayLit(const Location& loc,
             const std::vector<Expression*>& v,
             const std::vector<std::pair<int,int> >& dims);
    /// Constructor (existing content)
    ArrayLit(const Location& loc,
             ArrayLit& v,
             const std::vector<std::pair<int,int> >& dims);
    /// Constructor (one-dimensional, existing content)
    ArrayLit(const Location& loc,
             ArrayLit& v);
    /// Constructor (one-dimensional)
    ArrayLit(const Location& loc,
             const std::vector<Expression*>& v);
    /// Constructor (two-dimensional)
    ArrayLit(const Location& loc,
             const std::vector<std::vector<Expression*> >& v);
    /// Constructor for slices
    ArrayLit(const Location& loc, ArrayLit* v,
             const std::vector<std::pair<int,int> >& dims,
             const std::vector<std::pair<int,int> >& slice);
    /// Recompute hash value
    void rehash(void);

    // The following methods are only used for copying
    
    /// Access value
    ASTExprVec<Expression> getVec(void) const { assert(!_flag_2); return _u._v; }
    /// Set value
    void setVec(const ASTExprVec<Expression>& val) { assert(!_flag_2); _u._v = val.vec(); }
    /// Get underlying array (if this is an array slice) or NULL
    ArrayLit* getSliceLiteral(void) const { return _flag_2 ? _u._al : NULL; }
    /// Get underlying _dims vector
    ASTIntVec dimsInternal(void) const { return _dims; }

    /// Return number of dimensions
    int dims(void) const;
    /// Return minimum index of dimension \a i
    int min(int i) const;
    /// Return maximum index of dimension \a i
    int max(int i) const;
    /// Return the length of the array
    int length(void) const;
    /// Turn into 1d array (only used at the end of flattening)
    void make1d(void);
    /// Check if this array was produced by flattening
    bool flat(void) const { return _flag_1; }
    /// Set whether this array was produced by flattening
    void flat(bool b) { _flag_1 = b; }
    /// Return size of underlying array
    unsigned int size(void) const { return (_flag_2 || _u._v->flag()) ? length() : _u._v->size(); }
    /// Access element \a i
    Expression* operator[](int i) const { return (_flag_2 || _u._v->flag()) ? slice_get(i) : (*_u._v)[i]; }
    /// Set element \a i
    void set(int i, Expression* e) { if (_flag_2 || _u._v->flag()) { slice_set(i,e); } else { (*_u._v)[i] = e; } }
  };
  /// \brief Array access expression
  class ArrayAccess : public Expression {
  protected:
    /// The array to access
    Expression* _v;
    /// The indexes (for all array dimensions)
    ASTExprVec<Expression> _idx;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ARRAYACCESS;
    /// Constructor
    ArrayAccess(const Location& loc,
                Expression* v,
                const std::vector<Expression*>& idx);
    /// Constructor
    ArrayAccess(const Location& loc,
                Expression* v,
                ASTExprVec<Expression> idx);
    /// Access value
    Expression* v(void) const { return _v; }
    /// Set value
    void v(Expression* val) { _v = val; }
    /// Access index sets
    ASTExprVec<Expression> idx(void) const { return _idx; }
    /// Set index sets
    void idx(const ASTExprVec<Expression>& idx) { _idx = idx; }
    /// Recompute hash value
    void rehash(void);
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
    Generator(const std::vector<std::string>& v,
              Expression* in, Expression* where);
    /// Allocate
    Generator(const std::vector<ASTString>& v,
              Expression* in, Expression* where);
    /// Allocate
    Generator(const std::vector<Id*>& v,
              Expression* in, Expression* where);
    /// Allocate
    Generator(const std::vector<VarDecl*>& v,
              Expression* in, Expression* where);
    
  };
  /// \brief A list of generators with one where-expression
  struct Generators {
    /// %Generators
    std::vector<Generator> _g;
    /// Constructor
    Generators(void) {}
  };
  /// \brief An expression representing an array- or set-comprehension
  class Comprehension : public Expression {
    friend class Expression;
  protected:
    /// The expression to generate
    Expression* _e;
    /// A list of generator expressions
    ASTExprVec<Expression> _g;
    /// A list of indices where generators start
    ASTIntVec _g_idx;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_COMP;
    /// Constructor
    Comprehension(const Location& loc,
                  Expression* e, Generators& g, bool set);
    /// Recompute hash value
    void rehash(void);
    /// Whether comprehension is a set
    bool set(void) const;
    
    /// Return number of generators
    int n_generators(void) const;
    /// Return "in" expression for generator \a i
    Expression* in(int i);
    /// Return "in" expression for generator \a i
    const Expression* in(int i) const;
    /// Return number of declarations for generator \a i
    int n_decls(int i) const;
    /// Return declaration \a i for generator \a gen
    VarDecl* decl(int gen, int i);
    /// Return declaration \a i for generator \a gen
    const VarDecl* decl(int gen, int i) const;
    /// Return where clause for generator \a i
    Expression* where(int i);
    /// Return where clause for generator \a i
    const Expression* where(int i) const;
    /// Return generator body
    Expression* e(void) const { return _e; }
    /// Set generator body
    void e(Expression* e0) { _e = e0; }
    /// Re-construct (used for copying)
    void init(Expression* e, Generators& g);
  };
  /// \brief If-then-else expression
  class ITE : public Expression {
    friend class Expression;
  protected:
    /// List of if-then-pairs
    ASTExprVec<Expression> _e_if_then;
    /// Else-expression
    Expression* _e_else;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_ITE;
    /// Constructor
    ITE(const Location& loc,
        const std::vector<Expression*>& e_if_then,
        Expression* e_else);
    int size(void) const { return _e_if_then.size()/2; }
    Expression* e_if(int i) { return _e_if_then[2*i]; }
    Expression* e_then(int i) { return _e_if_then[2*i+1]; }
    Expression* e_else(void) { return _e_else; }
    const Expression* e_if(int i) const { return _e_if_then[2*i]; }
    const Expression* e_then(int i) const { return _e_if_then[2*i+1]; }
    const Expression* e_else(void) const { return _e_else; }
    void e_then(int i, Expression* e) { _e_if_then[2*i+1] = e; }
    void e_else(Expression* e) { _e_else = e; }
    /// Recompute hash value
    void rehash(void);
    /// Re-construct (used for copying)
    void init(const std::vector<Expression*>& e_if_then, Expression* e_else);
  };

  /// Type of binary operators
  enum BinOpType {
    BOT_PLUS, BOT_MINUS, BOT_MULT, BOT_DIV, BOT_IDIV, BOT_MOD,
    BOT_LE, BOT_LQ, BOT_GR, BOT_GQ, BOT_EQ, BOT_NQ,
    BOT_IN, BOT_SUBSET, BOT_SUPERSET, BOT_UNION, BOT_DIFF, BOT_SYMDIFF,
    BOT_INTERSECT,
    BOT_PLUSPLUS,
    BOT_EQUIV, BOT_IMPL, BOT_RIMPL, BOT_OR, BOT_AND, BOT_XOR,
    BOT_DOTDOT
  };
  /// \brief Binary-operator expression
  class BinOp : public Expression {
  protected:
    /// Left hand side expression
    Expression* _e0;
    /// Right hand side expression
    Expression* _e1;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_BINOP;
    /// Constructor
    BinOp(const Location& loc,
          Expression* e0, BinOpType op, Expression* e1);
    /// Access left hand side
    Expression* lhs(void) const { return _e0; }
    /// Set left hand side
    void lhs(Expression* e) { _e0 = e; }
    /// Access right hand side
    Expression* rhs(void) const { return _e1; }
    /// Set right hand side
    void rhs(Expression* e) { _e1 = e; }
    /// Access argument \a i
    Expression* arg(int i) { assert(i==0 || i==1); return i==0 ? _e0 : _e1; }
    /// Return number of arguments
    unsigned int n_args(void) const { return 2; }
    /// Access declaration
    FunctionI* decl(void) const { return _decl; }
    /// Set declaration
    void decl(FunctionI* f) { _decl = f; }
    /// Return string representation of the operator
    ASTString opToString(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Return operator type
    BinOpType op(void) const;
  };

  /// Type of unary operators
  enum UnOpType {
    UOT_NOT, UOT_PLUS, UOT_MINUS
  };
  /// \brief Unary-operator expressions
  class UnOp : public Expression {
  protected:
    /// %Expression
    Expression* _e0;
    /// The predicate or function declaration (or NULL)
    FunctionI* _decl;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_UNOP;
    /// Constructor
    UnOp(const Location& loc,
         UnOpType op, Expression* e);
    /// Access expression
    Expression* e(void) const { return _e0; }
    /// Set expression
    void e(Expression* e0) { _e0 = e0; }
    /// Access argument \a i
    Expression* arg(int i) { assert(i==0); return _e0; }
    /// Return number of arguments
    unsigned int n_args(void) const { return 1; }
    /// Access declaration
    FunctionI* decl(void) const { return _decl; }
    /// Set declaration
    void decl(FunctionI* f) { _decl = f; }
    ASTString opToString(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Return operator type
    UnOpType op(void) const;
  };
  
  /// \brief A predicate or function call expression
  class Call : public Expression {
    friend class Expression;
  protected:
    union {
      /// Identifier of called predicate or function
      ASTStringO* _id;
      /// The predicate or function declaration (or NULL)
      FunctionI* _decl;
    } _u_id;
    union {
      /// Single-argument call (tagged pointer)
      Expression* _oneArg;
      /// Arguments to the call
      ASTExprVecO<Expression*>* _args;
    } _u;
    /// Check if _u_id contains an id or a decl
    bool hasId(void) const;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_CALL;
    /// Constructor
    Call(const Location& loc,
         const std::string& id,
         const std::vector<Expression*>& args);
    /// Constructor
    Call(const Location& loc,
         const ASTString& id,
         const std::vector<Expression*>& args);
    /// Access identifier
    ASTString id(void) const;
    /// Set identifier (overwrites decl)
    void id(const ASTString& i);
    /// Number of arguments
    unsigned int n_args(void) const { return _u._oneArg->isUnboxedVal() || _u._oneArg->isTagged() ? 1 : _u._args->size(); }
    /// Access argument \a i
    Expression* arg(int i) const {
      if (_u._oneArg->isUnboxedVal() || _u._oneArg->isTagged()) {
        assert(i==0);
        return _u._oneArg->isUnboxedVal() ? _u._oneArg : _u._oneArg->untag();
      } else {
        return (*_u._args)[i];
      }
    }
    /// Set argument \a i
    void arg(int i, Expression* e) {
      if (_u._oneArg->isUnboxedVal() || _u._oneArg->isTagged()) {
        assert(i==0);
        _u._oneArg = e->isUnboxedVal() ? e : e->tag();
      } else {
        (*_u._args)[i] = e;
      }
    }
    /// Set arguments
    void args(const ASTExprVec<Expression>& a) {
      if (a.size()==1) {
        _u._oneArg = a[0]->isUnboxedVal() ? a[0] : a[0]->tag();
      } else {
        _u._args = a.vec();
        assert(!_u._oneArg->isTagged());
      }
    }
    /// Access declaration
    FunctionI* decl(void) const;
    /// Set declaration (overwrites id)
    void decl(FunctionI* f);
    /// Recompute hash value
    void rehash(void);
  };
  /// \brief A variable declaration expression
  class VarDecl : public Expression {
  protected:
    /// Type-inst of the declared variable
    TypeInst* _ti;
    /// Identifier
    Id* _id;
    /// Initialisation expression (can be NULL)
    Expression* _e;
    /// Flattened version of the VarDecl
    WeakRef _flat;
    /// Integer payload
    int _payload;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_VARDECL;
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, const std::string& id,
            Expression* e=NULL);
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, const ASTString& id, Expression* e=NULL);
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, long long int idn, Expression* e=NULL);
    /// Constructor
    VarDecl(const Location& loc,
            TypeInst* ti, Id* id, Expression* e=NULL);

    /// Access TypeInst
    TypeInst* ti(void) const { return _ti; }
    /// Set TypeInst
    void ti(TypeInst* t) { _ti=t; }
    /// Access identifier
    Id* id(void) const { return _id; }
    /// Access initialisation expression
    Expression* e(void) const;
    /// Set initialisation expression
    void e(Expression* rhs);
    /// Access flattened version
    VarDecl* flat(void) { return _flat() ? _flat()->cast<VarDecl>() : NULL; }
    /// Set flattened version
    void flat(VarDecl* vd);
    
    /// Recompute hash value
    void rehash(void);
    /// Whether variable is toplevel
    bool toplevel(void) const;
    /// Whether variable is toplevel
    void toplevel(bool t);
    /// Whether variable is introduced
    bool introduced(void) const;
    /// Whether variable is introduced
    void introduced(bool t);
    /// Whether variable has been evaluated
    bool evaluated(void) const;
    /// Whether variable has been evaluated
    void evaluated(bool t);
    /// Access payload
    int payload(void) const { return _payload; }
    /// Set payload
    void payload(int i) { _payload = i; }
    /// Put current value on trail
    void trail(void);
  };
  
  class EnvI;
  class CopyMap;
  
  /// \brief %Let expression
  class Let : public Expression {
    friend Expression* copy(EnvI& env, CopyMap& m, Expression* e, bool followIds, bool copyFundecls, bool isFlatModel);
    friend class Expression;
  protected:
    /// List of local declarations
    ASTExprVec<Expression> _let;
    /// Copy of original local declarations
    ASTExprVec<Expression> _let_orig;
    /// Body of the let
    Expression* _in;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_LET;
    /// Constructor
    Let(const Location& loc,
        const std::vector<Expression*>& let, Expression* in);
    /// Recompute hash value
    void rehash(void);

    /// Access local declarations
    ASTExprVec<Expression> let(void) const { return _let; }
    /// Access local declarations
    ASTExprVec<Expression> let_orig(void) const { return _let_orig; }
    /// Access body
    Expression* in(void) const { return _in; }
    
    /// Remember current let bindings
    void pushbindings(void);
    /// Restore previous let bindings
    void popbindings(void);
    
  };

  /// \brief Type-inst expression
  class TypeInst : public Expression {
  protected:
    /// Ranges of an array expression
    ASTExprVec<TypeInst> _ranges;
    /// Declared domain (or NULL)
    Expression* _domain;
  public:
    /// The identifier of this expression type
    static const ExpressionId eid = E_TI;
    /// Constructor
    TypeInst(const Location& loc,
             const Type& t,
             ASTExprVec<TypeInst> ranges,
             Expression* domain=NULL);
    /// Constructor
    TypeInst(const Location& loc,
             const Type& t,
             Expression* domain=NULL);
    
    /// Access ranges
    ASTExprVec<TypeInst> ranges(void) const { return _ranges; }
    /// Access domain
    Expression* domain(void) const { return _domain; }
    //// Set domain
    void domain(Expression* d) { _domain = d; }
    
    /// Set ranges to \a ranges
    void setRanges(const std::vector<TypeInst*>& ranges);
    bool isarray(void) const { return _ranges.size()>0; }
    bool hasTiVariable(void) const;
    /// Recompute hash value
    void rehash(void);
    /// Check if domain is computed from right hand side of variable
    bool computedDomain(void) const { return _flag_1; }
    /// Set if domain is computed from right hand side of variable
    void setComputedDomain(bool b) { _flag_1=b; }
    /// Check if this TypeInst represents an enum
    bool isEnum(void) const { return _flag_2; }
    /// Set if this TypeInst represents an enum
    void setIsEnum(bool b) { _flag_2=b; }
  };

  /**
   * \brief Base-class for items
   */
  class Item : public ASTNode {
  protected:
    /// Location of the item
    Location _loc;
  public:
    /// Identifier of the concrete item type
    enum ItemId {
      II_INC = Expression::EID_END+1, II_VD, II_ASN, II_CON, II_SOL,
      II_OUT, II_FUN, II_END = II_FUN
    };
    ItemId iid(void) const {
      return static_cast<ItemId>(_id);
    }
    
    const Location& loc(void) const {
      return _loc;
    }
  protected:
    /// Constructor
    Item(const Location& loc, const ItemId& iid)
      : ASTNode(iid), _loc(loc) { _flag_1 = false; }

  public:

    /// Test if item is of type \a T
    template<class T> bool isa(void) const {
      return _id==T::iid;
    }
    /// Cast item to type \a T*
    template<class T> T* cast(void) {
      assert(isa<T>());
      return static_cast<T*>(this);
    }
    /// Cast expression to type \a const T*
    template<class T> const T* cast(void) const {
      assert(isa<T>());
      return static_cast<const T*>(this);
    }
    /// Cast item to type \a T* or NULL if types do not match
    template<class T> T* dyn_cast(void) {
      return isa<T>() ? static_cast<T*>(this) : NULL;
    }
    /// Cast item to type \a const T* or NULL if types do not match
    template<class T> const T* dyn_cast(void) const {
      return isa<T>() ? static_cast<const T*>(this) : NULL;
    }

    /// Cast item to type \a T*
    template<class T> static T* cast(Item* i) {
      return i==NULL ? NULL : i->cast<T>();
    }
    /// Cast item to type \a const T*
    template<class T> static const T* cast(const Item* i) {
      return i==NULL ? NULL : i->cast<T>();
    }
    /// Cast item to type \a T* or NULL if types do not match
    template<class T> static T* dyn_cast(Item* i) {
      return i==NULL ? NULL : i->dyn_cast<T>();
    }
    /// Cast item to type \a const T* or NULL if types do not match
    template<class T> static const T* dyn_cast(const Item* i) {
      return i==NULL ? NULL : i->dyn_cast<T>();
    }
    
    /// Check if item should be removed
    bool removed(void) const { return _flag_1; }
    /// Set flag to remove item
    void remove(void) { _flag_1 = true; }
    /// Unset remove item flag (only possible if not already removed by compact())
    void unremove(void) { _flag_1 = false; }
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
    ASTString f(void) const { return _f; }
    /// Set filename
    void f(const ASTString& nf) { _f = nf; }
    /// Access model
    Model* m(void) const { return _m; }
    /// Set the model
    void m(Model* m0, bool own=true) {
      assert(_m==NULL || m0==NULL); _m = m0; _flag_2 = own;
    }
    bool own(void) const {
      return _flag_2;
    }
  };

  /// \brief Variable declaration item
  class VarDeclI : public Item {
  protected:
    /// The declaration expression
    VarDecl* _e;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_VD;
    /// Constructor
    VarDeclI(const Location& loc, VarDecl* e);
    /// Access expression
    VarDecl* e(void) const { return _e; }
    /// Set expression
    void e(VarDecl* vd) { _e = vd; }
    /// Flag used during compilation
    bool flag(void) const {
      return _flag_2;
    }
    /// Set flag used during compilation
    void flag(bool b) {
      _flag_2 = b;
    }
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
    AssignI(const Location& loc,
            const std::string& id, Expression* e);
    /// Access identifier
    ASTString id(void) const { return _id; }
    /// Access expression
    Expression* e(void) const { return _e; }
    /// Set expression
    void e(Expression* e0) { _e = e0; }
    /// Access declaration
    VarDecl* decl(void) const { return _decl; }
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
    Expression* e(void) const { return _e; }
    /// Set expression
    void e(Expression* e0) { _e = e0; }
    /// Flag used during compilation
    bool flag(void) const {
      return _flag_2;
    }
    /// Set flag used during compilation
    void flag(bool b) {
      _flag_2 = b;
    }
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
    const Annotation& ann(void) const { return _ann; }
    /// Access solve annotation
    Annotation& ann(void) { return _ann; }
    /// Access expression for optimisation
    Expression* e(void) const { return _e; }
    /// Set expression for optimisation
    void e(Expression* e0) { _e=e0; }
    /// Return type of solving
    SolveType st(void) const;
    /// Set type of solving
    void st(SolveType s);
  };

  /// \brief Output item
  class OutputI : public Item {
  protected:
    /// Expression to output
    Expression* _e;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_OUT;
    /// Constructor
    OutputI(const Location& loc, Expression* e);
    /// Access expression
    Expression* e(void) const { return _e; }
    /// Update expression
    void e(Expression* e) { _e=e; }
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
    bool _from_stdlib;
  public:
    /// The identifier of this item type
    static const ItemId iid = II_FUN;
    
    /// Type of builtin expression-valued functions
    typedef Expression* (*builtin_e) (EnvI&, Call*);
    /// Type of builtin int-valued functions
    typedef IntVal (*builtin_i) (EnvI&, Call*);
    /// Type of builtin bool-valued functions
    typedef bool (*builtin_b) (EnvI&, Call*);
    /// Type of builtin float-valued functions
    typedef FloatVal (*builtin_f) (EnvI&, Call*);
    /// Type of builtin set-valued functions
    typedef IntSetVal* (*builtin_s) (EnvI&, Call*);
    /// Type of builtin string-valued functions
    typedef std::string (*builtin_str) (EnvI&, Call*);

    /// Builtin functions (or NULL)
    struct {
      builtin_e e;
      builtin_i i;
      builtin_f f;
      builtin_b b;
      builtin_s s;
      builtin_str str;
    } _builtins;

    /// Constructor
    FunctionI(const Location& loc,
              const std::string& id, TypeInst* ti,
              const std::vector<VarDecl*>& params,
              Expression* e = NULL);
    /// Constructor
    FunctionI(const Location& loc,
              const ASTString& id, TypeInst* ti,
              const ASTExprVec<VarDecl>& params,
              Expression* e = NULL);

    /// Access identifier
    ASTString id(void) const { return _id; }
    /// Access TypeInst
    TypeInst* ti(void) const { return _ti; }
    /// Access parameters
    ASTExprVec<VarDecl> params(void) const { return _params; }
    /// Access annotation
    const Annotation& ann(void) const { return _ann; }
    /// Access annotation
    Annotation& ann(void) { return _ann; }
    /// Access body
    Expression* e(void) const { return _e; }
    /// Set body
    void e(Expression* b) { _e = b; }
    
    /** \brief Compute return type given argument types \a ta
     */
    Type rtype(EnvI& env, const std::vector<Expression*>& ta, bool strictEnums);
    /** \brief Compute return type given argument types \a ta
     */
    Type rtype(EnvI& env, const std::vector<Type>& ta, bool strictEnums);
    /** \brief Compute expected type of argument \a n given argument types \a ta
     */
    Type argtype(EnvI& env, const std::vector<Expression*>& ta, int n);

    /// Return whether function is defined in the standard library
    bool from_stdlib(void) const { return _from_stdlib; };
    
    /// Mark for GC
    void mark(void) {
      _gc_mark = 1;
      loc().mark();
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
    void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    void vSetLit(const SetLit&) {}
    /// Visit string literal
    void vStringLit(const StringLit&) {}
    /// Visit identifier
    void vId(const Id&) {}
    /// Visit anonymous variable
    void vAnonVar(const AnonVar&) {}
    /// Visit array literal
    void vArrayLit(const ArrayLit&) {}
    /// Visit array access
    void vArrayAccess(const ArrayAccess&) {}
    /// Visit array comprehension
    void vComprehension(const Comprehension&) {}
    /// Visit array comprehension (only generator \a gen_i)
    void vComprehensionGenerator(const Comprehension&, int gen_i) { (void) gen_i; }
    /// Visit if-then-else
    void vITE(const ITE&) {}
    /// Visit binary operator
    void vBinOp(const BinOp&) {}
    /// Visit unary operator
    void vUnOp(const UnOp&) {}
    /// Visit call
    void vCall(const Call&) {}
    /// Visit let
    void vLet(const Let&) {}
    /// Visit variable declaration
    void vVarDecl(const VarDecl&) {}
    /// Visit type inst
    void vTypeInst(const TypeInst&) {}
    /// Visit TIId
    void vTIId(const TIId&) {}
    /// Determine whether to enter node
    bool enter(Expression* e) { return true; }
    /// Exit node after processing has finished
    void exit(Expression* e) {}
  };

  /// Statically allocated constants
  class Constants {
  private:
      /// Garbage collection root set for constants
      Model* m;
  public:
      /// Literal true
      BoolLit* lit_true;
      /// Variable bound to true
      VarDecl* var_true;
      /// Literal false
      BoolLit* lit_false;
      /// Variable bound to false
      VarDecl* var_false;
      /// Special variable to signal compiler to ignore result
      VarDecl* var_ignore;
      /// Infinite set
      SetLit* infinity;
      /// Function item used to keep track of redefined variables
      FunctionI* var_redef;
      /// Literal absent value
      Expression* absent;
      /// Identifiers for builtins
      struct {
        ASTString forall;
        ASTString forall_reif;
        ASTString exists;
        ASTString clause;
        ASTString bool2int;
        ASTString int2float;
        ASTString bool2float;
        ASTString assert;
        ASTString trace;

        ASTString sum;
        ASTString lin_exp;
        ASTString element;
	
        ASTString show;
        ASTString fix;
        ASTString output;

        struct {
          ASTString lin_eq;
          ASTString lin_le;
          ASTString lin_ne;
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
        } int_;

        struct {
          ASTString lin_eq;
          ASTString lin_le;
          ASTString lin_ne;
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
        } int_reif;

        struct {
          ASTString lin_eq;
          ASTString lin_le;
          ASTString lin_lt;
          ASTString lin_ne;
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
        } float_;

        struct {
          ASTString lin_eq;
          ASTString lin_le;
          ASTString lin_lt;
          ASTString lin_ne;
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
        } float_reif;

        ASTString bool_eq;
        ASTString bool_eq_reif;
        ASTString array_bool_or;
        ASTString array_bool_and;
        ASTString bool_clause;
        ASTString bool_clause_reif;
        ASTString bool_xor;
        ASTString set_eq;
        ASTString set_in;
        ASTString set_card;
        
        ASTString introduced_var;
      } ids;
    
      /// Identifiers for Boolean contexts
      struct {
        Id* root;
        Id* pos;
        Id* neg;
        Id* mix;
      } ctx;
      /// Common annotations
      struct {
        Id* output_var;
        ASTString output_array;
        Id* add_to_output;
        Id* mzn_check_var;
        Id* is_defined_var;
        ASTString defines_var;
        Id* is_reverse_map;
        Id* promise_total;
        Id* maybe_partial;
        ASTString doc_comment;
        ASTString mzn_path;
        ASTString is_introduced;
        Id* user_cut;            // MIP
        Id* lazy_constraint;            // MIP
        Id* mzn_break_here;
      } ann;

      /// Command line options
      struct { /// basic MiniZinc command line options
        ASTString cmdlineData_str;
        ASTString cmdlineData_short_str;        
        ASTString datafile_str;
        ASTString datafile_short_str;
        ASTString globalsDir_str;
        ASTString globalsDir_alt_str;
        ASTString globalsDir_short_str;
        ASTString help_str;
        ASTString help_short_str;
        ASTString ignoreStdlib_str;
        ASTString include_str;
        ASTString inputFromStdin_str;
        ASTString instanceCheckOnly_str;
        ASTString no_optimize_str;
        ASTString no_optimize_alt_str;
        ASTString no_outputOzn_str;
        ASTString no_outputOzn_short_str;
        ASTString no_typecheck_str;       
        ASTString newfzn_str;        
        ASTString outputBase_str;
        ASTString outputFznToStdout_str;
        ASTString outputFznToStdout_alt_str;
        ASTString outputOznToFile_str;
        ASTString outputOznToStdout_str;
        ASTString outputFznToFile_str;
        ASTString outputFznToFile_alt_str;
        ASTString outputFznToFile_short_str;
        ASTString rangeDomainsOnly_str;
        ASTString statistics_str;
        ASTString statistics_short_str;
        ASTString stdlib_str;
        ASTString verbose_str;
        ASTString verbose_short_str;
        ASTString version_str;
        ASTString werror_str; 
        
        struct {
          ASTString all_sols_str;
          ASTString fzn_solver_str;
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
          ASTString fzn_solver;
          ASTString fzn_flags;
          ASTString fzn_flag;
          ASTString fzn_time_limit_ms;
          ASTString fzn_sigint;
        } solver;
        
      } opts;
      
      /// categories of the command line interface options
      struct {
        ASTString general;
        ASTString io;        
        ASTString solver;
        ASTString translation;
      } cli_cat;
      
      /// Keep track of allocated integer literals
      UNORDERED_NAMESPACE::unordered_map<IntVal, WeakRef> integerMap;
      /// Keep track of allocated float literals
      UNORDERED_NAMESPACE::unordered_map<FloatVal, WeakRef> floatMap;
      /// Constructor
      Constants(void);
      /// Return shared BoolLit
      BoolLit* boollit(bool b) {
        return b ? lit_true : lit_false;
      }
      static const int max_array_size = INT_MAX / 2;
  };
    
  /// Return static instance
  Constants& constants(void);

}

#include <minizinc/ast.hpp>

#endif
