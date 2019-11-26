include "nvalue.mzn";

/** @group globals.alldifferent
  Returns the number of distinct values in \a x.
*/
function var int: nvalue(array[int] of var int: x) =
  let { var 0..length(x): n::is_defined_var; constraint nvalue(n,x)::defines_var(n); } in n;
