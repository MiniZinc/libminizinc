% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_dpath_enum.mzn`, under the registry's name.

include "fzn_graph_path_directed_reif.mzn";
include "tree.mzn";
include "subgraph.mzn";

predicate fzn_dpath_reif(
  array [int] of int: from,
  array [int] of int: to,
  var int: source,
  var int: sink,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) =
  fzn_graph_path_directed_reif(array1d(from), array1d(to), min(index_set(ns)), source, sink, array1d(ns), array1d(es), b);
