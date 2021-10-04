include "fzn_count_geq_par.mzn";
include "fzn_count_geq.mzn";
include "fzn_count_geq_par_reif.mzn";
include "fzn_count_geq_reif.mzn";

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var $$E: x, var $$E: y, var int: c) =
    fzn_count_geq(array1d(x),y,c);

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var $$E: x, $$E: y, int: c) =
    fzn_count_geq_par(array1d(x),y,c);

%-----------------------------------------------------------------------------%
