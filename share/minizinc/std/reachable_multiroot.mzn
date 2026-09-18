include "fzn_reachable_multiroot_int.mzn";
include "fzn_reachable_multiroot_int_reif.mzn";
include "fzn_reachable_multiroot_enum.mzn";
include "fzn_reachable_multiroot_enum_reif.mzn";
include "fzn_dreachable_multiroot_int.mzn";
include "fzn_dreachable_multiroot_int_reif.mzn";
include "fzn_dreachable_multiroot_enum.mzn";
include "fzn_dreachable_multiroot_enum_reif.mzn";

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be reachable from roots \a rs.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param rs: a Boolean for each node whether it is a root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate dreachable_multiroot(int: N, int: E, array[int] of int: from, array[int] of int: to,
                               array[int] of var bool: rs, array[int] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = 1..E,"dreachable_multiroot: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"dreachable_multiroot: index set of to must be 1..\(E)") /\
    assert(index_set(rs) = 1..N,"dreachable_multiroot: index set of rs must be 1..\(N)") /\
    assert(index_set(ns) = 1..N,"dreachable_multiroot: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"dreachable_multiroot: index set of es must be 1..\(E)") /\
    fzn_dreachable_multiroot(N,E,from,to,rs,ns,es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be reachable from roots \a rs.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param rs: a Boolean for each node whether it is a root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate dreachable_multiroot(array[$$E] of $$N: from, array[$$E] of $$N: to,
                               array[$$N] of var bool: rs, array[$$N] of var bool: ns, array[$$E] of var bool: es) =
    assert(index_set(from) = index_set(to),"dreachable_multiroot: index set of from and to must be identical") /\
    assert(index_set(from) = index_set(es),"dreachable_multiroot: index set of from and es must be identical") /\
    assert(dom_array(from) subset index_set(rs),"dreachable_multiroot: nodes in from must be in index set of rs") /\
    assert(dom_array(to) subset index_set(rs),"dreachable_multiroot: nodes in to must be in index set of rs") /\
    assert(dom_array(from) subset index_set(ns),"dreachable_multiroot: nodes in from must be in index set of ns") /\
    assert(dom_array(to) subset index_set(ns),"dreachable_multiroot: nodes in to must be in index set of ns") /\
    fzn_dreachable_multiroot(
        index2int(enum2int(from)),
        index2int(enum2int(to)),
        index2int(rs),
        index2int(ns),
        index2int(es)
    );

%-----------------------------------------------------------------------------%

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be reachable from roots \a rs.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param rs: a Boolean for each node whether it is a root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate reachable_multiroot(int: N, int: E, array[int] of int: from, array[int] of int: to,
                              array[int] of var bool: rs, array[int] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = 1..E,"reachable_multiroot: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"reachable_multiroot: index set of to must be 1..\(E)") /\
    assert(index_set(rs) = 1..N,"reachable_multiroot: index set of rs must be 1..\(N)") /\
    assert(index_set(ns) = 1..N,"reachable_multiroot: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"reachable_multiroot: index set of es must be 1..\(E)") /\
    fzn_reachable_multiroot(N,E,from,to,rs,ns,es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be reachable from roots \a rs.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param rs: a Boolean for each node whether it is a root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate reachable_multiroot(array[$$E] of $$N: from, array[$$E] of $$N: to,
                              array[$$N] of var bool: rs, array[$$N] of var bool: ns, array[$$E] of var bool: es) =
    assert(index_set(from) = index_set(to),"reachable_multiroot: index set of from and to must be identical") /\
    assert(index_set(from) = index_set(es),"reachable_multiroot: index set of from and es must be identical") /\
    assert(dom_array(from) subset index_set(rs),"reachable_multiroot: nodes in from must be in index set of rs") /\
    assert(dom_array(to) subset index_set(rs),"reachable_multiroot: nodes in to must be in index set of rs") /\
    assert(dom_array(from) subset index_set(ns),"reachable_multiroot: nodes in from must be in index set of ns") /\
    assert(dom_array(to) subset index_set(ns),"reachable_multiroot: nodes in to must be in index set of ns") /\
    fzn_reachable_multiroot(
        index2int(enum2int(from)),
        index2int(enum2int(to)),
        index2int(rs),
        index2int(ns),
        index2int(es)
    );
