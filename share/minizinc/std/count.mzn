include "count_eq.mzn";

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x. 
*/
predicate count(array[int] of var int: x, var int: y, var int: c) =
    count_eq(x, y, c);

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count(array[int] of var int: x, var int: y) =
  let { var 0..length(x): c; constraint count(x,y,c); } in c;

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

