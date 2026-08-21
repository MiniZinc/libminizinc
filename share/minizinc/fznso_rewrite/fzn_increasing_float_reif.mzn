% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_increasing_float.mzn`, under the registry's name.

include "fzn_float_increasing_reif.mzn";

predicate fzn_increasing_float_reif(array [int] of var float: xs, var bool: b) =
  fzn_float_increasing_reif(array1d(xs), b);
