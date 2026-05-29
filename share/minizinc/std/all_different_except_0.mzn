include "all_different.mzn";

/**  @group globals.alldifferent
  Constrain the elements of the array \a xs to be pairwise different except for those
  elements that are assigned the value 0.

  This constraint is deprecated. Use all_different(\a xs, {0}) instead.
*/
predicate all_different_except_0(array [$X] of var int: xs) =
  all_different(xs, {0});

% Synonym for the above.
predicate alldifferent_except_0(array [$X] of var int: xs) = all_different(xs, {0});
