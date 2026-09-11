% The constraint `int_array_member`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_member` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_array_member(list of var int: xs, var int: value) =
  exists (i in index_set(xs)) (xs[i] == value);
