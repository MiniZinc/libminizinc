% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_increasing_bool.mzn`, under the registry's name.

include "fzn_bool_increasing_reif.mzn";

predicate fzn_increasing_bool_reif(array [int] of var bool: xs, var bool: b) =
  fzn_bool_increasing_reif(array1d(xs), b);
