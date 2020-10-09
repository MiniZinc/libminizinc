include "count_eq.mzn";

/** @group globals.counting
  Constrains \a c to be the number of occurrences of \a y in \a x.
*/
predicate count(array[$X] of var int: x, var int: y, var int: c) =
    count_eq(array1d(x), y, c);
