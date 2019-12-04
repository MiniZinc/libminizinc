include "at_most_int.mzn";
include "at_most_set.mzn";

/**  @group globals.deprecated
  Requires at most \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) <= n instead.
*/
predicate at_most(int: n, array[int] of var int: x, int: v) =
    at_most_int(n, x, v);

/**  @group globals.counting
  Requires at most \a n variables in \a x to take the value \a v.
*/
predicate at_most(int: n, array[int] of var set of int: x, set of int: v) =
    at_most_set(n, x, v);

% Synonyms for the above.

predicate atmost(int: n, array[int] of var int: x, int: v) =
    at_most_int(n, x, v);

predicate atmost(int: n, array[int] of var set of int: x, set of int: v) =
    at_most_set(n, x, v);
