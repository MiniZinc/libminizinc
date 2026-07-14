include "count_fn.mzn";

predicate fzn_count_neq_float_reif(
  array [int] of var float: xs,
  var float: value,
  var int: n,
  var bool: b,
) = b <-> sum (x_i in xs) (x_i = value) != n;