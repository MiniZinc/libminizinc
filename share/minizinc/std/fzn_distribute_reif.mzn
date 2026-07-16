predicate fzn_distribute_reif(
  array [int] of var int: count,
  array [int] of var int: cover,
  array [int] of var int: xs,
  var bool: b,
) =
   b <-> forall (i in index_set(count)) (count[i] == sum (j in index_set(xs)) (cover[i] = xs[j]));
