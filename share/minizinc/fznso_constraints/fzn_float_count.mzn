% The constraint `float_count`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_count` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_float_count(list of var float: xs, var float: value, var int: n) =
  n = sum (x_i in xs) (x_i == value);
