% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The bins are numbered by `load`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from is passed explicitly and the arrays
% are declared `list of` — one-based, as the type-checker checks.

include "fzn_int_bin_packing_load.mzn";

predicate fzn_bin_packing_load(
  array [int] of var int: load,
  array [int] of var int: bin,
  array [int] of int: weight,
) = fzn_int_bin_packing_load(array1d(bin), array1d(weight), array1d(load), min(index_set(load)));
