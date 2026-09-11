% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_table_bool.mzn`: the tuples are passed as one flat, one-based array.

include "fzn_bool_table_reif.mzn";

predicate fzn_table_bool_reif(
  array [int] of var bool: xs,
  array [int, int] of bool: tuples,
  var bool: b,
) = fzn_bool_table_reif(array1d(xs), array1d(tuples), b);
