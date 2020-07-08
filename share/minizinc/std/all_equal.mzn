include "fzn_all_equal_int.mzn";
include "fzn_all_equal_int_reif.mzn";
include "fzn_all_equal_set.mzn";
include "fzn_all_equal_set_reif.mzn";

/**  @group globals.alldifferent
  Constrain the array of integers \a x to be all equal
*/
predicate all_equal(array[$X] of var int: x) =
  if length(x) <= 1 then true else fzn_all_equal_int(array1d(x)) endif;

predicate all_equal_reif(array[$X] of var int: x, var bool: b) =
  if length(x) <= 1 then b = true else fzn_all_equal_int_reif(array1d(x),b) endif;

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a x to be all equal
*/
predicate all_equal(array[$X] of var set of int: x) =
  if length(x) <= 1 then true else fzn_all_equal_set(array1d(x)) endif;

predicate all_equal_reif(array[$X] of var set of int: x, var bool: b) =
  if length(x) <= 1 then b = true else fzn_all_equal_set_reif(array1d(x),b) endif;
