include "lex_less.mzn";

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically greater than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(
  array [$$E] of var bool: xs :: promise_ctx_antitone,
  array [$$F] of var bool: ys :: promise_ctx_antitone,
) = lex_less(index2int(ys), index2int(xs));

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically greater than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(
  array [$$E] of var $$T: xs :: promise_ctx_antitone,
  array [$$F] of var $$T: ys :: promise_ctx_antitone,
) = lex_less(index2int(enum2int(ys)), index2int(enum2int(xs)));

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically greater than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(
  array [$$E] of var float: xs :: promise_ctx_antitone,
  array [$$F] of var float: ys :: promise_ctx_antitone,
) = lex_less(index2int(ys), index2int(xs));

/** @group globals.lexicographic
  Requires that the array \a xs is strictly lexicographically greater than
  array \a ys. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(array [$$E] of var set of $$T: xs, array [$$F] of var set of $$T: ys) =
  lex_less(index2int(ys), index2int(xs));
