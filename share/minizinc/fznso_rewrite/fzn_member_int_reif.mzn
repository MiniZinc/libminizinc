% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_member_int.mzn`, under the registry's name.

include "fzn_int_array_member_reif.mzn";

predicate fzn_member_int_reif(array [int] of var int: xs, var int: value, var bool: b) =
  fzn_int_array_member_reif(array1d(xs), value, b);
