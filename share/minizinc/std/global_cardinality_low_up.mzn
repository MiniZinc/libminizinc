include "global_cardinality.mzn";

/**  @group globals.deprecated
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lbound[\p i]
  and at most \a ubound[\p i] times in the array \a x.

  This constraint is deprecated. Use global_cardinality(x, cover, lbound, ubound) instead.
*/
predicate global_cardinality_low_up(array[$X] of var int: x,
                                    array[$Y] of int: cover,
                                    array[$Y] of int: lbound,
                                    array[$Y] of int: ubound) =
  global_cardinality(x, cover, lbound, ubound);
