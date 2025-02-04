include "fzn_all_equal_int.mzn";
include "fzn_all_equal_int_reif.mzn";
include "fzn_all_equal_set.mzn";
include "fzn_all_equal_set_reif.mzn";

/**  @group globals.alldifferent
  Constrain the elements of the array \a x to be all equal.
*/
predicate all_equal(array [$X] of var $$E: x) =
  if length(x) <= 1 then true else fzn_all_equal_int(enum2int(array1d(x))) endif;

/**  @group globals.alldifferent
  Constrain the elements of the array \a x to be all equal.
*/
predicate all_equal(array [$X] of var set of $$E: x) =
  if length(x) <= 1 then true else fzn_all_equal_set(enum2int(array1d(x))) endif;
