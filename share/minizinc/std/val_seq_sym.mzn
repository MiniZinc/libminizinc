include "val_seq_sym_int.mzn";

/** @group globals.symmetry
  Declare a value sequence symmetry on the array \a x. Requires that all
  variables in \a x are distinct and all values in \a s are distinct. Means that
  rows of the array \a s are value sequences that can be pairwise interchanged
  across all the variables in \a x at once while preserving the solutions of
  the model. i.e. For any \p i and \p j, if for every \p m, every \a x[\p k]
  with value \a s[\p i,\p m] is set to \a s[\p j,\p m] and every \a x[\p k]
  with value \a s[\p j,\p m] is set to \a s[\p i,\p m] then solutions are
  preserved.
*/

predicate val_seq_sym(array[int] of var int: x, array[int,int] of int: s) =
    symmetry_breaking_constraint( val_seq_sym_int(x, s) );
