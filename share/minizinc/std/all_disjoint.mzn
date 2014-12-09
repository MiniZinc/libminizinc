/**  @group globals.alldifferent
  Constrain the array of sets of integers \a S to be pairwise disjoint.
*/
predicate all_disjoint(array[int] of var set of int: S) =
    forall(i,j in index_set(S) where i < j) ( disjoint(S[i], S[j]) );

include "disjoint.mzn";
