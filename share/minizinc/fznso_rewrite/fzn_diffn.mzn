% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `diffn` is called `no_overlap` everywhere outside the FlatZinc builtin list,
% and pairing each coordinate with its own extent reads better than two
% coordinate arrays followed by two extent arrays. The solver-facing form takes
% `list of`, which is what states that the arrays are one-based.

include "fzn_int_no_overlap.mzn";

predicate fzn_diffn(
  array [int] of var int: x,
  array [int] of var int: y,
  array [int] of var int: dx,
  array [int] of var int: dy,
) = fzn_int_no_overlap(array1d(x), array1d(dx), array1d(y), array1d(dy));
