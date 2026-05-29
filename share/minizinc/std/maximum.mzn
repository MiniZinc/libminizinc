/** @group globals.math Constrains \a max_value to be the maximum of the values in \a xs.

  Assumptions: |\a xs| > 0.
*/
predicate maximum(var $$E: max_value, array [$X] of var $$E: xs) =
  array_int_maximum(enum2int(max_value), enum2int(array1d(xs)));

/** @group globals.math Constrains \a max_value to be the maximum of the values in \a xs.

  Assumptions: |\a xs| > 0.
*/
predicate maximum(var float: max_value, array [$X] of var float: xs) = array_float_maximum(max_value, array1d(xs));
