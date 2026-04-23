include "fzn_count_gt.mzn";
include "fzn_count_gt_reif.mzn";
include "fzn_count_gt_bool.mzn";
include "fzn_count_gt_bool_reif.mzn";
include "fzn_count_gt_float.mzn";
include "fzn_count_gt_float_reif.mzn";
include "fzn_count_gt_set.mzn";
include "fzn_count_gt_set_reif.mzn";
include "fzn_count_gt_par.mzn";
include "fzn_count_gt_par_reif.mzn";
include "fzn_count_gt_bool_par.mzn";
include "fzn_count_gt_bool_par_reif.mzn";
include "fzn_count_gt_float_par.mzn";
include "fzn_count_gt_float_par_reif.mzn";
include "fzn_count_gt_set_par.mzn";
include "fzn_count_gt_set_par_reif.mzn";

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_gt(array [$X] of var $$E: x, var $$E: y, var int: c) =
  fzn_count_gt(enum2int(array1d(x)), enum2int(y), c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x.
*/
predicate count_gt(array [$X] of var bool: x, var bool: y, var int: c) =
  fzn_count_gt_bool(array1d(x), y, c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x.
*/
predicate count_gt(array [$X] of var float: x, var float: y, var int: c) =
  fzn_count_gt_float(array1d(x), y, c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x.
*/
predicate count_gt(array [$X] of var set of $$E: x, var set of $$E: y, var int: c) =
  fzn_count_gt_set(enum2int(array1d(x)), enum2int(y), c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_gt(array [$X] of var opt $$E: x, var $$E: y, var int: c) =
  let {
    % Set <> to something not y
    int: def = if 0 in enum2int(dom(y)) then enum2int(lb(y)) - 1 else 0 endif;
  } in count_gt([i default to_enum_internal(enum_of(y), def) | i in x], y, c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x. 
*/
predicate count_gt(array [$X] of var $$E: x, $$E: y, int: c) =
  fzn_count_gt_par(enum2int(array1d(x)), enum2int(y), c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x.
*/
predicate count_gt(array [$X] of var bool: x, bool: y, int: c) =
  fzn_count_gt_bool_par(array1d(x), y, c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x.
*/
predicate count_gt(array [$X] of var float: x, float: y, int: c) =
  fzn_count_gt_float_par(array1d(x), y, c);

/** @group globals.counting
  Constrains \a c to be strictly greater than the number of occurrences
  of \a y in \a x.
*/
predicate count_gt(array [$X] of var set of $$E: x, set of $$E: y, int: c) =
  fzn_count_gt_set_par(enum2int(array1d(x)), enum2int(y), c);

%-----------------------------------------------------------------------------%
