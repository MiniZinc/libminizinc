% The constraint `graph_tree`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_tree` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "fzn_graph_tree_directed.mzn";
predicate fzn_graph_tree(
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
      array [1..2 * E] of int: dfrom = from ++ to;
      array [1..2 * E] of int: dto = to ++ from;
      array [1..2 * E] of var bool: des;
    } in /* ensure that the directed edges selected agree with undirected edges */
    forall (e in 1..E) (es[e - 1 + min(index_set(es))] <-> (des[e] \/ des[e + E])) /\
      /* duplicate the edges so that the we can use directed graph reachability */
      fzn_graph_tree_directed(dfrom, dto, node_offset, root, nodes, des)
  );
