include "subgraph.mzn";

predicate fzn_dreachable_multiroot(array[$$E] of $$N: from, array[$$E] of $$N: to,
                                   array[$$N] of var bool: rs, array[$$N] of var bool: ns, array[$$E] of var bool: es) =
    let {
        array[index_set(ns)] of var 0..card(index_set(ns))-1: dist; /* distance from root */
        array[index_set(ns)] of var index_set(ns): parent; /* parent */
    } in
        forall(n in index_set(ns)) % root nodes must be chosen
              (rs[n] -> ns[n]) /\
        forall(n in index_set(ns)) % self-parent is the same as either non-chosen node or root node
              ((parent[n] = n) == ((not ns[n]) \/ rs[n])) /\
        forall(n in index_set(ns)) % each non-self-parent node is one more step removed from a root than its parent node
              (parent[n] != n -> dist[n] = dist[parent[n]] + 1) /\
        forall(n in index_set(ns)) % each non-self-parent node must have a chosen edge from its parent
              (parent[n] != n -> exists(e in index_set(from) where to[e] = n)(es[e] /\ from[e] = parent[n])) /\
        % nodes connected by chosen edges must be chosen
        subgraph(from,to,ns,es);

%-----------------------------------------------------------------------------%
