% The constraint `bool_lin_le_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lin_le_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% The reified form of the `bool_lin_le` builtin, which `flatzinc_builtins.mzn`
% does not declare. The fallback is the integer relation over the same
% coefficients: a Boolean linear sum is an integer one whose variables are 0/1.

predicate fzn_bool_lin_le_reif(list of int: coeffs, list of var bool: xs, int: bound, var bool: b) =
  int_lin_le_reif(coeffs, [bool2int(x) | x in xs], bound, b);
