% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_table_int.mzn`: the tuples are passed as one flat, one-based array.

include "fzn_bool_table.mzn";

predicate fzn_table_bool(array [int] of var bool: xs, array [int, int] of bool: tuples) =
  fzn_bool_table(array1d(xs), array1d(tuples));
