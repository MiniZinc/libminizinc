include "path.mzn";

predicate fzn_bounded_path(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                   var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
   path(N,E,from,to,s,t,ns,es) /\
   K = sum(e in 1..E)(es[e]*w[e]);
