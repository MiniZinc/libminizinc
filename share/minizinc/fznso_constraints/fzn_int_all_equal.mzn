% The constraint `int_all_equal`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_all_equal` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_all_equal(list of var int: xs) =
  forall (i, j in index_set(xs) where i < j) (xs[i] = xs[j]);
