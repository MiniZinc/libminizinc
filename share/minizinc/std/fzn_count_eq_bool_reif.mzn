predicate fzn_count_eq_bool_reif(
  array [int] of var bool: xs,
  var bool: value,
  var int: n,
  var bool: b,
) = b <-> n = sum (i in index_set(xs)) (xs[i] == value);

%-----------------------------------------------------------------------------%
