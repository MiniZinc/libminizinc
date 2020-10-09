include "fzn_count_lt.mzn";
include "fzn_count_lt_par.mzn";
include "fzn_count_lt_reif.mzn";
include "fzn_count_lt_par_reif.mzn";

/** @group globals.counting
  Constrains \a c to be strictly less than the number of occurrences
  of \a y in \a x. 
*/
predicate count_lt(array[$X] of var int: x, var int: y, var int: c) =
    fzn_count_lt(array1d(x),y,c);

/** @group globals.counting
  Constrains \a c to be strictly less than the number of occurrences
  of \a y in \a x. 
*/
predicate count_lt(array[$X] of var int: x, int: y, int: c) =
    fzn_count_lt_par(array1d(x),y,c);

predicate count_lt_reif(array[$X] of var int: x, var int: y, var int: c, var bool: b) =
    fzn_count_lt_reif(array1d(x),y,c,b);
predicate count_lt_reif(array[$X] of var int: x, int: y, int: c, var bool: b) =
    fzn_count_lt_par_reif(array1d(x),y,c,b);

%-----------------------------------------------------------------------------%
