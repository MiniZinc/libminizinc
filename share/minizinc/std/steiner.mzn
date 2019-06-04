include "fzn_steiner.mzn";
include "wst.mzn";

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
predicate dsteiner(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                        var int: r, array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
    assert(index_set(from) = 1..E,"dsteiner: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"dsteiner: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N,"dsteiner: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"dsteiner: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E,"dsteiner: index set of w must be 1..\(E)") /\
    if  forall(n in 1..N)(is_fixed(ns[n]) /\ fix(ns[n])) then
        dwst(N,E,from,to,w,r,es,K)
    else
        fzn_dsteiner(N,E,from,to,w,r,ns,es,K)
    endif;

/** @group globals.graph
    Constrains the set of edges \a es of a given undirected graph to be a weighted spanning tree of weight \a W.

    \a N is the number of nodes in the given graph
    \a E is the number of edges in the given graph
    \a from is the leaving node 1..\a N for each edge
    \a to is the entering node 1..\a N for each edge
    \a w is the weight of each edge
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
    \a K is the weight of the tree
**/   
predicate steiner(int: N, int: E, array[int] of int: from, array[int] of int: to, array[int] of int: w,
                  array[int] of var bool: ns, array[int] of var bool: es, var int: K) =
    assert(index_set(from) = 1..E,"steiner: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"steiner: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N,"steiner: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"steiner: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E,"steiner: index set of w must be 1..\(E)") /\
    if  forall(n in 1..N)(is_fixed(ns[n]) /\ fix(ns[n])) then
        wst(N,E,from,to,w,es,K)
    else
        fzn_steiner(N,E,from,to,w,ns,es,K)
    endif;
   
%-----------------------------------------------------------------------------%
