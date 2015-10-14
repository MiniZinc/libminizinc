%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable sequence symmetries.
% Prunes any symmetrically equivalent solutions by taking an order for the
% variables then constraining their values so that no sequence swap (pairwise
% swapping the values of two variable sequences) results in a lexicographically
% greater solution.
%-----------------------------------------------------------------------------%

include "lex_lesseq.mzn";
include "var_perm_sym_int.mzn";

predicate var_seq_sym_int(array[int,int] of var int: x) :: symmetry =
    let { int: l1 = min(index_set_1of2(x)),
          int: u1 = max(index_set_1of2(x)),
	  int: l2 = min(index_set_2of2(x)),
          int: u2 = max(index_set_2of2(x)) }
    in
    forall (i in l1..u1-1) (
        lex_lesseq([ x[i,j] | j in l2..u2 ], [ x[i+1,j] | j in l2..u2 ])
    );

predicate var_seq_sym_int_ord(array[int,int] of var int: x,
	                      array[int] of var int: order) =
    let { int: l = min(index_set_1of2(x)),
          int: u = max(index_set_1of2(x)),
	  int: n = 2*card(index_set_2of2(x)) }
    in
    forall (i in l..u, j in i+1..u) (
        let { array[1..n] of var int: y =
                     [ x[i,k] | k in index_set_2of2(x) ]
                  ++ [ x[j,k] | k in index_set_2of2(x) ],
              array[1..2,1..n] of int: p = array2d(1..2, 1..n,
                     [ k | k in 1..n ]
                  ++ [ k + (n div 2) | k in 1..(n div 2) ]
                  ++ [ k | k in 1..(n div 2) ]) }
        in
        var_perm_sym_int_ord(y, p, order)
    );
