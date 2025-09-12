include "fzn_strict_lex2.mzn";
include "fzn_strict_lex2_reif.mzn";

/** @group globals.lexicographic
  Require adjacent rows and adjacent columns in the array \a x to be
  lexicographically ordered.  Adjacent rows and adjacent columns cannot be equal.
*/
predicate lex2_strict(array [$$E, $$F] of var $$T: x) = fzn_strict_lex2(index2int(enum2int(x)));
