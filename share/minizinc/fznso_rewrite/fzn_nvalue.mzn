% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_nvalue`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_nvalue.mzn";

predicate fzn_nvalue(var int: n, array [int] of var int: xs) =
  fzn_int_nvalue(array1d(xs), n);
