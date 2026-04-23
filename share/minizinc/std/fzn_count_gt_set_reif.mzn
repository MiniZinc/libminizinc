include "count_fn.mzn";

predicate fzn_count_gt_set_reif(
  array [int] of var set of int: x,
  var set of int: y,
  var int: c,
  var bool: b,
) = b <-> sum (x_i in x) (x_i == y) < c;