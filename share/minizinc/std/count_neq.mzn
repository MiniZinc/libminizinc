include "fzn_count_neq_par.mzn";
include "fzn_count_neq.mzn";
include "fzn_count_neq_par_reif.mzn";
include "fzn_count_neq_reif.mzn";

/** @group globals.counting
  Constrains \a c to be not equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_neq(array[$X] of var $$E: x, var $$E: y, var int: c) =
    fzn_count_neq(array1d(x),y,c);

/** @group globals.counting
  Constrains \a c to be not equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_neq(array[$X] of var $$E: x, $$E: y, int: c) =
    fzn_count_neq_par(array1d(x),y,c);

%-----------------------------------------------------------------------------%
