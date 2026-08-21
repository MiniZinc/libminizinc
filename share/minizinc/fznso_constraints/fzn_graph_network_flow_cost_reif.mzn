% The constraint `graph_network_flow_cost_reif`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `graph_network_flow_cost_reif`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

include "fzn_graph_network_flow.mzn";

predicate fzn_graph_network_flow_cost_reif(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of int: balance,
  list of int: weight,
  list of var int: flow,
  var int: total_cost,
  var bool: b,
) =
  b <-> (
    fzn_graph_network_flow(from, to, node_offset, balance, flow) /\
      total_cost = sum (i in index_set(flow)) (flow[i] * weight[i])
  );
