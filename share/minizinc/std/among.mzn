include "among_fn.mzn";

include "fzn_among.mzn";
include "fzn_among_reif.mzn";

/**  @group globals.counting
  Requires exactly \a n variables in \a x to take one of the values in \a v.
*/
predicate among(var int: n, array[$X] of var $$E: x, set of $$E: v) =
    fzn_among(n, array1d(x), v);
