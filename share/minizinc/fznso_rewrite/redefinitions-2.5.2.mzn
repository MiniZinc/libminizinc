% Shadows `std`'s file of the same name. The two-dimensional element builtins
% are where a caller's own numbering, in both dimensions, survives into
% FlatZinc — which is what the registry's `offsets` and `sizes` carry.
%
% `std` folds the pair into one flat index and delegates to the one-dimensional
% form, which costs a linear constraint per access and hides a genuinely
% multi-dimensional propagator from any solver that has one.

include "fzn_int_array_element_nd.mzn";
include "fzn_bool_array_element_nd.mzn";
include "fzn_float_array_element_nd.mzn";
include "fzn_set_of_int_array_element_nd.mzn";

predicate array_var_int_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var int: xs,
  var int: y,
) =
  fzn_int_array_element_nd(
    array1d(xs),
    [min(index_set_1of2(xs)), min(index_set_2of2(xs))],
    [card(index_set_1of2(xs)), card(index_set_2of2(xs))],
    [idx1, idx2],
    y,
  );

predicate array_var_bool_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var bool: xs,
  var bool: y,
) =
  fzn_bool_array_element_nd(
    array1d(xs),
    [min(index_set_1of2(xs)), min(index_set_2of2(xs))],
    [card(index_set_1of2(xs)), card(index_set_2of2(xs))],
    [idx1, idx2],
    y,
  );

predicate array_var_float_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var float: xs,
  var float: y,
) =
  fzn_float_array_element_nd(
    array1d(xs),
    [min(index_set_1of2(xs)), min(index_set_2of2(xs))],
    [card(index_set_1of2(xs)), card(index_set_2of2(xs))],
    [idx1, idx2],
    y,
  );

predicate array_var_set_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var set of int: xs,
  var set of int: y,
) =
  fzn_set_of_int_array_element_nd(
    array1d(xs),
    [min(index_set_1of2(xs)), min(index_set_2of2(xs))],
    [card(index_set_1of2(xs)), card(index_set_2of2(xs))],
    [idx1, idx2],
    y,
  );
