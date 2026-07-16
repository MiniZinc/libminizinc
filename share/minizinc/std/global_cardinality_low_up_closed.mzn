include "global_cardinality_closed.mzn";

/**  @group globals.deprecated
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lower_bound[\p i]
  and at most \a upper_bound[\p i] times in the array \a x.

  The elements of \a x must take their values from \a cover.

  This constraint is deprecated. Use global_cardinality_closed(x, cover, lower_bound, upper_bound) instead.
*/
predicate global_cardinality_low_up_closed(
  array [$X] of var int: x,
  array [$Y] of int: cover,
  array [$Y] of int: lower_bound,
  array [$Y] of int: upper_bound,
) = global_cardinality_closed(x, cover, lower_bound, upper_bound);
