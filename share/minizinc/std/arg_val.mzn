include "fzn_arg_val_bool.mzn";
include "fzn_arg_val_bool_opt.mzn";
include "fzn_arg_val_float.mzn";
include "fzn_arg_val_float_opt.mzn";
include "fzn_arg_val_int.mzn";
include "fzn_arg_val_int_opt.mzn";

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function $$E: arg_val(array [$$E] of $T: x, $T: v) =
  let { any: matches = [i | i in index_set(x) where x[i] = v] } in matches[1];

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function opt $$E: arg_val_weak(array [$$E] of $T: x, $T: v) =
  let {
    any: matches = [i | i in index_set(x) where x[i] = v];
  } in if length(matches) = 0 then <> else matches[1] endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function var $$E: arg_val(array [$$E] of var bool: x, var bool: v) =
  let {
    constraint length(x) > 0;
  } in to_enum_internal(enum_of(index_set(x)), arg_val_total(index2int(x), v));

function var int: arg_val_total(array [int] of var bool: x, var bool: v) :: promise_total =
  if length(x) = 0 then
    min(index_set(x))
  else
    let {
      var index_set(x): i;
      constraint fzn_arg_val_bool(x, v, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function var opt $$E: arg_val_weak(array [$$E] of var bool: x, var bool: v) =
  to_enum_internal(enum_of(index_set(x)), arg_val_weak_bool(index2int(x), v));
function var opt int: arg_val_weak_bool(array [int] of var bool: x, var bool: v) :: promise_total =
  if length(x) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(x))..max(index_set(x)) + 1;
      any: xx = array1d(idx, x ++ [v]);
      var idx: i;
      var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
      constraint fzn_arg_val_bool(xx, v, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function var $$E: arg_val(array [$$E] of var opt bool: x, var opt bool: v) =
  let {
    constraint length(x) > 0;
  } in to_enum_internal(enum_of(index_set(x)), arg_val_total(index2int(x), v));

function var int: arg_val_total(array [int] of var opt bool: x, var opt bool: v) :: promise_total =
  if length(x) = 0 then
    min(index_set(x))
  else
    let {
      var index_set(x): i;
      constraint fzn_arg_val_bool_opt(x, v, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function var opt $$E: arg_val_weak(array [$$E] of var opt bool: x, var opt bool: v) =
  to_enum_internal(enum_of(index_set(x)), arg_val_weak_opt_bool(index2int(x), v));
function var opt int: arg_val_weak_opt_bool(array [int] of var opt bool: x, var opt bool: v)
  :: promise_total =
  if length(x) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(x))..max(index_set(x)) + 1;
      any: xx = array1d(idx, x ++ [v]);
      var idx: i;
      var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
      constraint fzn_arg_val_bool_opt(xx, v, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function var $$E: arg_val(array [$$E] of var float: x, var float: v) =
  let {
    constraint length(x) > 0;
  } in to_enum_internal(enum_of(index_set(x)), arg_val_total(index2int(x), v));

function var int: arg_val_total(array [int] of var float: x, var float: v) :: promise_total =
  if length(x) = 0 then
    min(index_set(x))
  else
    let {
      var index_set(x): i;
      constraint fzn_arg_val_float(x, v, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function var opt $$E: arg_val_weak(array [$$E] of var float: x, var float: v) =
  to_enum_internal(enum_of(index_set(x)), arg_val_weak_float(index2int(x), v));
function var opt int: arg_val_weak_float(array [int] of var float: x, var float: v)
   :: promise_total =
  if length(x) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(x))..max(index_set(x)) + 1;
      any: xx = array1d(idx, x ++ [v]);
      var idx: i;
      var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
      constraint fzn_arg_val_float(xx, v, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function var $$E: arg_val(array [$$E] of var opt float: x, var float: v) =
  let {
    constraint length(x) > 0;
  } in to_enum_internal(enum_of(index_set(x)), arg_val_total(index2int(x), v));

function var int: arg_val_total(array [int] of var opt float: x, var opt float: v)
  :: promise_total =
  if length(x) = 0 then
    min(index_set(x))
  else
    let {
      var index_set(x): i;
      constraint fzn_arg_val_float_opt(x, v, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function var opt $$E: arg_val_weak(array [$$E] of var opt float: x, var opt float: v) =
  to_enum_internal(enum_of(index_set(x)), arg_val_weak_opt_float(index2int(x), v));
function var opt int: arg_val_weak_opt_float(array [int] of var opt float: x, var opt float: v)
  :: promise_total =
  if length(x) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(x))..max(index_set(x)) + 1;
      any: xx = array1d(idx, x ++ [v]);
      var idx: i;
      var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
      constraint fzn_arg_val_float_opt(xx, v, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function var $$E: arg_val(array [$$E] of var $$V: x, var $$V: v) =
  let {
    constraint length(x) > 0;
  } in to_enum_internal(enum_of(index_set(x)), arg_val_total(index2int(enum2int(x)), enum2int(v)));

function var int: arg_val_total(array [int] of var int: x, var int: v) :: promise_total =
  if length(x) = 0 then
    min(index_set(x))
  else
    let {
      var index_set(x): i;
      constraint fzn_arg_val_int(x, v, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function var opt $$E: arg_val_weak(array [$$E] of var $$V: x, var $$V: v) =
  to_enum_internal(enum_of(index_set(x)), arg_val_weak_int(index2int(enum2int(x)), enum2int(v)));
function var opt int: arg_val_weak_int(array [int] of var int: x, var int: v) :: promise_total =
  if length(x) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(x))..max(index_set(x)) + 1;
      any: xx = array1d(idx, x ++ [v]);
      var idx: i;
      var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
      constraint fzn_arg_val_int(xx, v, i);
    } in ii
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x.
	When breaking ties the least index is returned.
	Note that this function forces the value \a v to occur in \a x.
*/
function var $$E: arg_val(array [$$E] of var opt $$V: x, var opt $$V: v) =
  let {
    constraint length(x) > 0;
  } in to_enum_internal(enum_of(index_set(x)), arg_val_total(index2int(enum2int(x)), enum2int(v)));

function var int: arg_val_total(array [int] of var opt int: x, var opt int: v) :: promise_total =
  if length(x) = 0 then
    min(index_set(x))
  else
    let {
      var index_set(x): i;
      constraint fzn_arg_val_int_opt(x, v, i);
    } in i
  endif;

/** @group globals.math
	Returns the index of the value \a v in the array \a x when \a v.
	When breaking ties the least index is returned.
	Returns absent when \a v does not occur in \a x.
*/
function var opt $$E: arg_val_weak(array [$$E] of var opt $$V: x, var opt $$V: v) =
  to_enum_internal(
    enum_of(index_set(x)),
    arg_val_weak_opt_int(index2int(enum2int(x)), enum2int(v)),
  );
function var opt int: arg_val_weak_opt_int(array [int] of var opt int: x, var opt int: v)
  :: promise_total =
  if length(x) = 0 then
    <>
  else
    let {
      set of int: idx = min(index_set(x))..max(index_set(x)) + 1;
      any: xx = array1d(idx, x ++ [v]);
      var idx: i;
      var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
      constraint fzn_arg_val_int_opt(xx, v, i);
    } in ii
  endif;
