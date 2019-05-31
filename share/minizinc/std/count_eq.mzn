/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count_eq(array[int] of var int: x, var int: y, var int: c) =
    fzn_count_eq(x,y,c);


/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count_eq(array[int] of var int: x, var int: y) ::promise_total =
  let { var 0..length(x): c; constraint fzn_count_eq(x,y,c); } in c;

function int: count_eq(array[int] of int: x, int: y) =
              sum(i in index_set(x))(x[i] = y);


include "fzn_count_eq.mzn";
%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
