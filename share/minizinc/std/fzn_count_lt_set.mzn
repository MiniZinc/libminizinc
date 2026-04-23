include "count_fn.mzn";

predicate fzn_count_lt_set(array [int] of var set of int: x, var set of int: y, var int: c) =
  sum (x_i in x) (x_i == y) > c;
