include "fzn_cluster_int.mzn";
include "fzn_cluster_int_reif.mzn";
include "fzn_cluster_enum.mzn";
include "fzn_cluster_enum_reif.mzn";

/** @group globals.graph
    Clusterize the subgraph \a ns and \a es of a given undirected graph into disjoint subgraphs,
    and produce a \a r node reachability matrix.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param r: node reachability matrix
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate cluster(int: N, int: E, array[int] of int: from, array[int] of int: to,
                  array[int,int] of var bool: r,
                  array[int] of var bool: ns, array[int] of var bool: es) =
    assert(index_set(from) = 1..E,"cluster: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E,"cluster: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N,"cluster: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E,"cluster: index set of es must be 1..\(E)") /\
    fzn_cluster(N,E,from,to,r,ns,es);

/** @group globals.graph
    Clusterize the subgraph \a ns and \a es of a given undirected graph into disjoint subgraphs,
    and produce a \a r node reachability matrix.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param r: node reachability matrix
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate cluster(array[$$E] of $$N: from, array[$$E] of $$N: to,
                  array[$$N,$$N] of var bool: r,
                  array[$$N] of var bool: ns, array[$$E] of var bool: es) =
    let {
        int: NUM_NODES = card(index_set(ns));
    } in
    assert(index_set(from) = index_set(to),"cluster: index set of from and to must be identical") /\
    assert(index_set(from) = index_set(es),"cluster: index set of from and es must be identical") /\
    assert(index_set_1of2(r) = index_set_2of2(r),"cluster: nodes in from must be in index set of r") /\
    assert(dom_array(from) subset index_set_1of2(r),"cluster: nodes in from must be in index set of r") /\
    assert(dom_array(to) subset index_set_1of2(r),"cluster: nodes in to must be in index set of r") /\
    assert(dom_array(from) subset index_set(ns),"cluster: nodes in from must be in index set of ns") /\
    assert(dom_array(to) subset index_set(ns),"cluster: nodes in to must be in index set of ns") /\
    fzn_cluster(
        index2int(enum2int(from)),
        index2int(enum2int(to)),
        index2int(r),
        index2int(ns),
        index2int(es)
    );
