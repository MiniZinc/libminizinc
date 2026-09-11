% The constraint `int_distribute`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_distribute` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

include "count.mzn";

predicate fzn_int_distribute(
  list of var int: xs,
  list of var int: cover,
  list of var int: count,
) =
  forall (i in index_set(count)) (count(xs, cover[i], count[i]));
