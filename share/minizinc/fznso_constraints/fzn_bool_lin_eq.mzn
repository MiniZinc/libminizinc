% The constraint `bool_lin_eq`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lin_eq` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `bool_lin_eq`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_bool_lin_eq(list of int: coeffs, list of var bool: xs, var int: total) =
  abort(
    "bool_lin_eq: this solver neither implements the constraint nor decomposes it",
  );
