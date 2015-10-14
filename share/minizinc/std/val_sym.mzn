include "val_sym_int.mzn";

/** @group globals.symmetry
  Declare a value symmetry on the array \a x. Requires that all variables in
  \a x are distinct and all values in \a s are distinct. Means that the values
  in \a s can be interchanged across all the variables in \a x at once while
  preserving the solutions of the model. i.e. For any \p i and \p j, if every
  \a x[\p k] with value \a s[\p i] is set to \a s[\p j] and every \a x[\p k]
  with value \a s[\p j] is set to \a s[\p i] then solutions are preserved.
*/

predicate val_sym(array[int] of var int: x, array[int] of int: s) =
    symmetry_breaking_constraint( val_sym_int(x, s) );
