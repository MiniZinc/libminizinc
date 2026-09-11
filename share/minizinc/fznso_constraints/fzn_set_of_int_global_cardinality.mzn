% The constraint `set_of_int_global_cardinality`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `set_of_int_global_cardinality` and the body below is dropped in favour of the
% propagator, or ship a file of this name with a decomposition of your own.

predicate fzn_set_of_int_global_cardinality(
  list of var set of int: xs,
  list of int: cover,
  list of var int: count,
) =
  forall (i in index_set(cover)) (count[i] = count (v in xs) (cover[i] in v));
