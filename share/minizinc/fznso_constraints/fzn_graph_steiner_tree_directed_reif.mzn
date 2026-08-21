% The constraint `graph_steiner_tree_directed_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `graph_steiner_tree_directed_reif` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

include "fzn_graph_tree_directed.mzn";

predicate fzn_graph_steiner_tree_directed_reif(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of int: weight,
  var int: root,
  list of var bool: nodes,
  list of var bool: edges,
  var int: total_weight,
  var bool: b,
) =
  b <-> (
    fzn_graph_tree_directed(from, to, node_offset, root, nodes, edges) /\
      total_weight = sum (e in index_set(edges)) (edges[e] * weight[e])
  );
