include "fzn_exactly_int.mzn";
include "fzn_exactly_int_reif.mzn";
include "fzn_exactly_set.mzn";
include "fzn_exactly_set_reif.mzn";

/** @group globals.deprecated
  Requires exactly \a n variables in \a xs to take the value \a value.

  This constraint is deprecated. Use count(i in xs)(i=value) = n instead.
*/
predicate exactly(int: n, array [$X] of var $$E: xs, $$E: value) =
  fzn_exactly_int(n, enum2int(array1d(xs)), enum2int(value));

/** @group globals.counting
  Requires exactly \a n variables in \a xs to take the value \a value.
*/
predicate exactly(int: n, array [$X] of var set of $$E: xs, set of $$E: value) =
  fzn_exactly_set(n, enum2int(array1d(xs)), enum2int(value));
