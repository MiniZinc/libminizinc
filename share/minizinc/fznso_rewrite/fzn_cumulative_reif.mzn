% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_cumulative.mzn`, under the registry's name.

include "fzn_int_cumulative_reif.mzn";

predicate fzn_cumulative_reif(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  var int: capacity,
  var bool: b,
) =
  fzn_int_cumulative_reif(array1d(start), array1d(duration), array1d(resource), capacity, b);
