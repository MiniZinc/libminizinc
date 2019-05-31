/**  @group globals.alldifferent
  Constrain the array of sets of integers \a S to be pairwise disjoint.
*/
predicate all_disjoint(array[int] of var set of int: S) =
   fzn_all_disjoint(S);

include "fzn_all_disjoint.mzn";   
