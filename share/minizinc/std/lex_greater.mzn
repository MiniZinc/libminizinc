include "lex_less.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(
  array [$$E] of var bool: x :: promise_ctx_antitone,
  array [$$F] of var bool: y :: promise_ctx_antitone,
) = lex_less(index2int(y), index2int(x));

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(
  array [$$E] of var $$T: x :: promise_ctx_antitone,
  array [$$F] of var $$T: y :: promise_ctx_antitone,
) = lex_less(index2int(enum2int(y)), index2int(enum2int(x)));

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(
  array [$$E] of var float: x :: promise_ctx_antitone,
  array [$$F] of var float: y :: promise_ctx_antitone,
) = lex_less(index2int(y), index2int(x));

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(array [$$E] of var set of $$T: x, array [$$F] of var set of $$T: y) =
  lex_less(index2int(y), index2int(x));
