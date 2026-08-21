% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

include "fzn_float_array_arg_min.mzn";

predicate fzn_minimum_arg_float(array [int] of var float: xs, var int: index) =
  fzn_float_array_arg_min(array1d(xs), min(index_set(xs)), index);
