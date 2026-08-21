% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_inverse.mzn` for what the offset is: `index` names a position in the
% caller's own numbering, which the flat array cannot carry.

include "fzn_int_array_write.mzn";

predicate fzn_write(array [int] of var int: I, var int: i, var int: v, array [int] of var int: O) =
  fzn_int_array_write(array1d(I), min(index_set(I)), i, v, array1d(O));
