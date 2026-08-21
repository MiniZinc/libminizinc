% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_subcircuit.mzn`, under the registry's name.

include "fzn_int_subcircuit_reif.mzn";
include "alldifferent.mzn";

predicate fzn_subcircuit_reif(array [int] of var int: xs, var bool: b) =
  fzn_int_subcircuit_reif(array1d(xs), min(index_set(xs)), b);
