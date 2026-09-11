% The constraint `float_tanh`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_tanh` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `float_tanh`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_float_tanh(var float: a, var float: b) =
  abort(
    "float_tanh: this solver neither implements the constraint nor decomposes it",
  );
