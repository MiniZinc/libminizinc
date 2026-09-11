% The constraint `int_global_cardinality_low_up_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `int_global_cardinality_low_up_reif` and the body below is dropped in favour
% of the propagator, or ship a file of this name with a decomposition of your
% own.

include "count_fn.mzn";

predicate fzn_int_global_cardinality_low_up_reif(
  list of var int: xs,
  list of int: cover,
  list of int: lower_bound,
  list of int: upper_bound,
  var bool: b,
) =
  b <-> forall (i in index_set(cover)) (count(xs, cover[i]) in lower_bound[i]..upper_bound[i]);
