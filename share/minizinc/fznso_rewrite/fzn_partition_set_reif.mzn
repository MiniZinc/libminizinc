% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_partition_set.mzn`, under the registry's name.

include "fzn_set_of_int_partition_reif.mzn";
include "all_disjoint.mzn";

predicate fzn_partition_set_reif(
  array [int] of var set of int: xs,
  set of int: universe,
  var bool: b,
) =
  fzn_set_of_int_partition_reif(array1d(xs), universe, b);
