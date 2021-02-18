include "lex_lesseq.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(array[int] of var bool: x ::promise_ctx_monotone,
                        array[int] of var bool: y ::promise_ctx_antitone) =
    lex_lesseq(y, x);

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(array[int] of var int: x ::promise_ctx_monotone,
                      array[int] of var int: y ::promise_ctx_antitone) =
    lex_lesseq(y, x);
 
/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(array[int] of var float: x ::promise_ctx_monotone,
                      array[int] of var float: y ::promise_ctx_antitone) =
    lex_lesseq(y, x);

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically greater than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greatereq(array[int] of var set of int: x,
                      array[int] of var set of int: y) =
    lex_lesseq(y, x);
