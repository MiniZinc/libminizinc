include "reachable.mzn";

predicate fzn_dconnected(array[int] of $$N: from, array[int] of $$N: to,
                         array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { var index_set(ns): r } in
        dreachable(from, to, r, ns, es);
   
%-----------------------------------------------------------------------------%
