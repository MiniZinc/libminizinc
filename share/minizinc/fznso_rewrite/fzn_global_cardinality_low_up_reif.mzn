% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_global_cardinality_low_up.mzn`, under the registry's name.

include "fzn_int_global_cardinality_low_up_reif.mzn";
include "count_fn.mzn";

predicate fzn_global_cardinality_low_up_reif(
  array [int] of var int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
  var bool: b,
) =
  fzn_int_global_cardinality_low_up_reif(array1d(xs), array1d(cover), array1d(lower_bound), array1d(upper_bound), b);
