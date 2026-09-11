% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_global_cardinality_closed_set.mzn`, under the registry's name.

include "fzn_set_of_int_global_cardinality_closed_reif.mzn";
include "global_cardinality.mzn";

predicate fzn_global_cardinality_closed_set_reif(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
  var bool: b,
) =
  fzn_set_of_int_global_cardinality_closed_reif(array1d(xs), array1d(cover), array1d(count), b);
