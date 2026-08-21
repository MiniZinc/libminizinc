% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

include "fzn_int_array_arg_max.mzn";

predicate fzn_maximum_arg_int(array [int] of var int: xs, var int: index) =
  fzn_int_array_arg_max(array1d(xs), min(index_set(xs)), index);
