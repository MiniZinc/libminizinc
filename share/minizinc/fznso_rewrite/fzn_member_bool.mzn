% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_member_bool`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_bool_array_member.mzn";

predicate fzn_member_bool(array [int] of var bool: xs, var bool: value) =
  fzn_bool_array_member(array1d(xs), value);
