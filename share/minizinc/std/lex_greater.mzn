include "lex_less.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(array[int] of var bool: x ::promise_ctx_antitone,
                      array[int] of var bool: y ::promise_ctx_antitone) =
    lex_less(y, x);                     

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(array[int] of var int: x ::promise_ctx_antitone,
                      array[int] of var int: y ::promise_ctx_antitone) =
    lex_less(y, x);
 
/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(array[int] of var float: x ::promise_ctx_antitone,
                      array[int] of var float: y ::promise_ctx_antitone) =
    lex_less(y, x);

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically greater than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_greater(array[int] of var set of int: x,
                      array[int] of var set of int: y) =
    lex_less(y, x);
