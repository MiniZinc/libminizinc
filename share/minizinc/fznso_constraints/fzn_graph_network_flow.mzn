% The constraint `graph_network_flow`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_network_flow` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% `std` passes the arcs as an n-by-2 matrix; a FlatZinc array is one-dimensional,
% so the registry gives the two columns a name each, as the other graph
% constraints do. See `fzn_subgraph_enum.mzn` for the node offset.

predicate fzn_graph_network_flow(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of int: balance,
  list of var int: flow,
) =
  forall (i in node_offset..node_offset + length(balance) - 1) (
    sum (j in index_set(from) where i = from[j]) (flow[j]) -
      sum (j in index_set(to) where i = to[j]) (flow[j]) =
      balance[i - node_offset + 1]
  );
