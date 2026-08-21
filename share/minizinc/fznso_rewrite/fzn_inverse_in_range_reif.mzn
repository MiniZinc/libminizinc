% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_inverse_in_range.mzn`, under the registry's name.

include "fzn_int_inverse_in_range_reif.mzn";
include "global_cardinality.mzn";

predicate fzn_inverse_in_range_reif(
  array [int] of var int: f,
  array [int] of var int: invf,
  var bool: b,
) =
  fzn_int_inverse_in_range_reif(array1d(f), min(index_set(f)), array1d(invf), min(index_set(invf)), b);
