include "fzn_disjoint.mzn";

predicate fzn_all_disjoint(array [int] of var set of int: xs) =
  forall (i, j in index_set(xs) where i < j) (fzn_disjoint(xs[i], xs[j]));
