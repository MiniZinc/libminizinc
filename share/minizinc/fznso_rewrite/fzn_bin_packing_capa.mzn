% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The bins are numbered by `capacity`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from is passed explicitly and the arrays are
% declared `list of` — one-based, as the type-checker checks.

include "fzn_int_bin_packing.mzn";

predicate fzn_bin_packing_capa(
  array [int] of int: capacity,
  array [int] of var int: bin,
  array [int] of int: weight,
) =
  fzn_int_bin_packing(array1d(bin), array1d(weight), array1d(capacity), min(index_set(capacity)));
