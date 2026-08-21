% The constraint `graph_spanning_tree_directed`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `graph_spanning_tree_directed`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

% There is no `nodes` array here, so the number of nodes travels as a plain
% `int`: without it an isolated vertex would be invisible.

include "fzn_graph_tree_directed.mzn";

predicate fzn_graph_spanning_tree_directed(
  list of int: from,
  list of int: to,
  int: node_offset,
  int: node_count,
  list of int: weight,
  var int: root,
  list of var bool: edges,
  var int: total_weight,
) =
  let {
    array [1..node_count] of bool: nodes = [true | n in 1..node_count];
  } in fzn_graph_tree_directed(from, to, node_offset, root, nodes, edges) /\
    total_weight = sum (e in index_set(edges)) (edges[e] * weight[e]);
