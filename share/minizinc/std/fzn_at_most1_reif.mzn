predicate fzn_at_most1_reif(array [int] of var set of int: xs, var bool: b) =
  b <-> forall (i, j in index_set(xs) where i < j) (card(xs[i] intersect xs[j]) <= 1);
