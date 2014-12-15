include "count.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count(array[int] of var int: x, var int: y) ::promise_total =
  let { var 0..length(x): c; constraint count(x,y,c); } in c;
