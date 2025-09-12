include "fzn_count_leq_par.mzn";
include "fzn_count_leq.mzn";
include "fzn_count_leq_par_reif.mzn";
include "fzn_count_leq_reif.mzn";

/** @group globals.counting
  Constrains \a c to be less than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_leq(array [$X] of var $$E: x, var $$E: y, var int: c) =
  fzn_count_leq(enum2int(array1d(x)), enum2int(y), c);

/** @group globals.counting
  Constrains \a c to be less than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_leq(array [$X] of var opt $$E: x, var $$E: y, var int: c) =
  let {
    % Set <> to something not y
    int: def = if 0 in enum2int(dom(y)) then enum2int(lb(y)) - 1 else 0 endif;
  } in count_leq([i default to_enum_internal(enum_of(y), def) | i in x], y, c);

/** @group globals.counting
  Constrains \a c to be less than or equal to the number of occurrences
  of \a y in \a x. 
*/
predicate count_leq(array [$X] of var $$E: x, $$E: y, int: c) =
  fzn_count_leq_par(enum2int(array1d(x)), enum2int(y), c);

%-----------------------------------------------------------------------------%
