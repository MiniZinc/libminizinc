include "global_cardinality.mzn";

/**  @group globals.deprecated
  Requires that for all \p i, the value \a cover[\p i] appears at least \a lower_bound[\p i]
  and at most \a upper_bound[\p i] times in the array \a x.

  This constraint is deprecated. Use global_cardinality(x, cover, lower_bound, upper_bound) instead.
*/
predicate global_cardinality_low_up(
  array [$X] of var int: x,
  array [$Y] of int: cover,
  array [$Y] of int: lower_bound,
  array [$Y] of int: upper_bound,
) = global_cardinality(x, cover, lower_bound, upper_bound);
