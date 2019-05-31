include "fzn_at_least_int.mzn";
include "fzn_at_least_set.mzn";

/**  @group globals.counting
  Requires at least \a n variables in \a x to take the value \a v.
*/
predicate at_least(int: n, array[int] of var int: x, int: v) =
    fzn_at_least_int(n, x, v);

/**  @group globals.counting
  Requires at least \a n variables in \a x to take the value \a v.
*/
predicate at_least(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_least_set(n, x, v);

% Synonyms for the above.

predicate atleast(int: n, array[int] of var int: x, int: v) =
    fzn_at_least_int(n, x, v);

predicate atleast(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_least_set(n, x, v);
