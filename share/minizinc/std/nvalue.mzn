/** @group globals.alldifferent
  Requires that the number of distinct values in \a x is \a n.
*/
predicate nvalue(var int: n, array[int] of var int: x) =
	fzn_nvalue(n, x);

include "fzn_nvalue.mzn";

/** @group globals.alldifferent
  Returns the number of distinct values in \a x.
*/
function var int: nvalue(array[int] of var int: x) =
  let { var 0..length(x): n; constraint nvalue(n,x); } in n;
