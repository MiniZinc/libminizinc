% The constraint `float_acosh`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_acosh` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `float_acosh`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_float_acosh(var float: a, var float: b) =
  abort(
    "float_acosh: this solver neither implements the constraint nor decomposes it",
  );
