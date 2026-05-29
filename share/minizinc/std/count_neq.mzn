include "fzn_count_neq.mzn";
include "fzn_count_neq_reif.mzn";
include "fzn_count_neq_bool.mzn";
include "fzn_count_neq_bool_reif.mzn";
include "fzn_count_neq_float.mzn";
include "fzn_count_neq_float_reif.mzn";
include "fzn_count_neq_set.mzn";
include "fzn_count_neq_set_reif.mzn";
include "fzn_count_neq_par.mzn";
include "fzn_count_neq_par_reif.mzn";
include "fzn_count_neq_bool_par.mzn";
include "fzn_count_neq_bool_par_reif.mzn";
include "fzn_count_neq_float_par.mzn";
include "fzn_count_neq_float_par_reif.mzn";
include "fzn_count_neq_set_par.mzn";
include "fzn_count_neq_set_par_reif.mzn";

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var $$E: xs, var $$E: value, var int: n) =
  fzn_count_neq(enum2int(array1d(xs)), enum2int(value), n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var bool: xs, var bool: value, var int: n) =
  fzn_count_neq_bool(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var float: xs, var float: value, var int: n) =
  fzn_count_neq_float(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var set of $$E: xs, var set of $$E: value, var int: n) =
  fzn_count_neq_set(enum2int(array1d(xs)), enum2int(value), n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var opt $$E: xs, var $$E: value, var int: n) =
  let {
    % Set <> to something not value
    int: def = if 0 in enum2int(dom(value)) then enum2int(lb(value)) - 1 else 0 endif;
  } in count_neq([i default to_enum_internal(enum_of(value), def) | i in xs], value, n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var $$E: xs, $$E: value, int: n) =
  fzn_count_neq_par(enum2int(array1d(xs)), enum2int(value), n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var bool: xs, bool: value, int: n) =
  fzn_count_neq_bool_par(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var float: xs, float: value, int: n) =
  fzn_count_neq_float_par(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be not equal to the number of occurrences
  of \a value in \a xs.
*/
predicate count_neq(array [$X] of var set of $$E: xs, set of $$E: value, int: n) =
  fzn_count_neq_set_par(enum2int(array1d(xs)), enum2int(value), n);

%-----------------------------------------------------------------------------%
