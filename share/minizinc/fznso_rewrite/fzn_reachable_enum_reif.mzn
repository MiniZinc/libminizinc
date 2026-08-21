% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_reachable_enum.mzn`, under the registry's name.

include "fzn_graph_reachable_reif.mzn";
include "subgraph.mzn";

predicate fzn_reachable_reif(
  array [int] of int: from,
  array [int] of int: to,
  var int: root,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) =
  fzn_graph_reachable_reif(array1d(from), array1d(to), min(index_set(ns)), root, array1d(ns), array1d(es), b);
