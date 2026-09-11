% The constraint `graph_tree_directed`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_tree_directed` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "subgraph.mzn";

predicate fzn_graph_tree_directed(
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
      set of int: EDGE = index_set(es);
      array [index_set(ns)] of var 0..length(ns) - 1: dist; /* distance from root */
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
      ) (parent[n] != n -> exists (e in EDGE where to[e] = n) (es[e] /\ from[e] = parent[n])) /\
      forall (
        e in EDGE, % each edge must be part of the parent relation
      ) (es[e] -> parent[to[e]] = from[e]) /\
      sum (e in EDGE) (es[e]) = sum (n in index_set(ns)) (ns[n]) - 1 /\ % redundant relationship of trees
      subgraph(from, to, ns, es)
  );
