% The constraint `bool_sliding_among`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_sliding_among` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "sliding_sum.mzn";

predicate fzn_bool_sliding_among(
  list of var bool: xs,
  var bool: value,
  int: window,
  int: lower_bound,
  int: upper_bound,
) =
  sliding_sum(lower_bound,upper_bound,window,[xi=value | xi in xs]);
