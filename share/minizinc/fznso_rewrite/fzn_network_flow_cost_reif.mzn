% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_network_flow_cost.mzn`, under the registry's name.

include "fzn_graph_network_flow_cost_reif.mzn";

predicate fzn_network_flow_cost_reif(
  array [int, 1..2] of int: arcs,
  array [int] of int: balance,
  array [int] of int: weight,
  array [int] of var int: flow,
  var int: total_cost,
  var bool: b,
) =
  fzn_graph_network_flow_cost_reif([arcs[a, 1] | a in index_set_1of2(arcs)],
    [arcs[a, 2] | a in index_set_1of2(arcs)],
    min(index_set(balance)),
    array1d(balance),
    array1d(weight),
    array1d(flow),
    total_cost, b);
