% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_piecewise_linear.mzn`, under the registry's name.

include "fzn_float_piecewise_linear_reif.mzn";

predicate fzn_piecewise_linear_reif(
  var float: x,
  var float: y,
  array [int] of float: x_knot,
  array [int] of float: y_knot,
  var bool: b,
) =
  fzn_float_piecewise_linear_reif(x, array1d(x_knot), array1d(y_knot), y, b);
