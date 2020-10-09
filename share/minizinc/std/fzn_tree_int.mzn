include "subgraph.mzn";

predicate fzn_tree(int: N, int: E, array[int] of int: from, array[int] of int: to,
                        var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        array[1..2*E] of int: dfrom = from ++ to;
        array[1..2*E] of int: dto   = to ++ from;
        array[1..2*E] of var bool: des;
    } in
      /* ensure that the directed edges selected agree with undirected edges */
      forall(e in 1..E)(es[e] <-> (des[e] \/ des[e+E])) /\
      /* duplicate the edges so that the we can use directed graph reachability */
      fzn_dtree(N,2*E,dfrom,dto,r,ns,des);
   
%-----------------------------------------------------------------------------%
