% The constraint `bool_array_element_nd`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_array_element_nd` and
% this declaration is matched, or ship a file of this name carrying a
% decomposition of your own.
%
% There is no decomposition here. A FlatZinc builtin backs
% `bool_array_element_nd`, which is to say it is primitive: every way of writing
% what it means uses the operator it defines, so a body here would flatten
% straight back to this identifier and the compiler would not terminate.

predicate fzn_bool_array_element_nd(
  list of var bool: xs,
  list of int: offsets,
  list of int: sizes,
  list of var int: indices,
  var bool: value,
) =
  abort(
    "bool_array_element_nd: this solver neither implements the constraint nor decomposes it",
  );
