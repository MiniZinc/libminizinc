% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_value_precede_set.mzn`, under the registry's name.

include "fzn_set_of_int_value_precede_reif.mzn";

predicate fzn_value_precede_set_reif(
  int: s,
  int: t,
  array [int] of var set of int: xs,
  var bool: b,
) =
  fzn_set_of_int_value_precede_reif(s, t, array1d(xs), b);
