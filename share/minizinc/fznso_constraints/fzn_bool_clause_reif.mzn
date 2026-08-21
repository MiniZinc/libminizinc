% The constraint `bool_clause_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_clause_reif` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `bool_clause_reif`,
% which is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_bool_clause_reif(
  list of var bool: pos,
  list of var bool: neg,
  var bool: b,
) =
  abort(
    "bool_clause_reif: this solver neither implements the constraint nor decomposes it",
  );
