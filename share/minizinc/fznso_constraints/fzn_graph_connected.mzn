% The constraint `graph_connected`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_connected` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "reachable.mzn";

predicate fzn_graph_connected(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of var bool: nodes,
  list of var bool: edges,
) =
  let {
    set of int: NODES = node_offset..node_offset + length(nodes) - 1;
    array [NODES] of var bool: ns = array1d(NODES, nodes);
    array [1..length(edges)] of var bool: es = array1d(1..length(edges), edges);
  } in (
    let { var index_set(ns): r } in reachable(from, to, r, ns, es)
  );
