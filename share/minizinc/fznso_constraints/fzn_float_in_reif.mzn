% The constraint `float_in_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_in_reif` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The reified form of `fzn_float_in.mzn`.

predicate fzn_float_in_reif(var float: x, float: lower_bound, float: upper_bound, var bool: b) =
  b <-> (x >= lower_bound /\ x <= upper_bound);
