% The constraint `set_of_int_partition`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_partition` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "all_disjoint.mzn";

predicate fzn_set_of_int_partition(list of var set of int: xs, set of int: universe) =
  all_disjoint(xs) /\ universe == array_union (i in index_set(xs)) (xs[i]);
