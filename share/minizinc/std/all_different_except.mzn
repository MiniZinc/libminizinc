include "all_different.mzn";

/**  @group globals.alldifferent
  Constrain the elements of the array \a xs to be pairwise different except for those
  values that appear in the set \a except.

  This constraint is deprecated. Use all_different(\a xs, \a except) instead.
*/
predicate all_different_except(array [$X] of var $$E: xs, set of $$E: except) =
  all_different(xs, except);

% Synonym for the above.
predicate alldifferent_except(array [$X] of var $$E: xs, set of $$E: except) =
  all_different(xs, except);
