predicate fzn_among(var int: n, array [int] of var int: xs, set of int: values) =
  n == sum (i in index_set(xs)) (xs[i] in values);
