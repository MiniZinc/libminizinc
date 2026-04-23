include "count_fn.mzn";

predicate fzn_count_neq_float_reif(
  array [int] of var float: x,
  var float: y,
  var int: c,
  var bool: b,
) = b <-> sum (x_i in x) (x_i = y) != c;