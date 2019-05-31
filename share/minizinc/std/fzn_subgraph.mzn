/** @group globals.graph
   Constrains that \a ns and \a es represent a subgraph of a given directed graph.

   \a N is the number of nodes in the given graph
   \a E is the number of edges in the given graph
   \a from is the leaving node 1..\a N for each edge
   \a to is the entering node 1..\a N for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_subgraph(int: N, int: E, array[int] of int: from, array[int] of int: to,
                       array[int] of var bool: ns, array[int] of var bool: es) =
   forall(e in 1..E)
         ((es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]]));

/** @group globals.graph
   Constrains that \a ns and \a es represent a subgraph of a given directed graph.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_subgraph(array[int] of $$N: from, array[int] of $$N: to,
                       array[$$N] of var bool: ns, array[int] of var bool: es) =
   forall(e in index_set(from))
         ((es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]]));

   
%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
