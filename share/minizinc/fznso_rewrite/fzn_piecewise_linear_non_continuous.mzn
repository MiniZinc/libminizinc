% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_piecewise_linear_non_continuous`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_float_piecewise_linear_non_continuous.mzn";

predicate fzn_piecewise_linear_non_continuous(
  var float: x,
  var float: y,
  array [int] of float: x_start,
  array [int] of float: x_end,
  array [int] of float: y_start,
  array [int] of float: y_end,
) =
  fzn_float_piecewise_linear_non_continuous(x, array1d(x_start), array1d(x_end), array1d(y_start), array1d(y_end), y);
