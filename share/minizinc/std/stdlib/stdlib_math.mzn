/***
  @groupdef stdlib.builtins.arithmetic Arithmetic Builtins

  These builtins implement arithmetic operations.
*/

/** @group stdlib.builtins.arithmetic Return \a x + \a y */
function     int:   '+'(    int: x,     int: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return \a x + \a y */
function var int:   '+'(var int: x,  var int: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return \a x + \a y */
function     float: '+'(    float: x,    float: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return \a x + \a y */
function var float: '+'(var float: x,var float: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var int: '+'(var opt int: x, var opt int: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;
/** @group stdlib.builtins.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function int: '+'(opt int: x, opt int: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;
/** @group stdlib.builtins.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var float: '+'(var opt float: x, var opt float: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;
/** @group stdlib.builtins.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function float: '+'(opt float: x, opt float: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group stdlib.builtins.arithmetic Return \a x - \a y */
function     int:   '-'(    int: x,     int: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return \a x - \a y */
function var int:   '-'(var int: x,  var int: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return \a x - \a y */
function     float: '-'(    float: x,    float: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return \a x - \a y */
function var float: '-'(var float: x,var float: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function var opt int: '-'(var opt int: x, var opt int: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function var int: '-'(var int: x, var opt int: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;
/** @group stdlib.builtins.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function var opt float: '-'(var opt float: x, var opt float: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function var float: '-'(var float: x, var opt float: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;
/** @group stdlib.builtins.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function opt int: '-'(opt int: x, opt int: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function int: '-'(int: x, opt int: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;
/** @group stdlib.builtins.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function opt float: '-'(opt float: x, opt float: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function float: '-'(float: x, opt float: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;

/** @group stdlib.builtins.arithmetic Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~+'(var opt int: x, var opt int: y) ::promise_total ::promise_commutative =
    let { 
    int: l = if lb(x)=-infinity \/ lb(y)=-infinity then -infinity else lb(x)+lb(y) endif;
    int: u = if ub(x)=infinity \/ ub(y)=infinity then infinity else ub(x)+ub(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)+deopt(y);
    } in result;

/** @group stdlib.builtins.arithmetic Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function opt int: '~+'(opt int: x, opt int: y) ::promise_total ::promise_commutative =
  if occurs(x) /\ occurs(y) then deopt(x) + deopt(y) else <> endif;

/** @group stdlib.builtins.arithmetic Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~-'(var opt int: x, var opt int: y) ::promise_total =
    let { 
    int: l = if lb(x)=-infinity \/ ub(y)=infinity then -infinity else lb(x)-ub(y) endif;
    int: u = if ub(x)=infinity \/ lb(y)=-infinity then infinity else ub(x)-lb(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)-deopt(y);
    } in result;

/** @group stdlib.builtins.arithmetic Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function opt int: '~-'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) /\ occurs(y) then deopt(x) - deopt(y) else <> endif;

/** @group stdlib.builtins.arithmetic Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~*'(var opt int: x, var opt int: y) ::promise_total ::promise_commutative =
    if absent(x) \/ absent(y) then <>
    else deopt(x)*deopt(y) endif;

/** @group stdlib.builtins.arithmetic Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function opt int: '~*'(opt int: x, opt int: y) ::promise_total ::promise_commutative =
  if occurs(x) /\ occurs(y) then deopt(x) * deopt(y) else <> endif;

/** @group stdlib.builtins.arithmetic Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~+'(var opt float: x, var opt float: y) ::promise_total ::promise_commutative =
    let { 
    float: l = if lb(x)=-infinity \/ lb(y)=-infinity then -infinity else lb(x)+lb(y) endif;
    float: u = if ub(x)=infinity \/ ub(y)=infinity then infinity else ub(x)+ub(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)+deopt(y);
    } in result;

/** @group stdlib.builtins.arithmetic Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~-'(var opt float: x, var opt float: y) ::promise_total =
    let { 
    float: l = if lb(x)=-infinity \/ ub(y)=infinity then -infinity else lb(x)-ub(y) endif;
    float: u = if ub(x)=infinity \/ lb(y)=-infinity then infinity else ub(x)-lb(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)-deopt(y);
    } in result;

/** @group stdlib.builtins.arithmetic Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~*'(var opt float: x, var opt float: y) ::promise_total ::promise_commutative =
    if absent(x) \/ absent(y) then <>
    else deopt(x)*deopt(y) endif;

/** @group stdlib.builtins.arithmetic Return \a x * \a y */
function     int:   '*'(    int: x,     int: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return \a x * \a y */
function var int:   '*'(var int: x,  var int: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var int: '*'(var opt int: x, var opt int: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;
/** @group stdlib.builtins.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function int: '*'(opt int: x, opt int: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;
/** @group stdlib.builtins.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var float: '*'(var opt float: x, var opt float: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;
/** @group stdlib.builtins.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function float: '*'(opt float: x, opt float: y) ::promise_total ::promise_commutative =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;


/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\) */
function     int:   '^'(    int: x,     int: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\) */
function var int:   '^'(var int: x,  var int: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return \a x * \a y */
function     float: '*'(    float: x,    float: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return \a x * \a y */
function var float: '*'(var float: x,var float: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\) */
function     float: '^'(    float: x,    float: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\) */
function var float: '^'(var float: x,var float: y) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return negative \a x */
function     int:   '-'(    int: x) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return negative \a x */
function var int:   '-'(var int: x) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return negative \a x */
function     float: '-'(    float: x) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return negative \a x */
function var float: '-'(var float: x) ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return negative \a x if it occurs, otherwise return absent*/
function opt int: '-'(opt int: x) =
  if occurs(x) then -deopt(x) else <> endif;
/** @group stdlib.builtins.arithmetic Return negative \a x if it occurs, otherwise return absent*/
function var opt int: '-'(var opt int: x) =
  if occurs(x) then -deopt(x) else <> endif;
/** @group stdlib.builtins.arithmetic Return negative \a x if it occurs, otherwise return absent*/
function opt float: '-'(opt float: x) =
  if occurs(x) then -deopt(x) else <> endif;
/** @group stdlib.builtins.arithmetic Return negative \a x if it occurs, otherwise return absent*/
function var opt float: '-'(var opt float: x) =
  if occurs(x) then -deopt(x) else <> endif;

/** @group stdlib.builtins.arithmetic Return result of integer division \a x / \a y */
function     int: 'div'(int: x,int: y);

/** @group stdlib.builtins.arithmetic Return result of integer division \a x / \a y */
function var int: 'div'(var int: x,var int: y) =
  if mzn_in_root_context(y) then div_t(x,y)
  elseif not (0 in dom(y)) then div_t(x,y)
  else let { constraint y != 0 } in div_mt(x,y) endif;

/** @group stdlib.builtins.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function var opt int: 'div'(var opt int: x, var opt int: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) div deopt(y) endif;

function var int: 'div'(var int: x, var opt int: y) =
  if absent(y) then x else x div deopt(y) endif;

/** @group stdlib.builtins.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function opt int: 'div'(opt int: x, opt int: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) div deopt(y) endif;

function int: 'div'(int: x, opt int: y) =
  if absent(y) then x else x div deopt(y) endif;

/** @group stdlib.builtins.arithmetic Weak optional division. Return absent if \a x or \a y is absent,
  \a x divided by \a y if both are present. */
function var opt int: '~div'(var opt int: x, var opt int: y) =
  if absent(x) \/ absent(y) then <> else deopt(x) div deopt(y) endif;

/** @group stdlib.builtins.arithmetic Weak optional division. Return absent if \a x or \a y is absent,
  \a x divided by \a y if both are present. */
function opt int: '~div'(opt int: x, opt int: y) =
  if absent(x) \/ absent(y) then <> else deopt(x) div deopt(y) endif;

/** @group stdlib.builtins.arithmetic Return remainder of integer division \a x % \a y */
function     int: 'mod'(int: x,int: y);

/** @group stdlib.builtins.arithmetic Return remainder of integer division \a x % \a y */
function var int: 'mod'(var int: x,var int: y) =
  if mzn_in_root_context(y) then mod_t(x,y)
  elseif not (0 in dom(y)) then mod_t(x,y)
  else let { constraint y != 0 } in mod_mt(x,y) endif;

/** @group stdlib.builtins.arithmetic Optional modulo. Return absent if \a x or \a y is absent,
  \a x modulo \a y if both are present. */
function var opt int: 'mod'(var opt int: x, var opt int: y) =
  if occurs(x) /\ occurs(y) then deopt(x) mod deopt(y) else <> endif;

/** @group stdlib.builtins.arithmetic Optional modulo. Return absent if \a x or \a y is absent,
  \a x modulo \a y if both are present. */
function opt int: 'mod'(opt int: x, opt int: y) =
  if occurs(x) /\ occurs(y) then deopt(x) mod deopt(y) else <> endif;


/** @group stdlib.builtins.arithmetic Return result of floating point division \a x / \a y */
function     float: '/'(    float: x,    float: y);
/** @group stdlib.builtins.arithmetic Return result of floating point division \a x / \a y */
function var float: '/'(var float: x,var float: y) =
  if mzn_in_root_context(y) then fldiv_t(x,y)
  elseif lb(y) > 0.0 \/ ub(y) < 0.0 then fldiv_t(x,y)
  else let { constraint y != 0.0 } in fldiv_mt(x,y) endif;
/** @group stdlib.builtins.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function var opt float: '/'(var opt float: x, var opt float: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) / deopt(y) endif;
function var float: '/'(var float: x, var opt float: y) =
  if absent(y) then x else x / deopt(y) endif;
/** @group stdlib.builtins.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function opt float: '/'(opt float: x, opt float: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) / deopt(y) endif;
function float: '/'(float: x, opt float: y) =
  if absent(y) then x else x / deopt(y) endif;

/** @group stdlib.builtins.arithmetic Weal optional division. Return absent if \a x or \a y is absent,
  \a x divided by \a y if both are present. */
function var opt float: '~/'(var opt float: x, var opt float: y) =
  if absent(x) \/ absent(y) then <> else deopt(x) / deopt(y) endif;

/** @group stdlib.builtins.arithmetic Weal optional division. Return absent if \a x or \a y is absent,
  \a x divided by \a y if both are present. */
function opt float: '~/'(opt float: x, opt float: y) =
  if absent(x) \/ absent(y) then <> else deopt(x) / deopt(y) endif;

/** @group stdlib.builtins.arithmetic Return number of true elments in array \a x */
function int:  count(array[$T] of bool: x) ::promise_commutative = sum([bool2int(y) | y in array1d(x)]);
/** @group stdlib.builtins.arithmetic Return number of true elments in array \a x */
function var int:  count(array[$T] of var bool: x :: promise_ctx_monotone) ::promise_commutative = let {
  array[int] of var bool: xx :: promise_ctx_monotone = array1d(x);
} in sum([bool2int(y) | y in xx]);
/** @group stdlib.builtins.arithmetic Return number of true elments in array \a x */
function var int:  count(array[$T] of var opt bool: x :: promise_ctx_monotone) ::promise_commutative =
  count(i in x)(i default false);

/** @group stdlib.builtins.arithmetic Return sum of elements in array \a x */
function     int:   sum(array[$T] of     int: x) ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return sum of elements in array \a x */
function var int:   sum(array[$T] of var int: x) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return sum of elements in array \a x */
function     float: sum(array[$T] of     float: x) ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return sum of elements in array \a x */
function var float: sum(array[$T] of var float: x) ::promise_commutative ::mzn_internal_representation;
/** @group stdlib.builtins.arithmetic Return sum of non-absent elements of \a x. */
function var int: sum(array[int] of var opt int: x) ::promise_commutative =
  sum (i in index_set(x)) (x[i] default 0);
/** @group stdlib.builtins.arithmetic Return sum of non-absent elements of \a x. */
function int: sum(array[int] of opt int: x) ::promise_commutative =
  sum (i in index_set(x)) (x[i] default 0);
/** @group stdlib.builtins.arithmetic Return sum of non-absent elements of \a x. */
function var float: sum(array[int] of var opt float: x) ::promise_commutative =
  sum (i in index_set(x)) (x[i] default 0.0);
/** @group stdlib.builtins.arithmetic Return sum of non-absent elements of \a x. */
function float: sum(array[int] of opt float: x) ::promise_commutative =
  sum (i in index_set(x)) (x[i] default 0.0);


/** @group stdlib.builtins.arithmetic Return product of elements in array \a x */
function     int:   product(array[$T] of     int: x) ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return product of elements in array \a x */
function var int:   product(array[$T] of var int: x) ::promise_commutative =
  product_rec(array1d(x));
/** @group stdlib.builtins.arithmetic Return product of elements in array \a x */
function     float: product(array[$T] of     float: x) ::promise_commutative;
/** @group stdlib.builtins.arithmetic Return product of elements in array \a x */
function var float: product(array[$T] of var float: x) ::promise_commutative =
  product_rec(array1d(x));
/** @group stdlib.builtins.arithmetic Return product of non-absent elements of \a x. */
function int: product(array[$T] of opt int: x) ::promise_commutative =
  product (xi in x) (xi default 1);
/** @group stdlib.builtins.arithmetic Return product of non-absent elements of \a x. */
function var int: product(array[$T] of var opt int: x) ::promise_commutative =
  product (xi in x) (xi default 1);
/** @group stdlib.builtins.arithmetic Return product of non-absent elements of \a x. */
function float: product(array[$T] of opt float: x) ::promise_commutative =
  product (xi in x) (xi default 1.0);
/** @group stdlib.builtins.arithmetic Return product of non-absent elements of \a x. */
function var float: product(array[$T] of var opt float: x) ::promise_commutative =
  product (xi in x) (xi default 1.0);


/** @group stdlib.builtins.arithmetic Return minimum of \a x and \a y */
function     $T: min(    $T: x,     $T: y) ::promise_commutative;

/** @group stdlib.builtins.arithmetic Return minimum of \a x and \a y */
function var int: min(var int: x, var int: y) :: promise_total :: promise_commutative =
  let { var min(lb(x),lb(y))..min(ub(x),ub(y)): m ::is_defined_var;
        constraint int_min(x,y,m) ::defines_var(m);
  } in m;

/** @group stdlib.builtins.arithmetic Return minimum of \a x and \a y */
function var float: min(var float: x, var float: y) :: promise_total :: promise_commutative =
  if has_bounds(x) /\ has_bounds(y) then
  let { var min(lb(x),lb(y))..min(ub(x),ub(y)): m ::is_defined_var;
        constraint float_min(x,y,m) ::defines_var(m);
  } in m
  else
  let { var float: m ::is_defined_var;
        constraint float_min(x,y,m) ::defines_var(m);
  } in m
  endif;

/** @group stdlib.builtins.arithmetic Return minimum of elements in array \a x */
function     $T: min(array[$U] of     par $T: x) :: promise_commutative;

/** @group stdlib.builtins.arithmetic Return minimum of elements in array \a x */
function var int: min(array[$U] of var int: x) :: promise_commutative =
  let {
    array[int] of var int: xx = array1d(x);
    constraint length(x) >= 1;
  } in min_t(xx);

/** @group stdlib.builtins.arithmetic Return minimum of elements in array \a x */
function var float: min(array[$U] of var float: x) :: promise_commutative =
  let {
    array[int] of var float: xx = array1d(x);
    constraint length(x) >= 1;
  } in min_t(xx);

/** @group stdlib.builtins.arithmetic Return minimum of elements in \a x that are not absent.
    Result is undefined when all values are absent. */
function var int: min(array[$X] of var opt int: x) :: promise_commutative =
  let {
    int: xub = max(0, ub_array(x));
    constraint exists (xi in x) (occurs(xi));
  } in min (xi in x) (xi default xub);

/** @group stdlib.builtins.arithmetic Return minimum of elements in \a x that are not absent.
    Result is absent when all values are absent. */
function var opt int: min_weak(array[$X] of var opt int: x) ::promise_commutative =
  let {
    int: xub = max(0, ub_array(x));
  } in if exists (xi in x) (occurs(xi)) then min (xi in x) (xi default xub) else <> endif;

/** @group stdlib.builtins.arithmetic Return minimum of elements in \a x that are not absent.
    Result is undefined when all values are absent. */
function var float: min(array[$X] of var opt float: x) ::promise_commutative =
  let {
    float: xub = max(0.0, ub_array(x));
    constraint exists (xi in x) (occurs(xi));
  } in min (xi in x) (xi default xub);

/** @group stdlib.builtins.arithmetic Return minimum of elements in \a x that are not absent.
    Result is absent when all values are absent. */
function var opt float: min_weak(array[$X] of var opt float: x) :: promise_commutative =
  let {
    float: xub = max(0.0, ub_array(x));
  } in if exists (xi in x) (occurs(xi)) then min (xi in x) (xi default xub) else <> endif;

/** @group stdlib.builtins.arithmetic Return minimum of elements in set \a x */
function $$E: min(set of $$E: x);

/** @group stdlib.builtins.arithmetic Return maximum of \a x and \a y */
function     $T: max(    $T: x,     $T: y) :: promise_commutative;

/** @group stdlib.builtins.arithmetic Return maximum of \a x and \a y */
function var int: max(var int: x, var int: y) :: promise_total :: promise_commutative =
  let { var max(lb(x),lb(y))..max(ub(x),ub(y)): m ::is_defined_var;
        constraint int_max(x,y,m) ::defines_var(m);
  } in m;

/** @group stdlib.builtins.arithmetic Return maximum of \a x and \a y */
function var float: max(var float: x, var float: y) :: promise_total :: promise_commutative =
  if has_bounds(x) /\ has_bounds(y) then
  let { var max(lb(x),lb(y))..max(ub(x),ub(y)): m ::is_defined_var;
        constraint float_max(x,y,m) ::defines_var(m);
  } in m
  else
  let { var float: m ::is_defined_var;
        constraint float_max(x,y,m) ::defines_var(m);
  } in m
  endif;

/** @group stdlib.builtins.arithmetic Return maximum of elements in array \a x */
function     $T: max(array[$U] of     $T: x) :: promise_commutative;

/** @group stdlib.builtins.arithmetic Return maximum of elements in array \a x */
function var int: max(array[$U] of var int: x) :: promise_commutative =
  let {
    array[int] of var int: xx = array1d(x);
    constraint length(x) >= 1;
  } in max_t(xx);

/** @group stdlib.builtins.arithmetic Return maximum of elements in array \a x */
function var float: max(array[$U] of var float: x) :: promise_commutative =
  let {
    array[int] of var float: xx = array1d(x);
    constraint length(x) >= 1;
  } in max_t(xx);

/** @group stdlib.builtins.arithmetic Return maximum of elements in \a x that are not absent.
    Result is undefined when all values are absent. */
function var int: max(array[$X] of var opt int: x) :: promise_commutative =
  let {
    int: xlb = min(0, lb_array(x));
    constraint exists (xi in x) (occurs(xi));
  } in max (xi in x) (xi default xlb);

/** @group stdlib.builtins.arithmetic Return maximum of elements in \a x that are not absent.
    Result is absent when all values are absent. */
function var opt int: max_weak(array[$X] of var opt int: x) :: promise_commutative =
  let {
    int: xlb = min(0, lb_array(x));
  } in if exists (xi in x) (occurs(xi)) then max (xi in x) (xi default xlb) else <> endif;

/** @group stdlib.builtins.arithmetic Return maximum of elements in \a x that are not absent.
    Result is undefined when all values are absent. */
function var float: max(array[$X] of var opt float: x) :: promise_commutative =
  let {
    float: xlb = min(0.0, lb_array(x));
    constraint exists (xi in x) (occurs(xi));
  } in max (xi in x) (xi default xlb);

/** @group stdlib.builtins.arithmetic Return maximum of elements in \a x that are not absent.
    Result is absent when all values are absent. */
function var opt float: max_weak(array[$X] of var opt float: x) :: promise_commutative =
  let {
    float: xlb = min(0.0, lb_array(x));
  } in if exists (xi in x) (occurs(xi)) then max (xi in x) (xi default xlb) else <> endif;

/** @group stdlib.builtins.arithmetic Return maximum of elements in set \a x */
function $$E: max(set of $$E: x);

% Floating point min and max
% TODO: add bounds reasoning

/** @group stdlib.builtins.arithmetic
    Returns the index of the minimum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_min(array[$$E] of bool: x);
/** @group stdlib.builtins.arithmetic
    Returns the index of the minimum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_min(array[$$E] of int: x);
/** @group stdlib.builtins.arithmetic
    Returns the index of the minimum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_min(array[$$E] of float: x);
/** @group stdlib.builtins.arithmetic
    Returns the index of the maximum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_max(array[$$E] of bool: x);
/** @group stdlib.builtins.arithmetic
    Returns the index of the maximum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_max(array[$$E] of int: x);
/** @group stdlib.builtins.arithmetic
    Returns the index of the maximum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_max(array[$$E] of float: x);

/** @group stdlib.builtins.arithmetic Return absolute value of \a x */
function int: abs(int: x);

/** @group stdlib.builtins.arithmetic Return absolute value of \a x */
function var int: abs(var int: x) :: promise_total =
  if has_bounds(x) /\ lb(x) >= 0 then x
  elseif has_bounds(x) /\ ub(x) <= 0 then -x else
  let { var 0..max(-lb(x),ub(x)): m ::is_defined_var;
        constraint int_abs(x,m) ::defines_var(m);
  } in m
  endif;

/** @group stdlib.builtins.arithmetic Return absolute value of \a x if it occurs, otherwise return absent */
function opt int: abs(opt int: x) =
  if occurs(x) then abs(deopt(x)) else <> endif;

/** @group stdlib.builtins.arithmetic Return absolute value of \a x if it occurs, otherwise return absent */
function var opt int: abs(var opt int: x) =
  if occurs(x) then abs(deopt(x)) else <> endif;

/** @group stdlib.builtins.arithmetic Return absolute value of \a x */
function float: abs(float: x);
/** @group stdlib.builtins.arithmetic Return absolute value of \a x */
function var float: abs(var float: x) :: promise_total =
  if has_bounds(x) then
    if lb(x) >= 0.0 then x
    elseif ub(x) <= 0.0 then -x else
    let { var 0.0..max(-lb(x),ub(x)): m ::is_defined_var;
          constraint float_abs(x,m) ::defines_var(m);
    } in m
    endif
  else
    let { var float: m ::is_defined_var;
          constraint m >= 0.0;
          constraint float_abs(x,m) ::defines_var(m);
    } in m
  endif;
/** @group stdlib.builtins.arithmetic Return absolute value of \a x if it occurs, otherwise return absent */
function opt float: abs(opt float: x) =
  if occurs(x) then abs(deopt(x)) else <> endif;
/** @group stdlib.builtins.arithmetic Return absolute value of \a x if it occurs, otherwise return absent */
function var opt float: abs(var opt float: x) =
  if occurs(x) then abs(deopt(x)) else <> endif;

/** @group stdlib.builtins.arithmetic Return \(\sqrt{\a x}\) */
function float: sqrt(float: x);
/** @group stdlib.builtins.arithmetic Return \(\sqrt{\a x}\) */
function var float: sqrt(var float: x) =
  if mzn_in_root_context(x) then sqrt_t(x)
  elseif lb(x) >= 0.0 then sqrt_t(x)
  else let { constraint x >= 0.0; } in sqrt_mt(x)
  endif;
/** @group stdlib.builtins.arithmetic Return \(\sqrt{\a x}\) if \a x occurs, otherwise return absent */
function opt float: sqrt(opt float: x) =
  if occurs(x) then sqrt(deopt(x)) else <> endif;
/** @group stdlib.builtins.arithmetic Return \(\sqrt{\a x}\) if \a x occurs, otherwise return absent */
function var opt float: sqrt(var opt float: x) =
  if occurs(x) then sqrt(deopt(x)) else <> endif;

/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\), when \(\a y < 0 \) it returns ``1 div pow(x, abs(y))``. */
function int: pow(int: x, int: y);

/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\), when \(\a y < 0 \) it returns ``1 div pow(x, abs(y))``. */
function var int: pow(var int: x, int: y) =
  if y = 0 then
    1
  elseif y = 1 then
    x
  elseif dom(x) = 0..1 /\ y >= 0 then
    x
  elseif dom(x) = -1..1 /\ y mod 2 = 1 then
    x
  elseif not (0 in dom(x) \/ -1 in dom(x)) /\ y < 0 then
    bool2int(x = 1)
  elseif y >= 0 \/ not (0 in dom(x)) then
    pow_fixed_t(x, y)
  elseif mzn_in_root_context(x) then
    pow_fixed_t(x, y)
  else 
    let {
      constraint x != 0;
    } in pow_fixed_mt(x, y)
  endif;

/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\), when \(\a y < 0 \) it returns ``1 div pow(x, abs(y))``. */
function var int: pow(var int: x, var int: y) =
  if lb(y) > 0 /\ dom(x) = 0..1 then
    x
  elseif lb(y) >= 0 then
    pow_t(x, y)
  elseif not (0 in dom(x)) then
    if ub(y) < 0 /\ not (-1 in dom(x)) then
      bool2int(x = 1)
    else
      pow_t(x, y)
    endif
  elseif mzn_in_root_context(x) then
    pow_t(x, y)
  else
    let {
      constraint y < 0 -> x != 0; 
    } in pow_mt(x, y)
  endif;


/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\) */
function float: pow(float: x, float: y);

/** @group stdlib.builtins.arithmetic Return \(\a x ^ {\a y}\) */
function var float: pow(var float: x, var float: y) ::promise_total =
  let {
    float: yy = if is_fixed(y) then fix(y) else -1.0 endif
  } in
  if yy = 0.0 then 1.0
  elseif yy = 1.0 then x else
  let { var float: r ::is_defined_var;
        constraint float_pow(x,y,r) ::defines_var(r);
  } in r
  endif;

/***
  @groupdef stdlib.builtins.explog Exponential and logarithmic builtins

  These builtins implement exponential and logarithmic functions.
*/

/** @group stdlib.builtins.explog Return \(e ^ {\a x}\) */
function float: exp(float: x);
/** @group stdlib.builtins.explog Return \(e ^ {\a x}\) */
function var float: exp(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_exp(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.explog Return \(e ^ {\a x}\) if \a x occurs, otherwise return absent */
function opt float: exp(opt float: x) =
  if occurs(x) then exp(deopt(x)) else <> endif;
/** @group stdlib.builtins.explog Return \(e ^ {\a x}\) if \a x occurs, otherwise return absent */
function var opt float: exp(var opt float: x) =
  if occurs(x) then exp(deopt(x)) else <> endif;

/** @group stdlib.builtins.explog Return \(\ln \a x\) */
function float: ln(float: x);
/** @group stdlib.builtins.explog Return \(\ln \a x\) */
function var float: ln(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_ln(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.explog Return \(\ln \a x\) if \a x occurs, otherwise return absent */
function opt float: ln(opt float: x) =
  if occurs(x) then ln(deopt(x)) else <> endif;
/** @group stdlib.builtins.explog Return \(\ln \a x\) if \a x occurs, otherwise return absent */
function var opt float: ln(var opt float: x) =
  if occurs(x) then ln(deopt(x)) else <> endif;

/** @group stdlib.builtins.explog Return \(\log_{10} \a x\) */
function float: log10(float: x);
/** @group stdlib.builtins.explog Return \(\log_{10} \a x\) */
function var float: log10(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_log10(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.explog Return \(\log_{10} \a x\) if \a x occurs, otherwise return absent */
function opt float: log10(opt float: x) =
  if occurs(x) then log10(deopt(x)) else <> endif;
/** @group stdlib.builtins.explog Return \(\log_{10} \a x\) if \a x occurs, otherwise return absent */
function var opt float: log10(var opt float: x) =
  if occurs(x) then log10(deopt(x)) else <> endif;

/** @group stdlib.builtins.explog Return \(\log_{2} \a x\) */
function float: log2(float: x);
/** @group stdlib.builtins.explog Return \(\log_{2} \a x\) */
function var float: log2(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_log2(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.explog Return \(\log_{2} \a x\) if \a x occurs, otherwise return absent */
function opt float: log2(opt float: x) =
  if occurs(x) then log2(deopt(x)) else <> endif;
/** @group stdlib.builtins.explog Return \(\log_{2} \a x\) if \a x occurs, otherwise return absent */
function var opt float: log2(var opt float: x) =
  if occurs(x) then log2(deopt(x)) else <> endif;

/** @group stdlib.builtins.explog Return \(\log_{\a x} \a y\) */
function float: log(float: x, float: y);

/***
  @groupdef stdlib.builtins.trigonometric Trigonometric functions

  These builtins implement the standard trigonometric functions.
*/

/** @group stdlib.builtins.trigonometric Return \(\sin \a x\) */
function float: sin(float: x);
/** @group stdlib.builtins.trigonometric Return \(\sin \a x\) */
function var float: sin(var float: x) ::promise_total =
  let {
    var -1.0..1.0: r ::is_defined_var;
    constraint float_sin(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\sin \a x\) if \a x occurs, otherwise return absent */
function opt float: sin(opt float: x) =
  if occurs(x) then sin(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\sin \a x\) if \a x occurs, otherwise return absent */
function var opt float: sin(var opt float: x) =
  if occurs(x) then sin(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\cos \a x\) */
function float: cos(float: x);
/** @group stdlib.builtins.trigonometric Return \(\cos \a x\) */
function var float: cos(var float: x) ::promise_total =
  let {
    var -1.0..1.0: r ::is_defined_var;
    constraint float_cos(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\cos \a x\) if \a x occurs, otherwise return absent */
function opt float: cos(opt float: x) =
  if occurs(x) then cos(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\cos \a x\) if \a x occurs, otherwise return absent */
function var opt float: cos(var opt float: x) =
  if occurs(x) then cos(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\tan \a x\) */
function float: tan(float: x);
/** @group stdlib.builtins.trigonometric Return \(\tan \a x\) */
function var float: tan(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_tan(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\tan \a x\) if \a x occurs, otherwise return absent */
function opt float: tan(opt float: x) =
  if occurs(x) then tan(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\tan \a x\) if \a x occurs, otherwise return absent */
function var opt float: tan(var opt float: x) =
  if occurs(x) then tan(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\mbox{asin}\ \a x\) */
function float: asin(float: x);
/** @group stdlib.builtins.trigonometric Return \(\mbox{asin}\ \a x\) */
function var float: asin(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_asin(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\mbox{asin}\ \a x\) if \a x occurs, otherwise return absent */
function opt float: asin(opt float: x) =
  if occurs(x) then asin(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\mbox{asin}\ \a x\) if \a x occurs, otherwise return absent */
function var opt float: asin(var opt float: x) =
  if occurs(x) then asin(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\mbox{acos}\ \a x\) */
function float: acos(float: x);
/** @group stdlib.builtins.trigonometric Return \(\mbox{acos}\ \a x\) */
function var float: acos(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_acos(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\mbox{acos}\ \a x\) if \a x occurs, otherwise return absent */
function opt float: acos(opt float: x) =
  if occurs(x) then acos(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\mbox{acos}\ \a x\) if \a x occurs, otherwise return absent */
function var opt float: acos(var opt float: x) =
  if occurs(x) then acos(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\mbox{atan}\ \a x\) */
function float: atan(float: x);
/** @group stdlib.builtins.trigonometric Return \(\mbox{atan}\ \a x\) */
function var float: atan(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_atan(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\mbox{atan}\ \a x\) if \a x occurs, otherwise return absent */
function opt float: atan(opt float: x) =
  if occurs(x) then atan(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\mbox{atan}\ \a x\) if \a x occurs, otherwise return absent */
function var opt float: atan(var opt float: x) =
  if occurs(x) then atan(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\sinh \a x\) */
function float: sinh(float: x);
/** @group stdlib.builtins.trigonometric Return \(\sinh \a x\) */
function var float: sinh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_sinh(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\sinh \a x\) if \a x occurs, otherwise return absent */
function opt float: sinh(opt float: x) =
  if occurs(x) then sinh(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\sinh \a x\) if \a x occurs, otherwise return absent */
function var opt float: sinh(var opt float: x) =
  if occurs(x) then sinh(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\cosh \a x\) */
function float: cosh(float: x);
/** @group stdlib.builtins.trigonometric Return \(\cosh \a x\) */
function var float: cosh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_cosh(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\cosh \a x\) if \a x occurs, otherwise return absent */
function opt float: cosh(opt float: x) =
  if occurs(x) then cosh(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\cosh \a x\) if \a x occurs, otherwise return absent */
function var opt float: cosh(var opt float: x) =
  if occurs(x) then cosh(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\tanh \a x\) */
function float: tanh(float: x);
/** @group stdlib.builtins.trigonometric Return \(\tanh \a x\) */
function var float: tanh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_tanh(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\tanh \a x\) if \a x occurs, otherwise return absent */
function opt float: tanh(opt float: x) =
  if occurs(x) then tanh(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\tanh \a x\) if \a x occurs, otherwise return absent */
function var opt float: tanh(var opt float: x) =
  if occurs(x) then tanh(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\mbox{asinh}\ \a x\) */
function float: asinh(float: x);
/** @group stdlib.builtins.trigonometric Return \(\mbox{asinh}\ \a x\) */
function var float: asinh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_asinh(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\mbox{asinh}\ \a x\) if \a x occurs, otherwise return absent */
function opt float: asinh(opt float: x) =
  if occurs(x) then asinh(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\mbox{asinh}\ \a x\) if \a x occurs, otherwise return absent */
function var opt float: asinh(var opt float: x) =
  if occurs(x) then asinh(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\mbox{acosh}\ \a x\) */
function float: acosh(float: x);
/** @group stdlib.builtins.trigonometric Return \(\mbox{acosh}\ \a x\) */
function var float: acosh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_acosh(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\mbox{acosh}\ \a x\) if \a x occurs, otherwise return absent */
function opt float: acosh(opt float: x) = 
  if occurs(x) then acosh(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\mbox{acosh}\ \a x\) if \a x occurs, otherwise return absent*/
function var opt float: acosh(var opt float: x) =
  if occurs(x) then acosh(deopt(x)) else <> endif;

/** @group stdlib.builtins.trigonometric Return \(\mbox{atanh}\ \a x\) */
function float: atanh(float: x);
/** @group stdlib.builtins.trigonometric Return \(\mbox{atanh}\ \a x\) */
function var float: atanh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_atanh(x,r) ::defines_var(r);
  } in r;
/** @group stdlib.builtins.trigonometric Return \(\mbox{atanh}\ \a x\) if \a x occurs, otherwise return absent */
function opt float: atanh(opt float: x) =
  if occurs(x) then atanh(deopt(x)) else <> endif;
/** @group stdlib.builtins.trigonometric Return \(\mbox{atanh}\ \a x\) if \a x occurs, otherwise return absent */
function var opt float: atanh(var opt float: x) =
  if occurs(x) then atanh(deopt(x)) else <> endif;
