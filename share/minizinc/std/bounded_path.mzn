/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a path from \a s to \a t of weight \a K.

   @param N: the number of nodes in the given graph
   @param E: the number of edges in the given graph
   @param from: the leaving node 1..\a N for each edge
   @param to: the entering node 1..\a N for each edge
   @param w: the weight of each edge
   @param s: the source node (which may be variable)
   @param t: the dest node (which may be variable)
   @param ns: a Boolean for each node whether it is in the subgraph
   @param es: a Boolean for each edge whether it is in the subgraph
   @param K: the cost of the path
*/
predicate bounded_dpath(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: s,
  var int: t,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
) =
  assert(index_set(from) = 1..E, "bounded_dpath: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E, "bounded_dpath: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N, "bounded_dpath: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E, "bounded_dpath: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E, "bounded_dpath: index set of w must be 1..\(E)") /\
    fzn_bounded_dpath(N, E, from, to, w, s, t, ns, es, K);

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given directed graph to be a path from \a s to \a t of weight \a K.

   @param from: the leaving node for each edge
   @param to: the entering node for each edge
   @param w: the weight of each edge
   @param s: the source node (which may be variable)
   @param t: the dest node (which may be variable)
   @param ns: a Boolean for each node whether it is in the subgraph
   @param es: a Boolean for each edge whether it is in the subgraph
   @param K: the cost of the path
*/
predicate bounded_dpath(
  array [int] of $$N: from,
  array [int] of $$N: to,
  array [int] of int: w,
  var $$N: s,
  var $$N: t,
  array [$$N] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
) =
  assert(
    index_set(from) = index_set(to),
    "bounded_dpath: index set of from and to must be identical",
  ) /\
    assert(
      index_set(from) = index_set(es),
      "bounded_dpath: index set of from and es must be identical",
    ) /\
    assert(
      index_set(w) = index_set(es),
      "bounded_dpath: index set of w and es must be identical",
    ) /\
    assert(
      dom_array(from) subset index_set(ns),
      "bounded_dpath: nodes in from must be in index set of ns",
    ) /\
    assert(
      dom_array(to) subset index_set(ns),
      "bounded_dpath: nodes in to must be in index set of ns",
    ) /\
    fzn_bounded_dpath(
      enum2int(from),
      enum2int(to),
      w,
      enum2int(s),
      enum2int(t),
      index2int(ns),
      es,
      K,
    );

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be a path from \a s to \a t of weight \a K.

   @param N: the number of nodes in the given graph
   @param E: the number of edges in the given graph
   @param from: the leaving node 1..\a N for each edge
   @param to: the entering node 1..\a N for each edge
   @param w: the weight of each edge
   @param s: the source node (which may be variable)
   @param t: the dest node (which may be variable)
   @param ns: a Boolean for each node whether it is in the subgraph
   @param es: a Boolean for each edge whether it is in the subgraph
   @param K: the cost of the path
*/
predicate bounded_path(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: s,
  var int: t,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
) =
  assert(index_set(from) = 1..E, "bounded_path: index set of from must be 1..\(E)") /\
    assert(index_set(to) = 1..E, "bounded_path: index set of to must be 1..\(E)") /\
    assert(index_set(ns) = 1..N, "bounded_path: index set of ns must be 1..\(N)") /\
    assert(index_set(es) = 1..E, "bounded_path: index set of es must be 1..\(E)") /\
    assert(index_set(w) = 1..E, "bounded_path: index set of w must be 1..\(E)") /\
    fzn_bounded_path(N, E, from, to, w, s, t, ns, es, K);

/** @group globals.graph
   Constrains the subgraph \a ns and \a es of a given undirected graph to be a path from \a s to \a t of weight \a K.

   @param from: the leaving node for each edge
   @param to: the entering node for each edge
   @param w: the weight of each edge
   @param s: the source node (which may be variable)
   @param t: the dest node (which may be variable)
   @param ns: a Boolean for each node whether it is in the subgraph
   @param es: a Boolean for each edge whether it is in the subgraph
   @param K: the cost of the path
*/
predicate bounded_path(
  array [int] of $$N: from,
  array [int] of $$N: to,
  array [int] of int: w,
  var $$N: s,
  var $$N: t,
  array [$$N] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
) =
  assert(
    index_set(from) = index_set(to),
    "bounded_path: index set of from and to must be identical",
  ) /\
    assert(
      index_set(from) = index_set(es),
      "bounded_path: index set of from and es must be identical",
    ) /\
    assert(index_set(w) = index_set(es), "bounded_path: index set of w and es must be identical") /\
    assert(
      dom_array(from) subset index_set(ns),
      "bounded_path: nodes in from must be in index set of ns",
    ) /\
    assert(
      dom_array(to) subset index_set(ns),
      "bounded_path: nodes in to must be in index set of ns",
    ) /\
    fzn_bounded_path(
      enum2int(from),
      enum2int(to),
      w,
      enum2int(s),
      enum2int(t),
      index2int(ns),
      es,
      K,
    );

include "fzn_bounded_path_int.mzn";
include "fzn_bounded_path_int_reif.mzn";
include "fzn_bounded_path_enum.mzn";
include "fzn_bounded_path_enum_reif.mzn";
include "fzn_bounded_dpath_int.mzn";
include "fzn_bounded_dpath_int_reif.mzn";
include "fzn_bounded_dpath_enum.mzn";
include "fzn_bounded_dpath_enum_reif.mzn";

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
