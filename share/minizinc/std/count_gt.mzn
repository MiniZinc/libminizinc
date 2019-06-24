include "fzn_count_gt.mzn";
include "count_fn.mzn";

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_gt(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_gt(x,y,c);

predicate count_gt_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
    b <-> count(x,y) < c;

%-----------------------------------------------------------------------------%
