include "tree.mzn";

predicate fzn_steiner(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                      array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
    let { var 1..N: r; } in
    tree(N,E,from,to,r,ns,es) /\
    K = sum(e in 1..E)(es[e]*w[e]);
   
%-----------------------------------------------------------------------------%
