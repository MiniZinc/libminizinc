include "fzn_lex_less_bool.mzn";
include "fzn_lex_less_bool_reif.mzn";
include "fzn_lex_less_float.mzn";
include "fzn_lex_less_float_reif.mzn";
include "fzn_lex_less_int.mzn";
include "fzn_lex_less_int_reif.mzn";
include "fzn_lex_less_set.mzn";
include "fzn_lex_less_set_reif.mzn";

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically less than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
  array [$$E] of var bool: xs :: promise_ctx_antitone,
  array [$$F] of var bool: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] < ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    length(ys) > 0
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_less_bool(array1d(xs), array1d(ys))
  endif;

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically less than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
  array [$$E] of var $$T: xs :: promise_ctx_antitone,
  array [$$F] of var $$T: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] < ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    length(ys) > 0
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_less_int(array1d(enum2int(xs)), array1d(enum2int(ys)))
  endif;

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically less than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(
  array [$$E] of var float: xs :: promise_ctx_antitone,
  array [$$F] of var float: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] < ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    length(ys) > 0
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_less_float(array1d(xs), array1d(ys))
  endif;

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically less than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(array [$$E] of var set of $$T: xs, array [$$F] of var set of $$T: ys) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] < ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    length(ys) > 0
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_less_set(array1d(enum2int(xs)), array1d(enum2int(ys)))
  endif;

% Alternative names for the above.
%
predicate lex_lt(
  array [$$E] of var bool: xs :: promise_ctx_antitone,
  array [$$F] of var bool: ys :: promise_ctx_monotone,
) = lex_less(xs, ys);

predicate lex_lt(
  array [$$E] of var $$T: xs :: promise_ctx_antitone,
  array [$$F] of var $$T: ys :: promise_ctx_monotone,
) = lex_less(xs, ys);

predicate lex_lt(
  array [$$E] of var float: xs :: promise_ctx_antitone,
  array [$$F] of var float: ys :: promise_ctx_monotone,
) = lex_less(xs, ys);

predicate lex_lt(array [$$F] of var set of $$T: xs, array [$$F] of var set of $$T: ys) =
  lex_less(xs, ys);
