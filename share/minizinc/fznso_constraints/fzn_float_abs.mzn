% The constraint `float_abs`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_abs` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `float_abs`, which is
% to say it is primitive: every way of writing what it means uses the operator
% it defines, so a body here would flatten straight back to this identifier and
% the compiler would not terminate.

predicate fzn_float_abs(var float: a, var float: b) =
  abort(
    "float_abs: this solver neither implements the constraint nor decomposes it",
  );
