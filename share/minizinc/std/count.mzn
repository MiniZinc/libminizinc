include "count_eq.mzn";

/** @group globals.counting
  Constrains \a n to be the number of occurrences of \a value in \a xs.
*/
predicate count(array [$X] of var opt $$E: xs, var $$E: value, var int: n) = count_eq(xs, value, n);
