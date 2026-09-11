% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_steiner`; the registry names it for the type it takes —
% a graph — and for the tree it is. See `fzn_tree_enum.mzn` for the offset.
%
% `std` calls the total `weight_bound`, but defines it as an equality, so the
% registry names it `total_weight`.

include "fzn_graph_steiner_tree.mzn";

predicate fzn_steiner(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: weight_bound,
) =
  fzn_graph_steiner_tree(
    array1d(from),
    array1d(to),
    1,
    array1d(w),
    array1d(ns),
    array1d(es),
    weight_bound,
  );
