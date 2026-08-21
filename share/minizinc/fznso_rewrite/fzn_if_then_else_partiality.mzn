% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `if_then_else_partiality` and `if_then_else_var_bool` are the same constraint
% written twice — both reduce to `selected(i) -> (result <-> values[i])` — so the
% registry gives them one name.

include "fzn_bool_if_then_else.mzn";

predicate fzn_if_then_else_partiality(
  array [int] of var bool: c,
  array [int] of var bool: def,
  var bool: b,
) = fzn_bool_if_then_else(array1d(c), array1d(def), b);
