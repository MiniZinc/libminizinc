include "lex_lesseq.mzn";

predicate fzn_var_perm_sym(array [int] of var int: xs, array [int, int] of int: permutations) =
  let {
    int: l = min(index_set_1of2(permutations));
    int: u = max(index_set_1of2(permutations));
    array [1..length(xs)] of var int: y = [xs[i] | i in index_set(xs)];
  } in forall (i, j in l..u where i != j) (
    fzn_var_perm_sym_pairwise(
      y,
      [permutations[i, k] | k in index_set_2of2(permutations)],
      [permutations[j, k] | k in index_set_2of2(permutations)],
    )
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
