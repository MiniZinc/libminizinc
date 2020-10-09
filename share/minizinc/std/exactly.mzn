include "exactly_int.mzn";
include "exactly_set.mzn";

/** @group globals.deprecated
  Requires exactly \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) = n instead.
*/
predicate exactly(int: n, array[int] of var int: x, int: v) =
    exactly_int(n, x, v);

/** @group globals.counting
  Requires exactly \a n variables in \a x to take the value \a v.
*/
predicate exactly(int: n, array[int] of var set of int: x, set of int: v) =
    exactly_set(n, x, v);

