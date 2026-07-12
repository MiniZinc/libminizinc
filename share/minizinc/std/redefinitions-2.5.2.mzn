% This file contains redefinitions of standard builtins for version 2.5.2
% that can be overridden by solvers.

predicate array_var_int_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var int: xs,
  var int: y,
) =
  let {
    int: dim = card(index_set_2of2(xs));
    int: min_flat = min(index_set_1of2(xs)) * dim + min(index_set_2of2(xs)) - 1;
  } in array_var_int_element_nonshifted((idx1 * dim + idx2 - min_flat) :: domain, array1d(xs), y);

predicate array_var_bool_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var bool: xs,
  var bool: y,
) =
  let {
    int: dim = card(index_set_2of2(xs));
    int: min_flat = min(index_set_1of2(xs)) * dim + min(index_set_2of2(xs)) - 1;
  } in array_var_bool_element_nonshifted((idx1 * dim + idx2 - min_flat) :: domain, array1d(xs), y);

predicate array_var_float_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var float: xs,
  var float: y,
) =
  let {
    int: dim = card(index_set_2of2(xs));
    int: min_flat = min(index_set_1of2(xs)) * dim + min(index_set_2of2(xs)) - 1;
  } in array_var_float_element_nonshifted((idx1 * dim + idx2 - min_flat) :: domain, array1d(xs), y);

predicate array_var_set_element2d_nonshifted(
  var int: idx1,
  var int: idx2,
  array [int, int] of var set of int: xs,
  var set of int: y,
) =
  let {
    int: dim = card(index_set_2of2(xs));
    int: min_flat = min(index_set_1of2(xs)) * dim + min(index_set_2of2(xs)) - 1;
  } in array_var_set_element_nonshifted((idx1 * dim + idx2 - min_flat) :: domain, array1d(xs), y);
