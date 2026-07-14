include "fzn_all_disjoint.mzn";
include "fzn_all_disjoint_reif.mzn";

predicate fzn_all_disjoint(array [int] of var set of int: xs);

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a xs to be pairwise disjoint.
*/
predicate all_disjoint(array [$X] of var set of $$E: xs) = fzn_all_disjoint(enum2int(array1d(xs)));
