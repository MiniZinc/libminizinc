include "fzn_reachable_int.mzn";
include "fzn_reachable_int_reif.mzn";
include "fzn_reachable_enum.mzn";
include "fzn_reachable_enum_reif.mzn";
include "fzn_dreachable_int.mzn";
include "fzn_dreachable_int_reif.mzn";
include "fzn_dreachable_enum.mzn";
include "fzn_dreachable_enum_reif.mzn";

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be reachable from \a r.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param r: the root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate dreachable(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  var int: r,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) =
  assert(index_set(from) = 1..E, "dreachable: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E, "dreachable: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N, "dreachable: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E, "dreachable: index set of es must be 1..\(E)") /\
    fzn_dreachable(N, E, from, to, r, ns, es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given directed graph to be reachable from \a r.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param r: the root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate dreachable(
  array [$$E] of $$N: from,
  array [$$E] of $$N: to,
  var $$N: r,
  array [$$N] of var bool: ns,
  array [$$E] of var bool: es,
) =
  assert(
    index_set(from) = index_set(to),
    "dreachable: index set of from and to must be identical",
  ) /\
    assert(
      index_set(from) = index_set(es),
      "dreachable: index set of from and es must be identical",
    ) /\
    assert(
      dom_array(from) subset index_set(ns),
      "dreachable: nodes in from must be in index set of ns",
    ) /\
    assert(
      dom_array(to) subset index_set(ns),
      "dreachable: nodes in to must be in index set of ns",
    ) /\
    fzn_dreachable(
      index2int(enum2int(from)),
      index2int(enum2int(to)),
      enum2int(r),
      index2int(ns),
      index2int(es),
    );

%-----------------------------------------------------------------------------%

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be reachable from \a r.

    @param N: the number of nodes in the given graph
    @param E: the number of edges in the given graph
    @param from: the leaving node 1..\a N for each edge
    @param to: the entering node 1..\a N for each edge
    @param r: the root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate reachable(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  var int: r,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) =
  assert(index_set(from) = 1..E, "reachable: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E, "reachable: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N, "reachable: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E, "reachable: index set of es must be 1..\(E)") /\
    fzn_reachable(N, E, from, to, r, ns, es);

/** @group globals.graph
    Constrains the subgraph \a ns and \a es of a given undirected graph to be reachable from \a r.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param r: the root node (which may be variable)
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
*/
predicate reachable(
  array [$$E] of $$N: from,
  array [$$E] of $$N: to,
  var $$N: r,
  array [$$N] of var bool: ns,
  array [$$E] of var bool: es,
) =
  assert(
    index_set(from) = index_set(to),
    "reachable: index set of from and to must be identical",
  ) /\
    assert(
      index_set(from) = index_set(es),
      "reachable: index set of from and es must be identical",
    ) /\
    assert(
      dom_array(from) subset index_set(ns),
      "reachable: nodes in from must be in index set of ns",
    ) /\
    assert(
      dom_array(to) subset index_set(ns),
      "reachable: nodes in to must be in index set of ns",
    ) /\
    fzn_reachable(
      index2int(enum2int(from)),
      index2int(enum2int(to)),
      enum2int(r),
      index2int(ns),
      index2int(es),
    );
