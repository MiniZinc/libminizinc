% The constraint `set_of_int_global_cardinality_low_up_closed_reif`, as the
% FZnSO registry defines it.
%
% This file is a solver's override point: declare
% `set_of_int_global_cardinality_low_up_closed_reif` and the body below is
% dropped in favour of the propagator, or ship a file of this name with a
% decomposition of your own.

include "global_cardinality.mzn";

predicate fzn_set_of_int_global_cardinality_low_up_closed_reif(
  list of var set of int: xs,
  list of int: cover,
  list of int: lower_bound,
  list of int: upper_bound,
  var bool: b,
) =
  b <-> (global_cardinality(xs, cover, lower_bound, upper_bound) /\ forall (v in xs) (v subset {d | d in cover}));
