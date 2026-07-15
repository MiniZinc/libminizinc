predicate fzn_inverse_in_range_reif(
  array [int] of var int: xs,
  array [int] of var int: ys,
  var bool: b,
) = b <-> forall (i in index_set(xs), j in index_set(ys)) ((j == xs[i] <-> i == ys[j]));
