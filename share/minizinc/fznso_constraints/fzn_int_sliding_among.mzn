% The constraint `int_sliding_among`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_sliding_among` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "sliding_sum.mzn";

predicate fzn_int_sliding_among(
  list of var int: xs,
  set of int: values,
  int: window,
  int: lower_bound,
  int: upper_bound,
) =
  sliding_sum(lower_bound,upper_bound,window,[xi in values | xi in xs]);
