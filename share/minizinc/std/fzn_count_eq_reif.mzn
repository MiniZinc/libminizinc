predicate fzn_count_eq_reif(array [int] of var int: xs, var int: value, var int: n, var bool: b) =
  b <-> count_eq(xs, value) = n;

%-----------------------------------------------------------------------------%
