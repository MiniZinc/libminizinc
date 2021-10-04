include "fzn_at_most_int.mzn";
include "fzn_at_most_int_reif.mzn";
include "fzn_at_most_set.mzn";
include "fzn_at_most_set_reif.mzn";

/**  @group globals.deprecated
  Requires at most \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) <= n instead.
*/
predicate at_most(int: n, array[$X] of var $$E: x, $$E: v) =
	fzn_at_most_int(n, array1d(x), v);

/**  @group globals.counting
  Requires at most \a n variables in \a x to take the value \a v.
*/
predicate at_most(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
	fzn_at_most_set(n, array1d(x), v);

% Synonyms for the above.

predicate atmost(int: n, array[$X] of var $$E: x, $$E: v) =
    at_most(n, x, v);

predicate atmost(int: n, array[$X] of var set of $$E: x, set of $$E: v) =
    at_most(n, x, v);
