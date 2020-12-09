include "fzn_count_gt.mzn";

predicate fzn_count_gt_par(array[int] of var int: x, int: y, int: c) =
  fzn_count_gt(x,y,c);

%-----------------------------------------------------------------------------%
