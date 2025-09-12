/** @group globals.math Constrains \a m to be the maximum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate maximum(var $$E: m, array [$X] of var $$E: x) =
  array_int_maximum(enum2int(m), enum2int(array1d(x)));

/** @group globals.math Constrains \a m to be the maximum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate maximum(var float: m, array [$X] of var float: x) = array_float_maximum(m, array1d(x));
