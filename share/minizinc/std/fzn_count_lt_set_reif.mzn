include "count_fn.mzn";

predicate fzn_count_lt_set_reif(
  array [int] of var set of int: xs,
  var set of int: value,
  var int: n,
  var bool: b,
) = b <-> sum (x_i in xs) (x_i == value) > n;
