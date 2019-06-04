/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a DAG.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_dag(array[int] of $$N: from, array[int] of $$N: to,
                    array[$$N] of var bool: ns, array[int] of var bool: es) =
   let {
         set of int: EDGE = index_set(es);
         array[index_set(ns)] of var 0..length(ns)-1: dist; /* distance of longest path */
   
   } in
   forall(n in index_set(ns))(not ns[n] -> dist[n] = 0) /\
   forall(e in EDGE)
         (es[e] -> dist[from[e]] + 1 <= dist[to[e]])
   /\
   % redundant constraint to ensure all distances are fixed
   forall(n in index_set(ns))
         (dist[n] = max( [0] ++ [ (dist[from[e]] + 1)*es[e] | e in EDGE where to[e] = n ]))  
   ;
