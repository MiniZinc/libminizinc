include "all_disjoint.mzn";

/** @group globals
  Constrains the sets in array \a S to partition the \a universe.
*/
predicate fzn_partition_set(array[int] of var set of int: S,
                            set of int: universe) =
    all_disjoint(S) /\ universe == array_union(i in index_set(S)) ( S[i] );

