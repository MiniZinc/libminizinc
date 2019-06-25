include "fzn_wst.mzn";
include "fzn_wst_reif.mzn";
include "fzn_dwst.mzn";
include "fzn_dwst_reif.mzn";

/** @group globals.graph
    Constrains the set of edges \a es of a given directed graph to be a weighted spanning tree rooted at \a r of weight \a W.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a w is the weight of each edge
    \a r is the root node (which may be variable)
    \a es is a Boolean for each edge whether it is in the subgraph
    \a K is the weight of the tree
*/
predicate d_weighted_spanning_tree(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                        var int: r, array[int] of var bool: es, var int: K) =
    assert(index_set(from) = 1..E,"dwst: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"dwst: index set of to must be 1..\(E)") /\
    assert(index_set(es) = 1..E,"dwst: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E,"dwst: index set of w must be 1..\(E)") /\
    fzn_dwst(N,E,from,to,w,r,es,K);

/** @group globals.graph
    Constrains the set of edges \a es of a given undirected graph to be a weighted spanning tree of weight \a W.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a w is the weight of each edge
    \a es is a Boolean for each edge whether it is in the subgraph
    \a K is the weight of the tree
**/   
predicate weighted_spanning_tree(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
              array[int] of var bool: es, var int: K) =
    assert(index_set(from) = 1..E,"wst: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"wst: index set of to must be 1..\(E)") /\
    assert(index_set(es) = 1..E,"wst: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E,"dwst: index set of w must be 1..\(E)") /\
    fzn_wst(N,E,from,to,w,es,K);

%-----------------------------------------------------------------------------%
