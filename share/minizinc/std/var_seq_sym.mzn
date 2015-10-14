include "var_seq_sym_int.mzn";
include "var_seq_sym_float.mzn";

/** @group globals.symmetry
  Declare a variable sequence symmetry on the array \a x of variable sequences.
  Requires that all variables in x are distinct. Means that rows of the array
  \a x are variable sequences that can be interchanged while preserving the
  solutions of the model. i.e. For any \p i and \p j, if for every \p k,
  \a x[\p i,\p k] is swapped with \a x[\p j,\p k] then solutions are preserved.
*/

predicate var_seq_sym(array[int,int] of var int: x) =
    symmetry_breaking_constraint( var_seq_sym_int(x) );

/** @group globals.symmetry
  Declare a variable sequence symmetry on the array \a x of variable sequences.
  Requires that all variables in x are distinct. Means that rows of the array
  \a x are variable sequences that can be interchanged while preserving the
  solutions of the model. i.e. For any \p i and \p j, if for every \p k,
  \a x[\p i,\p k] is swapped with \a x[\p j,\p k] then solutions are preserved.
*/

predicate var_seq_sym(array[int,int] of var float: x) =
    symmetry_breaking_constraint( var_seq_sym_float(x) );
