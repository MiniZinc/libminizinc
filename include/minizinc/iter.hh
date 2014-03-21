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

  class MinMax {
  protected:
    /// Minimum of current range
    IntVal mi;
    /// Maximum of current range
    IntVal ma;
    /// %Set range such that iteration stops
    void finish(void);
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    MinMax(void);
    /// Initialize with range \a min to \a max
    MinMax(IntVal min, IntVal max);
    //@}

    /// \name Iteration control
    //@{
    /// Test whether iterator is still at a range or done
    bool operator ()(void) const;
    //@}

    /// \name Range access
    //@{
    /// Return smallest value of range
    IntVal min(void) const;
    /// Return largest value of range
    IntVal max(void) const;
    /// Return width of range (distance between minimum and maximum)
    IntVal width(void) const;
    //@}
  };

  inline void
  MinMax::finish(void) {
    mi = 1; ma = 0;
  }

  inline
  MinMax::MinMax(void) {}

  inline
  MinMax::MinMax(IntVal min, IntVal max)
    : mi(min), ma(max) {}

  inline bool
  MinMax::operator ()(void) const {
    return mi <= ma;
  }

  inline IntVal
  MinMax::min(void) const {
    return mi;
  }
  inline IntVal
  MinMax::max(void) const {
    return ma;
  }
  inline IntVal
  MinMax::width(void) const {
    return ma-mi+1;
  }
  
  
  template<class I>
  class Bounded {
  protected:
    I i;
    IntVal _min;
    bool use_min;
    IntVal _max;
    bool use_max;
    Bounded(I& i, IntVal min0, bool umin0, IntVal max0, bool umax0);
  public:
    static Bounded miniter(I& i, IntVal min);
    static Bounded maxiter(I& i, IntVal max);
    static Bounded minmaxiter(I& i, IntVal min, IntVal max);

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
    IntVal min(void) const;
    /// Return largest value of range
    IntVal max(void) const;
    /// Return width of range (distance between minimum and maximum)
    UIntVal width(void) const;
    //@}
  };

  template<class I>
  inline
  Bounded<I>::Bounded(I& i0, IntVal min0, bool umin0, IntVal max0, bool umax0)
    : i(i0), _min(min0), use_min(umin0), _max(max0), use_max(umax0) {
    while (i() && use_min && i.max() < _min)
      ++i;
  }
  template<class I>
  inline Bounded<I>
  Bounded<I>::miniter(I& i, IntVal min) {
    return Bounded(i,min,true,0,false);
  }
  template<class I>
  inline Bounded<I>
  Bounded<I>::maxiter(I& i, IntVal max) {
    return Bounded(i,0,false,max,true);
  }
  template<class I>
  inline Bounded<I>
  Bounded<I>::minmaxiter(I& i, IntVal min, IntVal max) {
    return Bounded(i,min,true,max,true);
  }

  template<class I>
  inline bool
  Bounded<I>::operator ()(void) const {
    return i() && (!use_max || i.min() <= _max);
  }
  template<class I>
  inline void
  Bounded<I>::operator ++(void) {
    ++i;
    while (i() && use_min && i.max() < _min)
      ++i;
  }
  template<class I>
  inline IntVal
  Bounded<I>::min(void) const {
    return use_min ? std::max(_min,i.min()) : i.min();
  }
  template<class I>
  inline IntVal
  Bounded<I>::max(void) const {
    return use_max ? std::min(_max,i.max()) : i.max();
  }
  template<class I>
  inline UIntVal
  Bounded<I>::width(void) const {
    return static_cast<UIntVal>(max()-min())+1;
  }

  class Const {
  protected:
    IntVal _min;
    IntVal _max;
    bool done;
  public:
    Const(IntVal min0, IntVal max0);

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
    IntVal min(void) const;
    /// Return largest value of range
    IntVal max(void) const;
    /// Return width of range (distance between minimum and maximum)
    IntVal width(void) const;
    //@}
  };

  inline
  Const::Const(IntVal min0, IntVal max0) : _min(min0), _max(max0), done(min0>max0) {}
  inline bool
  Const::operator ()(void) const {
    return !done;
  }
  inline void
  Const::operator ++(void) {
    done = true;
  }
  inline IntVal
  Const::min(void) const { return _min; }
  inline IntVal
  Const::max(void) const { return _max; }
  inline IntVal
  Const::width(void) const { return max()-min()+1; }
  
  /**
   * \brief Range iterator for computing union (binary)
   *
   * \ingroup FuncIterRanges
   */
  template<class I, class J>
  class Union : public MinMax {
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





  /*
   * Binary union
   *
   */

  template<class I, class J>
  inline void
  Union<I,J>::operator ++(void) {
    if (!i() && !j()) {
      finish(); return;
    }

    if (!i() || (j() && (j.max()+1 < i.min()))) {
      mi = j.min(); ma = j.max(); ++j; return;
    }
    if (!j() || (i() && (i.max()+1 < j.min()))) {
      mi = i.min(); ma = i.max(); ++i; return;
    }

    mi = std::min(i.min(),j.min());
    ma = std::max(i.max(),j.max());

    ++i; ++j;

  next:
    if (i() && (i.min() <= ma+1)) {
      ma = std::max(ma,i.max()); ++i;
      goto next;
    }
    if (j() && (j.min() <= ma+1)) {
      ma = std::max(ma,j.max()); ++j;
      goto next;
    }
  }


  template<class I, class J>
  inline
  Union<I,J>::Union(void) {}

  template<class I, class J>
  inline
  Union<I,J>::Union(I& i0, J& j0)
    : i(i0), j(j0) {
    operator ++();
  }

  template<class I, class J>
  inline void
  Union<I,J>::init(I& i0, J& j0) {
    i = i0; j = j0;
    operator ++();
  }

  /**
   * \brief Range iterator for computing intersection (binary)
   *
   * \ingroup FuncIterRanges
   */
  template<class I, class J>
  class Inter : public MinMax {
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

  template<class I, class J>
  inline void
  Inter<I,J>::operator ++(void) {
    if (!i() || !j()) goto done;
    do {
      while (i() && (i.max() < j.min())) ++i;
      if (!i()) goto done;
      while (j() && (j.max() < i.min())) ++j;
      if (!j()) goto done;
    } while (i.max() < j.min());
    // Now the intervals overlap: consume the smaller interval
    ma = std::min(i.max(),j.max());
    mi = std::max(i.min(),j.min());
    if (i.max() < j.max()) ++i; else ++j;
    return;
  done:
    finish();
  }

  template<class I, class J>
  inline
  Inter<I,J>::Inter(void) {}

  template<class I, class J>
  inline
  Inter<I,J>::Inter(I& i0, J& j0)
    : i(i0), j(j0) {
    operator ++();
  }

  template<class I, class J>
  inline void
  Inter<I,J>::init(I& i0, J& j0) {
    i = i0; j = j0;
    operator ++();
  }

  /**
   * \brief Range iterator for computing set difference
   *
   * \ingroup FuncIterRanges
   */

  template<class I, class J>
  class Diff : public MinMax {
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



  template<class I, class J>
  inline void
  Diff<I,J>::operator ++(void) {
    // Precondition: mi <= ma
    // Task: find next mi greater than ma
    while (true) {
      if (!i()) break;
      mi = ma+1;
      ma = i.max();
      if (mi > i.max()) {
        ++i;
        if (!i()) break;
        mi = i.min();
        ma = i.max();
      }
      while (j() && (j.max() < mi))
        ++j;
      if (j() && (j.min() <= ma)) {
        // Now the interval [mi ... ma] must be shrunken
        // Is [mi ... ma] completely consumed?
        if ((mi >= j.min()) && (ma <= j.max()))
          continue;
        // Does [mi ... ma] overlap on the left?
        if (j.min() <= mi) {
          mi = j.max()+1;
          // Search for max!
          ++j;
          if (j() && (j.min() <= ma))
            ma = j.min()-1;
        } else {
          ma = j.min()-1;
        }
      }
      return;
    }
    finish();
  }

  template<class I, class J>
  inline
  Diff<I,J>::Diff(void) {}

  template<class I, class J>
  inline
  Diff<I,J>::Diff(I& i0, J& j0)
    : i(i0), j(j0) {
    if (!i()) {
      finish();
    } else {
      mi = i.min()-1; ma = mi;
      operator ++();
    }
  }

  template<class I, class J>
  inline void
  Diff<I,J>::init(I& i0, J& j0) {
    i = i0; j = j0;
    if (!i()) {
      finish();
    } else {
      mi = i.min()-1; ma = mi;
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
  /// Size of all ranges of range iterator \a i
  template<class I>
  IntVal size(I& i);

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
  size(I& i) {
    IntVal s = 0;
    while (i()) {
      s += i.width(); ++i;
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
  
}}

#endif
