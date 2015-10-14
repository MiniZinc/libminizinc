include "var_sqr_sym_int.mzn";
include "var_sqr_sym_float.mzn";

/** @group globals.symmetry
  Declare a variable square symmetry on the square array \a x. Requires that
  variables in \a x are distinct. Means that the standard square rotation and
  reflection symmetries can be applied to array \a x while preserving the
  solutions of the model.
*/

predicate var_sqr_sym(array[int,int] of var int: x) =
    symmetry_breaking_constraint( var_sqr_sym_int(x) );

/** @group globals.symmetry
  Declare a variable square symmetry on the square array \a x. Requires that
  variables in \a x are distinct. Means that the standard square rotation and
  reflection symmetries can be applied to array \a x while preserving the
  solutions of the model.
*/

predicate var_sqr_sym(array[int,int] of var float: x) =
    symmetry_breaking_constraint( var_sqr_sym_float(x) );
