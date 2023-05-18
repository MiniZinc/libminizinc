include "fzn_var_perm_sym.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is lex least under the given list of permutations \a p
*/
predicate var_perm_sym(array[$$X] of var $$Y: x, array[$$Z,$$X] of $$X: p) =
  fzn_var_perm_sym(x,p);
