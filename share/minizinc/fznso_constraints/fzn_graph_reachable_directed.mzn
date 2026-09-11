% The constraint `graph_reachable_directed`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_reachable_directed` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "subgraph.mzn";

predicate fzn_graph_reachable_directed(
  list of int: from,
  list of int: to,
  int: node_offset,
  var int: root,
  list of var bool: nodes,
  list of var bool: edges,
) =
  let {
    set of int: NODES = node_offset..node_offset + length(nodes) - 1;
    array [NODES] of var bool: ns = array1d(NODES, nodes);
    array [1..length(edges)] of var bool: es = array1d(1..length(edges), edges);
  } in (
    let {
      array [index_set(ns)] of var 0..card(index_set(ns)) - 1: dist; /* distance from root */
      array [index_set(ns)] of var index_set(ns): parent; /* parent */
    } in ns[root] /\ % the root must be chosen
      dist[root] = 0 /\ % root is at distance 0
      parent[root] = root /\ % root is its own parent
      forall (
        n in index_set(ns), % nonselected nodes have parent 0
      ) (not ns[n] -> parent[n] = n) /\
      forall (
        n in index_set(ns), % nonselected nodes have distance 0
      ) (not ns[n] -> dist[n] = 0) /\
      forall (
        n in index_set(ns), % each in node except root must have a parent
      ) (ns[n] -> (n = root \/ parent[n] != n)) /\
      forall (
        n in index_set(ns), % each in node with a parent must be in and also its parent
      ) (parent[n] != n -> (ns[n] /\ ns[parent[n]])) /\
      forall (
        n in index_set(ns), % each except with a parent is one more than its parent
      ) (parent[n] != n -> dist[n] = dist[parent[n]] + 1) /\
      forall (
        n in index_set(ns), % each node with a parent must have that edge in
      ) (
        parent[n] != n -> exists (e in index_set(from) where to[e] = n) (es[e] /\ from[e] = parent[n])
      ) /\
      subgraph(from, to, ns, es)
  );
