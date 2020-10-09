include "all_disjoint.mzn";

predicate fzn_partition_set(array[int] of var set of int: S,
                            set of int: universe) =
    all_disjoint(S) /\ universe == array_union(i in index_set(S)) ( S[i] );

