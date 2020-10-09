include "reachable.mzn";

predicate fzn_connected(array[int] of $$N: from, array[int] of $$N: to,
                        array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { var index_set(ns): r } in
        reachable(from, to, r, ns, es);

   
%-----------------------------------------------------------------------------%
