% The constraint `set_of_int_count`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_count` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_set_of_int_count(
  list of var set of int: xs,
  var set of int: value,
  var int: n,
) =
  n = sum (x_i in xs) (x_i == value);
