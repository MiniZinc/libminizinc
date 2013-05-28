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
#include <algorithm>

namespace MiniZinc {
  
  typedef long long int IntVal;
  typedef unsigned long long int UIntVal;

  typedef double FloatVal;

  class IntSetVal : public ASTChunk {
  private:
    struct Range {
      IntVal min; IntVal max;
      Range(IntVal m, IntVal n) : min(m), max(n) {}
      Range(void) {}
    };

    Range& get(int i) {
      return reinterpret_cast<Range*>(_data)[i];
    }
    const Range& get(int i) const {
      return reinterpret_cast<const Range*>(_data)[i];
    }

    IntSetVal(void) : ASTChunk(0) {}
    IntSetVal(int m, int n) : ASTChunk(sizeof(Range)) {
      get(0).min = m;
      get(0).max = n;
    }
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
    int size(void) const { return _size / sizeof(Range); }
    IntVal min(int i) const { assert(i<size()); return get(i).min; }
    IntVal max(int i) const { assert(i<size()); return get(i).max; }
    IntVal width(int i) const { assert(i<size()); return max(i)-min(i)+1; }

    /// Allocate empty set from context
    static IntSetVal* a(void) {
      IntSetVal* r = static_cast<IntSetVal*>(ASTChunk::alloc(0));
      new (r) IntSetVal();
      return r;
    }
    
    /// Allocate set \f$\{m,n\}\f$ from context
    static IntSetVal* a(IntVal m, IntVal n) {
      IntSetVal* r =
        static_cast<IntSetVal*>(ASTChunk::alloc(sizeof(Range)));
      new (r) IntSetVal(m,n);
      return r;
    }

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
          ASTChunk::alloc(sizeof(Range)*s.size()));
      new (r) IntSetVal(ranges);
      return r;
    }
    
    bool contains(const IntVal& v) {
      for (int i=0; i<size(); i++) {
        if (v < min(i))
          return false;
        if (v <= max(i))
          return true;
      }
      return false;
    }
    
    void mark(void) {
      _gc_mark = 1;
    }
  };
  
  class IntSetRanges {
    const IntSetVal* rs;
    int n;
  public:
    IntSetRanges(const IntSetVal* r) : rs(r), n(0) {}
    bool operator()(void) const { return n<rs->size(); }
    void operator++(void) { ++n; }
    IntVal min(void) const { return rs->min(n); }
    IntVal max(void) const { return rs->max(n); }
    IntVal width(void) const { return rs->width(n); }
  };
  
  
}

#endif
