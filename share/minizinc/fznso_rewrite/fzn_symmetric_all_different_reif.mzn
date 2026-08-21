% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_symmetric_all_different.mzn`, under the registry's name.

include "fzn_int_all_different_symmetric_reif.mzn";
include "all_different.mzn";
include "inverse.mzn";

predicate fzn_symmetric_all_different_reif(array [int] of var int: xs, var bool: b) =
  fzn_int_all_different_symmetric_reif(array1d(xs), min(index_set(xs)), b);
