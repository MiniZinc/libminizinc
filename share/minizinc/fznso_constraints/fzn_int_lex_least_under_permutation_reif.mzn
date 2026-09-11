% The constraint `int_lex_least_under_permutation_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `int_lex_least_under_permutation_reif` and the body below is dropped in favour
% of the propagator, or ship a file of this name with a decomposition of your
% own.

include "fzn_int_lex_least_under_permutation.mzn";
predicate fzn_int_lex_least_under_permutation_reif(
  list of var int: xs,
  list of int: permutations,
  var bool: b,
) =
  b <-> (
    let {
      int: n = length(xs);
      int: m = length(permutations) div n;
      array [1..m, 1..n] of int: p = array2d(1..m, 1..n, permutations);
    } in forall (i, j in 1..m where i != j) (
      fzn_var_perm_sym_pairwise(xs, [p[i, k] | k in 1..n], [p[j, k] | k in 1..n])
    )
  );
