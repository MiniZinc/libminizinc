include "tree.mzn";

/** @group globals.graph
    Constrains the subgraph \a  ns and \a es of a given directed graph to be a weighted spanning tree rooted at \a r of weight \a W.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a w is the weight of each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
    \a K is the weight of the tree
*/
predicate fzn_dsteiner(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                        var int: r, array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
    dtree(N,E,from,to,r,ns,es) /\
    K = sum(e in 1..E)(es[e]*w[e]);

/** @group globals.graph
    Constrains the subgraph \a  ns and \a es of a given undirected graph to be a weighted spanning tree of weight \a W.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a w is the weight of each edge
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
    \a K is the weight of the tree
**/   
predicate fzn_steiner(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                      array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
    let { var 1..N: r; } in
    tree(N,E,from,to,r,ns,es) /\
    K = sum(e in 1..E)(es[e]*w[e]);
   
%-----------------------------------------------------------------------------%
