% The constraint `int_times`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_times` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `int_times`, which is
% to say it is primitive: every way of writing what it means uses the operator
% it defines, so a body here would flatten straight back to this identifier and
% the compiler would not terminate.

predicate fzn_int_times(var int: a, var int: b, var int: c) =
  abort(
    "int_times: this solver neither implements the constraint nor decomposes it",
  );
