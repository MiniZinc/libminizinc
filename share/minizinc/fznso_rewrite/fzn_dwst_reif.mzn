% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_dwst.mzn`, under the registry's name.

include "fzn_graph_spanning_tree_directed_reif.mzn";

predicate fzn_dwst_reif(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: root,
  array [int] of var bool: es,
  var int: weight_bound,
  var bool: b,
) =
  fzn_graph_spanning_tree_directed_reif(array1d(from), array1d(to), 1, N, array1d(w), root, array1d(es), weight_bound, b);
