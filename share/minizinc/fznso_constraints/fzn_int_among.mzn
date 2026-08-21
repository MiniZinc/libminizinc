% The constraint `int_among`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_among` and the body below
% is dropped in favour of the propagator, or ship a file of this name with a
% decomposition of your own.

predicate fzn_int_among(list of var int: xs, set of int: values, var int: n) =
  n == sum (i in index_set(xs)) (xs[i] in values);
