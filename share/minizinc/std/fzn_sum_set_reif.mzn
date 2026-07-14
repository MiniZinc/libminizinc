predicate fzn_sum_set_reif(
  array [int] of int: values,
  array [int] of int: weights,
  var set of int: selected,
  var int: total,
  var bool: b,
) = b <-> total == sum (j in index_set(values)) ((j in selected) * weights[j]);
