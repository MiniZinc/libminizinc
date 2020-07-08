include "fzn_at_most_int.mzn";
include "fzn_at_most_int_reif.mzn";
include "fzn_at_most_set.mzn";
include "fzn_at_most_set_reif.mzn";

/**  @group globals.deprecated
  Requires at most \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) <= n instead.
*/
predicate at_most(int: n, array[int] of var int: x, int: v) =
    fzn_at_most_int(n, x, v);

predicate at_most_reif(int: n, array[int] of var int: x, int: v, var bool: b) =
    fzn_at_most_int_reif(n, x, v, b);

/**  @group globals.counting
  Requires at most \a n variables in \a x to take the value \a v.
*/
predicate at_most(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_most_set(n, x, v);
predicate at_most_reif(int: n, array[int] of var set of int: x, set of int: v, var bool: b) =
    fzn_at_most_set_reif(n, x, v,b);

% Synonyms for the above.

predicate atmost(int: n, array[int] of var int: x, int: v) =
    at_most(n, x, v);

predicate atmost(int: n, array[int] of var set of int: x, set of int: v) =
    at_most(n, x, v);
