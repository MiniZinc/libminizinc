predicate fzn_sum_pred_reif(
  var int: idx,
  array [int] of set of int: sets,
  array [int] of int: coefficients,
  var int: total,
  var bool: b,
) =
  let {
    array [index_set(sets)] of int: sums = [sum (k in sets[j]) (coefficients[k]) | j in index_set(sets)];
  } in b <-> sums[idx] = total;
