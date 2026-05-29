include "fzn_var_perm_sym.mzn";

/** @group globals.lexicographic
  Requires that the array \a xs is lex least under the given list of permutations \a permutations
*/
predicate var_perm_sym(array [$$X] of var $$Y: xs, array [$$Z, $$X] of $$X: permutations) =
  fzn_var_perm_sym(index2int(enum2int(xs)), index2int(enum2int(permutations)));
