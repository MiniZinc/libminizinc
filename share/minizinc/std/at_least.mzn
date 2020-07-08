include "fzn_at_least_int.mzn";
include "fzn_at_least_int_reif.mzn";
include "fzn_at_least_set.mzn";
include "fzn_at_least_set_reif.mzn";

/**  @group globals.deprecated
  Requires at least \a n variables in \a x to take the value \a v.

  This constraint is deprecated. Use count(i in x)(i=v) >= n instead.
*/
predicate at_least(int: n, array[int] of var int: x, int: v) =
    fzn_at_least_int(n, x, v);

predicate at_least_reif(int: n, array[int] of var int: x, int: v, var bool: b) =
    fzn_at_least_int_reif(n, x, v, b);


/**  @group globals.counting
  Requires at least \a n variables in \a x to take the value \a v.
*/
predicate at_least(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_least_set(n, x, v);

predicate at_least_reif(int: n, array[int] of var set of int: x, set of int: v, var bool: b) =
    fzn_at_least_set_reif(n, x, v, b);

% Synonyms for the above.

predicate atleast(int: n, array[int] of var int: x, int: v) =
    at_least(n, x, v);

predicate atleast(int: n, array[int] of var set of int: x, set of int: v) =
    at_least(n, x, v);
