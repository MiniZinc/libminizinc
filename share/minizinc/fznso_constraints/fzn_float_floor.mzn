% The constraint `float_floor`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_floor` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `float_floor`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_float_floor(var float: a, var int: b) =
  abort(
    "float_floor: this solver neither implements the constraint nor decomposes it",
  );
