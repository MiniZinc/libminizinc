include "var_perm_sym_int.mzn";
include "var_perm_sym_float.mzn";

/** @group globals.symmetry
  Declare a variable permutation symmetry on the array \a x. Requires that all
  variables in \a x are distinct and that each row in \a p is a permutation of
  the integers 1..length(x). Means that the variable sequences represented by
  rows in \a p, which are permutations of \a x, can be interchanged while
  preserving the solutions of the model. i.e. If for every \p k,
  \a x[\a p[\p i]] is set to the value of \a x[\a p[\p j,\p k]] then solutions
  are preserved.
*/

predicate var_perm_sym(array[int] of var int: x, array[int,int] of int: p) =
    symmetry_breaking_constraint( var_perm_sym_int(x, p) );

/** @group globals.symmetry
  Declare a variable permutation symmetry on the array \a x. Requires that all
  variables in \a x are distinct and that each row in \a p is a permutation of
  the integers 1..length(x). Means that the variable sequences represented by
  rows in \a p, which are permutations of \a x, can be interchanged while
  preserving the solutions of the model. i.e. If for every \p k,
  \a x[\a p[\p i]] is set to the value of \a x[\a p[\p j,\p k]] then solutions
  are preserved.
*/

predicate var_perm_sym(array[int] of var float: x, array[int,int] of int: p) =
    symmetry_breaking_constraint( var_perm_sym_float(x, p) );
