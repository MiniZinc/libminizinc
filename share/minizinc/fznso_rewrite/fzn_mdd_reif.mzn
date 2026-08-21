% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_mdd.mzn`, under the registry's name.

include "fzn_int_mdd_reif.mzn";

predicate fzn_mdd_reif(
  array [int] of var int: xs,
  int: nodes,
  array [int] of int: level,
  int: edges,
  array [int] of int: from,
  array [int] of set of int: label,
  array [int] of int: to,
  var bool: b,
) =
  fzn_int_mdd_reif(array1d(xs), nodes, array1d(level), edges, array1d(from), array1d(label), array1d(to), b);
