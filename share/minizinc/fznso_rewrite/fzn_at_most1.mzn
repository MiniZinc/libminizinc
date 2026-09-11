% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_at_most1`, which is this constraint with the bound
% fixed at one. The registry takes the bound as a parameter and names it for the
% type it constrains.

include "fzn_set_of_int_max_overlap.mzn";

predicate fzn_at_most1(array [int] of var set of int: xs) =
  fzn_set_of_int_max_overlap(array1d(xs), 1);
