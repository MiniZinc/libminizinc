% The constraint `int_global_cardinality_low_up`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare
% `int_global_cardinality_low_up` and the body below is dropped in favour of the
% propagator, or ship a file of this name with a decomposition of your own.

include "count_fn.mzn";

predicate fzn_int_global_cardinality_low_up(
  list of var int: xs,
  list of int: cover,
  list of int: lower_bound,
  list of int: upper_bound,
) =
  forall (i in index_set(cover)) (
    if upper_bound[i] >= length(xs) then
      count (xi in xs) (xi = cover[i]) >= lower_bound[i]
    elseif lower_bound[i] <= 0 then
      count (xi in xs) (xi = cover[i]) <= upper_bound[i]
    else
      count (xi in xs) (xi = cover[i]) in lower_bound[i]..upper_bound[i]
    endif
  );
