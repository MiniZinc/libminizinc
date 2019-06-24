include "tree.mzn";
include "subgraph.mzn";  

predicate fzn_dpath(array[int] of $$N: from, array[int] of $$N: to,
                    var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { set of int: EDGE = index_set(es);
          array[index_set(ns)] of var 0..length(ns)-1: dist; /* distance from root */
    } in
        ns[s] /\ % source is selected
        ns[t] /\ % dest is selected
        dist[s] = 0 /\ % distance of source is zero
        forall(n in index_set(ns)) % nonselected nodes have distance 0
              (not ns[n] -> dist[n] = 0) /\
        forall(n in index_set(ns))   % 1 incoming edge
              ((ns[n] /\ n != s) -> exists(e in EDGE where to[e] = n)(es[e])) /\
        forall(n in index_set(ns))   % 1 outgoing edge
              ((ns[n] /\ n != t) -> exists(e in EDGE where from[e] = n)(es[e])) /\
        forall(n in index_set(ns))   % 1 incoming edge
              ((ns[n] /\ n != s) -> sum(e in EDGE where to[e] = n)(es[e]) <= 1) /\
        forall(n in index_set(ns))   % 1 outgoing edge
              ((ns[n] /\ n != t) -> sum(e in EDGE where from[e] = n)(es[e]) <= 1) /\
      % alternate for the previous 8 lines
      %   forall(n in index_set(ns))   % 1 incoming edge
      %         ((ns[n] /\ n != s) -> sum(e in EDGE where to[e] = n)(es[e]) = 1) /\
      %   forall(n in index_set(ns))   % 1 outgoing edge
      %         ((ns[n] /\ n != t) -> sum(e in EDGE where from[e] = n)(es[e]) = 1) /\
        forall(e in EDGE)
              (es[e] -> dist[to[e]] = dist[from[e]] + 1) /\
        sum(n in index_set(ns))(ns[n]) = sum(e in EDGE)(es[e]) + 1 /\
        subgraph(from,to,ns,es);

%-----------------------------------------------------------------------------%
