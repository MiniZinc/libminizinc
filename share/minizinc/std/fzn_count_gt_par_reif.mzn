include "fzn_count_gt_reif.mzn";

predicate fzn_count_gt_par_reif(array[int] of var int: x, int: y, int: c, var bool: b) =
  fzn_count_gt_reif(x,y,c,b);

%-----------------------------------------------------------------------------%
