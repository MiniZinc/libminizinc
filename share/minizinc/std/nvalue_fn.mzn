include "nvalue.mzn";

/** @group globals.alldifferent
  Returns the number of distinct values in \a x.
*/
function var int: nvalue(array[int] of var int: x) =
  let { var 0..length(x): n; constraint nvalue(n,x); } in n;
