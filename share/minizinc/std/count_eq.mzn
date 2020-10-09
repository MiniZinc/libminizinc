include "fzn_count_eq.mzn";
include "fzn_count_eq_par.mzn";
include "fzn_count_eq_reif.mzn";
include "fzn_count_eq_par_reif.mzn";

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count_eq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_eq(x,y,c);

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count_eq(array[int] of var int: x, int: y, int: c) =
    fzn_count_eq_par(x,y,c);

predicate count_eq(array[int] of int: x, int: y, int: c) =
    c=count_eq(x,y);

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count_eq(array[int] of var int: x, var int: y) ::promise_total =
  let { var 0..length(x): c; constraint fzn_count_eq(x,y,c); } in c;

function int: count_eq(array[int] of int: x, int: y) =
              sum(i in index_set(x))(x[i] = y);

predicate count_eq_reif(array[int] of var int: x, var int: y, var int: c, var bool: b) =
  fzn_count_eq_reif(x, y, c, b);

predicate count_eq_reif(array[int] of var int: x, int: y, int: c, var bool: b) =
  fzn_count_eq_par_reif(x, y, c, b);

%-----------------------------------------------------------------------------%
