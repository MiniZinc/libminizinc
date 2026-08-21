% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_disjunctive_strict.mzn`, under the registry's name.

include "fzn_int_disjunctive_strict_reif.mzn";

predicate fzn_disjunctive_strict_reif(
  array [int] of var int: start,
  array [int] of var int: duration,
  var bool: b,
) =
  fzn_int_disjunctive_strict_reif(array1d(start), array1d(duration), b);
