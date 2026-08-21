% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_increasing_int`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_increasing.mzn";

predicate fzn_increasing_int(array [int] of var int: xs) =
  fzn_int_increasing(array1d(xs));
