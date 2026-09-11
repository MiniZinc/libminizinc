% The constraint `int_lex_least_under_permutation`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_lex_least_under_permutation` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% A FlatZinc array is one-dimensional, so the permutations are passed flat, one
% after another; each is as long as `xs`, which is what says where one ends and
% the next begins.

predicate fzn_int_lex_least_under_permutation(list of var int: xs, list of int: permutations) =
  let {
    int: n = length(xs);
    int: m = length(permutations) div n;
    array [1..m, 1..n] of int: p = array2d(1..m, 1..n, permutations);
  } in forall (i, j in 1..m where i != j) (
    fzn_var_perm_sym_pairwise(xs, [p[i, k] | k in 1..n], [p[j, k] | k in 1..n])
  );

predicate fzn_var_perm_sym_pairwise(
  array [int] of var int: x,
  array [int] of int: p1,
  array [int] of int: p2,
) =
  let {
    int: n = length(x);
    array [1..n] of 1..n: invp1 = [j | i, j in 1..n where p1[j] = i];
  } in lex_lesseq(x, [x[p2[invp1[i]]] | i in 1..n]);
