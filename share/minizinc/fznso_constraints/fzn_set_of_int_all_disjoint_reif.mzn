% The constraint `set_of_int_all_disjoint_reif`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `set_of_int_all_disjoint_reif`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

include "disjoint.mzn";
include "fzn_disjoint.mzn";

predicate fzn_set_of_int_all_disjoint_reif(list of var set of int: xs, var bool: b) =
  b <-> forall (i, j in index_set(xs) where i < j) (fzn_disjoint(xs[i], xs[j]));
