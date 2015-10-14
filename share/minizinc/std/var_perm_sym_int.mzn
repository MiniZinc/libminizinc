%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable permutation
% symmetries.
% Prunes any symmetrically equivalent solutions by taking an order for the
% variables then constraining their values so that no permutation swap (applying
% the inverse of a permutation in p then applying another permutation in p)
% results in a lexicographically greater solution.
%-----------------------------------------------------------------------------%

include "lex_lesseq.mzn";

predicate var_perm_sym_int(array[int] of var int: x,
                           array[int,int] of int: p) :: symmetry =
    let { int: l = min(index_set_1of2(p)),
          int: u = max(index_set_1of2(p)),
	  array[1..length(x)] of var int: y = [ x[i] | i in index_set(x) ] }
    in
    forall (i, j in l..u where i != j) (
        var_perm_sym_int_pairwise(y, [ p[i,k] | k in index_set_2of2(p) ],
                                     [ p[j,k] | k in index_set_2of2(p) ])
    );

predicate var_perm_sym_int_pairwise(array[int] of var int: x,
                                    array[int] of int: p1,
                                    array[int] of int: p2) =
    let { int: n = length(x),
          array[1..n] of 1..n: invp1 = [ j | i, j in 1..n where p1[j] = i ] }
    in
    lex_lesseq(x, [ x[p2[invp1[i]]] | i in 1..n ]);

predicate var_perm_sym_int_ord(array[int] of var int: x,
	                       array[int,int] of int: p,
                               array[int] of var int: order) =
    let { int: n = length(x),
          int: np = card(index_set_1of2(p)),
          array[1..n] of 1..n: r = index_sort(x, order),
          array[1..np,1..n] of 1..n: pr = array2d(1..np,1..n,
              [ p[i,r[j]] | i in index_set_1of2(p), j in 1..n ]) }
    in
    var_perm_sym_int(x, pr);
