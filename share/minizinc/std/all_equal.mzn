include "fzn_all_equal_int.mzn";
include "fzn_all_equal_int_reif.mzn";
include "fzn_all_equal_set.mzn";
include "fzn_all_equal_set_reif.mzn";

/**  @group globals.alldifferent
  Constrain the elements of the array \a x to be all equal.
*/
predicate all_equal(array[$X] of var int: x) =
  if length(x) <= 1 then true else fzn_all_equal_int(array1d(x)) endif;

/**  @group globals.alldifferent
  Constrain the elements of the array \a x to be all equal.
*/
predicate all_equal(array[$X] of var set of int: x) =
  if length(x) <= 1 then true else fzn_all_equal_set(array1d(x)) endif;
