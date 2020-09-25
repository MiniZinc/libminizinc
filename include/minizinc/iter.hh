/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/values.hh>

#include <cmath>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace MiniZinc {
namespace Ranges {

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

template <class Val>
class MinMax {
protected:
  /// Minimum of current range
  Val _mi;
  /// Maximum of current range
  Val _ma;
  /// %Set range such that iteration stops
  void finish();

public:
  /// \name Constructors and initialization
  //@{
  /// Default constructor
  MinMax();
  /// Initialize with range \a min to \a max
  MinMax(Val min, Val max);
  //@}

  /// \name Iteration control
  //@{
  /// Test whether iterator is still at a range or done
  bool operator()() const;
  //@}

  /// \name Range access
  //@{
  /// Return smallest value of range
  Val min() const;
  /// Return largest value of range
  Val max() const;
  /// Return width of range (distance between minimum and maximum)
  Val width() const;
  //@}
};

template <class Val>
inline void MinMax<Val>::finish() {
  _mi = 1;
  _ma = 0;
}

template <class Val>
inline MinMax<Val>::MinMax() {}

template <class Val>
inline MinMax<Val>::MinMax(Val min, Val max) : _mi(min), _ma(max) {}

template <class Val>
inline bool MinMax<Val>::operator()() const {
  return _mi <= _ma;
}

template <class Val>
inline Val MinMax<Val>::min() const {
  return _mi;
}
template <class Val>
inline Val MinMax<Val>::max() const {
  return _ma;
}
template <class Val>
inline Val MinMax<Val>::width() const {
  if (_mi > _ma) {
    return 0;
  }
  if (_mi.isFinite() && _ma.isFinite()) {
    return _ma - _mi + 1;
  }
  return Val::infinity();
}

template <class Val, class I>
class Bounded {
protected:
  I _i;
  Val _min;
  bool _useMin;
  Val _max;
  bool _useMax;
  Bounded(I& i, Val min0, bool umin0, Val max0, bool umax0);

public:
  static Bounded miniter(I& i, Val min);
  static Bounded maxiter(I& i, Val max);
  static Bounded minmaxiter(I& i, Val min, Val max);

  /// \name Iteration control
  //@{
  /// Test whether iterator is still at a range or done
  bool operator()() const;
  /// Move iterator to next range (if possible)
  void operator++();
  //@}

  /// \name Range access
  //@{
  /// Return smallest value of range
  Val min() const;
  /// Return largest value of range
  Val max() const;
  /// Return width of range (distance between minimum and maximum)
  Val width() const;
  //@}
};

template <class Val, class I>
inline Bounded<Val, I>::Bounded(I& i, Val min0, bool umin0, Val max0, bool umax0)
    : _i(i), _min(min0), _useMin(umin0), _max(max0), _useMax(umax0) {
  while (_i() && _useMin && _i.max() < _min) {
    ++_i;
  }
}
template <class Val, class I>
inline Bounded<Val, I> Bounded<Val, I>::miniter(I& i, Val min) {
  return Bounded(i, min, true, 0, false);
}
template <class Val, class I>
inline Bounded<Val, I> Bounded<Val, I>::maxiter(I& i, Val max) {
  return Bounded(i, 0, false, max, true);
}
template <class Val, class I>
inline Bounded<Val, I> Bounded<Val, I>::minmaxiter(I& i, Val min, Val max) {
  return Bounded(i, min, true, max, true);
}

template <class Val, class I>
inline bool Bounded<Val, I>::operator()() const {
  return _i() && (!_useMax || _i.min() <= _max);
}
template <class Val, class I>
inline void Bounded<Val, I>::operator++() {
  ++_i;
  while (_i() && _useMin && _i.max() < _min) {
    ++_i;
  }
}
template <class Val, class I>
inline Val Bounded<Val, I>::min() const {
  return _useMin ? std::max(_min, _i.min()) : _i.min();
}
template <class Val, class I>
inline Val Bounded<Val, I>::max() const {
  return _useMax ? std::min(_max, _i.max()) : _i.max();
}
template <class Val, class I>
inline Val Bounded<Val, I>::width() const {
  if (min() > max()) {
    return 0;
  }
  if (min().isFinite() && max().isFinite()) {
    return max() - min() + 1;
  }
  return Val::infinity();
}

template <class Val>
class Const {
protected:
  Val _min;
  Val _max;
  bool _done;

public:
  Const(Val min0, Val max0);

  /// \name Iteration control
  //@{
  /// Test whether iterator is still at a range or done
  bool operator()() const;
  /// Move iterator to next range (if possible)
  void operator++();
  //@}

  /// \name Range access
  //@{
  /// Return smallest value of range
  Val min() const;
  /// Return largest value of range
  Val max() const;
  /// Return width of range (distance between minimum and maximum)
  Val width() const;
  //@}
};

template <class Val>
inline Const<Val>::Const(Val min0, Val max0) : _min(min0), _max(max0), _done(min0 > max0) {}
template <class Val>
inline bool Const<Val>::operator()() const {
  return !_done;
}
template <class Val>
inline void Const<Val>::operator++() {
  _done = true;
}
template <class Val>
inline Val Const<Val>::min() const {
  return _min;
}
template <class Val>
inline Val Const<Val>::max() const {
  return _max;
}
template <class Val>
inline Val Const<Val>::width() const {
  if (min() > max()) {
    return 0;
  }
  if (min().isFinite() && max().isFinite()) {
    return max() - min() + 1;
  }
  return Val::infinity();
}

/**
 * \brief Range iterator for computing union (binary)
 *
 * \ingroup FuncIterRanges
 */
template <class Val, class I, class J>
class Union : public MinMax<Val> {
protected:
  /// First iterator
  I _i;
  /// Second iterator
  J _j;

public:
  /// \name Constructors and initialization
  //@{
  /// Default constructor
  Union();
  /// Initialize with iterator \a i and \a j
  Union(I& i, J& j);
  /// Initialize with iterator \a i and \a j
  void init(I& i, J& j);
  //@}

  /// \name Iteration control
  //@{
  /// Move iterator to next range (if possible)
  void operator++();
  //@}
};

/// Return whether an interval ending with \a x overlaps with an interval starting at \a y
inline bool overlaps(const IntVal& x, const IntVal& y) { return x.plus(1) >= y; }
/// Return whether an interval ending with \a x overlaps with an interval starting at \a y
inline bool overlaps(const FloatVal& x, const FloatVal& y) {
  if (x.isPlusInfinity()) {
    return true;
  }
  if (y.isMinusInfinity()) {
    return true;
  }
  if (x.isFinite() && y.isFinite()) {
    return std::nextafter(x.toDouble(), INFINITY) >= y.toDouble();
  }
  return x >= y;
}
inline IntVal next_higher(const IntVal& x) { return x.plus(1); }
inline IntVal next_lower(const IntVal& x) { return x.minus(1); }
inline FloatVal next_higher(const FloatVal& x) {
  if (x.isFinite()) {
    return std::nextafter(x.toDouble(), INFINITY);
  }
  return x;
}
inline FloatVal next_lower(const FloatVal& x) {
  if (x.isFinite()) {
    return std::nextafter(x.toDouble(), -INFINITY);
  }
  return x;
}

/*
 * Binary union
 *
 */

template <class Val, class I, class J>
inline void Union<Val, I, J>::operator++() {
  if (!_i() && !_j()) {
    MinMax<Val>::finish();
    return;
  }

  if (!_i() || (_j() && (!overlaps(_j.max(), _i.min())))) {
    MinMax<Val>::_mi = _j.min();
    MinMax<Val>::_ma = _j.max();
    ++_j;
    return;
  }
  if (!_j() || (_i() && (!overlaps(_i.max(), _j.min())))) {
    MinMax<Val>::_mi = _i.min();
    MinMax<Val>::_ma = _i.max();
    ++_i;
    return;
  }

  MinMax<Val>::_mi = std::min(_i.min(), _j.min());
  MinMax<Val>::_ma = std::max(_i.max(), _j.max());

  ++_i;
  ++_j;

next:
  if (_i() && (overlaps(MinMax<Val>::_ma, _i.min()))) {
    MinMax<Val>::_ma = std::max(MinMax<Val>::_ma, _i.max());
    ++_i;
    goto next;
  }
  if (_j() && (overlaps(MinMax<Val>::_ma, _j.min()))) {
    MinMax<Val>::_ma = std::max(MinMax<Val>::_ma, _j.max());
    ++_j;
    goto next;
  }
}

template <class Val, class I, class J>
inline Union<Val, I, J>::Union() {}

template <class Val, class I, class J>
inline Union<Val, I, J>::Union(I& i, J& j) : _i(i), _j(j) {
  operator++();
}

template <class Val, class I, class J>
inline void Union<Val, I, J>::init(I& i, J& j) {
  _i = i;
  _j = j;
  operator++();
}

/**
 * \brief Range iterator for computing intersection (binary)
 *
 * \ingroup FuncIterRanges
 */
template <class Val, class I, class J>
class Inter : public MinMax<Val> {
protected:
  /// First iterator
  I _i;
  /// Second iterator
  J _j;

public:
  /// \name Constructors and initialization
  //@{
  /// Default constructor
  Inter();
  /// Initialize with iterator \a i and \a j
  Inter(I& i, J& j);
  /// Initialize with iterator \a i and \a j
  void init(I& i, J& j);
  //@}

  /// \name Iteration control
  //@{
  /// Move iterator to next range (if possible)
  void operator++();
  //@}
};

/*
 * Binary intersection
 *
 */

template <class Val, class I, class J>
inline void Inter<Val, I, J>::operator++() {
  if (!_i() || !_j()) {
    goto done;
  }
  do {
    while (_i() && (_i.max() < _j.min())) {
      ++_i;
    }
    if (!_i()) {
      goto done;
    }
    while (_j() && (_j.max() < _i.min())) {
      ++_j;
    }
    if (!_j()) {
      goto done;
    }
  } while (_i.max() < _j.min());
  // Now the intervals overlap: consume the smaller interval
  MinMax<Val>::_ma = std::min(_i.max(), _j.max());
  MinMax<Val>::_mi = std::max(_i.min(), _j.min());
  if (_i.max() < _j.max()) {
    ++_i;
  } else {
    ++_j;
  }
  return;
done:
  MinMax<Val>::finish();
}

template <class Val, class I, class J>
inline Inter<Val, I, J>::Inter() {}

template <class Val, class I, class J>
inline Inter<Val, I, J>::Inter(I& i, J& j) : _i(i), _j(j) {
  operator++();
}

template <class Val, class I, class J>
inline void Inter<Val, I, J>::init(I& i, J& j) {
  _i = i;
  _j = j;
  operator++();
}

/**
 * \brief Range iterator for computing set difference
 *
 * \ingroup FuncIterRanges
 */

template <class Val, class I, class J>
class Diff : public MinMax<Val> {
protected:
  /// Iterator from which to subtract
  I _i;
  /// Iterator to be subtracted
  J _j;

public:
  /// \name Constructors and initialization
  //@{
  /// Default constructor
  Diff();
  /// Initialize with iterator \a i and \a j
  Diff(I& i, J& j);
  /// Initialize with iterator \a i and \a j
  void init(I& i, J& j);
  //@}

  /// \name Iteration control
  //@{
  /// Move iterator to next range (if possible)
  void operator++();
  //@}
};

template <class Val, class I, class J>
inline void Diff<Val, I, J>::operator++() {
  // Precondition: mi <= ma
  // Task: find next mi greater than ma
  while (true) {
    if (!_i()) {
      break;
    }
    bool isInfinite = (!MinMax<Val>::_ma.isFinite() && MinMax<Val>::_ma > 0);
    MinMax<Val>::_mi = next_higher(MinMax<Val>::_ma);
    MinMax<Val>::_ma = _i.max();
    if (isInfinite || MinMax<Val>::_mi > _i.max()) {
      ++_i;
      if (!_i()) {
        break;
      }
      MinMax<Val>::_mi = _i.min();
      MinMax<Val>::_ma = _i.max();
    }
    while (_j() && (_j.max() < MinMax<Val>::_mi)) {
      ++_j;
    }
    if (_j() && (_j.min() <= MinMax<Val>::_ma)) {
      // Now the interval [mi ... ma] must be shrunken
      // Is [mi ... ma] completely consumed?
      if ((MinMax<Val>::_mi >= _j.min()) && (MinMax<Val>::_ma <= _j.max())) {
        continue;
      }
      // Does [mi ... ma] overlap on the left?
      if (_j.min() <= MinMax<Val>::_mi) {
        MinMax<Val>::_mi = next_higher(_j.max());
        // Search for max!
        ++_j;
        if (_j() && (_j.min() <= MinMax<Val>::_ma)) {
          MinMax<Val>::_ma = next_lower(_j.min());
        }
      } else {
        MinMax<Val>::_ma = next_lower(_j.min());
      }
    }
    return;
  }
  MinMax<Val>::finish();
}

template <class Val, class I, class J>
inline Diff<Val, I, J>::Diff() {}

template <class Val, class I, class J>
inline Diff<Val, I, J>::Diff(I& i, J& j) : _i(i), _j(j) {
  if (!_i()) {
    MinMax<Val>::finish();
  } else {
    MinMax<Val>::_mi = next_lower(_i.min());
    MinMax<Val>::_ma = MinMax<Val>::_mi;
    operator++();
  }
}

template <class Val, class I, class J>
inline void Diff<Val, I, J>::init(I& i, J& j) {
  _i = i;
  _j = j;
  if (!_i()) {
    MinMax<Val>::finish();
  } else {
    MinMax<Val>::_mi = next_lower(_i.min());
    MinMax<Val>::_ma = MinMax<Val>::_mi;
    operator++();
  }
}

/**
 * \brief Value iterator from range iterator
 *
 * \ingroup FuncIterValues
 */
template <class I>
class ToValues {
protected:
  /// Range iterator used
  I _i;
  /// Current value
  IntVal _cur;
  /// End of current range
  IntVal _max;
  /// Initialize iterator
  void start();

public:
  /// \name Constructors and initialization
  //@{
  /// Default constructor
  ToValues();
  /// Initialize with values from range iterator \a i
  ToValues(I& i);
  /// Initialize with values from range iterator \a i
  void init(I& i);
  //@}

  /// \name Iteration control
  //@{
  /// Test whether iterator is still at a value or done
  bool operator()() const;
  /// Move iterator to next value (if possible)
  void operator++();
  //@}

  /// \name Value access
  //@{
  /// Return current value
  IntVal val() const;
  //@}
};

template <class I>
inline ToValues<I>::ToValues() {}

template <class I>
inline void ToValues<I>::start() {
  if (_i()) {
    _cur = _i.min();
    _max = _i.max();
  } else {
    _cur = 1;
    _max = 0;
  }
}

template <class I>
inline ToValues<I>::ToValues(I& i) : _i(i) {
  start();
}

template <class I>
inline void ToValues<I>::init(I& i) {
  _i = i;
  start();
}

template <class I>
inline bool ToValues<I>::operator()() const {
  return (_cur <= _max);
}

template <class I>
inline void ToValues<I>::operator++() {
  ++_cur;
  if (_cur > _max) {
    ++_i;
    if (_i()) {
      _cur = _i.min();
      _max = _i.max();
    }
  }
}

template <class I>
inline IntVal ToValues<I>::val() const {
  return _cur;
}

/**
 * \defgroup FuncIterRangesOp Operations on range iterators
 *
 * \ingroup FuncIterRanges
 */

//@{
/// Cardinality of the set represented by range iterator \a i
template <class I>
IntVal cardinality(I& i);

/// Check whether range iterators \a i and \a j are equal
template <class I, class J>
bool equal(I& i, J& j);

/// Check whether range iterator \a i is subset of range iterator \a j
template <class I, class J>
bool subset(I& i, J& j);

/// Check whether range iterators \a i and \a j are disjoint
template <class I, class J>
bool disjoint(I& i, J& j);

/// Comapre two iterators with each other
enum CompareStatus {
  CS_SUBSET,    ///< First is subset of second iterator
  CS_DISJOINT,  ///< Intersection is empty
  CS_NONE       ///< Neither of the above
};

/// Check whether range iterator \a i is a subset of \a j, or whether they are disjoint
template <class I, class J>
CompareStatus compare(I& i, J& j);
//@}

template <class I>
inline IntVal cardinality(I& i) {
  IntVal s = 0;
  while (i()) {
    if (i.width().isFinite()) {
      s += i.width();
      ++i;
    } else {
      return IntVal::infinity();
    }
  }
  return s;
}

template <class I, class J>
inline bool equal(I& i, J& j) {
  // Are i and j equal?
  while (i() && j()) {
    if ((i.min() == j.min()) && (i.max() == j.max())) {
      ++i;
      ++j;
    } else {
      return false;
    }
  }
  return !i() && !j();
}

template <class I, class J>
inline bool subset(I& i, J& j) {
  // Is i subset of j?
  while (i() && j()) {
    if (j.max() < i.min()) {
      ++j;
    } else if ((i.min() >= j.min()) && (i.max() <= j.max())) {
      ++i;
    } else {
      return false;
    }
  }
  return !i();
}

template <class I, class J>
inline bool disjoint(I& i, J& j) {
  // Are i and j disjoint?
  while (i() && j()) {
    if (j.max() < i.min()) {
      ++j;
    } else if (i.max() < j.min()) {
      ++i;
    } else {
      return false;
    }
  }
  return true;
}

template <class I, class J>
inline CompareStatus compare(I& i, J& j) {
  bool subset = true;
  bool disjoint = true;
  while (i() && j()) {
    if (j.max() < i.min()) {
      ++j;
    } else if (i.max() < j.min()) {
      ++i;
      subset = false;
    } else if ((i.min() >= j.min()) && (i.max() <= j.max())) {
      ++i;
      disjoint = false;
    } else if (i.max() <= j.max()) {
      ++i;
      disjoint = false;
      subset = false;
    } else if (j.max() <= i.max()) {
      ++j;
      disjoint = false;
      subset = false;
    }
  }
  if (i()) {
    subset = false;
  }
  if (subset) {
    return CS_SUBSET;
  }
  return disjoint ? CS_DISJOINT : CS_NONE;
}

template <class I, class J>
inline bool less(I& i, J& j) {
  while (i()) {
    if (!j()) {
      return false;
    }
    if (i.min() < j.min()) {
      return true;
    }
    if (i.min() > j.min()) {
      return false;
    }
    if (i.max() < j.max()) {
      return true;
    }
    if (i.max() > j.max()) {
      ++j;
      return j();
    }
    ++i;
    ++j;
  }
  return static_cast<bool>(j());
}

template <class I, class J>
inline bool less_eq(I& i, J& j) {
  while (i()) {
    if (!j()) {
      return false;
    }
    if (i.min() < j.min()) {
      return true;
    }
    if (i.min() > j.min()) {
      return false;
    }
    if (i.max() < j.max()) {
      return true;
    }
    if (i.max() > j.max()) {
      ++j;
      return j();
    }
    ++i;
    ++j;
  }
  return true;
}

}  // namespace Ranges
}  // namespace MiniZinc
