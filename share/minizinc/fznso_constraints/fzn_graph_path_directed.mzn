% The constraint `graph_path_directed`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `graph_path_directed` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "tree.mzn";
include "subgraph.mzn";

predicate fzn_graph_path_directed(
  list of int: from,
  list of int: to,
  int: node_offset,
  var int: source,
  var int: sink,
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
      array [index_set(ns)] of var 0..length(ns) - 1: dist; /* distance from root */
    } in ns[source] /\ % source is selected
      ns[sink] /\ % dest is selected
      dist[source] = 0 /\ % distance of source is zero
      forall (
        n in index_set(ns), % nonselected nodes have distance 0
      ) (not ns[n] -> dist[n] = 0) /\
      forall (
        n in index_set(ns), % 1 incoming edge
      ) ((ns[n] /\ n != source) -> exists (e in EDGE where to[e] = n) (es[e])) /\
      forall (
        n in index_set(ns), % 1 outgoing edge
      ) ((ns[n] /\ n != sink) -> exists (e in EDGE where from[e] = n) (es[e])) /\
      forall (
        n in index_set(ns), % 1 incoming edge
      ) ((ns[n] /\ n != source) -> sum (e in EDGE where to[e] = n) (es[e]) <= 1) /\
      forall (
        n in index_set(ns), % 1 outgoing edge
      ) ((ns[n] /\ n != sink) -> sum (e in EDGE where from[e] = n) (es[e]) <= 1) /\
      % alternate for the previous 8 lines
      %   forall(n in index_set(ns))   % 1 incoming edge
      %         ((ns[n] /\ n != s) -> sum(e in EDGE where to[e] = n)(es[e]) = 1) /\
      %   forall(n in index_set(ns))   % 1 outgoing edge
      %         ((ns[n] /\ n != t) -> sum(e in EDGE where from[e] = n)(es[e]) = 1) /\
      forall (e in EDGE) (es[e] -> dist[to[e]] = dist[from[e]] + 1) /\
      sum (n in index_set(ns)) (ns[n]) = sum (e in EDGE) (es[e]) + 1 /\
      subgraph(from, to, ns, es)
  );
