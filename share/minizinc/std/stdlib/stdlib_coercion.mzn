/***
  @groupdef stdlib.coercion Coercions

  These functions implement coercions, or channeling, between different types.
*/

/** @group stdlib.coercion Return \( \lceil{ \a x} \rceil \) */
function int: ceil(float: x);
/** @group stdlib.coercion Return \( \lfloor{ \a x} \rfloor \) */
function int: floor(float: x);
/** @group stdlib.coercion Return \a x rounded to nearest integer */
function int: round(float: x);

/** @group stdlib.coercion Return Boolean \a b coerced to an integer  */
function int: bool2int(bool: b);

/** @group stdlib.coercion Return Boolean \a b coerced to an integer  */
function opt int: bool2int(opt bool: b) =
  if occurs(b) then bool2int(deopt(b)) else <> endif;

/** @group stdlib.coercion Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt int: bool2int(var opt bool: x) ::promise_total =
  let {
    var opt 0..1: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2int(deopt(x));
  } in xi;


/** @group stdlib.coercion Return Boolean \a b coerced to a float  */
function float: bool2float(bool: b) = if b then 1.0 else 0.0 endif;
/** @group stdlib.coercion Return Boolean \a b coerced to a float  */
function opt float: bool2float(opt bool: b) =
  if occurs(b) then bool2float(deopt(b)) else <> endif;

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of float: bool2float(array[$T] of bool: x) ::promise_total =
  let {
    array[int] of bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);
/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of var float: bool2float(array[$T] of var bool: x) ::promise_total =
  let {
    array[int] of var bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);
/** @group stdlib.coercion Return Boolean \a b coerced to an integer  */
function var int: bool2int(var bool: b);
/** @group stdlib.coercion Return array of Booleans \a b coerced to an array of integers  */
function array[$T] of var int: bool2int(array[$T] of var bool: b);
/** @group stdlib.coercion Return Boolean \a b coerced to a float  */
function var float: bool2float(var bool: b) = int2float(bool2int(b));
/** @group stdlib.coercion Return integer \a x coerced to a float  */
function float: int2float(int: x);
/** @group stdlib.coercion Return integer \a x coerced to a float  */
function opt float: int2float(opt int: x) =
  if occurs(x) then int2float(deopt(x)) else <> endif;
/** @group stdlib.coercion Return integer \a x coerced to a float  */
function var float: int2float(var int: x) ::promise_total;

/** @group stdlib.arithmetic Return optional 0/1 float that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: bool2float(var opt bool: x) ::promise_total =
  let {
    var opt 0.0..1.0: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2float(deopt(x));
  } in xi;
/** @group stdlib.arithmetic Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: int2float(var opt int: x) ::promise_total =
  let {
    var opt int2float(lb(x))..int2float(ub(x)): xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=int2float(deopt(x));
  } in xi;

function set of int: bool2int(set of bool: b) =
  if b={false,true} then {0,1}
  elseif b={false} then {0}
  elseif b={true} then {1}
  else {} endif;

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of int: bool2int(array[$T] of bool: x) ::promise_total =
  let {
    array[int] of bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of sets of Booleans \a x coerced to an array of sets of integers  */
function array[$T] of set of int: bool2int(array[$T] of set of bool: x) ::promise_total =
  let {
    array[int] of set of bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of var int: bool2int(array[$T] of var bool: x) ::promise_total =
  let {
    array[int] of var bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of opt int: bool2int(array[$T] of opt bool: x) =
  let {
    array[int] of opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of var opt int: bool2int(array[$T] of var opt bool: x) ::promise_total =
  let {
    array[int] of var opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of opt float: bool2float(array[$T] of opt bool: x) =
  let {
    array[int] of opt bool: xx = array1d(x)
  } in arrayXd(x, [bool2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of var opt float: bool2float(array[$T] of var opt bool: x) ::promise_total =
  let {
    array[int] of var opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of float: int2float(array[$T] of int: x) ::promise_total =
  let {
    array[int] of int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of opt float: int2float(array[$T] of opt int: x) ::promise_total =
  let {
    array[int] of opt int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of var float: int2float(array[$T] of var int: x) ::promise_total =
  let {
    array[int] of var int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.coercion Return array of optional integers \a x coerced to an array of optional floats  */
function array[$T] of var opt float: int2float(array[$T] of var opt int: x) ::promise_total =
  let {
    array[int] of var opt int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

% Only supported for set of int:
% function array[int] of $T: set2array(set of $T);
/** @group stdlib.coercion Return a set of integers \a x coerced to an array of integers  */
function array[int] of $$E: set2array(set of $$E: x);

