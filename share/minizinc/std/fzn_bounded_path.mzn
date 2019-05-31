/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a path from \a s to \a t of weight \a K.

   \a N is the number of nodes in the given graph
   \a E is the number of edges in the given graph
   \a from is the leaving node 1..\a N for each edge
   \a to is the entering node 1..\a N for each edge
   \a w is the weight of each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
   \a K is the cost of the path
*/
predicate fzn_bounded_dpath(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                    var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
   dpath(N,E,from,to,s,t,ns,es) /\
   K = sum(e in 1..E)(es[e]*w[e]);

include "path.mzn";

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a path from \a s to \a t of weight \a K.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a w is the weight of each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
   \a K is the cost of the path
*/   
predicate fzn_bounded_dpath(array[int] of $$N: from, array[int] of $$N: to, array[int] of int: w,
                        var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es, var int: K) =
   dpath(from,to,s,t,ns,es) /\
   K = sum(e in index_set(es))(es[e]*w[e]);



   
/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be a path from \a s to \a t of weight \a K.

   \a N is the number of nodes in the given graph
   \a E is the number of edges in the given graph
   \a from is the leaving node 1..\a N for each edge
   \a to is the entering node 1..\a N for each edge
   \a w is the weight of each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
   \a K is the cost of the path
*/   
predicate fzn_bounded_path(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                   var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
   path(N,E,from,to,s,t,ns,es) /\
   K = sum(e in 1..E)(es[e]*w[e]);

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be a path from \a s to \a t of weight \a K.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a w is the weight of each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
   \a K is the cost of the path
*/   
predicate fzn_bounded_path(array[int] of $$N: from, array[int] of $$N: to, array[int] of int: w,
                        var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es, var int: K) =
   path(from,to,s,t,ns,es) /\
   K = sum(e in index_set(es))(es[e]*w[e]);

   
%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
