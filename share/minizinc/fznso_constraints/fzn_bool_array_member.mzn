% The constraint `bool_array_member`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_array_member` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_bool_array_member(list of var bool: xs, var bool: value) =
  exists (i in index_set(xs)) (xs[i] == value);
