include "fzn_at_least_int.mzn";
include "fzn_at_least_int_reif.mzn";
include "fzn_at_least_set.mzn";
include "fzn_at_least_set_reif.mzn";

/**  @group globals.deprecated
  Requires at least \a n variables in \a xs to take the value \a value.

  This constraint is deprecated. Use count(i in xs)(i=value) >= n instead.
*/
predicate at_least(int: n, array [$X] of var $$E: xs, $$E: value) =
  fzn_at_least_int(n, enum2int(array1d(xs)), enum2int(value));

/**  @group globals.counting
  Requires at least \a n variables in \a xs to take the value \a value.
*/
predicate at_least(int: n, array [$X] of var set of $$E: xs, set of $$E: value) =
  fzn_at_least_set(n, enum2int(array1d(xs)), enum2int(value));

% Synonyms for the above.

predicate atleast(int: n, array [$X] of var $$E: xs, $$E: value) = at_least(n, xs, value);

predicate atleast(int: n, array [$X] of var set of $$E: xs, set of $$E: value) = at_least(n, xs, value);
