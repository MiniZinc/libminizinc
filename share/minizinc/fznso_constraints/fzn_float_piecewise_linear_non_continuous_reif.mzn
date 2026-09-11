% The constraint `float_piecewise_linear_non_continuous_reif`, as the FZnSO
% registry defines it.
%
% This file is a solver's override point: declare
% `float_piecewise_linear_non_continuous_reif` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

predicate fzn_float_piecewise_linear_non_continuous_reif(
  var float: x,
  list of float: x_start,
  list of float: x_end,
  list of float: y_start,
  list of float: y_end,
  var float: y,
  var bool: b,
) =
  abort("Reified piecewise_linear (non_continuous) constraint is not supported");
