include "count_fn.mzn";

predicate fzn_count_geq_set(array [int] of var set of int: xs, var set of int: value, var int: n) =
  sum (x_i in xs) (x_i == value) <= n;
