include "fzn_count_gt_par.mzn";
include "fzn_count_gt.mzn";
include "fzn_count_gt_par_reif.mzn";
include "fzn_count_gt_reif.mzn";

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_gt(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_gt(x,y,c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_gt(array[int] of var int: x, int: y, int: c) =
    fzn_count_gt_par(x,y,c);

predicate count_gt_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
    fzn_count_gt_reif(x,y,c,b);
predicate count_gt_reif(array[int] of var int: x, int: y, int: c, var bool: b) =
    fzn_count_gt_par_reif(x,y,c,b);

%-----------------------------------------------------------------------------%
