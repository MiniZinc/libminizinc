% The constraint `bool_lin_ne_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lin_ne_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% The reified form of `fzn_bool_lin_ne.mzn`.

predicate fzn_bool_lin_ne_reif(
  list of int: coeffs,
  list of var bool: xs,
  var int: total,
  var bool: b,
) = int_lin_ne_reif(coeffs ++ [-1], [bool2int(x) | x in xs] ++ [total], 0, b);
