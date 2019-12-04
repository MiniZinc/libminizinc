include "fzn_count_neq_par.mzn";
include "fzn_count_neq.mzn";
include "count_fn.mzn";

/** @group globals.counting
  Constrains \a c to be not equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_neq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_neq(x,y,c);

/** @group globals.counting
  Constrains \a c to be not equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_neq(array[int] of var int: x, int: y, int: c) =
    fzn_count_neq_par(x,y,c);

predicate count_neq_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
    b <-> count(x,y) != c;

%-----------------------------------------------------------------------------%
