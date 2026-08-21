% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_distribute`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_distribute.mzn";
include "count.mzn";

predicate fzn_distribute(
  array [int] of var int: count,
  array [int] of var int: cover,
  array [int] of var int: xs,
) =
  fzn_int_distribute(array1d(xs), array1d(cover), array1d(count));
