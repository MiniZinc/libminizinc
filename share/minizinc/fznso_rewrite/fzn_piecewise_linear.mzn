% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_piecewise_linear`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_float_piecewise_linear.mzn";

predicate fzn_piecewise_linear(
  var float: x,
  var float: y,
  array [int] of float: x_knot,
  array [int] of float: y_knot,
) =
  fzn_float_piecewise_linear(x, array1d(x_knot), array1d(y_knot), y);
