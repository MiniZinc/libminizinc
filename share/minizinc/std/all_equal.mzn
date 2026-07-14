include "fzn_all_equal_int.mzn";
include "fzn_all_equal_int_reif.mzn";
include "fzn_all_equal_set.mzn";
include "fzn_all_equal_set_reif.mzn";

predicate fzn_all_equal_int(array [int] of var int: xs);
predicate fzn_all_equal_set(array [int] of var set of int: xs);

/**  @group globals.alldifferent
  Constrain the elements of the array \a xs to be all equal.
*/
predicate all_equal(array [$X] of var $$E: xs) =
  if length(xs) <= 1 then true else fzn_all_equal_int(enum2int(array1d(xs))) endif;

/**  @group globals.alldifferent
  Constrain the elements of the array \a xs to be all equal.
*/
predicate all_equal(array [$X] of var set of $$E: xs) =
  if length(xs) <= 1 then true else fzn_all_equal_set(enum2int(array1d(xs))) endif;
