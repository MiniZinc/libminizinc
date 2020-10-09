include "tree.mzn";

predicate fzn_dwst(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                   var int: r, array[int] of var bool: es, var int: K) =
   let { array[1..N] of bool: ns = [true | n in 1..N]; } in
      dtree(N,E,from,to,r,ns,es) /\
      K = sum(e in 1..E)(es[e]*w[e]);
   
%-----------------------------------------------------------------------------%
