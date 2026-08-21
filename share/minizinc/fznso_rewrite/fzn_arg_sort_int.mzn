% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

include "fzn_int_array_arg_sort.mzn";
include "all_different.mzn";

predicate fzn_arg_sort_int(
  array [int] of var int: xs,
  array [int] of var int: permutation,
) =
  fzn_int_array_arg_sort(array1d(xs), min(index_set(xs)), array1d(permutation));
