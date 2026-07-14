predicate fzn_count_eq(array [int] of var int: xs, var int: value, var int: n) =
  n = sum (i in index_set(xs)) (xs[i] == value);

%-----------------------------------------------------------------------------%
