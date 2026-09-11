% The constraint `float_array_element`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_array_element` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs
% `float_array_element`, which is to say it is primitive: every way of writing
% what it means uses the operator it defines, so a body here would flatten
% straight back to this identifier and the compiler would not terminate.

predicate fzn_float_array_element(
  list of var float: xs,
  int: offset,
  var int: index,
  var float: value,
) =
  abort(
    "float_array_element: this solver neither implements the constraint nor decomposes it",
  );
