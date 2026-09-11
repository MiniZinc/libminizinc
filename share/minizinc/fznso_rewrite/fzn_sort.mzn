% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_sort`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_array_sort.mzn";
include "alldifferent.mzn";
include "increasing.mzn";

predicate fzn_sort(array [int] of var int: xs, array [int] of var int: ys) =
  fzn_int_array_sort(array1d(xs), array1d(ys));
