% The constraint `graph_reachable_directed_reif`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `graph_reachable_directed_reif` and the body below is dropped in favour of the
% propagator, or ship a file of this name with a decomposition of your own.

include "subgraph.mzn";

predicate fzn_graph_reachable_directed_reif(
  list of int: from,
  list of int: to,
  int: node_offset,
  var int: root,
  list of var bool: nodes,
  list of var bool: edges,
  var bool: b,
) =
  let {
    set of int: NODES = node_offset..node_offset + length(nodes) - 1;
    array [NODES] of var bool: ns = array1d(NODES, nodes);
    array [1..length(edges)] of var bool: es = array1d(1..length(edges), edges);
  } in (
    abort("Reified dreachable constraint is not supported")
  );
