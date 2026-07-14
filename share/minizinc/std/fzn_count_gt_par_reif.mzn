include "fzn_count_gt_reif.mzn";

predicate fzn_count_gt_par_reif(array [int] of var int: xs, int: value, int: n, var bool: b) =
  fzn_count_gt_reif(xs, value, n, b);

%-----------------------------------------------------------------------------%
