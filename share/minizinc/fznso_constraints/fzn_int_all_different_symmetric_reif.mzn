% The constraint `int_all_different_symmetric_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `int_all_different_symmetric_reif` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

include "all_different.mzn";
include "inverse.mzn";

predicate fzn_int_all_different_symmetric_reif(
  list of var int: xs,
  int: offset,
  var bool: b,
) =
  b <-> (all_different(xs) /\ forall (i, j in index_set(xs) where i != j) (xs[i] = j -> xs[j] = i));
