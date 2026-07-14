predicate fzn_among_reif(var int: n, array [int] of var int: xs, set of int: values, var bool: b) =
  b <-> (n == sum (i in index_set(xs)) (xs[i] in values));
