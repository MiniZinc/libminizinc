include "val_perm_sym_int.mzn";

/** @group globals.symmetry
  Declare a value permutation symmetry on the array \a x. Requires that all
  variables in \a x are distinct and that each row of \a s covers the same set
  of values. Means that the rows of the array \a s are value permutations which
  can be interchanged across all the variables in \a x at once while preserving
  the solutions of the model. i.e. For any \p i and \p j, if every \a x[\p k]
  with value \a s[\p i,\p m] is set to s[\p j,\p m] then solutions are preserved.
*/

predicate val_perm_sym(array[int] of var int: x, array[int,int] of int: s) =
    symmetry_breaking_constraint( val_perm_sym_int(x, s) );
