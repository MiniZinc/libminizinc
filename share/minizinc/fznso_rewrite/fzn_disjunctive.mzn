% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_disjunctive`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_disjunctive.mzn";

predicate fzn_disjunctive(
  array [int] of var int: start,
  array [int] of var int: duration,
) =
  fzn_int_disjunctive(array1d(start), array1d(duration));
