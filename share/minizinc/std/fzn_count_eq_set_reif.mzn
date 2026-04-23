predicate fzn_count_eq_set_reif(
  array [int] of var set of int: x,
  var set of int: y,
  var int: c,
  var bool: b,
) = b <-> c = sum (x_i in x) (x_i == y);

%-----------------------------------------------------------------------------%
