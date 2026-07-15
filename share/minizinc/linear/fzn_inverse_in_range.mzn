predicate fzn_inverse_in_range(array [int] of var int: xs, array [int] of var int: ys) =
  forall (i in index_set(xs), j in index_set(ys)) ((j == xs[i] <-> i == ys[j]));
