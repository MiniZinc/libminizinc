% The constraint `bool_count`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_count` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_bool_count(list of var bool: xs, var bool: value, var int: n) =
  n = sum (x_i in xs) (x_i == value);
