include "fzn_all_equal_int.mzn";
include "fzn_all_equal_set.mzn";

/**  @group globals.alldifferent
  Constrain the array of integers \a x to be all equal
*/
predicate all_equal(array[int] of var int: x) =
	fzn_all_equal_int(x);

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a x to be all different
*/
predicate all_equal(array[int] of var set of int: x) =
	fzn_all_equal_set(x);
