% The constraint `int_disjunctive`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_disjunctive` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_disjunctive(list of var int: start, list of var int: duration) =
  forall (i in index_set(duration)) (duration[i] >= 0) /\
    forall (i, j in index_set(duration) where i < j) (
      duration[i] = 0 \/ duration[j] = 0 \/ start[i] + duration[i] <= start[j] \/ start[j] + duration[j] <= start[i]
    );
