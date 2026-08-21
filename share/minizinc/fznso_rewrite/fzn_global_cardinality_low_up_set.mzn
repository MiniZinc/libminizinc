% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_global_cardinality_low_up_set`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_set_of_int_global_cardinality_low_up.mzn";

predicate fzn_global_cardinality_low_up_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of int: lower_bound,
  array [int] of int: upper_bound,
) =
  fzn_set_of_int_global_cardinality_low_up(array1d(xs), array1d(cover), array1d(lower_bound), array1d(upper_bound));
