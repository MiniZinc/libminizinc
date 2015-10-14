%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for value sequence symmetries.
% Prunes any symmetrically equivalent solutions by taking an order for the
% variables then constraining their values so that no value sequence swap
% across x (pairwise swapping the values of two value sequences across all
% variables in x) will result in a lexicographically greater solution.
%-----------------------------------------------------------------------------%

include "lex_lesseq.mzn";

predicate val_seq_sym_int(array[int] of var int: x,
                          array[int,int] of int: s) :: symmetry =
    let { int: l1 = min(index_set_1of2(s)),
          int: u1 = max(index_set_1of2(s)),
          int: l2 = min(index_set_2of2(s)),
          int: u2 = max(index_set_2of2(s)),
          set of int: A = { min([s[i,j] | i in l1..u1]) | j in l2..u2 },
          set of int: B = { s[i,j] | i in l1..u1, j in l2..u2 } diff A,
          int: l = min(index_set(x)),
          int: u = max(index_set(x)),
          array[l..u] of var 0..2: y }
    in
    forall (i in index_set(x)) (
        (y[i] = 0 <-> (x[i] in A)) /\ (y[i] = 2 <-> (x[i] in B))
    )
    /\
    lex_lesseq(y, [ 1 | i in index_set(x) ]);

predicate val_seq_sym_int_ord(array[int] of var int: x,
                              array[int,int] of int: s,
                              array[int] of var int: order) =
    let { array[1..length(x)] of var int: x2 = sort_by_var(x, order) }
    in
    val_seq_sym_int(x2, s);
