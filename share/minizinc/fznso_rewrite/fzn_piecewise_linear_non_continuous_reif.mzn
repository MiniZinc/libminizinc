% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_piecewise_linear_non_continuous.mzn`, under the registry's name.

include "fzn_float_piecewise_linear_non_continuous_reif.mzn";

predicate fzn_piecewise_linear_non_continuous_reif(
  var float: x,
  var float: y,
  array [int] of float: x_start,
  array [int] of float: x_end,
  array [int] of float: y_start,
  array [int] of float: y_end,
  var bool: b,
) =
  fzn_float_piecewise_linear_non_continuous_reif(x, array1d(x_start), array1d(x_end), array1d(y_start), array1d(y_end), y, b);
