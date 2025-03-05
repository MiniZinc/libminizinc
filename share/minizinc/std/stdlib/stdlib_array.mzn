/***
  @groupdef stdlib.builtins.array Array operations

  These functions implement the basic operations on arrays.
*/

/** @group stdlib.builtins.array Return the concatenation of arrays \a x and \a y */
function array [int] of any $T: '++'(array [$$X] of any $T: x, array [$$Y] of any $T: y)
  :: mzn_internal_representation;

/** @group stdlib.builtins.array Return the concatenation of the multidimensional arrays \a x and \a y

The index sets of the arrays must match except for the first index set.
 */
function array [int, $$E] of any $T: '++'(
  array [$$X, $$E] of any $T: x,
  array [$$Y, $$E] of any $T: y,
) =
  assert(
    index_sets_agree(index2int(x)[1..0, ..], index2int(y)[1..0, ..]),
    "Column index sets must match",
    array2d(
      1..card(index_set_1of2(x)) + card(index_set_1of2(y)),
      index_set_2of2(x),
      array1d(x) ++ array1d(y),
    ),
  );
/** @group stdlib.builtins.array Return the concatenation of the multidimensional arrays \a x and \a y

The index sets of the arrays must match except for the first index set.
 */
function array [int, $$E, $$F] of any $T: '++'(
  array [$$X, $$E, $$F] of any $T: x,
  array [$$Y, $$E, $$F] of any $T: y,
) =
  assert(
    index_sets_agree(index2int(x)[1..0, .., ..], index2int(y)[1..0, .., ..]),
    "Index sets other than the first index set must match",
    array3d(
      1..card(index_set_1of3(x)) + card(index_set_1of3(y)),
      index_set_2of3(x),
      index_set_3of3(x),
      array1d(x) ++ array1d(y),
    ),
  );
/** @group stdlib.builtins.array Return the concatenation of the multidimensional arrays \a x and \a y

The index sets of the arrays must match except for the first index set.
 */
function array [int, $$E, $$F, $$G] of any $T: '++'(
  array [$$X, $$E, $$F, $$G] of any $T: x,
  array [$$Y, $$E, $$F, $$G] of any $T: y,
) =
  assert(
    index_sets_agree(index2int(x)[1..0, .., .., ..], index2int(y)[1..0, .., .., ..]),
    "Index sets other than the first index set must match",
    array4d(
      1..card(index_set_1of4(x)) + card(index_set_1of4(y)),
      index_set_2of4(x),
      index_set_3of4(x),
      index_set_4of4(x),
      array1d(x) ++ array1d(y),
    ),
  );
/** @group stdlib.builtins.array Return the concatenation of the multidimensional arrays \a x and \a y

The index sets of the arrays must match except for the first index set.
 */
function array [int, $$E, $$F, $$G, $$H] of any $T: '++'(
  array [$$X, $$E, $$F, $$G, $$H] of any $T: x,
  array [$$Y, $$E, $$F, $$G, $$H] of any $T: y,
) =
  assert(
    index_sets_agree(index2int(x)[1..0, .., .., .., ..], index2int(y)[1..0, .., .., .., ..]),
    "Index sets other than the first index set must match",
    array5d(
      1..card(index_set_1of5(x)) + card(index_set_1of5(y)),
      index_set_2of5(x),
      index_set_3of5(x),
      index_set_4of5(x),
      index_set_5of5(x),
      array1d(x) ++ array1d(y),
    ),
  );
/** @group stdlib.builtins.array Return the concatenation of the multidimensional arrays \a x and \a y

The index sets of the arrays must match except for the first index set.
 */
function array [int, $$E, $$F, $$G, $$H, $$I] of any $T: '++'(
  array [$$X, $$E, $$F, $$G, $$H, $$I] of any $T: x,
  array [$$Y, $$E, $$F, $$G, $$H, $$I] of any $T: y,
) =
  assert(
    index_sets_agree(
      index2int(x)[1..0, .., .., .., .., ..],
      index2int(y)[1..0, .., .., .., .., ..],
    ),
    "Index sets other than the first index set must match",
    array6d(
      1..card(index_set_1of6(x)) + card(index_set_1of6(y)),
      index_set_2of6(x),
      index_set_3of6(x),
      index_set_4of6(x),
      index_set_5of6(x),
      index_set_6of6(x),
      array1d(x) ++ array1d(y),
    ),
  );

/** @group stdlib.builtins.array Return the length of array \a x

  Note that the length is defined as the number of elements in the
  array, regardless of its dimensionality.
*/
function int: length(array [$T] of any $U: x);

/** @group stdlib.builtins.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array [$$E] of any $T: reverse(array [$$E] of any $T: x) =
  if length(x) = 0 then
    []
  else
    let {
      any: xx = array1d(x);
      int: l = length(x) + 1;
    } in array1d(index_set(x), [xx[l - i] | i in index_set(xx)])
  endif;

/** @group stdlib.builtins.array Test if \a x and \a y have the same index sets */
test index_sets_agree(array [$T] of any $U: x, array [$T] of any $W: y);

/** @group stdlib.builtins.array Return index set of one-dimensional array \a x */
function set of $$E: index_set(array [$$E] of any $U: x);
/** @group stdlib.builtins.array Return index set of first dimension of two-dimensional array \a x */
function set of $$E: index_set_1of2(array [$$E, $$F] of any $U: x);
/** @group stdlib.builtins.array Return index set of second dimension of two-dimensional array \a x */
function set of $$F: index_set_2of2(array [$$E, $$F] of any $U: x);
/** @group stdlib.builtins.array Return index set of first dimension of 3-dimensional array \a x */
function set of $$E: index_set_1of3(array [$$E, $$F, $$G] of any $U: x);
/** @group stdlib.builtins.array Return index set of second dimension of 3-dimensional array \a x */
function set of $$F: index_set_2of3(array [$$E, $$F, $$G] of any $U: x);
/** @group stdlib.builtins.array Return index set of third dimension of 3-dimensional array \a x */
function set of $$G: index_set_3of3(array [$$E, $$F, $$G] of any $U: x);
/** @group stdlib.builtins.array Return index set of first dimension of 4-dimensional array \a x */
function set of $$E: index_set_1of4(array [$$E, $$F, $$G, $$H] of any $U: x);
/** @group stdlib.builtins.array Return index set of second dimension of 4-dimensional array \a x */
function set of $$F: index_set_2of4(array [$$E, $$F, $$G, $$H] of any $U: x);
/** @group stdlib.builtins.array Return index set of third dimension of 4-dimensional array \a x */
function set of $$G: index_set_3of4(array [$$E, $$F, $$G, $$H] of any $U: x);
/** @group stdlib.builtins.array Return index set of fourth dimension of 4-dimensional array \a x */
function set of $$H: index_set_4of4(array [$$E, $$F, $$G, $$H] of any $U: x);
/** @group stdlib.builtins.array Return index set of first dimension of 5-dimensional array \a x */
function set of $$E: index_set_1of5(array [$$E, $$F, $$G, $$H, $$I] of any $U: x);
/** @group stdlib.builtins.array Return index set of second dimension of 5-dimensional array \a x */
function set of $$F: index_set_2of5(array [$$E, $$F, $$G, $$H, $$I] of any $U: x);
/** @group stdlib.builtins.array Return index set of third dimension of 5-dimensional array \a x */
function set of $$G: index_set_3of5(array [$$E, $$F, $$G, $$H, $$I] of any $U: x);
/** @group stdlib.builtins.array Return index set of fourth dimension of 5-dimensional array \a x */
function set of $$H: index_set_4of5(array [$$E, $$F, $$G, $$H, $$I] of any $U: x);
/** @group stdlib.builtins.array Return index set of fifth dimension of 5-dimensional array \a x */
function set of $$I: index_set_5of5(array [$$E, $$F, $$G, $$H, $$I] of any $U: x);
/** @group stdlib.builtins.array Return index set of first dimension of 6-dimensional array \a x */
function set of $$E: index_set_1of6(array [$$E, $$F, $$G, $$H, $$I, $$J] of any $U: x);
/** @group stdlib.builtins.array Return index set of second dimension of 6-dimensional array \a x */
function set of $$F: index_set_2of6(array [$$E, $$F, $$G, $$H, $$I, $$J] of any $U: x);
/** @group stdlib.builtins.array Return index set of third dimension of 6-dimensional array \a x */
function set of $$G: index_set_3of6(array [$$E, $$F, $$G, $$H, $$I, $$J] of any $U: x);
/** @group stdlib.builtins.array Return index set of fourth dimension of 6-dimensional array \a x */
function set of $$H: index_set_4of6(array [$$E, $$F, $$G, $$H, $$I, $$J] of any $U: x);
/** @group stdlib.builtins.array Return index set of fifth dimension of 6-dimensional array \a x */
function set of $$I: index_set_5of6(array [$$E, $$F, $$G, $$H, $$I, $$J] of any $U: x);
/** @group stdlib.builtins.array Return index set of sixth dimension of 6-dimensional array \a x */
function set of $$J: index_set_6of6(array [$$E, $$F, $$G, $$H, $$I, $$J] of any $U: x);

/** @group stdlib.builtins.array Return array \a x coerced to index set 1..length(\a x).
  Coercions are performed by considering the array \a x in row-major order. */
function array [int] of any $V: array1d(array [$U] of any $V: x);

/** @group stdlib.builtins.array Return array \a x coerced to one-dimensional array with index set \a S.
  Coercions are performed by considering the array \a x in row-major order.  */
function array [$$E] of any $V: array1d(set of $$E: S, array [$U] of any $V: x);

/** @group stdlib.builtins.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2.
  Coercions are performed by considering the array \a x in row-major order.  */
function array [$$E, $$F] of any $V: array2d(
  set of $$E: S1,
  set of $$F: S2,
  array [$U] of any $V: x,
);

function array [$$E, $$F] of any $V: array2d(
  array [int] of $$E: S1,
  array [int] of $$F: S2,
  array [int] of any $V: x,
) =
  let {
    int: d = length(S2);
  } in [(S1[(i - 1) div d + 1], S2[(i - 1) mod d + 1]): x[i] | i in index_set(x)];

function array [$$E] of any $V: arrayNd(array [int] of $$E: S1, array [int] of any $V: x) =
  if length(S1) = 1 /\ length(x) > 1 then
    array1d(S1[1]..to_enum(enum_of(S1[1]), enum2int(S1[1]) + length(x) - 1), x)
  else
    [S1[i]: x[i] | i in index_set(x)]
  endif;
function array [$$E, $$F] of any $V: arrayNd(
  array [int] of $$E: S1,
  array [int] of $$F: S2,
  array [int] of any $V: x,
) = [(S1[i], S2[i]): x[i] | i in index_set(x)];
function array [$$E, $$F, $$G] of any $V: arrayNd(
  array [int] of $$E: S1,
  array [int] of $$F: S2,
  array [int] of $$G: S3,
  array [int] of any $V: x,
) = [(S1[i], S2[i], S3[i]): x[i] | i in index_set(x)];
function array [$$E, $$F, $$G, $$H] of any $V: arrayNd(
  array [int] of $$E: S1,
  array [int] of $$F: S2,
  array [int] of $$G: S3,
  array [int] of $$H: S4,
  array [int] of any $V: x,
) = [(S1[i], S2[i], S3[i], S4[i]): x[i] | i in index_set(x)];
function array [$$E, $$F, $$G, $$H, $$I] of any $V: arrayNd(
  array [int] of $$E: S1,
  array [int] of $$F: S2,
  array [int] of $$G: S3,
  array [int] of $$H: S4,
  array [int] of $$I: S5,
  array [int] of any $V: x,
) = [(S1[i], S2[i], S3[i], S4[i], S5[i]): x[i] | i in index_set(x)];
function array [$$E, $$F, $$G, $$H, $$I, $$J] of any $V: arrayNd(
  array [int] of $$E: S1,
  array [int] of $$F: S2,
  array [int] of $$G: S3,
  array [int] of $$H: S4,
  array [int] of $$I: S5,
  array [int] of $$J: S6,
  array [int] of any $V: x,
) = [(S1[i], S2[i], S3[i], S4[i], S5[i], S6[i]): x[i] | i in index_set(x)];

/** @group stdlib.builtins.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array [$$E, $$F, $$G] of any $V: array3d(
  set of $$E: S1,
  set of $$F: S2,
  set of $$G: S3,
  array [$U] of any $V: x,
);

/** @group stdlib.builtins.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array [$$E, $$F, $$G, $$H] of any $V: array4d(
  set of $$E: S1,
  set of $$F: S2,
  set of $$G: S3,
  set of $$H: S4,
  array [$U] of any $V: x,
);

/** @group stdlib.builtins.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array [$$E, $$F, $$G, $$H, $$I] of any $V: array5d(
  set of $$E: S1,
  set of $$F: S2,
  set of $$G: S3,
  set of $$H: S4,
  set of $$I: S5,
  array [$U] of any $V: x,
);

/** @group stdlib.builtins.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array [$$E, $$F, $$G, $$H, $$I, $$J] of any $V: array6d(
  set of $$E: S1,
  set of $$F: S2,
  set of $$G: S3,
  set of $$H: S4,
  set of $$I: S5,
  set of $$J: S6,
  array [$U] of any $V: x,
);

/** @group stdlib.builtins.array Forces a arrayNd call to throw an error if the new index sets are offsets of the current index sets.
*/
annotation array_check_form;

/** @group stdlib.builtins.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x.
  Coercions are performed by considering the array \a y in row-major order. 
*/
function array [$T] of any $V: arrayXd(array [$T] of any $X: x, array [$U] of any $V: y);

/** @group stdlib.builtins.array Return row \a r of array \a x */
function array [$$E] of any $T: row(array [$$F, $$E] of any $T: x, $$F: r) = x[r, ..];

/** @group stdlib.builtins.array Return column \a c of array \a x */
function array [$$E] of any $T: col(array [$$E, $$F] of any $T: x, $$F: c) = x[.., c];

/** @group stdlib.builtins.array Return slice of array \a x specified by sets \a s, coerced to new 1d array with index set \a dims1 */
function array [$$F] of any $T: slice_1d(
  array [$E] of any $T: x,
  array [int] of set of int: s,
  set of $$F: dims1,
);
/** @group stdlib.builtins.array Return slice of array \a x specified by sets \a s, coerced to new 2d array with index sets \a dims1 and \a dims2 */
function array [$$F, $$G] of any $T: slice_2d(
  array [$E] of any $T: x,
  array [int] of set of int: s,
  set of $$F: dims1,
  set of $$G: dims2,
);
/** @group stdlib.builtins.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2 and \a dims3 */
function array [$$F, $$G, $$H] of any $T: slice_3d(
  array [$E] of any $T: x,
  array [int] of set of int: s,
  set of $$F: dims1,
  set of $$G: dims2,
  set of $$H: dims3,
);
/** @group stdlib.builtins.array Return slice of array \a x specified by sets \a s, coerced to new 4d array with index sets \a dims1, \a dims2, \a dims3, \a dims4 */
function array [$$F, $$G, $$H, $$I] of any $T: slice_4d(
  array [$E] of any $T: x,
  array [int] of set of int: s,
  set of $$F: dims1,
  set of $$G: dims2,
  set of $$H: dims3,
  set of $$I: dims4,
);
/** @group stdlib.builtins.array Return slice of array \a x specified by sets \a s, coerced to new 5d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5 */
function array [$$F, $$G, $$H, $$I, $$J] of any $T: slice_5d(
  array [$E] of any $T: x,
  array [int] of set of int: s,
  set of $$F: dims1,
  set of $$G: dims2,
  set of $$H: dims3,
  set of $$I: dims4,
  set of $$J: dims5,
);
/** @group stdlib.builtins.array Return slice of array \a x specified by sets \a s, coerced to new 6d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5, \a dims6 */
function array [$$F, $$G, $$H, $$I, $$J, $$K] of any $T: slice_6d(
  array [$E] of any $T: x,
  array [int] of set of int: s,
  set of $$F: dims1,
  set of $$G: dims2,
  set of $$H: dims3,
  set of $$I: dims4,
  set of $$J: dims5,
  set of $$K: dims6,
);

/** @group stdlib.builtins.array Test if \a i is in the index set of \a x */
test has_index($$E: i, array [$$E] of any $T: x) = i in index_set(x);
/** @group stdlib.builtins.array Test if \a e is an element of array \a x */
test has_element($T: e, array [int] of $T: x) = exists (i in index_set(x)) (x[i] = e);
/** @group stdlib.builtins.array Test if \a e is an element of array \a x */
test has_element($T: e, array [int] of opt $T: x) = exists (i in index_set(x)) (x[i] = e);
/** @group stdlib.builtins.array Test if \a e is an element of array \a x */
predicate has_element($T: e, array [$$E] of any $T: x) = exists (i in index_set(x)) (x[i] = e);

/** @group stdlib.builtins.array Return the set containing the elements of \a x */
function var set of $$T: array2set(array [$X] of var $$T: x) :: promise_total =
  if length(x) = 0 then
    {}
  else
    let {
      any: D = dom_array(x);
      constraint
        assert(
          min(enum2int(D)) > -infinity /\ max(enum2int(D)) < infinity,
          "array2set needs finite bounds on argument array",
        );
      var set of D: y;
      constraint fzn_array_int_union(enum2int(array1d(x)), enum2int(y));
    } in y
  endif;

/** @group stdlib.builtins.array Return the set containing the non-absent elements of \a x */
function var set of $$T: array2set(array [$X] of var opt $$T: x) :: promise_total =
  if length(x) = 0 then
    {}
  else
    let {
      any: D = dom_array_occurring(x);
      constraint
        assert(
          min(enum2int(D)) > -infinity /\ max(enum2int(D)) < infinity,
          "array2set needs finite bounds on argument array",
        );
      var set of D: y;
      constraint fzn_array_opt_int_union(enum2int(array1d(x)), enum2int(y));
    } in y
  endif;

/** @group stdlib.builtins.array Return the set containing the elements of \a x */
function set of $$T: array2set(array [int] of $$T: x) = {x[i] | i in index_set(x)};

/** @group stdlib.builtins.array Return the set containing the non-absent elements of \a x */
function set of $$T: array2set(array [int] of opt $$T: x) = {
  deopt(x[i]) |
    i in index_set(x) where occurs(x[i]),
};

/** @group stdlib.builtins.array Return the set containing the elements of \a x */
function set of bool: array2set(
  array [int] of bool: x,
) =
  let {
    bool: f = exists (b in x) (not b);
    bool: t = exists (b in x) (b);
  } in if f /\ t then {false, true} elseif f then {false} elseif t then {true} else {} endif;

/** @group stdlib.builtins.array Return the set containing the elements of \a x */
function set of float: array2set(array [int] of float: x) = {x[i] | i in index_set(x)};

/** @group stdlib.builtins.array Return if \a y contains \a x */
function bool: 'in'($X: x, array [$T] of $X: y) = exists (z in array1d(y)) (z = x);

/** @group stdlib.builtins.array Return if \a y contains \a x */
function bool: 'in'(opt $X: x, array [$T] of opt $X: y) = exists (z in array1d(y)) (z = x);

/** @group stdlib.builtins.array Return if \a y contains \a x */
function var bool: 'in'(var $X: x, array [$T] of var $X: y) = exists (z in array1d(y)) (z = x);

/** @group stdlib.builtins.array Return if \a y contains \a x */
function var bool: 'in'(var opt $X: x, array [$T] of var opt $X: y) =
  exists (z in array1d(y)) (z = x);

/** @group stdlib.builtins.array Return if \a y contains \a x */
function bool: 'in'(set of $X: x, array [$T] of set of $X: y) = exists (z in array1d(y)) (z = x);

/** @group stdlib.builtins.array Return if \a y contains \a x */
function var bool: 'in'(var set of $$X: x, array [$T] of var set of $$X: y) =
  exists (z in array1d(y)) (z = x);
