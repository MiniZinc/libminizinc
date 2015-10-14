include "var_seq_rev_sym_int.mzn";
include "var_seq_rev_sym_float.mzn";

/** @group globals.symmetry
  Declare a variable sequence reverse symmetry on the array \a x. Requires that
  all variables in \a x are distinct. Means that the variable sequence \a x
  can be reversed while preserving solutions.
*/

predicate var_seq_rev_sym(array[int] of var int: x) =
    symmetry_breaking_constraint( var_seq_rev_sym_int(x) );

/** @group globals.symmetry
  Declare a variable sequence reverse symmetry on the array \a x. Requires that
  all variables in \a x are distinct. Means that the variable sequence \a x
  can be reversed while preserving solutions.
*/

predicate var_seq_rev_sym(array[int] of var float: x) =
    symmetry_breaking_constraint( var_seq_rev_sym_float(x) );
