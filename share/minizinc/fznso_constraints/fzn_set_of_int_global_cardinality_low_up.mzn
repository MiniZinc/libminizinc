% The constraint `set_of_int_global_cardinality_low_up`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `set_of_int_global_cardinality_low_up` and the body below is dropped in favour
% of the propagator, or ship a file of this name with a decomposition of your
% own.

predicate fzn_set_of_int_global_cardinality_low_up(
  list of var set of int: xs,
  list of int: cover,
  list of int: lower_bound,
  list of int: upper_bound,
) =
  forall (i in index_set(cover)) (
    let { var int: sd = count (v in xs) (cover[i] in v) } in lower_bound[i] <= sd /\ sd <= upper_bound[i]
  );
