include "fzn_exactly_int.mzn";
include "fzn_exactly_int_reif.mzn";
include "fzn_exactly_set.mzn";
include "fzn_exactly_set_reif.mzn";

/** @group globals.deprecated
  Requires exactly \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) = n instead.
*/
predicate exactly(int: n, array[$X] of var $$E: x, $$E: v) =
	fzn_exactly_int(n, array1d(x), v);

/** @group globals.counting
  Requires exactly \a n variables in \a x to take the value \a v.
*/
predicate exactly(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
	fzn_exactly_set(n, array1d(x), v);

