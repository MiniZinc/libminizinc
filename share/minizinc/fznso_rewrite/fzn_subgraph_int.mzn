% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The form that states the node and edge counts. `std` asserts that it
% numbers the nodes from one, so that is the offset it passes on.

include "fzn_graph_subgraph.mzn";

predicate fzn_subgraph(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) =
  fzn_graph_subgraph(array1d(from), array1d(to), 1, array1d(ns), array1d(es));
