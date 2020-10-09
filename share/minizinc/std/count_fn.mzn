include "count.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count(array[int] of var int: x, var int: y) ::promise_total =
  let { var 0..length(x): c::is_defined_var; constraint count(x,y,c)::defines_var(c); } in c;

function int: count(array[int] of int: x, int: y) = count(i in x)(i=y);
