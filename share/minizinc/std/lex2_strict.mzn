include "strict_lex2.mzn";

/** @group globals.lexicographic
  Require adjacent rows and adjacent columns in the array \a x to be
  lexicographically ordered.  Adjacent rows and adjacent columns cannot be equal.
*/
predicate lex2_strict(array[int, int] of var int: x) =
	strict_lex2(x);
