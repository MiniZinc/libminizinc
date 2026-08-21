% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_member_set.mzn`, under the registry's name.

include "fzn_set_of_int_array_member_reif.mzn";

predicate fzn_member_set_reif(
  array [int] of var set of int: xs,
  var set of int: value,
  var bool: b,
) =
  fzn_set_of_int_array_member_reif(array1d(xs), value, b);
