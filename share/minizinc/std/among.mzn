include "among_fn.mzn";

include "fzn_among.mzn";
include "fzn_among_reif.mzn";

/**  @group globals.counting
  Requires exactly \a n variables in \a xs to take one of the values in \a values.
*/
predicate among(var int: n, array [$X] of var $$E: xs, set of $$E: values) =
  fzn_among(n, enum2int(array1d(xs)), enum2int(values));
