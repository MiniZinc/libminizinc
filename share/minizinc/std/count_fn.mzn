include "count.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count(array[$X] of var $$E: x, var $$E: y) ::promise_total =
  let { var 0..length(x): c::is_defined_var; constraint count(array1d(x),y,c)::defines_var(c); } in c;

function int: count(array[$X] of $$E: x, $$E: y) = count(i in array1d(x))(i=y);
