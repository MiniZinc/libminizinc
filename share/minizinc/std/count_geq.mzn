include "fzn_count_geq_par.mzn";
include "fzn_count_geq.mzn";
include "fzn_count_geq_par_reif.mzn";
include "fzn_count_geq_reif.mzn";

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var $$E: x, var $$E: y, var int: c) =
  fzn_count_geq(array1d(x), y, c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var opt $$E: x, var $$E: y, var int: c) =let {
    % Set <> to something not y
    int: def = if 0 in dom(y) then lb(y)-1 else 0 endif;
  } in count_geq([i default def | i in x], y, c);

/** @group globals.counting
  Constrains \a c to be greater than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_geq(array[$X] of var $$E: x, $$E: y, int: c) =
  fzn_count_geq_par(array1d(x), y, c);

%-----------------------------------------------------------------------------%
