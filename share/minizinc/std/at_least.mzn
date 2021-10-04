include "fzn_at_least_int.mzn";
include "fzn_at_least_int_reif.mzn";
include "fzn_at_least_set.mzn";
include "fzn_at_least_set_reif.mzn";

/**  @group globals.deprecated
  Requires at least \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) >= n instead.
*/
predicate at_least(int: n, array[$X] of var $$E: x, $$E: v) =
	fzn_at_least_int(n, array1d(x), v);

/**  @group globals.counting
  Requires at least \a n variables in \a x to take the value \a v.
*/
predicate at_least(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
	fzn_at_least_set(n, array1d(x), v);

% Synonyms for the above.

predicate atleast(int: n, array[$X] of var $$E: x, $$E: v) =
  at_least(n, x, v);

predicate atleast(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
  at_least(n, x, v);
