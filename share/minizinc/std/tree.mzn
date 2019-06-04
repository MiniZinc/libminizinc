include "fzn_tree.mzn";
   
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
predicate dtree(int: N, int: E, array[int] of int: from, array[int] of int: to,
                var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = 1..E,"dtree: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"dtree: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N,"dtree: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"dtree: index set of es must be 1..\(E)") /\
    fzn_dtree(N,E,from,to,r,ns,es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be at tree rooted at \a r.

    \a from is the leaving node for each edge
    \a to is the entering node for each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate dtree(array[int] of $$N: from, array[int] of $$N: to,
                var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = index_set(to),"dreachable: index set of from and to must be identical") /\
    assert(index_set(from) = index_set(es),"dreachable: index set of from and es must be identical") /\
    assert(dom_array(from) subset index_set(ns),"dreachable: nodes in from must be in index set of ns") /\
    assert(dom_array(from) subset index_set(ns),"dreachable: nodes in to must be in index set of ns") /\
    fzn_dtree(from,to,r,ns,es);

%-----------------------------------------------------------------------------%

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
predicate tree(int: N, int: E, array[int] of int: from, array[int] of int: to,
               var int: r, array[int] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = 1..E,"tree: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"tree: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N,"tree: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"tree: index set of es must be 1..\(E)") /\
    fzn_tree(N,E,from,to,r,ns,es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be at tree rooted at \a r.

    \a from is the leaving node for each edge
    \a to is the entering node for each edge
    \a r is the root node (which may be variable)
    \a ns is a Boolean for each node whether it is in the subgraph
    \a es is a Boolean for each edge whether it is in the subgraph
*/
predicate tree(array[int] of $$N: from, array[int] of $$N: to,
               var $$N: r, array[$$N] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = index_set(to),"dreachable: index set of from and to must be identical") /\
    assert(index_set(from) = index_set(es),"dreachable: index set of from and es must be identical") /\
    assert(dom_array(from) subset index_set(ns),"dreachable: nodes in from must be in index set of ns") /\
    assert(dom_array(from) subset index_set(ns),"dreachable: nodes in to must be in index set of ns") /\
    fzn_tree(from,to,r,ns,es);   