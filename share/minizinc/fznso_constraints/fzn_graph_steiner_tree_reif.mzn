% The constraint `graph_steiner_tree_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_steiner_tree_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

include "fzn_graph_tree.mzn";

predicate fzn_graph_steiner_tree_reif(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of int: weight,
  list of var bool: nodes,
  list of var bool: edges,
  var int: total_weight,
  var bool: b,
) =
  b <-> (
    let {
      var node_offset..node_offset + length(nodes) - 1: r;
    } in fzn_graph_tree(from, to, node_offset, r, nodes, edges) /\
      total_weight = sum (e in index_set(edges)) (edges[e] * weight[e])
  );
