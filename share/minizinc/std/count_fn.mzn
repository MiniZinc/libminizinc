include "count_eq.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a y in \a x. 
*/
function var int: count(array[$X] of var opt $$E: x, var $$E: y) = count_eq(x, y);

function int: count(array[$X] of opt $$E: x, $$E: y) = count_eq(x, y);
