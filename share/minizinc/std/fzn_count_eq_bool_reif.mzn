predicate fzn_count_eq_bool_reif(
  array [int] of var bool: x,
  var bool: y,
  var int: c,
  var bool: b,
) = b <-> c = sum (i in index_set(x)) (x[i] == y);

%-----------------------------------------------------------------------------%
