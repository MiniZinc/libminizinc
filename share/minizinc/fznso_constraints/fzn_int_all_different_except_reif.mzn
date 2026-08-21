% The constraint `int_all_different_except_reif`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `int_all_different_except_reif` and the body below is dropped in favour of the
% propagator, or ship a file of this name with a decomposition of your own.

include "global_cardinality_low_up.mzn";

predicate fzn_int_all_different_except_reif(
  list of var int: xs,
  set of int: except,
  var bool: b,
) =
  b <-> forall (i, j in index_set(xs) where i < j) ((xs[i] in except /\ xs[j] in except) \/ xs[i] != xs[j]);
