predicate fzn_inverse_in_range_reif(
  array [int] of var int: xs,
  array [int] of var int: ys,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(xs)) (xs[i] in index_set(ys) -> (ys[xs[i]] == i)) /\
        forall (j in index_set(ys)) (ys[j] in index_set(xs) -> (xs[ys[j]] == j))
    );
