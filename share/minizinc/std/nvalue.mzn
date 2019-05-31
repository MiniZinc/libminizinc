include "fzn_nvalue.mzn";

/** @group globals.alldifferent
  Requires that the number of distinct values in \a x is \a n.
*/
predicate nvalue(var int: n, array[int] of var int: x) =
	fzn_nvalue(n, x);
