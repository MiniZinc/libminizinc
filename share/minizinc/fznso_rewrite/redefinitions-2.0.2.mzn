% Shadows `std`'s file of the same name, so everything it declares is repeated
% here — only the element constraints differ, and they are the reason the file
% exists in this overlay at all.
%
% The `_nonshifted` element builtins are where the caller's own numbering
% survives into FlatZinc, which is exactly what the registry's `offset` argument
% carries. `std` rebases them onto the one-based form; here they go straight to
% the registry constraint with the offset they already have, so a solver that
% implements it never sees the rebasing at all.

include "fzn_int_array_element.mzn";
include "fzn_bool_array_element.mzn";
include "fzn_float_array_element.mzn";
include "fzn_set_of_int_array_element.mzn";

predicate symmetry_breaking_constraint(var bool: b) = b;

predicate redundant_constraint(var bool: b) = b;

predicate array_var_int_element_nonshifted(
  var int: idx,
  array [int] of var int: xs,
  var int: y,
) = fzn_int_array_element(array1d(xs), min(index_set(xs)), idx, y);

predicate array_var_bool_element_nonshifted(
  var int: idx,
  array [int] of var bool: xs,
  var bool: y,
) = fzn_bool_array_element(array1d(xs), min(index_set(xs)), idx, y);

predicate array_var_float_element_nonshifted(
  var int: idx,
  array [int] of var float: xs,
  var float: y,
) = fzn_float_array_element(array1d(xs), min(index_set(xs)), idx, y);

predicate array_var_set_element_nonshifted(
  var int: idx,
  array [int] of var set of int: xs,
  var set of int: y,
) = fzn_set_of_int_array_element(array1d(xs), min(index_set(xs)), idx, y);
