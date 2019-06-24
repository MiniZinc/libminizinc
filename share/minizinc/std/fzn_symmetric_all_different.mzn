include "all_different.mzn";

predicate fzn_symmetric_all_different(array[int] of var int:x) =
    all_different(x)
    /\  forall(i, j in index_set(x) where i!=j) (x[i] = j -> x[j] = i);
