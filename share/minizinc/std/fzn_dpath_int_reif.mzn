include "tree.mzn";
include "subgraph.mzn";  

predicate fzn_dpath_reif(int: N, int: E, array[int] of int: from, array[int] of int: to,
                         var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es,
                         var bool: b) =
   b <-> ( dtree(N,E,from,to,s,ns,es) /\
           dtree(N,E,to,from,t,ns,es) );

%-----------------------------------------------------------------------------%
