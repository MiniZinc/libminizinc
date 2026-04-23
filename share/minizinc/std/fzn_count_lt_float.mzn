include "count_fn.mzn";

predicate fzn_count_lt_float(array [int] of var float: x, var float: y, var int: c) =
  sum (x_i in x) (x_i == y) > c;
