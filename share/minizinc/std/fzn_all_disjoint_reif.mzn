include "fzn_disjoint.mzn";

predicate fzn_all_disjoint_reif(array[int] of var set of int: S, var bool: b) =
    b <-> forall(i,j in index_set(S) where i < j) ( fzn_disjoint(S[i], S[j]) );
