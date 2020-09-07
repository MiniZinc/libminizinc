/***
  @groupdef stdlib.array Array operations

  These functions implement the basic operations on arrays.
*/

/** @group stdlib.array Return the concatenation of arrays \a x and \a y */
function array[int] of $T: '++'(array[int] of $T: x, array[int] of $T: y);
/** @group stdlib.array Return the concatenation of arrays \a x and \a y */
function array[int] of opt $T: '++'(array[int] of opt $T: x, array[int] of opt $T: y);
/** @group stdlib.array Return the concatenation of arrays \a x and \a y */
function array[int] of var $T: '++'(array[int] of var $T: x, array[int] of var $T: y);
/** @group stdlib.array Return the concatenation of arrays \a x and \a y */
function array[int] of var opt $T: '++'(array[int] of var opt $T: x, array[int] of var opt $T: y);

/** @group stdlib.array Return the length of array \a x

  Note that the length is defined as the number of elements in the
  array, regardless of its dimensionality.
*/
function int: length(array[$T] of var opt $U: x);

/** @group stdlib.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of $T: reverse(array[$$E] of $T: x) =
  if length(x)=0 then [] else
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)])
  endif;

/** @group stdlib.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of opt $T: reverse(array[$$E] of opt $T: x) =
  if length(x)=0 then [] else
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)])
  endif;

/** @group stdlib.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of var $T: reverse(array[$$E] of var $T: x) =
  if length(x)=0 then [] else
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)])
  endif;

/** @group stdlib.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of var opt $T: reverse(array[$$E] of var opt $T: x) =
  if length(x)=0 then [] else
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)])
  endif;

/** @group stdlib.array Test if \a x and \a y have the same index sets */
test index_sets_agree(array[$T] of var opt $U: x, array[$T] of var opt $W: y);

/** @group stdlib.array Return index set of one-dimensional array \a x */
function set of $$E: index_set(array[$$E] of var opt $U: x);
/** @group stdlib.array Return index set of first dimension of two-dimensional array \a x */
function set of $$E: index_set_1of2(array[$$E,int] of var opt $U: x);
/** @group stdlib.array Return index set of second dimension of two-dimensional array \a x */
function set of $$E: index_set_2of2(array[int,$$E] of var opt $U: x);
/** @group stdlib.array Return index set of first dimension of 3-dimensional array \a x */
function set of $$E: index_set_1of3(array[$$E,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of second dimension of 3-dimensional array \a x */
function set of $$E: index_set_2of3(array[int,$$E,int] of var opt $U: x);
/** @group stdlib.array Return index set of third dimension of 3-dimensional array \a x */
function set of $$E: index_set_3of3(array[int,int,$$E] of var opt $U: x);
/** @group stdlib.array Return index set of first dimension of 4-dimensional array \a x */
function set of $$E: index_set_1of4(array[$$E,int,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of second dimension of 4-dimensional array \a x */
function set of $$E: index_set_2of4(array[int,$$E,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of third dimension of 4-dimensional array \a x */
function set of $$E: index_set_3of4(array[int,int,$$E,int] of var opt $U: x);
/** @group stdlib.array Return index set of fourth dimension of 4-dimensional array \a x */
function set of $$E: index_set_4of4(array[int,int,int,$$E] of var opt $U: x);
/** @group stdlib.array Return index set of first dimension of 5-dimensional array \a x */
function set of $$E: index_set_1of5(array[$$E,int,int,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of second dimension of 5-dimensional array \a x */
function set of $$E: index_set_2of5(array[int,$$E,int,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of third dimension of 5-dimensional array \a x */
function set of $$E: index_set_3of5(array[int,int,$$E,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of fourth dimension of 5-dimensional array \a x */
function set of $$E: index_set_4of5(array[int,int,int,$$E,int] of var opt $U: x);
/** @group stdlib.array Return index set of fifth dimension of 5-dimensional array \a x */
function set of $$E: index_set_5of5(array[int,int,int,int,$$E] of var opt $U: x);
/** @group stdlib.array Return index set of first dimension of 6-dimensional array \a x */
function set of $$E: index_set_1of6(array[$$E,int,int,int,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of second dimension of 6-dimensional array \a x */
function set of $$E: index_set_2of6(array[int,$$E,int,int,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of third dimension of 6-dimensional array \a x */
function set of $$E: index_set_3of6(array[int,int,$$E,int,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of fourth dimension of 6-dimensional array \a x */
function set of $$E: index_set_4of6(array[int,int,int,$$E,int,int] of var opt $U: x);
/** @group stdlib.array Return index set of fifth dimension of 6-dimensional array \a x */
function set of $$E: index_set_5of6(array[int,int,int,int,$$E,int] of var opt $U: x);
/** @group stdlib.array Return index set of sixth dimension of 6-dimensional array \a x */
function set of $$E: index_set_6of6(array[int,int,int,int,int,$$E] of var opt $U: x);

/** @group stdlib.array Return array \a x coerced to index set 1..length(\a x).
  Coercions are performed by considering the array \a x in row-major order. */
function array[int] of $V: array1d(array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to index set 1..length(\a x).
  Coercions are performed by considering the array \a x in row-major order.  */
function array[int] of opt $V: array1d(array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to index set 1..length(\a x).
  Coercions are performed by considering the array \a x in row-major order.  */
function array[int] of var $V: array1d(array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to index set 1..length(\a x).
  Coercions are performed by considering the array \a x in row-major order.  */
function array[int] of var opt $V: array1d(array[$U] of var opt $V: x);

/** @group stdlib.array Return array \a x coerced to one-dimensional array with index set \a S.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E] of $V: array1d(set of $$E: S,
                                   array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to one-dimensional array with index set \a S.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E] of opt $V: array1d(set of $$E: S,
                                       array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to one-dimensional array with index set \a S.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E] of var $V: array1d(set of $$E: S,
                                       array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to one-dimensional array with index set \a S.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E] of var opt $V: array1d(set of $$E: S,
                                           array[$U] of var opt $V: x);

/** @group stdlib.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E,$$F] of $V: array2d(set of $$E: S1, set of $$F: S2, 
                                       array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E,$$F] of opt $V: array2d(set of $$E: S1, set of $$F: S2, 
                                           array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E,$$F] of var $V: array2d(set of $$E: S1, set of $$F: S2, 
                                           array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2.
  Coercions are performed by considering the array \a x in row-major order.  */
function array[$$E,$$F] of var opt $V: array2d(set of $$E: S1, set of $$F: S2, 
                                               array[$U] of var opt $V: x);

/** @group stdlib.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G] of $V: array3d(set of $$E: S1,
                                           set of $$F: S2,
                                           set of $$G: S3,
                                           array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G] of opt $V: array3d(set of $$E: S1,
                                               set of $$F: S2,
                                               set of $$G: S3,
                                               array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G] of var $V: array3d(set of $$E: S1,
                                               set of $$F: S2,
                                               set of $$G: S3,
                                               array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G] of var opt $V: array3d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   array[$U] of var opt $V: x);

/** @group stdlib.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H] of $V: array4d(set of $$E: S1,
                                               set of $$F: S2,
                                               set of $$G: S3,
                                               set of $$H: S4,
                                               array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H] of opt $V: array4d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   set of $$H: S4,
                                                   array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H] of var $V: array4d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   set of $$H: S4,
                                                   array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H] of var opt $V: array4d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       array[$U] of var opt $V: x);

/** @group stdlib.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I] of $V: array5d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   set of $$H: S4,
                                                   set of $$I: S5,
                                                   array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I] of opt $V: array5d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       set of $$I: S5,
                                                       array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I] of var $V: array5d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       set of $$I: S5,
                                                       array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I] of var opt $V: array5d(set of $$E: S1,
                                                           set of $$F: S2,
                                                           set of $$G: S3,
                                                           set of $$H: S4,
                                                           set of $$I: S5,
                                                           array[$U] of var opt $V: x);

/** @group stdlib.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of $V: array6d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       set of $$I: S5,
                                                       set of $$J: S6,
                                                       array[$U] of $V: x);
/** @group stdlib.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of opt $V: array6d(set of $$E: S1,
                                                           set of $$F: S2,
                                                           set of $$G: S3,
                                                           set of $$H: S4,
                                                           set of $$I: S5,
                                                           set of $$J: S6,
                                                           array[$U] of opt $V: x);
/** @group stdlib.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of var $V: array6d(set of $$E: S1,
                                                           set of $$F: S2,
                                                           set of $$G: S3,
                                                           set of $$H: S4,
                                                           set of $$I: S5,
                                                           set of $$J: S6,
                                                           array[$U] of var $V: x);
/** @group stdlib.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6.
  Coercions are performed by considering the array \a x in row-major order. 
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of var opt $V: array6d(set of $$E: S1,
                                                               set of $$F: S2,
                                                               set of $$G: S3,
                                                               set of $$H: S4,
                                                               set of $$I: S5,
                                                               set of $$J: S6,
                                                               array[$U] of var opt $V: x);

/** @group stdlib.array Forces a arrayNd call to throw an error if the new index sets are offsets of the current index sets.
*/
annotation array_check_form;

/** @group stdlib.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x.
  Coercions are performed by considering the array \a y in row-major order. 
*/
function array[$T] of $V: arrayXd(array[$T] of var opt $X: x, array[$U] of $V: y);
/** @group stdlib.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x.
  Coercions are performed by considering the array \a y in row-major order. 
*/
function array[$T] of opt $V: arrayXd(array[$T] of var opt $X: x, array[$U] of opt $V: y);
/** @group stdlib.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x.
  Coercions are performed by considering the array \a y in row-major order. 
*/
function array[$T] of var $V: arrayXd(array[$T] of var opt $X: x, array[$U] of var $V: y);
/** @group stdlib.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x.
  Coercions are performed by considering the array \a y in row-major order. 
*/
function array[$T] of var opt $V: arrayXd(array[$T] of var opt $X: x, array[$U] of var opt $V: y);

/** @group stdlib.array Return row \a r of array \a x */
function array[$$E] of $T: row(array[int, $$E] of $T: x, int: r) = x[r,..];
/** @group stdlib.array Return row \a r of array \a x */
function array[$$E] of opt $T: row(array[int, $$E] of opt $T: x, int: r) = x[r,..];
/** @group stdlib.array Return row \a r of array \a x */
function array[$$E] of var $T: row(array[int, $$E] of var $T: x, int: r) = x[r,..];
/** @group stdlib.array Return row \a r of array \a x */
function array[$$E] of var opt $T: row(array[int, $$E] of var opt $T: x, int: r) = x[r,..];

/** @group stdlib.array Return column \a c of array \a x */
function array[$$E] of $T: col(array[$$E,int] of $T: x, int: c) = x[..,c];
/** @group stdlib.array Return column \a c of array \a x */
function array[$$E] of opt $T: col(array[$$E,int] of opt $T: x, int: c) = x[..,c];
/** @group stdlib.array Return column \a c of array \a x */
function array[$$E] of var $T: col(array[$$E,int] of var $T: x, int: c) = x[..,c];
/** @group stdlib.array Return column \a c of array \a x */
function array[$$E] of var opt $T: col(array[$$E,int] of var opt $T: x, int: c) = x[..,c];

/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 1d array with index set \a dims1 */
function array[int] of $T: slice_1d(array[$E] of $T: x, array[int] of set of int: s, set of int: dims1);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 2d array with index sets \a dims1 and \a dims2 */
function array[int,int] of $T: slice_2d(array[$E] of $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2 and \a dims3 */
function array[int,int,int] of $T: slice_3d(array[$E] of $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4 */
function array[int,int,int] of $T: slice_4d(array[$E] of $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5 */
function array[int,int,int] of $T: slice_5d(array[$E] of $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5, \a dims6 */
function array[int,int,int] of $T: slice_6d(array[$E] of $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5, set of int: dims6);

/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 1d array with index set \a dims1 */
function array[int] of opt $T: slice_1d(array[$E] of opt $T: x, array[int] of set of int: s, set of int: dims1);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 2d array with index sets \a dims1 and \a dims2 */
function array[int,int] of opt $T: slice_2d(array[$E] of opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2 and \a dims3 */
function array[int,int,int] of opt $T: slice_3d(array[$E] of opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4 */
function array[int,int,int] of opt $T: slice_4d(array[$E] of opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5 */
function array[int,int,int] of opt $T: slice_5d(array[$E] of opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5, \a dims6 */
function array[int,int,int] of opt $T: slice_6d(array[$E] of opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5, set of int: dims6);

/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 1d array with index set \a dims1 */
function array[int] of var $T: slice_1d(array[$E] of var $T: x, array[int] of set of int: s, set of int: dims1);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 2d array with index sets \a dims1 and \a dims2 */
function array[int,int] of var $T: slice_2d(array[$E] of var $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2 and \a dims3 */
function array[int,int,int] of var $T: slice_3d(array[$E] of var $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4 */
function array[int,int,int] of var $T: slice_4d(array[$E] of var $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5 */
function array[int,int,int] of var $T: slice_5d(array[$E] of var $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5, \a dims6 */
function array[int,int,int] of var $T: slice_6d(array[$E] of var $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5, set of int: dims6);

/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 1d array with index set \a dims1 */
function array[int] of var opt $T: slice_1d(array[$E] of var opt $T: x, array[int] of set of int: s, set of int: dims1);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 2d array with index sets \a dims1 and \a dims2 */
function array[int,int] of var opt $T: slice_2d(array[$E] of var opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2 and \a dims3 */
function array[int,int,int] of var opt $T: slice_3d(array[$E] of var opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4 */
function array[int,int,int] of var opt $T: slice_4d(array[$E] of var opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5 */
function array[int,int,int] of var opt $T: slice_5d(array[$E] of var opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5);
/** @group stdlib.array Return slice of array \a x specified by sets \a s, coerced to new 3d array with index sets \a dims1, \a dims2, \a dims3, \a dims4, \a dims5, \a dims6 */
function array[int,int,int] of var opt $T: slice_6d(array[$E] of var opt $T: x, array[int] of set of int: s, set of int: dims1, set of int: dims2, set of int: dims3, set of int: dims4, set of int: dims5, set of int: dims6);


/** @group stdlib.array Test if \a i is in the index set of \a x */
test has_index(int: i, array[int] of var opt $T: x) = i in index_set(x);
/** @group stdlib.array Test if \a e is an element of array \a x */
test has_element($T: e, array[int] of $T: x) = exists (i in index_set(x)) (x[i]=e);
/** @group stdlib.array Test if \a e is an element of array \a x */
test has_element($T: e, array[int] of opt $T: x) = exists (i in index_set(x)) (x[i]=e);
/** @group stdlib.array Test if \a e is an element of array \a x */
predicate has_element($T: e, array[$$E] of var opt $T: x) = exists (i in index_set(x)) (x[i]=e);

/** @group stdlib.array Return the set containing the elements of \a x */
function var set of $$T: array2set(array[int] of var $$T: x) ::promise_total =
    let {
      set of int: D = dom_array(x);
      constraint assert(min(D) > -infinity /\ max(D) < infinity, "array2set needs finite bounds on argument array");
      var set of D: y = array_union([
        let { var set of dom(x[i]): s;
        constraint x[i] in s /\ card(s)=1;
        } in s | i in index_set(x)]);
    } in y;

function var set of $$T: array2set(array[int] of var opt $$T: x) ::promise_total =
    let {
      set of int: D = dom_array(x);
      constraint assert(min(D) > -infinity /\ max(D) < infinity, "array2set needs finite bounds on argument array");
      var set of D: y = array_union([
        let { var set of dom(x[i]): s;
        constraint if occurs(x[i]) then deopt(x[i]) in s /\ card(s)=1 else card(s)=0 endif;
        } in s | i in index_set(x)]);
    } in y;

/** @group stdlib.array Return the set containing the elements of \a x */
function set of $$T: array2set(array[int] of $$T: x) =
  { x[i] | i in index_set(x) };

/** @group stdlib.array Return the set containing the elements of \a x */
function set of $$T: array2set(array[int] of opt $$T: x) =
  { deopt(x[i]) | i in index_set(x) where occurs(x[i]) };

/** @group stdlib.array Return the set containing the elements of \a x */
function set of bool: array2set(array[int] of bool: x) =
  let { bool: f = exists(b in x)(not b);
        bool: t = exists(b in x)(b);
  } in if f /\ t then {false,true} elseif f then {false} elseif t then {true} else {} endif;

/** @group stdlib.array Return the set containing the elements of \a x */
function set of float: array2set(array[int] of float: x) =
  { x[i] | i in index_set(x) };

