% The constraint `graph_spanning_tree`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_spanning_tree` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% There is no `nodes` array here, so the number of nodes travels as a plain
% `int`: without it an isolated vertex would be invisible.

include "fzn_graph_tree.mzn";

predicate fzn_graph_spanning_tree(
  list of int: from,
  list of int: to,
  int: node_offset,
  int: node_count,
  list of int: weight,
  list of var bool: edges,
  var int: total_weight,
) =
  let {
    var node_offset..node_offset + node_count - 1: r;
    array [1..node_count] of bool: nodes = [true | n in 1..node_count];
  } in fzn_graph_tree(from, to, node_offset, r, nodes, edges) /\
    total_weight = sum (e in index_set(edges)) (edges[e] * weight[e]);
