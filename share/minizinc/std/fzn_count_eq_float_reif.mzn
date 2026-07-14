predicate fzn_count_eq_float_reif(
  array [int] of var float: xs,
  var float: value,
  var int: n,
  var bool: b,
) = b <-> n = sum (x_i in xs) (x_i == value);

%-----------------------------------------------------------------------------%
