% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_global_cardinality_set`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_set_of_int_global_cardinality.mzn";

predicate fzn_global_cardinality_set(
  array [int] of var set of int: xs,
  array [int] of int: cover,
  array [int] of var int: count,
) =
  fzn_set_of_int_global_cardinality(array1d(xs), array1d(cover), array1d(count));
