predicate fzn_reachable_reif(int: N, int: E, array[int] of int: from, array[int] of int: to,
                             var int: r, array[int] of var bool: ns, array[int] of var bool: es,
                             var bool: b) =
    let {
        array[1..2*E] of int: dfrom = from ++ to;
        array[1..2*E] of int: dto   = to ++ from;
        array[1..2*E] of var bool: des = es ++ es;
    } in
        /* duplicate the edges so that the we can use directed graph reachability */
        b <-> dreachable(N,2*E,dfrom,dto,r,ns,des);
     
%-----------------------------------------------------------------------------%
