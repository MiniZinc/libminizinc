% The constraint `graph_reachable_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_reachable_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "fzn_graph_reachable_directed.mzn";
include "subgraph.mzn";

predicate fzn_graph_reachable_reif(
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
    let {
      int: E = length(es);
      set of int: NODE = index_set(ns);
      array [1..2 * E] of NODE: dfrom = from ++ to;
      array [1..2 * E] of NODE: dto = to ++ from;
      array [1..2 * E] of var bool: des = es ++ es;
      array [NODE] of var bool: dns = array1d(NODE, ns);
      var NODE: dr = root;
    } in /* duplicate the edges so that we can use directed graph reachability */
    b <-> fzn_graph_reachable_directed(dfrom, dto, node_offset, dr, nodes, des)
  );
