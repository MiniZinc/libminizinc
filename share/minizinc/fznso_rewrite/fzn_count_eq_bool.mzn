% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_count_eq_bool`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_bool_count.mzn";

predicate fzn_count_eq_bool(array [int] of var bool: xs, var bool: value, var int: n) =
  fzn_bool_count(array1d(xs), value, n);
