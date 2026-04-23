include "fzn_count_gt_bool.mzn";

predicate fzn_count_gt_bool_par(array [int] of var bool: x, bool: y, int: c) =
  fzn_count_gt_bool(x, y, c);

%-----------------------------------------------------------------------------%
