% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_alldifferent_except`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_all_different_except.mzn";
include "global_cardinality_low_up.mzn";

predicate fzn_alldifferent_except(array [int] of var int: xs, set of int: except) =
  fzn_int_all_different_except(array1d(xs), except);
