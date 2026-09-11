% The constraint `int_to_float`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_to_float` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `int_to_float`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_int_to_float(var int: a, var float: b) =
  abort(
    "int_to_float: this solver neither implements the constraint nor decomposes it",
  );
