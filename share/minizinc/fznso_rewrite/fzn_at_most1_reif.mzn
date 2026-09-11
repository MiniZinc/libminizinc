% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_at_most1.mzn`, under the registry's name.

include "fzn_set_of_int_max_overlap_reif.mzn";

predicate fzn_at_most1_reif(array [int] of var set of int: xs, var bool: b) =
  fzn_set_of_int_max_overlap_reif(array1d(xs), 1, b);
