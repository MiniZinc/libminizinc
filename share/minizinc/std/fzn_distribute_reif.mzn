predicate fzn_distribute_reif(
  array [int] of var int: counts,
  array [int] of var int: cover,
  array [int] of var int: xs,
  var bool: b,
) =
   b <-> forall (i in index_set(counts)) (counts[i] == sum (j in index_set(xs)) (cover[i] = xs[j]));
