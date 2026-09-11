% The constraint `int_lin_ne`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_lin_ne` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `int_lin_ne`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_int_lin_ne(list of int: coeffs, list of var int: xs, int: total) =
  abort(
    "int_lin_ne: this solver neither implements the constraint nor decomposes it",
  );
