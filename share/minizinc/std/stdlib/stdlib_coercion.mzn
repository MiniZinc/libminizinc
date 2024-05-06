/***
  @groupdef stdlib.builtins.coercion Coercions

  These functions implement coercions, or channeling, between different types.
*/

/** @group stdlib.builtins.coercion Return \( \lceil{ \a x} \rceil \) */
function int: ceil(float: x);
/** @group stdlib.builtins.coercion Return \( \lfloor{ \a x} \rfloor \) */
function int: floor(float: x);
/** @group stdlib.builtins.coercion Return \a x rounded to nearest integer */
function int: round(float: x);

/** @group stdlib.builtins.coercion Return \( \lceil{ \a x} \rceil \) */
function var int: ceil(var float: x) ::promise_total =
  let {
    var ceil(lb(x))..ceil(ub(x)): r;
    constraint float_ceil(x, r);
  } in r;

/** @group stdlib.builtins.coercion Return \( \lfloor{ \a x} \rfloor \) */
function var int: floor(var float: x) ::promise_total =
  let {
    var floor(lb(x))..floor(ub(x)): r;
    constraint float_floor(x, r);
  } in r;

/** @group stdlib.builtins.coercion Return \a x rounded to nearest integer */
function var int: round(var float: x) ::promise_total =
  let {
    var floor(lb(x))..ceil(ub(x)): r;
    constraint float_round(x, r);
  } in r;

/** @group stdlib.builtins.coercion Return Boolean \a b coerced to an integer  */
function int: bool2int(bool: b);

/** @group stdlib.builtins.coercion Return Boolean \a b coerced to an integer  */
function opt int: bool2int(opt bool: b) =
  if occurs(b) then bool2int(deopt(b)) else <> endif;

/** @group stdlib.builtins.coercion Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt int: bool2int(var opt bool: x) ::promise_total =
  let {
    var opt 0..1: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2int(deopt(x));
  } in xi;


/** @group stdlib.builtins.coercion Return Boolean \a b coerced to a float  */
function float: bool2float(bool: b) = if b then 1.0 else 0.0 endif;
/** @group stdlib.builtins.coercion Return Boolean \a b coerced to a float  */
function opt float: bool2float(opt bool: b) =
  if occurs(b) then bool2float(deopt(b)) else <> endif;

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of float: bool2float(array[$T] of bool: x) ::promise_total =
  let {
    array[int] of bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);
/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of var float: bool2float(array[$T] of var bool: x) ::promise_total =
  let {
    array[int] of var bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);
/** @group stdlib.builtins.coercion Return Boolean \a b coerced to an integer  */
function var int: bool2int(var bool: b) ::mzn_internal_representation;
/** @group stdlib.builtins.coercion Return array of Booleans \a b coerced to an array of integers  */
function array[$T] of var int: bool2int(array[$T] of var bool: b);
/** @group stdlib.builtins.coercion Return Boolean \a b coerced to a float  */
function var float: bool2float(var bool: b) = int2float(bool2int(b));
/** @group stdlib.builtins.coercion Return integer \a x coerced to a float  */
function float: int2float(int: x);
/** @group stdlib.builtins.coercion Return integer \a x coerced to a float  */
function opt float: int2float(opt int: x) =
  if occurs(x) then int2float(deopt(x)) else <> endif;
/** @group stdlib.builtins.coercion Return integer \a x coerced to a float  */
function var float: int2float(var int: x) ::promise_total ::mzn_internal_representation;

/** @group stdlib.builtins.coercion Return optional 0/1 float that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: bool2float(var opt bool: x) ::promise_total =
  let {
    var opt 0.0..1.0: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2float(deopt(x));
  } in xi;
/** @group stdlib.builtins.coercion Return optional 0/1 integer that is absent iff \a x
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

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of int: bool2int(array[$T] of bool: x) ::promise_total =
  let {
    array[int] of bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of sets of Booleans \a x coerced to an array of sets of integers  */
function array[$T] of set of int: bool2int(array[$T] of set of bool: x) ::promise_total =
  let {
    array[int] of set of bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of var int: bool2int(array[$T] of var bool: x) ::promise_total =
  let {
    array[int] of var bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of opt int: bool2int(array[$T] of opt bool: x) =
  let {
    array[int] of opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of var opt int: bool2int(array[$T] of var opt bool: x) ::promise_total =
  let {
    array[int] of var opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of opt float: bool2float(array[$T] of opt bool: x) =
  let {
    array[int] of opt bool: xx = array1d(x)
  } in arrayXd(x, [bool2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of var opt float: bool2float(array[$T] of var opt bool: x) ::promise_total =
  let {
    array[int] of var opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of float: int2float(array[$T] of int: x) ::promise_total =
  let {
    array[int] of int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of opt float: int2float(array[$T] of opt int: x) ::promise_total =
  let {
    array[int] of opt int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of var float: int2float(array[$T] of var int: x) ::promise_total =
  let {
    array[int] of var int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.coercion Return array of optional integers \a x coerced to an array of optional floats  */
function array[$T] of var opt float: int2float(array[$T] of var opt int: x) ::promise_total =
  let {
    array[int] of var opt int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

% Only supported for set of int:
% function array[int] of $T: set2array(set of $T);
/** @group stdlib.builtins.coercion Return a set of integers \a x coerced to an array of integers  */
function array[int] of $$E: set2array(set of $$E: x);

/** @group stdlib.debug If \a x is defined and not absent, return \a x, otherwise return \a y. */
function $T: 'default'(opt $T:x, $T: y);
/** @group stdlib.debug If \a x is defined and not absent, return \a x, otherwise return \a y. */
function opt $T: 'default'(opt $T:x, opt $T: y);
/** @group stdlib.debug If \a x is defined and not absent, return \a x, otherwise return \a y. */
function var $T: 'default'(var opt $T:x, var $T: y);
/** @group stdlib.debug If \a x is defined and not absent, return \a x, otherwise return \a y. */
function var opt $T: 'default'(var opt $T:x, var opt $T: y);

/** @group stdlib.debug If \a x is defined, return \a x, otherwise return \a y. */
function array[$U] of $T: 'default'(array[$U] of $T:x, array[$U] of $T: y);
/** @group stdlib.debug If \a x is defined, return \a x, otherwise return \a y. */
function array[$U] of opt $T: 'default'(array[$U] of opt $T:x, array[$U] of opt $T: y);
/** @group stdlib.debug If \a x is defined, return \a x, otherwise return \a y. */
function array[$U] of var $T: 'default'(array[$U] of var $T:x, array[$U] of var $T: y);
/** @group stdlib.debug If \a x is defined, return \a x, otherwise return \a y. */
function array[$U] of var opt $T: 'default'(array[$U] of var opt $T:x, array[$U] of var opt $T: y);

/** @group stdlib.debug If \a x is defined, return \a x, otherwise return \a y. */
function set of $$E: 'default'(set of $$E:x, set of $$E: y);
/** @group stdlib.debug If \a x is defined, return \a x, otherwise return \a y. */
function var set of $$E: 'default'(var set of $$E:x, var set of $$E: y);

/** @group stdlib.debug If \a x is define amd mpt absent, return \a x, otherwise return \a y. */
function set of $T: 'default'(opt set of $T: x, set of $T: y);
/** @group stdlib.debug If \a x is define amd mpt absent, return \a x, otherwise return \a y. */
function opt set of $T: 'default'(opt set of $T: x, opt set of $T: y);
