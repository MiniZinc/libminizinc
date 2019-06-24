include "tree.mzn";

predicate fzn_wst(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                  array[int] of var bool: es, var int: K) =
    let { var 1..N: r; /* root of tree */
          array[1..N] of bool: ns = [true | n in 1..N]; } in
        tree(N,E,from,to,r,ns,es) /\
        K = sum(e in 1..E)(es[e]*w[e]);
   
%-----------------------------------------------------------------------------%
