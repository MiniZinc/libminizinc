predicate fzn_sum_pred(
  var int: idx,
  array [int] of set of int: sets,
  array [int] of int: coefficients,
  var int: total,
) =
  let {
    array [index_set(sets)] of int: sums = [sum (k in sets[j]) (coefficients[k]) | j in index_set(sets)];
  } in sums[idx] = total;
