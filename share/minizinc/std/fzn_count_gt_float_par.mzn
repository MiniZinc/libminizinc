include "fzn_count_gt_float.mzn";

predicate fzn_count_gt_float_par(array [int] of var float: x, float: y, int: c) =
  fzn_count_gt_float(x, y, c);

%-----------------------------------------------------------------------------%
