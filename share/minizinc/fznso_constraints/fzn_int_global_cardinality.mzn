% The constraint `int_global_cardinality`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_global_cardinality` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

include "count.mzn";

predicate fzn_int_global_cardinality(
  list of var int: xs,
  list of int: cover,
  list of var int: count,
) =
  forall (i in index_set(cover)) (count(xs, cover[i], count[i])) /\
    % Implied constraint
    length(xs) >= sum(count);
