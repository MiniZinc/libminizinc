include "fzn_count_geq_par.mzn";
include "fzn_count_geq.mzn";
include "fzn_count_geq_par_reif.mzn";
include "fzn_count_geq_reif.mzn";

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var int: x, var int: y, var int: c) =
    fzn_count_geq(array1d(x),y,c);

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var int: x, int: y, int: c) =
    fzn_count_geq_par(array1d(x),y,c);

predicate count_geq_reif(array[$X] of var int: x, var int: y, var int: c, var bool: b) =
    fzn_count_geq_reif(array1d(x),y,c,b);
predicate count_geq_reif(array[$X] of var int: x, int: y, int: c, var bool: b) =
    fzn_count_geq_par_reif(array1d(x),y,c,b);

%-----------------------------------------------------------------------------%
