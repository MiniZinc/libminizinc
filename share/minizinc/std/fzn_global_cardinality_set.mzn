predicate fzn_global_cardinality_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of var int: counts,
) = forall (i in index_set(cover)) (counts[i] = count (v in xs) (cover[i] in v));
