/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_ITER_HH__
#define __MINIZINC_ITER_HH__

#include <minizinc/values.hh>
#include <cmath>

#ifdef _MSC_VER 
#define _CRT_SECURE_NO_WARNINGS
#undef ERROR    // MICROsoft.
#undef min
#undef max
#endif

namespace MiniZinc { namespace Ranges {
  
  /**
   * \brief Base for range iterators with explicit min and max
   *
   * The iterator provides members \a mi and \a ma for storing the
   * limits of the currently iterated range. The iterator
   * continues until \a mi becomes greater than \a ma. The member function
   * finish does exactly that.
   *
   * \ingroup FuncIterRanges
   */

  template<class Val>
  class MinMax {
  protected:
    /// Minimum of current range
    Val mi;
    /// Maximum of current range
    Val ma;
    /// %Set range such that iteration stops
    void finish(void);
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    MinMax(void);
    /// Initialize with range \a min to \a max
    MinMax(Val min, Val max);
    //@}

    /// \name Iteration control
    //@{
    /// Test whether iterator is still at a range or done
    bool operator ()(void) const;
    //@}

    /// \name Range access
    //@{
    /// Return smallest value of range
    Val min(void) const;
    /// Return largest value of range
    Val max(void) const;
    /// Return width of range (distance between minimum and maximum)
    Val width(void) const;
    //@}
  };

  template<class Val>
  inline void
  MinMax<Val>::finish(void) {
    mi = 1; ma = 0;
  }

  template<class Val>
  inline
  MinMax<Val>::MinMax(void) {}

  template<class Val>
  inline
  MinMax<Val>::MinMax(Val min, Val max)
    : mi(min), ma(max) {}

  template<class Val>
  inline bool
  MinMax<Val>::operator ()(void) const {
    return mi <= ma;
  }

  template<class Val>
  inline Val
  MinMax<Val>::min(void) const {
    return mi;
  }
  template<class Val>
  inline Val
  MinMax<Val>::max(void) const {
    return ma;
  }
  template<class Val>
  inline Val
  MinMax<Val>::width(void) const {
    if (mi > ma)
      return 0;
    if (mi.isFinite() && ma.isFinite())
      return ma-mi+1;
    return Val::infinity();
  }
  
  
  template<class Val,class I>
  class Bounded {
  protected:
    I i;
    Val _min;
    bool use_min;
    Val _max;
    bool use_max;
    Bounded(I& i, Val min0, bool umin0, Val max0, bool umax0);
  public:
    static Bounded miniter(I& i, Val min);
    static Bounded maxiter(I& i, Val max);
    static Bounded minmaxiter(I& i, Val min, Val max);

    /// \name Iteration control
    //@{
    /// Test whether iterator is still at a range or done
    bool operator ()(void) const;
    /// Move iterator to next range (if possible)
    void operator ++(void);
    //@}

    /// \name Range access
    //@{
    /// Return smallest value of range
    Val min(void) const;
    /// Return largest value of range
    Val max(void) const;
    /// Return width of range (distance between minimum and maximum)
    Val width(void) const;
    //@}
  };

  template<class Val,class I>
  inline
  Bounded<Val,I>::Bounded(I& i0, Val min0, bool umin0, Val max0, bool umax0)
    : i(i0), _min(min0), use_min(umin0), _max(max0), use_max(umax0) {
    while (i() && use_min && i.max() < _min)
      ++i;
  }
  template<class Val,class I>
  inline Bounded<Val,I>
  Bounded<Val,I>::miniter(I& i, Val min) {
    return Bounded(i,min,true,0,false);
  }
  template<class Val,class I>
  inline Bounded<Val,I>
  Bounded<Val,I>::maxiter(I& i, Val max) {
    return Bounded(i,0,false,max,true);
  }
  template<class Val,class I>
  inline Bounded<Val,I>
  Bounded<Val,I>::minmaxiter(I& i, Val min, Val max) {
    return Bounded(i,min,true,max,true);
  }

  template<class Val,class I>
  inline bool
  Bounded<Val,I>::operator ()(void) const {
    return i() && (!use_max || i.min() <= _max);
  }
  template<class Val,class I>
  inline void
  Bounded<Val,I>::operator ++(void) {
    ++i;
    while (i() && use_min && i.max() < _min)
      ++i;
  }
  template<class Val,class I>
  inline Val
  Bounded<Val,I>::min(void) const {
    return use_min ? std::max(_min,i.min()) : i.min();
  }
  template<class Val,class I>
  inline Val
  Bounded<Val,I>::max(void) const {
    return use_max ? std::min(_max,i.max()) : i.max();
  }
  template<class Val,class I>
  inline Val
  Bounded<Val,I>::width(void) const {
    if (min() > max())
      return 0;
    if (min().isFinite() && max().isFinite())
      return max()-min()+1;
    return Val::infinity();
  }

  template<class Val>
  class Const {
  protected:
    Val _min;
    Val _max;
    bool done;
  public:
    Const(Val min0, Val max0);

    /// \name Iteration control
    //@{
    /// Test whether iterator is still at a range or done
    bool operator ()(void) const;
    /// Move iterator to next range (if possible)
    void operator ++(void);
    //@}

    /// \name Range access
    //@{
    /// Return smallest value of range
    Val min(void) const;
    /// Return largest value of range
    Val max(void) const;
    /// Return width of range (distance between minimum and maximum)
    Val width(void) const;
    //@}
  };

  template<class Val>
  inline
  Const<Val>::Const(Val min0, Val max0) : _min(min0), _max(max0), done(min0>max0) {}
  template<class Val>
  inline bool
  Const<Val>::operator ()(void) const {
    return !done;
  }
  template<class Val>
  inline void
  Const<Val>::operator ++(void) {
    done = true;
  }
  template<class Val>
  inline Val
  Const<Val>::min(void) const { return _min; }
  template<class Val>
  inline Val
  Const<Val>::max(void) const { return _max; }
  template<class Val>
  inline Val
  Const<Val>::width(void) const {
    if (min() > max())
      return 0;
    if (min().isFinite() && max().isFinite())
      return max()-min()+1;
    return Val::infinity();
  }
  
  /**
   * \brief Range iterator for computing union (binary)
   *
   * \ingroup FuncIterRanges
   */
  template<class Val, class I, class J>
  class Union : public MinMax<Val> {
  protected:
    /// First iterator
    I i;
    /// Second iterator
    J j;
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    Union(void);
    /// Initialize with iterator \a i and \a j
    Union(I& i, J& j);
    /// Initialize with iterator \a i and \a j
    void init(I& i, J& j);
    //@}

    /// \name Iteration control
    //@{
    /// Move iterator to next range (if possible)
    void operator ++(void);
    //@}
  };

  /// Return whether an interval ending with \a x overlaps with an interval starting at \a y
  inline
  bool overlaps(const IntVal& x, const IntVal& y) {
    return x.plus(1) >= y;
  }
  /// Return whether an interval ending with \a x overlaps with an interval starting at \a y
  inline
  bool overlaps(const FloatVal& x, const FloatVal& y) {
    if (x.isPlusInfinity())
      return true;
    if (y.isMinusInfinity())
      return true;
    if (x.isFinite() && y.isFinite()) {
      return std::nextafter(x.toDouble(),INFINITY) >= y.toDouble();
    }
    return x >= y;
  }
  inline
  IntVal nextHigher(const IntVal& x) { return x.plus(1); }
  inline
  IntVal nextLower(const IntVal& x) { return x.minus(1); }
  inline
  FloatVal nextHigher(const FloatVal& x) {
    if (x.isFinite())
      return std::nextafter(x.toDouble(),INFINITY);
    return x;
  }
  inline
  FloatVal nextLower(const FloatVal& x) {
    if (x.isFinite())
      return std::nextafter(x.toDouble(),-INFINITY);
    return x;
  }


  /*
   * Binary union
   *
   */

  template<class Val, class I, class J>
  inline void
  Union<Val,I,J>::operator ++(void) {
    if (!i() && !j()) {
      MinMax<Val>::finish(); return;
    }

    if (!i() || (j() && (!overlaps(j.max(),i.min())))) {
      MinMax<Val>::mi = j.min(); MinMax<Val>::ma = j.max(); ++j; return;
    }
    if (!j() || (i() && (!overlaps(i.max(),j.min())))) {
      MinMax<Val>::mi = i.min(); MinMax<Val>::ma = i.max(); ++i; return;
    }

    MinMax<Val>::mi = std::min(i.min(),j.min());
    MinMax<Val>::ma = std::max(i.max(),j.max());

    ++i; ++j;

  next:
    if (i() && (overlaps(MinMax<Val>::ma,i.min()))) {
      MinMax<Val>::ma = std::max(MinMax<Val>::ma,i.max()); ++i;
      goto next;
    }
    if (j() && (overlaps(MinMax<Val>::ma,j.min()))) {
      MinMax<Val>::ma = std::max(MinMax<Val>::ma,j.max()); ++j;
      goto next;
    }
  }


  template<class Val, class I, class J>
  inline
  Union<Val,I,J>::Union(void) {}

  template<class Val, class I, class J>
  inline
  Union<Val,I,J>::Union(I& i0, J& j0)
    : i(i0), j(j0) {
    operator ++();
  }

  template<class Val, class I, class J>
  inline void
  Union<Val,I,J>::init(I& i0, J& j0) {
    i = i0; j = j0;
    operator ++();
  }

  /**
   * \brief Range iterator for computing intersection (binary)
   *
   * \ingroup FuncIterRanges
   */
  template<class Val, class I, class J>
  class Inter : public MinMax<Val> {
  protected:
    /// First iterator
    I i;
    /// Second iterator
    J j;
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    Inter(void);
    /// Initialize with iterator \a i and \a j
    Inter(I& i, J& j);
    /// Initialize with iterator \a i and \a j
    void init(I& i, J& j);
    //@}

    /// \name Iteration control
    //@{
    /// Move iterator to next range (if possible)
    void operator ++(void);
    //@}
  };




  /*
   * Binary intersection
   *
   */

  template<class Val, class I, class J>
  inline void
  Inter<Val,I,J>::operator ++(void) {
    if (!i() || !j()) goto done;
    do {
      while (i() && (i.max() < j.min())) ++i;
      if (!i()) goto done;
      while (j() && (j.max() < i.min())) ++j;
      if (!j()) goto done;
    } while (i.max() < j.min());
    // Now the intervals overlap: consume the smaller interval
    MinMax<Val>::ma = std::min(i.max(),j.max());
    MinMax<Val>::mi = std::max(i.min(),j.min());
    if (i.max() < j.max()) ++i; else ++j;
    return;
  done:
    MinMax<Val>::finish();
  }

  template<class Val, class I, class J>
  inline
  Inter<Val,I,J>::Inter(void) {}

  template<class Val, class I, class J>
  inline
  Inter<Val,I,J>::Inter(I& i0, J& j0)
    : i(i0), j(j0) {
    operator ++();
  }

  template<class Val, class I, class J>
  inline void
  Inter<Val,I,J>::init(I& i0, J& j0) {
    i = i0; j = j0;
    operator ++();
  }

  /**
   * \brief Range iterator for computing set difference
   *
   * \ingroup FuncIterRanges
   */

  template<class Val, class I, class J>
  class Diff : public MinMax<Val> {
  protected:
    /// Iterator from which to subtract
    I i;
    /// Iterator to be subtracted
    J j;
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    Diff(void);
    /// Initialize with iterator \a i and \a j
    Diff(I& i, J& j);
    /// Initialize with iterator \a i and \a j
    void init(I& i, J& j);
    //@}

    /// \name Iteration control
    //@{
    /// Move iterator to next range (if possible)
    void operator ++(void);
    //@}
  };



  template<class Val, class I, class J>
  inline void
  Diff<Val,I,J>::operator ++(void) {
    // Precondition: mi <= ma
    // Task: find next mi greater than ma
    while (true) {
      if (!i()) break;
      bool isInfinite = (!MinMax<Val>::ma.isFinite() && MinMax<Val>::ma>0);
      MinMax<Val>::mi = nextHigher(MinMax<Val>::ma);
      MinMax<Val>::ma = i.max();
      if (isInfinite || MinMax<Val>::mi > i.max()) {
        ++i;
        if (!i()) break;
        MinMax<Val>::mi = i.min();
        MinMax<Val>::ma = i.max();
      }
      while (j() && (j.max() < MinMax<Val>::mi))
        ++j;
      if (j() && (j.min() <= MinMax<Val>::ma)) {
        // Now the interval [mi ... ma] must be shrunken
        // Is [mi ... ma] completely consumed?
        if ((MinMax<Val>::mi >= j.min()) && (MinMax<Val>::ma <= j.max()))
          continue;
        // Does [mi ... ma] overlap on the left?
        if (j.min() <= MinMax<Val>::mi) {
          MinMax<Val>::mi = nextHigher(j.max());
          // Search for max!
          ++j;
          if (j() && (j.min() <= MinMax<Val>::ma))
            MinMax<Val>::ma = nextLower(j.min());
        } else {
          MinMax<Val>::ma = nextLower(j.min());
        }
      }
      return;
    }
    MinMax<Val>::finish();
  }

  template<class Val, class I, class J>
  inline
  Diff<Val,I,J>::Diff(void) {}

  template<class Val, class I, class J>
  inline
  Diff<Val,I,J>::Diff(I& i0, J& j0)
    : i(i0), j(j0) {
    if (!i()) {
      MinMax<Val>::finish();
    } else {
      MinMax<Val>::mi = nextLower(i.min()); MinMax<Val>::ma = MinMax<Val>::mi;
      operator ++();
    }
  }

  template<class Val, class I, class J>
  inline void
  Diff<Val,I,J>::init(I& i0, J& j0) {
    i = i0; j = j0;
    if (!i()) {
      MinMax<Val>::finish();
    } else {
      MinMax<Val>::mi = nextLower(i.min()); MinMax<Val>::ma = MinMax<Val>::mi;
      operator ++();
    }
  }

  /**
   * \brief Value iterator from range iterator
   *
   * \ingroup FuncIterValues
   */
  template<class I>
  class ToValues {
  protected:
    /// Range iterator used
    I i;
    /// Current value
    IntVal cur;
    /// End of current range
    IntVal max;
    /// Initialize iterator
    void start(void);
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    ToValues(void);
    /// Initialize with values from range iterator \a i
    ToValues(I& i);
    /// Initialize with values from range iterator \a i
    void init(I& i);
    //@}

    /// \name Iteration control
    //@{
    /// Test whether iterator is still at a value or done
    bool operator ()(void) const;
    /// Move iterator to next value (if possible)
    void operator ++(void);
    //@}

    /// \name Value access
    //@{
    /// Return current value
    IntVal  val(void) const;
    //@}
  };



  template<class I>
  inline
  ToValues<I>::ToValues(void) {}

  template<class I>
  inline void
  ToValues<I>::start(void) {
    if (i()) {
      cur = i.min(); max = i.max();
    } else {
      cur = 1;       max = 0;
    }
  }

  template<class I>
  inline
  ToValues<I>::ToValues(I& i0)
    : i(i0) {
    start();
  }

  template<class I>
  inline void
  ToValues<I>::init(I& i0) {
    i = i0;
    start();
  }


  template<class I>
  inline bool
  ToValues<I>::operator ()(void) const {
    return (cur <= max);
  }

  template<class I>
  inline void
  ToValues<I>::operator ++(void) {
    ++cur;
    if (cur > max) {
      ++i;
      if (i()) {
        cur = i.min(); max = i.max();
      }
    }
  }

  template<class I>
  inline IntVal
  ToValues<I>::val(void) const {
    return cur;
  }

  /**
   * \defgroup FuncIterRangesOp Operations on range iterators
   *
   * \ingroup FuncIterRanges
   */

  //@{
  /// Cardinality of the set represented by range iterator \a i
  template<class I>
  IntVal cardinality(I& i);

  /// Check whether range iterators \a i and \a j are equal
  template<class I, class J>
  bool equal(I& i, J& j);

  /// Check whether range iterator \a i is subset of range iterator \a j
  template<class I, class J>
  bool subset(I& i, J& j);

  /// Check whether range iterators \a i and \a j are disjoint
  template<class I, class J>
  bool disjoint(I& i, J& j);

  /// Comapre two iterators with each other
  enum CompareStatus {
    CS_SUBSET,   ///< First is subset of second iterator
    CS_DISJOINT, ///< Intersection is empty
    CS_NONE      ///< Neither of the above
  };

  /// Check whether range iterator \a i is a subset of \a j, or whether they are disjoint
  template<class I, class J>
  CompareStatus compare(I& i, J& j);
  //@}


  template<class I>
  inline IntVal
  cardinality(I& i) {
    IntVal s = 0;
    while (i()) {
      if (i.width().isFinite()) {
        s += i.width(); ++i;
      } else {
        return IntVal::infinity();
      }
    }
    return s;
  }

  template<class I, class J>
  inline bool
  equal(I& i, J& j) {
    // Are i and j equal?
    while (i() && j())
      if ((i.min() == j.min()) && (i.max() == j.max())) {
        ++i; ++j;
      } else {
        return false;
      }
    return !i() && !j();
  }

  template<class I, class J>
  inline bool
  subset(I& i, J& j) {
    // Is i subset of j?
    while (i() && j())
      if (j.max() < i.min()) {
        ++j;
      } else if ((i.min() >= j.min()) && (i.max() <= j.max())) {
        ++i;
      } else {
        return false;
      }
    return !i();
  }

  template<class I, class J>
  inline bool
  disjoint(I& i, J& j) {
    // Are i and j disjoint?
    while (i() && j())
      if (j.max() < i.min()) {
        ++j;
      } else if (i.max() < j.min()) {
        ++i;
      } else {
        return false;
      }
    return true;
  }

  template<class I, class J>
  inline CompareStatus
  compare(I& i, J& j) {
    bool subset = true;
    bool disjoint = true;
    while (i() && j()) {
      if (j.max() < i.min()) {
        ++j;
      } else if (i.max() < j.min()) {
        ++i; subset = false;
      } else if ((i.min() >= j.min()) && (i.max() <= j.max())) {
        ++i; disjoint = false;
      } else if (i.max() <= j.max()) {
        ++i; disjoint = false; subset = false;
      } else if (j.max() <= i.max()) {
        ++j; disjoint = false; subset = false;
      }
    }
    if (i())
      subset = false;
    if (subset)
      return CS_SUBSET;
    return disjoint ? CS_DISJOINT : CS_NONE;
  }

  template<class I, class J>
  inline bool
  less(I& i, J& j) {
    while (i()) {
      if (!j())
        return false;
      if (i.min() < j.min())
        return true;
      if (i.min() > j.min())
        return false;
      if (i.max() < j.max())
        return true;
      if (i.max() > j.max())
        return false;
      ++i;
      ++j;
    }
    if (j())
      return true;
    return false;
  }

  template<class I, class J>
  inline bool
  lessEq(I& i, J& j) {
    while (i()) {
      if (!j())
        return false;
      if (i.min() < j.min())
        return true;
      if (i.min() > j.min())
        return false;
      if (i.max() < j.max())
        return true;
      if (i.max() > j.max())
        return false;
      ++i;
      ++j;
    }
    return true;
  }

}}

#endif
