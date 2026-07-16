include "count.mzn";

predicate fzn_distribute(
  array [int] of var int: count,
  array [int] of var int: cover,
  array [int] of var int: xs,
) = forall (i in index_set(count)) (count(xs, cover[i], count[i]));
