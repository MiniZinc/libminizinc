predicate fzn_count_eq_float_reif(
  array [int] of var float: x,
  var float: y,
  var int: c,
  var bool: b,
) = b <-> c = sum (x_i in x) (x_i == y);

%-----------------------------------------------------------------------------%
