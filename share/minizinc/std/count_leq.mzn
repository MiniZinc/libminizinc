include "fzn_count_leq_par.mzn";
include "fzn_count_leq.mzn";
include "count_fn.mzn";

/** @group globals.counting
  Constrains \a c to be less than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_leq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_leq(x,y,c);

/** @group globals.counting
  Constrains \a c to be less than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_leq(array[int] of var int: x, int: y, int: c) =
    fzn_count_leq_par(x,y,c);

predicate count_leq_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
    b <-> count(x,y) >= c;

%-----------------------------------------------------------------------------%
