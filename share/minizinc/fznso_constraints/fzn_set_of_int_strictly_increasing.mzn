% The constraint `set_of_int_strictly_increasing`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `set_of_int_strictly_increasing` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

predicate fzn_set_of_int_strictly_increasing(list of var set of int: xs) =
  forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] < xs[i]);
