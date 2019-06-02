include "subgraph.mzn";

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be a tree rooted at \a r.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_dtree(int: N, int: E, array[int] of int: from, array[int] of int: to,
                        var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        set of int: NODE = 1..N;
        set of int: EDGE = 1..E;
        array[NODE] of var 0..N-1: dist; /* distance from root */
        array[NODE] of var 0..N: parent; /* parent */
    } in
        ns[r] /\ % the root must be chosen
        dist[r] = 0 /\ % root is at distance 0
        forall(n in NODE) % nonselected nodes have parent 0
              (not ns[n] -> parent[n] <= 0) /\
        forall(n in NODE) % nonselected nodes have distance 0
              (not ns[n] -> dist[n] = 0) /\
        forall(n in NODE) % each in node except root must have a parent
              (ns[n] -> (n = r \/ parent[n] > 0)) /\
        forall(n in NODE) % each node with a parent then parent is in
              (parent[n] > 0 -> (ns[n] /\ ns[parent[n]])) /\ 
        forall(n in NODE) % each node with a parent is one more than its parent
              (parent[n] > 0 -> dist[n] = dist[parent[n]] + 1) /\
        forall(n in NODE) % each node with a parent must have that edge in
              (parent[n] > 0 -> exists(e in EDGE)(es[e] /\ from[e] = parent[n] /\ to[e] = n)) /\
        forall(e in EDGE) % each edge must be part of the parent relation
              (es[e] -> parent[to[e]] = from[e]) /\
        sum(e in EDGE)(es[e]) = sum(n in NODE)(ns[n]) - 1 /\ % redundant relationship of trees
        subgraph(N,E,from,to,ns,es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be a tree rooted at \a r.

    \a from is the leaving node for each edge
    \a to is the entering node for each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_dtree(array[int] of $$N: from, array[int] of $$N: to,
                        var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let {
        set of int: EDGE = index_set(es);
        array[index_set(ns)] of var 0..length(ns)-1: dist; /* distance from root */
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
              (parent[n] != n -> exists(e in EDGE where to[e] = n)(es[e] /\ from[e] = parent[n])) /\
        forall(e in EDGE) % each edge must be part of the parent relation
              (es[e] -> parent[to[e]] = from[e]) /\
        sum(e in EDGE)(es[e]) = sum(n in index_set(ns))(ns[n]) - 1 /\ % redundant relationship of trees
        subgraph(from,to,ns,es);
   
/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be a tree rooted at \a r.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
*/   
predicate fzn_tree(int: N, int: E, array[int] of int: from, array[int] of int: to,
                        var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
    let {
        array[1..2*E] of int: dfrom = from ++ to;
        array[1..2*E] of int: dto   = to ++ from;
        array[1..2*E] of var bool: des;
    } in
      /* ensure that the directed edges selected agree with undirected edges */
      forall(e in 1..E)(es[e] <-> (des[e] \/ des[e+E])) /\
      /* duplicate the edges so that the we can use directed graph reachability */
      fzn_dtree(N,2*E,dfrom,dto,r,ns,des);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be a tree rooted at \a r.

    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
*/   
predicate fzn_tree(array[int] of $$N: from, array[int] of $$N: to,
                   var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    let {
        int: E = length(es);
        array[1..2*E] of int: dfrom = from ++ to;
        array[1..2*E] of int: dto   = to ++ from;
        array[1..2*E] of var bool: des;
    } in
        /* ensure that the directed edges selected agree with undirected edges */
        forall(e in 1..E)(es[e-1+min(index_set(es))] <-> (des[e] \/ des[e+E])) /\
        /* duplicate the edges so that the we can use directed graph reachability */
        fzn_dtree(dfrom,dto,r,ns,des);
   
%-----------------------------------------------------------------------------%
