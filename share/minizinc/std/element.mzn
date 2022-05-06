/** @group globals.array
  Constrains \a i to be the index of the element \a y in the array \a x.
*/
predicate element(var $$E: i, array[$$E] of var bool: x, var bool: y) =
	y = x[i];

/** @group globals.array
  Constrains \a i to be the index of the element \a y in the array \a x.
*/
predicate element(var $$E: i, array[$$E] of var float: x, var float: y) =
	y = x[i];

/** @group globals.array
  Constrains \a i to be the index of the element \a y in the array \a x.
*/
predicate element(var $$E: i, array[$$E] of var $$T: x, var $$T: y) =
	y = x[i];

/** @group globals.array
  Constrains \a i to be the index of the element \a y in the array \a x.
*/
predicate element(
	var $$E: i,
	array[$$E] of var set of $$T: x,
	var set of $$T: y
) =
	y = x[i];
