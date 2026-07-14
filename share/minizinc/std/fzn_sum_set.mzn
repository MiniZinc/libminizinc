predicate fzn_sum_set(
  array [int] of int: values,
  array [int] of int: weights,
  var set of int: selected,
  var int: total,
) = total == sum (j in index_set(values)) ((values[j] in selected) * weights[j]);
