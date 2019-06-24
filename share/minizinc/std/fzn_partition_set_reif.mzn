include "all_disjoint.mzn";

predicate fzn_partition_set_reif(array[int] of var set of int: S,
                                 set of int: universe,
                                 var bool: b) =
    b <-> ( all_disjoint(S) /\ universe == array_union(i in index_set(S)) ( S[i] ) );
