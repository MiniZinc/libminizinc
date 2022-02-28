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
function var $$E: arg_val(array [$$E] of var bool: x, var bool: v) =
	let { constraint length(x) > 0; } in arg_val_total(x, v);

function var $$E: arg_val_total(array [$$E] of var bool: x, var bool: v) :: promise_total =
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
function var opt $$E: arg_val_weak(array[$$E] of var bool: x, var bool: v) :: promise_total = 
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
	let { constraint length(x) > 0; } in arg_val_total(x, v);

function var $$E: arg_val_total(array [$$E] of var opt bool: x, var opt bool: v) :: promise_total =
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
function var opt $$E: arg_val_weak(array[$$E] of var opt bool: x, var opt bool: v) :: promise_total = 
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
	let { constraint length(x) > 0; } in arg_val_total(x, v);

function var $$E: arg_val_total(array [$$E] of var float: x, var float: v) :: promise_total =
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
function var opt $$E: arg_val_weak(array[$$E] of var float: x, var float: v) :: promise_total = 
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
	let { constraint length(x) > 0; } in arg_val_total(x, v);

function var $$E: arg_val_total(array [$$E] of var opt float: x, var opt float: v) :: promise_total =
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
function var opt $$E: arg_val_weak(array[$$E] of var opt float: x, var opt float: v) :: promise_total = 
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
	let { constraint length(x) > 0; } in arg_val_total(x, v);

function var $$E: arg_val_total(array [$$E] of var $$V: x, var $$V: v) :: promise_total =
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
function var opt $$E: arg_val_weak(array [$$E] of var $$V: x, var $$V: v) :: promise_total = 
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
	let { constraint length(x) > 0; } in arg_val_total(x, v);

function var $$E: arg_val_total(array [$$E] of var opt $$V: x, var opt $$V: v) :: promise_total =
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
function var opt $$E: arg_val_weak(array [$$E] of var opt $$V: x, var opt $$V: v) :: promise_total = 
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
