/**  @group globals.alldifferent
  Constrain the array of sets of integers \a S to be pairwise disjoint.
*/
predicate fzn_all_disjoint(array[int] of var set of int: S) =
    forall(i,j in index_set(S) where i < j) ( fzn_disjoint(S[i], S[j]) );

include "fzn_disjoint.mzn";
