/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_VALUES_HH__
#define __MINIZINC_VALUES_HH__

#include <minizinc/gc.hh>
#include <minizinc/exception.hh>
#include <minizinc/stl_map_set.hh>
#include <minizinc/thirdparty/SafeInt3.hpp>
#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <limits.h>

namespace MiniZinc {
  class IntVal;
}
namespace std {
  MiniZinc::IntVal abs(const MiniZinc::IntVal& x);
}

#ifdef _MSC_VER
#define MZN_NORETURN __declspec(noreturn)
#define MZN_NORETURN_ATTR
#else
#define MZN_NORETURN
#define MZN_NORETURN_ATTR __attribute__((__noreturn__))
#endif

namespace MiniZinc {
  
  class MiniZincSafeIntExceptionHandler
  {
  public:
    static MZN_NORETURN void SafeIntOnOverflow() MZN_NORETURN_ATTR
    {
      throw ArithmeticError( "integer overflow" );
    }
    
    static MZN_NORETURN void SafeIntOnDivZero() MZN_NORETURN_ATTR
    {
      throw ArithmeticError( "integer division by zero" );
    }
  };
}

#undef MZN_NORETURN

namespace MiniZinc {
  
  class FloatVal;
  
  class IntVal {
    friend IntVal operator +(const IntVal& x, const IntVal& y);
    friend IntVal operator -(const IntVal& x, const IntVal& y);
    friend IntVal operator *(const IntVal& x, const IntVal& y);
    friend IntVal operator /(const IntVal& x, const IntVal& y);
    friend IntVal operator %(const IntVal& x, const IntVal& y);
    friend IntVal std::abs(const MiniZinc::IntVal& x);
    friend bool operator ==(const IntVal& x, const IntVal& y);
    friend class FloatVal;
  private:
    long long int _v;
    bool _infinity;
    IntVal(long long int v, bool infinity) : _v(v), _infinity(infinity) {}
    typedef SafeInt<long long int, MiniZincSafeIntExceptionHandler> SI;
    SI toSafeInt(void) const { return _v; }
    IntVal(SI v) : _v(v), _infinity(false) {}
  public:
    IntVal(void) : _v(0), _infinity(false) {}
    IntVal(long long int v) : _v(v), _infinity(false) {}
    IntVal(const FloatVal& v);
    
    long long int toInt(void) const {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      return _v;
    }
    
    bool isFinite(void) const { return !_infinity; }
    bool isPlusInfinity(void) const { return _infinity && _v==1; }
    bool isMinusInfinity(void) const { return _infinity && _v==-1; }
    
    IntVal& operator +=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = toSafeInt() + x.toSafeInt();
      return *this;
    }
    IntVal& operator -=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = toSafeInt() - x.toSafeInt();
      return *this;
    }
    IntVal& operator *=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = toSafeInt() * x.toSafeInt();
      return *this;
    }
    IntVal& operator /=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = toSafeInt() / x.toSafeInt();
      return *this;
    }
    IntVal operator -() const {
      IntVal r = *this;
      r._v = -r.toSafeInt();
      return r;
    }
    IntVal& operator ++() {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = toSafeInt() + 1;
      return *this;
    }
    IntVal operator ++(int) {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      IntVal ret = *this;
      _v = toSafeInt() + 1;
      return ret;
    }
    IntVal& operator --() {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = toSafeInt() - 1;
      return *this;
    }
    IntVal operator --(int) {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      IntVal ret = *this;
      _v = toSafeInt() - 1;
      return ret;
    }
    static const IntVal minint(void);
    static const IntVal maxint(void);
    static const IntVal infinity(void);
    
    /// Infinity-safe addition
    IntVal plus(int x) const {
      if (isFinite())
        return toSafeInt()+x;
      else
        return *this;
    }
    /// Infinity-safe subtraction
    IntVal minus(int x) const {
      if (isFinite())
        return toSafeInt()-x;
      else
        return *this;
    }
    
    size_t hash(void) const {
      HASH_NAMESPACE::hash<long long int> longhash;
      return longhash(_v);
    }
    
  };

  inline
  bool operator ==(const IntVal& x, const IntVal& y) {
    return x._infinity==y._infinity && x._v == y._v;
  }
  inline
  bool operator <=(const IntVal& x, const IntVal& y) {
    return y.isPlusInfinity() || x.isMinusInfinity() || (x.isFinite() && y.isFinite() && x.toInt() <= y.toInt());
  }
  inline
  bool operator <(const IntVal& x, const IntVal& y) {
    return
      (y.isPlusInfinity() && !x.isPlusInfinity()) ||
      (x.isMinusInfinity() && !y.isMinusInfinity()) ||
      (x.isFinite() && y.isFinite() && x.toInt() < y.toInt());
  }
  inline
  bool operator >=(const IntVal& x, const IntVal& y) {
    return y <= x;
  }
  inline
  bool operator >(const IntVal& x, const IntVal& y) {
    return y < x;
  }
  inline
  bool operator !=(const IntVal& x, const IntVal& y) {
    return !(x==y);
  }
  inline
  IntVal operator +(const IntVal& x, const IntVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toSafeInt()+y.toSafeInt();
  }
  inline
  IntVal operator -(const IntVal& x, const IntVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toSafeInt()-y.toSafeInt();
  }
  inline
  IntVal operator *(const IntVal& x, const IntVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toSafeInt()*y.toSafeInt();
  }
  inline
  IntVal operator /(const IntVal& x, const IntVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toInt()/y.toInt();
  }
  inline
  IntVal operator %(const IntVal& x, const IntVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toInt()%y.toInt();
  }
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const IntVal& s) {
    if (s.isMinusInfinity())
      return os << "-infinity";
    else if (s.isPlusInfinity())
      return os << "infinity";
    else
      return os << s.toInt();
  }
  
}


namespace std {
  inline
  MiniZinc::IntVal abs(const MiniZinc::IntVal& x) {
    if (!x.isFinite()) return MiniZinc::IntVal::infinity();
    MiniZinc::IntVal::SI y(x.toInt());
    return y < 0 ? MiniZinc::IntVal(static_cast<long long int>(-y)) : x;
  }
  
  inline
  MiniZinc::IntVal min(const MiniZinc::IntVal& x, const MiniZinc::IntVal& y) {
    return x <= y ? x : y;
  }
  inline
  MiniZinc::IntVal max(const MiniZinc::IntVal& x, const MiniZinc::IntVal& y) {
    return x >= y ? x : y;
  }
  
  template<>
  struct equal_to<MiniZinc::IntVal> {
  public:
    bool operator()(const MiniZinc::IntVal& s0,
                    const MiniZinc::IntVal& s1) const {
      return s0==s1;
    }
  };

  inline MiniZinc::FloatVal abs(const MiniZinc::FloatVal&);
}

OPEN_HASH_NAMESPACE {
  template<>
  struct hash<MiniZinc::IntVal> {
  public:
    size_t operator()(const MiniZinc::IntVal& s) const {
      return s.hash();
    }
  };
CLOSE_HASH_NAMESPACE }

namespace MiniZinc {
  
  class FloatVal {
    friend FloatVal operator +(const FloatVal& x, const FloatVal& y);
    friend FloatVal operator -(const FloatVal& x, const FloatVal& y);
    friend FloatVal operator *(const FloatVal& x, const FloatVal& y);
    friend FloatVal operator /(const FloatVal& x, const FloatVal& y);
    friend FloatVal std::abs(const MiniZinc::FloatVal& x);
    friend bool operator ==(const FloatVal& x, const FloatVal& y);
    friend class IntVal;
  private:
    double _v;
    bool _infinity;
    FloatVal(double v, bool infinity) : _v(v), _infinity(infinity) {}
  public:
    FloatVal(void) : _v(0.0), _infinity(false) {}
    FloatVal(double v) : _v(v), _infinity(false) {}
    FloatVal(const IntVal& v) : _v(v._v), _infinity(!v.isFinite()) {}
    
    double toDouble(void) const {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      return _v;
    }
    
    bool isFinite(void) const { return !_infinity; }
    bool isPlusInfinity(void) const { return _infinity && _v==1.0; }
    bool isMinusInfinity(void) const { return _infinity && _v==-1.0; }
    
    FloatVal& operator +=(const FloatVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v += x._v;
      return *this;
    }
    FloatVal& operator -=(const FloatVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v -= x._v;
      return *this;
    }
    FloatVal& operator *=(const FloatVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v *= x._v;
      return *this;
    }
    FloatVal& operator /=(const FloatVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = _v / x._v;
      return *this;
    }
    FloatVal operator -() const {
      FloatVal r = *this;
      r._v = -r._v;
      return r;
    }
    FloatVal& operator ++() {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = _v + 1;
      return *this;
    }
    FloatVal operator ++(int) {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      FloatVal ret = *this;
      _v = _v + 1;
      return ret;
    }
    FloatVal& operator --() {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      _v = _v - 1;
      return *this;
    }
    FloatVal operator --(int) {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      FloatVal ret = *this;
      _v = _v - 1;
      return ret;
    }

    static const FloatVal infinity(void);
    
    /// Infinity-safe addition
    FloatVal plus(int x) {
      if (isFinite())
        return (*this)+x;
      else
        return *this;
    }
    /// Infinity-safe subtraction
    FloatVal minus(int x) {
      if (isFinite())
        return (*this)-x;
      else
        return *this;
    }
    
    size_t hash(void) const {
      HASH_NAMESPACE::hash<long long int> longhash;
      return longhash(_v);
    }
    
  };
  
  inline
  bool operator ==(const FloatVal& x, const FloatVal& y) {
    return x._infinity==y._infinity && x._v == y._v;
  }
  inline
  bool operator <=(const FloatVal& x, const FloatVal& y) {
    return y.isPlusInfinity() || x.isMinusInfinity() || (x.isFinite() && y.isFinite() && x.toDouble() <= y.toDouble());
  }
  inline
  bool operator <(const FloatVal& x, const FloatVal& y) {
    return
    (y.isPlusInfinity() && !x.isPlusInfinity()) ||
    (x.isMinusInfinity() && !y.isMinusInfinity()) ||
    (x.isFinite() && y.isFinite() && x.toDouble() < y.toDouble());
  }
  inline
  bool operator >=(const FloatVal& x, const FloatVal& y) {
    return y <= x;
  }
  inline
  bool operator >(const FloatVal& x, const FloatVal& y) {
    return y < x;
  }
  inline
  bool operator !=(const FloatVal& x, const FloatVal& y) {
    return !(x==y);
  }
  inline
  FloatVal operator +(const FloatVal& x, const FloatVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toDouble()+y.toDouble();
  }
  inline
  FloatVal operator -(const FloatVal& x, const FloatVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toDouble()-y.toDouble();
  }
  inline
  FloatVal operator *(const FloatVal& x, const FloatVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toDouble()*y.toDouble();
  }
  inline
  FloatVal operator /(const FloatVal& x, const FloatVal& y) {
    if (! (x.isFinite() && y.isFinite()))
      throw ArithmeticError("arithmetic operation on infinite value");
    return x.toDouble()/y.toDouble();
  }
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const FloatVal& s) {
    if (s.isMinusInfinity())
      return os << "-infinity";
    else if (s.isPlusInfinity())
      return os << "infinity";
    else
      return os << s.toDouble();
  }
  
  inline
  IntVal::IntVal(const FloatVal& v)
    : _v(static_cast<long long int>(v._v)), _infinity(!v.isFinite()) {}
  
}


namespace std {
  inline
  MiniZinc::FloatVal abs(const MiniZinc::FloatVal& x) {
    if (!x.isFinite()) return MiniZinc::FloatVal::infinity();
    return x.toDouble() < 0 ? MiniZinc::FloatVal(-x.toDouble()) : x;
  }
  
  inline
  MiniZinc::FloatVal min(const MiniZinc::FloatVal& x, const MiniZinc::FloatVal& y) {
    return x <= y ? x : y;
  }
  inline
  MiniZinc::FloatVal max(const MiniZinc::FloatVal& x, const MiniZinc::FloatVal& y) {
    return x >= y ? x : y;
  }
  
  inline
  MiniZinc::FloatVal floor(const MiniZinc::FloatVal& x) {
    if (!x.isFinite()) return x;
    return floor(x.toDouble());
  }
  inline
  MiniZinc::FloatVal ceil(const MiniZinc::FloatVal& x) {
    if (!x.isFinite()) return x;
    return ceil(x.toDouble());
  }

  template<>
  struct equal_to<MiniZinc::FloatVal> {
  public:
    bool operator()(const MiniZinc::FloatVal& s0,
                    const MiniZinc::FloatVal& s1) const {
      return s0==s1;
    }
  };
}

OPEN_HASH_NAMESPACE {
  template<>
  struct hash<MiniZinc::FloatVal> {
  public:
    size_t operator()(const MiniZinc::FloatVal& s) const {
      return s.hash();
    }
  };
CLOSE_HASH_NAMESPACE }


namespace MiniZinc {

  typedef unsigned long long int UIntVal;

  /// An integer set value
  class IntSetVal : public ASTChunk {
  public:
    /// Contiguous range
    struct Range {
      /// Range minimum
      IntVal min;
      /// Range maximum
      IntVal max;
      /// Construct range from \a m to \a n
      Range(IntVal m, IntVal n) : min(m), max(n) {}
      /// Default constructor
      Range(void) {}
    };
  private:
    /// Return range at position \a i
    Range& get(int i) {
      return reinterpret_cast<Range*>(_data)[i];
    }
    /// Return range at position \a i
    const Range& get(int i) const {
      return reinterpret_cast<const Range*>(_data)[i];
    }
    /// Construct empty set
    IntSetVal(void) : ASTChunk(0) {}
    /// Construct set of single range
    IntSetVal(IntVal m, IntVal n);
    /// Construct set from \a s
    IntSetVal(const std::vector<Range>& s)
      : ASTChunk(sizeof(Range)*s.size()) {
      for (unsigned int i=s.size(); i--;)
        get(i) = s[i];
    }

    /// Disabled
    IntSetVal(const IntSetVal& r);
    /// Disabled
    IntSetVal& operator =(const IntSetVal& r);
  public:
    /// Return number of ranges
    int size(void) const { return _size / sizeof(Range); }
    /// Return minimum, or infinity if set is empty
    IntVal min(void) const { return size()==0 ? IntVal::infinity() : get(0).min; }
    /// Return maximum, or minus infinity if set is empty
    IntVal max(void) const { return size()==0 ? -IntVal::infinity() : get(size()-1).max; }
    /// Return minimum of range \a i
    IntVal min(int i) const { assert(i<size()); return get(i).min; }
    /// Return maximum of range \a i
    IntVal max(int i) const { assert(i<size()); return get(i).max; }
    /// Return width of range \a i
    IntVal width(int i) const {
      assert(i<size());
      if (min(i).isFinite() && max(i).isFinite())
        return max(i)-min(i)+1;
      else
        return IntVal::infinity();
    }
    /// Return cardinality
    IntVal card(void) const {
      IntVal c = 0;
      for (unsigned int i=size(); i--;) {
        if (width(i).isFinite())
          c += width(i);
        else
          return IntVal::infinity();
      }
      return c;
    }

    /// Allocate empty set from context
    static IntSetVal* a(void) {
      IntSetVal* r = static_cast<IntSetVal*>(ASTChunk::alloc(0));
      new (r) IntSetVal();
      return r;
    }
    
    /// Allocate set \f$\{m,n\}\f$ from context
    static IntSetVal* a(IntVal m, IntVal n) {
      if (m>n) {
        return a();
      } else {
        IntSetVal* r =
          static_cast<IntSetVal*>(ASTChunk::alloc(sizeof(Range)));
        new (r) IntSetVal(m,n);
        return r;
      }
    }

    /// Allocate set using iterator \a i
    template<class I>
    static IntSetVal* ai(I& i) {
      std::vector<Range> s;
      for (; i(); ++i)
        s.push_back(Range(i.min(),i.max()));
      IntSetVal* r = static_cast<IntSetVal*>(
          ASTChunk::alloc(sizeof(Range)*s.size()));
      new (r) IntSetVal(s);
      return r;
    }
    
    /// Allocate set from vector \a s0 (may contain duplicates)
    static IntSetVal* a(const std::vector<IntVal>& s0) {
      if (s0.size()==0)
        return a();
      std::vector<IntVal> s=s0;
      std::sort(s.begin(),s.end());
      std::vector<Range> ranges;
      IntVal min=s[0];
      IntVal max=min;
      for (unsigned int i=1; i<s.size(); i++) {
        if (s[i]>max+1) {
          ranges.push_back(Range(min,max));
          min=s[i]; max=min;
        } else {
          max=s[i];
        }
      }
      ranges.push_back(Range(min,max));
      IntSetVal* r = static_cast<IntSetVal*>(
          ASTChunk::alloc(sizeof(Range)*ranges.size()));
      new (r) IntSetVal(ranges);
      return r;
    }
    static IntSetVal* a(const std::vector<Range>& ranges) {
      IntSetVal* r = static_cast<IntSetVal*>(ASTChunk::alloc(sizeof(Range)*ranges.size()));
      new (r) IntSetVal(ranges);
      return r;
    }
    
    /// Check if set contains \a v
    bool contains(const IntVal& v) {
      for (int i=0; i<size(); i++) {
        if (v < min(i))
          return false;
        if (v <= max(i))
          return true;
      }
      return false;
    }
    
    /// Check if it is equal to \a s
    bool equal(const IntSetVal* s) {
      if (size()!=s->size())
        return false;
      for (int i=0; i<size(); i++)
        if (min(i)!=s->min(i) || max(i)!=s->max(i))
          return false;
      return true;
    }
    
    /// Mark for garbage collection
    void mark(void) {
      _gc_mark = 1;
    }
  };
  
  /// Iterator over an IntSetVal
  class IntSetRanges {
    /// The set value
    const IntSetVal* rs;
    /// The current range
    int n;
  public:
    /// Constructor
    IntSetRanges(const IntSetVal* r) : rs(r), n(0) {}
    /// Check if iterator is still valid
    bool operator()(void) const { return n<rs->size(); }
    /// Move to next range
    void operator++(void) { ++n; }
    /// Return minimum of current range
    IntVal min(void) const { return rs->min(n); }
    /// Return maximum of current range
    IntVal max(void) const { return rs->max(n); }
    /// Return width of current range
    IntVal width(void) const { return rs->width(n); }
  };
  
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const IntSetVal& s) {
    if (s.size()==0) {
      os << "1..0";
    } else {
      for (IntSetRanges isr(&s); isr(); ++isr)
        os << isr.min() << ".." << isr.max() << " ";
    }
    return os;
  }
  
  /// An integer set value
  class FloatSetVal : public ASTChunk {
  public:
    /// Contiguous range
    struct Range {
      /// Range minimum
      FloatVal min;
      /// Range maximum
      FloatVal max;
      /// Construct range from \a m to \a n
      Range(FloatVal m, FloatVal n) : min(m), max(n) {}
      /// Default constructor
      Range(void) {}
    };
  private:
    /// Return range at position \a i
    Range& get(int i) {
      return reinterpret_cast<Range*>(_data)[i];
    }
    /// Return range at position \a i
    const Range& get(int i) const {
      return reinterpret_cast<const Range*>(_data)[i];
    }
    /// Construct empty set
    FloatSetVal(void) : ASTChunk(0) {}
    /// Construct set of single range
    FloatSetVal(FloatVal m, FloatVal n);
    /// Construct set from \a s
    FloatSetVal(const std::vector<Range>& s)
    : ASTChunk(sizeof(Range)*s.size()) {
      for (unsigned int i=s.size(); i--;)
        get(i) = s[i];
    }
    
    /// Disabled
    FloatSetVal(const FloatSetVal& r);
    /// Disabled
    FloatSetVal& operator =(const FloatSetVal& r);
  public:
    /// Return number of ranges
    int size(void) const { return _size / sizeof(Range); }
    /// Return minimum, or infinity if set is empty
    FloatVal min(void) const { return size()==0 ? FloatVal::infinity() : get(0).min; }
    /// Return maximum, or minus infinity if set is empty
    FloatVal max(void) const { return size()==0 ? -FloatVal::infinity() : get(size()-1).max; }
    /// Return minimum of range \a i
    FloatVal min(int i) const { assert(i<size()); return get(i).min; }
    /// Return maximum of range \a i
    FloatVal max(int i) const { assert(i<size()); return get(i).max; }
    /// Return width of range \a i
    FloatVal width(int i) const {
      assert(i<size());
      if (min(i).isFinite() && max(i).isFinite() && min(i)==max(i))
        return max(i)-min(i)+1;
      else
        return IntVal::infinity();
    }
    /// Return cardinality
    FloatVal card(void) const {
      IntVal c = 0;
      for (unsigned int i=size(); i--;) {
        if (width(i).isFinite())
          c += width(i);
        else
          return IntVal::infinity();
      }
      return c;
    }
    
    /// Allocate empty set from context
    static FloatSetVal* a(void) {
      FloatSetVal* r = static_cast<FloatSetVal*>(ASTChunk::alloc(0));
      new (r) FloatSetVal();
      return r;
    }
    
    /// Allocate set \f$\{m,n\}\f$ from context
    static FloatSetVal* a(FloatVal m, FloatVal n) {
      if (m>n) {
        return a();
      } else {
        FloatSetVal* r =
          static_cast<FloatSetVal*>(ASTChunk::alloc(sizeof(Range)));
        new (r) FloatSetVal(m,n);
        return r;
      }
    }
    
    /// Allocate set using iterator \a i
    template<class I>
    static FloatSetVal* ai(I& i) {
      std::vector<Range> s;
      for (; i(); ++i)
        s.push_back(Range(i.min(),i.max()));
      FloatSetVal* r = static_cast<FloatSetVal*>(ASTChunk::alloc(sizeof(Range)*s.size()));
      new (r) FloatSetVal(s);
      return r;
    }
    
    /// Allocate set from vector \a s0 (may contain duplicates)
    static FloatSetVal* a(const std::vector<FloatVal>& s0) {
      if (s0.size()==0)
        return a();
      std::vector<FloatVal> s=s0;
      std::sort(s.begin(),s.end());
      std::vector<Range> ranges;
      FloatVal min=s[0];
      FloatVal max=min;
      for (unsigned int i=1; i<s.size(); i++) {
        if (s[i]>max) {
          ranges.push_back(Range(min,max));
          min=s[i]; max=min;
        } else {
          max=s[i];
        }
      }
      ranges.push_back(Range(min,max));
      FloatSetVal* r = static_cast<FloatSetVal*>(ASTChunk::alloc(sizeof(Range)*ranges.size()));
      new (r) FloatSetVal(ranges);
      return r;
    }
    static FloatSetVal* a(const std::vector<Range>& ranges) {
      FloatSetVal* r = static_cast<FloatSetVal*>(ASTChunk::alloc(sizeof(Range)*ranges.size()));
      new (r) FloatSetVal(ranges);
      return r;
    }
    
    /// Check if set contains \a v
    bool contains(const FloatVal& v) {
      for (int i=0; i<size(); i++) {
        if (v < min(i))
          return false;
        if (v <= max(i))
          return true;
      }
      return false;
    }
    
    /// Check if it is equal to \a s
    bool equal(const FloatSetVal* s) {
      if (size()!=s->size())
        return false;
      for (int i=0; i<size(); i++)
        if (min(i)!=s->min(i) || max(i)!=s->max(i))
          return false;
      return true;
    }
    
    /// Mark for garbage collection
    void mark(void) {
      _gc_mark = 1;
    }
  };
  
  /// Iterator over an IntSetVal
  class FloatSetRanges {
    /// The set value
    const FloatSetVal* rs;
    /// The current range
    int n;
  public:
    /// Constructor
    FloatSetRanges(const FloatSetVal* r) : rs(r), n(0) {}
    /// Check if iterator is still valid
    bool operator()(void) const { return n<rs->size(); }
    /// Move to next range
    void operator++(void) { ++n; }
    /// Return minimum of current range
    FloatVal min(void) const { return rs->min(n); }
    /// Return maximum of current range
    FloatVal max(void) const { return rs->max(n); }
    /// Return width of current range
    FloatVal width(void) const { return rs->width(n); }
  };
  
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const FloatSetVal& s) {
    for (FloatSetRanges isr(&s); isr(); ++isr)
      os << isr.min() << ".." << isr.max() << " ";
    return os;
  }
}

#endif
