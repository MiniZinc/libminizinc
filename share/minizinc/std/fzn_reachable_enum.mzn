include "subgraph.mzn";

predicate fzn_reachable(array[int] of $$N: from, array[int] of $$N: to,
                   var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let {
        int: E = length(es);
        set of int: NODE = min(index_set(ns))..max(index_set(ns));
        array[1..2*E] of NODE: dfrom = from ++ to;
        array[1..2*E] of NODE: dto   = to ++ from;
        array[1..2*E] of var bool: des = es ++ es;
        array[NODE] of var bool: dns = array1d(NODE,ns);
        var NODE: dr = r;
    } in
        /* duplicate the edges so that we can use directed graph reachability */
        fzn_dreachable(dfrom,dto,dr,dns,des);
   
%-----------------------------------------------------------------------------%
