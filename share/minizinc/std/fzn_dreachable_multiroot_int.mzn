include "subgraph.mzn";

predicate fzn_dreachable_multiroot(int: N, int: E, array[int] of int: from, array[int] of int: to,
                                   array[int] of var bool: rs, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        array[1..N] of var 0..N-1: dist; /* distance from root */
        array[1..N] of var 1..N: parent; /* parent */
    } in
        forall(n in 1..N) % root nodes must be chosen
              (rs[n] -> ns[n]) /\
        forall(n in 1..N) % self-parent is the same as either non-chosen node or root node
              ((parent[n] = n) == ((not ns[n]) \/ rs[n])) /\
        forall(n in 1..N) % each non-self-parent node is one more step removed from a root than its parent node
              (parent[n] != n -> dist[n] = dist[parent[n]] + 1) /\
        forall(n in 1..N) % each non-self-parent node must have a chosen edge from its parent
              (parent[n] != n -> exists(e in 1..E where to[e] = n)(es[e] /\ from[e] = parent[n])) /\
        % nodes connected by chosen edges must be chosen
        subgraph(N,E,from,to,ns,es);

%-----------------------------------------------------------------------------%
