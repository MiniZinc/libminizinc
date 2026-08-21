% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_writes.mzn`, under the registry's name.

include "fzn_int_array_writes_reif.mzn";

predicate fzn_writes_reif(
  array [int] of var int: I,
  array [int] of var int: P,
  array [int] of var int: V,
  array [int] of var int: O,
  var bool: b,
) =
  fzn_int_array_writes_reif(array1d(I), min(index_set(I)), array1d(P), array1d(V), array1d(O), b);
