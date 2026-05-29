/** @group globals.math Constrains \a min_value to be the minimum of the values in \a xs.

  Assumptions: |\a xs| > 0.
*/
predicate minimum(var float: min_value, array [$X] of var float: xs) = array_float_minimum(min_value, array1d(xs));

/** @group globals.math Constrains \a min_value to be the minimum of the values in \a xs.

  Assumptions: |\a xs| > 0.
*/
predicate minimum(var $$E: min_value, array [$X] of var $$E: xs) =
  array_int_minimum(enum2int(min_value), enum2int(array1d(xs)));
