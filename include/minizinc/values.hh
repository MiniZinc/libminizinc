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

  /// An integer set value
  class IntSetVal : public ASTChunk {
  private:
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
    IntSetVal(int m, int n) : ASTChunk(sizeof(Range)) {
      get(0).min = m;
      get(0).max = n;
    }
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
    /// Return minimum of range \a i
    IntVal min(int i) const { assert(i<size()); return get(i).min; }
    /// Return maximum of range \a i
    IntVal max(int i) const { assert(i<size()); return get(i).max; }
    /// Return width of range \a i
    IntVal width(int i) const { assert(i<size()); return max(i)-min(i)+1; }
    /// Return cardinality
    unsigned int card(void) const {
      unsigned int c = 0;
      for (unsigned int i=size(); i--;)
        c += width(i);
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
        new (r) IntSetVal(static_cast<int>(m),static_cast<int>(n));
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
  
  
}

#endif
