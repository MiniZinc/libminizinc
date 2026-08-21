% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_diffn.mzn`. This is the form in which a box of zero extent overlaps
% nothing, so it is a constraint of its own rather than a special case.

include "fzn_int_no_overlap_nonstrict.mzn";

predicate fzn_diffn_nonstrict(
  array [int] of var int: x,
  array [int] of var int: y,
  array [int] of var int: dx,
  array [int] of var int: dy,
) =
  fzn_int_no_overlap_nonstrict(array1d(x), array1d(dx), array1d(y), array1d(dy));
