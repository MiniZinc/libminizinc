% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` passes the arcs as an n-by-2 matrix; a FlatZinc array is one-dimensional,
% so the registry gives the two columns a name each, as the other graph
% constraints do. See `fzn_subgraph_enum.mzn` for the node offset.

include "fzn_graph_network_flow.mzn";

predicate fzn_network_flow(
  array [int, 1..2] of int: arcs,
  array [int] of int: balance,
  array [int] of var int: flow,
) =
  fzn_graph_network_flow(
    [arcs[a, 1] | a in index_set_1of2(arcs)],
    [arcs[a, 2] | a in index_set_1of2(arcs)],
    min(index_set(balance)),
    array1d(balance),
    array1d(flow),
  );
