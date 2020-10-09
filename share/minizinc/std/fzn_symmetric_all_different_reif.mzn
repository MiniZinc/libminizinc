include "all_different.mzn";

predicate fzn_symmetric_all_different_reif(array[int] of var int:x, var bool: b) =
    b <-> ( all_different(x)
            /\  forall(i, j in index_set(x) where i!=j) (x[i] = j -> x[j] = i) );
