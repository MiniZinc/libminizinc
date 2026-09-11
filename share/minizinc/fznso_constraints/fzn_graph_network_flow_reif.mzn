% The constraint `graph_network_flow_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_network_flow_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_graph_network_flow_reif(
  list of int: from,
  list of int: to,
  int: node_offset,
  list of int: balance,
  list of var int: flow,
  var bool: b,
) =
  b <-> (
    forall (i in node_offset..node_offset + length(balance) - 1) (
      sum (j in index_set(from) where i = from[j]) (flow[j]) -
        sum (j in index_set(to) where i = to[j]) (flow[j]) =
        balance[i - node_offset + 1]
    )
  );
