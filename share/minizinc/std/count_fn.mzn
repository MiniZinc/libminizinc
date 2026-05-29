include "count_eq.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a value in \a xs. 
*/
function var int: count(array [$X] of var opt $$E: xs, var $$E: value) = count_eq(xs, value);

function int: count(array [$X] of opt $$E: xs, $$E: value) = count_eq(xs, value);
