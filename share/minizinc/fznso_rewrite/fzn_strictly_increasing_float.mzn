% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_strictly_increasing_float`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_float_strictly_increasing.mzn";

predicate fzn_strictly_increasing_float(array [int] of var float: xs) =
  fzn_float_strictly_increasing(array1d(xs));
