/***
  @groupdef stdlib.arithmetic Arithmetic Builtins

  These builtins implement arithmetic operations.
*/

/** @group stdlib.arithmetic Return \a x + \a y */
function     int:   '+'(    int: x,     int: y);
/** @group stdlib.arithmetic Return \a x + \a y */
function var int:   '+'(var int: x,  var int: y);
/** @group stdlib.arithmetic Return \a x + \a y */
function     float: '+'(    float: x,    float: y);
/** @group stdlib.arithmetic Return \a x + \a y */
function var float: '+'(var float: x,var float: y);
/** @group stdlib.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var int: '+'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;
/** @group stdlib.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function int: '+'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;
/** @group stdlib.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var float: '+'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;
/** @group stdlib.arithmetic Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function float: '+'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group stdlib.arithmetic Return \a x - \a y */
function     int:   '-'(    int: x,     int: y);
/** @group stdlib.arithmetic Return \a x - \a y */
function var int:   '-'(var int: x,  var int: y);
/** @group stdlib.arithmetic Return \a x - \a y */
function     float: '-'(    float: x,    float: y);
/** @group stdlib.arithmetic Return \a x - \a y */
function var float: '-'(var float: x,var float: y);
/** @group stdlib.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function var opt int: '-'(var opt int: x, var opt int: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function var int: '-'(var int: x, var opt int: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;
/** @group stdlib.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function var opt float: '-'(var opt float: x, var opt float: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function var float: '-'(var float: x, var opt float: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;
/** @group stdlib.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function opt int: '-'(opt int: x, opt int: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function int: '-'(int: x, opt int: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;
/** @group stdlib.arithmetic Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function opt float: '-'(opt float: x, opt float: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;
function float: '-'(float: x, opt float: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;


/** @group stdlib.arithmetic Return \a x * \a y */
function     int:   '*'(    int: x,     int: y);
/** @group stdlib.arithmetic Return \a x * \a y */
function var int:   '*'(var int: x,  var int: y);
/** @group stdlib.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var int: '*'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;
/** @group stdlib.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function int: '*'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;
/** @group stdlib.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var float: '*'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;
/** @group stdlib.arithmetic Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function float: '*'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;


/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function     int:   '^'(    int: x,     int: y);
/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function var int:   '^'(var int: x,  var int: y);
/** @group stdlib.arithmetic Return \a x * \a y */
function     float: '*'(    float: x,    float: y);
/** @group stdlib.arithmetic Return \a x * \a y */
function var float: '*'(var float: x,var float: y);
/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function     float: '^'(    float: x,    float: y);
/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function var float: '^'(var float: x,var float: y);
/** @group stdlib.arithmetic Return negative \a x */
function     int:   '-'(    int: x);
/** @group stdlib.arithmetic Return negative \a x */
function var int:   '-'(var int: x);
/** @group stdlib.arithmetic Return negative \a x */
function     float: '-'(    float: x);
/** @group stdlib.arithmetic Return negative \a x */
function var float: '-'(var float: x);

/** @group stdlib.arithmetic Return result of integer division \a x / \a y */
function     int: 'div'(int: x,int: y);

/** @group stdlib.arithmetic Return result of integer division \a x / \a y */
function var int: 'div'(var int: x,var int: y) =
  if mzn_in_root_context(y) then div_t(x,y)
  elseif not (0 in dom(y)) then div_t(x,y)
  else let { constraint y != 0 } in div_mt(x,y) endif;

/** @group stdlib.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function var opt int: 'div'(var opt int: x, var opt int: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) div deopt(y) endif;

function var int: 'div'(var int: x, var opt int: y) =
  if absent(y) then x else x div deopt(y) endif;

/** @group stdlib.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function opt int: 'div'(opt int: x, opt int: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) div deopt(y) endif;

function int: 'div'(int: x, opt int: y) =
  if absent(y) then x else x div deopt(y) endif;
/** @group stdlib.arithmetic Return remainder of integer division \a x % \a y */
function     int: 'mod'(int: x,int: y);

/** @group stdlib.arithmetic Return remainder of integer division \a x % \a y */
function var int: 'mod'(var int: x,var int: y) =
  if mzn_in_root_context(y) then mod_t(x,y)
  elseif not (0 in dom(y)) then mod_t(x,y)
  else let { constraint y != 0 } in mod_mt(x,y) endif;

/** @group stdlib.arithmetic Optional modulo. Return absent if \a x or \a y is absent,
  \a x modulo \a y if both are present. */
function var opt int: 'mod'(var opt int: x, var opt int: y) =
  if occurs(x) /\ occurs(y) then deopt(x) mod deopt(y) else <> endif;

/** @group stdlib.arithmetic Optional modulo. Return absent if \a x or \a y is absent,
  \a x modulo \a y if both are present. */
function opt int: 'mod'(opt int: x, opt int: y) =
  if occurs(x) /\ occurs(y) then deopt(x) mod deopt(y) else <> endif;


/** @group stdlib.arithmetic Return result of floating point division \a x / \a y */
function     float: '/'(    float: x,    float: y);
/** @group stdlib.arithmetic Return result of floating point division \a x / \a y */
function var float: '/'(var float: x,var float: y) =
  if mzn_in_root_context(y) then fldiv_t(x,y)
  elseif lb(y) > 0.0 \/ ub(y) < 0.0 then fldiv_t(x,y)
  else let { constraint y != 0.0 } in fldiv_mt(x,y) endif;
/** @group stdlib.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function var opt float: '/'(var opt float: x, var opt float: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) / deopt(y) endif;
function var float: '/'(var float: x, var opt float: y) =
  if absent(y) then x else x / deopt(y) endif;
/** @group stdlib.arithmetic Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function opt float: '/'(opt float: x, opt float: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) / deopt(y) endif;
function float: '/'(float: x, opt float: y) =
  if absent(y) then x else x / deopt(y) endif;


/** @group stdlib.arithmetic Return number of true elments in array \a x */
function int:  count(array[$T] of bool: x) = sum([bool2int(y) | y in array1d(x)]);
/** @group stdlib.arithmetic Return number of true elments in array \a x */
function var int:  count(array[$T] of var bool: x) = sum([bool2int(y) | y in array1d(x)]);

/** @group stdlib.arithmetic Return sum of elements in array \a x */
function     int:   sum(array[$T] of     int: x);
/** @group stdlib.arithmetic Return sum of elements in array \a x */
function var int:   sum(array[$T] of var int: x);
/** @group stdlib.arithmetic Return sum of elements in array \a x */
function     float: sum(array[$T] of     float: x);
/** @group stdlib.arithmetic Return sum of elements in array \a x */
function var float: sum(array[$T] of var float: x);
/** @group stdlib.arithmetic Return sum of non-absent elements of \a x. */
function var int: sum(array[int] of var opt int: x) =
  sum (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);
/** @group stdlib.arithmetic Return sum of non-absent elements of \a x. */
function int: sum(array[int] of opt int: x) =
  sum (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 0 endif);
/** @group stdlib.arithmetic Return sum of non-absent elements of \a x. */
function var float: sum(array[int] of var opt float: x) =
  sum (i in index_set(x)) (let { var float: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);
/** @group stdlib.arithmetic Return sum of non-absent elements of \a x. */
function float: sum(array[int] of opt float: x) =
  sum (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 0 endif);


/** @group stdlib.arithmetic Return product of elements in array \a x */
function     int:   product(array[$T] of     int: x);
/** @group stdlib.arithmetic Return product of elements in array \a x */
function var int:   product(array[$T] of var int: x) =
  product_rec(array1d(x));
/** @group stdlib.arithmetic Return product of non-absent elements of \a x. */
function var int: product(array[int] of var opt int: x) =
  product (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 1 endif);
/** @group stdlib.arithmetic Return product of non-absent elements of \a x. */
function int: product(array[int] of opt int: x) =
  product (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 1 endif);
/** @group stdlib.arithmetic Return product of elements in array \a x */
function     float: product(array[$T] of     float: x);
/** @group stdlib.arithmetic Return product of elements in array \a x */
function var float: product(array[$T] of var float: x) =
  product_rec(array1d(x));
/** @group stdlib.arithmetic Return product of non-absent elements of \a x. */
function var float: product(array[int] of var opt float: x) =
  product (i in index_set(x)) (let { var float: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 1 endif);
/** @group stdlib.arithmetic Return product of non-absent elements of \a x. */
function float: product(array[int] of opt float: x) =
  product (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 1 endif);


/** @group stdlib.arithmetic Return minimum of \a x and \a y */
function     $T: min(    $T: x,     $T: y);
/** @group stdlib.arithmetic Return minimum of elements in array \a x */
function     $T: min(array[$U] of     par $T: x);
/** @group stdlib.arithmetic Return minimum of elements in \a x that are not absent. */
function var int: min(array[int] of var opt int: x) =
    let {
        var lb_array(x)..ub_array(x): xmax = max(xi in x)(deopt(xi));
        % Array cannot be empty, otherwise min is undefined
        constraint exists(xi in x)(occurs(xi));
    } in min([if occurs(xi) then deopt(xi) else xmax endif | xi in x]);
/** @group stdlib.arithmetic Return minimum of elements in \a x that are not absent. */
function var float: min(array[int] of var opt float: x) =
    let {
        var lb_array(x)..ub_array(x): xmax = max(xi in x)(deopt(xi));
        % Array cannot be empty, otherwise min is undefined
        constraint exists(xi in x)(occurs(xi));
    } in min([if occurs(xi) then deopt(xi) else xmax endif | xi in x]);

/** @group stdlib.arithmetic Return maximum of \a x and \a y */
function     $T: max(    $T: x,     $T: y);
/** @group stdlib.arithmetic Return maximum of elements in array \a x */
function     $T: max(array[$U] of     $T: x);
/** @group stdlib.arithmetic Return maximum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var int: max(array[int] of var opt int: x) =
    let {
        var lb_array(x)..ub_array(x): xmin = min(xi in x)(deopt(xi));
        constraint exists (xi in x) (occurs(xi));
    } in max([if occurs(xi) then deopt(xi) else xmin endif | xi in x]);
/** @group stdlib.arithmetic Return minimum of elements in set \a x */
function $$E: min(set of $$E: x);
/** @group stdlib.arithmetic Return maximum of elements in set \a x */
function $$E: max(set of $$E: x);

/** @group stdlib.arithmetic Return maximum of \a x and \a y */
function var int: max(var int: x, var int: y) :: promise_total =
  let { var max(lb(x),lb(y))..max(ub(x),ub(y)): m ::is_defined_var;
        constraint int_max(x,y,m) ::defines_var(m);
  } in m;

/** @group stdlib.arithmetic Return maximum of elements in array \a x */
function var int: max(array[$U] of var int: x) =
  let {
    array[int] of var int: xx = array1d(x);
    constraint length(x) >= 1;
  } in max_t(xx);
/** @group stdlib.arithmetic Return maximum of elements in \a x that are not absent. */
function var float: max(array[int] of var opt float: x) =
    let {
        var lb_array(x)..ub_array(x): xmin = min(xi in x)(deopt(xi));
        constraint exists (xi in x) (occurs(xi));
    } in max([if occurs(xi) then deopt(xi) else xmin endif | xi in x]);

/** @group stdlib.arithmetic Return minimum of \a x and \a y */
function var int: min(var int: x, var int: y) :: promise_total =
  let { var min(lb(x),lb(y))..min(ub(x),ub(y)): m ::is_defined_var;
        constraint int_min(x,y,m) ::defines_var(m);
  } in m;

/** @group stdlib.arithmetic Return minimum of elements in array \a x */
function var int: min(array[$U] of var int: x) =
  let {
    array[int] of var int: xx = array1d(x);
    constraint length(x) >= 1;
  } in min_t(xx);

% Floating point min and max
% TODO: add bounds reasoning

/** @group stdlib.arithmetic Return maximum of \a x and \a y */
function var float: max(var float: x, var float: y) :: promise_total =
  if has_bounds(x) /\ has_bounds(y) then
  let { var max(lb(x),lb(y))..max(ub(x),ub(y)): m ::is_defined_var;
        constraint float_max(x,y,m) ::defines_var(m);
  } in m
  else
  let { var float: m ::is_defined_var;
        constraint float_max(x,y,m) ::defines_var(m);
  } in m
  endif;

/** @group stdlib.arithmetic Return maximum of elements in array \a x */
function var float: max(array[$U] of var float: x) =
  let {
    array[int] of var float: xx = array1d(x);
    constraint length(x) >= 1;
  } in max_t(xx);

/** @group stdlib.arithmetic Return minimum of \a x and \a y */
function var float: min(var float: x, var float: y) :: promise_total =
  if has_bounds(x) /\ has_bounds(y) then
  let { var min(lb(x),lb(y))..min(ub(x),ub(y)): m ::is_defined_var;
        constraint float_min(x,y,m) ::defines_var(m);
  } in m
  else
  let { var float: m ::is_defined_var;
        constraint float_min(x,y,m) ::defines_var(m);
  } in m
  endif;

/** @group stdlib.arithmetic Return minimum of elements in array \a x */
function var float: min(array[$U] of var float: x) =
  let {
    array[int] of var float: xx = array1d(x);
    constraint length(x) >= 1;
  } in min_t(xx);


/** @group stdlib.arithmetic
    Returns the index of the minimum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_min(array[$$E] of bool: x);
/** @group stdlib.arithmetic
    Returns the index of the minimum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_min(array[$$E] of int: x);
/** @group stdlib.arithmetic
    Returns the index of the minimum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_min(array[$$E] of float: x);
/** @group stdlib.arithmetic
    Returns the index of the maximum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_max(array[$$E] of bool: x);
/** @group stdlib.arithmetic
    Returns the index of the maximum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_max(array[$$E] of int: x);
/** @group stdlib.arithmetic
    Returns the index of the maximum value in the array \a x.
    When breaking ties the least index is returned.
*/
function $$E: arg_max(array[$$E] of float: x);

/** @group stdlib.arithmetic Return absolute value of \a x */
function int: abs(int: x);

/** @group stdlib.arithmetic Return absolute value of \a x */
function var int: abs(var int: x) :: promise_total =
  if has_bounds(x) /\ lb(x) >= 0 then x
  elseif has_bounds(x) /\ ub(x) <= 0 then -x else
  let { var 0..max(-lb(x),ub(x)): m ::is_defined_var;
        constraint int_abs(x,m) ::defines_var(m);
  } in m
  endif;

/** @group stdlib.arithmetic Return absolute value of \a x */
function float: abs(float: x);
/** @group stdlib.arithmetic Return absolute value of \a x */
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

/** @group stdlib.arithmetic Return \(\sqrt{\a x}\) */
function float: sqrt(float: x);
/** @group stdlib.arithmetic Return \(\sqrt{\a x}\) */
function var float: sqrt(var float: x) =
  let {
    constraint x >= 0.0;
  } in sqrt_t(x);

function var float: sqrt_t(var float: x) ::promise_total =
  let {
    var float: r;
    var float: xx;
    constraint x < 0.0 -> xx = 1.0;
    constraint x < 0.0 \/ xx = x;
    constraint float_sqrt(xx,r);
  } in r;

/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function int: pow(int: x, int: y);

/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function var int: pow(var int: x, var int: y) =
  let {
    int: yy = if is_fixed(y) then fix(y) else -1 endif;
  } in
  if yy = 0 then 1
  elseif yy = 1 then x
  elseif 0..1==dom(x) then x
  elseif -1..1==dom(x) /\ is_fixed(y) /\ 1 == fix(y) mod 2 then x else
  let { var int: r ::is_defined_var;
        constraint if is_fixed(y) then int_pow_fixed(x,fix(y),r) ::defines_var(r) else int_pow(x,y,r) ::defines_var(r) endif;
  } in r
  endif;


/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function float: pow(float: x, float: y);

/** @group stdlib.arithmetic Return \(\a x ^ {\a y}\) */
function var float: pow(var float: x, var float: y) =
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
  @groupdef stdlib.explog Exponential and logarithmic builtins

  These builtins implement exponential and logarithmic functions.
*/

/** @group stdlib.explog Return \(e ^ {\a x}\) */
function float: exp(float: x);
/** @group stdlib.explog Return \(e ^ {\a x}\) */
function var float: exp(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_exp(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.explog Return \(\ln \a x\) */
function float: ln(float: x);
/** @group stdlib.explog Return \(\ln \a x\) */
function var float: ln(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_ln(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.explog Return \(\log_{10} \a x\) */
function float: log10(float: x);
/** @group stdlib.explog Return \(\log_{10} \a x\) */
function var float: log10(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_log10(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.explog Return \(\log_{2} \a x\) */
function float: log2(float: x);
/** @group stdlib.explog Return \(\log_{2} \a x\) */
function var float: log2(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_log2(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.explog Return \(\log_{\a x} \a y\) */
function float: log(float: x, float: y);

/***
  @groupdef stdlib.trigonometric Trigonometric functions

  These builtins implement the standard trigonometric functions.
*/

/** @group stdlib.trigonometric Return \(\sin \a x\) */
function float: sin(float: x);
/** @group stdlib.trigonometric Return \(\sin \a x\) */
function var float: sin(var float: x) ::promise_total =
  let {
    var -1.0..1.0: r ::is_defined_var;
    constraint float_sin(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\cos \a x\) */
function float: cos(float: x);
/** @group stdlib.trigonometric Return \(\cos \a x\) */
function var float: cos(var float: x) ::promise_total =
  let {
    var -1.0..1.0: r ::is_defined_var;
    constraint float_cos(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\tan \a x\) */
function float: tan(float: x);
/** @group stdlib.trigonometric Return \(\tan \a x\) */
function var float: tan(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_tan(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\mbox{asin}\ \a x\) */
function float: asin(float: x);
/** @group stdlib.trigonometric Return \(\mbox{asin}\ \a x\) */
function var float: asin(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_asin(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\mbox{acos}\ \a x\) */
function float: acos(float: x);
/** @group stdlib.trigonometric Return \(\mbox{acos}\ \a x\) */
function var float: acos(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_acos(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\mbox{atan}\ \a x\) */
function float: atan(float: x);
/** @group stdlib.trigonometric Return \(\mbox{atan}\ \a x\) */
function var float: atan(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_atan(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\sinh \a x\) */
function float: sinh(float: x);
/** @group stdlib.trigonometric Return \(\sinh \a x\) */
function var float: sinh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_sinh(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\cosh \a x\) */
function float: cosh(float: x);
/** @group stdlib.trigonometric Return \(\cosh \a x\) */
function var float: cosh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_cosh(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\tanh \a x\) */
function float: tanh(float: x);
/** @group stdlib.trigonometric Return \(\tanh \a x\) */
function var float: tanh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_tanh(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\mbox{asinh}\ \a x\) */
function float: asinh(float: x);
/** @group stdlib.trigonometric Return \(\mbox{asinh}\ \a x\) */
function var float: asinh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_asinh(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\mbox{acosh}\ \a x\) */
function float: acosh(float: x);
/** @group stdlib.trigonometric Return \(\mbox{acosh}\ \a x\) */
function var float: acosh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_acosh(x,r) ::defines_var(r);
  } in r;

/** @group stdlib.trigonometric Return \(\mbox{atanh}\ \a x\) */
function float: atanh(float: x);
/** @group stdlib.trigonometric Return \(\mbox{atanh}\ \a x\) */
function var float: atanh(var float: x) ::promise_total =
  let {
    var float: r ::is_defined_var;
    constraint float_atanh(x,r) ::defines_var(r);
  } in r;

