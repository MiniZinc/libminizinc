predicate fzn_knapsack(
  array [int] of int: weight,
  array [int] of int: profit,
  array [int] of var int: xs,
  var int: total_weight,
  var int: total_profit,
) =
  forall (i in index_set(xs)) (xs[i] >= 0) /\
    total_weight >= 0 /\
    total_profit >= 0 /\
    total_profit = sum (i in index_set(profit)) (xs[i] * profit[i]) /\
    total_weight = sum (i in index_set(weight)) (xs[i] * weight[i]);
