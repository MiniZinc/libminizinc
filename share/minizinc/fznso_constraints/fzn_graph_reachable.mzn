% The constraint `graph_reachable`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_reachable` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "fzn_graph_reachable_directed.mzn";
include "subgraph.mzn";

predicate fzn_graph_reachable(
  list of int: from,
  list of int: to,
  int: node_offset,
  var int: root,
  list of var bool: nodes,
  list of var bool: edges,
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
    fzn_graph_reachable_directed(dfrom, dto, node_offset, dr, nodes, des)
  );
