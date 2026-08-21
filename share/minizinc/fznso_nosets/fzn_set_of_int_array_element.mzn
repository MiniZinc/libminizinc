% `set_of_int_array_element` for a solver with no set variables. The registry
% passes a one-based array and the index the caller numbers it from, so the
% index is rebased on the way in.

include "fznso_set_operations.mzn";

predicate fzn_set_of_int_array_element(
  list of var set of int: xs,
  int: offset,
  var int: index,
  var set of int: value,
) =
  fznso_nosets_array_var_set_element(index - offset + 1, array1d(xs), value);
