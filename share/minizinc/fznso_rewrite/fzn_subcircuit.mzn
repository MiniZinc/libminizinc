% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% A FlatZinc array is one-based, so the numbering the successors are given in
% has to be passed explicitly rather than left implicit in an `index_set` the
% solver never receives.

include "fzn_int_subcircuit.mzn";
include "alldifferent.mzn";

predicate fzn_subcircuit(array [int] of var int: xs) =
  fzn_int_subcircuit(array1d(xs), min(index_set(xs)));
