% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The form that states the node and edge counts. `std` asserts that it
% numbers the nodes from one, so that is the offset it passes on.

include "fzn_graph_path_directed.mzn";

predicate fzn_dpath(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  var int: source,
  var int: sink,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) =
  fzn_graph_path_directed(array1d(from), array1d(to), 1, source, sink, array1d(ns), array1d(es));
