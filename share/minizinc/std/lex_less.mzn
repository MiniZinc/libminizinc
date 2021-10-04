include "fzn_lex_less_bool.mzn";
include "fzn_lex_less_bool_reif.mzn";
include "fzn_lex_less_float.mzn";
include "fzn_lex_less_float_reif.mzn";
include "fzn_lex_less_int.mzn";
include "fzn_lex_less_int_reif.mzn";
include "fzn_lex_less_set.mzn";
include "fzn_lex_less_set_reif.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
	array[int] of var bool: x ::promise_ctx_antitone,
	array[int] of var bool: y ::promise_ctx_monotone
) =
	if length(x)=1 /\ length(y)=1
		then x[min(index_set(x))] < y[min(index_set(y))]
	elseif length(x)=0 then
		length(y)>0
	elseif length(y)=0 then
		false
	else
		fzn_lex_less_bool(array1d(x), array1d(y))
	endif;

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
	array[int] of var int: x ::promise_ctx_antitone,
	array[int] of var int: y ::promise_ctx_monotone,
) =
	if length(x)=1 /\ length(y)=1 then
		x[min(index_set(x))] < y[min(index_set(y))]
	elseif length(x)=0 then
		length(y)>0
	elseif length(y)=0 then
		false
	else
		fzn_lex_less_int(array1d(x), array1d(y))
	endif;

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
	array[int] of var float: x ::promise_ctx_antitone,
	array[int] of var float: y ::promise_ctx_monotone,
) =
	if length(x)=1 /\ length(y)=1 then
		x[min(index_set(x))] < y[min(index_set(y))]
	elseif length(x)=0 then
		length(y)>0
	elseif length(y)=0 then
		false
	else
		fzn_lex_less_float(array1d(x), array1d(y))
	endif;

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
	array[int] of var set of int: x,
	array[int] of var set of int: y,
) =
	if length(x)=1 /\ length(y)=1 then
		x[min(index_set(x))] < y[min(index_set(y))]
	elseif length(x)=0 then
		length(y)>0
	elseif length(y)=0 then
		false
	else
		fzn_lex_less_set(array1d(x), array1d(y))
	endif;

% Alternative names for the above.
%
predicate lex_lt(array[int] of var bool: x ::promise_ctx_antitone,
                 array[int] of var bool: y ::promise_ctx_monotone) =
	lex_less(x, y);

predicate lex_lt(array[int] of var int: x ::promise_ctx_antitone,
                 array[int] of var int: y ::promise_ctx_monotone) =
	lex_less(x, y);

predicate lex_lt(array[int] of var float: x ::promise_ctx_antitone,
                 array[int] of var float: y ::promise_ctx_monotone) =
	lex_less(x, y);

predicate lex_lt(array[int] of var set of int: x,
                  array[int] of var set of int: y) =
	lex_less(x, y);

