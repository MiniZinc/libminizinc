include "var_sym_int.mzn";
include "var_sym_float.mzn";

/** @group globals.symmetry
  Declare a variable symmetry on the array \a x. Requires that all variables in
  \a x are distinct. Means that the variables in \a x are interchangeable while
  preserving the solutions of the model. i.e. For any \p i and \p j, \a x[\p i]
  can be swapped with \a x[\p j].
*/

predicate var_sym(array[int] of var int: x) =
    symmetry_breaking_constraint( var_sym_int(x) );

/** @group globals.symmetry
  Declare a variable symmetry on the array \a x. Requires that all variables in
  \a x are distinct. Means that the variables in \a x are interchangeable while
  preserving the solutions of the model. i.e. For any \p i and \p j, \a x[\p i]
  can be swapped with \a x[\p j].
*/

predicate var_sym(array[int] of var float: x) =
    symmetry_breaking_constraint( var_sym_float(x) );
