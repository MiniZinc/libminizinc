% The constraint `int_global_cardinality_closed`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `int_global_cardinality_closed` and the body below is dropped in favour of the
% propagator, or ship a file of this name with a decomposition of your own.

include "global_cardinality.mzn";

predicate fzn_int_global_cardinality_closed(
  list of var int: xs,
  list of int: cover,
  list of var int: count,
) =
  forall (i in index_set(xs)) (xs[i] in {d | d in cover}) /\ global_cardinality(xs, cover, count);
