include "subgraph.mzn";

predicate fzn_reachable_multiroot(array[$$E] of $$N: from, array[$$E] of $$N: to,
                                  array[$$N] of var bool: rs, array[$$N] of var bool: ns, array[$$E] of var bool: es) =
    let {
        int: E = length(es);
        set of int: NODE = index_set(ns);
        array[1..2*E] of NODE: dfrom   = from ++ to;
        array[1..2*E] of NODE: dto     = to ++ from;
        array[1..2*E] of var bool: des = es ++ es;
    } in
        /* duplicate the edges so that we can use directed graph reachability */
        fzn_dreachable_multiroot(dfrom,dto,rs,ns,des);

%-----------------------------------------------------------------------------%
