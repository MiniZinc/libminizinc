include "lex_lesseq.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(
  array [$$E] of var bool: x :: promise_ctx_monotone,
  array [$$F] of var bool: y :: promise_ctx_antitone,
) = lex_lesseq(index2int(y), index2int(x));

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(
  array [$$E] of var $$T: x :: promise_ctx_monotone,
  array [$$F] of var $$T: y :: promise_ctx_antitone,
) = lex_lesseq(index2int(enum2int(y)), index2int(enum2int(x)));

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(
  array [$$E] of var float: x :: promise_ctx_monotone,
  array [$$F] of var float: y :: promise_ctx_antitone,
) = lex_lesseq(index2int(y), index2int(x));

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(array [$$E] of var set of $$T: x, array [$$F] of var set of $$T: y) =
  lex_lesseq(index2int(y), index2int(x));
