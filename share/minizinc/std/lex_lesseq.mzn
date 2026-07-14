include "fzn_lex_lesseq_bool.mzn";
include "fzn_lex_lesseq_bool_reif.mzn";
include "fzn_lex_lesseq_float.mzn";
include "fzn_lex_lesseq_float_reif.mzn";
include "fzn_lex_lesseq_int.mzn";
include "fzn_lex_lesseq_int_reif.mzn";
include "fzn_lex_lesseq_set.mzn";
include "fzn_lex_lesseq_set_reif.mzn";

predicate fzn_lex_lesseq_bool(
  array [int] of var bool: xs :: promise_ctx_antitone,
  array [int] of var bool: ys :: promise_ctx_monotone,
);
predicate fzn_lex_lesseq_float(array [int] of var float: xs, array [int] of var float: ys);
predicate fzn_lex_lesseq_int(
  array [int] of var int: xs :: promise_ctx_antitone,
  array [int] of var int: ys :: promise_ctx_monotone,
);
predicate fzn_lex_lesseq_set(array [int] of var set of int: xs, array [int] of var set of int: ys);

/** @group globals.lexicographic
  Requires that the array \a xs is lexicographically less than or equal to
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var bool: xs :: promise_ctx_antitone,
  array [$$F] of var bool: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] <= ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    true
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_lesseq_bool(index2int(xs), index2int(ys))
  endif;

/** @group globals.lexicographic
  Requires that the array \a xs is lexicographically less than or equal to
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var float: xs :: promise_ctx_antitone,
  array [$$F] of var float: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] <= ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    true
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_lesseq_float(index2int(xs), index2int(ys))
  endif;

/** @group globals.lexicographic
  Requires that the array \a xs is lexicographically less than or equal to
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var $$T: xs :: promise_ctx_antitone,
  array [$$F] of var $$T: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] <= ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    true
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_lesseq_int(index2int(enum2int(xs)), index2int(enum2int(ys)))
  endif;

/** @group globals.lexicographic
  Requires that the array \a xs is lexicographically less than or equal to
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(
  array [$$E] of var set of $$T: xs :: promise_ctx_antitone,
  array [$$F] of var set of $$T: ys :: promise_ctx_monotone,
) =
  if length(xs) = 1 /\ length(ys) = 1 then
    xs[min(index_set(xs))] <= ys[min(index_set(ys))]
  elseif length(xs) = 0 then
    true
  elseif length(ys) = 0 then
    false
  else
    fzn_lex_lesseq_set(index2int(enum2int(xs)), index2int(enum2int(ys)))
  endif;

% Alternative names for the above.
%
predicate lex_leq(
  array [$$E] of var bool: xs :: promise_ctx_antitone,
  array [$$F] of var bool: ys :: promise_ctx_monotone,
) = lex_lesseq(xs, ys);

predicate lex_leq(
  array [$$E] of var $$T: xs :: promise_ctx_antitone,
  array [$$F] of var $$T: ys :: promise_ctx_monotone,
) = lex_lesseq(xs, ys);

predicate lex_leq(
  array [$$E] of var float: xs :: promise_ctx_antitone,
  array [$$F] of var float: ys :: promise_ctx_monotone,
) = lex_lesseq(xs, ys);

predicate lex_leq(
  array [$$E] of var set of $$T: xs :: promise_ctx_antitone,
  array [$$F] of var set of $$T: ys :: promise_ctx_monotone,
) = lex_lesseq(xs, ys);
