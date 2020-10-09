include "tree.mzn";
include "subgraph.mzn";  

predicate fzn_path(array[int] of $$N: from, array[int] of $$N: to,
                        var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { int: E = length(es);
          array[1..2*E] of int: dfrom = from ++ to;
          array[1..2*E] of int: dto   = to ++ from;
          array[1..2*E] of var bool: des;
    } in
        /* ensure that the directed edges selected agree with undirected edges */
        forall(e in 1..E)(es[e-1+min(index_set(es))] <-> (des[e] \/ des[e+E])) /\
        /* duplicate the edges so that the we can use directed graph path */
        fzn_dpath(dfrom,dto,s,t,ns,des);

%-----------------------------------------------------------------------------%
