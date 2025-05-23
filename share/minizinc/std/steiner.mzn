include "fzn_steiner.mzn";
include "fzn_steiner_reif.mzn";
include "fzn_dsteiner.mzn";
include "fzn_dsteiner_reif.mzn";
include "weighted_spanning_tree.mzn";

/** @group globals.graph
    Constrains the subgraph \a  ns and \a es of a given directed graph to be a weighted spanning tree rooted at \a r of weight \a W.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param w: the weight of each edge
    @param r: the root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
    @param K: the weight of the tree
*/
predicate dsteiner(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: r,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
) =
  assert(index_set(from) = 1..E, "dsteiner: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E, "dsteiner: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N, "dsteiner: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E, "dsteiner: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E, "dsteiner: index set of w must be 1..\(E)") /\
    if forall (n in 1..N) (is_fixed(ns[n]) /\ fix(ns[n])) then
      d_weighted_spanning_tree(N, E, from, to, w, r, es, K)
    else
      fzn_dsteiner(N, E, from, to, w, r, ns, es, K)
    endif;

/** @group globals.graph
    Constrains the set of edges \a es of a given undirected graph to be a weighted spanning tree of weight \a W.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param w: the weight of each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
    @param K: the weight of the tree
**/
predicate steiner(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
) =
  assert(index_set(from) = 1..E, "steiner: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E, "steiner: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N, "steiner: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E, "steiner: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E, "steiner: index set of w must be 1..\(E)") /\
    if forall (n in 1..N) (is_fixed(ns[n]) /\ fix(ns[n])) then
      weighted_spanning_tree(N, E, from, to, w, es, K)
    else
      fzn_steiner(N, E, from, to, w, ns, es, K)
    endif;

%-----------------------------------------------------------------------------%
