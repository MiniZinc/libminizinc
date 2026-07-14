predicate fzn_at_most1(array [int] of var set of int: xs) =
  forall (i, j in index_set(xs) where i < j) (card(xs[i] intersect xs[j]) <= 1);
