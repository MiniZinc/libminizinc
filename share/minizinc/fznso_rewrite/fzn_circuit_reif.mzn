% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_circuit.mzn`, under the registry's name.

include "fzn_int_circuit_reif.mzn";
include "all_different.mzn";

predicate fzn_circuit_reif(array [int] of var int: xs, var bool: b) =
  fzn_int_circuit_reif(array1d(xs), min(index_set(xs)), b);
