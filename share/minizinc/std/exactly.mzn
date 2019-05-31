include "fzn_exactly_int.mzn";
include "fzn_exactly_set.mzn";

/** @group globals.counting
  Requires exactly \a n variables in \a x to take the value \a v.
*/
predicate exactly(int: n, array[int] of var int: x, int: v) =
    fzn_exactly_int(n, x, v);

/** @group globals.counting
  Requires exactly \a n variables in \a x to take the value \a v.
*/
predicate exactly(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_exactly_set(n, x, v);

