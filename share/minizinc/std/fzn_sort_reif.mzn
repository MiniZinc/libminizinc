include "sort_fn.mzn";

predicate fzn_sort_reif(array [int] of var int: xs, array [int] of var int: ys, var bool: b) =
  let { array [int] of var int: s = sort(xs) } in b <-> s = ys;
