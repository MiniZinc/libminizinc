% The constraint `graph_steiner_tree_directed`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `graph_steiner_tree_directed`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

% `std` calls the total `weight_bound`, but defines it as an equality, so the
% registry names it `total_weight`.

include "fzn_graph_tree_directed.mzn";

predicate fzn_graph_steiner_tree_directed(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of int: weight,
  var int: root,
  list of var bool: nodes,
  list of var bool: edges,
  var int: total_weight,
) =
  fzn_graph_tree_directed(from, to, node_offset, root, nodes, edges) /\
    total_weight = sum (e in index_set(edges)) (edges[e] * weight[e]);
