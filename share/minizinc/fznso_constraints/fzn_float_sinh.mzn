% The constraint `float_sinh`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_sinh` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `float_sinh`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_float_sinh(var float: a, var float: b) =
  abort(
    "float_sinh: this solver neither implements the constraint nor decomposes it",
  );
