include "fzn_global_cardinality_low_up_closed.mzn";
include "fzn_global_cardinality_low_up_closed_reif.mzn";

/** @group globals.counting
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.

  The elements of \a x must take their values from \a cover.
*/
predicate global_cardinality_low_up_closed(array[int] of var int: x,
                                           array[int] of int: cover,
                                           array[int] of int: lbound,
                                           array[int] of int: ubound) =
    fzn_global_cardinality_low_up_closed(x, cover, lbound, ubound);
