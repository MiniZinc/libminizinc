include "fzn_var_sqr_sym.mzn";

/** @group globals.lexicographic
  Requires that the square array \a x is lex least under the 7 symmetries:
  3 rotations and 4 reflections
*/
predicate var_sqr_sym(array[$$X, $$X] of var $$Y: x) =
  let { 
    int: n = card(index_set_1of2(x));
    constraint assert(n = card(index_set_2of2(x)),"square symmetry applied to non-square array");
  } in fzn_var_sqr_sym(array2d(1..n, 1..n, x));
