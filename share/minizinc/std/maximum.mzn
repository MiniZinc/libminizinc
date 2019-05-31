/** @group globals Constrains \a m to be the maximum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate maximum(var int: m, array[int] of var int: x) =
    array_int_maximum(m, x);

/** @group globals Constrains \a m to be the maximum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate maximum(var float: m, array[int] of var float: x) =
    array_float_maximum(m, x);
