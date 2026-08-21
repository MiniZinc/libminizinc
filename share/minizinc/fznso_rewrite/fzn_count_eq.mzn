% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_count_eq`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_count.mzn";

predicate fzn_count_eq(array [int] of var int: xs, var int: value, var int: n) =
  fzn_int_count(array1d(xs), value, n);
