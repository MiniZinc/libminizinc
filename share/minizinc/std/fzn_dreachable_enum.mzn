include "subgraph.mzn";

predicate fzn_dreachable(array[int] of $$N: from, array[int] of $$N: to,
                        var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { 
        array[index_set(ns)] of var 0..card(index_set(ns))-1: dist; /* distance from root */
        array[index_set(ns)] of var index_set(ns): parent; /* parent */
    } in
        ns[r] /\ % the root must be chosen
        dist[r] = 0 /\ % root is at distance 0
        parent[r] = r /\ % root is its own parent
        forall(n in index_set(ns)) % nonselected nodes have parent 0
              (not ns[n] -> parent[n] = n) /\
        forall(n in index_set(ns)) % nonselected nodes have distance 0
              (not ns[n] -> dist[n] = 0) /\
        forall(n in index_set(ns)) % each in node except root must have a parent
              (ns[n] -> (n = r \/ parent[n] != n)) /\
        forall(n in index_set(ns)) % each in node with a parent must be in and also its parent
              (parent[n] != n -> (ns[n] /\ ns[parent[n]])) /\
        forall(n in index_set(ns)) % each except with a parent is one more than its parent
              (parent[n] != n -> dist[n] = dist[parent[n]] + 1) /\
        forall(n in index_set(ns)) % each node with a parent must have that edge in
              (parent[n] != n -> exists(e in index_set(from) where to[e] = n)(es[e] /\ from[e] = parent[n])) /\
        subgraph(from,to,ns,es);
   
%-----------------------------------------------------------------------------%
