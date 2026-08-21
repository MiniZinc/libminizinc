% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-dimensional and one-based, so the tuples are passed
% flat, laid out one tuple after another, and both arrays are declared `list of`
% — which is what says one-based, and what the type-checker holds the caller to.
% The two-dimensional form the standard library's `table` calls forwards to it.

include "fzn_int_table.mzn";

predicate fzn_table_int(array [int] of var int: xs, array [int, int] of int: tuples) =
  fzn_int_table(array1d(xs), array1d(tuples));
