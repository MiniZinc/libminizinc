include "fzn_count_gt_bool.mzn";

predicate fzn_count_gt_bool_par(array [int] of var bool: xs, bool: value, int: n) =
  fzn_count_gt_bool(xs, value, n);

%-----------------------------------------------------------------------------%
