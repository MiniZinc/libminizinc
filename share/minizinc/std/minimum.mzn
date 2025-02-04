/** @group globals.math Constrains \a m to be the minimum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate minimum(var float: m, array [$X] of var float: x) = array_float_minimum(m, array1d(x));

/** @group globals.math Constrains \a m to be the minimum of the values in \a x.

  Assumptions: |\a x| > 0.
*/
predicate minimum(var $$E: m, array [$X] of var $$E: x) =
  array_int_minimum(enum2int(m), enum2int(array1d(x)));
