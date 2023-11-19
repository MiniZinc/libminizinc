include "fzn_count_eq.mzn";
include "fzn_count_eq_par.mzn";
include "fzn_count_eq_reif.mzn";
include "fzn_count_eq_par_reif.mzn";

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count_eq(array[$X] of var $$E: x, var $$E: y, var int: c) =
  fzn_count_eq(array1d(x), y, c);

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count_eq(array[$X] of var opt $$E: x, var $$E: y, var int: c) = let {
    % Set <> to something not y
    int: def = assert(
      has_bounds(y),
      "Unable to decompose count_eq without bounds for the y argument",
      if 0 in dom(y) then lb(y)-1 else 0 endif
    );
  } in count_eq([i default def | i in x], y, c);

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count_eq(array[$X] of var $$E: x, $$E: y, int: c) =
  fzn_count_eq_par(array1d(x), y, c);

test count_eq(array[$X] of $$E: x, $$E: y, int: c) =
  c = count_eq(x, y);

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count_eq(array[$X] of var opt $$E: x, var $$E: y) ::promise_total =
  let { var 0..length(x): c ::is_defined_var; constraint count_eq(x, y, c) ::defines_var(c); } in c;

function int: count_eq(array[$X] of opt $$E: x, $$E: y) =
  sum(v in x where occurs(v))( v = y );

%-----------------------------------------------------------------------------%
