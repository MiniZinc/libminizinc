predicate fzn_tree(array[int] of $$N: from, array[int] of $$N: to,
                   var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let {
        int: E = length(es);
        array[1..2*E] of int: dfrom = from ++ to;
        array[1..2*E] of int: dto   = to ++ from;
        array[1..2*E] of var bool: des;
    } in
        /* ensure that the directed edges selected agree with undirected edges */
        forall(e in 1..E)(es[e-1+min(index_set(es))] <-> (des[e] \/ des[e+E])) /\
        /* duplicate the edges so that the we can use directed graph reachability */
        fzn_dtree(dfrom,dto,r,ns,des);
   
%-----------------------------------------------------------------------------%
