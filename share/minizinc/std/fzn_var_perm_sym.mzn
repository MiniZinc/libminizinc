include "lex_lesseq.mzn";

predicate fzn_var_perm_sym(array[int] of var int: x,
                       array[int,int] of int: p) =
  let { int: l = min(index_set_1of2(p)),
        int: u = max(index_set_1of2(p)),
	array[1..length(x)] of var int:
          y = [ x[i] | i in index_set(x) ] }
  in
  forall (i, j in l..u where i != j) (
    fzn_var_perm_sym_pairwise(y,
      [ p[i,k] | k in index_set_2of2(p) ],
      [ p[j,k] | k in index_set_2of2(p) ])
  );

predicate fzn_var_perm_sym_pairwise(
    array[int] of var int: x,
    array[int] of int: p1,
    array[int] of int: p2) =
  let { int: n = length(x),
        array[1..n] of 1..n:
          invp1 = [ j | i, j in 1..n
                      where p1[j] = i ] }
  in
  lex_lesseq(x, [ x[p2[invp1[i]]] | i in 1..n ]);
