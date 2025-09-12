include "fzn_all_disjoint.mzn";
include "fzn_all_disjoint_reif.mzn";

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a S to be pairwise disjoint.
*/
predicate all_disjoint(array [$X] of var set of $$E: S) = fzn_all_disjoint(enum2int(array1d(S)));
