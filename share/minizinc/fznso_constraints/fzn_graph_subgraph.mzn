% The constraint `graph_subgraph`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_subgraph` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

predicate fzn_graph_subgraph(
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
    forall (e in index_set(from)) ((es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]]))
  );
