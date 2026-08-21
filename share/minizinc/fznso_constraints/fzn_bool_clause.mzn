% The constraint `bool_clause`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_clause` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `bool_clause`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_bool_clause(list of var bool: pos, list of var bool: neg) =
  abort(
    "bool_clause: this solver neither implements the constraint nor decomposes it",
  );
