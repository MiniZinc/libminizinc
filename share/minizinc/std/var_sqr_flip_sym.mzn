include "var_sqr_flip_sym_int.mzn";
include "var_sqr_flip_sym_float.mzn";

/** @group globals.symmetry
  Declare a variables rectangle flip symmetry on the two-dimensional array \a x.
  Requires that all variables in \a x are distinct. Means that \a x can be
  flipped on the horizontal/vertical axis and the two diagonal lines while
  preserving solutions.
*/

predicate var_sqr_flip_sym(array[int,int] of var int: x) =
    symmetry_breaking_constraint( var_sqr_flip_sym_int(x) );

/** @group globals.symmetry
  Declare a variables rectangle flip symmetry on the two-dimensional array \a x.
  Requires that all variables in \a x are distinct. Means that \a x can be
  flipped on the horizontal/vertical axis and the two diagonal lines while
  preserving solutions.
*/

predicate var_sqr_flip_sym(array[int,int] of var float: x) =
    symmetry_breaking_constraint( var_sqr_flip_sym_float(x) );
