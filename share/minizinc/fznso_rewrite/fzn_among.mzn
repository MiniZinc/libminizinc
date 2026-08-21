% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_among`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_among.mzn";

predicate fzn_among(var int: n, array [int] of var int: xs, set of int: values) =
  fzn_int_among(array1d(xs), values, n);
