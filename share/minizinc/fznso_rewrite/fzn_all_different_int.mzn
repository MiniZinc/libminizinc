% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_all_different_int`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_all_different.mzn";

predicate fzn_all_different_int(array [int] of var int: xs) =
  fzn_int_all_different(array1d(xs));
