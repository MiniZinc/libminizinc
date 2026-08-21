% The constraint `float_strictly_increasing_reif`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `float_strictly_increasing_reif` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

predicate fzn_float_strictly_increasing_reif(list of var float: xs, var bool: b) =
  b <-> forall (i in index_set(xs) diff {min(index_set(xs))}) (xs[i - 1] < xs[i]);
