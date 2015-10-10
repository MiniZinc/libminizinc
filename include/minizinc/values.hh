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
  
  class IntVal {
    friend IntVal operator +(const IntVal& x, const IntVal& y);
    friend IntVal operator -(const IntVal& x, const IntVal& y);
    friend IntVal operator *(const IntVal& x, const IntVal& y);
    friend IntVal operator /(const IntVal& x, const IntVal& y);
    friend IntVal operator %(const IntVal& x, const IntVal& y);
    friend IntVal std::abs(const MiniZinc::IntVal& x);
    friend bool operator ==(const IntVal& x, const IntVal& y);
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
      _v += x._v;
      return *this;
    }
    IntVal& operator -=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v -= x._v;
      return *this;
    }
    IntVal& operator *=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v *= x._v;
      return *this;
    }
    IntVal& operator /=(const IntVal& x) {
      if (! (isFinite() && x.isFinite()))
        throw ArithmeticError("arithmetic operation on infinite value");
      _v /= x._v;
      return *this;
    }
    IntVal operator -() const {
      IntVal r = *this;
      r._v = -r._v;
      return r;
    }
    void operator ++() {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      ++_v;
    }
    void operator ++(int) {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      ++_v;
    }
    void operator --() {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      --_v;
    }
    void operator --(int) {
      if (!isFinite())
        throw ArithmeticError("arithmetic operation on infinite value");
      --_v;
    }
    static const IntVal minint(void);
    static const IntVal maxint(void);
    static const IntVal infinity(void);
    
    /// Infinity-safe addition
    IntVal plus(int x) {
      if (isFinite())
        return toSafeInt()+x;
      else
        return *this;
    }
    /// Infinity-safe subtraction
    IntVal minus(int x) {
      if (isFinite())
        return toSafeInt()-x;
      else
        return *this;
    }
  };

  inline
  bool operator ==(const IntVal& x, const IntVal& y) {
    return x.isFinite()==y.isFinite() && x._v == y._v;
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
}

OPEN_HASH_NAMESPACE {
  template<>
  struct hash<MiniZinc::IntVal> {
  public:
    size_t operator()(const MiniZinc::IntVal& s) const {
      HASH_NAMESPACE::hash<long long int> longhash;
      size_t h;
      if (s.isPlusInfinity())
        h = longhash(LONG_MAX);
      else if (s.isMinusInfinity())
        h = longhash(LONG_MIN);
      else
        h = longhash(s.toInt());
      HASH_NAMESPACE::hash<bool> boolhash;
      h ^= boolhash(s.isFinite()) + 0x9e3779b9 + (h << 6) + (h >> 2);
      return h;
    }
  };
CLOSE_HASH_NAMESPACE }

namespace MiniZinc {

  typedef unsigned long long int UIntVal;

  typedef double FloatVal;

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
    for (IntSetRanges isr(&s); isr(); ++isr)
      os << isr.min() << ".." << isr.max() << " ";
    return os;
  }
  
}

#endif
