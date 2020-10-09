/***
  @groupdef stdlib.compare Comparison Builtins

  These builtins implement comparison operations.
*/

/** @group stdlib.compare Return if \a x is less than \a y */
function     bool: '<'(    $T: x,    $T: y);
/** @group stdlib.compare Return if \a x is less than \a y */
function var bool: '<'(var $T: x,var $T: y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);

/** @group stdlib.compare Return if \a x is greater than \a y */
function     bool: '>'(    $T: x,    $T: y);
/** @group stdlib.compare Return if \a x is greater than \a y */
function var bool: '>'(var $T: x,var $T: y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);

/** @group stdlib.compare Return if \a x is less than or equal to \a y */
function     bool: '<='(    $T: x,    $T: y);
/** @group stdlib.compare Return if \a x is less than or equal to \a y */
function var bool: '<='(var $T: x, var $T: y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group stdlib.compare Return if \a x is greater than or equal to \a y */
function     bool: '>='(    $T: x,    $T: y);
/** @group stdlib.compare Return if \a x is greater than or equal to \a y */
function var bool: '>='(var $T: x,var $T: y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.arithmetic Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);

/** @group stdlib.compare Return if \a x is equal to \a y */
function     bool: '='(    $T: x,    $T: y);
/** @group stdlib.compare Return if \a x is equal to \a y */
function     bool: '='(opt $T: x, opt $T: y) =
  absent(x) /\ absent(y) \/ occurs(x) /\ occurs(y) /\ deopt(x) = deopt(y);
/** @group stdlib.compare Return if \a x is equal to \a y */
function var bool: '='(var $T: x,var $T: y);
/** @group stdlib.compare Return if \a x is equal to \a y */
function var bool: '='(var opt $T: x,var opt $T: y);

/** @group stdlib.compare Return if \a x is not equal to \a y */
function     bool: '!='(    $T: x,     $T: y);
/** @group stdlib.compare Return if \a x is not equal to \a y */
function     bool: '!='(opt $T: x, opt $T: y) = not (x = y);
/** @group stdlib.compare Return if \a x is not equal to \a y */
function var bool: '!='(var $T: x, var $T: y);
/** @group stdlib.compare Return if \a x is not equal to \a y */
function var bool: '!='(var opt $T: x, var opt $T: y);

% Special case comparison operators for integer variable and float constant
function var bool: '<='(var int: x, float: y) = (x <= floor(y));
function var bool: '>='(var int: x, float: y) = (x >= ceil(y));
function var bool: '<='(float: x, var int: y) = (y >= ceil(x));
function var bool: '>='(float: x, var int: y) = (y <= floor(x));

function var bool: '<'(var int: x, float: y) = (x <= ceil(y)-1);
function var bool: '>'(float: x, var int: y) = (y <= ceil(x)-1);
function var bool: '>'(var int: x, float: y) = (x >= floor(y)+1);
function var bool: '<'(float: x, var int: y) = (y >= floor(x)+1);

function var bool: '='(var int: x, float: y) =
  if ceil(y)=floor(y) then x=ceil(y) else false endif;
function var bool: '='(float: x, var int: y) =
  if ceil(x)=floor(x) then y=ceil(x) else false endif;

function var bool: '!='(var int: x, float: y) =
  if ceil(y)=floor(y) then x != ceil(y) else true endif;
function var bool: '!='(float: x, var int: y) =
  if ceil(x)=floor(x) then y != ceil(x) else true endif;

function bool: '<='(int: x, float: y) = (x <= floor(y));
function bool: '>='(int: x, float: y) = (x >= ceil(y));
function bool: '<='(float: x, int: y) = (y >= ceil(x));
function bool: '>='(float: x, int: y) = (y <= floor(x));

function bool: '<'(int: x, float: y) = (x <= ceil(y)-1);
function bool: '>'(float: x, int: y) = (y <= ceil(x)-1);
function bool: '>'(int: x, float: y) = (x >= floor(y)+1);
function bool: '<'(float: x, int: y) = (y >= floor(x)+1);

function bool: '='(int: x, float: y) =
  if ceil(y)=floor(y) then x=ceil(y) else false endif;
function bool: '='(float: x, int: y) =
  if ceil(x)=floor(x) then y=ceil(x) else false endif;

function bool: '!='(int: x, float: y) =
  if ceil(y)=floor(y) then x != ceil(y) else true endif;
function bool: '!='(float: x, int: y) =
  if ceil(x)=floor(x) then y != ceil(x) else true endif;


/** @group stdlib.compare Return if array \a x is lexicographically smaller than array \a y */
function     bool: '<'(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group stdlib.compare Return if array \a x is lexicographically smaller than array \a y */
function var bool: '<'(array[$U] of var $T: x,array[$U] of var $T: y);
/** @group stdlib.compare Return if array \a x is lexicographically greater than array \a y */
function     bool: '>'(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group stdlib.compare Return if array \a x is lexicographically greater than array \a y */
function var bool: '>'(array[$U] of var $T: x,array[$U] of var $T: y);
/** @group stdlib.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function     bool: '<='(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group stdlib.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function var bool: '<='(array[$U] of var $T: x,array[$U] of var $T: y);
/** @group stdlib.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function     bool: '>='(array[$U] of     $T: x,array[$U] of     $T: y);
function var bool: '>='(array[$U] of var $T: x,array[$U] of var $T: y);

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     int: x,array[$T] of     int: y) =
    let {
        array[int] of int: xx = array1d(x);
        array[int] of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var int: x,array[$T] of var int: y) =
    let {
        array[int] of var int: xx = array1d(x);
        array[int] of var int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     bool: x,array[$T] of     bool: y) =
    let {
        array[int] of bool: xx = array1d(x);
        array[int] of bool: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var bool: x,array[$T] of var bool: y) =
    let {
        array[int] of var bool: xx = array1d(x);
        array[int] of var bool: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     set of int: x,array[$T] of     set of int: y) =
    let {
        array[int] of set of int: xx = array1d(x);
        array[int] of set of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var set of int: x,array[$T] of var set of int: y) =
    let {
        array[int] of var set of int: xx = array1d(x);
        array[int] of var set of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     float: x,array[$T] of     float: y) =
    let {
        array[int] of float: xx = array1d(x);
        array[int] of float: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var float: x,array[$T] of var float: y) =
    let {
        array[int] of var float: xx = array1d(x);
        array[int] of var float: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function     bool: '!='(array[$T] of     int: x,array[$T] of     int: y) =
    let {
        array[int] of int: xx = array1d(x);
        array[int] of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$T] of var int: x,array[$T] of var int: y) =
    let {
        array[int] of var int: xx = array1d(x);
        array[int] of var int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function     bool: '!='(array[$T] of     bool: x,array[$T] of     bool: y) =
    let {
        array[int] of bool: xx = array1d(x);
        array[int] of bool: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$T] of var bool: x,array[$T] of var bool: y) =
    let {
        array[int] of var bool: xx = array1d(x);
        array[int] of var bool: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function     bool: '!='(array[$T] of     set of int: x,array[$T] of     set of int: y) =
    let {
        array[int] of set of int: xx = array1d(x);
        array[int] of set of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$T] of var set of int: x,array[$T] of var set of int: y) =
    let {
        array[int] of var set of int: xx = array1d(x);
        array[int] of var set of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function     bool: '!='(array[$T] of     float: x,array[$T] of     float: y) =
    let {
        array[int] of float: xx = array1d(x);
        array[int] of float: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );

/** @group stdlib.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$T] of var float: x,array[$T] of var float: y) =
    let {
        array[int] of var float: xx = array1d(x);
        array[int] of var float: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      exists (i in index_set(xx)) (xx[i]!=yy[i])
    );
