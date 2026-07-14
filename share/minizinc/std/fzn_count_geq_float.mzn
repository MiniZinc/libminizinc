include "count_fn.mzn";

predicate fzn_count_geq_float(array [int] of var float: xs, var float: value, var int: n) =
  sum(x_i in xs) (x_i == value) <= n;
