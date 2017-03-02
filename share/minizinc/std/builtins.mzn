include "flatzinc_builtins.mzn";

/***
  @groupdef builtins Builtins
  
  These functions and predicates define built-in operations of the MiniZinc language.
  
*/

/***
  @groupdef builtins.compare Comparison Builtins
  
  These builtins implement comparison operations.
*/

/** @group builtins.compare Return if \a x is less than \a y */
function     bool: '<'(    $T: x,    $T: y);
/** @group builtins.compare Return if \a x is less than \a y */
function var bool: '<'(var $T: x,var $T: y);
/** @group builtins.compare Return if \a x is greater than \a y */
function     bool: '>'(    $T: x,    $T: y);
/** @group builtins.compare Return if \a x is greater than \a y */
function var bool: '>'(var $T: x,var $T: y);
/** @group builtins.compare Return if \a x is less than or equal to \a y */
function     bool: '<='(    $T: x,    $T: y);
/** @group builtins.compare Return if \a x is less than or equal to \a y */
function var bool: '<='(var $T: x, var $T: y);
/** @group builtins.compare Return if \a x is greater than or equal to \a y */
function     bool: '>='(    $T: x,    $T: y);
/** @group builtins.compare Return if \a x is greater than or equal to \a y */
function var bool: '>='(var $T: x,var $T: y);
/** @group builtins.compare Return if \a x is equal to \a y */
function     bool: '='(    $T: x,    $T: y);
/** @group builtins.compare Return if \a x is equal to \a y */
function     bool: '='(opt $T: x, opt $T: y);
/** @group builtins.compare Return if \a x is equal to \a y */
function var bool: '='(var $T: x,var $T: y);
/** @group builtins.compare Return if \a x is equal to \a y */
function var bool: '='(var opt $T: x,var opt $T: y);

/** @group builtins.compare Return if \a x is not equal to \a y */
function     bool: '!='(    $T: x,     $T: y);
/** @group builtins.compare Return if \a x is not equal to \a y */
function var bool: '!='(var $T: x, var $T: y);

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


/** @group builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function     bool: '<'(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function var bool: '<'(array[$U] of var $T: x,array[$U] of var $T: y);
/** @group builtins.compare Return if array \a x is lexicographically greater than array \a y */
function     bool: '>'(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group builtins.compare Return if array \a x is lexicographically greater than array \a y */
function var bool: '>'(array[$U] of var $T: x,array[$U] of var $T: y);
/** @group builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function     bool: '<='(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function var bool: '<='(array[$U] of var $T: x,array[$U] of var $T: y);
/** @group builtins.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function     bool: '>='(array[$U] of     $T: x,array[$U] of     $T: y);
function var bool: '>='(array[$U] of var $T: x,array[$U] of var $T: y);

/** @group builtins.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     int: x,array[$T] of     int: y) =
    let {
        array[int] of int: xx = array1d(x);
        array[int] of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var int: x,array[$T] of var int: y) =
    let {
        array[int] of var int: xx = array1d(x);
        array[int] of var int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     bool: x,array[$T] of     bool: y) =
    let {
        array[int] of bool: xx = array1d(x);
        array[int] of bool: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var bool: x,array[$T] of var bool: y) =
    let {
        array[int] of var bool: xx = array1d(x);
        array[int] of var bool: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     set of int: x,array[$T] of     set of int: y) =
    let {
        array[int] of set of int: xx = array1d(x);
        array[int] of set of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var set of int: x,array[$T] of var set of int: y) =
    let {
        array[int] of var set of int: xx = array1d(x);
        array[int] of var set of int: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function     bool: '='(array[$T] of     float: x,array[$T] of     float: y) =
    let {
        array[int] of float: xx = array1d(x);
        array[int] of float: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$T] of var float: x,array[$T] of var float: y) =
    let {
        array[int] of var float: xx = array1d(x);
        array[int] of var float: yy = array1d(y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

/** @group builtins.compare Return if array \a x is not equal to array \a y */
function     bool: '!='(array[$U] of     $T: x,array[$U] of     $T: y);
/** @group builtins.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$U] of var $T: x,array[$U] of var $T: y);

/***
  @groupdef builtins.arithmetic Arithmetic Builtins

  These builtins implement arithmetic operations.
*/

/** @group builtins.arithmetic Return \a x + \a y */
function     int:   '+'(    int: x,     int: y);
/** @group builtins.arithmetic Return \a x + \a y */
function var int:   '+'(var int: x,  var int: y);
/** @group builtins.arithmetic Return \a x + \a y */
function     float: '+'(    float: x,    float: y);
/** @group builtins.arithmetic Return \a x + \a y */
function var float: '+'(var float: x,var float: y);
/** @group builtins.arithmetic Return \a x - \a y */
function     int:   '-'(    int: x,     int: y);
/** @group builtins.arithmetic Return \a x - \a y */
function var int:   '-'(var int: x,  var int: y);
/** @group builtins.arithmetic Return \a x - \a y */
function     float: '-'(    float: x,    float: y);
/** @group builtins.arithmetic Return \a x - \a y */
function var float: '-'(var float: x,var float: y);
/** @group builtins.arithmetic Return \a x * \a y */
function     int:   '*'(    int: x,     int: y);
/** @group builtins.arithmetic Return \a x * \a y */
function var int:   '*'(var int: x,  var int: y);
/** @group builtins.arithmetic Return \a x * \a y */
function     float: '*'(    float: x,    float: y);
/** @group builtins.arithmetic Return \a x * \a y */
function var float: '*'(var float: x,var float: y);
/** @group builtins.arithmetic Return negative \a x */
function     int:   '-'(    int: x);
/** @group builtins.arithmetic Return negative \a x */
function var int:   '-'(var int: x);
/** @group builtins.arithmetic Return negative \a x */
function     float: '-'(    float: x);
/** @group builtins.arithmetic Return negative \a x */
function var float: '-'(var float: x);

/** @group builtins.arithmetic Return result of integer division \a x / \a y */
function     int: 'div'(int: x,int: y);

/** @group builtins.arithmetic Return result of integer division \a x / \a y */
function var int: 'div'(var int: x,var int: y) =
  if mzn_in_root_context(y) \/ not (0 in dom(y)) then div_t(x,y) else
  let { constraint y != 0 } in div_mt(x,y) endif;

/** @group builtins.arithmetic Return remainder of integer division \a x % \a y */
function     int: 'mod'(int: x,int: y);

/** @group builtins.arithmetic Return remainder of integer division \a x % \a y */
function var int: 'mod'(var int: x,var int: y) =
  if mzn_in_root_context(y) \/ not (0 in dom(y)) then mod_t(x,y) else
  let { constraint y != 0 } in mod_mt(x,y) endif;

/** @group builtins.arithmetic Return result of floating point division \a x / \a y */
function     float: '/'(    float: x,    float: y);
/** @group builtins.arithmetic Return result of floating point division \a x / \a y */
function var float: '/'(var float: x,var float: y);

/** @group builtins.arithmetic Return sum of elements in array \a x */
function     int:   sum(array[$T] of     int: x);
/** @group builtins.arithmetic Return sum of elements in array \a x */
function var int:   sum(array[$T] of var int: x);
/** @group builtins.arithmetic Return sum of elements in array \a x */
function     float: sum(array[$T] of     float: x);
/** @group builtins.arithmetic Return sum of elements in array \a x */
function var float: sum(array[$T] of var float: x);

/** @group builtins.arithmetic Return product of elements in array \a x */
function     int:   product(array[$T] of     int: x);
/** @group builtins.arithmetic Return product of elements in array \a x */
function var int:   product(array[$T] of var int: x) =
  product_rec(array1d(x));

/** @group builtins.arithmetic Return product of elements in array \a x */
function     float: product(array[$T] of     float: x);
/** @group builtins.arithmetic Return product of elements in array \a x */
function var float: product(array[$T] of var float: x) =
  product_rec(array1d(x));

/** @group builtins.arithmetic Return minimum of \a x and \a y */
function     $T: min(    $T: x,     $T: y);
/** @group builtins.arithmetic Return minimum of elements in array \a x */
function     $T: min(array[$U] of     par $T: x);
/** @group builtins.arithmetic Return maximum of \a x and \a y */
function     $T: max(    $T: x,     $T: y);
/** @group builtins.arithmetic Return maximum of elements in array \a x */
function     $T: max(array[$U] of     $T: x);
/** @group builtins.arithmetic Return minimum of elements in set \a x */
function $$E: min(set of $$E: x);
/** @group builtins.arithmetic Return maximum of elements in set \a x */
function $$E: max(set of $$E: x);

/** @group builtins.arithmetic Return maximum of \a x and \a y */
function var int: max(var int: x, var int: y) :: promise_total =
  let { var max(lb(x),lb(y))..max(ub(x),ub(y)): m;
        constraint int_max(x,y,m);
  } in m;

/** @group builtins.arithmetic Return maximum of elements in array \a x */
function var int: max(array[$U] of var int: x) =
  let {
    array[int] of var int: xx = array1d(x);
    constraint length(x) >= 1;
  } in max_t(xx);

/** @group builtins.arithmetic Return minimum of \a x and \a y */
function var int: min(var int: x, var int: y) :: promise_total =
  let { var min(lb(x),lb(y))..min(ub(x),ub(y)): m;
        constraint int_min(x,y,m);
  } in m;

/** @group builtins.arithmetic Return minimum of elements in array \a x */
function var int: min(array[$U] of var int: x) =
  let {
    array[int] of var int: xx = array1d(x);
    constraint length(x) >= 1;
  } in min_t(xx);

% Floating point min and max
% TODO: add bounds reasoning

/** @group builtins.arithmetic Return maximum of \a x and \a y */
function var float: max(var float: x, var float: y) :: promise_total =
  let { var float: m;
        constraint float_max(x,y,m);
  } in m;

/** @group builtins.arithmetic Return maximum of elements in array \a x */
function var float: max(array[$U] of var float: x) =
  let {
    array[int] of var float: xx = array1d(x);
    constraint length(x) >= 1;
  } in max_t(xx);

/** @group builtins.arithmetic Return minimum of \a x and \a y */
function var float: min(var float: x, var float: y) :: promise_total =
  let { var float: m;
        constraint float_min(x,y,m);
  } in m;

/** @group builtins.arithmetic Return minimum of elements in array \a x */
function var float: min(array[$U] of var float: x) =
  let {
    array[int] of var float: xx = array1d(x);
    constraint length(x) >= 1;
  } in min_t(xx);


/** @group builtins.arithmetic Return index of minimum of elements in array \a x */
function $$E: arg_min(array[$$E] of int: x);
/** @group builtins.arithmetic Return index of minimum of elements in array \a x */
function $$E: arg_min(array[$$E] of float: x);
/** @group builtins.arithmetic Return index of maximum of elements in array \a x */
function $$E: arg_max(array[$$E] of int: x);
/** @group builtins.arithmetic Return index of maximum of elements in array \a x */
function $$E: arg_max(array[$$E] of float: x);

/** @group builtins.arithmetic Return absolute value of \a x */
function int: abs(int: x);

/** @group builtins.arithmetic Return absolute value of \a x */
function var int: abs(var int: x) :: promise_total =
  if has_bounds(x) /\ lb(x) >= 0 then x else
  let { var 0..max(-lb(x),ub(x)): m;
        constraint int_abs(x,m);
  } in m
  endif;

/** @group builtins.arithmetic Return absolute value of \a x */
function float: abs(float: x);
/** @group builtins.arithmetic Return absolute value of \a x */
function var float: abs(var float: x) :: promise_total =
  if has_bounds(x) then
    if lb(x) >= 0.0 then x else
    let { var 0.0..max(-lb(x),ub(x)): m;
          constraint float_abs(x,m);
    } in m
    endif
  else
    let { var float: m;
          constraint m >= 0.0;
          constraint float_abs(x,m);
    } in m
  endif;

/** @group builtins.arithmetic Return \(\sqrt{\a x}\) */
function float: sqrt(float: x);
/** @group builtins.arithmetic Return \(\sqrt{\a x}\) */
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

/** @group builtins.arithmetic Return \(\a x ^ {\a y}\) */
function int: pow(int: x, int: y);

/** @group builtins.arithmetic Return \(\a x ^ {\a y}\) */
function var int: pow(var int: x, var int: y) =
  let {
    int: yy = if is_fixed(y) then fix(y) else -1 endif;
  } in
  if yy = 0 then 1
  elseif yy = 1 then x
  elseif yy = 2 then x*x else
  let { var int: r;
        constraint int_pow(x,y,r);
  } in r
  endif;


/** @group builtins.arithmetic Return \(\a x ^ {\a y}\) */
function float: pow(float: x, float: y);

/** @group builtins.arithmetic Return \(\a x ^ {\a y}\) */
function var float: pow(var float: x, var float: y) =
  let {
    float: yy = if is_fixed(y) then fix(y) else -1.0 endif
  } in
  if yy = 0.0 then 1.0
  elseif yy = 1.0 then x
  elseif yy = 2.0 then x*x else
  let { var float: r;
        constraint float_pow(x,y,r);
  } in r
  endif;

/***
  @groupdef builtins.explog Exponential and logarithmic builtins

  These builtins implement exponential and logarithmic functions.
*/

/** @group builtins.explog Return \(e ^ {\a x}\) */
function float: exp(float: x);
/** @group builtins.explog Return \(e ^ {\a x}\) */
function var float: exp(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_exp(x,r);
  } in r;

/** @group builtins.explog Return \(\ln \a x\) */
function float: ln(float: x);
/** @group builtins.explog Return \(\ln \a x\) */
function var float: ln(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_ln(x,r);
  } in r;

/** @group builtins.explog Return \(\log_{10} \a x\) */
function float: log10(float: x);
/** @group builtins.explog Return \(\log_{10} \a x\) */
function var float: log10(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_log10(x,r);
  } in r;

/** @group builtins.explog Return \(\log_{2} \a x\) */
function float: log2(float: x);
/** @group builtins.explog Return \(\log_{2} \a x\) */
function var float: log2(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_log2(x,r);
  } in r;

/** @group builtins.explog Return \(\log_{\a x} \a y\) */
function float: log(float: x, float: y);

/***
  @groupdef builtins.trigonometric Trigonometric functions

  These builtins implement the standard trigonometric functions.
*/

/** @group builtins.trigonometric Return \(\sin \a x\) */
function float: sin(float: x);
/** @group builtins.trigonometric Return \(\sin \a x\) */
function var float: sin(var float: x) ::promise_total =
  let {
    var -1.0..1.0: r;
    constraint float_sin(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\cos \a x\) */
function float: cos(float: x);
/** @group builtins.trigonometric Return \(\cos \a x\) */
function var float: cos(var float: x) ::promise_total =
  let {
    var -1.0..1.0: r;
    constraint float_cos(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\tan \a x\) */
function float: tan(float: x);
/** @group builtins.trigonometric Return \(\tan \a x\) */
function var float: tan(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_tan(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\mbox{asin}\ \a x\) */
function float: asin(float: x);
/** @group builtins.trigonometric Return \(\mbox{asin}\ \a x\) */
function var float: asin(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_asin(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\mbox{acos}\ \a x\) */
function float: acos(float: x);
/** @group builtins.trigonometric Return \(\mbox{acos}\ \a x\) */
function var float: acos(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_acos(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\mbox{atan}\ \a x\) */
function float: atan(float: x);
/** @group builtins.trigonometric Return \(\mbox{atan}\ \a x\) */
function var float: atan(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_atan(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\sinh \a x\) */
function float: sinh(float: x);
/** @group builtins.trigonometric Return \(\sinh \a x\) */
function var float: sinh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_sinh(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\cosh \a x\) */
function float: cosh(float: x);
/** @group builtins.trigonometric Return \(\cosh \a x\) */
function var float: cosh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_cosh(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\tanh \a x\) */
function float: tanh(float: x);
/** @group builtins.trigonometric Return \(\tanh \a x\) */
function var float: tanh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_tanh(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\mbox{asinh}\ \a x\) */
function float: asinh(float: x);
/** @group builtins.trigonometric Return \(\mbox{asinh}\ \a x\) */
function var float: asinh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_asinh(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\mbox{acosh}\ \a x\) */
function float: acosh(float: x);
/** @group builtins.trigonometric Return \(\mbox{acosh}\ \a x\) */
function var float: acosh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_acosh(x,r);
  } in r;

/** @group builtins.trigonometric Return \(\mbox{atanh}\ \a x\) */
function float: atanh(float: x);
/** @group builtins.trigonometric Return \(\mbox{atanh}\ \a x\) */
function var float: atanh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_atanh(x,r);
  } in r;


/***
  @groupdef builtins.logic Logical operations
  
  Logical operations are the standard operators of Boolean logic.
*/

/** @group builtins.logic Return truth value of \a x &and; \a y */
function     bool: '/\'(    bool: x,     bool: y);
/** @group builtins.logic Return truth value of \a x &and; \a y */
function var bool: '/\'(var bool: x, var bool: y);
/** @group builtins.logic Return truth value of \a x &or; \a y */
function     bool: '\/'(    bool: x,     bool: y);
/** @group builtins.logic Return truth value of \a x &or; \a y */
function var bool: '\/'(var bool: x, var bool: y);
/** @group builtins.logic Return truth value of \a x implies \a y */
function     bool: '->'(    bool: x,     bool: y);
/** @group builtins.logic Return truth value of \a x implies \a y */
function var bool: '->'(var bool: x, var bool: y);
/** @group builtins.logic Return truth value of \a y implies \a x */
function     bool: '<-'(    bool: x,     bool: y);
/** @group builtins.logic Return truth value of \a y implies \a x */
function var bool: '<-'(var bool: x, var bool: y);
/** @group builtins.logic Return truth value of \a x if-and-only-if \a y */
function     bool: '<->'(    bool: x,     bool: y);
/** @group builtins.logic Return truth value of \a x if-and-only-if \a y */
function var bool: '<->'(var bool: x, var bool: y);
/** @group builtins.logic Return truth value of \a x xor \a y */
function     bool: 'xor'(    bool: x,     bool: y);
/** @group builtins.logic Return truth value of \a x xor \a y */
function var bool: 'xor'(var bool: x, var bool: y);
/** @group builtins.logic Return truth value of the negation of \a x */
function     bool: 'not'(    bool: x);
/** @group builtins.logic Return truth value of the negation of \a x */
function var bool: 'not'(var bool: x);

/** @group builtins.logic Return truth value of \(\bigwedge_i \a x[i]\) */
function     bool: forall(array[$T] of     bool: x);
/** @group builtins.logic Return truth value of \(\bigwedge_i \a x[i]\) */
function var bool: forall(array[$T] of var bool: x);
/** @group builtins.logic Return truth value of \(\bigvee_i \a x[i]\) */
function     bool: exists(array[$T] of     bool: x);
/** @group builtins.logic Return truth value of \(\bigvee_i \a x[i]\) */
function var bool: exists(array[$T] of var bool: x);
/** @group builtins.logic Return truth value of \(\oplus_i \a x[i]\) */
function     bool: xorall(array[$T] of     bool: x);
/** @group builtins.logic Return truth value of \(\oplus_i \a x[i]\) */
function var bool: xorall(array[$T] of var bool: x) =
  array_bool_xor(array1d(x));
/** @group builtins.logic Return truth value of \(\text{true}\oplus (\oplus_i \a x[i])\) */
function     bool: iffall(array[$T] of     bool: x);
/** @group builtins.logic Return truth value of \(\text{true}\oplus (\oplus_i \a x[i])\) */
function var bool: iffall(array[$T] of var bool: x) =
  array_bool_xor(array1d(x)++[true]);
/** @group builtins.logic Return truth value of \((\bigvee_i \a x[i]) \lor (\bigvee_j \lnot \a y[j])\) */
function var bool: clause(array[$T] of var bool: x, array[$T] of var bool: y);
/** @group builtins.logic Return truth value of \((\bigvee_i \a x[i]) \lor (\bigvee_j \lnot \a y[j])\) */
function var bool: clause(array[$T] of     bool: x, array[$T] of     bool: y);

/***
  @groupdef builtins.set Set operations
  
  These functions implement the basic operations on sets.
*/

/** @group builtins.set Test if \a x is an element of the set \a y */
function     bool: 'in'(    int: x,     set of int: y);
/** @group builtins.set \a x is an element of the set \a y */
function var bool: 'in'(var int: x, var set of int: y);

/** @group builtins.set Test if \a x is an element of the set \a y */
function     bool: 'in'(    float: x,     set of float: y);
/** @group builtins.set Test if \a x is an element of the set \a y */
function var bool: 'in'(var float: x,     set of float: y);

/** @group builtins.set Test if \a x is a subset of \a y */
function     bool: 'subset'(    set of $T: x,      set of $T: y);
/** @group builtins.set \a x is a subset of \a y */
function var bool: 'subset'(var set of int: x, var set of int: y);
/** @group builtins.set Test if \a x is a superset of \a y */
function     bool: 'superset'(    set of $T: x,      set of $T: y);
/** @group builtins.set \a x is a superset of \a y */
function var bool: 'superset'(var set of int: x, var set of int: y);

/** @group builtins.set Return the union of sets \a x and \a y */
function     set of $T:  'union'(    set of $T: x,      set of $T: y);
/** @group builtins.set Return the union of sets \a x and \a y */
function var set of int: 'union'(var set of int: x, var set of int: y);
/** @group builtins.set Return the intersection of sets \a x and \a y */
function     set of $T:  'intersect'(    set of $T: x,      set of $T: y);
/** @group builtins.set Return the intersection of sets \a x and \a y */
function var set of int: 'intersect'(var set of int: x, var set of int: y);
/** @group builtins.set Return the set difference of sets \a x &minus; \a y */
function     set of $T:  'diff'(    set of $T: x,      set of $T: y);
/** @group builtins.set Return the set difference of sets \a x &minus; \a y */
function var set of int: 'diff'(var set of int: x, var set of int: y);
/** @group builtins.set Return the symmetric set difference of sets \a x and \a y */
function     set of $T:  'symdiff'(    set of $T: x,      set of $T: y);
/** @group builtins.set Return the symmetric set difference of sets \a x and \a y */
function var set of int: 'symdiff'(var set of int: x, var set of int: y);

/** @group builtins.set Return the set \(\{\a a,\ldots,\a b\}\) */
function set of $$E: '..'($$E: a,$$E: b);
/** @group builtins.set Return the set \(\{\a a,\ldots,\a b\}\) */
function set of float: '..'(float: a,float: b);

function var set of int: '..'(var int: a, var int: b) ::promise_total =
  let {
    var set of min(lb(a),lb(b))..max(ub(a),ub(b)): s;
    constraint forall (i in ub(s)) (i in s <-> (a <= i /\ i <= b));
  } in s;

/** @group builtins.set Return the cardinality of the set \a x */
function     int: card(    set of $T: x);
/** @group builtins.set Return the cardinality of the set \a x */
function var int: card(var set of int: x);

/** @group builtins.set Return the union of the sets in array \a x */
function     set of $U:  array_union(array[$T] of     set of $U: x);
/** @group builtins.set Return the union of the sets in array \a x */
function var set of int: array_union(array[int] of var set of int: x) ::promise_total =
    if length(x)=0 then {}
    elseif length(x)=1 then x[min(index_set(x))]
    else
      let {
        int: l=min(index_set(x));
        int: u=max(index_set(x));
        array[l..u-1] of var set of ub_array(x): y;
        constraint y[l]=x[l] union x[l+1];
        constraint forall (i in l+2..u) (y[i-1]=y[i-2] union x[i]);
      } in y[u-1]
    endif;


/** @group builtins.set Return the intersection of the sets in array \a x */
function     set of $U:  array_intersect(array[$T] of     set of $U: x);

/** @group builtins.set Return the intersection of the sets in array \a x */
function var set of int: array_intersect(array[int] of var set of int: x) ::promise_total =
    if length(x)=0 then assert(false,"can't be!",-infinity..infinity)
    elseif length(x)=1 then x[min(index_set(x))]
    else
      let {
        int: l=min(index_set(x));
        int: u=max(index_set(x));
        array[l..u-1] of var set of ub_array(x): y;
        constraint y[l]=x[l] intersect x[l+1];
        constraint forall (i in l+2..u) (y[i-1]=y[i-2] intersect x[i]);
      } in y[u-1]
    endif;

/** @group builtins.set Return the minimum of the set \a s */
function var $$E: min(var set of $$E: s);

/** @group builtins.set Return the maximum of the set \a s */
function var $$E: max(var set of $$E: s);

/***
  @groupdef builtins.array Array operations
  
  These functions implement the basic operations on arrays.
*/

/** @group builtins.array Return the concatenation of arrays \a x and \a y */
function array[int] of $T: '++'(array[int] of $T: x, array[int] of $T: y);
/** @group builtins.array Return the concatenation of arrays \a x and \a y */
function array[int] of var $T: '++'(array[int] of var $T: x, array[int] of var $T: y);
/** @group builtins.array Return the concatenation of arrays \a x and \a y */
function array[int] of var opt $T: '++'(array[int] of var opt $T: x, array[int] of var opt $T: y);

/** @group builtins.array Return the length of array \a x

  Note that the length is defined as the number of elements in the
  array, regardless of its dimensionality.
*/
function int: length(array[$T] of var opt $U: x);

/** @group builtins.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of $T: reverse(array[$$E] of $T: x) =
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)]);

/** @group builtins.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of var $T: reverse(array[$$E] of var $T: x) =
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)]);

/** @group builtins.array Return the array \a x in reverse order

  The resulting array has the same index set as \a x.
*/
function array[$$E] of var opt $T: reverse(array[$$E] of var opt $T: x) =
  let { int: l = max(index_set(x))+min(index_set(x)) } in
  array1d(index_set(x),[x[l-i] | i in index_set(x)]);

/** @group builtins.array Test if \a x and \a y have the same index sets */
test index_sets_agree(array[$T] of var opt $U: x, array[$T] of var opt $W: y);

/** @group builtins.array Return index set of one-dimensional array \a x */
function set of $$E: index_set(array[$$E] of var opt $U: x);
/** @group builtins.array Return index set of first dimension of two-dimensional array \a x */
function set of $$E: index_set_1of2(array[$$E,int] of var opt $U: x);
/** @group builtins.array Return index set of second dimension of two-dimensional array \a x */
function set of $$E: index_set_2of2(array[int,$$E] of var opt $U: x);
/** @group builtins.array Return index set of first dimension of 3-dimensional array \a x */
function set of $$E: index_set_1of3(array[$$E,int,int] of var opt $U: x);
/** @group builtins.array Return index set of second dimension of 3-dimensional array \a x */
function set of $$E: index_set_2of3(array[int,$$E,int] of var opt $U: x);
/** @group builtins.array Return index set of third dimension of 3-dimensional array \a x */
function set of $$E: index_set_3of3(array[int,int,$$E] of var opt $U: x);
/** @group builtins.array Return index set of first dimension of 4-dimensional array \a x */
function set of $$E: index_set_1of4(array[$$E,int,int,int] of var opt $U: x);
/** @group builtins.array Return index set of second dimension of 4-dimensional array \a x */
function set of $$E: index_set_2of4(array[int,$$E,int,int] of var opt $U: x);
/** @group builtins.array Return index set of third dimension of 4-dimensional array \a x */
function set of $$E: index_set_3of4(array[int,int,$$E,int] of var opt $U: x);
/** @group builtins.array Return index set of fourth dimension of 4-dimensional array \a x */
function set of $$E: index_set_4of4(array[int,int,int,$$E] of var opt $U: x);
/** @group builtins.array Return index set of first dimension of 5-dimensional array \a x */
function set of $$E: index_set_1of5(array[$$E,int,int,int,int] of var opt $U: x);
/** @group builtins.array Return index set of second dimension of 5-dimensional array \a x */
function set of $$E: index_set_2of5(array[int,$$E,int,int,int] of var opt $U: x);
/** @group builtins.array Return index set of third dimension of 5-dimensional array \a x */
function set of $$E: index_set_3of5(array[int,int,$$E,int,int] of var opt $U: x);
/** @group builtins.array Return index set of fourth dimension of 5-dimensional array \a x */
function set of $$E: index_set_4of5(array[int,int,int,$$E,int] of var opt $U: x);
/** @group builtins.array Return index set of fifth dimension of 5-dimensional array \a x */
function set of $$E: index_set_5of5(array[int,int,int,int,$$E] of var opt $U: x);
/** @group builtins.array Return index set of first dimension of 6-dimensional array \a x */
function set of $$E: index_set_1of6(array[$$E,int,int,int,int,int] of var opt $U: x);
/** @group builtins.array Return index set of second dimension of 6-dimensional array \a x */
function set of $$E: index_set_2of6(array[int,$$E,int,int,int,int] of var opt $U: x);
/** @group builtins.array Return index set of third dimension of 6-dimensional array \a x */
function set of $$E: index_set_3of6(array[int,int,$$E,int,int,int] of var opt $U: x);
/** @group builtins.array Return index set of fourth dimension of 6-dimensional array \a x */
function set of $$E: index_set_4of6(array[int,int,int,$$E,int,int] of var opt $U: x);
/** @group builtins.array Return index set of fifth dimension of 6-dimensional array \a x */
function set of $$E: index_set_5of6(array[int,int,int,int,$$E,int] of var opt $U: x);
/** @group builtins.array Return index set of sixth dimension of 6-dimensional array \a x */
function set of $$E: index_set_6of6(array[int,int,int,int,int,$$E] of var opt $U: x);

/** @group builtins.array Return array \a x coerced to index set 1..length(\a x) */
function array[int] of $V: array1d(array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to index set 1..length(\a x) */
function array[int] of var $V: array1d(array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to index set 1..length(\a x) */
function array[int] of var opt $V: array1d(array[$U] of var opt $V: x);

/** @group builtins.array Return array \a x coerced to one-dimensional array with index set \a S */
function array[$$E] of $V: array1d(set of $$E: S,
                                   array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to one-dimensional array with index set \a S */
function array[$$E] of var $V: array1d(set of $$E: S,
                                       array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to one-dimensional array with index set \a S */
function array[$$E] of var opt $V: array1d(set of $$E: S,
                                           array[$U] of var opt $V: x);

/** @group builtins.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2 */
function array[$$E,$$F] of $V: array2d(set of $$E: S1, set of $$F: S2, 
                                       array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2 */
function array[$$E,$$F] of var $V: array2d(set of $$E: S1, set of $$F: S2, 
                                           array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to two-dimensional array with index sets \a S1 and \a S2 */
function array[$$E,$$F] of var opt $V: array2d(set of $$E: S1, set of $$F: S2, 
                                               array[$U] of var opt $V: x);

/** @group builtins.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3
*/
function array[$$E,$$F,$$G] of $V: array3d(set of $$E: S1,
                                           set of $$F: S2,
                                           set of $$G: S3,
                                           array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3
*/
function array[$$E,$$F,$$G] of var $V: array3d(set of $$E: S1,
                                               set of $$F: S2,
                                               set of $$G: S3,
                                               array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to three-dimensional array with
  index sets \a S1, \a S2 and \a S3
*/
function array[$$E,$$F,$$G] of var opt $V: array3d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   array[$U] of var opt $V: x);

/** @group builtins.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4
*/
function array[$$E,$$F,$$G,$$H] of $V: array4d(set of $$E: S1,
                                               set of $$F: S2,
                                               set of $$G: S3,
                                               set of $$H: S4,
                                               array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4
*/
function array[$$E,$$F,$$G,$$H] of var $V: array4d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   set of $$H: S4,
                                                   array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to 4-dimensional array with
  index sets \a S1, \a S2, \a S3 and \a S4
*/
function array[$$E,$$F,$$G,$$H] of var opt $V: array4d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       array[$U] of var opt $V: x);

/** @group builtins.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5
*/
function array[$$E,$$F,$$G,$$H,$$I] of $V: array5d(set of $$E: S1,
                                                   set of $$F: S2,
                                                   set of $$G: S3,
                                                   set of $$H: S4,
                                                   set of $$I: S5,
                                                   array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5
*/
function array[$$E,$$F,$$G,$$H,$$I] of var $V: array5d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       set of $$I: S5,
                                                       array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to 5-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4 and \a S5
*/
function array[$$E,$$F,$$G,$$H,$$I] of var opt $V: array5d(set of $$E: S1,
                                                           set of $$F: S2,
                                                           set of $$G: S3,
                                                           set of $$H: S4,
                                                           set of $$I: S5,
                                                           array[$U] of var opt $V: x);

/** @group builtins.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of $V: array6d(set of $$E: S1,
                                                       set of $$F: S2,
                                                       set of $$G: S3,
                                                       set of $$H: S4,
                                                       set of $$I: S5,
                                                       set of $$J: S6,
                                                       array[$U] of $V: x);
/** @group builtins.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of var $V: array6d(set of $$E: S1,
                                                           set of $$F: S2,
                                                           set of $$G: S3,
                                                           set of $$H: S4,
                                                           set of $$I: S5,
                                                           set of $$J: S6,
                                                           array[$U] of var $V: x);
/** @group builtins.array Return array \a x coerced to 6-dimensional array with
  index sets \a S1, \a S2, \a S3, \a S4, \a S5 and \a S6
*/
function array[$$E,$$F,$$G,$$H,$$I,$$J] of var opt $V: array6d(set of $$E: S1,
                                                               set of $$F: S2,
                                                               set of $$G: S3,
                                                               set of $$H: S4,
                                                               set of $$I: S5,
                                                               set of $$J: S6,
                                                               array[$U] of var opt $V: x);

/** @group builtins.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x
*/
function array[$T] of $V: arrayXd(array[$T] of var opt $X: x, array[$U] of $V: y);
/** @group builtins.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x
*/
function array[$T] of var $V: arrayXd(array[$T] of var opt $X: x, array[$U] of var $V: y);
/** @group builtins.array Return array \a y coerced to array with same number of
  dimensions and same index sets as array \a x
*/
function array[$T] of var opt $V: arrayXd(array[$T] of var opt $X: x, array[$U] of var opt $V: y);

/** @group builtins.array Return row \a r of array \a x */
function array[$$E] of $T: row(array[int, $$E] of $T: x, int: r) =
  array1d(index_set_2of2(x), [x[r,i] | i in index_set_2of2(x)]);
/** @group builtins.array Return row \a r of array \a x */
function array[$$E] of var $T: row(array[int, $$E] of var $T: x, int: r) =
  array1d(index_set_2of2(x), [x[r,i] | i in index_set_2of2(x)]);
/** @group builtins.array Return row \a r of array \a x */
function array[$$E] of var opt $T: row(array[int, $$E] of var opt $T: x, int: r) =
  array1d(index_set_2of2(x), [x[r,i] | i in index_set_2of2(x)]);

/** @group builtins.array Return column \a c of array \a x */
function array[$$E] of $T: col(array[$$E,int] of $T: x, int: c) =
  array1d(index_set_1of2(x), [x[i,c] | i in index_set_1of2(x)]);
/** @group builtins.array Return column \a c of array \a x */
function array[$$E] of var $T: col(array[$$E,int] of var $T: x, int: c) =
  array1d(index_set_1of2(x), [x[i,c] | i in index_set_1of2(x)]);
/** @group builtins.array Return column \a c of array \a x */
function array[$$E] of var opt $T: col(array[$$E,int] of var opt $T: x, int: c) =
  array1d(index_set_1of2(x), [x[i,c] | i in index_set_1of2(x)]);

/** @group builtins.array Test if \a i is in the index set of \a x */
test has_index(int: i, array[int] of var opt $T: x) = i in index_set(x);
/** @group builtins.array Test if \a e is an element of array \a x */
test has_element($T: e, array[int] of $T: x) = exists (i in index_set(x)) (x[i]=e);
/** @group builtins.array Test if \a e is an element of array \a x */
predicate has_element($T: e, array[$$E] of var opt $T: x) = exists (i in index_set(x)) (x[i]=e);

/***
  @groupdef builtins.sort Array sorting operations
*/

/** @group builtins.sort Return array \a x sorted by the values in \a y in non-decreasing order 

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of var opt $T: sort_by(array[$$E] of var opt $T: x, array[$$E] of int: y);
/** @group builtins.sort Return array \a x sorted by the values in \a y in non-decreasing order

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of var $T: sort_by(array[$$E] of var $T: x, array[$$E] of int: y);
/** @group builtins.sort Return array \a x sorted by the values in \a y in non-decreasing order

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of $T: sort_by(array[$$E] of $T: x, array[$$E] of int: y);

/** @group builtins.sort Return array \a x sorted by the values in \a y in non-decreasing order 

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of var opt $T: sort_by(array[$$E] of var opt $T: x, array[$$E] of float: y);
/** @group builtins.sort Return array \a x sorted by the values in \a y in non-decreasing order

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of var $T: sort_by(array[$$E] of var $T: x, array[$$E] of float: y);
/** @group builtins.sort Return array \a x sorted by the values in \a y in non-decreasing order

The sort is stable, i.e. if \a y[\p i] = \a y[\p j] with \p i < \p j, then
\a x[\p i] will appear in the output before \a x[\p j].
*/
function array[$$E] of $T: sort_by(array[$$E] of $T: x, array[$$E] of float: y);

/** @group builtins.sort Return values from array \a x sorted in non-decreasing order */
function array[$$E] of int: sort(array[$$E] of int: x);
/** @group builtins.sort Return values from array \a x sorted in non-decreasing order */
function array[$$E] of float: sort(array[$$E] of float: x);
/** @group builtins.sort Return values from array \a x sorted in non-decreasing order */
function array[$$E] of bool: sort(array[$$E] of bool: x);

/** @group builtins.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of $$E: arg_sort(array[$$E] of int:x) =
  sort_by([i | i in index_set(x)], x);

/** @group builtins.sort
Returns the permutation \a p which causes \a x to be in sorted order hence
\a x[\a p[\p i]] <= \a x[\a p[\p i+1]].

The permutation is the stable sort hence \a x[\a p[\p i]] = \a x[\a p[\p i+1]] \(\rightarrow\) \a p[\p i] < \a p[\p i+1].
*/
function array[int] of $$E: arg_sort(array[$$E] of float:x) =
  sort_by([i | i in index_set(x)], x);

/***
  @groupdef builtins.coercion Coercions
  
  These functions implement coercions, or channeling, between different types.
*/

/** @group builtins.coercion Return \( \lceil{ \a x} \rceil \) */
function int: ceil(float: x);
/** @group builtins.coercion Return \( \lfloor{ \a x} \rfloor \) */
function int: floor(float: x);
/** @group builtins.coercion Return \a x rounded to nearest integer */
function int: round(float: x);

/** @group builtins.coercion Return Boolean \a b coerced to an integer  */
function int: bool2int(bool: b);
/** @group builtins.coercion Return Boolean \a b coerced to a float  */
function float: bool2float(bool: b) = if b then 1.0 else 0.0 endif;
/** @group builtins.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of float: bool2float(array[$T] of bool: x) ::promise_total =
  let {
    array[int] of bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);
/** @group builtins.coercion Return array of Booleans \a x coerced to an array of floats  */
function array[$T] of var float: bool2float(array[$T] of var bool: x) ::promise_total =
  let {
    array[int] of var bool: xx = array1d(x)
  } in arrayXd(x,[bool2float(xx[i]) | i in index_set(xx)]);
/** @group builtins.coercion Return Boolean \a b coerced to an integer  */
function var int: bool2int(var bool: b);
/** @group builtins.coercion Return array of Booleans \a b coerced to an array of integers  */
function array[$$E] of var int: bool2int(array[$$E] of var bool: b);
/** @group builtins.coercion Return Boolean \a b coerced to a float  */
function var float: bool2float(var bool: b) = int2float(bool2int(b));
/** @group builtins.coercion Return integer \a x coerced to a float  */
function float: int2float(int: x);
/** @group builtins.coercion Return integer \a x coerced to a float  */
function var float: int2float(var int: x) ::promise_total =
  let { var int2float(lb(x))..int2float(ub(x)): y;
        constraint int2float(x,y); } in y;

function set of int: bool2int(set of bool: b) =
  if b={false,true} then {0,1}
  elseif b={false} then {0}
  elseif b={true} then {1}
  else {} endif;

/** @group builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of int: bool2int(array[$T] of bool: x) ::promise_total =
  let {
    array[int] of bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group builtins.coercion Return array of sets of Booleans \a x coerced to an array of sets of integers  */
function array[$T] of set of int: bool2int(array[$T] of set of bool: x) ::promise_total =
  let {
    array[int] of set of bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of var int: bool2int(array[$T] of var bool: x) ::promise_total =
  let {
    array[int] of var bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group builtins.coercion Return array of Booleans \a x coerced to an array of integers  */
function array[$T] of var opt int: bool2int(array[$T] of var opt bool: x) ::promise_total =
  let {
    array[int] of var opt bool: xx = array1d(x)
  } in arrayXd(x,[bool2int(xx[i]) | i in index_set(xx)]);

/** @group builtins.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of float: int2float(array[$T] of int: x) ::promise_total =
  let {
    array[int] of int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

/** @group builtins.coercion Return array of integers \a x coerced to an array of floats  */
function array[$T] of var float: int2float(array[$T] of var int: x) ::promise_total =
  let {
    array[int] of var int: xx = array1d(x)
  } in arrayXd(x,[int2float(xx[i]) | i in index_set(xx)]);

% Only supported for set of int:
% function array[int] of $T: set2array(set of $T);
/** @group builtins.coercion Return a set of integers \a x coerced to an array of integers  */
function array[int] of $$E: set2array(set of $$E: x);


/***
  @groupdef builtins.string String operations
  
  These functions implement operations on strings.
*/

/** @group builtins.string Convert \a x into a string */
function string: show(var opt set of $T: x);
/** @group builtins.string Convert \a x into a string */
function string: show(var opt $T: x);
/** @group builtins.string Convert \a x into a string */
function string: show(array[$U] of var opt $T: x);

function string: showDzn(var opt set of $T: x);
function string: showDzn(var opt $T: x);
function string: showDzn(array[$U] of var opt $T: x);


/** @group builtins.string Formatted to-string conversion for integers

Converts the integer \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: show_int(int: w, var int: x);
/** @group builtins.string Formatted to-string conversion for floats.

Converts the float \a x into a string right justified
by the number of characters given by \a w, or left justified if
\a w is negative. The number of digits to appear after the decimal
point is given by \a p. It is a run-time error for \a p to be negative.
*/
function string: show_float(int: w, int: p, var float: x);

/** @group builtins.string Convert two-dimensional array \a x into a string */
function string: show2d(array[int,int] of var opt $T: x) =
  let { int: rows=card(index_set_1of2(x));
        int: cols=card(index_set_2of2(x));
        array[int] of string: s = [show(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)];
        int: max_length = max([string_length(s[i]) | i in index_set(s)])
  } in
  "[| "++
  concat([format(max_length,s[(i-1)*cols+j])++
   if j<cols then ", "
   elseif i<rows then " |\n   " else " |]\n"
   endif | i in 1..rows, j in 1..cols
  ]);

/** @group builtins.string Convert three-dimensional array \a x into a string */
function string: show3d(array[int,int,int] of var opt $T: x) =
  let { int: len1=card(index_set_1of3(x));
        int: len2=card(index_set_2of3(x));
        int: len3=card(index_set_3of3(x));
        array[int] of string: s = [show(x[i,j,k]) | i in index_set_1of3(x),
                                                    j in index_set_2of3(x),
                                                    k in index_set_3of3(x)];
        int: max_length = max([string_length(s[i]) | i in index_set(s)])
  } in
  "[| | "++
  concat([format(max_length,s[(i-1)*len2*len3+(j-1)*len3+k])++
   if k<len3 then ", "
   elseif j<len2 then " |\n     "
   elseif i<len1 then " |,\n\n   | "
   else " | |]\n"
   endif | i in 1..len1, j in 1..len2, k in 1..len3
  ]);

/** @group builtins.string Convert \a x into JSON string */
function string: showJSON(var opt $T: x);

/** @group builtins.string Convert \a x into JSON string */
function string: showJSON(array[$U] of var opt $T: x);

/** @group builtins.string Return length of \a s */
function int: string_length(string: s);

/** @group builtins.string Return concatenation of \a s1 and \a s2 */
function string: '++'(string: s1, string: s2);
/** @group builtins.string Return concatenation of strings in array \a s */
function string: concat(array[$T] of string: s);
/** @group builtins.string Join string in array \a s using delimiter \a d */
function string: join(string: d, array[$T] of string: s);

/** @group builtins.string Convert \a x into a string */
function string: format(var opt $T: x) = show(x);
/** @group builtins.string Convert \a x into a string */
function string: format(var opt set of $T: x) = show(x);
/** @group builtins.string Convert \a x into a string */
function string: format(array[$U] of var opt $T: x) = show(x);

/** @group builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p, or the maximum number of digits after the decimal point for floating
point numbers. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, var opt $T: x);
/** @group builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, var opt set of $T: x);
/** @group builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.

The maximum length of the string representation of \a x is given by
\a p. It is a run-time error for \a p to be negative.
*/
function string: format(int: w, int: p, array[$U] of var opt $T: x);

/** @group builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, var opt $T: x);
/** @group builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, var opt set of $T: x);
/** @group builtins.string Formatted to-string conversion

Converts the value \a x into a string right
justified by the number of characters given by \a w, or left
justified if \a w is negative.
*/
function string: format(int: w, array[$U] of var opt $T: x);

/** @group builtins.string Return path of file where this function is called */
function string: file_path();

/***
  @groupdef builtins.reflect Reflection operations
  
  These functions return information about declared or inferred variable
  bounds and domains.
*/

/** @group builtins.reflect Return lower bound of \a x */
function int: lb(var int: x);
/** @group builtins.reflect Return upper bound of \a x */
function int: ub(var int: x);
/** @group builtins.reflect Return lower bound of \a x */
function int: lb(var opt int: x);
/** @group builtins.reflect Return upper bound of \a x */
function int: ub(var opt int: x);
/** @group builtins.reflect Return lower bound of \a x */
function float: lb(var float: x);
/** @group builtins.reflect Return upper bound of \a x */
function float: ub(var float: x);
/** @group builtins.reflect Return lower bound of \a x */
function set of int: lb(var set of int: x);
/** @group builtins.reflect Return upper bound of \a x */
function set of int: ub(var set of int: x);
/** @group builtins.reflect Return array of lower bounds of the elements in array \a x */
function array[$U] of int: lb(array[$U] of var int: x) =
  arrayXd(x,[lb(xx) | xx in array1d(x)]);
/** @group builtins.reflect Return array of upper bounds of the elements in array \a x */
function array[$U] of int: ub(array[$U] of var int: x) =
  arrayXd(x,[ub(xx) | xx in array1d(x)]);
/** @group builtins.reflect Return array of lower bounds of the elements in array \a x */
function array[$U] of float: lb(array[$U] of var float: x) =
  arrayXd(x,[lb(xx) | xx in array1d(x)]);
/** @group builtins.reflect Return array of upper bounds of the elements in array \a x */
function array[$U] of float: ub(array[$U] of var float: x) =
  arrayXd(x,[ub(xx) | xx in array1d(x)]);
/** @group builtins.reflect Return array of lower bounds of the elements in array \a x */
function array[$U] of set of int: lb(array[$U] of var set of int: x) =
  arrayXd(x,[lb(xx) | xx in array1d(x)]);
/** @group builtins.reflect Return array of upper bounds of the elements in array \a x */
function array[$U] of set of int: ub(array[$U] of var set of int: x) =
  arrayXd(x,[ub(xx) | xx in array1d(x)]);
/** @group builtins.reflect Return minimum of all lower bounds of the elements in array \a x */
function int: lb_array(array[$U] of var opt int: x);
/** @group builtins.reflect Return maximum of all upper bounds of the elements in array \a x */
function int: ub_array(array[$U] of var opt int: x);
/** @group builtins.reflect Return minimum of all lower bounds of the elements in array \a x */
function float: lb_array(array[$U] of var float: x);
/** @group builtins.reflect Return maximum of all upper bounds of the elements in array \a x */
function float: ub_array(array[$U] of var float: x);
/** @group builtins.reflect Return intersection of all lower bounds of the elements in array \a x */
function set of int: lb_array(array[$U] of var set of int: x);
/** @group builtins.reflect Return union of all upper bounds of the elements in array \a x */
function set of int: ub_array(array[$U] of var set of int: x);
/** @group builtins.reflect Return domain of \a x */
function set of int: dom(var int: x);
function set of int: dom(var bool: b) =
  if is_fixed(b) then if fix(b) then {1} else {0} endif else {0,1} endif;

/** @group builtins.reflect Return union of all domains of the elements in array \a x */
function set of int: dom_array(array[$T] of var int: x);
/** @group builtins.reflect Return approximation of union of all domains of the elements in array \a x */
function set of int: dom_bounds_array(array[$T] of var int: x);
/** @group builtins.reflect Return cardinality of the domain of \a x */
function int: dom_size(var int: x) = card(dom(x));

/** @group builtins.reflect Test if variable \a x has declared, finite bounds */
function par bool: has_bounds(var int: x);
/** @group builtins.reflect Test if variable \a x has declared, finite bounds */
function par bool: has_bounds(var float: x);
/** @group builtins.reflect Test if variable \a x has a declared, finite upper bound */
function par bool: has_ub_set(var set of int: x);

/** @group builtins.reflect Check if the value of \a x is fixed at this point
  in evaluation. If it is fixed, return its value, otherwise abort. */
function $T: fix(var opt $T: x);
/** @group builtins.reflect Check if the value of every element of the array \a x is fixed
at this point in evaluation. If all are fixed, return an array of their values, otherwise abort. */
function array[$U] of $T: fix(array[$U] of var opt $T: x);
/** @group builtins.reflect Test if \a x is fixed */
function bool: is_fixed(var opt $T: x);
/** @group builtins.reflect Test if every element of array \a x is fixed */
function bool: is_fixed(array[$U] of var opt $T: x);

/***
  @groupdef builtins.debug Assertions and debugging functions
  
  These functions help debug models and check that input data conforms to
  the expectations.
*/

/** @group builtins.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function $T: assert(bool: b, string: msg, $T: x);
/** @group builtins.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function var $T: assert(bool: b, string: msg, var $T: x);
/** @group builtins.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function var opt $T: assert(bool: b, string: msg, var opt $T: x);

/** @group builtins.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of $T: assert(bool: b, string: msg, array[$U] of $T: x);
/** @group builtins.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of var $T: assert(bool: b, string: msg, array[$U] of var $T: x);
/** @group builtins.debug If \a b is true, return \a x, otherwise abort with message \a msg. */
function array[$U] of var opt $T: assert(bool: b, string: msg, array[$U] of var opt $T: x);

/** @group builtins.debug If \a b is true, return true, otherwise abort with message \a msg. */
function bool: assert(bool: b, string: msg);
/** @group builtins.debug Return \a x, and print message \a msg. */
function $T: trace(string: msg, $T: x);
/** @group builtins.debug Return \a x, and print message \a msg. */
function var $T: trace(string: msg, var $T: x);
/** @group builtins.debug Return \a x, and print message \a msg. */
function var opt $T: trace(string: msg, var opt $T: x);
/** @group builtins.debug Return true, and print message \a msg. */
function bool: trace(string: msg);

/** @group builtins.debug Return \a x, and print message \a msg. */
function $T: trace_stdout(string: msg, $T: x);
/** @group builtins.debug Return \a x, and print message \a msg. */
function var $T: trace_stdout(string: msg, var $T: x);
/** @group builtins.debug Return \a x, and print message \a msg. */
function var opt $T: trace_stdout(string: msg, var opt $T: x);
/** @group builtins.debug Return true, and print message \a msg. */
function bool: trace_stdout(string: msg);

/** @group builtins.debug Abort evaluation and print message \a msg. */
function bool: abort(string: msg);

/***
  @groupdef builtins.enum Functions for enums
  
*/

/** @group builtins.enum Return next greater enum value of \a x */
function $$E: enum_next(set of $$E: e, $$E: x);
/** @group builtins.enum Return next greater enum value of \a x */
function var $$E: enum_next(set of $$E: e, var $$E: x) =
  let { constraint x < max(e) } in x+1;

/** @group builtins.enum Return next smaller enum value of \a x */
function $$E: enum_prev(set of $$E: e, $$E: x);
/** @group builtins.enum Return next smaller enum value of \a x */
function var $$E: enum_prev(set of $$E: e, var $$E: x) =
  let { constraint x > min(e) } in x-1;

/** @group builtins.enum Convert \a x to enum type \a X */
function $$E: to_enum(set of $$E: X, int: x);
/** @group builtins.enum Convert \a x to enum type \a X */
function var $$E: to_enum(set of $$E: X, var int: x) =
  let { constraint x in X } in x;




%-----------------------------------------------------------------------------%
%
% Internal compiler functions
%
% These functions are used internally by the compiler.
%

% domain constraints
predicate var_dom(var int:x, set of int: s) =
  if has_bounds(x) /\ dom(x) subset s then true
  else x in s
  endif;
predicate var_dom(var set of int: x, set of int: s) =
  if has_ub_set(x) /\ ub(x) subset s then true
  else set_subset(x,s)
  endif;
predicate var_dom(var float:x, float: l, float: u) =
  if has_bounds(x) /\ lb(x) >= l /\ ub(x) <= u then true
  else x >= l /\ x <= u
  endif;
predicate var_dom(var float:x, set of float: d) =
  x in d;
test var_dom(float:x, float: l, float: u) =
  x >= l /\ x <= u;
test var_dom(float:x, set of float: d) =
  x in d;

predicate var_dom(array[$T] of var set of int: x, set of int: d) =
    let { array[int] of var set of int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
predicate var_dom(array[$T] of var int: x, set of int: d) =
    let { array[int] of var int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
predicate var_dom(array[$T] of var float: x, float: l, float: u) =
    let { array[int] of var float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],l,u));
predicate var_dom(array[$T] of var float: x, set of float: d) =
    let { array[int] of var float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

test var_dom(array[$T] of set of int: x, set of int: d) =
    let { array[int] of set of int: xx = array1d(x) }
    in forall (i in index_set(xx)) (xx[i] subset d);
test var_dom(array[$T] of int: x, set of int: d) =
    let { array[int] of int: xx = array1d(x) }
    in forall (i in index_set(xx)) (xx[i] in d);
test var_dom(array[$T] of float: x, float: l, float: u) =
    let { array[int] of float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],l,u));
test var_dom(array[$T] of float: x, set of float: d) =
    let { array[int] of float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

function var set of int: array2set(array[int] of var int: x) ::promise_total =
    let {
      var set of int: y = array_union([
        let { var set of dom(x[i]): s;
        constraint x[i] in s /\ card(s)=1;
        } in s | i in index_set(x)]);
    } in y;

function set of int: array2set(array[int] of int: x) =
  { x[i] | i in index_set(x) };

predicate array_var_int_element(var int: x, array[int] of int: y, var int: z) =
  array_int_element(x,y,z);
predicate array_var_bool_element(var int: x, array[int] of bool: y, var bool: z) =
  array_bool_element(x,y,z);
predicate array_var_float_element(var int: x, array[int] of float: y, var float: z) =
  array_float_element(x,y,z);
predicate array_var_set_element(var int: x, array[int] of set of int: y, var set of int: z) =
  array_set_element(x,y,z);

predicate bool_xor_reif(var bool: a, var bool: b, var bool: c) =
  bool_xor(a,b,c);

predicate xorall_reif(array[int] of var bool: b, var bool: c) =
  let { var bool: nc ::is_defined_var; constraint xorall([nc]++b) ::defines_var(nc); } in c = not nc;

function var int: lin_exp(array[int] of int, array[int] of var int, int);
function var float: lin_exp(array[int] of float, array[int] of var float, float);

test mzn_in_root_context(var $T);

test mzn_in_redundant_constraint();

%-----------------------------------------------------------------------------%
%
% Element constraint implementations
%
% MiniZinc compiles element constraints using a series of intermediate
% functions that test whether the constraint is total and perform array slicing
% for multi-dimensional element constraints.
%

%%%%%%%%%%%%%%%%%%%
% Element on ints

function var int: element_t(var int: idx, array[int] of var int: x) :: promise_total =
  let {
    var dom_bounds_array(x): r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_int_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var int: element_mt(var int: idx, array[int] of var int: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_int_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var int: element(var int: idx, array[int] of var int: x) =
  if mzn_in_root_context(idx) then let {
    constraint idx in index_set(x)
  } in element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var int: element(var int: idx1, var int: idx2,
                          array[int,int] of var int: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       elseif
       ((has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
        /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))))
       then
         element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3,
                          array[int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))))
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                          array[int,int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of4(x);
           constraint idx2 in index_set_2of4(x);
           constraint idx3 in index_set_3of4(x);
           constraint idx4 in index_set_4of4(x);
         } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

%%%%%%%%%%%%%%%%%%%
% Element on floats

function var float: element_t(var int: idx, array[int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_float_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var float: element_mt(var int: idx, array[int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_float_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var float: element(var int: idx, array[int] of var float: x) =
  if mzn_in_root_context(idx) then let {
    constraint idx in index_set(x)
  } in element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var float: element(var int: idx1, var int: idx2,
                            array[int,int] of var float: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))) )
       then
         element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3,
                            array[int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))) )
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;
                      
%%%%%%%%%%%%%%%%%
% Element on sets

function var set of int: element_t(var int: idx, array[int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_set_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var set of int: element_mt(var int: idx, array[int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_set_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var set of int: element(var int: idx, array[int] of var set of int: x) =
  if mzn_in_root_context(idx) then let {
    constraint idx in index_set(x)
  } in element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var set of int: element(var int: idx1, var int: idx2,
                                 array[int,int] of var set of int: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))) )
       then
         element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3,
                                 array[int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))) )
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                                 array[int,int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

%%%%%%%%%%%%%%%%%%
% Element on bools

function var bool: element_t(var int: idx, array[int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_bool_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var bool: element_mt(var int: idx, array[int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_bool_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var bool: element(var int: idx, array[int] of var bool: x) =
  if mzn_in_root_context(idx) then
    idx in index_set(x) /\ element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else idx in index_set(x) /\ element_mt(idx,x)
  endif;

function var bool: element(var int: idx1, var int: idx2,
                          array[int,int] of var bool: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))) )
       then
         element_t( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt( (idx1*dim+idx2-min_flat)::domain, array1d(x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3,
                            array[int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))) )
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

%-----------------------------------------------------------------------------%
%
% Internal functions for implementing div, mod etc

function set of int:compute_div_bounds(var int: x, var int: y);

function var int: div_t(var int: x, var int: y) :: promise_total =
  let {
    var (compute_div_bounds(x,y)): z ::is_defined_var;
    constraint y != 0;
    constraint int_div(x,y,z) ::defines_var(z); } in z;

function var int: div_mt(var int: x, var int: y) :: promise_total =
  let {
    var ((dom(y) diff {0}) union {1}): yy = if y=0 then 1 else y endif;
  } in div_t(x,yy);  

function var int: mod_t(var int: x, var int: y) :: promise_total =
  let { var -(max(ub(y),-lb(y)))..max(ub(y),-lb(y)): z;
        constraint y != 0;
        constraint int_mod(x,y,z); } in z;

function var int: mod_mt(var int: x, var int: y) :: promise_total =
  let {
    var {1} union dom(y): yy = if y=0 then 1 else y endif;
  } in mod_t(x,yy);

function var int:   product_rec(array[int] of var int: x) =
  if length(x)=0 then 1
  elseif length(x)=1 then x[min(index_set(x))]
  else x[min(index_set(x))]*
       product_rec([x[i] | i in min(index_set(x))+1..max(index_set(x))])
  endif;

function var float:   product_rec(array[int] of var float: x) =
  if length(x)=0 then 1.0
  elseif length(x)=1 then x[min(index_set(x))]
  else x[min(index_set(x))]*
       product_rec([x[i] | i in min(index_set(x))+1..max(index_set(x))])
  endif;

function var int: max_t(array[int] of var int: x) :: promise_total =
  if length(x)=0 then 0
  elseif length(x)=1 then x[min(index_set(x))]
  elseif length(x)=2 then max(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m;
    constraint array_int_maximum(m,x);
  } in m
  endif;

function var int: min_t(array[int] of var int: x) :: promise_total =
  if length(x)=0 then 0
  elseif length(x)=1 then x[1]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m;
    constraint array_int_minimum(m,x);
  } in m
  endif;

function var float: max_t(array[int] of var float: x) :: promise_total =
  if length(x)=0 then 0.0
  elseif length(x)=1 then x[min(index_set(x))]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var float: m;
    constraint array_float_maximum(m,x);
  } in m
  endif;
  
function var float: min_t(array[int] of var float: x) :: promise_total =
  if length(x)=0 then 0.0
  elseif length(x)=1 then x[1]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var float: m;
    constraint array_float_minimum(m,x);
  } in m
  endif;
  
/***
  @groupdef builtins.random Random Number Generator builtins
  
  These functions implement random number generators from different 
 probability distributions.
*/  

/** @group builtins.random Return a sample from the normal distribution defined by \(\a mean, \a std\) */
function float: normal(float: mean, float: std);  

/** @group builtins.random Return a sample from the normal distribution defined by \(\a mean, \a std\) */
function float: normal(int: mean, float: std);  

/** @group builtins.random Return a sample from the uniform distribution defined by \(\a lowerbound, \a upperbound\) */
function float: uniform(float: lowerbound, float: upperbound);  

/** @group builtins.random Return a sample from the uniform distribution defined by \(\a lowerbound, \a upperbound\) */
function int: uniform(int: lowerbound, int: upperbound);  

/** @group builtins.random Return a sample from the poisson distribution defined by \a mean */
function int: poisson(float: mean);

/** @group builtins.random Return a sample from the poisson distribution defined by an integer \a mean */
function int: poisson(int: mean);

/** @group builtins.random Return a sample from the gamma distribution defined by \(\a alpha, \a beta\) */
function float: gamma(float: alpha, float: beta);

/** @group builtins.random Return a sample from the gamma distribution defined by \(\a alpha, \a beta\) */
function float: gamma(int: alpha, float: beta);

/** @group builtins.random Return a sample from the Weibull distribution defined by \(\a shape, \a scale\) */
function float: weibull(float: shape, float: scale);

/** @group builtins.random Return a sample from the Weibull distribution defined by \(\a shape, \a scale\) */
function float: weibull(int: shape, float: scale);

/** @group builtins.random Return a sample from the exponential distribution defined by \(\a lambda\) */
function float: exponential(int: lambda);

/** @group builtins.random Return a sample from the exponential distribution defined by \(\a lambda\) */
function float: exponential(float: lambda);

/** @group builtins.random Return a sample from the lognormal distribution defined by \(\a mean, \a std\) */
function float: lognormal(float: mean, float: std);  

/** @group builtins.random Return a sample from the lognormal distribution defined by \(\a mean, \a std\) */
function float: lognormal(int: mean, float: std);  

/** @group builtins.random Return a sample from the chi-squared distribution defined by the degree of freedom \(\a n\) */
function float: chisquared(int: n);

/** @group builtins.random Return a sample from the chi-squared distribution defined by the degree of freedom \(\a n\) */
function float: chisquared(float: n);

/** @group builtins.random Return a sample from the cauchy distribution defined by \(\a mean, \a scale\) */
function float: cauchy(float: mean, float: scale);

/** @group builtins.random Return a sample from the cauchy distribution defined by \(\a mean, \a scale\) */
function float: cauchy(int: mean, float: scale);

/** @group builtins.random Return a sample from the Fisher-Snedecor F-distribution defined by the degrees of freedom \(\a d1, \a d2\) */
function float: fdistribution(float: d1, float: d2);

/** @group builtins.random Return a sample from the Fisher-Snedecor F-distribution defined by the degrees of freedom \(\a d1, \a d2\) */
function float: fdistribution(int: d1, int: d2);

/** @group builtins.random Return a sample from the student's t-distribution defined by the sample size \(\a n\) */
function float: tdistribution(float: n);

/** @group builtins.random Return a sample from the student's t-distribution defined by the sample size \(\a n\) */
function float: tdistribution(int: n);

/** @group builtins.random Return a sample from the discrete distribution defined by the array of weights \(\a weights\) that assigns a weight to each integer starting from zero */
function int: discrete_distribution(array[int] of int: weights);

/** @group builtins.random Return a boolean sample from the Bernoulli distribution defined by probability \(\a p\) */
function bool: bernoulli(float: p);

/** @group builtins.random Return a sample from the binomial distribution defined by sample number \a t and probability \a p */
function int: binomial(int: t, float: p);  

/***
  @groupdef builtins.special Special constraints
  
  These predicates allow users to mark constraints as e.g. symmetry breaking
  or redundant, so that solvers can choose to implement them differently.
  
  We cannot easily use annotations for this purpose, since annotations are
  propagated to all constraints in a decomposition, which may be incorrect
  for redundant or symmetry breaking constraints in the presence of
  common subexpression elimination (CSE).
*/  

/** @group builtins.special Mark \a b as a symmetry breaking constraint */
predicate symmetry_breaking_constraint(var bool: b);

/** @group builtins.special Mark \a b as a redundant constraint */
predicate redundant_constraint(var bool: b);

/** @group builtins.special Mark \a b as an implied constraint (synonym for redundant_constraint) */
predicate implied_constraint(var bool: b) = redundant_constraint(b);

function set of int: anon_enum(int: n) = 1..n;

/***
  @groupdef builtins.language Language information
  
  These functions return information about the MiniZinc system.
*/  

/** @group builtins.language Return MiniZinc version encoded as an integer (major*10000+minor*1000+patch). */
function int: mzn_compiler_version();

/** @group builtins.language Return string representation of \a v given an integer major*10000+minor*1000+patch
*/
function string: mzn_version_to_string(int: v) =
  show(v div 10000)++"."++show((v div 100) mod 100)++"."++show(v mod 100);

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

% Include solver-specific redefinitions for any FlatZinc built-ins.
%
include "redefinitions.mzn";
include "redefinitions-2.0.mzn";
include "redefinitions-2.0.2.mzn";
include "redefinitions-2.1.mzn";
include "redefinitions-2.1.1.mzn";

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
