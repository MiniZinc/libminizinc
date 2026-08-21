% The constraint `set_of_int_max_overlap_reif`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `set_of_int_max_overlap_reif`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

predicate fzn_set_of_int_max_overlap_reif(
  list of var set of int: xs,
  int: max_overlap,
  var bool: b,
) =
  b <-> forall (i, j in index_set(xs) where i < j) (card(xs[i] intersect xs[j]) <= 1);
