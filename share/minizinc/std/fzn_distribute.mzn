include "count.mzn";

predicate fzn_distribute(
  array [int] of var int: counts,
  array [int] of var int: cover,
  array [int] of var int: xs,
) = forall (i in index_set(counts)) (count(xs, cover[i], counts[i]));
