% The constraint `graph_subgraph_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_subgraph_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_graph_subgraph_reif(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of var bool: nodes,
  list of var bool: edges,
  var bool: b,
) =
  let {
    set of int: NODES = node_offset..node_offset + length(nodes) - 1;
    array [NODES] of var bool: ns = array1d(NODES, nodes);
    array [1..length(edges)] of var bool: es = array1d(1..length(edges), edges);
  } in (
    b <-> forall (e in index_set(from)) ((es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]]))
  );
