include "global_cardinality_closed.mzn";

/**  @group globals.deprecated
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.

  The elements of \a x must take their values from \a cover.

  This constraint is deprecated. Use global_cardinality_closed(x, cover, lbound, ubound) instead.
*/
predicate global_cardinality_low_up_closed(array[$X] of var int: x,
                                           array[$Y] of int: cover,
                                           array[$Y] of int: lbound,
                                           array[$Y] of int: ubound) =
  global_cardinality_closed(x, cover, lbound, ubound);
