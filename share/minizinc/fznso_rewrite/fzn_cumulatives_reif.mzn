% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_cumulatives.mzn`, under the registry's name.

include "fzn_int_cumulatives_reif.mzn";

predicate fzn_cumulatives_reif(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
  int: min_machine,
  var bool: b,
) =
  fzn_int_cumulatives_reif(array1d(start),
    array1d(duration),
    array1d(resource),
    array1d(machine),
    array1d(bound),
    min_machine,
    is_upper_bound, b);
