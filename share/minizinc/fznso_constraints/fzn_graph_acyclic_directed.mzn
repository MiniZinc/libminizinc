% The constraint `graph_acyclic_directed`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_acyclic_directed` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

predicate fzn_graph_acyclic_directed(
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
    let {
      set of int: EDGE = index_set(es);
      array [index_set(ns)] of var 0..length(ns) - 1: dist; /* distance of longest path */
    } in forall (n in index_set(ns)) (not ns[n] -> dist[n] = 0) /\
      forall (e in EDGE) (es[e] -> dist[from[e]] + 1 <= dist[to[e]]) /\
      % redundant constraint to ensure all distances are fixed
      forall (n in index_set(ns)) (
        dist[n] = max([0] ++ [(dist[from[e]] + 1) * es[e] | e in EDGE where to[e] = n])
      )
  );
