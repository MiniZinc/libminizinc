include "fzn_count_geq.mzn";
include "count_fn.mzn";

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_geq(x,y,c);

predicate count_geq_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
    b <-> count(x,y) <= c;

%-----------------------------------------------------------------------------%
