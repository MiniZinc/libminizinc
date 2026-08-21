% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-based, so the numbering the successors are given in
% has to be passed explicitly rather than left implicit in an `index_set` the
% solver never receives. `list of` is what states the one-basing, and what the
% type-checker holds the caller to. The form the standard library's `circuit`
% calls forwards to it.

include "fzn_int_circuit.mzn";
include "all_different.mzn";

predicate fzn_circuit(array [int] of var int: xs) =
  fzn_int_circuit(array1d(xs), min(index_set(xs)));
