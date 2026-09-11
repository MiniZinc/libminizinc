% The constraint `int_knapsack`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_knapsack` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_knapsack(
  list of int: weight,
  list of int: profit,
  list of var int: xs,
  var int: total_weight,
  var int: total_profit,
) =
  forall (i in index_set(xs)) (xs[i] >= 0) /\
    total_weight >= 0 /\
    total_profit >= 0 /\
    total_profit = sum (i in index_set(profit)) (xs[i] * profit[i]) /\
    total_weight = sum (i in index_set(weight)) (xs[i] * weight[i]);
