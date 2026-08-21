% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_write.mzn`. Two writes to one position have to agree here, so this
% can fail; `int_array_writes_sequential` lets the later one win instead, which
% is why the two are separate constraints and only that one is functional.

include "fzn_int_array_writes.mzn";

predicate fzn_writes(
  array [int] of var int: I,
  array [int] of var int: P,
  array [int] of var int: V,
  array [int] of var int: O,
) = fzn_int_array_writes(array1d(I), min(index_set(I)), array1d(P), array1d(V), array1d(O));
