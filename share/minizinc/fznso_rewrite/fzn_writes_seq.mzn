% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_writes.mzn`. Here the last write to a position wins, so the result is
% always determined and the constraint is functional.

include "fzn_int_array_writes_sequential.mzn";
include "arg_max.mzn";

predicate fzn_writes_seq(
  array [int] of var int: I,
  array [int] of var int: P,
  array [int] of var int: V,
  array [int] of var int: O,
) =
  fzn_int_array_writes_sequential(
    array1d(I),
    min(index_set(I)),
    array1d(P),
    array1d(V),
    array1d(O),
  );
