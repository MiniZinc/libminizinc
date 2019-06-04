include "reachable.mzn";

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph is a connected subgraph.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_dconnected(array[int] of $$N: from, array[int] of $$N: to,
                         array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { var index_set(ns): r } in
        dreachable(from, to, r, ns, es);

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph is connected.

   \a from is the leaving node 1..\a N for each edge
   \a to is the entering node 1..\a N for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/   
predicate fzn_connected(array[int] of $$N: from, array[int] of $$N: to,
                        array[$$N] of var bool: ns, array[int] of var bool: es) =
    let { var index_set(ns): r } in
        reachable(from, to, r, ns, es);

   
%-----------------------------------------------------------------------------%
