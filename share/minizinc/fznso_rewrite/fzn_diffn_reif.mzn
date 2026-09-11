% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_diffn.mzn`, under the registry's name.

include "fzn_int_no_overlap_reif.mzn";

predicate fzn_diffn_reif(
  array [int] of var int: x,
  array [int] of var int: y,
  array [int] of var int: dx,
  array [int] of var int: dy,
  var bool: b,
) =
  fzn_int_no_overlap_reif(array1d(x), array1d(dx), array1d(y), array1d(dy), b);
