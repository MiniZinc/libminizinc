include "fzn_arg_val_bool.mzn";
include "fzn_arg_val_bool_opt.mzn";
include "fzn_arg_val_float.mzn";
include "fzn_arg_val_float_opt.mzn";
include "fzn_arg_val_int.mzn";
include "fzn_arg_val_int_opt.mzn";

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function $$E: arg_val(array [$$E] of $T: xs, $T: value) =
  let { any: matches = [i | i in index_set(xs) where xs[i] = value] } in matches[1];

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function opt $$E: arg_val_weak(array [$$E] of $T: xs, $T: value) =
  let {
    any: matches = [i | i in index_set(xs) where xs[i] = value];
  } in if length(matches) = 0 then <> else matches[1] endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function var $$E: arg_val(array [$$E] of var bool: xs, var bool: value) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_val_total(index2int(xs), value));

function var int: arg_val_total(array [int] of var bool: xs, var bool: value) :: promise_total =
  if length(xs) = 0 then
    min(index_set(xs))
  else
    let {
      var index_set(xs): i;
      constraint fzn_arg_val_bool(xs, value, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function var opt $$E: arg_val_weak(array [$$E] of var bool: xs, var bool: value) =
  to_enum_internal(enum_of(index_set(xs)), arg_val_weak_bool(index2int(xs), value));
function var opt int: arg_val_weak_bool(array [int] of var bool: xs, var bool: value) :: promise_total =
  if length(xs) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [value]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_arg_val_bool(xx, value, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function var $$E: arg_val(array [$$E] of var opt bool: xs, var opt bool: value) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_val_total(index2int(xs), value));

function var int: arg_val_total(array [int] of var opt bool: xs, var opt bool: value) :: promise_total =
  if length(xs) = 0 then
    min(index_set(xs))
  else
    let {
      var index_set(xs): i;
      constraint fzn_arg_val_bool_opt(xs, value, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function var opt $$E: arg_val_weak(array [$$E] of var opt bool: xs, var opt bool: value) =
  to_enum_internal(enum_of(index_set(xs)), arg_val_weak_opt_bool(index2int(xs), value));
function var opt int: arg_val_weak_opt_bool(array [int] of var opt bool: xs, var opt bool: value)
  :: promise_total =
  if length(xs) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [value]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_arg_val_bool_opt(xx, value, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function var $$E: arg_val(array [$$E] of var float: xs, var float: value) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_val_total(index2int(xs), value));

function var int: arg_val_total(array [int] of var float: xs, var float: value) :: promise_total =
  if length(xs) = 0 then
    min(index_set(xs))
  else
    let {
      var index_set(xs): i;
      constraint fzn_arg_val_float(xs, value, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function var opt $$E: arg_val_weak(array [$$E] of var float: xs, var float: value) =
  to_enum_internal(enum_of(index_set(xs)), arg_val_weak_float(index2int(xs), value));
function var opt int: arg_val_weak_float(array [int] of var float: xs, var float: value)
   :: promise_total =
  if length(xs) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [value]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_arg_val_float(xx, value, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function var $$E: arg_val(array [$$E] of var opt float: xs, var float: value) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_val_total(index2int(xs), value));

function var int: arg_val_total(array [int] of var opt float: xs, var opt float: value)
  :: promise_total =
  if length(xs) = 0 then
    min(index_set(xs))
  else
    let {
      var index_set(xs): i;
      constraint fzn_arg_val_float_opt(xs, value, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function var opt $$E: arg_val_weak(array [$$E] of var opt float: xs, var opt float: value) =
  to_enum_internal(enum_of(index_set(xs)), arg_val_weak_opt_float(index2int(xs), value));
function var opt int: arg_val_weak_opt_float(array [int] of var opt float: xs, var opt float: value)
  :: promise_total =
  if length(xs) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [value]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_arg_val_float_opt(xx, value, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function var $$E: arg_val(array [$$E] of var $$V: xs, var $$V: value) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_val_total(index2int(enum2int(xs)), enum2int(value)));

function var int: arg_val_total(array [int] of var int: xs, var int: value) :: promise_total =
  if length(xs) = 0 then
    min(index_set(xs))
  else
    let {
      var index_set(xs): i;
      constraint fzn_arg_val_int(xs, value, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function var opt $$E: arg_val_weak(array [$$E] of var $$V: xs, var $$V: value) =
  to_enum_internal(enum_of(index_set(xs)), arg_val_weak_int(index2int(enum2int(xs)), enum2int(value)));
function var opt int: arg_val_weak_int(array [int] of var int: xs, var int: value) :: promise_total =
  if length(xs) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [value]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_arg_val_int(xx, value, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs.
	When breaking ties the least index is returned.
	Note that this function forces the value \a value to occur in \a xs.
*/
function var $$E: arg_val(array [$$E] of var opt $$V: xs, var opt $$V: value) =
  let {
    constraint length(xs) > 0;
  } in to_enum_internal(enum_of(index_set(xs)), arg_val_total(index2int(enum2int(xs)), enum2int(value)));

function var int: arg_val_total(array [int] of var opt int: xs, var opt int: value) :: promise_total =
  if length(xs) = 0 then
    min(index_set(xs))
  else
    let {
      var index_set(xs): i;
      constraint fzn_arg_val_int_opt(xs, value, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a value in the array \a xs when \a value.
	When breaking ties the least index is returned.
	Returns absent when \a value does not occur in \a xs.
*/
function var opt $$E: arg_val_weak(array [$$E] of var opt $$V: xs, var opt $$V: value) =
  to_enum_internal(
    enum_of(index_set(xs)),
    arg_val_weak_opt_int(index2int(enum2int(xs)), enum2int(value)),
  );
function var opt int: arg_val_weak_opt_int(array [int] of var opt int: xs, var opt int: value)
  :: promise_total =
  if length(xs) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(xs))..max(index_set(xs)) + 1;
      any: xx = array1d(idx, xs ++ [value]);
      var idx: i;
      var opt index_set(xs): ii = if i in index_set(xs) then i else <> endif;
      constraint fzn_arg_val_int_opt(xx, value, i);
    } in ii
  endif;
