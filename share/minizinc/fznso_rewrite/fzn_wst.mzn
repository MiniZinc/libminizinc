% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_wst`; the registry names it for the type it takes —
% a graph — and for the tree it is. See `fzn_tree_enum.mzn` for the offset.
%
% There is no `nodes` array here, so the number of nodes travels as a plain
% `int`: without it an isolated vertex would be invisible.

include "fzn_graph_spanning_tree.mzn";

predicate fzn_wst(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  array [int] of var bool: es,
  var int: weight_bound,
) =
  fzn_graph_spanning_tree(
    array1d(from),
    array1d(to),
    1,
    N,
    array1d(w),
    array1d(es),
    weight_bound,
  );
