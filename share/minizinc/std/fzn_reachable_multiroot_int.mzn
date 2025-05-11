include "subgraph.mzn";

predicate fzn_reachable_multiroot(int: N, int: E, array[int] of int: from, array[int] of int: to,
                                  array[int] of var bool: rs, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        array[1..2*E] of 1..N: dfrom   = from ++ to;
        array[1..2*E] of 1..N: dto     = to ++ from;
        array[1..2*E] of var bool: des = es ++ es;
    } in
        /* duplicate the edges so that we can use directed graph reachability */
        fzn_dreachable_multiroot(N,2*E,dfrom,dto,rs,ns,des);

%-----------------------------------------------------------------------------%
