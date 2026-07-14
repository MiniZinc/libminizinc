include "fzn_count_gt.mzn";

predicate fzn_count_gt_par(array [int] of var int: xs, int: value, int: n) = fzn_count_gt(xs, value, n);

%-----------------------------------------------------------------------------%
