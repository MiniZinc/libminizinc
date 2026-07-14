include "fzn_arg_max_int.mzn";
include "fzn_arg_max_int_opt.mzn";
include "fzn_arg_max_bool.mzn";
include "fzn_arg_max_bool_opt.mzn";
include "fzn_arg_max_float.mzn";
include "fzn_arg_max_float_opt.mzn";

predicate fzn_maximum_arg_bool(array [int] of var bool: xs, var int: idx);
predicate fzn_maximum_arg_bool_opt(array [int] of var opt bool: xs, var int: idx);
predicate fzn_maximum_arg_float(array [int] of var float: xs, var int: idx);
predicate fzn_maximum_arg_float_opt(array [int] of var opt float: xs, var int: idx);
predicate fzn_maximum_arg_int(array [int] of var int: xs, var int: idx);
predicate fzn_maximum_arg_int_opt(array [int] of var opt int: xs, var int: idx);

/** @group globals.math
	Returns the index of the maximum value in the array \a xs.
	When breaking ties the least index is returned.
*/
function var $$E: arg_max(array [$$E] of var $$T: xs) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_max_total(index2int(enum2int(xs))));

/** @group globals.math
	Returns the index of the maximum value in the array \a xs.
	When breaking ties the least index is returned.
*/
function var $$E: arg_max(array [$$E] of var bool: xs) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_max_total(index2int(xs)));

/** @group globals.math
	Returns the index of the maximum value in the array \a xs.
	When breaking ties the least index is returned.
*/
function var $$E: arg_max(array [$$E] of var float: xs) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_max_total(index2int(xs)));

/** @group globals.math
	Returns the index of the maximum non-absent value in the array \a xs.
	When breaking ties the least index is returned.
*/
function var $$E: arg_max(array [$$E] of var opt $$T: xs) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_max_total(index2int(enum2int(xs))));

/** @group globals.math
	Returns the index of the maximum non-absent value in the array \a xs.
	When breaking ties the least index is returned.
	Returns absent when all elements are absent.
*/
function var opt $$E: arg_max_weak(array [$$E] of var opt $$T: xs) =
  to_enum_internal(enum_of(index_set(xs)), arg_max_weak_int(index2int(enum2int(xs))));
function var opt int: arg_max_weak_int(array [int] of var opt int: xs) :: promise_total =
  if length(xs) = 0 then
    <>
  elseif exists (xi in xs) (is_fixed(occurs(xi)) /\ fix(occurs(xi))) then
    let {
      var index_set(xs): i;
      constraint fzn_maximum_arg_int_opt(xs, i);
    } in i
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [lb_array(xs)]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_maximum_arg_int_opt(xx, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the maximum non-absent value in the array \a xs.
	When breaking ties the least index is returned.
*/
function var $$E: arg_max(array [$$E] of var opt bool: xs) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_max_total(index2int(xs)));

/** @group globals.math
	Returns the index of the maximum non-absent value in the array \a xs.
	When breaking ties the least index is returned.
	Returns absent when all elements are absent.
*/
function var opt $$E: arg_max_weak(array [$$E] of var opt bool: xs) =
  to_enum_internal(enum_of(index_set(xs)), arg_max_weak_bool(index2int(xs)));
function var opt int: arg_max_weak_bool(array [int] of var opt bool: xs) :: promise_total =
  if length(xs) = 0 then
    <>
  elseif exists (xi in xs) (is_fixed(occurs(xi)) /\ fix(occurs(xi))) then
    let {
      var index_set(xs): i;
      constraint fzn_maximum_arg_bool_opt(xs, i);
    } in i
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [false]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_maximum_arg_bool_opt(xx, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the maximum non-absent value in the array \a xs.
	When breaking ties the least index is returned.
*/
function var $$E: arg_max(array [$$E] of var opt float: xs) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_max_total(index2int(xs)));

/** @group globals.math
	Returns the index of the maximum non-absent value in the array \a xs.
	When breaking ties the least index is returned.
	Returns absent when all elements are absent.
*/
function var opt $$E: arg_max_weak(array [$$E] of var opt float: xs) =
  to_enum_internal(enum_of(index_set(xs)), arg_max_weak_float(index2int(xs)));
function var opt int: arg_max_weak_float(array [int] of var opt float: xs) :: promise_total =
  if length(xs) = 0 then
    <>
  elseif exists (xi in xs) (is_fixed(occurs(xi)) /\ fix(occurs(xi))) then
    let {
      var index_set(xs): i;
      constraint fzn_maximum_arg_float_opt(xs, i);
    } in i
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [lb_array(xs)]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_maximum_arg_float_opt(xx, i);
    } in ii
  endif;

function var int: arg_max_total(array [int] of var int: xs) :: promise_total =
  if length(xs) = 0 then
    0
  else
    let {
      var min(index_set(xs))..max(index_set(xs)): i;
      constraint fzn_maximum_arg_int(xs, i);
    } in i
  endif;

function var int: arg_max_total(array [int] of var bool: xs) :: promise_total =
  if length(xs) = 0 then
    0
  else
    let {
      var min(index_set(xs))..max(index_set(xs)): i;
      constraint fzn_maximum_arg_bool(xs, i);
    } in i
  endif;

function var int: arg_max_total(array [int] of var float: xs) :: promise_total =
  if length(xs) = 0 then
    0
  else
    let {
      var min(index_set(xs))..max(index_set(xs)): i;
      constraint fzn_maximum_arg_float(xs, i);
    } in i
  endif;

function var int: arg_max_total(array [int] of var opt int: xs) :: promise_total =
  if length(xs) = 0 then
    0
  elseif exists (xi in xs) (is_fixed(occurs(xi)) /\ fix(occurs(xi))) then
    let {
      var index_set(xs): i;
      constraint fzn_maximum_arg_int_opt(xs, i);
    } in i
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [lb_array(xs)]);
      var idx: i;
      var index_set(xs): ii = if i in index_set(xs) then i else min(index_set(xs)) endif;
      constraint fzn_maximum_arg_int_opt(xx, i);
    } in ii
  endif;

function var int: arg_max_total(array [int] of var opt bool: xs) :: promise_total =
  if length(xs) = 0 then
    0
  elseif exists (xi in xs) (is_fixed(occurs(xi)) /\ fix(occurs(xi))) then
    let {
      var index_set(xs): i;
      constraint fzn_maximum_arg_bool_opt(xs, i);
    } in i
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [false]);
      var idx: i;
      var index_set(xs): ii = if i in index_set(xs) then i else min(index_set(xs)) endif;
      constraint fzn_maximum_arg_bool_opt(xx, i);
    } in ii
  endif;

function var int: arg_max_total(array [int] of var opt float: xs) :: promise_total =
  if length(xs) = 0 then
    0
  elseif exists (xi in xs) (is_fixed(occurs(xi)) /\ fix(occurs(xi))) then
    let {
      var index_set(xs): i;
      constraint fzn_maximum_arg_float_opt(xs, i);
    } in i
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [lb_array(xs)]);
      var idx: i;
      var index_set(xs): ii = if i in index_set(xs) then i else min(index_set(xs)) endif;
      constraint fzn_maximum_arg_float_opt(xx, i);
    } in ii
  endif;

/** @group globals.math
	Constrain \a idx to be the index of the maximum value in the array \a xs.
	When breaking ties the least index is returned.

	Assumption: |\a xs| > 0
*/
predicate maximum_arg(array [$$E] of var $$T: xs, var $$E: idx) =
  arg_max(xs) = idx;

/** @group globals.math
	Constrain \a idx to be the index of the maximum value in the array \a xs.
	When breaking ties the least index is returned.

	Assumption: |\a xs| > 0
*/
predicate maximum_arg(array [$$E] of var bool: xs, var $$E: idx) =
  arg_max(xs) = idx;

/** @group globals.math
	Constrain \a idx to be the index of the maximum value in the array \a xs.
	When breaking ties the least index is returned.
    
	Assumption: |\a xs| > 0
*/
predicate maximum_arg(array [$$E] of var float: xs, var $$E: idx) =
  arg_max(xs) = idx;
