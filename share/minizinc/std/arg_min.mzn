include "fzn_arg_min_int.mzn";
include "fzn_arg_min_int_opt.mzn";
include "fzn_arg_min_bool.mzn";
include "fzn_arg_min_bool_opt.mzn";
include "fzn_arg_min_float.mzn";
include "fzn_arg_min_float_opt.mzn";

/** @group globals.math
	Returns the index of the minimum value in the array \a x.
	When breaking ties the least index is returned.
*/
function var $$E: arg_min(array[$$E] of var int: x)  = 
	let { constraint length(x) > 0; } in arg_min_total(x);

/** @group globals.math
	Returns the index of the minimum value in the array \a x.
	When breaking ties the least index is returned.
*/
function var $$E: arg_min(array[$$E] of var bool: x)  = 
	let { constraint length(x) > 0; } in arg_min_total(x);

/** @group globals.math
	Returns the index of the minimum value in the array \a x.
	When breaking ties the least index is returned.
*/
function var $$E: arg_min(array[$$E] of var float: x)  = 
	let { constraint length(x) > 0; } in arg_min_total(x);


/** @group globals.math
	Returns the index of the minimum non-absent value in the array \a x.
	When breaking ties the least index is returned.
*/
function var $$E: arg_min(array[$$E] of var opt int: x) = 
    let {
		constraint length(x) > 0;
	} in arg_min_total(x);

/** @group globals.math
	Returns the index of the minimum non-absent value in the array \a x.
	When breaking ties the least index is returned.
	Returns absent when all elements are absent.
*/
function var opt $$E: arg_min_weak(array[$$E] of var opt int: x) :: promise_total = 
	if length(x) = 0 then
		<>
	elseif exists (xi in x) (is_fixed(occurs(xi)) /\ (fix(occurs(xi)))) then
		let {
			var index_set(x): i;
			constraint fzn_minimum_arg_int_opt(x, i);
		} in i
	else
		let {
			set of int: idx = min(index_set(x)) .. max(index_set(x)) + 1;
			any: xx = array1d(idx, x ++ [ub_array(x)]);
			var idx: i;
			var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
			constraint fzn_minimum_arg_int_opt(xx, i);
		} in ii
	endif;

/** @group globals.math
	Returns the index of the minimum non-absent value in the array \a x.
	When breaking ties the least index is returned.
*/
function var $$E: arg_min(array[$$E] of var opt bool: x) = 
	let {
		constraint length(x) > 0;
	} in arg_max_total(x);

/** @group globals.math
	Returns the index of the minimum non-absent value in the array \a x.
	When breaking ties the least index is returned.
	Returns absent when all elements are absent.
*/
function var opt $$E: arg_min_weak(array[$$E] of var opt bool: x) :: promise_total = 
	if length(x) = 0 then
		<>
	elseif exists (xi in x) (is_fixed(occurs(xi)) /\ (fix(occurs(xi)))) then
		let {
			var index_set(x): i;
			constraint fzn_minimum_arg_bool_opt(x, i);
		} in i
	else
		let {
			set of int: idx = min(index_set(x)) .. max(index_set(x)) + 1;
			any: xx = array1d(idx, x ++ [true]);
			var idx: i;
			var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
			constraint fzn_minimum_arg_bool_opt(xx, i);
		} in ii
	endif;

/** @group globals.math
	Returns the index of the minimum non-absent value in the array \a x.
	When breaking ties the least index is returned.
*/
function var $$E: arg_min(array[$$E] of var opt float: x) = 
	let {
		constraint length(x) > 0;
	} in arg_min_total(x);

/** @group globals.math
	Returns the index of the minimum non-absent value in the array \a x.
	When breaking ties the least index is returned.
	Returns absent when all elements are absent.
*/
function var opt $$E: arg_min_weak(array[$$E] of var opt float: x) :: promise_total = 
	if length(x) = 0 then
		<>
	elseif exists (xi in x) (is_fixed(occurs(xi)) /\ (fix(occurs(xi)))) then
		let {
			var index_set(x): i;
			constraint fzn_minimum_arg_float_opt(x, i);
		} in i
	else
		let {
			set of int: idx = min(index_set(x)) .. max(index_set(x)) + 1;
			any: xx = array1d(idx, x ++ [ub_array(x)]);
			var idx: i;
			var opt index_set(x): ii = if i in index_set(x) then i else <> endif;
			constraint fzn_minimum_arg_float_opt(xx, i);
		} in ii
	endif;

function var $$E: arg_min_total(array[$$E] of var int: x) :: promise_total = 
	if length(x) = 0 then
		0
	else
		let {
			var min(index_set(x)) .. max(index_set(x)): i;
			constraint fzn_minimum_arg_int(x, i);
		} in i
	endif;

function var $$E: arg_min_total(array[$$E] of var bool: x) :: promise_total = 
	if length(x) = 0 then
		0
	else
		let {
			var min(index_set(x)) .. max(index_set(x)): i;
			constraint fzn_minimum_arg_bool(x, i);
		} in i
	endif;

function var $$E: arg_min_total(array[$$E] of var float: x) :: promise_total = 
	if length(x) = 0 then
		0
	else
		let {
			var min(index_set(x)) .. max(index_set(x)): i;
			constraint fzn_minimum_arg_float(x, i);
		} in i
	endif;


function var $$E: arg_min_total(array[$$E] of var opt int: x) :: promise_total = 
	if length(x) = 0 then
		min(index_set(x))
	elseif exists (xi in x) (is_fixed(occurs(xi)) /\ (fix(occurs(xi)))) then
		let {
			var index_set(x): i;
			constraint fzn_minimum_arg_int_opt(x, i);
		} in i
	else
		let {
			set of int: idx = min(index_set(x)) .. max(index_set(x)) + 1;
			any: xx = array1d(idx, x ++ [ub_array(x)]);
			var idx: i;
			var index_set(x): ii = if i in index_set(x) then i else min(index_set(x)) endif;
			constraint fzn_minimum_arg_int_opt(xx, i);
		} in ii
	endif;

function var $$E: arg_min_total(array[$$E] of var opt bool: x) :: promise_total = 
	if length(x) = 0 then
		0
	elseif exists (xi in x) (is_fixed(occurs(xi)) /\ (fix(occurs(xi)))) then
		let {
			var index_set(x): i;
			constraint fzn_minimum_arg_bool_opt(x, i);
		} in i
	else
		let {
			set of int: idx = min(index_set(x)) .. max(index_set(x)) + 1;
			any: xx = array1d(idx, x ++ [true]);
			var idx: i;
			var index_set(x): ii = if i in index_set(x) then i else min(index_set(x)) endif;
			constraint fzn_minimum_arg_bool_opt(xx, i);
		} in ii
	endif;

function var $$E: arg_min_total(array[$$E] of var opt float: x) :: promise_total = 
	if length(x) = 0 then
		0
	elseif exists (xi in x) (is_fixed(occurs(xi)) /\ (fix(occurs(xi)))) then
		let {
			var index_set(x): i;
			constraint fzn_minimum_arg_float_opt(x, i);
		} in i
	else
		let {
			set of int: idx = min(index_set(x)) .. max(index_set(x)) + 1;
			any: xx = array1d(idx, x ++ [ub_array(x)]);
			var idx: i;
			var index_set(x): ii = if i in index_set(x) then i else min(index_set(x)) endif;
			constraint fzn_minimum_arg_float_opt(xx, i);
		} in ii
	endif;

/** @group globals.math
	Constrain \a i to be the index of the minimum value in the array \a x.
	When breaking ties the least index is returned.

	Assumption: |\a x| > 0
*/
predicate minimum_arg(array[int] of var int: x, var int: i) =
	fzn_minimum_arg_int(x, i);

/** @group globals.math
	Constrain \a i to be the index of the minimum value in the array \a x.
	When breaking ties the least index is returned.

	Assumption: |\a x| > 0
*/
predicate minimum_arg(array[int] of var bool: x, var int: i) =
	fzn_minimum_arg_bool(x, i);

/** @group globals.math
	Constrain \a i to be the index of the minimum value in the array \a x.
	When breaking ties the least index is returned.
	
	Assumption: |\a x| > 0
*/
predicate minimum_arg(array[int] of var float: x, var int: i) =
	fzn_minimum_arg_float(x, i);

