predicate fzn_knapsack(
  array [int] of int: weights,
  array [int] of int: profits,
  array [int] of var int: xs,
  var int: total_weight,
  var int: total_profit,
) =
  forall (i in index_set(xs)) (xs[i] >= 0) /\
    total_weight >= 0 /\
    total_profit >= 0 /\
    total_profit = sum (i in index_set(profits)) (xs[i] * profits[i]) /\
    total_weight = sum (i in index_set(weights)) (xs[i] * weights[i]);
