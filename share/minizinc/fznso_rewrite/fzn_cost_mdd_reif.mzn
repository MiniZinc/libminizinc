% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_cost_mdd.mzn`, under the registry's name.

include "fzn_int_mdd_cost_reif.mzn";

predicate fzn_cost_mdd_reif(
  array [int] of var int: xs,
  int: nodes,
  array [int] of int: level,
  int: edges,
  array [int] of int: from,
  array [int] of set of int: label,
  array [int] of int: weight,
  array [int] of int: to,
  var int: total_cost,
  var bool: b,
) =
  fzn_int_mdd_cost_reif(array1d(xs), nodes, array1d(level), edges, array1d(from), array1d(label),
    array1d(weight), array1d(to), total_cost, b);
