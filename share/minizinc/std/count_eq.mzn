include "fzn_count_eq.mzn";
include "fzn_count_eq_reif.mzn";
include "fzn_count_eq_bool.mzn";
include "fzn_count_eq_bool_reif.mzn";
include "fzn_count_eq_float.mzn";
include "fzn_count_eq_float_reif.mzn";
include "fzn_count_eq_set.mzn";
include "fzn_count_eq_set_reif.mzn";
include "fzn_count_eq_par.mzn";
include "fzn_count_eq_par_reif.mzn";
include "fzn_count_eq_bool_par.mzn";
include "fzn_count_eq_bool_par_reif.mzn";
include "fzn_count_eq_float_par.mzn";
include "fzn_count_eq_float_par_reif.mzn";
include "fzn_count_eq_set_par.mzn";
include "fzn_count_eq_set_par_reif.mzn";

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs. 
*/
predicate count_eq(array [$X] of var $$E: xs, var $$E: value, var int: n) =
  fzn_count_eq(enum2int(array1d(xs)), enum2int(value), n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var bool: xs, var bool: value, var int: n) =
  fzn_count_eq_bool(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var float: xs, var float: value, var int: n) =
  fzn_count_eq_float(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var set of $$E: xs, var set of $$E: value, var int: n) =
  fzn_count_eq_set(enum2int(array1d(xs)), enum2int(value), n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs. 
*/
predicate count_eq(array [$X] of var opt $$E: xs, var $$E: value, var int: n) =
  let {
    % Set <> to something not value
    int: def =
      assert(
        has_bounds(value),
        "Unable to decompose count_eq without bounds for the value argument",
        if 0 in dom(enum2int(value)) then enum2int(lb(value)) - 1 else 0 endif,
      );
  } in count_eq([i default to_enum_internal(enum_of(value), def) | i in xs], value, n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs. 
*/
predicate count_eq(array [$X] of var opt bool: xs, var bool: value, var int: n) =
  count_eq([i default 2 | i in xs], value, n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var $$E: xs, $$E: value, int: n) =
  fzn_count_eq_par(enum2int(array1d(xs)), enum2int(value), n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var bool: xs, bool: value, int: n) =
  fzn_count_eq_bool_par(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var float: xs, float: value, int: n) =
  fzn_count_eq_float_par(array1d(xs), value, n);

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count_eq(array [$X] of var set of $$E: xs, set of $$E: value, int: n) =
  fzn_count_eq_set_par(enum2int(array1d(xs)), enum2int(value), n);

test count_eq(array [$X] of $$E: xs, $$E: value, int: n) = n = count_eq(xs, value);

/** @group globals.counting
  Returns the number of occurrences of \a value in \a xs. 
*/
function var int: count_eq(array [$X] of var opt $$E: xs, var $$E: value) :: promise_total =
  let {
    var 0..length(xs): n :: is_defined_var;
    constraint count_eq(xs, value, n) :: defines_var(n);
  } in n;

function int: count_eq(array [$X] of opt $$E: xs, $$E: value) = sum (v in xs where occurs(v)) (v = value);

%-----------------------------------------------------------------------------%
