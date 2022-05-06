/** @group globals.math Constrains \a m to be the maximum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate maximum(var $$E: m, array[int] of var $$E: x) =
    array_int_maximum(m, x);

/** @group globals.math Constrains \a m to be the maximum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate maximum(var float: m, array[int] of var float: x) =
    array_float_maximum(m, x);
