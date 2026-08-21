% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_bin_packing_capa.mzn`, under the registry's name.

include "fzn_int_bin_packing_reif.mzn";

predicate fzn_bin_packing_capa_reif(
  array [int] of int: capacity,
  array [int] of var int: bin,
  array [int] of int: weight,
  var bool: b,
) =
  fzn_int_bin_packing_reif(array1d(bin), array1d(weight), array1d(capacity), min(index_set(capacity)), b);
