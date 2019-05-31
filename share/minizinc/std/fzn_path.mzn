/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a path from \a s to \a t.

   \a N is the number of nodes in the given graph
   \a E is the number of edges in the given graph
   \a from is the leaving node 1..\a N for each edge
   \a to is the entering node 1..\a N for each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate fzn_dpath(int: N, int: E, array[int] of int: from, array[int] of int: to,
                    var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es) =
   dtree(N,E,from,to,s,ns,es) /\
   dtree(N,E,to,from,t,ns,es);

include "tree.mzn";

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a path from \a s to \a t.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/
/*   
predicate fzn_dpath(array[int] of $$N: from, array[int] of $$N: to,
                        var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es) =
   dtree(from,to,s,ns,es) /\
   dtree(to,from,t,ns,es);
*/   

/* alternate definition */
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

 include "subgraph.mzn";  
   
/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be a path from \a s to \a t..

   \a N is the number of nodes in the given graph
   \a E is the number of edges in the given graph
   \a from is the leaving node 1..\a N for each edge
   \a to is the entering node 1..\a N for each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/   
predicate fzn_path(int: N, int: E, array[int] of int: from, array[int] of int: to,
                   var int: s, var int: t, array[int] of var bool: ns, array[int] of var bool: es) =
   let { array[1..2*E] of int: dfrom = from ++ to;
         array[1..2*E] of int: dto   = to ++ from;
         array[1..2*E] of var bool: des;
   } in
   /* ensure that the directed edges selected agree with undirected edges */
   forall(e in 1..E)(es[e] <-> (des[e] \/ des[e+E])) /\
   /* duplicate the edges so that the we can use directed graph path */
   dpath(N,2*E,dfrom,dto,s,t,ns,des);
   
/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be a path from \a s to \a t.

   \a from is the leaving node for each edge
   \a to is the entering node for each edge
   \a s is the source node (which may be variable)
   \a t is the dest node (which may be variable)
   \a ns is a Boolean for each node whether it is in the subgraph
   \a es is a Boolean for each edge whether it is in the subgraph
*/   
predicate fzn_path(array[int] of $$N: from, array[int] of $$N: to,
                        var $$N: s, var $$N: t, array[$$N] of var bool: ns, array[int] of var bool: es) =
   let { int: E = length(es);
         array[1..2*E] of int: dfrom = from ++ to;
         array[1..2*E] of int: dto   = to ++ from;
         array[1..2*E] of var bool: des;
   } in
   /* ensure that the directed edges selected agree with undirected edges */
   forall(e in 1..E)(es[e-1+min(index_set(es))] <-> (des[e] \/ des[e+E])) /\
   /* duplicate the edges so that the we can use directed graph path */
   dpath(dfrom,dto,s,t,ns,des);

   
%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
