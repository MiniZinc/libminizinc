include "fzn_lex_lesseq_bool.mzn";
include "fzn_lex_lesseq_bool_reif.mzn";
include "fzn_lex_lesseq_float.mzn";
include "fzn_lex_lesseq_float_reif.mzn";
include "fzn_lex_lesseq_int.mzn";
include "fzn_lex_lesseq_int_reif.mzn";
include "fzn_lex_lesseq_set.mzn";
include "fzn_lex_lesseq_set_reif.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var bool: x :: promise_ctx_antitone,
  array [$$F] of var bool: y :: promise_ctx_monotone,
) =
  if length(x) = 1 /\ length(y) = 1 then
    x[min(index_set(x))] <= y[min(index_set(y))]
  elseif length(x) = 0 then
    true
  elseif length(y) = 0 then
    false
  else
    fzn_lex_lesseq_bool(index2int(x), index2int(y))
  endif;

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var float: x :: promise_ctx_antitone,
  array [$$F] of var float: y :: promise_ctx_monotone,
) =
  if length(x) = 1 /\ length(y) = 1 then
    x[min(index_set(x))] <= y[min(index_set(y))]
  elseif length(x) = 0 then
    true
  elseif length(y) = 0 then
    false
  else
    fzn_lex_lesseq_float(index2int(x), index2int(y))
  endif;

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var $$T: x :: promise_ctx_antitone,
  array [$$F] of var $$T: y :: promise_ctx_monotone,
) =
  if length(x) = 1 /\ length(y) = 1 then
    x[min(index_set(x))] <= y[min(index_set(y))]
  elseif length(x) = 0 then
    true
  elseif length(y) = 0 then
    false
  else
    fzn_lex_lesseq_int(index2int(enum2int(x)), index2int(enum2int(y)))
  endif;

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var set of $$T: x :: promise_ctx_antitone,
  array [$$F] of var set of $$T: y :: promise_ctx_monotone,
) =
  if length(x) = 1 /\ length(y) = 1 then
    x[min(index_set(x))] <= y[min(index_set(y))]
  elseif length(x) = 0 then
    true
  elseif length(y) = 0 then
    false
  else
    fzn_lex_lesseq_set(index2int(enum2int(x)), index2int(enum2int(y)))
  endif;

% Alternative names for the above.
%
predicate lex_leq(
  array [$$E] of var bool: x :: promise_ctx_antitone,
  array [$$F] of var bool: y :: promise_ctx_monotone,
) = lex_lesseq(x, y);

predicate lex_leq(
  array [$$E] of var $$T: x :: promise_ctx_antitone,
  array [$$F] of var $$T: y :: promise_ctx_monotone,
) = lex_lesseq(x, y);

predicate lex_leq(
  array [$$E] of var float: x :: promise_ctx_antitone,
  array [$$F] of var float: y :: promise_ctx_monotone,
) = lex_lesseq(x, y);

predicate lex_leq(
  array [$$E] of var set of $$T: x :: promise_ctx_antitone,
  array [$$F] of var set of $$T: y :: promise_ctx_monotone,
) = lex_lesseq(x, y);
