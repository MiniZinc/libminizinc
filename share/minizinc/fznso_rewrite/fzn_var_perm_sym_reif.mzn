% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_var_perm_sym.mzn`, under the registry's name.

include "fzn_int_lex_least_under_permutation_reif.mzn";

predicate fzn_var_perm_sym_reif(
  array [int] of var int: xs,
  array [int, int] of int: permutations,
  var bool: b,
) =
  fzn_int_lex_least_under_permutation_reif(array1d(xs), array1d(permutations), b);
