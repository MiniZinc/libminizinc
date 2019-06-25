/** @group globals Constrains \a m to be the minimum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate minimum(var float: m, array[int] of var float: x) =
    array_float_minimum(m, x);

/** @group globals Constrains \a m to be the minimum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate minimum(var int: m, array[int] of var int: x) =
    array_int_minimum(m, x);
