% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_array_int_union`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_array_union.mzn";

predicate fzn_array_int_union(array [int] of var int: xs, var set of int: result) =
  fzn_int_array_union(array1d(xs), result);
