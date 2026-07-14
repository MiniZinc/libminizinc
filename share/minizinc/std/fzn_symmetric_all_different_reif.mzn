include "all_different.mzn";

predicate fzn_symmetric_all_different_reif(array [int] of var int: xs, var bool: b) =
  b <-> (all_different(xs) /\ forall (i, j in index_set(xs) where i != j) (xs[i] = j -> xs[j] = i));
