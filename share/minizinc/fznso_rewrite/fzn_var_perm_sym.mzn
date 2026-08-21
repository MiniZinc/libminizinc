% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-dimensional, so the permutations are passed flat, one
% after another; each is as long as `xs`, which is what says where one ends and
% the next begins.

include "fzn_int_lex_least_under_permutation.mzn";

predicate fzn_var_perm_sym(array [int] of var int: xs, array [int, int] of int: permutations) =
  fzn_int_lex_least_under_permutation(array1d(xs), array1d(permutations));
